/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qpixmap.h>
#endif

#include <App/DocumentObjectGroup.h>
#include <App/Document.h>

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "ViewProviderDocumentObjectGroup.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Document.h"
#include "Tree.h"


using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderDocumentObjectGroup, Gui::ViewProviderDocumentObject)


/**
 * Creates the view provider for an object group.
 */
ViewProviderDocumentObjectGroup::ViewProviderDocumentObjectGroup()
{
}

ViewProviderDocumentObjectGroup::~ViewProviderDocumentObjectGroup()
{
}

/**
 * Whenever a property of the group gets changed then the same property of all associated view providers of
 * the objects of the object group get changed as well.
 */
void ViewProviderDocumentObjectGroup::onChanged(const App::Property* prop)
{
  ViewProviderDocumentObject::onChanged(prop);

  //FIXME: Do this changes only in the specialized Gui commands otherwise loading of project files leads to strange effects.
  //But changing from within property editor doesn't affect the child objects!?
  //Probably it's best to remove the properties from the base class.
//  std::vector<ViewProviderDocumentObject*> vp;
//  getViewProviders(vp);
//  for ( std::vector<ViewProviderDocumentObject*>::iterator it = vp.begin(); it != vp.end(); ++it )
//  {
//    if ( prop == &ShapeColor ) {
//      const App::Color& c = ShapeColor.getValue();
//      (*it)->ShapeColor.setValue(c);
//    } else if ( prop == &Transparency ) {
//      int trans = Transparency.getValue();
//      (*it)->Transparency.setValue(trans);
//    } else if ( prop == &ShapeMaterial ) {
//      const App::Material& Mat = ShapeMaterial.getValue();
//      (*it)->ShapeMaterial.setValue(Mat);
//    } else if ( prop == &Visibility ) {
//      bool val = Visibility.getValue();
//      (*it)->Visibility.setValue(val);
//    }
//  }
}

void ViewProviderDocumentObjectGroup::attach(App::DocumentObject *pcObj)
{
  ViewProviderDocumentObject::attach(pcObj);
}

std::vector<std::string> ViewProviderDocumentObjectGroup::getDisplayModes(void) const
{
  // empty
  return std::vector<std::string>();
}

bool ViewProviderDocumentObjectGroup::isShow(void) const
{
  return Visibility.getValue();
}

/**
 * Extracts the associated view providers of the objects of the associated object group group. 
 */
void ViewProviderDocumentObjectGroup::getViewProviders(std::vector<ViewProviderDocumentObject*>& vp) const
{
  App::DocumentObject* doc = getObject();
  if ( doc->getTypeId().isDerivedFrom(App::DocumentObjectGroup::getClassTypeId()) )
  {
    Gui::Document* gd = Application::Instance->getDocument(&doc->getDocument());
    App::DocumentObjectGroup* grp = (App::DocumentObjectGroup*)doc;
    std::vector<App::DocumentObject*> obj = grp->getObjects();
    for ( std::vector<App::DocumentObject*>::iterator it = obj.begin(); it != obj.end(); ++it )
    {
      ViewProvider* v = gd->getViewProvider(*it);
      if ( v && v->getTypeId().isDerivedFrom(ViewProviderDocumentObject::getClassTypeId()) )
        vp.push_back((ViewProviderDocumentObject*)v);
    }
  }
}

/**
 * Returns the pixmap for the opened list item.
 */
QPixmap ViewProviderDocumentObjectGroup::getOpenedGroupIcon() const
{
  return Gui::BitmapFactory().pixmap("PrefTree_GroupOpen");
}

/**
 * Returns the pixmap for the closed list item.
 */
QPixmap ViewProviderDocumentObjectGroup::getClosedGroupIcon() const
{
  return Gui::BitmapFactory().pixmap("PrefTree_GroupClosed");
}
