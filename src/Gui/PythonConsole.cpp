/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#include "PythonConsole.h"
#include "PythonConsolePy.h"
#include "Application.h"
#include "Action.h"
#include "Command.h"
#include "DlgEditorImp.h"
#include "FileDialog.h"
#include "MainWindow.h"


#include <Base/Interpreter.h>
#include <Base/Exception.h>

using namespace Gui;

namespace Gui {
struct PythonConsoleP
{
  QMap<QString, QColor> colormap; // Color map
  PythonConsoleP()
  {
    colormap["Text"] = Qt::black;
    colormap["Bookmark"] = Qt::cyan;
    colormap["Breakpoint"] = Qt::red;
    colormap["Keyword"] = Qt::blue;
    colormap["Comment"] = QColor(0, 170, 0);
    colormap["Block comment"] = QColor(160, 160, 164);
    colormap["Number"] = Qt::blue;
    colormap["String"] = Qt::red;
    colormap["Character"] = Qt::red;
    colormap["Class name"] = QColor(255, 170, 0);
    colormap["Define name"] = QColor(255, 170, 0);
    colormap["Operator"] = QColor(160, 160, 164);
    colormap["Python output"] = QColor(170, 170, 127);
    colormap["Python error"] = Qt::red;
  }
};
} // namespace Gui

PythonConsole * PythonConsole::_instance = 0;
PyObject      * PythonConsole::_stdoutPy = 0;
PyObject      * PythonConsole::_stderrPy = 0;
PyObject      * PythonConsole::_stdinPy  = 0;
PyObject      * PythonConsole::_stdout   = 0;
PyObject      * PythonConsole::_stderr   = 0;
PyObject      * PythonConsole::_stdin    = 0;

/* TRANSLATOR Gui::PythonConsole */

/**
 *  Constructs a PythonConsole which is a child of 'parent', with the
 *  name 'name'. 
 */
PythonConsole::PythonConsole(QWidget *parent)
  : TextEdit(parent), WindowParameter( "Editor" ), _startPara(0), _indent(false), _autoTabs(true), _blockComment(false)
{
  d = new PythonConsoleP();
  _instance = this;
  _stdoutPy = new PythonStdoutPy( _instance );
  _stderrPy = new PythonStderrPy( _instance );
  _stdinPy  = new PythonStdinPy ( _instance );

  // use the console highlighter
  pythonSyntax = new PythonConsoleHighlighter(this);
  ParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Attach( this );

  zoomIn(2);
#ifdef FC_OS_LINUX
  QFont serifFont( "Courier", 15, QFont::Normal );
#else
  QFont serifFont( "Courier", 10, QFont::Normal );
#endif
  setFont(serifFont);

  // set colors and font
  hPrefGrp->NotifyAll();

  setTabStopWidth(32);
  // disable undo/redo stuff
  setUndoRedoEnabled( false );
  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops( TRUE );

  // try to override Python's stdout/err
  try
  {
    _stdout = PySys_GetObject("stdout");
    _stderr = PySys_GetObject("stderr");
    _stdin  = PySys_GetObject("stdin");
	  (void) Py_InitModule("PyConsole", PythonConsole::Methods);
    Base::Interpreter().runString("import sys, PyConsole");
    Base::Interpreter().runString("PyConsole.redirectStdout()");
    Base::Interpreter().runString("PyConsole.redirectStderr()");
    Base::Interpreter().runString("PyConsole.redirectStdin()");
    Base::Interpreter().runString("print 'Python' + sys.version + ' on ' + sys.platform");
    Base::Interpreter().runString("print \"Type 'help', 'copyright', 'credits' or 'license' for more information.\"");
    Base::Interpreter().runString("PyConsole.restoreStdout()");
    Base::Interpreter().runString("PyConsole.restoreStderr()");
  }
  catch (...)
  {
    setText("Initialization of Python failed.\n");
  }

  printPrompt();
  moveCursor( MoveLineEnd, false );
}

