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


#ifndef PYTHON_CONSOLE_H
#define PYTHON_CONSOLE_H

#ifndef _PreComp_
# include <qstringlist.h>
# include <qsyntaxhighlighter.h>
#endif

#include "PythonEditor.h"

#include <Base/PyExportImp.h>

namespace Gui {

/**
 * This class implements the history for the Python console.
 * @author Werner Mayer
 */
class GuiExport ConsoleHistory
{
public:
  ConsoleHistory();
  ~ConsoleHistory();

  void first();
  bool more();
  bool next();
  bool prev();
  bool isEmpty() const;
  QString value() const;
  void append( const QString& );

private:
  QStringList _history;
  QStringList::ConstIterator it;
};

/**
 * Python text console with syntax highlighting..
 * @author Werner Mayer
 */
class PythonConsoleHighlighter;
class GuiExport PythonConsole : public TextEdit, public WindowParameter
{
  Q_OBJECT

public:
  PythonConsole(QWidget *parent = 0,const char *name = 0);
  ~PythonConsole();

  void OnChange( Base::Subject<const char*> &rCaller,const char* rcReason );
  void doKeyboardAction ( KeyboardAction action );
  void clear ();
  void paste();
  void pasteSubType( const QCString &subtype );
  void cut();
  void removeSelectedText ( int selNum = 0 );

  bool printCommand( const QString& cmd );

public slots:
  void onSaveHistoryAs();

protected:
  void keyPressEvent          ( QKeyEvent         * e );
  void showEvent              ( QShowEvent        * e );
  void contentsDropEvent      ( QDropEvent        * e );
  void contentsDragEnterEvent ( QDragEnterEvent   * e );
  void contentsDragMoveEvent  ( QDragMoveEvent    * e );

  void overwriteParagraph( int para, const QString& txt );
  bool isComment( const QString& ) const;
  bool isBlockComment( const QString& ) const;

  /** Pops up the context menu with some extensions */
  QPopupMenu * createPopupMenu ( const QPoint & pos );

  PYFUNCDEF_S(sStdoutPy);
  PYFUNCDEF_S(sStderrPy);
  PYFUNCDEF_S(sStdinPy);
  PYFUNCDEF_S(sStdout);
  PYFUNCDEF_S(sStderr);
  PYFUNCDEF_S(sStdin);
	static PyMethodDef    Methods[]; 

private:
  int tabsIndent( const QString& ) const;
  bool performPythonCommand();
  void printPrompt();
  void insertPythonOutput( const QString& );
  void insertPythonError ( const QString& );

private:
  int _startPara;
  bool _indent;
  bool _autoTabs;
  bool _blockComment;
  ConsoleHistory _history;
  QString _output, _error;

  static PythonConsole* _instance;
  static PyObject* _stdoutPy;
  static PyObject* _stderrPy;
  static PyObject* _stdinPy;
  static PyObject* _stdout;
  static PyObject* _stderr;
  static PyObject* _stdin;

  friend class PythonStdoutPy;
  friend class PythonStderrPy;

private:
  PythonConsoleHighlighter* pythonSyntax;
};

/**
 * Syntax highlighter for Python console.
 * \author Werner Mayer
 */
class GuiExport PythonConsoleHighlighter : public PythonSyntaxHighlighter
{
public:
  PythonConsoleHighlighter(QTextEdit* );
  ~PythonConsoleHighlighter();

  int highlightParagraph ( const QString & text, int endStateOfLastPara );

  void highlightOutput (bool b);
  void highlightError (bool b);

protected:
  void colorChanged( const QString& type, const QColor& col );

private:
  int endStateOfLastParagraph() const;

private:
  bool _output, _error;
  int _endState;

  friend class PythonConsole;
};

} // namespace Gui

#endif // __PYTHON_EDITOR_H__
