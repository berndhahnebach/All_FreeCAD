/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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

 


#ifndef _FeatureDataProviderPy_h_
#define _FeatureDataProviderPy_h_

#ifndef _PreComp_
# include <TDF_Label.hxx>
#endif

#include <Base/PyExportImp.h>

#include "FeatureDataProvider.h"

/*
class TFunction_Logbook;
class PyObjectBase;
class FeaturePy;
class TopoDS_Shape;
*/

namespace App
{

//===========================================================================
// FeaturePy - Python wrapper
//===========================================================================

/** The DocTypeStd python class
 */
class AppExport FeatureDataProviderPy :public Base::PyObjectBase
{
	/// always start with Py_Header
	Py_Header;

public:
	FeatureDataProviderPy(FeatureDataProvider *pcFeatureDataProvider, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~FeatureDataProviderPy();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
//	PYFUNCDEF_D(FeatureDataProviderPy,AddFeatureDataProvider)


	//---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------
  void SetProperty(const char *attr, PyObject *value);

private:
  FeatureDataProvider *_pcFeatureDataProvider;

};


} //namespace App



#endif
