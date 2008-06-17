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

#include <string>
#include <map>
#include <typeinfo>


namespace Base
{

/** Handle class
 *  Implementation of the referenc counting pattern
 *  Only able to instatiate with a class inhereting
 *  FCHandled!
 */
template <class HandledType>
class Reference
{
public:
    //**************************************************************************
    // construction destruction

    /** pointer and default constructor
     *  the good way would be not using pointer
     *  instead using a overwriten new operator in the
     *  HandledType class! But is not easy to inforce!
     */
    Reference(HandledType* ToHandel=0L)
            :_pHandels(ToHandel) {
        if (_pHandels)
            _pHandels->AttachRef(this);
    }

    /// Copy constructor
    Reference(const Reference<HandledType>& ToHandel)
            :_pHandels(ToHandel._pHandels) {
        if (_pHandels)
            _pHandels->AttachRef(this);
    }

    /** destructor
     *  Release the referenc count which cause,
     *  if was the last one, the referenced object to
     *  destruct!
     */
    ~Reference() {
        if (_pHandels)
            _pHandels->DetachRef(this);
    }

    //**************************************************************************
    // operator implementation

    // assign operator from a pointer
    Reference <HandledType>& operator=(HandledType* other) {
        // check if we want to reassign the same object
        if (_pHandels == other)
            return *this;
        if (_pHandels)
            _pHandels->DetachRef(this);
        _pHandels = other;
        if (_pHandels)
            _pHandels->AttachRef(this);
        return *this;
    }

    // assign operator from a handle
    Reference <HandledType>& operator=(const Reference<HandledType>& other) {
        // check if we want to reassign the same object
        if (_pHandels == other._pHandels)
            return *this;
        if (_pHandels)
            _pHandels->DetachRef(this);
        _pHandels = other._pHandels;
        if (_pHandels)
            _pHandels->AttachRef(this);
        return *this;
    }

    /// derefrence operators
    HandledType& operator*() const {
        return *_pHandels;
    }

    /// derefrence operators
    HandledType* operator->() const {
        return _pHandels;
    }

    /** lower operator
     *  needed for sorting in maps and sets
     */
    bool operator<(const Reference<HandledType>& other) const {
        return _pHandels<other._pHandels;
    }

    /// equal operator
    bool operator==(const Reference<HandledType>& other) const {
        return _pHandels==other._pHandels;
    }


    //**************************************************************************
    // checking on the state

    /// Test if it handels something
    bool IsValid(void) const {
        return _pHandels!=0;
    }

    /// Test if it not handels something
    bool IsNull(void) const {
        return _pHandels==0;
    }

    /// Test if this is the last Referenc
    bool IsLastRef(void) const {
        if (_pHandels && _pHandels->GetReferenceCount()==1)
            return true;
        return false;
    }

    /// Get number of references on the object, including this one
    long GetReferenceCount(void) const {
        if (_pHandels)
            return _pHandels->GetReferenceCount();
        return 0;
    }

private:
    /// the pointer on the handled object
    HandledType *_pHandels;
};



class BaseExport Handled
{
public:
    Handled();
    virtual ~Handled();

    void  AttachRef(void* pHandle);
    void  DetachRef(void* pHandle);
    virtual void  OnLastRef() {}

    long GetReferenceCount(void) const {
        return _lRefCount;
    }
    const Handled& operator = (const Handled&);

private:
    long _lRefCount;
};

} // namespace Base

#endif // __HANDLE_H__
