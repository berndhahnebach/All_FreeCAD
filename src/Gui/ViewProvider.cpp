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




using namespace Gui;


//**************************************************************************
// Construction/Destruction

       
ViewProvider::ViewProvider()
{
}

ViewProvider::~ViewProvider()
{

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
}


ViewProviderInventor::~ViewProviderInventor()
{
  pcRoot->unref();

}



