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

#include <Base/Persistance.h>



#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif

namespace App
{

class PropertyContainer;

/** Base class of all Properties
 *  This is the father of all properties. Properties are
 *  are object which are used in the document tree to parametrize 
 *  e.g. features and ist grafical output. They also used to 
 *  gain acces from the scripting facility.
 *  /par
 *  This abstract base class defines all methods shared by all
 *  possible properties. Its also possible to define user properties
 *  and use them in the frame work....
 */
class AppExport Property: public Base::Persistance
{
  TYPESYSTEM_HEADER();

public:

       
	Property();
	virtual ~Property();

  /// get the name of this property in the belonging Container
  const char* getName(void) const;

  /// is called by the framework to set the Father (Container)
  void setContainer(PropertyContainer *Father);

  /// get a pointer to the PropertyContainer derived class the property belonging to
  PropertyContainer *getContainer(void) const {return father;}
  
  friend class PropertyContainer;

protected:
  /// is called by all setValue() methodes after the value was changed
  void hasSetValue(void);
  /// is called by all setValue() methodes befor the value is changed
  void aboutToSetValue(void);

private:

  PropertyContainer *father;

};


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
class AppExport PropertyLists: public Property
{
  TYPESYSTEM_HEADER();

public:

  virtual void setSize(int newSize)=0;   
  virtual int getSize(void) const =0;   

};

} // namespace App

#endif // __PROPERTIES_H__
