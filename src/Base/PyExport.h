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

// (re-)defined in pyconfig.h
#if defined (_POSIX_C_SOURCE)
#	undef  _POSIX_C_SOURCE
#endif

#undef slots
#include <Python.h>
#define slots
//#include <iostream>
//#include <typeinfo>

// forward
//class FCInterpreter;
typedef struct _object PyObject;


namespace Base
{
class PyObjectBase;

/** The PyHandler class
 *  This class is the base class of all FreeCAD classes
 *  which exports into the python space. This class handles the 
 *  creation referencing of the python export object.
 *
 *  @remark GetPyObject() returns the associated Python object to any C++ subclasses. As we cannot 
 *  determine for sure if we can increment the returned Python object from outside of GetPyObject()
 *  we have specified that GetPyObject() does already the increment of the reference counter
 *  if needed. 
 *
 *  E.g. if GetPyObject() always returns a new Python object then no increment is necessary,
 *  because at construction time the reference counter is already set to 1. If the Python
 *  interpreter stores this object pointer into a local variable and destroys this variable
 *  then the reference counter gets decremented (to 0) and the object gets destroyed automatically.
 *  In case we didn't make this specification and increment the Python object from outside once again then
 *  the reference counter would be set to 2 and there would be no chance to destroy the object again.
 *
 *  The other case is that we have a member variable in our C++ class that holds the Python object
 *  then we either can create this Python in the constructor or create it the first  time when GetPyObject()
 *  gets called. In the destructor then we must decrement the Python object to avoid a memory leak while
 *  GetPyObject() then increments the Python object everytime it gets called.
 *
 *  @remark One big consequence of this specification is that the programmer must know whether the Python interpreter
 *  gets the Python object or not. If the interpreter gets the object then it decrements the counter later on when
 *  the internal variable is freed. In case the interpreter doesn't get this object then the programmer must do the
 *  decrement on his own.
 *
 *  @note To not to undermine this specification the programmer must make sure to get the Python object always via 
 *  GetPyObject().
 *
 *  @see PyHandle
 *  @
 */
class BaseExport PyHandler
{
public:
	void IncRef(void);
	void DecRef(void);
	virtual PyObjectBase *GetPyObject(void)=0;

};


/** Python Object handle class
 *  Using pointers on classes derived from PyObjectBase would
 *  be potentionaly dangerous because you would have to take
 *  care of the referenc counting of python by your self. There
 *  fore this class was designd. It takes care of references and
 *  as long as a object of this class exists the handled class get
 *  not destructed. That means a PyObjectBase derived object you can
 *  only destruct by destructing all FCPyHandle and all python
 *  references on it!
 *  @see PyObjectBase,FCDocument
 */
template <class HandledType>
class PyHandle
{
public:
	//**************************************************************************
	// construction destruction

	/** pointer and default constructor
	 *  the good way would be not using pointer
	 *  instead using a overwriten new operator in the
	 *  HandledType class! But is not easy to inforce!
	 */
	PyHandle(HandledType *ToHandel=0L)
		:_pHandels(ToHandel)
	{
		if(_pHandels)
			_pHandels->IncRef();
	}

	/// Copy constructor
	PyHandle(const PyHandle <HandledType> &ToHandel)
		:_pHandels(ToHandel._pHandels)
	{
		if(_pHandels)
			_pHandels->IncRef();
	}

	/** destructor
	 *  Release the referenc count which cause,
	 *  if was the last one, the referenced object to
	 *  destruct!
	 */
	~PyHandle()
	{
		if(_pHandels)
			_pHandels->DecRef();
	}

	//**************************************************************************
	// operator implementation

	// assign operator from a pointer
	PyHandle <HandledType>  &operator=(/*const*/ HandledType* other)
	{
		if(_pHandels)
			_pHandels->DecRef();
    // FIXME: Should be without "->_pHandels", shouldn't it? (Werner)
		_pHandels = other;//_pHandels = other->_pHandels;
		if(_pHandels)
			_pHandels->IncRef();
		return *this;
	}
/*
  // FIXME: Does this method make sense? I don't think so. (Werner)
	// assign operator from a unknown pointer
	PyHandle <HandledType>  &operator=(void* other)
	{
		if(_pHandels)
			_pHandels->DecRef();
		if( PointsOn(other) )
    // FIXME: Should be without "->_pHandels", shouldn't it? (Werner)
		_pHandels = reinterpret_cast<HandledType*>(other);//_pHandels = other->_pHandels;
		else
			// invalid handle
			_pHandels = 0L;
		if(_pHandels)
			_pHandels->IncRef();
		return *this;
	}
*/
	// assign operator from a handle
	PyHandle <HandledType>  &operator=(const PyHandle <HandledType> &other)
	{
		if(_pHandels)
			_pHandels->DecRef();
		_pHandels = other._pHandels;
		if(_pHandels)
			_pHandels->IncRef();
		return *this;
	}

	/// derefrence operators
	HandledType &operator*()
	{
		return *_pHandels;
	}

	/// derefrence operators
	HandledType *operator->()
	{
		return _pHandels;
	}

	/// derefrence operators
	const HandledType &operator*() const
	{
		return _pHandels;
	}

	/// derefrence operators
	const HandledType *operator->() const
	{
		return _pHandels;
	}

	/** lower operator
	 *  needed for sorting in maps and sets
	 */
	bool operator<(const PyHandle<HandledType> &other) const
	{
		//return _pHandels<&other;
    // FIXME: Shouldn't we compare both pointers?. (Werner)
		return _pHandels<other._pHandels;
	}

	/// equal operator
	bool operator==(const PyHandle<HandledType> &other) const
	{
		//return _pHandels==&other;
    // FIXME: Shouldn't we compare both pointers?. (Werner)
		return _pHandels==other._pHandels;
	}

	/// returns the type as PyObject
	PyObject* GetPyObject(void)
	{
		// return (PyObject*) _pHandels;
    // FIXME: Shouldn't we return the pointer's object?. (Werner)
		return _pHandels->GetPyObject();
	}
	//**************************************************************************
	// checking on the state

	/// Test if it handels something
	bool IsValid(void) const
	{
		return _pHandels!=0;
	}

	/// Test if it not handels something
	bool IsNull(void) const
	{
		return _pHandels==0;
	}
/*
	/// Test if this is the last Referenc
	bool IsLastRef(void) const
	{
		if(_pHandels && _pHadels->GetReferenceCount()==1)
			return true;
		return false;
	}

	/// Get number of references on the object, including this one
	long GetReferenceCount(void) const
	{
		if(_pHandels)
			return _pHadels->GetReferenceCount();
		return 0;
	}
*/
	/** Type checking
	 *  test for a point if its the right type for handling
	 *  with this concrete handle object
	 */
  // FIXME: Does this method make sense? I don't think so. (Werner)
  /*
	bool PointsOn(const void* other) const
	{
		if(!_pHandels)
			if(other)
				return false;
			else
				return true;
		return typeid(*other) == typeid(HandledType) ;
	}*/

private:
	/// the pointer on the handled object
	HandledType *_pHandels;

};


} // namespace Base

#endif
