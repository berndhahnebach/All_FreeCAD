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
  PyObject *_stdoutPy, *_stderrPy, *_stdinPy;
  PyObject *_stdout, *_stderr, *_stdin;
  InteractiveInterpreter* interpreter;
  QMap<QString, QColor> colormap; // Color map
  PythonConsoleP()
  {
    interpreter = 0;
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
struct InteractiveInterpreterP
{
  PyObject* interpreter;
  PyObject* sysmodule;
  QStringList buffer;
};
} // namespace Gui

InteractiveInterpreter::InteractiveInterpreter()
{
  // import code.py and create an instance of InteractiveInterpreter
  PyObject* module = PyImport_ImportModule("code");
  if (!module)
    throw Base::PyException();
  PyObject* func = PyObject_GetAttrString(module, "InteractiveInterpreter");
  PyObject* args = Py_BuildValue("()");
  d = new InteractiveInterpreterP;
  d->interpreter = PyEval_CallObject(func,args);
  Py_DECREF(args);
  Py_DECREF(func);
  Py_DECREF(module);

  setPrompt();
}

InteractiveInterpreter::~InteractiveInterpreter()
{
  Py_XDECREF(d->interpreter);
  Py_XDECREF(d->sysmodule);
  delete d;
}

/**
 * Set the ps1 and ps2 members of the sys module if not yet defined.
 */
void InteractiveInterpreter::setPrompt()
{
  // import code.py and create an instance of InteractiveInterpreter
  d->sysmodule = PyImport_ImportModule("sys");
  if (!PyObject_HasAttrString(d->sysmodule, "ps1"))
    PyObject_SetAttrString(d->sysmodule, "ps1", PyString_FromString(">>> "));
  if (!PyObject_HasAttrString(d->sysmodule, "ps2"))
    PyObject_SetAttrString(d->sysmodule, "ps2", PyString_FromString("... "));
}

/**
 * Compile a command and determine whether it is incomplete.
 * 
 * The source string may contain \n characters.
 * Return value / exceptions raised:
 * \item Return a code object if the command is complete and valid
 * \item Return None if the command is incomplete
 * \item Raise SyntaxError, ValueError or OverflowError if the command is a
 * syntax error (OverflowError and ValueError can be produced by
 * malformed literals).
 */
PyObject* InteractiveInterpreter::compile(const char* source) const
{
  PyObject* func = PyObject_GetAttrString(d->interpreter, "compile");
  PyObject* args = Py_BuildValue("(s)", source);
  PyObject* eval = PyEval_CallObject(func,args);  // must decref later

  Py_DECREF(args);
  Py_DECREF(func);

  if (eval){
    return eval;
  } else {
    // do not throw Base::PyException as this clears the error indicator
    throw Base::Exception();
  }

  // can never happen
  return 0;
}

/**
 * Compile and run some source in the interpreter.
 *
 * One several things can happen:
 *
 * \item The input is incorrect; compile() raised an exception (SyntaxError or OverflowError).  
 * A syntax traceback will be printed by calling the showsyntaxerror() method.
 *
 * \item The input is incomplete, and more input is required; compile() returned False. 
 * Nothing happens.
 *
 * \item The input is complete; compile() returned true.  The code is executed by calling 
 * Base::Interpreter().runInteractiveString() (which also handles run-time exceptions, except 
 * for SystemExit).
 * 
 * The return value is True if the input is incomplete, False in the other cases (unless
 * an exception is raised). The return value can be used to decide whether to use sys.ps1 
 * or sys.ps2 to prompt the next line.
 */
bool InteractiveInterpreter::runSource(const char* source) const
{
  PyObject* code;
  try {
    code = compile(source);
  } catch (const Base::Exception&) {
    // A system, overflow or value error was raised.
    // We clear the traceback info as this might be a longly
    // message we don't need.
    PyObject *errobj, *errdata, *errtraceback;
    PyErr_Fetch(&errobj, &errdata, &errtraceback);
    PyErr_Restore(errobj, errdata, 0);
    // print error message
    if (PyErr_Occurred()) PyErr_Print();
    return false;
  }

  // the command is incomplete
  if (PyObject_TypeCheck(Py_None, code->ob_type)) {
    Py_DECREF(code);
    return true;
  }

  // run the code and return false
  runCode((PyCodeObject*)code);
  return false;
}

/* Execute a code object.
 *
 * When an exception occurs,  a traceback is displayed.
 * All exceptions are caught except SystemExit, which is reraised.
 */
void InteractiveInterpreter::runCode(PyCodeObject* code) const
{
  PyObject *module, *dict, *presult;           /* "exec code in d, d" */
  module = PyImport_AddModule("__main__");     /* get module, init python */
  if (module == NULL) 
    throw Base::PyException();                 /* not incref'd */
  dict = PyModule_GetDict(module);             /* get dict namespace */
  if (dict == NULL) 
    throw Base::PyException();                 /* not incref'd */

  presult = PyEval_EvalCode(code, dict, dict); /* run compiled bytecode */
  Py_XDECREF(code);                            /* decref the code object */
  if (!presult) {
	  if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
		  // throw SystemExit exception
      throw Base::SystemExitException();
	  }
    if ( PyErr_Occurred() )                    /* get latest python exception information */
      PyErr_Print();                           /* and print the error to the error output */
  }
}

bool InteractiveInterpreter::push(const char* line)
{
  d->buffer.append(line);
  QString source = d->buffer.join("\n");
  try {
    bool more = runSource(source.toAscii());
    if (!more)
      d->buffer.clear();
    return more;
  } catch (const Base::SystemExitException& e) {
    d->buffer.clear();
    throw e;
  } catch (const Base::Exception&) {
    d->buffer.clear();
  }
}