/** Destroys the object and frees any allocated resources */
PythonConsole::~PythonConsole()
{
  getWindowParameter()->Detach( this );
  delete pythonSyntax;
  Py_XDECREF(_stdoutPy);
  Py_XDECREF(_stderrPy);
  Py_XDECREF(_stdinPy);
  delete d;
}

// PythonConsole Methods						// Methods structure
PyMethodDef PythonConsole::Methods[] = {
	{"redirectStdout", (PyCFunction) PythonConsole::sStdoutPy,  1},
	{"redirectStderr", (PyCFunction) PythonConsole::sStderrPy,  1},
	{"redirectStdin",  (PyCFunction) PythonConsole::sStdinPy,   1},
	{"restoreStdout",  (PyCFunction) PythonConsole::sStdout,    1},
	{"restoreStderr",  (PyCFunction) PythonConsole::sStderr,    1},
	{"restoreStdin",   (PyCFunction) PythonConsole::sStdin,     1},
  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(PythonConsole,sStdoutPy)
{
  PySys_SetObject("stdout", _stdoutPy);
  return Py_None; 
}

PYFUNCIMP_S(PythonConsole,sStderrPy)
{
  PySys_SetObject("stderr", _stderrPy);
  return Py_None; 
}

PYFUNCIMP_S(PythonConsole,sStdinPy)
{
  PySys_SetObject("stdin", _stdinPy);
  return Py_None; 
}

PYFUNCIMP_S(PythonConsole,sStdout)
{
  PySys_SetObject("stdout", _stdout);
  return Py_None; 
}

PYFUNCIMP_S(PythonConsole,sStderr)
{
  PySys_SetObject("stderr", _stderr);
  return Py_None; 
}

PYFUNCIMP_S(PythonConsole,sStdin)
{
  PySys_SetObject("stdin", _stdin);
  return Py_None; 
}

/** Sets the new color for \a rcColor. */  
void PythonConsole::OnChange( Base::Subject<const char*> &rCaller,const char* sReason )
{
  ParameterGrp::handle hPrefGrp = getWindowParameter();

  QFont font = currentFont();
  font.setBold( false ); // if current font is bold we must reset it first
  if (strcmp(sReason, "FontSize") == 0)
  {
#ifdef FC_OS_LINUX
    QString txt = hPrefGrp->GetASCII( "FontSize", "15" ).c_str();
#else
    QString txt = hPrefGrp->GetASCII( "FontSize", "10" ).c_str();
#endif

    bool ok;
    int size = txt.toInt(&ok);
#ifdef FC_OS_LINUX
    if ( !ok ) size = 15; 
#else
    if ( !ok ) size = 10; 
#endif

    font.setPointSize( size );
    setFont( font );
  }
  else if (strcmp(sReason, "Font") == 0)
  {
    QString family = hPrefGrp->GetASCII( "Font", "Courier" ).c_str();
    font.setFamily( family );
    setFont( font );
  }
  else
  {
    QMap<QString, QColor>::ConstIterator it = d->colormap.find(sReason);
    if (it != d->colormap.end()) {
      QColor color = it.data();
      unsigned long col = (color.red() << 24) | (color.green() << 16) | (color.blue() << 8);
      col = hPrefGrp->GetUnsigned( sReason, col);
      color.setRgb((col>>24)&0xff, (col>>16)&0xff, (col>>8)&0xff);
      pythonSyntax->setColor( sReason, color );
    }
  }
}

/** 
 * Move the cursor to the right position before the key 
 * event is performed. 
 */
void PythonConsole::doKeyboardAction ( KeyboardAction action )
{
  int para, index;
  getCursorPosition ( &para, &index );

  if ( action == ActionReturn )
  {
    // move to the end to avoid splitting the paragraph 
    // (if the last paragraph is wrapped make sure to set the cursor to the index after the last character)
    QString line = text( paragraphs()-1 );
    setCursorPosition( paragraphs()-1, line.length() );
    moveCursor( MoveLineEnd, false );
  }
  // pass to base class or "eat" the action
  else if ( action == ActionBackspace )
  {
    // must not modify ">>> " or "... "
    if ( index < 5 || para < _startPara )
    {
      QApplication::beep();
      return;
    }
  }
  // pass to base class or "eat" the action
  else if ( action == ActionDelete )
  {
    // must not modify ">>> " or "... "
    if ( index < 4 || para < _startPara )
    {
      QApplication::beep();
      return;
    }
  }

  TextEdit::doKeyboardAction( action );
}

/** Clears the content and writes the propmpt. */
void PythonConsole::clear ()
{
  TextEdit::clear();
  setText(">>> ");
  moveCursor( MoveLineEnd, false );
  _startPara = 0;
  _indent = false;
}

void PythonConsole::removeSelectedText ( int selNum )
{
  int para, index;
  getCursorPosition ( &para, &index );

  // cannot remove ">>> " or "... "
  if ( (_startPara <= para) && index > 3 )
    TextEdit::removeSelectedText( selNum );
  else
    QApplication::beep();
}

/** Allows pasting in the active line only. */
void PythonConsole::paste()
{
  int para, index, indexFrom, paraTo, indexTo;
  int paraFrom = _startPara;
  getCursorPosition ( &para, &index );
  if ( hasSelectedText() )
    getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

  // cannot remove ">>> " or "... "
  if ( (_startPara <= para) && index > 3 && _startPara <= paraFrom )
    TextEdit::paste();
  else
    QApplication::beep();
}

/**
 * Decodes the clipboard plain text and insert line by line to this text edit and
 * simulate a "return" event to let decide keyPressEvent() how to continue. This
 * is to run the Python interpreter if needed.
 */
void PythonConsole::pasteSubType( const QByteArray &subtype )
{
#if 0 //TODO Reimplement for Qt4
  QMimeSource* mime = QApplication::clipboard()->data( QClipboard::Clipboard );
  if ( !mime )
    return; // no valid data
  Q3CString st = subtype;
  st.prepend( "text/" );

  if ( document()->hasSelection( QTextDocument::Standard ) )
    removeSelectedText();
  if ( !mime->provides( st.data() ) )
    return;

  QString t;
  if ( !Q3TextDrag::canDecode( mime) )
    return; // cannot decode
  if ( !Q3TextDrag::decode( mime, t ) )
    return;
#if defined(Q_OS_WIN32)
  // Need to convert CRLF to LF
  t.replace( "\r\n", "\n" );
#elif defined(Q_OS_MAC)
  //need to convert CR to LF
  t.replace( '\r', '\n' );
#endif
  QChar *uc = (QChar *)t.unicode();
  for ( int i=0; (uint) i<t.length(); i++ ) {
      if ( uc[ i ] < ' ' && uc[ i ] != '\n' && uc[ i ] != '\t' )
    uc[ i ] = ' ';
  }

  if ( !t.isEmpty() )
  {
    int ct = t.contains('\n');
    int i=0;
    QStringList lst = QStringList::split('\n', t, true);
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); it++, i++ )
    {
      insert( *it );

      if ( i < ct )
      {
        // emulate an key return event to let decide keyPressEvent() how to continue
        QKeyEvent ke( QEvent::KeyPress, Qt::Key_Return, '\n', Qt::NoButton );
        _autoTabs = false; // do insert tabs automatically
        QApplication::sendEvent( this, &ke );
        _autoTabs = true;
      }
    }
  }
