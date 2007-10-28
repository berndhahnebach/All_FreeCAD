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


#ifndef GUI_VIEWPROVIDER_GEOMETRYOBJECT_H
#define GUI_VIEWPROVIDER_GEOMETRYOBJECT_H

#include <Inventor/lists/SoPickedPointList.h> 
#include "ViewProviderDocumentObject.h"

class SbVec2s;

namespace Gui {

class View3DInventorViewer;

//FIXME: At the moment the class ViewProviderDocumentObject has properties like ShapeColor, Transparency, ... which are rather
//related to a geometric data type. So, we should removed these properties from there and use ViewProviderGeometryObject as 
//base class for all geometry descendant view types.

class GuiExport ViewProviderGeometryObject : public ViewProviderDocumentObject
{
  PROPERTY_HEADER(Gui::ViewProviderGeometryObject);

public:
  /// constructor.
  ViewProviderGeometryObject();

  /// destructor.
  virtual ~ViewProviderGeometryObject();

  /*
   * Note: Due to the redesign using display properties in this class directly instead of DocumentObject several
   * methods can be removed, such as copy(), setMatFromObject(), setTransparency(), setColor(), setPointSize() and
   * setLineWidth()
   */

  // Display properties
  App::PropertyColor ShapeColor;
  App::PropertyEnumeration DisplayMode;
  App::PropertyPercent Transparency;
  App::PropertyBool Visibility;
  App::PropertyMaterial ShapeMaterial;

  virtual void attach(App::DocumentObject *pcObject);
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

  virtual void updateData(void){};

protected:
  /// get called by the container whenever a property has been changed
  virtual void onChanged(const App::Property* prop);

protected:
  SoMaterial  *pcShapeMaterial;
};


} // namespace Gui

#endif // GUI_VIEWPROVIDER_GEOMETRYOBJECT_H

