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


#ifndef GUI_VIEWPROVIDER_FEATURE_H
#define GUI_VIEWPROVIDER_FEATURE_H

#include <Inventor/lists/SoPickedPointList.h> 
#include "ViewProviderDocumentObject.h"

class SoPickedPointList;
class SbVec2s;

namespace Gui {

class SoFCSelection;
class View3DInventorViewer;

class GuiExport ViewProviderFeature:public ViewProviderDocumentObject
{
  PROPERTY_HEADER(Gui::ViewProviderFeature);

public:
  /// constructor.
  ViewProviderFeature();

  /// destructor.
  virtual ~ViewProviderFeature();

  /**
   * Attaches the document object to this view provider.
   */
  virtual void attach(App::DocumentObject *pcObj);
  SoFCSelection* getHighlightNode() const { return pcHighlight; }

  /**
   * Returns a list of picked points from the geometry under \a pcHighlight.
   * If \a pickAll is false (the default) only the intersection point closest to the camera will be picked, otherwise
   * all intersection points will be picked. 
   */
  SoPickedPointList getPickedPoints(const SbVec2s& pos, const View3DInventorViewer& viewer,bool pickAll=false) const;
  /**
   * This method is provided for convenience and does basically the same as getPickedPoints() unless that only the closest
   * point to the camera will be picked.
   * \note It is in the response of the client programmer to delete the returned SoPickedPoint object.
   */
  SoPickedPoint* getPickedPoint(const SbVec2s& pos, const View3DInventorViewer& viewer) const;

protected:
  SoFCSelection* pcHighlight;
};

} // namespace Gui

#endif // GUI_VIEWPROVIDER_FEATURE_H