#endif
}

/** Allows cutting in the active line only. */
void PythonConsole::cut()
{
  int para, index, paraFrom, indexFrom, paraTo, indexTo;
  getCursorPosition ( &para, &index );
  getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

  // cannot remove ">>> " or "... "
  if ( (_startPara <= para) && index > 3 && _startPara <= paraFrom )
    TextEdit::cut();
  else
    QApplication::beep();
}

/**
 * Checks the input of the console to make the correct indentations.
 * After a command is prompted completely the Python interpreter is started.
 */
void PythonConsole::keyPressEvent(QKeyEvent * e)
{
  // find current cursor position
  int para, index;
  getCursorPosition ( &para, &index );

  // avoid to modify the first four characters in a line
  if ( (index < 4 || para < _startPara ) && (e->state() == Qt::NoButton) )
  {
    // ignore all modifying events
    QString key = e->text();
    if ( !key.isNull() )
    {
      setCursorPosition( paragraphs()-1, 0 );
      moveCursor( MoveLineEnd, false );
    }
  }

  TextEdit::keyPressEvent(e);

  switch (e->key())
  {
  // running Python interpreter?
  case Qt::Key_Return:
  case Qt::Key_Enter:
    {
      // get the last paragraph text
      int para; int idx;
      getCursorPosition(&para, &idx);
      QString txt = text(para-1);

      // and skip the first 4 characters consisting of either ">>> " or "... "
      txt = txt.mid(4);

      // put statement to the history
      if ( txt.length() > 1 )
      {
        QString cmd = txt;
        // truncate the last whitespace that is appended from QTextEdit
        cmd.truncate( cmd.length()-1 );
        _history.append( cmd );
      }

      // insert tabs if needed
      int tabs = tabsIndent( txt );
      if ( isBlockComment( txt ) && tabs == 0 )
      {
        insertAt("... ", para, 0 );
        moveCursor( MoveLineEnd, false );

        // store paragraph where Python command starts
        if ( _indent == false )
          _startPara = para - 1;
        _indent = true;
        _blockComment = true;
      }
      else if ( _blockComment ) // just closed
      {
        _blockComment = false;
        // recursive call to invoke performPythonCommand()
        QKeyEvent ke( QEvent::KeyPress, Qt::Key_Return, '\n', Qt::NoButton );
        QApplication::sendEvent( this, &ke );
      }
      else if ( tabs > 0 )
      {
        insertAt("... ", para, 0 );
        if ( _autoTabs )
        {
          for ( int i=0; i<tabs; i++ )
            insertAt("\t", para, i+4);
        }
        moveCursor( MoveLineEnd, false );

        // store paragraph where Python command starts
        if ( _indent == false )
          _startPara = para - 1;
        _indent = true;
      }
      else
      {
        // start Python interpreter here
        performPythonCommand();
      }
    } break;
  case Qt::Key_Up:
    if ( e->state() & Qt::ControlModifier )
    {
      if ( !_history.isEmpty() )
      {
        if ( _history.prev() )
        {
          QString cmd = _history.value();
          overwriteParagraph( paragraphs()-1, cmd );
        }
      }
    }
    break;
  case Qt::Key_Down:
    if ( e->state() & Qt::ControlModifier )
    {
      if ( !_history.isEmpty() )
      {
        if ( _history.next() )
        {
          QString cmd = _history.value();
          overwriteParagraph( paragraphs()-1, cmd );
        }
      }
    }
    break;
  default: break;
  }
}

