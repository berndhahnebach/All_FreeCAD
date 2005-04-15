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


#ifndef __PYTHON_CONSOLE_H__
#define __PYTHON_CONSOLE_H__

#ifndef _PreComp_
# include <qstringlist.h>
#endif

#include "PythonEditor.h"
#include <Base/PyExportImp.h>

namespace Gui {

/**
 * Python text console with syntax highlighting..
 * @author Werner Mayer
 */
class GuiExport PythonConsole : public PythonWindow
{
public:
  PythonConsole(QWidget *parent = 0,const char *name = 0);
  ~PythonConsole();

  void doKeyboardAction ( KeyboardAction action );
  void clear ();
  void paste();
  void cut();
  void removeSelectedText ( int selNum = 0 );

  bool printCommand( const QString& cmd );

protected:
  void keyPressEvent          ( QKeyEvent         * e );
  void showEvent              ( QShowEvent        * e );
  void contentsDropEvent      ( QDropEvent        * e );
  void contentsDragEnterEvent ( QDragEnterEvent   * e );
  void contentsDragMoveEvent  ( QDragMoveEvent    * e );

  void overwriteParagraph( int para, const QString& txt );

  PYFUNCDEF_S(sStdoutPy);
  PYFUNCDEF_S(sStderrPy);
  PYFUNCDEF_S(sStdout);
  PYFUNCDEF_S(sStderr);
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
  QStringList _history;

  static PythonConsole* _instance;
  static PyObject* _stdoutPy;
  static PyObject* _stderrPy;
  static PyObject* _stdout;
  static PyObject* _stderr;

  friend class PythonStdoutPy;
  friend class PythonStderrPy;
};

} // namespace Gui

#endif // __PYTHON_EDITOR_H__
