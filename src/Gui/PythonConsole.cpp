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

PythonConsole* PythonConsole::instance = 0;

/**
 *  Constructs a PythonConsole which is a child of 'parent', with the
 *  name 'name'. 
 */
PythonConsole::PythonConsole(QWidget *parent,const char *name)
    : PythonWindow(parent, name), startPara(0), indent(false)
{
  instance = this;

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
	  (void) Py_InitModule("PyConsole", PythonConsole::Methods);
    Base::Interpreter().runString("import sys, PyConsole");
    Base::Interpreter().runString("sys.stdout=PyConsole.stdout()");
    Base::Interpreter().runString("sys.stderr=PyConsole.stderr()");
//        self.write("Python %s on %s\n%s\n%s\nIDLE %s      %s\n" %
//                   (sys.version, sys.platform, self.COPYRIGHT,
//                    self.firewallmessage, idlever.IDLE_VERSION, nosub))
//    PySys_SetObject("stdout",new PythonStdoutPy(this));
  }
  catch (...)
  {
  }

  setText(">>> ");
  moveCursor( MoveLineEnd, false );
}

/** Destroys the object and frees any allocated resources */
PythonConsole::~PythonConsole()
{
}

// PythonConsole Methods						// Methods structure
PyMethodDef PythonConsole::Methods[] = {
	{"stdout", (PyCFunction) PythonConsole::sPyStdout,      1},
	{"stderr", (PyCFunction) PythonConsole::sPyStderr,      1},
  {NULL, NULL}		/* Sentinel */
};

PYFUNCIMP_S(PythonConsole,sPyStdout)
{
  return new PythonStdoutPy( instance ); 
}

PYFUNCIMP_S(PythonConsole,sPyStderr)
{
//  PySys_SetObject("",0);
//  PyObject *f = PySys_GetObject("stderr");
  return new PythonStderrPy( instance ); 
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
    if ( index < 5 || para < startPara )
    {
      QApplication::beep();
      return;
    }
  }
  // pass to base class or "eat" the action
  else if ( action == ActionDelete )
  {
    // must not modify ">>> " or "... "
    if ( index < 4 || para < startPara )
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
  if ( (startPara <= para) && index > 3 )
    PythonWindow::removeSelectedText( selNum );
  else
    QApplication::beep();
}

/** Allows pasting in the active line only. */
void PythonConsole::paste()
{
  int para, index, indexFrom, paraTo, indexTo;
  int paraFrom = startPara;
  getCursorPosition ( &para, &index );
  if ( hasSelectedText() )
    getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

  // cannot remove ">>> " or "... "
  if ( (startPara <= para) && index > 3 && startPara <= paraFrom )
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
  if ( (startPara <= para) && index > 3 && startPara <= paraFrom )
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
  if ( (index < 4 || para < startPara ) && (e->state() == Qt::NoButton) )
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

      // check if we must begin the next line either with ">>> " or "... "
      // (matching Python keyword followed by any characters followed by a colon,
      //  this is a block-opening statement )
      QRegExp le("\\b(?:class|def|eif|else|except|for|if|try|while)\\b.*:");
      le.setMinimal( true ); // not in greedy mode
      int pos = txt.find( le );
      bool ind1 = pos > -1;

      if ( ind1 )
      {
        // furthermore check if the actual command follows the colon
        QString cmd = txt.mid( pos + le.matchedLength() );
        QRegExp re("^\\s+$");
        // not empty, thus call Python interpreter immediately
        ind1 = cmd.find( re ) > -1;
      }

      // if we are already in "..." mode test if this paragraph either is empty
      // or consists only of whitespaces
      QRegExp re("^\\s+$");
      bool ind2 = (indent == true && txt.find( re ) == -1 );
      if (  ind1 || ind2 )
      {
        // store paragraph where Python command starts
        if ( indent == false )
          startPara = para - 1;
        indent = true;
        insertAt("... \t", para, 0 );
        moveCursor( MoveLineEnd, false );
      }
      else
      {
        // Build up Python command from startPara to para
        QString pyCmd;
        if ( startPara < para-1 )
        {
          for ( int i=startPara; i<para-1; i++ )
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
        }
        catch ( const Base::Exception& )
        {
          // Write Python's error output instead, if there is!
        }

        printPrompt();
      }
    } break;
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
  startPara = paragraphs() - 1;
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
  startPara = paragraphs() - 1;
}

/** Prints the ps1 prompt (>>> ) to the console window. */ 
void PythonConsole::printPrompt()
{
  // go to last the paragraph as we don't know sure whether Python 
  // has written something to the console
  int para = paragraphs();
  startPara = para - 1;
  setCursorPosition(para-1, 0);
  indent = false;
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
