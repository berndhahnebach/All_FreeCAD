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
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Material.h>
#include <App/Feature.h>
#include "Selection.h"
#include "SoFCSelection.h"
#include "ViewProviderExtern.h"
#include "Tree.h"

using std::vector;
using std::string;

#define new DEBUG_CLIENTBLOCK
using namespace Gui;

PROPERTY_SOURCE(Gui::ViewProviderExtern, Gui::ViewProvider)

      
ViewProviderExtern::ViewProviderExtern()
{

}


ViewProviderExtern::~ViewProviderExtern()
{

}


void ViewProviderExtern::setModeByString(const char* name, const char* ivFragment)
{
  SoInput in;
  in.setBuffer((void*)ivFragment,strlen(ivFragment));

  setModeBySoInput(name,in);

}


void ViewProviderExtern::setModeByFile(const char* name, const char* ivFileName)
{
  SoInput in;
  if (in.openFile(ivFileName)) 
  {
     setModeBySoInput(name,in);
  }
}

void ViewProviderExtern::setModeBySoInput(const char* name, SoInput &ivFileInput)
{
  SoSeparator * root = SoDB::readAll(&ivFileInput);
  if (root) {

    std::vector<std::string>::iterator pos = std::find<std::vector<std::string>::iterator,string>(modes.begin(),modes.end(),string(name));

    if(pos == modes.end())
    { // new mode
      modes.push_back(name);
      addDisplayMaskMode(root, name);
      setDisplayMaskMode(name);
    }else
    { // existing mode
      // not implemented yet
      assert(0);
      root->unref();
    }
  }else
    throw Base::Exception("nothing read");
  return;
}

void ViewProviderExtern::adjustDocumentName(const char* docname)
{
  for ( int i=0; i<this->pcModeSwitch->getNumChildren(); i++ )
  {
    SoNode* child = this->pcModeSwitch->getChild(i);
    adjustRecursiveDocumentName(child, docname);
  }
}

void ViewProviderExtern::adjustRecursiveDocumentName(SoNode* child, const char* docname)
{
  if ( child->getTypeId().isDerivedFrom( SoFCSelection::getClassTypeId() ) ) {
    reinterpret_cast<SoFCSelection*>(child)->documentName = docname;
  } else if ( child->getTypeId().isDerivedFrom( SoGroup::getClassTypeId() ) ) {
    SoGroup* group = (SoGroup*)child;
    for ( int i=0; i<group->getNumChildren(); i++ ) {
      SoNode* subchild = group->getChild(i);
      adjustRecursiveDocumentName(subchild, docname);
    }
  }
}

const char* ViewProviderExtern::getDefaultDisplayMode() const
{
  // returns the first item of the available modes
  return (modes.empty() ? "" : modes.front().c_str());
}

std::vector<std::string> ViewProviderExtern::getDisplayModes(void) const
{
  return modes;
}
