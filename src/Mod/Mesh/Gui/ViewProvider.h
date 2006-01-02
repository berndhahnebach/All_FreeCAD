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

#ifndef _PreComp_
# include <qdatetime.h>
# include <vector>
# include <Inventor/fields/SoSFVec2f.h>
#endif

#include "../App/Core/Elements.h"
#include <Gui/ViewProviderFeature.h>


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

namespace Gui {
  class View3DInventorViewer;
  class SoFCSelection;
  class AbstractMouseModel;
}


namespace Mesh {
  class MeshWithProperty;
  class MeshPropertyColor;
}


namespace MeshGui {

/**
 * The ViewProviderMesh class creates
 * a node representing the mesh data structure.
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMesh : public Gui::ViewProviderFeature
{
public:
  ViewProviderMesh();
  virtual ~ViewProviderMesh();

  /** 
   * Extracts the mesh data from the feature \a pcFeature and creates
   * an Inventor node \a SoNode with these data. 
   */
  virtual void attach(App::Feature *);
  /// Sets the correct display mode
  virtual void setMode(const char* ModeName);
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void);
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

protected:
  /// helper methode to build up the FaceSet
  void createMesh(Mesh::MeshWithProperty *pcMesh);
  /// Creates a tool mesh from the previous picked polygon on the viewer
  bool createToolMesh( const SbViewVolume& vol, const Base::Vector3D& rcNormal, std::vector<MeshCore::MeshGeomFacet>& ) const;

protected:
  SoCoordinate3     *pcMeshCoord;
  SoNormal          *pcMeshNormal;
  SoIndexedFaceSet  *pcMeshFaces;
  //SoFaceSet         *pcMeshFaces;
  Gui::SoFCSelection *pcHighlight;

private:
  Gui::AbstractMouseModel* _mouseModel;
  bool m_bEdit;
  QTime _timer;
  std::vector<SbVec2f> _clPoly;
};

} // namespace MeshGui


#endif // __VIEWPROVIDERMESH_H__

