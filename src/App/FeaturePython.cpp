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
#endif


#include <Base/Console.h>
#include <Base/Interpreter.h>
#include "FeaturePython.h"
#include "FeaturePythonPy.h"


using namespace App;
using namespace std;


PROPERTY_SOURCE(App::FeaturePython, App::AbstractFeature)


FeaturePython::FeaturePython()
{
}

int FeaturePython::execute(void)
{
  // Run the callback function of the Python object. There is no need to handle any exceptions here as the calling
  // instance does this for us.
  Base::Interpreter().runMethodVoid(pcFeaturePy, "execute");

  return 0;
}

void FeaturePython::getPropertyMap(std::map<std::string,Property*> &Map) const
{
  // get the properties of the base class first and insert the dynamic properties afterwards
  AbstractFeature::getPropertyMap(Map);
  for ( std::map<std::string,Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it )
    Map[it->first] = it->second;
}

Property *FeaturePython::getPropertyByName(const char* name) const
{
  std::map<std::string,Property*>::const_iterator it = objectProperties.find( name );
  if ( it != objectProperties.end() )
    return it->second;
  return AbstractFeature::getPropertyByName( name );
}

const char* FeaturePython::getName(const Property* prop) const
{
  for ( std::map<std::string,Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it ) {
    if (it->second == prop)
      return it->first.c_str();
  }
  return AbstractFeature::getName(prop);
}

void FeaturePython::addDynamicProperty(const char* type, const char* name)
{  
  Property* pcObject = (Property*) Base::Type::createInstanceByName(type,true);


  string ObjectName;

	if(pcObject)
	{
    assert(pcObject->getTypeId().isDerivedFrom(App::Property::getClassTypeId()));

    // get Unique name
    if(name)
      ObjectName = getUniquePropertyName(name);
    else
      ObjectName = getUniquePropertyName(type);

    pcObject->setContainer(this);
    objectProperties[name] = pcObject;

  }  
}

string FeaturePython::getUniquePropertyName(const char *Name) const
{
  // strip ilegal chars
  string CleanName;
  const char *It=Name;

  while(*It != '\0')
  {
    if(   (*It>=48 && *It<=57)   // Numbers
        ||(*It>=65 && *It<=90)   // Upercase letters
        ||(*It>=97 && *It<=122)  // Upercase letters
       )
    {
      CleanName += *It;
    }else{
      switch(*It)
      {
      case 'ä': CleanName += "ae"; break;
      case 'ü': CleanName += "ue"; break;
      case 'ö': CleanName += "oe"; break;
      case 'Ä': CleanName += "Ae"; break;
      case 'Ü': CleanName += "Ue"; break;
      case 'Ö': CleanName += "Oe"; break;
      default:
        CleanName += '_';
      }
    }
    It++;
  }

  std::map<std::string,Property*>::const_iterator pos;

  // name in use?
  pos = objectProperties.find(CleanName);

  if (pos == objectProperties.end())
    // if not, name is OK
    return CleanName;
  else
  {
    // find highes sufix
    int nSuff = 0;  
    for(pos = objectProperties.begin();pos != objectProperties.end();++pos)
    {
      const string &rclObjName = pos->first;
      if (rclObjName.substr(0, strlen(CleanName.c_str())) == CleanName)  // Prefix gleich
      {
        string clSuffix(rclObjName.substr(strlen(CleanName.c_str())));
        if (clSuffix.size() > 0){
          int nPos = clSuffix.find_first_not_of("0123456789");
          if(nPos==-1)
            nSuff = max<int>(nSuff, atol(clSuffix.c_str()));
        }
      }
    }
    char szName[200];
    sprintf(szName, "%s%d", CleanName.c_str(), nSuff + 1);
	
    return string(szName);
  }
	
}

Base::PyObjectBase *FeaturePython::GetPyObject(void)
{
  if(!pcFeaturePy){
    // ref counter is set to 1
    pcFeaturePy = new FeaturePythonPy(this);
  }
  pcFeaturePy->IncRef();
	return pcFeaturePy; 
}