/* TRANSLATOR Gui::PythonConsole */

/**
 *  Constructs a PythonConsole which is a child of 'parent'. 
 */
PythonConsole::PythonConsole(QWidget *parent)
  : TextEdit(parent), WindowParameter( "Editor" ), _indent(false), _autoTabs(true)
{
  d = new PythonConsoleP();

  // create an instance of InteractiveInterpreter
  try { 
    d->interpreter = new InteractiveInterpreter();
  } catch (const Base::Exception& e) {
    setText(e.what());
    setEnabled(false);
  }

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
  d->_stdoutPy = new PythonStdoutPy(this);
  d->_stderrPy = new PythonStderrPy(this);
  d->_stdinPy  = new PythonStdinPy (this);
  d->_stdout = PySys_GetObject("stdout");
  d->_stderr = PySys_GetObject("stderr");
  d->_stdin  = PySys_GetObject("stdin");
  PySys_SetObject("stdin", d->_stdinPy);

  const char* version  = PyString_AsString(PySys_GetObject("version"));
  const char* platform = PyString_AsString(PySys_GetObject("platform"));
  _output = QString("Python %1 on + %2\n"
    "Type 'help', 'copyright', 'credits' or 'license' for more information.").arg(version).arg(platform);
  printPrompt(false);
}

/** Destroys the object and frees any allocated resources */
PythonConsole::~PythonConsole()
{
  getWindowParameter()->Detach( this );
  delete pythonSyntax;
  Py_XDECREF(d->_stdoutPy);
  Py_XDECREF(d->_stderrPy);
  Py_XDECREF(d->_stdinPy);
  delete d;
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
    if ( index < 5 || para < paragraphs()-1 )
    {
      QApplication::beep();
      return;
    }
  }
  // pass to base class or "eat" the action
  else if ( action == ActionDelete )
  {
    // must not modify ">>> " or "... "
    if ( index < 4 || para < paragraphs()-1 )
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
  _indent = false;
}

void PythonConsole::removeSelectedText ( int selNum )
{
  int paraFrom, indexFrom, paraTo, indexTo;
  getSelection (&paraFrom, &indexFrom, &paraTo, &indexTo, selNum);

  // cannot remove ">>> " or "... "
  if (paraTo == paragraphs()-1 && indexTo >= 4)
    TextEdit::removeSelectedText(selNum);
  else
    QApplication::beep();
}

/** Allows pasting in the active line only. */
void PythonConsole::paste()
{
  int para, index, indexFrom, paraTo, indexTo;
  int paraFrom = paragraphs()-1;
  getCursorPosition ( &para, &index );
  if ( hasSelectedText() )
    getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

  // cannot remove ">>> " or "... "
  if ( (paragraphs()-1 <= para) && index > 3 && paragraphs()-1 <= paraFrom )
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
  //int para, index, paraFrom, indexFrom, paraTo, indexTo;
  //getCursorPosition ( &para, &index );
  //getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

  //// cannot remove ">>> " or "... "
  //if ( (_startPara <= para) && index > 3 && _startPara <= paraFrom )
    TextEdit::cut();
  //else
  //  QApplication::beep();
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
  if ( (index < 4 || para < paragraphs()-1 ) && (e->state() == Qt::NoButton) )
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
      QString line = text(para-1);

      // and skip the first 4 characters consisting of either ">>> " or "... "
      line = line.mid(4);
      // truncate the last whitespace that is appended from QTextEdit
      line.truncate(line.length()-1);

      // put statement to the history
      if ( line.length() > 0 )
        _history.append(line);

      // evaluate and run the command
      runSource(line);
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
void PythonConsole::printPrompt(bool incomplete)
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

  if (incomplete) {
    // store paragraph where Python command starts
    _indent = true;
    append("... ");
    moveCursor( MoveLineEnd, false );
  } else {
    _indent = false;
    // go to last the paragraph as we don't know sure whether Python 
    // has written something to the console
    append(">>> ");
    setCursorPosition(paragraphs()-1, 0);
    moveCursor( MoveLineEnd, false );
  }
}

/**
 * Builds up the Python command and pass it to the interpreter.
 */
void PythonConsole::runSource(const QString& line)
{
  bool complete = false;
  PySys_SetObject("stdout", d->_stdoutPy);
  PySys_SetObject("stderr", d->_stderrPy);
  try {
    // launch the command now
    complete = d->interpreter->push(line.toAscii());
    setFocus(); // if focus was lost
  } catch (const Base::SystemExitException&) {
    int ret = QMessageBox::question(this, tr("System exit"), tr("The application is still running.\nDo you want to exit without saving your data?"),
      QMessageBox::Yes, QMessageBox::No|QMessageBox::Escape|QMessageBox::Default);
    if (ret == QMessageBox::Yes) {
      Base::Interpreter().systemExit();
    } else {
      PyErr_Clear();
    }
  }

  PySys_SetObject("stdout", d->_stdout);
  PySys_SetObject("stderr", d->_stderr);
  printPrompt(complete);
}

/**
 * Prints the Python statement cmd to the console.
 * @note The statement gets only printed but not invoked.
 */
bool PythonConsole::printStatement( const QString& cmd )
{
  // go to the last paragraph before inserting new text 
  int last = paragraphs() - 1;
  setCursorPosition(last, 0);
  moveCursor( MoveLineEnd, false );

  QStringList statements = QStringList::split( "\n", cmd );
  for ( QStringList::Iterator it = statements.begin(); it != statements.end(); ++it )
  {
    // printed command
    insert( *it );
    _history.append( *it );
    // prompt
    printPrompt(false);
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
      printStatement( QString("Gui.RunCommand(\"%1\")").arg(action));
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
