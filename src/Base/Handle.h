/** \file FileTemplate.h
 *  \brief FileTemplate example header
 *  \author $Author$
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
#include "Export.h"

#include "PyExport.h"

#include <string>
#include <map>
#include <boost/any.hpp>


/** Handle Base class
 *  Implementation of the referenc counting pattern
 */
template <class HandledType>
class BaseExport FCHandle
{
public:
	FCHandle();
	~FCHandle();

	HandledType operator*(){return _pHandels;}

	HandledType operator->(){return _pHandels;}

	bool operator<(const FCHandle<HandledType> &other){return _pHandels<&other;}

	bool operator==(const FCHandle<HandledType> &other){return _pHandels==&other;}



private:

	HandledType *_pHandels;

};



class BaseExport FCHandled
{
public:
	FCHandled();
	~FCHandled();

	void  AttacheRef(void* pHandle);

	void  DettachRef(void* pHandle);

	virtual void  OnLastRef(){}

  	long GetReferenceCount(void) { return _lRefCount; };


private:

	long _lRefCount;

};






#endif // __HANDLE_H__