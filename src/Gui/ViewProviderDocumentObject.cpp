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
# include <qpixmap.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <App/Material.h>
#include <App/Feature.h>
#include "Application.h"
#include "Document.h"
#include "Selection.h"
#include "ViewProviderDocumentObject.h"




using namespace Gui;


PROPERTY_SOURCE(Gui::ViewProviderDocumentObject, Gui::ViewProvider)

ViewProviderDocumentObject::ViewProviderDocumentObject()
  : pcObject(0)
{
  ADD_PROPERTY(DisplayMode,((long)0));
  ADD_PROPERTY(Visibility,(true));

  sPixmap = "Feature";
}


ViewProviderDocumentObject::~ViewProviderDocumentObject()
{
  // Make sure that the property class does not destruct our string list
  DisplayMode.setEnums(0);
}

void ViewProviderDocumentObject::onChanged(const App::Property* prop)
{
  if ( prop == &DisplayMode ) {
    setActiveMode();
  } else if ( prop == &Visibility ) {
    Visibility.getValue() ? show() : hide();
  }
}

void ViewProviderDocumentObject::attach(App::DocumentObject *pcObj)
{
  // save Object pointer
  pcObject = pcObj;

  // Retrieve the supported display modes of the view provider
  aDisplayModesArray = this->getDisplayModes();

  if( aDisplayModesArray.empty() )
    aDisplayModesArray.push_back("");

#if 1
  // We must collect the const char* of the strings and give it to PropertyEnumeration,
  // but we are still responsible for them, i.e. the property class must not delete the literals.
  for ( std::vector<std::string>::iterator it = aDisplayModesArray.begin(); it != aDisplayModesArray.end(); ++it ) {
    aDisplayEnumsArray.push_back( it->c_str() );
  }
  aDisplayEnumsArray.push_back(0); // null termination
  DisplayMode.setEnums(&(aDisplayEnumsArray[0]));
#else
  // We must convert the array of strings into an array of const char*
  std::vector<const char*>* enums = new std::vector<const char*>;
  for ( std::vector<std::string>::iterator it = modes.begin(); it != modes.end(); ++it ) {
    // We must duplicate the string literals because the vector gets deleted later on all its content is lost
#if defined (_MSC_VER)
    const char* mode = _strdup( it->c_str() );
#else
    const char* mode = strdup( it->c_str() );
#endif
    enums->push_back( mode );
  }
  enums->push_back(0); // null termination

  DisplayMode.enableNotify(false);
  DisplayMode.setEnums(&((*enums)[0]));
#endif  
  // set the active mode
  const char* defmode = this->getDefaultDisplayMode();
  if ( defmode )
    DisplayMode.setValue(defmode);
}

SoSeparator* ViewProviderDocumentObject::findFrontRootOfType( const SoType& type) const
{
  // first get the document this object is part of and get its GUI counterpart
  const App::Document& pAppDoc = pcObject->getDocument();
  Gui::Document* pGuiDoc = Gui::Application::Instance->getDocument(pAppDoc.getName());

  // search in all view providers for the node type
  std::vector<App::DocumentObject*> obj = pAppDoc.getObjects();
  for ( std::vector<App::DocumentObject*>::iterator it = obj.begin(); it != obj.end(); ++it )
  {
    const ViewProvider* vp = pGuiDoc->getViewProvider(*it);
    // Ignore 'this' view provider. It could also happen that vp is 0, e.g. when
    // several objects have been added to the App::Document before notifying the
    // Gui::Document
    if ( !vp || vp == this )
      continue;
    SoSeparator* front = vp->getFrontRoot();
    if ( front && front->getTypeId() == type ) 
      return front;
  }

  return 0;
}

void ViewProviderDocumentObject::setActiveMode()
{
  const char* mode = DisplayMode.getValueAsString();
  if ( mode )
    setDisplayMode(mode);
  if ( !Visibility.getValue() )
    ViewProvider::hide();
}

const char* ViewProviderDocumentObject::getDefaultDisplayMode() const
{
  // We use the first item then
  return 0;
}

std::vector<std::string> ViewProviderDocumentObject::getDisplayModes(void) const
{
  // empty
  return std::vector<std::string>();
}
