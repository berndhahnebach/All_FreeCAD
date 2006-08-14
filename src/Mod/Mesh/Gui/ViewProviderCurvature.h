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

#ifndef MESHGUI_VIEWPROVIDER_MESH_CURVATURE_H
#define MESHGUI_VIEWPROVIDER_MESH_CURVATURE_H

#include <Base/Observer.h>
#include "ViewProvider.h"

class SoSeparator;
class SbVec3f;
class SoSwitch;
class SoCoordinate3;
class SoNormal;
class SoIndexedFaceSet;
class SoFaceSet;
class SoPath;
class SoLocateHighlight;
class SoTransformerManip;

namespace Gui {
  class SoFCColorBar;
  class View3DInventorViewer;
}

namespace App {
  class Feature;
}

namespace MeshGui {

/** The ViewProviderMeshCurvature class is associated to the mesh curvature feature. It allows to display the most known types of
 * curvatures, such as Gaussian curvature, mean curvature, minimum and maximum curvature.
 * Moreover a color bar is also added to the scene.
 *
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshCurvature : public ViewProviderMesh, public Base::Observer<int> {
  typedef ViewProviderMesh inherited;

  PROPERTY_HEADER(MeshGui::ViewProviderMeshCurvature);

public:
  ViewProviderMeshCurvature();
  virtual ~ViewProviderMeshCurvature();

  /// Extracts the mesh data from the feature \a pcFeature and creates an Inventor node \a SoNode with these data. 
  void attach(App::DocumentObject* pcFeature);
  /// Handles pick events to write curvature value to the status bar.
  bool handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer);
  /// Sets the viewing mode
  void setMode(const char* ModeName);
  /// Returns a list of all possible modes
  std::list<std::string> getModes(void) const;
  /// Updates the mesh feature representation
  void updateData();
  /// Returns a pixmap for the associated feature type
  QPixmap getIcon() const;
  /// Once the color bar settinhs has been changed this method gets called to update the feature's representation
  void OnChange(Base::Subject<int> &rCaller,int rcReason);
  /// Returns a color bar
  SoSeparator* getFrontRoot(void) const;

protected:
  void setVertexCurvatureMode(int mode);

private:
  void init(App::DocumentObject *pcFeat);

protected:
  SoMaterial       * pcColorMat;
  Gui::SoFCColorBar* pcColorBar;
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDER_MESH_CURVATURE_H

