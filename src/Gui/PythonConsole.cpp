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

#ifndef _PreComp_
# include <qapplication.h>
# include <qregexp.h>
#endif

#include "PythonConsole.h"
#include "PythonConsolePy.h"
#include "Application.h"
#include "Action.h"
#include "Command.h"
#include "DlgEditorImp.h"

#include <Base/Interpreter.h>
#include <Base/Exception.h>

using namespace Gui;
using Gui::Dialog::GetDefCol;

PythonConsole * PythonConsole::_instance = 0;
PyObject      * PythonConsole::_stdoutPy = 0;
PyObject      * PythonConsole::_stderrPy = 0;
PyObject      * PythonConsole::_stdinPy  = 0;
PyObject      * PythonConsole::_stdout   = 0;
PyObject      * PythonConsole::_stderr   = 0;
PyObject      * PythonConsole::_stdin    = 0;

/**
 *  Constructs a PythonConsole which is a child of 'parent', with the
 *  name 'name'. 
 */
PythonConsole::PythonConsole(QWidget *parent,const char *name)
  : TextEdit(parent, name), WindowParameter( "Editor" ), _startPara(0), _indent(false)
{
  _instance = this;
  _stdoutPy = new PythonStdoutPy( _instance );
  _stderrPy = new PythonStderrPy( _instance );
  _stdinPy  = new PythonStdinPy ( _instance );

  // use the console highlighter
  pythonSyntax = new PythonConsoleHighlighter(this);
  FCParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Attach( this );
  // set colors and font
  hPrefGrp->NotifyAll();

  zoomIn(2);
#ifdef FC_OS_LINUX
  QFont serifFont( "Courier", 15, QFont::Normal );
#else
  QFont serifFont( "Courier", 10, QFont::Normal );
#endif
  setFont(serifFont);

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
  }
  catch (...)
  {
    setText("Initialization of Python failed.\n");
  }

  append(">>> ");
  moveCursor( MoveLineEnd, false );
}

