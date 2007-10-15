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

#ifndef MESHGUI_VIEWPROVIDERMESHFACESET_H
#define MESHGUI_VIEWPROVIDERMESHFACESET_H

#include <Gui/ViewProviderFeature.h>
#include <Mod/Mesh/App/Core/Elements.h>

#include <vector>
#include <Inventor/fields/SoSFVec2f.h>

class SbViewVolume;
class SoBaseColor;
class SoShapeHints;
class SoEventCallback;

namespace Gui {
  class SoFCSelection;
  class AbstractMouseModel;
}
namespace MeshGui {
class SoFCMeshVertex;
class SoFCMeshFacet;
class SoFCMeshFaceSet;

/**
 * The ViewProviderMeshFaceSet class creates a node representing the mesh data structure.
 * It does basically the same ViewProviderMeshNode by rendering directly the FreeCAD mesh
 * structure whereas this class follows the Inventor way. For more details @see 
 * SoFCMeshNode and SoFCMeshFaceSet.
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshFaceSet : public Gui::ViewProviderFeature
{
  PROPERTY_HEADER(TriangulationGui::ViewProviderMeshFaceSet);

public:
  ViewProviderMeshFaceSet();
  virtual ~ViewProviderMeshFaceSet();
  
  // Display properties
  App::PropertyFloatConstraint LineWidth;
  App::PropertyFloatConstraint PointSize;
  App::PropertyBool OpenEdges;
  App::PropertyBool BoundingBox;
  App::PropertyEnumeration Lighting;

  void attach(App::DocumentObject *pcFeat);
  virtual void updateData();
  virtual QIcon getIcon() const;
  virtual void setDisplayMode(const char* ModeName);
  virtual std::vector<std::string> getDisplayModes() const;

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
  void faceInfo(unsigned long facet);
  void fillHole(unsigned long facet);
  //@}

protected:
  /// get called by the container whenever a proptery has been changed
  void onChanged(const App::Property* prop);
  void showOpenEdges( bool );
  void showBoundingBox( bool );
  void setOpenEdgeColorFrom( const App::Color& col );
  virtual void cutMesh( const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer &Viewer);

  SoFCMeshVertex * pcVertexNode;
  SoFCMeshFacet  * pcFacetNode;
  SoFCMeshFaceSet* pcFaceSet;
  SoDrawStyle    * pcLineStyle;
  SoDrawStyle    * pcPointStyle;
  SoSeparator    * pcOpenEdge;
  SoSeparator    * pBoundingBox;
  SoBaseColor    * pOpenColor;
  SoShapeHints   * pShapeHints;

public:
  static void faceInfoCallback(void * ud, SoEventCallback * n);
  static void fillHoleCallback(void * ud, SoEventCallback * n);

private:
  bool m_bEdit;

  static App::PropertyFloatConstraint::Constraints floatRange;
  static const char* LightingEnums[];
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDERMESHFACESET_H

