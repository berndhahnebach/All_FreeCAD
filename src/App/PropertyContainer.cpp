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
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "PropertyContainer.h"
#include "Property.h"

using namespace App;
using namespace Base;
using namespace std;
 
TYPESYSTEM_SOURCE(App::PropertyContainer,Base::Persistance);


//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
PropertyContainer::PropertyContainer()
{
  propertyData.parentPropertyData = 0;
}

PropertyContainer::~PropertyContainer()
{

}

Property *PropertyContainer::getPropertyByName(const char* name) const
{
  return getPropertyData().getPropertyByName(this,name);
}

void PropertyContainer::getPropertyMap(std::map<std::string,Property*> &Map) const
{
  getPropertyData().getPropertyMap(this,Map);
}

const char* PropertyContainer::getName(const Property* prop)const
{
  return getPropertyData().getName(this,prop);
}


const PropertyData * PropertyContainer::getPropertyDataPtr(void){return &propertyData;} 
const PropertyData & PropertyContainer::getPropertyData(void) const{return propertyData;} 

PropertyData PropertyContainer::propertyData;

void PropertyContainer::Save (Writer &writer) const 
{
  std::map<std::string,Property*> Map;
  getPropertyMap(Map);

  writer << writer.ind() << "<Properties Count=\"" << Map.size() << "\">" << endl;
  std::map<std::string,Property*>::iterator it;
  for(it = Map.begin(); it != Map.end(); ++it)
  {
    writer << writer.ind() << "<Property name=\"" << it->first << "\" type=\"" << it->second->getTypeId().getName() << "\">" ;    
    writer.incInd();
    it->second->Save(writer);
    writer.decInd();
    writer << "</Property>" << endl;    
  }
  writer << writer.ind() << "</Properties>" << endl;
}

void PropertyContainer::Restore(Base::XMLReader &reader)
{
  reader.readElement("Properties");
  int Cnt = reader.getAttributeAsInteger("Count");

  for(int i=0 ;i<Cnt ;i++)
  {
    reader.readElement("Property");
    string PropName = reader.getAttribute("name");
    Property* prop = getPropertyByName(PropName.c_str());
    if(prop)
      prop->Restore(reader);

    reader.readEndElement("Property");
  }
  reader.readEndElement("Properties");
}




void PropertyData::addProperty(PropertyContainer *container,const char* PropName, Property *Prop)
{
  std::map<std::string,int>::const_iterator pos = propertyData.find(PropName);

  if(pos == propertyData.end())
  {
    propertyData[PropName] = (int) ((char*)Prop - (char*)container);
  }
}

const char* PropertyData::getName(const PropertyContainer *container,const Property* prop)const
{
  const int diff = (int) ((char*)prop - (char*)container);

  for(std::map<std::string,int>::const_iterator pos = propertyData.begin();pos != propertyData.end();++pos)
    if(pos->second == diff)
      return pos->first.c_str();

  if(parentPropertyData)
    return parentPropertyData->getName(container,prop);

  return 0;
}


Property *PropertyData::getPropertyByName(const PropertyContainer *container,const char* name) const 
{
  std::map<std::string,int>::const_iterator pos = propertyData.find(name);

  if(pos != propertyData.end())
  {
    // calculate propterty by offset
    return (Property *) (pos->second + (char *)container);
  }else{
    if(parentPropertyData)
      return parentPropertyData->getPropertyByName(container,name);
    else
      return 0;
  }
}

void PropertyData::getPropertyMap(const PropertyContainer *container,std::map<std::string,Property*> &Map) const
{
  std::map<std::string,int>::const_iterator pos;

  for(pos = propertyData.begin();pos != propertyData.end();++pos)
  {
    Map[pos->first] = (Property *) (pos->second + (char *)container);
  }
  if(parentPropertyData)
    parentPropertyData->getPropertyMap(container,Map);

}



/** \defgroup The property framework
\section Introduction
The property framework introduce the ability to acces attributs (member variables) of a class by name without
knowing the type of class. Its like the reflection mechanism of Jave or C#.
This ability is introduced by the App::PropertyContainer class and can be used by all derived classes.

This makes it possible in the first place to make a automatic mapping to python (e.g. in App::FeaturePy) and 
bstract editing Properties in Gui::PropertyEditor.

\section Examples

Here some little examples how to use it:

\code
// search in PropertyList
Property *prop = _pcFeature->getPropertyByName(attr);
if(prop)
{
  return prop->getPyObject();
}
\endcode

or:

\code
void PropertyContainer::Restore(Base::Reader &reader)
{
  reader.readElement("Properties");
  int Cnt = reader.getAttributeAsInteger("Count");

  for(int i=0 ;i<Cnt ;i++)
  {
    reader.readElement("Property");
    string PropName = reader.getAttribute("name");
    Property* prop = getPropertyByName(PropName.c_str());
    if(prop)
      prop->Restore(reader);

    reader.readEndElement("Property");
  }
  reader.readEndElement("Properties");
}
\endcode

*/

