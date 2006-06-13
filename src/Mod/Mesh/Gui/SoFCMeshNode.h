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

#ifndef MESHGUI_SOFC_MESH_NODE_H
#define MESHGUI_SOFC_MESH_NODE_H

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFInt32.h>

namespace Mesh {
class Feature;
}

namespace MeshGui {

class GuiMeshExport SoFCMeshNode : public SoShape {
  typedef SoShape inherited;

  SO_NODE_HEADER(SoFCMeshNode);
    
public:
  static void initClass();
  SoFCMeshNode(const Mesh::Feature* mesh=0);
  void setMesh(const Mesh::Feature* mesh);

  virtual void write( SoWriteAction* action );
  unsigned int MaximumTriangles;

protected:
  virtual void GLRender(SoGLRenderAction *action);
  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center);
  virtual void getPrimitiveCount(SoGetPrimitiveCountAction * action);
  virtual void generatePrimitives(SoAction *action);
  virtual SoDetail * createTriangleDetail(SoRayPickAction * action,
                                          const SoPrimitiveVertex * v1,
                                          const SoPrimitiveVertex * v2,
                                          const SoPrimitiveVertex * v3,
                                          SoPickedPoint * pp);

  virtual SbBool readInstance( SoInput* in, unsigned short  flags );

private:
  // Force using the reference count mechanism.
  virtual ~SoFCMeshNode() {};
  virtual void notify(SoNotList * list);
  // Draw faces
  void drawFaces(SbBool needNormals);
  void drawPoints(SbBool needNormals);
  unsigned int countTriangles() const;

private:
  const Mesh::Feature*  _mesh;
  SoMFVec3f point;
  SoMFInt32 coordIndex;
};

} // namespace MeshGui


#endif // MESHGUI_SOFC_MESH_NODE_H

