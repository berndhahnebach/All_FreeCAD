/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#ifndef POINTSGUI_VIEWPROVIDERPOINTS_H
#define POINTSGUI_VIEWPROVIDERPOINTS_H

#include <Base/Vector3D.h>
#include <Gui/ViewProviderFeature.h>
#include <Inventor/SbVec2f.h>


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
class PointsGuiExport ViewProviderPoints : public Gui::ViewProviderFeature
{
  PROPERTY_HEADER(PointsGui::ViewProviderPoints);

public:
  ViewProviderPoints();
  virtual ~ViewProviderPoints();

  App::PropertyFloat PointSize;

  /** 
   * Extracts the point data from the feature \a pcFeature and creates
   * an Inventor node \a SoNode with these data. 
   */
  virtual void attach(App::DocumentObject *);
  /// set the viewing mode
  virtual void setDisplayMode(const char* ModeName);
  /// returns a list of all possible modes
  virtual std::vector<std::string> getDisplayModes(void) const;
  /// Update the point representation
  virtual void updateData(const App::Property*);
  virtual QIcon getIcon() const;

  /// Sets the edit mnode
  void setEdit(void);
  /// Unsets the edit mode
  void unsetEdit(void);

  bool handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer);

protected:
  void onChanged(const App::Property* prop);
  void createPoints(const std::vector<Base::Vector3f>& pnts);
  void setVertexColorMode(App::PropertyColorList*);
  void setVertexGreyvalueMode(Points::PropertyGreyValueList*);
  void setVertexNormalMode(Points::PropertyNormalList*);
  virtual void cut( const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer &Viewer);

protected:
  SoCoordinate3     *pcPointsCoord;
  SoPointSet        *pcPoints;
  SoMaterial        *pcColorMat;
  SoNormal          *pcPointsNormal;
  SoDrawStyle       *pcPointStyle;

private:
  bool _bEdit;
};

} // namespace PointsGui


#endif // POINTSGUI_VIEWPROVIDERPOINTS_H

