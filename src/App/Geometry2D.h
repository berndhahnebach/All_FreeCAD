/** \file Geometry2D.h
 *  \brief FileTemplate example header
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.cpp
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#ifndef __GEOMETRY2D_H__
#define __GEOMETRY2D_H__

// Std. configurations

#include "../Base/PyExport.h"

//#include <string>
//#include <map>

#include <TopoDS_Shape.hxx>

/** The TopoDSShape wrapper class
 *  This class wrapps the functionality of the Topology package. It wrapps not
 *  strictly after the OCC rules. It includes also a lot algorithems from oter
 *  packages like BRepTools and BRepBuilder. Also iterators and so on.
 */
class AppExport FCGeometry2D :public FCPyObject
{
	// always start with Py_Header 
	Py_Header;

public:

	/// Constructer 
	FCGeometry2D(const TopoDS_Shape &cShape, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCGeometry2D();


	//---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	/// Gets the OCC Label
	TopoDS_Shape GetTopoShape(void){return _cTopoShape;}

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods

	PYFUNCDEF_D (FCGeometry2D,PyHasChild);


protected:

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#endif
	/// The OCC Label
	TopoDS_Shape _cTopoShape;

};




#endif // __GEOMETRY2D_H__