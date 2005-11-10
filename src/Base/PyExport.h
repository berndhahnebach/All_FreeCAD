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
#undef slots

// (re-)defined in pyconfig.h
#if defined (_POSIX_C_SOURCE)
#	undef  _POSIX_C_SOURCE
#endif

#include <Python.h>
#define slots
//#include <iostream>

#ifndef _PreComp_
#	include<typeinfo>
#endif

// forward
//class FCInterpreter;
typedef struct _object PyObject;


namespace Base
{
class PyObjectBase;

/** The PyHandler class
 *  This class is the base class of all FreeCAD classes
 *  which export into the python space. This class handles the 
 *  creation referencing of the python export object.
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