/**
 * Insert an output message to the console. This message comes from
 * the Python interpreter and is redirected from sys.stdout.
 */
void PythonConsole::insertPythonOutput( const QString& msg )
{
  _output += msg;
}

/**
 * Insert an error message to the console. This message comes from
 * the Python interpreter and is redirected from sys.stdout.
 */
void PythonConsole::insertPythonError ( const QString& err )
{
  _error += err;
}

/** Prints the ps1 prompt (>>> ) to the console window. */ 
void PythonConsole::printPrompt()
{
  // write normal messages
  if (!_output.isEmpty())
  {
    pythonSyntax->highlightOutput(true);
    append( _output);
    _output=QString::null;
    pythonSyntax->highlightOutput(false);
  }

  // write error messages
  if (!_error.isEmpty())
  {
    pythonSyntax->highlightError(true);
    append( _error);
    _error = QString::null;
    pythonSyntax->highlightError(false);
  }
  // prompt
  append(">>> ");

  // go to last the paragraph as we don't know sure whether Python 
  // has written something to the console
  _startPara = paragraphs() - 1;
  setCursorPosition(_startPara, 0);
  moveCursor( MoveLineEnd, false );
  _indent = false;
}

/**
 * Prints a Python command to the console.
 */
bool PythonConsole::printCommand( const QString& cmd )
{
  bool ok = true;
  try
  {
    Base::Interpreter().runString("PyConsole.redirectStdout()");
    Base::Interpreter().runString("PyConsole.redirectStderr()");
    Base::Interpreter().runStringArg("print '%s'", cmd.latin1() );
    Base::Interpreter().runString("PyConsole.restoreStdout()");
    Base::Interpreter().runString("PyConsole.restoreStderr()");
    _history.append( cmd );
  }
  catch( ... )
  {
    Base::Interpreter().runString("PyConsole.restoreStdout()");
    Base::Interpreter().runString("PyConsole.restoreStderr()");
    ok = false; // an error occurred
  }

  printPrompt();

  return ok;
}

