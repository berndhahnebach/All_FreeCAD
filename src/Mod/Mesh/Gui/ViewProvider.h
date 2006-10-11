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

#ifndef __VIEWPROVIDERMESH_H__
#define __VIEWPROVIDERMESH_H__

#include <qdatetime.h>
#include <vector>
#include <Inventor/fields/SoSFVec2f.h>

#include "../App/Core/Elements.h"
#include <Gui/ViewProviderFeature.h>
#include <App/PropertyStandard.h>


class SoSeparator;
class SbVec3f;
class SoSwitch;
class SoCoordinate3;
class SoNormal;
class SoIndexedFaceSet;
class SoFaceSet;
class SoPath;
class SoLocateHighlight;
class SoEventCallback;
class SbViewVolume;
class SoBaseColor;

namespace App {
  class Color;
}

namespace Gui {
  class View3DInventorViewer;
  class SoFCSelection;
  class AbstractMouseModel;
}


namespace MeshCore {
  class MeshKernel;
}


namespace MeshGui {

/**
 * The ViewProviderExport class creates an empty node.
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderExport : public Gui::ViewProviderDocumentObject
{
  PROPERTY_HEADER(MeshGui::ViewProviderExport);

public:
  ViewProviderExport();
  virtual ~ViewProviderExport();

  virtual QPixmap getIcon() const;
  void update();
  SoSeparator* getRoot(void){return 0;}
  std::vector<std::string> getDisplayModes(void) const;
  const char* getDefaultDisplayMode() const;
};

/**
 * The ViewProviderMesh class creates
 * a node representing the mesh data structure.
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMesh : public Gui::ViewProviderFeature
{
  PROPERTY_HEADER(MeshGui::ViewProviderMesh);

public:
  ViewProviderMesh();
  virtual ~ViewProviderMesh();

  // Display properties
  App::PropertyFloat LineWidth;
  App::PropertyFloat PointSize;
  App::PropertyBool OpenEdges;

  /** 
   * Extracts the mesh data from the feature \a pcFeature and creates
   * an Inventor node \a SoNode with these data. 
   */
  virtual void attach(App::DocumentObject *);
  /// Sets the correct display mode
  virtual void setDisplayMode(const char* ModeName);
  /// returns a list of all possible modes
  virtual std::vector<std::string> getDisplayModes(void) const;
  /// Update the Mesh representation
  virtual void updateData();
  virtual QPixmap getIcon() const;

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

  /// Creates a tool mesh from the previous picked polygon on the viewer
  static bool createToolMesh( const std::vector<SbVec2f>& rclPoly, const SbViewVolume& vol, const Base::Vector3f& rcNormal, std::vector<MeshCore::MeshGeomFacet>& );

protected:
  /// get called by the container whenever a proptery has been changed
  void onChanged(const App::Property* prop);
  /// helper method to build up the FaceSet
  void createMesh( const MeshCore::MeshKernel& pcMesh );
  void showOpenEdges( bool );
  void setOpenEdgeColorFrom( const App::Color& col );

protected:
  SoCoordinate3     *pcMeshCoord;
  SoIndexedFaceSet  *pcMeshFaces;
  //SoFaceSet         *pcMeshFaces;
  SoBaseColor       *pOpenEdges;
  SoDrawStyle       *pcLineStyle;
  SoDrawStyle       *pcPointStyle;

private:
  bool m_bEdit;
};

} // namespace MeshGui


#endif // __VIEWPROVIDERMESH_H__

