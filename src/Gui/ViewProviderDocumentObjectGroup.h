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


#ifndef GUI_VIEWPROVIDER_DOCUMENTOBJECTGROUP_H
#define GUI_VIEWPROVIDER_DOCUMENTOBJECTGROUP_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif


#include "ViewProviderDocumentObject.h"

namespace Gui {

class GuiExport ViewProviderDocumentObjectGroup:public ViewProviderDocumentObject
{
  PROPERTY_HEADER(Gui::ViewProviderDocumentObjectGroup);

public:
  /// constructor.
  ViewProviderDocumentObjectGroup();

  /// destructor.
  virtual ~ViewProviderDocumentObjectGroup();

  virtual void attach(App::DocumentObject *pcObject);
  //FIXME: getIcon(Icon) enum Icon {Active, Disabled, Open, Closed, ...} 
  virtual QPixmap getIcon(void) const;
  virtual QPixmap getOpenedGroupIcon() const;
  virtual QPixmap getClosedGroupIcon() const;
  /// returns a list of all possible modes
  virtual std::vector<std::string> getDisplayModes(void) const;
  bool isShow(void) const;

protected:
  /// get called by the container whenever a property has been changed
  virtual void onChanged(const App::Property* prop);
  void getViewProviders(std::vector<ViewProviderDocumentObject*>&) const;
};


} // namespace Gui

#endif // GUI_VIEWPROVIDER_DOCUMENTOBJECTGROUP_H

