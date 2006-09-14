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
:pcFeaturePy(0), _execute(false)
{


  ADD_PROPERTY(status,(New));
}

AbstractFeature::~AbstractFeature(void)
{
  if (pcFeaturePy)
  {
    pcFeaturePy->setInvalid();
    pcFeaturePy->DecRef();
  }
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

void AbstractFeature::onChanged(const Property* prop)
{
  // call father for Undo/Redo...
  DocumentObject::onChanged(prop);

  // Ignore some properties
  if ( prop == &status )
    return;
  else if ( prop == &name )
    return;
  touchPropertyTime.setToActual();
  setModified(true);
}

void AbstractFeature::setModified(bool b)
{
  _execute = b;
}

bool AbstractFeature::isModified() const
{
  return _execute;
}

bool AbstractFeature::mustExecute(void)
{
  // If the object's label is modified:
  if (getStatus() != Valid && getStatus()!= Inactive) 
    return true;

  //FIXME: The setModified() method is a workaround for the problems with the time stamp.
  //E.g. when a property has changed then the feature itself has changed. If the recomputation takes place
  //immediately and is finished very fast then it could happen that the OS returns the same time stamp for both
  //events. And the other way round a property has changed immediately after a recomputation of the feature.
  //In both cases the time stamp doesn't work as expected. Using a boolean instead solves the problem.
  return _execute;
//  if ( touchTime <= touchPropertyTime ) 
//    return true;
//
//  return false;

}

void AbstractFeature::recompute(void)
{
  _pDoc->recomputeFeature(this);
}

const char* AbstractFeature::getStatusString(void) const
{
  switch (status.getValue())
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
  unsigned int format_len = strlen(pMsg)+4024;
  char* format = (char*) malloc(format_len);

  va_list namelessVars;
  va_start(namelessVars, pMsg);  // Get the "..." vars
  vsnprintf(format, format_len, pMsg, namelessVars);
  va_end(namelessVars);

  /*_eStatus = Error;*/
  status.setValue(Error);
  _cErrorMessage = format;
}






