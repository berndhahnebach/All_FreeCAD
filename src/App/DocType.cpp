/** \file FCDocType.cpp
 *  \brief document type system
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2000 - 2003                 *   
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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "../Base/PyExportImp.h"
#include "../Base/Console.h"

#include "Document.h"
#include "DocType.h"
#include "Feature.h"
#include "FeatureAttr.h"

using namespace App;


//===========================================================================
// DocType 
//===========================================================================

//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
DocType::DocType()
{

}

DocType::~DocType()
{

}


const char *DocType::GetTypeName(void)
{
	return "Base";
}



//===========================================================================
// DocTypeStd 
//===========================================================================

//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
DocTypeStd::DocTypeStd()
: DocType()
{

}

DocTypeStd::~DocTypeStd()
{

}


const char *DocTypeStd::GetTypeName(void)
{
	return "Std";
}

void DocTypeStd::Init (FCDocument *pcDoc)
{
	Base::Console().Log("Initialising Doc: %p trough DocTypeStd\n",pcDoc);

	TDF_Label lMain = pcDoc->Main();

	_lBase    = lMain.FindChild(1);
	_lPos     = lMain.FindChild(2);
	_lFeature = lMain.FindChild(3);


	TDataStd_Name::Set(_lBase,    TCollection_ExtendedString((Standard_CString)"Base"));
	TDataStd_Name::Set(_lPos,     TCollection_ExtendedString((Standard_CString)"Pos"));
	TDataStd_Name::Set(_lFeature, TCollection_ExtendedString((Standard_CString)"Features"));

	_iNextFreeFeature = 1;
	_lActiveFeature;
}

//**************************************************************************
// Feature handling


Feature *DocTypeStd::AddFeature(const char* sName)
{
	Feature *pcFeature = FeatureFactory().Produce(sName);

	if(pcFeature)
	{
		FeatureAttr::Set(_lFeature.FindChild(_iNextFreeFeature),pcFeature);
		_lActiveFeature = _lFeature.FindChild(_iNextFreeFeature);
		_iNextFreeFeature++;
		return pcFeature;

	}else return 0;

}


//===========================================================================
// TpyeStdPy - Python wrapper 
//===========================================================================

/** The DocTypeStd python class 
 */
class AppExport DocTypeStdPy :public FCPyObject
{
	/// always start with Py_Header
	Py_Header;

public:
	DocTypeStdPy(DocTypeStd *pcDocTypeStd, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~DocTypeStdPy();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function


private:
	DocTypeStd *_pcDocTypeStd;

};

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject DocTypeStdPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"DocTypeStdPy",				/*tp_name*/
	sizeof(DocTypeStdPy),			/*tp_basicsize*/
	0,						/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 			/*tp_print*/
	__getattr, 				/*tp_getattr*/
	__setattr, 				/*tp_setattr*/
	0,						/*tp_compare*/
	__repr,					/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash*/
	0,						/*tp_call */
};

//--------------------------------------------------------------------------
// Methods structure
//--------------------------------------------------------------------------
PyMethodDef DocTypeStdPy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject DocTypeStdPy::Parents[] = {&FCPyObject::Type, NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
DocTypeStdPy::DocTypeStdPy(DocTypeStd *pcDocTypeStd, PyTypeObject *T)
: FCPyObject( T), _pcDocTypeStd(pcDocTypeStd)
{
	//Base::Console().Log("Create DocTypeStdPy: %p \n",this);
}

PyObject *DocTypeStdPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new DocTypeStdPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

FCPyObject *DocTypeStd::GetPyObject(void)
{
	return new DocTypeStdPy(this);
}


//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
DocTypeStdPy::~DocTypeStdPy()						// Everything handled in parent
{
//	Base::Console().Log("Destroy DocTypeStdPy: %p \n",this);
} 


//--------------------------------------------------------------------------
// DocTypeStdPy representation
//--------------------------------------------------------------------------
PyObject *DocTypeStdPy::_repr(void)
{
	return Py_BuildValue("s", "FreeCAD Document");
}
//--------------------------------------------------------------------------
// DocTypeStdPy Attributes
//--------------------------------------------------------------------------
PyObject *DocTypeStdPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (streq(attr, "XXXX"))						
			return Py_BuildValue("i",1); 
		else
			_getattr_up(FCPyObject); 						
	}catch(...){
		Py_Error(PyExc_Exception,"Error in get Attribute");
	}
} 

int DocTypeStdPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (streq(attr, "XXXX")){						// settable new state
		//_pcDoc->SetUndoLimit(PyInt_AsLong(value)); 
		return 1;
	}else  
		return FCPyObject::_setattr(attr, value); 	// send up to parent
} 



//--------------------------------------------------------------------------
// Python wrappers
//--------------------------------------------------------------------------
/*
PyObject *DocTypeStdPy::PyUndo(PyObject *args)
{ 
	_pcDoc->Undo(); 
	Py_Return; 
} 
*/