/** Destroys the object and frees any allocated resources */
PythonConsole::~PythonConsole()
{
  getWindowParameter()->Detach( this );
  delete pythonSyntax;
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
void PythonConsole::OnChange( FCSubject<const char*> &rCaller,const char* sReason )
{
  FCParameterGrp::handle hPrefGrp = getWindowParameter();

  QFont font = currentFont();
  font.setBold( false ); // if current font is bold we must reset it first
  if (strcmp(sReason, "FontSize") == 0)
  {
    QString txt = hPrefGrp->GetASCII( "FontSize", "9" ).c_str();

    bool ok;
    int size = txt.toInt(&ok);
    if ( !ok ) size = 9; 

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
    long col = hPrefGrp->GetInt( sReason, GetDefCol().color( sReason ));
    QColor color;
    color.setRgb(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    pythonSyntax->setColor( sReason, color );
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
    setCursorPosition( paragraphs()-1, 0 );
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
void PythonConsole::pasteSubType( const QCString &subtype )
{
  QMimeSource* mime = QApplication::clipboard()->data( QClipboard::Clipboard );
  if ( !mime )
    return; // no valid data
  QCString st = subtype;
  st.prepend( "text/" );

  if ( document()->hasSelection( QTextDocument::Standard ) )
    removeSelectedText();
  if ( !mime->provides( st.data() ) )
    return;

  QString t;
  if ( !QTextDrag::canDecode( mime) )
    return; // cannot decode
  if ( !QTextDrag::decode( mime, t ) )
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
    QStringList lst = QStringList::split('\n', t, true);
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); it++ )
    {
      insert( *it );
      // emulate an key return event to let decide keyPressEvent() how to continue
      QKeyEvent ke( QEvent::KeyPress, Key_Return, '\n', Qt::NoButton );
      QApplication::sendEvent( this, &ke );
    }
  }
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
  case Key_Return:
  case Key_Enter:
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
      if ( tabs > 0 )
      {
        insertAt("... ", para, 0 );
        for ( int i=0; i<tabs; i++ )
          insertAt("\t", para, i+4);
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
  case Key_Up:
    if ( e->state() & ControlButton )
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
  case Key_Down:
    if ( e->state() & ControlButton )
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
  int para; int idx;
  getCursorPosition(&para, &idx);
  pythonSyntax->highlightOutput(true);
  insert( msg);
  pythonSyntax->highlightOutput(false);
  _startPara = paragraphs() - 1;
}

/**
 * Insert an error message to the console. This message comes from
 * the Python interpreter and is redirected from sys.stdout.
 */
void PythonConsole::insertPythonError ( const QString& err )
{
  int para; int idx;
  getCursorPosition(&para, &idx);
  pythonSyntax->highlightError(true);
  insert( err);
  pythonSyntax->highlightError(false);
  _startPara = paragraphs() - 1;
}

/** Prints the ps1 prompt (>>> ) to the console window. */ 
void PythonConsole::printPrompt()
{
  // go to last the paragraph as we don't know sure whether Python 
  // has written something to the console
  int para = paragraphs();
  _startPara = para - 1;
  setCursorPosition(para-1, 0);
  _indent = false;
  insertAt(">>> ", para-1, 0 );
  moveCursor( MoveLineEnd, false );
}

/**
 * Prints a Python command to the console.
 */
bool PythonConsole::printCommand( const QString& cmd )
{
  bool ok = true;
  try
  {
    Base::Interpreter().runStringArg("print '%s'", cmd.latin1() );
  }
  catch( ... )
  {
    ok = false; // an error occurred
  }

  printPrompt();

  return ok;
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
  QString action;
  if ( ActionDrag::decode(e, action) )
  {
    if (!action.isEmpty())
    {
      ActionDrag::actions.clear();
      CommandManager& rclMan = ApplicationWindow::Instance->commandManager();
      Command* pCmd = rclMan.getCommandByName(action.latin1());

      if (pCmd)
      {
        printCommand( QString("Gui.RunCommand(\"%1\")").arg(pCmd->getName() ) );
      }
    }
  }

  TextEdit::dropEvent(e);
}

/** Dragging of action objects is allowed. */ 
void PythonConsole::contentsDragMoveEvent( QDragMoveEvent *e )
{
  bool can = QTextDrag::canDecode(e) || ActionDrag::canDecode(e);
  if ( !can )
    e->ignore();
}

/** Dragging of action objects is allowed. */ 
void PythonConsole::contentsDragEnterEvent ( QDragEnterEvent * e )
{
  if ( ActionDrag::canDecode( e ) )
    e->accept(true);
  else
    TextEdit::dragEnterEvent( e );
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
      if ( line.length() > 1 )
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
    if ( line.length() > 1 )
    {
      pyCmd += line;
      pyCmd += "\n";
    }
  }

  try
  {
    // launch the command now
    bool bMute = GuiConsoleObserver::bMute;
    GuiConsoleObserver::bMute = true;
    if ( !pyCmd.isEmpty() )
    {
      Base::Interpreter().runString( pyCmd.latin1() );
    }
    GuiConsoleObserver::bMute = bMute;
    setFocus(); // if focus was lost
  }
  catch ( const Base::Exception& )
  {
    // Write Python's error output instead, if there is!
    ok = false;
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

// ---------------------------------------------------------------------

PythonConsoleHighlighter::PythonConsoleHighlighter(QTextEdit* edit)
  : PythonSyntaxHighlighter(edit),_output(false), _error(false)
{
}

PythonConsoleHighlighter::~PythonConsoleHighlighter()
{
}

int PythonConsoleHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
  const int ErrorOutput   = 9;
  const int MessageOutput = 10;

  if ( _output )
    endStateOfLastPara=MessageOutput;
  else if ( _error )
    endStateOfLastPara=ErrorOutput;
  else if ( endStateOfLastPara==MessageOutput || endStateOfLastPara==ErrorOutput)
    endStateOfLastPara=0;
  else if ( endStateOfLastPara==-2 )
    endStateOfLastPara=MessageOutput;

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

  return endStateOfLastPara;
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
