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
#	include <TDataStd_Name.hxx>
#	include <TDF_Label.hxx>
# include <TDF_MapIteratorOfLabelMap.hxx>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "../Base/PyExportImp.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

#include "Document.h"
#include "DocType.h"
#include "Feature.h"
#include "FeatureAttr.h"

using namespace App;


//===========================================================================
// TpyeStdPy - Python wrapper 
//===========================================================================

/** The DocTypeStd python class 
 */
class AppExport DocTypeStdPy :public Base::FCPyObject
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
	PYFUNCDEF_D(DocTypeStdPy,AddFeature)
	PYFUNCDEF_D(DocTypeStdPy,GetActiveFeature)
	PYFUNCDEF_D(DocTypeStdPy,Update)

private:
	DocTypeStd *_pcDocTypeStd;

};


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
	_pcDocTypeStdPy = new DocTypeStdPy(this);
	//_pcDocTypeStdPy->_INCREF();

}

DocTypeStd::~DocTypeStd()
{
	_pcDocTypeStdPy->_DECREF();

}


const char *DocTypeStd::GetTypeName(void)
{
	return "Std";
}

Base::FCPyObject *DocTypeStd::GetPyObject(void)
{
	_pcDocTypeStdPy->_INCREF();
	return _pcDocTypeStdPy;
}




void DocTypeStd::Init (FCDocument *pcDoc)
{
	Base::Console().Log("Initialising Doc: %p trough DocTypeStd\n",pcDoc);

  _pcDoc = pcDoc;

	TDF_Label lMain = pcDoc->Main();

	_lBase    = lMain.FindChild(1);
	_lPos     = lMain.FindChild(2);
	_lFeature = lMain.FindChild(3);


	TDataStd_Name::Set(_lBase,    TCollection_ExtendedString((Standard_CString)"Base"));
	TDataStd_Name::Set(_lPos,     TCollection_ExtendedString((Standard_CString)"Pos"));
	TDataStd_Name::Set(_lFeature, TCollection_ExtendedString((Standard_CString)"Features"));

	_iNextFreeFeature = 1;
//	_lActiveFeature; 
}

//**************************************************************************
// Feature handling


Feature *DocTypeStd::AddFeature(const char* sName)
{
	Feature *pcFeature = FeatureFactory().Produce(sName);

	if(pcFeature)
	{
		// next free label
		TDF_Label FeatureLabel = _lFeature.FindChild(_iNextFreeFeature++);
		// mount the feature on its place
		FeatureAttr::Set(FeatureLabel,pcFeature);
		// name
		TDataStd_Name::Set(FeatureLabel,TCollection_ExtendedString((Standard_CString) sName ));
		// the rest of the setup do the feature itself
		pcFeature->AttachLabel(FeatureLabel);
		// update the pointer
		_lActiveFeature = FeatureLabel;

    pcFeature->SetDocType(this);

		// return the feature
		return pcFeature;

	}else return 0;

}

Feature *DocTypeStd::GetActiveFeature(void)
{
  // checks if there is no active Feature
  if(_lActiveFeature.IsNull()) 
    return 0;
  
  return GetFeature(_lActiveFeature);
}

Feature *DocTypeStd::GetFeature(const TDF_Label &l)
{
  Handle(FeatureAttr) hFeat;
  
  if(! l.FindAttribute(FeatureAttr::GetID(),hFeat))
    //throw Base::Exception("DocTypeStd::GetFeature() internal error, feature attribute missing\n");
    return 0;
  
  return hFeat->Get();
}

//**************************************************************************
// State handling


void DocTypeStd::TouchState(const TDF_Label &l)
{
  _LogBook.SetTouched(l);
}

void DocTypeStd::ImpactState(const TDF_Label &l)
{
  _LogBook.SetImpacted(l);
}



void DocTypeStd::UpdateDoc(void)
{

  TDF_MapIteratorOfLabelMap It;

  for(It.Initialize(_LogBook.GetTouched());It.More();It.Next())
  {
    // Get the Feature of this label
    Feature *Feat = GetFeature(It.Key());
    if(!Feat) continue;

    // find the name of this label
    Handle(TDataStd_Name) hName;
    if(! It.Key().FindAttribute(TDataStd_Name::GetID(),hName))
      throw Base::Exception("DocTypeStd::UpdateDoc() internal error, feature label with no name\n");
    Base::Console().Log("Update: %s\n",TCollection_AsciiString(hName->Get()).ToCString());

		if (Feat->MustExecute(_LogBook))
		{
			//_LogBook.SetTouched(It.Key());
			if(Feat->Execute(_LogBook))
        Base::Console().Message("Recompute of Feature failed\n");
		}

  }

  _pcDoc->Recompute();
}


//===========================================================================
// TpyeStdPy - Python wrapper
//===========================================================================


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
//	{"AddFeature",         (PyCFunction) sAddFeature,         Py_NEWARGS},
	PYMETHODEDEF(AddFeature)
	PYMETHODEDEF(GetActiveFeature)
	PYMETHODEDEF(Update)
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
	Base::Console().Log("Create DocTypeStdPy: %p \n",this);
}

PyObject *DocTypeStdPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new DocTypeStdPy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}


//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
DocTypeStdPy::~DocTypeStdPy()						// Everything handled in parent
{
	Base::Console().Log("Destroy DocTypeStdPy: %p \n",this);
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
	PY_TRY{
		if (Base::streq(attr, "XXXX"))						
			return Py_BuildValue("i",1); 
		else
			_getattr_up(FCPyObject); 						
	}PY_CATCH;
} 

int DocTypeStdPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	if (Base::streq(attr, "XXXX")){						// settable new state
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

PYFUNCIMP_D(DocTypeStdPy,AddFeature)
{
	char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

  PY_TRY {
	  Feature *pcFtr = _pcDocTypeStd->AddFeature(pstr);
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
		  Py_Error(PyExc_Exception,"No Feature with this name!");
  }PY_CATCH;
}


PYFUNCIMP_D(DocTypeStdPy,GetActiveFeature)
{
	
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
    return NULL;                       // NULL triggers exception 

  PY_TRY {
	  Feature *pcFtr = _pcDocTypeStd->GetActiveFeature();
	  if(pcFtr)
		  return pcFtr->GetPyObject();
	  else
		  Py_Error(PyExc_Exception,"No active Feature");
  } PY_CATCH;
}


PYFUNCIMP_D(DocTypeStdPy,Update)
{
  if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

  PY_TRY{
	  _pcDocTypeStd->UpdateDoc();
  
    Py_Return;
  }PY_CATCH;

}
