#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

import template,os
import generateBase.generateModel_Module
import generateBase.generateTools

class TemplateClassPyExport (template.ModelTemplate):
	def Generate(self):
		#self.ParentNamespace = "Base"
		#self.Namespace = "Base"
		print "TemplateClassPyExport",self.path + self.export.Name
		# Imp.cpp must not exist, neither in self.path nor in self.dirname
		if(not os.path.exists(self.path + self.export.Name + "Imp.cpp")):
			if(not os.path.exists(self.dirname + self.export.Name + "Imp.cpp")):
				file = open(self.path + self.export.Name + "Imp.cpp",'w')
				generateBase.generateTools.replace(self.TemplateImplement,locals(),file)
		file = open(self.path + self.export.Name + ".cpp",'w')
		generateBase.generateTools.replace(self.TemplateModule,locals(),file)
		file = open(self.path + self.export.Name + ".h",'w')
		generateBase.generateTools.replace(self.TemplateHeader,locals(),file)
		#file.write( generateBase.generateTools.replace(self.Template,locals()))

	TemplateHeader = """
// This file is generated out of ... Every change you make here get lost at the next full rebuild!
#ifndef @self.export.Namespace.upper()@_@self.export.Name.upper()@_H
#define @self.export.Namespace.upper()@_@self.export.Name.upper()@_H

#include <@self.export.Namespace@/@self.export.FatherInclude@>

namespace @self.export.Namespace@
{

class @self.export.Twin@;

//===========================================================================
// @self.export.Name@ - Python wrapper
//===========================================================================

/** The python export class for @self.export.Twin@
 */
class @self.export.Namespace@Export @self.export.Name@ : public @self.export.FatherNamespace@::@self.export.Father@
{
	/// always start with Py_Header
	Py_Header;

public:
	@self.export.Name@(@self.export.Twin@ *pc@self.export.Twin@Object, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);
	~@self.export.Name@();


	virtual PyObject *_repr(void);  				      // the representation
	PyObject *_getattr(char *attr);					      // __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function

	/** @name callbacks and implementer for the python object methods */
	//@{
+ for i in self.export.Methode:
	/// callback for the @i.Name@() method
	static PyObject * staticCallback_@i.Name@ (PyObject *self, PyObject *args, PyObject *kwd);
	/// implementer for the @i.Name@() method
	PyObject*  @i.Name@(PyObject *args);
-
	//@}

	
	/** @name callbacks and implementer for the python object methods */
	//@{
+ for i in self.export.Attribute:
	/// getter for the @i.Name@ attribute
	Py::@i.Parameter.Type@ get@i.Name@(void) const;
	/// setter for the @i.Name@ attribute
	void set@i.Name@(Py::@i.Parameter.Type@ arg);
-
+ if(self.export.CustomAttributes != None):
	/// getter methode for special attributes (e.g. dynamic ones)
	PyObject *getCustomAttributes(const char* attr) const;
	/// setter for special attributes (e.g. dynamic ones)
	int setCustomAttributes(const char* attr, PyObject *obj);
-
	//@}

	/// getter for the object handled by this class
	@self.export.Twin@ *get@self.export.Twin@Object(void) const;
};

#define PARENTS@self.export.Name@ &@self.export.Name@::Type,PARENTS@self.export.Father@

}  //namespace @self.export.Namespace@

#endif  // @self.export.Namespace.upper()@_@self.export.Name.upper()@_H

"""

	TemplateModule = """
#include "PreCompiled.h"

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/PyCXX/Objects.hxx>

#include "@self.export.Twin@.h"
#include "@self.export.Name@.h"
#define new DEBUG_CLIENTBLOCK

using Base::streq;
using namespace @self.export.Namespace@;

/// Type structure of @self.export.Name@
PyTypeObject @self.export.Name@::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,                                                /*ob_size*/
	"@self.export.Twin@",                             /*tp_name*/
	sizeof(@self.export.Name@),                       /*tp_basicsize*/
	0,                                                /*tp_itemsize*/
	/* methods */
	PyDestructor,                                     /*tp_dealloc*/
	0,                                                /*tp_print*/
	__getattr,                                        /*tp_getattr*/
	__setattr,                                        /*tp_setattr*/
	0,                                                /*tp_compare*/
	__repr,                                           /*tp_repr*/
	0,                                                /*tp_as_number*/
	0,                                                /*tp_as_sequence*/
	0,                                                /*tp_as_mapping*/
	0,                                                /*tp_hash*/
	0,                                                /*tp_call */
	0,                                                /*tp_str  */
	0,                                                /*tp_getattro*/
	0,                                                /*tp_setattro*/
	/* --- Functions to access object as input/output buffer ---------*/
	0,                                                /* tp_as_buffer */
	/* --- Flags to define presence of optional/expanded features */
	Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,        /*tp_flags */
	"About @self.export.Twin@",                       /*tp_doc */
	0,                                                /*tp_traverse */
	0,                                                /*tp_clear */
	0,                                                /*tp_richcompare */
	0,                                                /*tp_weaklistoffset */
	0,                                                /*tp_iter */
	0,                                                /*tp_iternext */
	@self.export.Namespace@::@self.export.Name@::Methods,                /*tp_methods */
	0,                                                /*tp_members */
	0,                                                /*tp_getset */
	&@self.export.FatherNamespace@::@self.export.Father@::Type,                        /*tp_base */
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

/// Methods structure of @self.export.Name@
PyMethodDef @self.export.Name@::Methods[] = {
+ for i in self.export.Methode:
	{"@i.Name@",(PyCFunction) staticCallback_@i.Name@,Py_NEWARGS},
-
	{NULL, NULL}		/* Sentinel */
};

+ for i in self.export.Methode:
// @i.Name@() callback and implementer
// PyObject*  @self.export.Name@::@i.Name@(PyObject *args){};
// has to be implemented in @self.export.Name@Imp.cpp
PyObject * @self.export.Name@::staticCallback_@i.Name@ (PyObject *self, PyObject *args, PyObject *kwd)
{
	try { // catches all exeptions coming up from c++ and generate a python exeption
	  return ((@self.export.Name@*)self)->@i.Name@(args);
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
-

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject @self.export.Name@::Parents[] = { PARENTS@self.export.Name@ };

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
@self.export.Name@::@self.export.Name@(@self.export.Twin@ *pc@self.export.Twin@, PyTypeObject *T)
	: @self.export.Father@(pc@self.export.Twin@, T)
{

}

PyObject *@self.export.Name@::PyMake(PyObject *ignored, PyObject *args)	// Python wrapper
{
	// never create such objects with the constructor
	return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
@self.export.Name@::~@self.export.Name@()						// Everything handled in parent
{
}

//--------------------------------------------------------------------------
// PersistancePy representation
//--------------------------------------------------------------------------
PyObject *@self.export.Name@::_repr(void)
{
	std::stringstream a;
	a << "@self.export.Twin@";
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
PyObject *@self.export.Name@::_getattr(char *attr)				// __getattr__ function: note only need to handle new state
{ 
	try {
+ for i in self.export.Attribute:
		if (streq(attr, "@i.Name@"))
			return Py::new_reference_to(get@i.Name@()); 
-
+ if(self.export.CustomAttributes != None):
		// getter methode for special Attributes (e.g. dynamic ones)
		PyObject *r = getCustomAttributes(attr);
		if(r) return r;
-
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

	_getattr_up(@self.export.Father@);
} 

int @self.export.Name@::_setattr(char *attr, PyObject *value) 	// __setattr__ function: note only need to handle new state
{ 
	try {
+ for i in self.export.Attribute:
		if (streq(attr, "@i.Name@")){
			set@i.Name@(Py::@i.Parameter.Type@(value));
			return 0;
		}
-
+ if(self.export.CustomAttributes != None):
		// setter for  special Attributes (e.g. dynamic ones)
		int r = setCustomAttributes(attr, value);
		if(r==1) return 1;
-
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

	return @self.export.Father@::_setattr(attr, value);
}

@self.export.Twin@ *@self.export.Name@::get@self.export.Twin@Object(void) const
{
	return dynamic_cast<@self.export.Twin@ *>(_pcBaseClass);
}

/* from here on the methods you have to implement, but NOT in this module. Implement in @self.export.Name@Imp.cpp! This prototypes 
    are just for convenience!
		

+ for i in self.export.Methode:
PyObject*  @self.export.Name@::@i.Name@(PyObject *args)
{

}
-
+ for i in self.export.Attribute:

Py::@i.Parameter.Type@ @self.export.Name@::get@i.Name@(void) const
{
	return Py::@i.Parameter.Type@();
}

void  set@i.Name@(Py::@i.Parameter.Type@ arg)
{

}
-
+ if(self.export.CustomAttributes != None):

PyObject *@self.export.Name@::getCustomAttributes(const char* attr) const
{
	return 0;
}

int @self.export.Name@::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}
-

*/
	
"""	

	# here the template for the user part of the implementation. This gets NOT overridden when already existing
	TemplateImplement = """

#include "PreCompiled.h"

#include "@self.export.Name@.h"
#include "@self.export.Twin@.h"

using namespace @self.export.Namespace@;

+ for i in self.export.Methode:
PyObject*  @self.export.Name@::@i.Name@(PyObject *args)
{
	return 0;
}
-
+ for i in self.export.Attribute:

Py::@i.Parameter.Type@ @self.export.Name@::get@i.Name@(void) const
{
	return Py::@i.Parameter.Type@();
}

void  @self.export.Name@::set@i.Name@(Py::@i.Parameter.Type@ arg)
{

}
-
+ if(self.export.CustomAttributes != None):

PyObject *@self.export.Name@::getCustomAttributes(const char* attr) const
{
	return 0;
}

int @self.export.Name@::setCustomAttributes(const char* attr, PyObject *obj)
{
	return 0; 
}
-

"""
