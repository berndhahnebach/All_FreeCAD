
#include "PreCompiled.h"

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/PyCXX/Objects.hxx>

#include "Document.h"
#include "DocumentPy.h"
#define new DEBUG_CLIENTBLOCK

using Base::streq;
using namespace App;

/// Type structure of DocumentPy
PyTypeObject DocumentPy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,						              /*ob_size*/
	"Document",				/*tp_name*/
	sizeof(DocumentPy), /*tp_basicsize*/
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
	"About Document",                       /*tp_doc */
	0,                                                /*tp_traverse */
	0,                                                /*tp_clear */
	0,                                                /*tp_richcompare */
	0,                                                /*tp_weaklistoffset */
	0,                                                /*tp_iter */
	0,                                                /*tp_iternext */
	App::DocumentPy::Methods,                /*tp_methods */
	0,                                                /*tp_members */
	0,                                                /*tp_getset */
	&App::PropertyContainerPy::Type,                        /*tp_base */
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

/// Methods structure of DocumentPy
PyMethodDef DocumentPy::Methods[] = {
	{"save",(PyCFunction) staticCallback_save,Py_NEWARGS},
	{"restore",(PyCFunction) staticCallback_restore,Py_NEWARGS},
	{"openTransaction",(PyCFunction) staticCallback_openTransaction,Py_NEWARGS},
	{"abortTransaction",(PyCFunction) staticCallback_abortTransaction,Py_NEWARGS},
	{"commitTransaction",(PyCFunction) staticCallback_commitTransaction,Py_NEWARGS},
	{"addObject",(PyCFunction) staticCallback_addObject,Py_NEWARGS},
	{"removeObject",(PyCFunction) staticCallback_removeObject,Py_NEWARGS},
	{"undo",(PyCFunction) staticCallback_undo,Py_NEWARGS},
	{"redo",(PyCFunction) staticCallback_redo,Py_NEWARGS},
	{"clearUndos",(PyCFunction) staticCallback_clearUndos,Py_NEWARGS},
	{"recompute",(PyCFunction) staticCallback_recompute,Py_NEWARGS},
	{"getObject",(PyCFunction) staticCallback_getObject,Py_NEWARGS},
	{NULL, NULL}		/* Sentinel */
};

// save() callback and implementer
// PyObject*  DocumentPy::save(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_save (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->save(args);
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
}
// restore() callback and implementer
// PyObject*  DocumentPy::restore(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_restore (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->restore(args);
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
}
// openTransaction() callback and implementer
// PyObject*  DocumentPy::openTransaction(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_openTransaction (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->openTransaction(args);
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
}
// abortTransaction() callback and implementer
// PyObject*  DocumentPy::abortTransaction(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_abortTransaction (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->abortTransaction(args);
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
}
// commitTransaction() callback and implementer
// PyObject*  DocumentPy::commitTransaction(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_commitTransaction (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->commitTransaction(args);
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
}
// addObject() callback and implementer
// PyObject*  DocumentPy::addObject(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_addObject (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->addObject(args);
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
}
// removeObject() callback and implementer
// PyObject*  DocumentPy::removeObject(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_removeObject (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->removeObject(args);
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
}
// undo() callback and implementer
// PyObject*  DocumentPy::undo(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_undo (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->undo(args);
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
}
// redo() callback and implementer
// PyObject*  DocumentPy::redo(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_redo (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->redo(args);
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
}
// clearUndos() callback and implementer
// PyObject*  DocumentPy::clearUndos(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_clearUndos (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->clearUndos(args);
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
}
// recompute() callback and implementer
// PyObject*  DocumentPy::recompute(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_recompute (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->recompute(args);
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
}
// getObject() callback and implementer
// PyObject*  DocumentPy::getObject(PyObject *args){};
// has to be implemented in DocumentPyImp.cpp
PyObject * DocumentPy::staticCallback_getObject (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((DocumentPy*)self)->getObject(args);
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
}

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject DocumentPy::Parents[] = { PARENTSDocumentPy };

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
DocumentPy::DocumentPy(Document *pcDocument, PyTypeObject *T)
	: PropertyContainerPy(pcDocument, T)
{

}

PyObject *DocumentPy::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
	// never create such objects with the constructor
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
DocumentPy::~DocumentPy()						// Everything handled in parent
{
}

