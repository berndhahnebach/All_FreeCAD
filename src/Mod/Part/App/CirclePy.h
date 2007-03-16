/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef PART_CIRCLEPY_H
#define PART_CIRCLEPY_H

#include <Base/PyObjectBase.h>
#include <gp_Circ.hxx>

namespace Part
{

/** The Circle wrapper class for Python.
 * @author Werner Mayer
 */
class AppPartExport CirclePy : public Base::PyObjectBase
{
	/** always start with Py_Header */
	Py_Header;

protected:
	/// Destruction 
	~CirclePy();

public:
  /** @name Construction */
  //@{
	/// Constructor 
  CirclePy(PyTypeObject *T = &Type);
	/// Constructor 
  CirclePy(const gp_Circ& circle, PyTypeObject *T = &CirclePy::Type);
  /// For construction in Python 
  static PyObject *PyMake(PyTypeObject*, PyObject*, PyObject*);
  /// For initialization in Python 
  static int PyInit(PyObject*, PyObject*, PyObject*);
  //@}

  //---------------------------------------------------------------------
  // Python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
  //---------------------------------------------------------------------

  /** @name Python export */
  //@{
  /// Representation
  PyObject *_repr(void);  				
  /// Get attributes
	PyObject *_getattr(char *attr);
  /// Set attributes
	int _setattr(char *attr, PyObject *value);
  //@}

	const gp_Circ& value(void) const {return _circle;}
	void setValue(const gp_Circ& circle) {_circle = circle;}

  PYFUNCDEF_D(CirclePy,radius)
  PYFUNCDEF_D(CirclePy,setRadius)
  PYFUNCDEF_D(CirclePy,position)
  PYFUNCDEF_D(CirclePy,setPosition)
  PYFUNCDEF_D(CirclePy,axis)
  PYFUNCDEF_D(CirclePy,setAxis)

protected:
	gp_Circ _circle;
};

} //namespace Part

#endif // PART_CIRCLEPY_H
