/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef POINTS_PY_H
#define POINTS_PY_H

#include <Base/PyExportImp.h>


namespace Points
{

class PointsWithProperty;

//===========================================================================
// PointsPy - Python wrapper 
//===========================================================================

// The DocTypeStd python class 
class PointsAppExport PointsPy :public Base::PyObjectBase
{
  /// always start with Py_Header
  Py_Header;

protected:
  ~PointsPy();

public:
  PointsPy(PyTypeObject *T = &Type);
  PointsPy(const PointsWithProperty& rcPoints, PyTypeObject *T = &Type);
  static PyObject *PyMake(PyObject *, PyObject *);

  void setPoints(const PointsWithProperty& rcPoints);
  const PointsWithProperty& getPoints(void) const;
  PointsWithProperty& refToPoints(void);

  //---------------------------------------------------------------------
  // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------

  virtual PyObject *_repr(void);  				// the representation
  PyObject *_getattr(char *attr);					// __getattr__ function
  int _setattr(char *attr, PyObject *value);		// __setattr__ function

  
  PYFUNCDEF_D(PointsPy,count)
  PYFUNCDEF_D(PointsPy,read)
  PYFUNCDEF_D(PointsPy,write)
  PYFUNCDEF_D(PointsPy,translate)
  PYFUNCDEF_D(PointsPy,rotate)
  PYFUNCDEF_D(PointsPy,scale)
  PYFUNCDEF_D(PointsPy,addPoint)
  PYFUNCDEF_D(PointsPy,clear)
  PYFUNCDEF_D(PointsPy,copy)

protected:
  PointsWithProperty *_pcPoints;
};

} //namespace Points

 

#endif // POINTS_PY_H 