//--------------------------------------------------------------------------
// PersistancePy representation
//--------------------------------------------------------------------------
PyObject *DocumentPy::_repr(void)
{
	std::stringstream a;
	a << "Document";
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
PyObject *DocumentPy::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try {
		if (streq(attr, "ActiveObject"))
			return Py::new_reference_to(getActiveObject()); 
		if (streq(attr, "Objects"))
			return Py::new_reference_to(getObjects()); 
		if (streq(attr, "UndoMode"))
			return Py::new_reference_to(getUndoMode()); 
		if (streq(attr, "UndoRedoMemSize"))
			return Py::new_reference_to(getUndoRedoMemSize()); 
		if (streq(attr, "UndoCount"))
			return Py::new_reference_to(getUndoCount()); 
		if (streq(attr, "RedoCount"))
			return Py::new_reference_to(getRedoCount()); 
		if (streq(attr, "UndoNames"))
			return Py::new_reference_to(getUndoNames()); 
		if (streq(attr, "RedoNames"))
			return Py::new_reference_to(getRedoNames()); 
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

	_getattr_up(PropertyContainerPy);
} 

int DocumentPy::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	try {
		if (streq(attr, "ActiveObject")){
			setActiveObject(Py::Object(value));
			return 0;
		}
		if (streq(attr, "Objects")){
			setObjects(Py::List(value));
			return 0;
		}
		if (streq(attr, "UndoMode")){
			setUndoMode(Py::Int(value));
			return 0;
		}
		if (streq(attr, "UndoRedoMemSize")){
			setUndoRedoMemSize(Py::Int(value));
			return 0;
		}
		if (streq(attr, "UndoCount")){
			setUndoCount(Py::Int(value));
			return 0;
		}
		if (streq(attr, "RedoCount")){
			setRedoCount(Py::Int(value));
			return 0;
		}
		if (streq(attr, "UndoNames")){
			setUndoNames(Py::List(value));
			return 0;
		}
		if (streq(attr, "RedoNames")){
			setRedoNames(Py::List(value));
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

	return PropertyContainerPy::_setattr(attr, value);
}

Document *DocumentPy::getDocumentObject(void) const
{
	return dynamic_cast<Document *>(_pcBaseClass);
}

/* from here on the methodes you have to implement, but NOT in this module. implement in DocumentPyImp.cpp! This prototypes 
    are just for convinience!
		

PyObject*  DocumentPy::save(PyObject *args)
{

}
PyObject*  DocumentPy::restore(PyObject *args)
{

}
PyObject*  DocumentPy::openTransaction(PyObject *args)
{

}
PyObject*  DocumentPy::abortTransaction(PyObject *args)
{

}
PyObject*  DocumentPy::commitTransaction(PyObject *args)
{

}
PyObject*  DocumentPy::addObject(PyObject *args)
{

}
PyObject*  DocumentPy::removeObject(PyObject *args)
{

}
PyObject*  DocumentPy::undo(PyObject *args)
{

}
PyObject*  DocumentPy::redo(PyObject *args)
{

}
PyObject*  DocumentPy::clearUndos(PyObject *args)
{

}
PyObject*  DocumentPy::recompute(PyObject *args)
{

}
PyObject*  DocumentPy::getObject(PyObject *args)
{

}

Py::Object DocumentPy::getActiveObject(void)
{
	return Py::Object();
}

void  setActiveObject(Py::Object arg)
{

}

Py::List DocumentPy::getObjects(void)
{
	return Py::List();
}

void  setObjects(Py::List arg)
{

}

Py::Int DocumentPy::getUndoMode(void)
{
	return Py::Int();
}

void  setUndoMode(Py::Int arg)
{

}

Py::Int DocumentPy::getUndoRedoMemSize(void)
{
	return Py::Int();
}

void  setUndoRedoMemSize(Py::Int arg)
{

}

Py::Int DocumentPy::getUndoCount(void)
{
	return Py::Int();
}

void  setUndoCount(Py::Int arg)
{

}

Py::Int DocumentPy::getRedoCount(void)
{
	return Py::Int();
}

void  setRedoCount(Py::Int arg)
{

}

Py::List DocumentPy::getUndoNames(void)
{
	return Py::List();
}

void  setUndoNames(Py::List arg)
{

}

Py::List DocumentPy::getRedoNames(void)
{
	return Py::List();
}

void  setRedoNames(Py::List arg)
{

}

PyObject *DocumentPy::getCustomAttributes(const char* attr)
{
	return 0;
}

int DocumentPy::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}

*/
	