/**
 * Prints the Python statement cmd to the console.
 * @note The statement gets only printed but not invoked.
 */
bool PythonConsole::printStatement( const char* cmd )
{
  // go to the last paragraph before inserting new text 
  int last = paragraphs() - 1;
  setCursorPosition(last, 0);
  moveCursor( MoveLineEnd, false );

  QString line=cmd;
  QStringList statements = QStringList::split( "\n", line );
  for ( QStringList::Iterator it = statements.begin(); it != statements.end(); ++it )
  {
    // printed command
    insert( *it );
    _history.append( *it );
    // prompt
    printPrompt();
  }

  return true;
}

/**
 * Shows the Python window and sets the focus to set text cursor.
 */
void PythonConsole::showEvent ( QShowEvent * e )
{
  TextEdit::showEvent( e );
  // set also the text cursor to the edit field
  setFocus();
}

/**
 * Drops the event \a e and writes the right Python command.
 */
void PythonConsole::contentsDropEvent ( QDropEvent * e )
{
  const QMimeData* mimeData = e->mimeData();
  if ( mimeData->hasFormat("text/x-action-items") ) {
    QByteArray itemData = mimeData->data("text/x-action-items");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    int ctActions; dataStream >> ctActions;
    for (int i=0; i<ctActions; i++) {
      QString action;
      dataStream >> action;
      printCommand( QString("Gui.RunCommand(\"%1\")").arg(action));
    }

    e->setDropAction(Qt::CopyAction);
    e->accept();
  } else {
    e->ignore();
  }
}

/** Dragging of action objects is allowed. */ 
void PythonConsole::contentsDragMoveEvent( QDragMoveEvent *e )
{
  const QMimeData* mimeData = e->mimeData();
  if ( mimeData->hasFormat("text/x-action-items") )
    e->accept();
  else
    e->ignore();
}

/** Dragging of action objects is allowed. */ 
void PythonConsole::contentsDragEnterEvent ( QDragEnterEvent * e )
{
  const QMimeData* mimeData = e->mimeData();
  if ( mimeData->hasFormat("text/x-action-items") )
    e->accept();
  else
    e->ignore();
}

/** Figures out how many tabs must be set next paragraph. 0 indicates that
 * the command must be build (maybe including previous paragraphs) and 
 * given to the Python interpreter.
 */
int PythonConsole::tabsIndent( const QString& txt ) const
{
  int tabs=0;

  // Check if this paragraph is empty (either really empty or consists of whitespace only)
  QRegExp rx("^\\s+$");
  if ( txt.find( rx ) == 0 )
    return 0; // perform command
  
  // search for tabs
  rx.setPattern("^\\s+");
  if ( txt.find( rx ) == 0 )
    tabs = rx.matchedLength(); // tabs found at the beginning
  
  // Check if Python keyword followed by any characters followed by a colon matches
  rx.setPattern("\\b(?:class|def|eif|else|except|for|if|try|while)\\b.*:");
  rx.setMinimal( true ); // not in greedy mode
  int pos;
  if ( (pos=txt.find( rx )) > -1 )
  {
    // furthermore check if the actual command follows the colon
    QString cmd = txt.mid( pos + rx.matchedLength() );
    QRegExp re("^\\s+$");
    // whitespaces, thus we assume a new block begins
    if ( cmd.find( re ) > -1) 
      tabs++; // open new block
  }

  return tabs;
}

