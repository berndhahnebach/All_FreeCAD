/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <werner.wm.mayer@gmx.de>              *
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

#ifndef __VIEWPROVIDERPOINTS_H__
#define __VIEWPROVIDERPOINTS_H__

#include <Gui/ViewProviderFeature.h>


class SoSwitch;
class SoPointSet;
class SoLocateHighlight;
class SoCoordinate3;
class SoNormal;

namespace App {
  class PropertyColorList;
}

namespace Gui {
  class SoFCSelection;
}

namespace Points {
  class PropertyGreyValueList;
  class PropertyNormalList;
  class Feature;
}

namespace PointsGui {

/**
 * The ViewProviderPoints class creates
 * a node representing the point data structure.
 * @author Werner Mayer
 */
class PointsGuiExport ViewProviderPoints : public Gui::ViewProviderDocumentObject
{
  PROPERTY_HEADER(PointsGui::ViewProviderPoints);

public:
  ViewProviderPoints();
  virtual ~ViewProviderPoints();

  /** 
   * Extracts the point data from the feature \a pcFeature and creates
   * an Inventor node \a SoNode with these data. 
   */
  virtual void attach(App::DocumentObject *);
  /// set the viewing mode
  virtual void setMode(const char* ModeName);
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void);
  /// Update the point representation
  virtual void updateData();
  virtual QPixmap getIcon() const;

  /// Set the transparency
  virtual void setTransparency(float);
  /// Set the color
  virtual void setColor(const App::Color &c);

protected:
  void createPoints(Points::Feature *pcFeature);
  void setVertexColorMode(App::PropertyColorList*);
  void setVertexGreyvalueMode(Points::PropertyGreyValueList*);
  void setVertexNormalMode(Points::PropertyNormalList*);

protected:
  SoCoordinate3     *pcPointsCoord;
  SoPointSet        *pcPoints;
  Gui::SoFCSelection *pcHighlight;
  SoMaterial        *pcColorMat;
  SoNormal          *pcPointsNormal;
};

} // namespace MeshGui


#endif // __VIEWPROVIDERPOINTS_H__

