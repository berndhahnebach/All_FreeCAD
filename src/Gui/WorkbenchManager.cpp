/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "WorkbenchManager.h"
#include "Workbench.h"
#include "WorkbenchFactory.h"

using namespace Gui;

WorkbenchManager* WorkbenchManager::_instance = 0;

WorkbenchManager* WorkbenchManager::instance()
{
  if ( _instance == 0 )
    _instance = new WorkbenchManager;
  return _instance;
}

WorkbenchManager::WorkbenchManager()
{
}

WorkbenchManager::~WorkbenchManager()
{
}

Workbench* WorkbenchManager::getWorkbench ( const QString& name )
{
  Workbench* wb=0;

  QMap<QString, Workbench*>::Iterator it = _workbenches.find( name );
  if ( it != _workbenches.end() )
  {
    // returns the already created object
    wb = it.data();
  }
  else
  {
    // try to create an instance now
    wb = WorkbenchFactory().createWorkbench( name.latin1() );
    if ( wb )
    {
      _workbenches[ name ] = wb;
    }
  }
  
  return wb;
}

bool WorkbenchManager::activate( const QString& name )
{
  Workbench* wb = getWorkbench( name );
  if ( wb ) 
    return wb->activate();
  return false;
}
