/** \file $RCSfile$
 *  \brief The attribute module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

#include "PreCompiled.h"

#ifndef _PreComp_
#	include <TDF_Label.hxx>
#	include <TDF_ChildIterator.hxx>
#	include <TDF_Tool.hxx>
#	include <TCollection_AsciiString.hxx>
#	include <TDF_ListIteratorOfAttributeList.hxx>
#	include <Standard_GUID.hxx>
#endif

#include "../Base/PyExportImp.h"
#include "../Base/Console.h"
using Base::Console;


#include "Feature.h"

using namespace App;

//===========================================================================
// FeaturePy - Python wrapper 
//===========================================================================

/** The DocTypeStd python class 
 */
class AppExport FeaturePy :public Base::FCPyObject
{
	/// always start with Py_Header
	Py_Header;

public:
	FeaturePy(Feature *pcFeature, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~FeaturePy();

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function


private:
	Feature *_pcFeature;

};

//--------------------------------------------------------------------------
// Type structure
//--------------------------------------------------------------------------

PyTypeObject FeaturePy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						/*ob_size*/
	"FeaturePy",				/*tp_name*/
	sizeof(FeaturePy),			/*tp_basicsize*/
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
PyMethodDef FeaturePy::Methods[] = {
//  {"Undo",         (PyCFunction) sPyUndo,         Py_NEWARGS},

  {NULL, NULL}		/* Sentinel */
};

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject FeaturePy::Parents[] = {&FCPyObject::Type, NULL};     

//--------------------------------------------------------------------------
//t constructor
//--------------------------------------------------------------------------
FeaturePy::FeaturePy(Feature *pcFeature, PyTypeObject *T)
: FCPyObject( T), _pcFeature(pcFeature)
{
	//Base::Console().Log("Create FeaturePy: %p \n",this);
}

PyObject *FeaturePy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
  //return new FeaturePy(name, n, tau, gamma);			// Make new Python-able object
	return 0;
}

Base::FCPyObject *Feature::GetPyObject(void)
{
	return new FeaturePy(this);
}


//--------------------------------------------------------------------------
// destructor 
//--------------------------------------------------------------------------
FeaturePy::~FeaturePy()						// Everything handled in parent
{
//	Base::Console().Log("Destroy FeaturePy: %p \n",this);
} 


//--------------------------------------------------------------------------
// FeaturePy representation
//--------------------------------------------------------------------------
PyObject *FeaturePy::_repr(void)
{
	return Py_BuildValue("s", "FreeCAD Document");
}
//--------------------------------------------------------------------------
// FeaturePy Attributes
//--------------------------------------------------------------------------
PyObject *FeaturePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try{
		if (Base::streq(attr, "XXXX"))						
			return Py_BuildValue("i",1); 
		else
			_getattr_up(FCPyObject); 						
	}catch(...){
		Py_Error(PyExc_Exception,"Error in get Attribute");
	}
} 

int FeaturePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
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
PyObject *FeaturePy::PyUndo(PyObject *args)
{ 
	_pcDoc->Undo(); 
	Py_Return; 
} 
*/



//===========================================================================
// FeatureFactorySingleton - Factory for Features
//===========================================================================



FeatureFactorySingleton* FeatureFactorySingleton::_pcSingleton = NULL;

FeatureFactorySingleton& FeatureFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FeatureFactorySingleton;
  return *_pcSingleton;
}

void FeatureFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

Feature* FeatureFactorySingleton::Produce (const char* sName) const
{
	Feature* w = (Feature*)Produce(sName);

  // this Feature class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    Console().Warning("\"%s\" is not registered\n", sName);
#else
    Console().Log("\"%s\" is not registered\n", sName);
#endif
    return NULL;
  }

  return w;
}

