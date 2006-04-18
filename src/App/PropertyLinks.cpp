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
#include <Base/Writer.h>

#include "FeaturePy.h"
#include "Feature.h"
#include "DocumentObject.h"
#include "DocumentObjectPy.h"
#include "Document.h"

#include "PropertyLinks.h"

using namespace App;
using namespace Base;
using namespace std;




//**************************************************************************
//**************************************************************************
// PropertyLink
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


void PropertyLink::setValue(App::DocumentObject * lValue)
{
	_pcLink=lValue;
  hasSetValue();
}

App::DocumentObject * PropertyLink::getValue(void) const
{
	return _pcLink;
}

App::DocumentObject * PropertyLink::getValue(Base::Type t) const
{
  if(_pcLink->getTypeId().isDerivedFrom(t))
	  return _pcLink;
  else
    return 0;
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

void PropertyLink::Save (Writer &writer) const
{
  writer << "<Link value=\"" <<  _pcLink->name.getValue() <<"\"/>" ;
}

void PropertyLink::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("Link");
  // get the value of my Attribute
  string name = reader.getAttribute("value");

  // Property not in a Feature!
  assert(getContainer()->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );

  DocumentObject *pcObject = dynamic_cast<DocumentObject*>(getContainer())->getDocument().getObject(name.c_str());

  assert(pcObject->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );

  _pcLink = dynamic_cast<DocumentObject*>(pcObject);

}


Property *PropertyLink::Copy(void) const
{
  PropertyLink *p= new PropertyLink();
  p->_pcLink = _pcLink;
  return p;
}

void PropertyLink::Paste(const Property &from)
{
  _pcLink = dynamic_cast<const PropertyLink&>(from)._pcLink;
}



TYPESYSTEM_SOURCE(App::PropertyLinkList , App::PropertyLists);

//**************************************************************************
// Construction/Destruction


PropertyLinkList::PropertyLinkList()
{

}

PropertyLinkList::~PropertyLinkList()
{

}

void PropertyLinkList::setValue(DocumentObject* lValue)
{
  aboutToSetValue();
  _lValueList.resize(1);
	_lValueList[0]=lValue;
  hasSetValue();
}

PyObject *PropertyLinkList::getPyObject(void)
{
  PyObject* list = PyList_New(	getSize() );

  for(int i = 0;i<getSize(); i++)
     PyList_SetItem( list, i, _lValueList[i]->GetPyObject());

  return list;
}

void PropertyLinkList::setPyObject(PyObject *value)
{
  if ( PyList_Check( value) )
  {
    aboutToSetValue();

    int nSize = PyList_Size( value );
    _lValueList.resize(nSize);

    for (int i=0; i<nSize;++i)
    {
      PyObject* item = PyList_GetItem(value, i);
      if ( PyObject_TypeCheck(item, &(FeaturePy::Type)) )
      {
   	    FeaturePy  *pcObject = (FeaturePy*)item;
        _lValueList[i] = pcObject->getFeature();
        hasSetValue();
      }
      else
      {
        _lValueList.resize(1);
        _lValueList[0] = 0;
        throw Base::Exception("Not allowed type in list (float expected)...");
      }
    }

    hasSetValue();
  }
  else if( PyObject_TypeCheck(value, &(FeaturePy::Type)) )
  {
    aboutToSetValue();
   	FeaturePy  *pcObject = (FeaturePy*)value;
    setValue( pcObject->getFeature() );
  }
  else
  {
    throw Base::Exception("Not allowed type used (Feature expected)...");
  }
}

void PropertyLinkList::Save (Writer &writer) const
{
  writer << "<LinkList count=\"" <<  getSize() <<"\">" << endl;
  for(int i = 0;i<getSize(); i++)
    writer << "<Link value=\"" <<  _lValueList[i]->name.getValue() <<"\"/>" << endl; ;
  writer << "</LinkList>" << endl ;
}

void PropertyLinkList::Restore(Base::XMLReader &reader)
{
  // read my Element
  reader.readElement("LinkList");
  // get the value of my Attribute
  int count = reader.getAttributeAsInteger("count");

  setSize(count);

  assert(getContainer()->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );

  for(int i = 0;i<count; i++)
  {
    reader.readElement("Link");
    std::string name = reader.getAttribute("value");

    // Property not in a Feature!
    //assert(getContainer()->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );
    //_lValueList[i] = reinterpret_cast<App::DocumentObject*>(getContainer())->getDocument().getObject(name.c_str());

    // Property not in a Feature!
    DocumentObject *pcObject = dynamic_cast<DocumentObject*>(getContainer())->getDocument().getObject(name.c_str());
    assert(pcObject->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()) );
    _lValueList[i] = dynamic_cast<DocumentObject*>(pcObject);

  }

  reader.readEndElement("LinkList");
}


Property *PropertyLinkList::Copy(void) const
{
  PropertyLinkList *p= new PropertyLinkList();
  p->_lValueList = _lValueList;
  return p;
}

void PropertyLinkList::Paste(const Property &from)
{
  _lValueList = dynamic_cast<const PropertyLinkList&>(from)._lValueList;
}
