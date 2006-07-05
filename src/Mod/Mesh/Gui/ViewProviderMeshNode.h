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

#ifndef MESHGUI_VIEWPROVIDERMESHNODE_H
#define MESHGUI_VIEWPROVIDERMESHNODE_H

#include <Gui/ViewProviderFeature.h>
#include <Mod/Mesh/App/Core/Elements.h>

#include <qdatetime.h>
#include <vector>
#include <Inventor/fields/SoSFVec2f.h>

class SbViewVolume;

namespace Gui {
  class SoFCSelection;
  class AbstractMouseModel;
}
namespace MeshGui {

/**
 * The ViewProviderMeshNode class creates a node representing the mesh data structure.
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshNode : public Gui::ViewProviderFeature
{
  PROPERTY_HEADER(TriangulationGui::ViewProviderMeshNode);

public:
  ViewProviderMeshNode();
  virtual ~ViewProviderMeshNode();
  
  // Display properties
  App::PropertyFloat LineWidth;
  App::PropertyFloat PointSize;
  App::PropertyBool OpenEdges;

  void attach(App::DocumentObject *pcFeat);
  virtual QPixmap getIcon() const;
  virtual void setMode(const char* ModeName);
  virtual std::list<std::string> getModes() const;

  /** @name Polygon picking */
	//@{
  // Draws the picked polygon
  bool handleEvent(const SoEvent * const ev,Gui::View3DInventorViewer &Viewer);
  /// Sets the edit mnode
  void setEdit(void);
  /// Unsets the edit mode
  void unsetEdit(void);
  /// Returns the edit mode
  const char* getEditModeName(void);
	//@}

protected:
  /// get called by the container whenever a proptery has been changed
  void onChanged(const App::Property* prop);

  SoDrawStyle *pcLineStyle;
  SoDrawStyle *pcPointStyle;


private:
  bool createPolygonMesh( const std::vector<SbVec2f>& rclPoly, const SbViewVolume& vol, const Base::Vector3f& rcNormal, std::vector<MeshCore::MeshGeomFacet>& ) const;
  Gui::AbstractMouseModel* _mouseModel;
  bool m_bEdit;
  QTime _timer;
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDERMESHNODE_H

