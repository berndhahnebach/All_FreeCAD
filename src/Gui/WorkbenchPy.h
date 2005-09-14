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

#ifndef _PreComp_
#endif

#include "../Base/PyExportImp.h"

namespace Gui {
class PythonWorkbench;

/** 
 * The workbench Python class.
 * @author Werner Mayer
 */
class GuiExport WorkbenchPy : public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

public:
  WorkbenchPy(PythonWorkbench *pcWb, PyTypeObject *T = &Type);
  static PyObject *PyMake(PyObject *, PyObject *);

  ~WorkbenchPy();

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------
  virtual PyObject *_repr(void);                // the representation
  PyObject *_getattr(char *attr);               // __getattr__ function
  int _setattr(char *attr, PyObject *value);    // __setattr__ function

  PYFUNCDEF_D(WorkbenchPy,Name)
  PYFUNCDEF_D(WorkbenchPy,Activate)

  // menu stuff
  PYFUNCDEF_D(WorkbenchPy,AppendMenu)
  PYFUNCDEF_D(WorkbenchPy,RemoveMenu)

  // context menu stuff
  PYFUNCDEF_D(WorkbenchPy,AppendContextMenu)
  PYFUNCDEF_D(WorkbenchPy,RemoveContextMenu)

  // toolbar stuff
  PYFUNCDEF_D(WorkbenchPy,AppendToolbar)
  PYFUNCDEF_D(WorkbenchPy,RemoveToolbar)

  // commandbar stuff
  PYFUNCDEF_D(WorkbenchPy,AppendCommandbar)
  PYFUNCDEF_D(WorkbenchPy,RemoveCommandbar)

protected:
  PythonWorkbench *_pcWorkbench;
};

} // namespace Gui 

#endif // WORKBENCH_PY_H 
