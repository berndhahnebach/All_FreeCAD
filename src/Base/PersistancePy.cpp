
#include "PreCompiled.h"

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/PyCXX/Objects.hxx>

#include "Persistance.h"
#include "PersistancePy.h"
#define new DEBUG_CLIENTBLOCK

using Base::streq;
using namespace Base;

/// Type structure of PersistancePy
PyTypeObject PersistancePy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						              /*ob_size*/
	"Persistance",				/*tp_name*/
	sizeof(PersistancePy), /*tp_basicsize*/
	0,						              /*tp_itemsize*/
	/* methods */
	PyDestructor,	  		        /*tp_dealloc*/
	0,			 			              /*tp_print*/
	__getattr, 				          /*tp_getattr*/
	__setattr, 				          /*tp_setattr*/
	0,						              /*tp_compare*/
	__repr,					            /*tp_repr*/
	0,						              /*tp_as_number*/
	0,						              /*tp_as_sequence*/
	0,						              /*tp_as_mapping*/
	0,						              /*tp_hash*/
	0,						              /*tp_call */
	0,                                                /*tp_str  */
	0,                                                /*tp_getattro*/
	0,                                                /*tp_setattro*/
	/* --- Functions to access object as input/output buffer ---------*/
	0,                                                /* tp_as_buffer */
	/* --- Flags to define presence of optional/expanded features */
	Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
	"About Persistance",                       /*tp_doc */
	0,                                                /*tp_traverse */
	0,                                                /*tp_clear */
	0,                                                /*tp_richcompare */
	0,                                                /*tp_weaklistoffset */
	0,                                                /*tp_iter */
	0,                                                /*tp_iternext */
	Base::PersistancePy::Methods,                /*tp_methods */
	0,                                                /*tp_members */
	0,                                                /*tp_getset */
	&Base::BaseClassPy::Type,                        /*tp_base */
	0,                                                /*tp_dict */
	0,                                                /*tp_descr_get */
	0,                                                /*tp_descr_set */
	0,                                                /*tp_dictoffset */
	0,                                                /*tp_init */
	0,                                                /*tp_alloc */
	0,                                                /*tp_new */
	0,                                                /*tp_free   Low-level free-memory routine */
	0,                                                /*tp_is_gc  For PyObject_IS_GC */
	0,                                                /*tp_bases */
	0,                                                /*tp_mro    method resolution order */
	0,                                                /*tp_cache */
	0,                                                /*tp_subclasses */
	0                                                 /*tp_weaklist */
};

/// Methods structure of PersistancePy
PyMethodDef PersistancePy::Methods[] = {
	{NULL, NULL}		/* Sentinel */
};


//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject PersistancePy::Parents[] = { PARENTSPersistancePy };

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
PersistancePy::PersistancePy(Persistance *pcPersistance, PyTypeObject *T)
	: BaseClassPy(pcPersistance, T)
{

}

PyObject *PersistancePy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
	// never create such objects with the constructor
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
PersistancePy::~PersistancePy()						// Everything handled in parent
{
}

//--------------------------------------------------------------------------
// PersistancePy representation
//--------------------------------------------------------------------------
PyObject *PersistancePy::_repr(void)
{
	std::stringstream a;
	a << "Persistance";
//  for(std::map<std::string,int>::const_iterator It = _pcFeature->_PropertiesMap.begin();It!=_pcFeature->_PropertiesMap.end();It++)
//  {
//    a << It->first << "=" << _pcFeature->GetProperty(It->first.c_str()).GetAsString() << "; ";
//  }
	a << "" << std::endl;
	return Py_BuildValue("s", a.str().c_str());
}
//--------------------------------------------------------------------------
// PersistancePy Attributes
//--------------------------------------------------------------------------
PyObject *PersistancePy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try {
		if (streq(attr, "Content"))
			return Py::new_reference_to(getContent()); 
		if (streq(attr, "MemSize"))
			return Py::new_reference_to(getMemSize()); 
		// getter methode for special Attributes (e.g. dynamic ones)
		PyObject *r = getCustomAttributes(attr);
		if(r) return r;
	}
#ifndef DONT_CATCH_CXX_EXCEPTIONS 
	catch(Base::Exception &e) // catch the FreeCAD exeptions
	{
		std::string str;
		str += "FreeCAD exception thrown (";
		str += e.what();
		str += ")";
		e.ReportException();
		Py_Error(PyExc_Exception,str.c_str());
	}
	catch(std::exception &e) // catch other c++ exeptions
	{
		std::string str;
		str += "FC++ exception thrown (";
		str += e.what();
		str += ")";
		Base::Console().Error(str.c_str());
		Py_Error(PyExc_Exception,str.c_str());
	}
	catch(...)  // catch the rest!
	{
		Py_Error(PyExc_Exception,"Unknown C++ exception");
	}
#else  // DONT_CATCH_CXX_EXCEPTIONS  
	catch(Base::Exception &e) // catch the FreeCAD exeptions
	{
		std::string str;
		str += "FreeCAD exception thrown (";
		str += e.what();
		str += ")";
		e.ReportException();
		Py_Error(PyExc_Exception,str.c_str());
	}
#endif  // DONT_CATCH_CXX_EXCEPTIONS

	_getattr_up(BaseClassPy);
} 

int PersistancePy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	try {
		if (streq(attr, "Content")){
			setContent(Py::String(value));
			return 0;
		}
		if (streq(attr, "MemSize")){
			setMemSize(Py::Int(value));
			return 0;
		}
		// setter for  special Attributes (e.g. dynamic ones)
		int r = setCustomAttributes(attr, value);
		if(r==1) return 1;
	}
#ifndef DONT_CATCH_CXX_EXCEPTIONS 
	catch(Base::Exception &e) // catch the FreeCAD exeptions
	{
		std::string str;
		str += "FreeCAD exception thrown (";
		str += e.what();
		str += ")";
		e.ReportException();
		Py_Error(PyExc_Exception,str.c_str());
	}
	catch(std::exception &e) // catch other c++ exeptions
	{
		std::string str;
		str += "FC++ exception thrown (";
		str += e.what();
		str += ")";
		Base::Console().Error(str.c_str());
		Py_Error(PyExc_Exception,str.c_str());
	}
	catch(...)  // catch the rest!
	{
		Py_Error(PyExc_Exception,"Unknown C++ exception");
	}
#else  // DONT_CATCH_CXX_EXCEPTIONS  
	catch(Base::Exception &e) // catch the FreeCAD exeptions
	{
		std::string str;
		str += "FreeCAD exception thrown (";
		str += e.what();
		str += ")";
		e.ReportException();
		Py_Error(PyExc_Exception,str.c_str());
	}
#endif  // DONT_CATCH_CXX_EXCEPTIONS

	return BaseClassPy::_setattr(attr, value);
}

Persistance *PersistancePy::getPersistanceObject(void) const
{
	return dynamic_cast<Persistance *>(_pcBaseClass);
}

/* from here on the methodes you have to implement, but NOT in this module. implement in PersistancePyImp.cpp! This prototypes 
    are just for convinience!
		


Py::String PersistancePy::getContent(void)
{
	return Py::String();
}

void  setContent(Py::String arg)
{

}

Py::Int PersistancePy::getMemSize(void)
{
	return Py::Int();
}

void  setMemSize(Py::Int arg)
{

}

PyObject *PersistancePy::getCustomAttributes(const char* attr)
{
	return 0;
}

int PersistancePy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}

*/
	