/**
 * Builds up the Python command and pass it to the interpreter.
 */
bool PythonConsole::performPythonCommand()
{
  bool ok = true;
  int para; int idx;
  getCursorPosition(&para, &idx);

  // Build up Python command from startPara to para
  QString pyCmd;
  if ( _startPara < para-1 )
  {
    for ( int i=_startPara; i<para-1; i++ )
    {
      // skip ">>> " or "... "
      QString line = text( i ).mid( 4 );
      if ( line.length() > 1 && !isComment(line) )
      {
        pyCmd += line;
        pyCmd += "\n";
      }
    }
  }
  else
  {
    // skip ">>> " or "... "
    QString line = text( para-1 ).mid( 4 );
    if ( line.length() > 1 && !isComment(line) )
    {
      pyCmd += line;
      pyCmd += "\n";
    }
  }

  if ( !pyCmd.isEmpty() )
  {
    // switch off warnings and errors to avoid pop-up dialogs
    ConsoleMsgFlags ret = Base::Console().SetEnabledMsgType("MessageBox",ConsoleMsgType::MsgType_Wrn|
                                                                         ConsoleMsgType::MsgType_Err, false);
    try
    {
      // launch the command now
      Base::Interpreter().runString("PyConsole.redirectStdout()");
      Base::Interpreter().runString("PyConsole.redirectStderr()");
      Base::Interpreter().runInteractiveString( pyCmd.latin1() );
      Base::Interpreter().runString("PyConsole.restoreStdout()");
      Base::Interpreter().runString("PyConsole.restoreStderr()");
      setFocus(); // if focus was lost
    }
    catch ( const Base::Exception& )
    {
      // Write Python's error output instead, if there is!
      Base::Interpreter().runString("PyConsole.restoreStdout()");
      Base::Interpreter().runString("PyConsole.restoreStderr()");
      ok = false;
    }

    // restore observer settings
    Base::Console().SetEnabledMsgType("MessageBox", ret, true);
  }

  printPrompt();

  return ok;
}

/**
 * Overwrites the text in paragraph \a para with \a txt.
 */
void PythonConsole::overwriteParagraph( int para, const QString& txt )
{
  QString oldText = text( para );
  setSelection( para, 4, para, oldText.length() );
  del();
  setCursorPosition( para, 4 );
  insert( txt );
}

/**
 * Checks if the string \a text is a comment beginning with '#'. If so, then the line
 * is ignored and not given to the Python interpreter.
 */
bool PythonConsole::isComment( const QString& text ) const
{
  int i = 0;
  QChar ch;
  bool ok=false;
  while ( i < text.length() )
  {
    ch = text.at( i ).latin1();
    if ( ch == '#' )
    {
      ok = true; // line comment
      break;
    }
    else if ( ch == ' ' || ch == '\t' ) 
    {
      // white space -> continue
    }
    // first valid character is part of a command
    else
    {
      ok = false;
      break;
    }

    i++;
  }

  return ok;
}

bool PythonConsole::isBlockComment( const QString& ) const
{
  int mode = pythonSyntax->endStateOfLastParagraph();
  // for the values 5 and 6 see PythonSyntaxHighlighter::highlightParagraph()
  if ( mode == 5 || mode == 6 )
    return true;
  else
    return false;
}

Q3PopupMenu * PythonConsole::createPopupMenu ( const QPoint & pos )
{
  Q3PopupMenu* menu = Q3TextEdit::createPopupMenu(pos);
  menu->insertItem( tr("Save history as..."), this, SLOT(onSaveHistoryAs()));
  menu->insertItem( tr("Insert file name..."), this, SLOT(onInsertFileName()));
  return menu;
}

