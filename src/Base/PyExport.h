/** \file PyExport.h
 *  \brief the python object export base class
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    5.2001
 */

#ifndef __PYEXPORT_H__
#define __PYEXPORT_H__

// Std. configurations
#include "Export.h"

class FCInterpreter;

/** The FCPythonExport class, exports the class as a python type
 *  FCPythonExport is the base class for all C++ classes which
 *  need to get exported into the python name space. This class is 
 *  very importand because nearly all imported classes in FreeCAD
 *  are visible in python for Macro recording and Automation purpose.
 *  \par
 *  FCConsole is a singelton! That means you can access the only
 *  instance of the class from every where in c++ by simply using:
 *  \code
 *  #include <Base/Console.h>
 *  GetConsole().Log("Stage: %d",i);
 *  \endcode
 *  \par
 *  FCConsole is abel to switch between several modes to, e.g. switch
 *  the logging on or off, or treat Warnings as Errors, and so on...
 *  @see FCConsoleObserver
 */
class BaseExport FCPythonExport
{

protected:
	virtual void PyRegister(){};
	virtual void PyInstance(){};

	friend FCInterpreter;
};

#define PYFUNCDEF(X)   static PyObject* X (PyObject *self,PyObject *args,PyObject *kwd)
#define PYFUNCIMP(Y,X) PyObject* Y::X (PyObject *self,PyObject *args,PyObject *kwd)


#include <python.h>
#include <iostream>
//#include <Python.h>
//#include <iostream.h>

/*------------------------------
 * Basic defines
------------------------------*/
//typedef const char * version;			// define "version"



inline int streq(const char *A, const char *B)	// define "streq"
{ return strcmp(A,B) == 0;};


inline void Assert(int expr, char *msg)		// C++ assert
{
  if (!expr) 
    {
      fprintf(stderr, "%s\n", msg);
      exit(-1);
    };
}


/*------------------------------
 * Python defines
------------------------------*/

/// some basic python macros
#define Py_NEWARGS 1			
#define Py_Return Py_INCREF(Py_None); return Py_None;	

#define Py_Error(E, M)   {PyErr_SetString(E, M); return NULL;}
#define Py_Try(F) {if (!(F)) return NULL;}
#define Py_Assert(A,E,M) {if (!(A)) {PyErr_SetString(E, M); return NULL;}}

inline void Py_Fatal(char *M) {stlport::cout << M << stlport::endl; exit(-1);};

/// This must be the first line of each PyC++ class
#define Py_Header												\
 public:														\
  static PyTypeObject   Type;									\
  static PyMethodDef    Methods[];								\
  static PyParentObject Parents[];								\
  virtual PyTypeObject *GetType(void) {return &Type;};			\
  virtual PyParentObject *GetParents(void) {return Parents;}

/** This defines the _getattr_up macro
 *  which allows attribute and method calls
 *  to be properly passed up the hierarchy.
 */
#define _getattr_up(Parent)										\
{																\
	PyObject *rvalue = Py_FindMethod(Methods, this, attr);		\
	if (rvalue == NULL)											\
	{															\
		PyErr_Clear();											\
		return Parent::_getattr(attr);							\
	}															\
	else														\
		return rvalue;											\
} 

/*------------------------------
 * FCPyObject
------------------------------*/
/// Define the PyParent Object
typedef PyTypeObject * PyParentObject;			


/** The FCPyObject class, exports the class as a python type
 *  FCPyObject is the base class for all C++ classes which
 *  need to get exported into the python name space. This class is 
 *  very importand because nearly all imported classes in FreeCAD
 *  are visible in python for Macro recording and Automation purpose.
 *  \par
 *  FCConsole is a singelton! That means you can access the only
 *  instance of the class from every where in c++ by simply using:
 *  \code
 *  #include <Base/Console.h>
 *  GetConsole().Log("Stage: %d",i);
 *  \endcode
 *  \par
 *  FCConsole is abel to switch between several modes to, e.g. switch
 *  the logging on or off, or treat Warnings as Errors, and so on...
 *  @see FCConsoleObserver
 */
class BaseExport FCPyObject : public PyObject 
{				
	/** Py_Header struct from python.h.
	 *  Every FCPyObject object is also a python object. So you can use
	 *  every Python C-Library function also on a FCPyObject object
	 */
	Py_Header;

 public:  
 	/** Constructor
 	 *  Sets the Type of the object (for inherintance) and decrease the
 	 *  the reference count of the PyObject.
 	 */
	FCPyObject(PyTypeObject *T) 				// constructor
		{this->ob_type = T; _Py_NewReference(this);};
	/// destructor
	virtual ~FCPyObject() {};
	/// Wrapper for the Python destructor
	static void PyDestructor(PyObject *P)				// python wrapper
		{  delete ((FCPyObject *) P);  };
	/// incref method wrapper (see python extending manual)
	void _INCREF(void) {Py_INCREF(this);};
	/// decref method wrapper (see python extending manual)	
	void _DECREF(void) {Py_DECREF(this);};

	/** GetAttribute implementation
	 *  This method implements the retriavel of object attributes.
	 *  If you whant to implement attributes in your class, reimplement
	 *  this method, the FCDocument is a good expample.
	 *  You have to call the method of the base class.
	 *  @see FCDocument
	 */
	virtual PyObject *_getattr(char *attr);	
	/// static wrapper for pythons _getattr()
	static  PyObject *__getattr(PyObject * PyObj, char *attr) 	// This should be the entry in Type. 
		{ return ((FCPyObject*) PyObj)->_getattr(attr); };
   
	/** SetAttribute implementation
	 *  This method implements the seting of object attributes.
	 *  If you whant to implement attributes in your class, reimplement
	 *  this method, the FCDocument is a good expample.
	 *  You have to call the method of the base class.
	 *  @see FCDocument
	 */
  	virtual int _setattr(char *attr, PyObject *value);		// _setattr method
	/// static wrapper for pythons _setattr()
	static  int __setattr(PyObject *PyObj, 			// This should be the entry in Type. 
			      char *attr, 
			      PyObject *value)
		{ return ((FCPyObject*) PyObj)->_setattr(attr, value);  };
	/// _repr method
	virtual PyObject *_repr(void);				
	/// python wrapper for the _repr() function
	static  PyObject *__repr(PyObject *PyObj)		
		{  return ((FCPyObject*) PyObj)->_repr();  };

	/// Type checking							
	bool isA(PyTypeObject *T);
	/// Type checking
	bool isA(const char *type_name);
	/// Python type checking
	PyObject *Py_isA(PyObject *args);
	/// static python wrapper
	static PyObject *sPy_isA(PyObject *self, PyObject *args, PyObject *kwd)
		{return ((FCPyObject*)self)->Py_isA(args);};
};



#endif
