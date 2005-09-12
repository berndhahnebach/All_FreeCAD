/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "WorkbenchFactory.h"
#include "Workbench.h"

using namespace Gui;

Gui::WorkbenchFactoryInst* Gui::WorkbenchFactoryInst::_pcSingleton = 0;

WorkbenchFactoryInst& WorkbenchFactoryInst::instance()
{
  if (_pcSingleton == 0L)
    _pcSingleton = new WorkbenchFactoryInst;
  return *_pcSingleton;
}

void WorkbenchFactoryInst::destruct ()
{
  if ( _pcSingleton != 0 )
    delete _pcSingleton;
}

Workbench* WorkbenchFactoryInst::createWorkbench ( const char* sName ) const
{
  Workbench* obj = (Workbench*)Produce( sName );
  Workbench* w = dynamic_cast<Workbench*>(obj);
  if ( !w )
  {
    delete obj; // delete the unknown object as no workbench object
    return 0;
  }

  w->setName( sName );
  return w;
}

QStringList WorkbenchFactoryInst::workbenches() const
{
  QStringList wb;
  for ( std::map<const std::string, Base::AbstractProducer*>::const_iterator it = _mpcProducers.begin(); it != _mpcProducers.end(); ++it )
    wb << it->first.c_str();
  return wb;
}
