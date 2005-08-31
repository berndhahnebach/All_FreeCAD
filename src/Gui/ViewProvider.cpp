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
# include <Inventor/nodes/SoSeparator.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "../Base/Console.h"
#include "ViewProvider.h"
#include "Tree.h"

# include <Inventor/nodes/SoSwitch.h>


using namespace std;
using namespace Gui;


//**************************************************************************
// Construction/Destruction

       
ViewProvider::ViewProvider()
{
}

ViewProvider::~ViewProvider()
{

}

void ViewProvider::setEdit(void)
{
  if(getEditModeName())
    setMode(getEditModeName());
}

void ViewProvider::unsetEdit(void)
{
  setMode(0);
}




//**************************************************************************
//**************************************************************************
// Seperator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


       
ViewProviderTree::ViewProviderTree()
{

}


ViewProviderTree::~ViewProviderTree()
{

}


//**************************************************************************
// provider methodes

QListViewItem* ViewProviderTree::create()
{
  return 0;//new QListViewItem();
}


//**************************************************************************
//**************************************************************************
// Seperator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


       
ViewProviderInventor::ViewProviderInventor()
{
  pcRoot = new SoSeparator();
  pcRoot->ref();
  pcModeSwitch = new SoSwitch();
  pcModeSwitch->ref();
  
  pcRoot->addChild(pcModeSwitch);


}


ViewProviderInventor::~ViewProviderInventor()
{
  pcRoot->unref();

}


void ViewProviderInventor::setMode(const char* ModeName)
{
  // collect all modes (with subclasses)
  vector<string> modes = getModes();
  vector<string>::iterator p;

  p = find(modes.begin(),modes.end(),ModeName);

  if(p!=modes.end())
    setMode(p-modes.begin());
  else
    Base::Console().Warning("Unknown mode '%s' in ViewProviderInventor::setMode(), ignored\n", ModeName);

}

void ViewProviderInventor::setMode(int Mode)
{
  pcModeSwitch->whichChild = Mode;
}


int ViewProviderInventor::getMode(void)
{
 return pcModeSwitch->whichChild.getValue();
}


std::string ViewProviderInventor::getModeName(void)
{
  return getModes()[getMode()];
}


