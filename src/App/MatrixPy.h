/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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

 


#ifndef _MatrixPy_h_
#define _MatrixPy_h_

#ifndef _PreComp_
# include <TDF_Label.hxx>
#endif

#include <Base/PyExportImp.h>

/*
class TFunction_Logbook;
class PyObjectBase;
class MatrixPy;
class TopoDS_Shape;
*/

#include <Base/Matrix.h>

namespace App
{

class Material;

//===========================================================================
// MatrixPy - Python wrapper
//===========================================================================

/** The DocTypeStd python class
 */
class AppExport MatrixPy :public Base::PyObjectBase
{
	/// always start with Py_Header
	Py_Header;

public:
  MatrixPy(const Base::Matrix4D &rcMatrix = Base::Matrix4D(), PyTypeObject *T = &Type);
	static PyObject *PyMake(PyTypeObject *, PyObject *, PyObject *);

	~MatrixPy();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PYFUNCDEF_D(MatrixPy,set)


	//---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------



  Base::Matrix4D _cMatrix;

};



} //namespace App



#endif
