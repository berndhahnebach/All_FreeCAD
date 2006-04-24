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
#include "FeaturePython.h"
#include "FeaturePythonPy.h"


using namespace App;
using namespace std;


PROPERTY_SOURCE(App::FeaturePython, App::AbstractFeature)


FeaturePython::FeaturePython()
{
  ADD_PROPERTY(ExecuteCode ,(""));
}


int FeaturePython::execute(void)
{
  Base::Console().Message("FeaturePython::execute() with \"%s\"\n",ExecuteCode.getValue());

  return 0;
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
    objectProperies[name] = pcObject;

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
  pos = objectProperies.find(CleanName);

  if (pos == objectProperies.end())
    // if not, name is OK
    return CleanName;
  else
  {
    // find highes sufix
    int nSuff = 0;  
    for(pos = objectProperies.begin();pos != objectProperies.end();++pos)
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
