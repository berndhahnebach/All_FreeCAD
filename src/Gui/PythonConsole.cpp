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

#include <Base/Interpreter.h>
#include <Base/Exception.h>

using namespace Gui;

PythonConsole * PythonConsole::_instance = 0;
PyObject      * PythonConsole::_stdoutPy = 0;
PyObject      * PythonConsole::_stderrPy = 0;
PyObject      * PythonConsole::_stdout   = 0;
PyObject      * PythonConsole::_stderr   = 0;

/**
 *  Constructs a PythonConsole which is a child of 'parent', with the
 *  name 'name'. 
 */
PythonConsole::PythonConsole(QWidget *parent,const char *name)
    : PythonWindow(parent, name), _startPara(0), _indent(false)
{
  _instance = this;
  _stdoutPy = new PythonStdoutPy( _instance );
  _stderrPy = new PythonStderrPy( _instance );

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
	  (void) Py_InitModule("PyConsole", PythonConsole::Methods);
    Base::Interpreter().runString("import sys, PyConsole");
    Base::Interpreter().runString("PyConsole.redirectStdout()");
    Base::Interpreter().runString("PyConsole.redirectStderr()");
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
}

// PythonConsole Methods						// Methods structure
PyMethodDef PythonConsole::Methods[] = {
	{"redirectStdout", (PyCFunction) PythonConsole::sStdoutPy,  1},
	{"redirectStderr", (PyCFunction) PythonConsole::sStderrPy,  1},
	{"restoreStdout",  (PyCFunction) PythonConsole::sStdout,    1},
	{"restoreStderr",  (PyCFunction) PythonConsole::sStderr,    1},
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

  PythonWindow::doKeyboardAction( action );
}

/** Clears the content and writes the propmpt. */
void PythonConsole::clear ()
{
  PythonWindow::clear();
  setText(">>> ");
  moveCursor( MoveLineEnd, false );
}

void PythonConsole::removeSelectedText ( int selNum )
{
  int para, index;
  getCursorPosition ( &para, &index );

  // cannot remove ">>> " or "... "
  if ( (_startPara <= para) && index > 3 )
    PythonWindow::removeSelectedText( selNum );
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
    PythonWindow::paste();
  else
    QApplication::beep();
}

/** Allows cutting in the active line only. */
void PythonConsole::cut()
{
  int para, index, paraFrom, indexFrom, paraTo, indexTo;
  getCursorPosition ( &para, &index );
  getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

  // cannot remove ">>> " or "... "
  if ( (_startPara <= para) && index > 3 && _startPara <= paraFrom )
    PythonWindow::cut();
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

  PythonWindow::keyPressEvent(e);

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
        _history.prepend( txt );

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
  case Key_P:
    if ( e->state() & AltButton )
    {
      if ( _history.size() > 0 )
      {
        QString cmd = _history.front();
        _history.pop_front();
        _history.append( cmd );
//        setCursorPosition( paragraphs()-1, 4 );
  //      setOverwriteMode(true);
        overwriteParagraph( paragraphs()-1, cmd );
//        insert( cmd );
  //      setOverwriteMode(false);
      }
    }
    break;
  case Key_N:
    if ( e->state() & AltButton )
    {
      if ( _history.size() > 0 )
      {
        QString cmd = _history.back();
        _history.pop_back();
        _history.prepend( cmd );
//        setCursorPosition( paragraphs()-1, 4 );
  //      setOverwriteMode(true);
        overwriteParagraph( paragraphs()-1, cmd );
//        insert( cmd );
  //      setOverwriteMode(false);
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
  PythonWindow::showEvent( e );
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

  PythonWindow::dropEvent(e);
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
    PythonWindow::dragEnterEvent( e );
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