void PythonConsole::onSaveHistoryAs()
{
  QString cMacroPath = getDefaultParameter()->GetGroup( "Macro" )->GetASCII("MacroPath",App::GetApplication().GetHomePath()).c_str();
  QString fn = FileDialog::getSaveFileName(cMacroPath,"Macro Files (*.FCMacro *.py)", this, tr("Save History"));
  if (!fn.isEmpty())
  {
    int dot = fn.find('.');
    if (dot != -1)
    {
      QFile f(fn);
      if (f.open(QIODevice::WriteOnly))
      {
        QTextStream t (&f);
        for ( _history.first(); _history.more(); _history.next() )
          t << _history.value() << "\n";
        f.close();
      }
    }
  }
}

void PythonConsole::onInsertFileName()
{
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "All Files (*.*)", Gui::getMainWindow() );
	if ( fn.isEmpty() )
		return;

  //fn.replace('\\',"/");
  insert( fn );

}

// ---------------------------------------------------------------------

PythonConsoleHighlighter::PythonConsoleHighlighter(Q3TextEdit* edit)
  : PythonSyntaxHighlighter(edit),_output(false), _error(false), _endState(-2)
{
}

PythonConsoleHighlighter::~PythonConsoleHighlighter()
{
}

int PythonConsoleHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
  const int ErrorOutput   = 20;
  const int MessageOutput = 21;

  if ( _output )
    endStateOfLastPara=MessageOutput;
  else if ( _error )
    endStateOfLastPara=ErrorOutput;
  else if ( endStateOfLastPara==MessageOutput || endStateOfLastPara==ErrorOutput)
    endStateOfLastPara=0;

  switch ( endStateOfLastPara )
  {
  case ErrorOutput:
    {
      // begin a comment
      QFont font = textEdit()->currentFont();
      font.setBold( false );
      font.setItalic( true );
      setFormat( 0, text.length(), font, color("Python error"));
      endStateOfLastPara=ErrorOutput;
    } break;
  case MessageOutput:
    {
      // begin a comment
      setFormat( 0, text.length(), color("Python output"));
      endStateOfLastPara=MessageOutput;
    } break;
  default:
    {
      endStateOfLastPara=PythonSyntaxHighlighter::highlightParagraph( text, endStateOfLastPara );
    } break;
  }

  _endState = endStateOfLastPara;

  return endStateOfLastPara;
}

int PythonConsoleHighlighter::endStateOfLastParagraph() const
{
  return _endState;
}

void PythonConsoleHighlighter::colorChanged( const QString& type, const QColor& col )
{
}

/**
 * If \a b is set to true the following input to the editor is highlighted as normal output.
 */
void PythonConsoleHighlighter::highlightOutput (bool b)
{
  _output = b;
}

/**
 * If \a b is set to true the following input to the editor is highlighted as error.
 */
void PythonConsoleHighlighter::highlightError (bool b)
{
  _error = b;
}

// ---------------------------------------------------------------------

ConsoleHistory::ConsoleHistory()
{
  it = _history.end();
}

ConsoleHistory::~ConsoleHistory()
{
}

void ConsoleHistory::first()
{
  it = _history.begin();
}

bool ConsoleHistory::more()
{
  return (it != _history.end());
}

bool ConsoleHistory::next() 
{
  if ( it != _history.end() )
  {
    it++;
    return true;
  }

  return false;
}

bool ConsoleHistory::prev() 
{
  if ( it != _history.begin() )
  {
    it--;
    return true;
  }

  return false;
}

bool ConsoleHistory::isEmpty() const
{
  return _history.isEmpty();
}

QString ConsoleHistory::value() const
{
  if ( it != _history.end() )
    return *it;
  else
    return QString::null;
}

void ConsoleHistory::append( const QString& item )
{
  _history.append( item );
  it = _history.end();
}

#include "moc_PythonConsole.cpp"
