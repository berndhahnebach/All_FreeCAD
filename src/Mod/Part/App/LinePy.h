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


#ifndef PART_LINEPY_H
#define PART_LINEPY_H

#include <Base/PyExportImp.h>
#include "PropertyLine.h"

namespace Part
{

/** The PropertyLine wrapper class for Python.
 * @author Werner Mayer
 */
class AppPartExport LinePy : public Base::PyObjectBase
{
	/** always start with Py_Header */
	Py_Header;

protected:
	/// Destruction 
	~LinePy();

public:
  /** @name Construction */
  //@{
	/// Constructor 
  LinePy(PyTypeObject *T = &Type);
	/// Constructor 
  LinePy(const Line3f& cLine, PyTypeObject *T = &LinePy::Type);
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

	const Line3f value(void) const {return _Line;}
	void setValue(const Line3f& line) {_Line = line;}

protected:
	Line3f _Line;
};

} //namespace Part

#endif // PART_LINEPY_H
