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


#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <Base/Exception.h>
#include <Base/Reader.h>

#include "FeaturePy.h"
#include "Feature.h"
#include "Document.h"

#include "PropertyLinks.h"

using namespace App;
using namespace std;




//**************************************************************************
//**************************************************************************
// PropertyInteger
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyLink , App::Property);

//**************************************************************************
// Construction/Destruction


PropertyLink::PropertyLink()
:_pcLink(0)
{

}


PropertyLink::~PropertyLink()
{

}

//**************************************************************************
// Base class implementer


void PropertyLink::setValue(App::Feature * lValue)
{
	_pcLink=lValue;
  hasSetValue();
}

App::Feature * PropertyLink::getValue(void)
{
	return _pcLink;
}

PyObject *PropertyLink::getPyObject(void)
{
  if(_pcLink)
    return _pcLink->GetPyObject();
  else
    Py_Return;
}

void PropertyLink::setPyObject(PyObject *value)
{ 
  if( PyObject_TypeCheck(value, &(FeaturePy::Type)) )
  {
   	FeaturePy  *pcObject = (FeaturePy*)value;
    _pcLink = pcObject->getFeature();
    hasSetValue();
  }else
    throw Base::Exception("Not allowed type used (Feature expected)...");
}

void PropertyLink::Save (short indent,std::ostream &str)
{
  str << "<Link value=\"" <<  _pcLink->getName() <<"\"/>" ;
}

void PropertyLink::Restore(Base::Reader &reader)
{
  // read my Element
  reader.readElement("Link");
  // get the value of my Attribute
  string name = reader.getAttribute("value");

  // Property not in a Feature!
  assert(getContainer()->getTypeId().isDerivedFrom(App::Feature::getClassTypeId()) );

  _pcLink = reinterpret_cast<App::Feature*>(getContainer())->getDocument().getFeature(name.c_str());

}


