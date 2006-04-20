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


#ifndef WORKBENCH_PY_H
#define WORKBENCH_PY_H

#include "../Base/PyExportImp.h"

namespace Gui {
class Workbench;
class PythonWorkbench;

/** 
 * The workbench Python base class doesn't allow you to manipulate the C++ workbench class behind.
 * You're only allowed either to activate the workbench class or get its name.
 * The WorkbenchPy class is associated to all C++ workbench classes except of PythonWorkbench.
 * @see Workbench
 * @see PythonWorkbench
 * @see PythonWorkbenchPy
 * @author Werner Mayer
 */
class GuiExport WorkbenchPy : public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~WorkbenchPy();

public:
  WorkbenchPy(Workbench* pcWb, PyTypeObject *T = &Type);
  static PyObject *PyMake(PyObject *, PyObject *);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  virtual PyObject *_repr(void);                // the representation
  PyObject *_getattr(char *attr);               // __getattr__ function
  int _setattr(char *attr, PyObject *value);    // __setattr__ function

  /** @name Basic methods */
  //@{
  /// Retrieves the workbench name
  PYFUNCDEF_D(WorkbenchPy,Name)
  /// Activates the workbench object
  PYFUNCDEF_D(WorkbenchPy,Activate)
  //@}

protected:
  Workbench *_pcWorkbench;
};

/** 
 * The workbench Python class provides additional methods for manipulation of python worbench
 * objects. 
 * From the view of Python PythonWorkbenchPy is also derived from WorkbenchPy as in C++.
 * @see Workbench
 * @see WorkbenchPy
 * @see PythonWorkbench
 * @author Werner Mayer
 */
class GuiExport PythonWorkbenchPy : public WorkbenchPy
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~PythonWorkbenchPy();

public:
  PythonWorkbenchPy(PythonWorkbench *pcWb, PyTypeObject *T = &Type);
  static PyObject *PyMake(PyObject *, PyObject *);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  virtual PyObject *_repr(void);                // the representation
  PyObject *_getattr(char *attr);               // __getattr__ function
  int _setattr(char *attr, PyObject *value);    // __setattr__ function

  /** @name Manipulation methods */
  //@{
  // menu stuff
  /// Appends a new menu
  PYFUNCDEF_D(PythonWorkbenchPy,AppendMenu)
  /// Removes a menu
  PYFUNCDEF_D(PythonWorkbenchPy,RemoveMenu)
  //// Shows a list of all menus
  PYFUNCDEF_D(PythonWorkbenchPy,ListMenus)

  // context menu stuff
  /// Appends new context menu items
  PYFUNCDEF_D(PythonWorkbenchPy,AppendContextMenu)
  /// Removes a context menu
  PYFUNCDEF_D(PythonWorkbenchPy,RemoveContextMenu)

  // toolbar stuff
  /// Appends a new toolbar
  PYFUNCDEF_D(PythonWorkbenchPy,AppendToolbar)
  /// Removes a toolbar
  PYFUNCDEF_D(PythonWorkbenchPy,RemoveToolbar)
  //// Shows a list of all toolbars
  PYFUNCDEF_D(PythonWorkbenchPy,ListToolbars)

  // commandbar stuff
  /// Appends a new command bar
  PYFUNCDEF_D(PythonWorkbenchPy,AppendCommandbar)
  /// Removes a command bar
  PYFUNCDEF_D(PythonWorkbenchPy,RemoveCommandbar)
  //// Shows a list of all command bars
  PYFUNCDEF_D(PythonWorkbenchPy,ListCommandbars)
  //@}

protected:
  PythonWorkbench *_pcWorkbench;
};

} // namespace Gui 

#endif // WORKBENCH_PY_H 
