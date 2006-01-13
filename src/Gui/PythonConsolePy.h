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


#ifndef __PYTHON_CONSOLE_PY_H__
#define __PYTHON_CONSOLE_PY_H__

#include <Base/PyExportImp.h>

#include "PythonEditor.h"

namespace Gui {
class PythonConsole;

/**
 * Python class for redirection of stdout to FreeCAD's Python
 * console window. This allows to show all Python messages in same 
 * window where the commands are performed.
 * @see PythonStderrPy
 * @see PythonConsole
 * @author Werner Mayer
 */
class GuiExport PythonStdoutPy : public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~PythonStdoutPy();

public:
  PythonStdoutPy(PythonConsole *pcWb, PyTypeObject *T = &Type);

  static PyObject *PyMake(PyObject *, PyObject *);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  virtual PyObject *_repr(void);                // the representation
  PyObject *_getattr(char *attr);               // __getattr__ function
  int _setattr(char *attr, PyObject *value);    // __setattr__ function

  PYFUNCDEF_D(PythonStdoutPy,write)
  PYFUNCDEF_D(PythonStdoutPy,flush)

private:
  PythonConsole* pyConsole;
};

/**
 * Python class for redirection of stderr to FreeCAD's Python
 * console window. This allows to show all Python messages in same 
 * window where the commands are performed.
 * @see PythonStdoutPy
 * @see PythonConsole
 * @author Werner Mayer
 */
class GuiExport PythonStderrPy : public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~PythonStderrPy();

public:
  PythonStderrPy(PythonConsole *pcWb, PyTypeObject *T = &Type);

  static PyObject *PyMake(PyObject *, PyObject *);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  virtual PyObject *_repr(void);                // the representation
  PyObject *_getattr(char *attr);               // __getattr__ function
  int _setattr(char *attr, PyObject *value);    // __setattr__ function

  PYFUNCDEF_D(PythonStderrPy,write)
  PYFUNCDEF_D(PythonStderrPy,flush)

private:
  PythonConsole* pyConsole;
};

/**
 * Python class for redirection of stdin to an input dialog of Qt.
 * @author Werner Mayer
 */
class GuiExport PythonStdinPy : public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~PythonStdinPy();

public:
  PythonStdinPy(PythonConsole *pcWb, PyTypeObject *T = &Type);

  static PyObject *PyMake(PyObject *, PyObject *);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  virtual PyObject *_repr(void);                // the representation
  PyObject *_getattr(char *attr);               // __getattr__ function
  int _setattr(char *attr, PyObject *value);    // __setattr__ function

  PYFUNCDEF_D(PythonStdinPy,readline)

private:
  PythonConsole* pyConsole;
};

} // namespace Gui

#endif // __PYTHON_CONSOLE_PY_H__
