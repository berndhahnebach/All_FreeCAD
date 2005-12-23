/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef __PROPERTY_H__
#define __PROPERTY_H__

// Std. configurations

//#include "PyExport.h"

#include <string>
#include <vector>

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif

namespace App
{



/** Base class of all Properties
 *  This is the father of all properties. Properties are
 *  are object which are used in the document tree to parametrize 
 *  e.g. features and ist grafical output. They also used to 
 *  gain acces from the scripting facility.
 *  /par
 *  This abstract base class defines all methodes shared by all
 *  possible properties. Its also possible to define user properties
 *  and use them in the frame work....
 */
class AppExport Property
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	Property();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~Property();
	
	/** Sets the property throug a init string
	 */
	virtual void Set(const char* Str)=0;

	/** This method returns a string representation of the property
	 * This representation can be simple in case of strings or numbers
	 * or more comples in cas of e.g. color
	 */
	virtual const char* GetAsString(void)=0;

	/** Gets the type of the concrete Property
	 * Properties inherit from this class reports
	 * its type through this methode.
	 */

	virtual const char* GetType(void)=0;

	/** Returns the value of a sub property
	 * This method is mainly for scripting and allow setting  
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* SetSubProperty(const char* sSubPropName,const char* sNewValue)=0;

	/** sets the value of a sub property
	 * This method is mainly for scripting and allow geting 
	 * of additional information, like e.g. limits. 
	 */
	virtual const char* GetSubProperty(const char* sSubPropName)=0;

	/** gets all possible subproperty names
	 * return a line feed seperated list of all sub property
	 * names.
	 */
	virtual const char* GetSubPropertyNames(void)=0;

};


} // namespace App

#endif // __PROPERTIES_H__
