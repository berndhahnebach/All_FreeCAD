/** \file Handle.h
 *  \brief Handle header
 *  \author jriegel 
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.cpp
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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


#ifndef __HANDLE_H__
#define __HANDLE_H__

// Std. configurations

#include "PyExport.h"

#include <string>
#include <map>


/** Handle class
 *  Implementation of the referenc counting pattern
 *  Only able to instatiate with a class inhereting 
 *  FCHandled! 
 */
template <class HandledType>
class BaseExport FCHandle
{
public:
	//**************************************************************************
	// construction destruction

	/** pointer and default constructor
	 *  the good way would be not using pointer
	 *  instead using a overwriten new operator in the
	 *  HandledType class! But is not easy to inforce!
	 */
	FCHandle(HandledType *ToHandel=0L)
		:_pHandels(ToHandel)
	{
		if(_pHandels)
			ToHandel->AttacheRef(this);
	}

	/// Copy constructor 
	FCHandle(const FCHandle <HandledType> &ToHandel)
		:_pHandels(ToHandel._pHandels)
	{
		if(_pHandels)
			ToHandel->AttacheRef(this);
	}

	/** destructor
	 *  Release the referenc count which cause,
	 *  if was the last one, the referenced object to 
	 *  destruct! 
	 */
	~FCHandle()
	{
		if(_pHandels)
			_pHandels->DettachRef(this);
	}

	//**************************************************************************
	// operator implementation

	// assign operator from a pointer
	FCHandle <HandledType>  &operator=(const HandledType* other)
	{		
		if(_pHandels)
			_pHandels->DettachRef(this);
		_pHandels = other->_pHandels;
		if(_pHandels)
			ToHandel->AttacheRef(this);
		return *this;
	}

	// assign operator from a unknown pointer
	FCHandle <HandledType>  &operator=(const void* other)
	{		
		if(_pHandels)
			_pHandels->DettachRef(this);
		if( PointsOn(other) )
			_pHandels = other->_pHandels;
		else
			// invalid handle
			_pHandels = 0L;
		if(_pHandels)
			_pHandels->AttacheRef(this);
		return *this;
	}

	// assign operator from a handle
	FCHandle <HandledType>  &operator=(const FCHandle <HandledType> &other)
	{
		if(_pHandels)
			_pHandels->DettachRef(this);
		_pHandels = other._pHandels;
		if(_pHandels)
			_pHandels->AttacheRef(this);
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
	HandledType &operator*() const
	{
		return _pHandels;
	}

	/// derefrence operators
	HandledType *operator->() const
	{
		return _pHandels;
	}

	/** lower operator
	 *  needed for sorting in maps and sets
	 */
	bool operator<(const FCHandle<HandledType> &other) const
	{
		return _pHandels<&other;
	}

	/// equal operator
	bool operator==(const FCHandle<HandledType> &other) const
	{
		return _pHandels==&other;
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

	/** Type checking
	 *  test for a point if its the right type for handling
	 *  with this concrete handle object
	 */
	bool PointsOn(const void* other) const
	{
		if(!_pHandels)
			if(other)
				return false;
			else
				return true;
		return typeid(*other) == typeid(HandledType) ; 
	}                                     

private:
	/// the pointer on the handled object
	HandledType *_pHandels;

};



class BaseExport FCHandled
{
public:
	FCHandled();
	virtual ~FCHandled();

	void  AttacheRef(void* pHandle);

	void  DettachRef(void* pHandle);

	virtual void  OnLastRef(){}

  	long GetReferenceCount(void) const 
	{ 
		return _lRefCount; 
	}


private:

	long _lRefCount;

};






#endif // __HANDLE_H__
