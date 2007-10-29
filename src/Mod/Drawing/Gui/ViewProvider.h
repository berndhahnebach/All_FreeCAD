/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is Drawing of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A DrawingICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef __VIEWPROVIDERDrawing_H__
#define __VIEWPROVIDERDrawing_H__

#include <Gui/ViewProviderFeature.h>


namespace DrawingGui {


class DrawingGuiExport ViewProviderDrawing:public Gui::ViewProviderDocumentObject
{
  PROPERTY_HEADER(DrawingGui::ViewProviderDrawing);

public:
  /// constructor
  ViewProviderDrawing();
  /// destructor
  virtual ~ViewProviderDrawing();


  virtual void attach(App::DocumentObject *);
  virtual void setDisplayMode(const char* ModeName);
  /// returns a list of all possible modes
  virtual std::vector<std::string> getDisplayModes(void) const;
  /// Update the Drawing representation
  //virtual void update(const ChangeType&);

  virtual void updateData(const App::Property*);

protected:

};

} // namespace DrawingGui


#endif // __VIEWPROVIDERDrawing_H__

