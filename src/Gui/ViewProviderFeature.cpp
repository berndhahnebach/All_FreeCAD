/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qlistview.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "../Base/Console.h"
#include "ViewProviderFeature.h"
#include "Tree.h"




using namespace Gui;


      
ViewProviderInventorFeature::ViewProviderInventorFeature()
{

}


ViewProviderInventorFeature::~ViewProviderInventorFeature()
{

}



//===========================================================================
// FeatureFactorySingleton - Factory for Features
//===========================================================================



ViewProviderInventorFeatureFactorySingleton* ViewProviderInventorFeatureFactorySingleton::_pcSingleton = NULL;

ViewProviderInventorFeatureFactorySingleton& ViewProviderInventorFeatureFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new ViewProviderInventorFeatureFactorySingleton;
  return *_pcSingleton;
}

void ViewProviderInventorFeatureFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

ViewProviderInventorFeature* ViewProviderInventorFeatureFactorySingleton::Produce (const char* sName) const
{
	ViewProviderInventorFeature* w = (ViewProviderInventorFeature*)Factory::Produce(sName);

  // this Feature class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    Base::Console().Warning("\"%s\" ViewProvider is not registered\n", sName);
#else
    Base::Console().Log("Warn: %s ViewProvider is not registered\n", sName);
#endif
    return NULL;
  }

  return w;
}

