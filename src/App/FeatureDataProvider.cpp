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
# include <TDF_ChildIterator.hxx>
# include <TDF_Tool.hxx>
# include <TCollection_AsciiString.hxx>
# include <TDF_ListIteratorOfAttributeList.hxx>
# include <TFunction_Logbook.hxx>
# include <TFunction_DriverTable.hxx>
# include <TFunction_Function.hxx>
# include <Standard_GUID.hxx>
# include <TNaming_Builder.hxx>
# include <TDataStd_Name.hxx>
# include <sstream>
#endif

#include <Base/PyExportImp.h>
#include <Base/Console.h>
#include <Base/Exception.h>
using Base::Console;

#include "Document.h"
#include "FeatureDataProvider.h"
#include "FeatureDataProviderPy.h"

using namespace App;


//===========================================================================
// FeatureDataProvider
//===========================================================================

FeatureDataProvider::FeatureDataProvider(void)
{

}
	

void FeatureDataProvider::AttachLabel(const TDF_Label &rcLabel,Document* dt)
{

}

Base::PyObjectBase *FeatureDataProvider::GetPyObject(void)
{
	return new FeatureDataProviderPy(this);
}


//===========================================================================
// FeatureDataProviderFactorySingleton - Factory for FeatureDataProviders
//===========================================================================



FeatureDataProviderFactorySingleton* FeatureDataProviderFactorySingleton::_pcSingleton = NULL;

FeatureDataProviderFactorySingleton& FeatureDataProviderFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FeatureDataProviderFactorySingleton;
  return *_pcSingleton;
}

void FeatureDataProviderFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

FeatureDataProvider* FeatureDataProviderFactorySingleton::Produce (const char* sName) const
{
	FeatureDataProvider* w = (FeatureDataProvider*)Factory::Produce(sName);

  // this FeatureDataProvider class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    Console().Warning("\"%s\" is not registered\n", sName);
#else
    Console().Log("\"%s\" is not registered\n", sName);
#endif
    return NULL;
  }

  return w;
}

