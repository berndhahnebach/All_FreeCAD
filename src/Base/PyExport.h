/** \file PyExport.h
 *  \brief the python object export base class
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    5.2001
 */

#ifndef __PYEXPORT_H__
#define __PYEXPORT_H__

// Std. configurations

// needed header
#include <python.h>
#include <iostream>

// forward
class FCInterpreter;




/** The FCPythonExport class, exports the class as a python type
 *  FCPythonExport is the base class for all C++ classes which
 *  need to get exported into the python name space. Note: only
 *  the class is exported! Not objects! If you want export objects
 *  you need to use FCPyObject.
 *  \par
 *  A good example for exporting a class is the FCApplication. To
 *  minimize the work two macros are providet for method definition
 *  (PYFUNCDEF_S) and implementation (PYFUNCIMP_S).
 *  @see FCPyObject
 *  @see FCApplication
 *  @see PYFUNCDEF_S
 *  @see PYFUNCIMP_S
 */
class BaseExport FCPythonExport
{

protected:
	virtual void PyRegister(){};
	virtual void PyInstance(){};

	friend FCInterpreter;
};

/** Python static class macro for definition
 * sets up a static function entry in a class inheriting 
 * from FCPythonExport. Its a pure confiniance macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * static PyObject* X (PyObject *self,PyObject *args,PyObject *kwd);
 * \endcode
 * @param SFUNC is the static method name (use what you want)
 * @see PYFUNCIMP_S
 * @see FCPythonExport
 */
#define PYFUNCDEF_S(SFUNC)   static PyObject* SFUNC (PyObject *self,PyObject *args,PyObject *kwd);


/** Python static class macro for implementation
 * used to set up a implementation for PYFUNCDEF_S definition.
 * Its a pure confiniance macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyObject* CLASS::SFUNC (PyObject *self,PyObject *args,PyObject *kwd)
 * \endcode
 * see PYFUNCDEF_S for details 
 * @param CLASS is the class in which the macro take place.
 * @param SFUNC is the object method get implemented
 * @see PYFUNCDEF_S
 * @see FCPythonExport
 */
#define PYFUNCIMP_S(CLASS,SFUNC) PyObject* CLASS::SFUNC (PyObject *self,PyObject *args,PyObject *kwd)




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

inline void Py_Fatal(char *M) {FCcout << M << FCendl; exit(-1);};

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
 *  The FCDocument is a good expample for a exported class.
 *  @see FCDocument
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
	 *  Note: if you reimplement _gettattr() in a inheriting class you
	 *  need to call the method of the base class! Otherwise even the 
	 *  methodes of the object will disapiear!
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
	static PyObject *sPy_isA(PyObject *self, PyObject *args, PyObject *)
		{return ((FCPyObject*)self)->Py_isA(args);};
};

/** Python dynamic class macro for definition
 * sets up a static/dynamic function entry in a class inheriting 
 * from FCPyObject. Its a pure confiniance macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyObject *PyGetGrp(PyObject *args);
 * static PyObject *sPyGetGrp(PyObject *self, PyObject *args, PyObject *kwd)
 *        {return ((FCPyParametrGrp*)self)->PyGetGrp(args);};
 * \endcode
 * first the method is defined which have the functionality then the
 * static wrapper is used to provide a callback for python. The call 
 * is simply mapped to the method.
 * @param CLASS is the class in which the macro take place.
 * @param DFUNC is the object method get defined and called
 * @param SFUNC is the static method name (use what you want)
 * @see PYFUNCIMP_D
 * @see FCPyObject
 */
#define PYFUNCDEF_D(CLASS,DFUNC,SFUNC)	PyObject * DFUNC (PyObject *args);	\
static PyObject * SFUNC (PyObject *self, PyObject *args, PyObject *kwd){return (( CLASS *)self)-> DFUNC (args);};

/** Python dynamic class macro for implementation
 * used to set up a impementation for PYFUNCDEF_D definition.
 * Its a pure confiniance macro. You can also do
 * it by hand if you want. It looks like that:
 * \code
 * PyObject *FCPyParametrGrp::PyGetGrp(PyObject *args)
 * \endcode
 * see PYFUNCDEF_D for details * @param CLASS is the class in which the macro take place.
 * @param DFUNC is the object method get defined and called
 * @see PYFUNCDEF_D
 * @see FCPyObject
 */
#define PYFUNCIMP_D(CLASS,DFUNC) PyObject* Y::X (PyObject *self,PyObject *args,PyObject *kwd)


#endif
