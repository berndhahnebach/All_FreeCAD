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
# include <sstream>
#endif

#include <Base/PyExportImp.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Vector3D.h>
#include <Base/Matrix.h>
using Base::Console;

#include "Document.h"
#include "Feature.h"
#include "FeaturePy.h"
#include "Property.h"

using namespace App;

using std::vector;
using std::string;
using std::map;

PROPERTY_SOURCE_ABSTRACT(App::AbstractFeature, App::DocumentObject)


//===========================================================================
// Feature
//===========================================================================

AbstractFeature::AbstractFeature(void)
:_nextFreeLabel(1),pcFeaturePy(0)
{
  _solidMaterial.ambientColor.set(0.2f,0.2f,0.2f);
  _solidMaterial.diffuseColor.set(0.8f,0.8f,0.8f);
  _solidMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _solidMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _solidMaterial.shininess = 0.2f;
  _solidMaterial.transparency = 0.0f;

  _lineMaterial.ambientColor.set(0.2f,0.2f,0.2f);
  _lineMaterial.diffuseColor.set(0.1f,0.1f,0.1f);
  _lineMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _lineMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _lineMaterial.shininess = 0.0f;
  _lineMaterial.transparency = 0.0f;
  _lineSize = 2.0f;

  _pointMaterial.ambientColor.set(0.9f,0.9f,0.9f);
  _pointMaterial.diffuseColor.set(0.8f,0.8f,0.8f);;
  _pointMaterial.specularColor.set(0.0f,0.0f,0.0f);
  _pointMaterial.emissiveColor.set(0.0f,0.0f,0.0f);
  _pointMaterial.shininess = 0.3f;
  _pointMaterial.transparency = 0.0f;
  _pointSize = 2.0;
  //_showMode = "Flat";

  touchTime.setToActual();
  touchViewTime.setToActual();

  ADD_PROPERTY(name,("Unnamed"));
  ADD_PROPERTY(visibility,(true));
  ADD_PROPERTY(showMode,("Flat"));
  ADD_PROPERTY(status,(Valid));
}

AbstractFeature::~AbstractFeature(void)
{
  if (pcFeaturePy)
    pcFeaturePy->DecRef();
}

Base::PyObjectBase *AbstractFeature::GetPyObject(void)
{
  if(!pcFeaturePy){
    // ref counter is set to 1
    pcFeaturePy = new FeaturePy(this);
  }
  pcFeaturePy->IncRef();
	return pcFeaturePy; 
}

void AbstractFeature::onChanged(Property* prop)
{
  // Ignore some properties
  if ( prop == &status )
    return;
  else if ( prop == &showMode )
    return;
  else if ( prop == &visibility )
    return;
  else if ( prop == &name )
    return;
  touchPropertyTime.setToActual();
}

void AbstractFeature::Touch(void)
{
  touchTime.setToActual();
}

void AbstractFeature::TouchView(void)
{
  touchViewTime.setToActual();
}

bool AbstractFeature::MustExecute(void)
{
  //Handle(TDF_Reference) RefAttr;
  //TDF_Label L;

  // If the object's label is modified:
  if (getStatus() != Valid && getStatus()!= Inactive) 
    return true;

  if(touchTime<touchPropertyTime || touchTime==touchPropertyTime) return true;

  return false;

}

void AbstractFeature::recompute(void)
{

  _pDoc->RecomputeFeature(this);

}

void AbstractFeature::removeModifications(void)
{
}

const char* AbstractFeature::getStatusString(void) const
{
  switch (/*_eStatus*/status.getValue())
  {
  case Valid:
    return "Valid";
  case New:
    return "New";
  case Inactive:
    return "Inactive";
  case Recompute:
    return "Recompute";
  case Error:
    return "Error";
  default:
    return "Unknown";
  }
}

void AbstractFeature::setError(const char* pMsg,...)
{
  // temp buffer
  char* format = (char*) malloc(strlen(pMsg)+4024);

  va_list namelessVars;
  va_start(namelessVars, pMsg);  // Get the "..." vars
  vsprintf(format, pMsg, namelessVars);
  va_end(namelessVars);

  /*_eStatus = Error;*/
  status.setValue(Error);
  _cErrorMessage = format;
}






