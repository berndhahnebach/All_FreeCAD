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


#include "PreCompiled.h"

#ifndef _PreComp_
# ifdef FC_OS_WIN32
# include <windows.h>
# endif
# include <GL/gl.h>
# include <Inventor/SbBox.h>
# include <Inventor/SoPrimitiveVertex.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/actions/SoGetPrimitiveCountAction.h>
# include <Inventor/bundles/SoMaterialBundle.h>
# include <Inventor/bundles/SoTextureCoordinateBundle.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/details/SoPointDetail.h>
# include <Inventor/elements/SoGLCacheContextElement.h>
# include <Inventor/elements/SoLightModelElement.h>
# include <Inventor/misc/SoState.h>
#endif

#include "SoFCMeshNode.h"
#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/App/MeshFeature.h>

using namespace MeshGui;

// Helper functions: draw vertices
inline void glVertex(const MeshCore::MeshPoint& _v)  
{ 
  float v[3];
  v[0]=_v.x; v[1]=_v.y;v[2]=_v.z;
  glVertex3fv(v); 
}

// Helper functions: draw normal
inline void glNormal(const Base::Vector3f& _n)
{ 
  float n[3];
  n[0]=_n.x; n[1]=_n.y;n[2]=_n.z;
  glNormal3fv(n); 
}

// Helper functions: draw normal
inline void glNormal(float* n)
{ 
  glNormal3fv(n); 
}

// Helper function: convert Vec to SbVec3f
inline SbVec3f sbvec3f(const Base::Vector3f& _v) {
  return SbVec3f(_v.x, _v.y, _v.z); 
}

SO_NODE_SOURCE(SoFCMeshNode);

void SoFCMeshNode::initClass()
{
  SO_NODE_INIT_CLASS(SoFCMeshNode, SoShape, "Shape");
}

SoFCMeshNode::SoFCMeshNode(const Mesh::Feature* mesh) : _mesh(mesh)
{
  SO_NODE_CONSTRUCTOR(SoFCMeshNode);
}

void SoFCMeshNode::notify(SoNotList * node)
{
  SoShape::notify(node);
}

void SoFCMeshNode::setMesh(const Mesh::Feature* mesh)
{ 
  _mesh = mesh; 
}

void SoFCMeshNode::GLRender(SoGLRenderAction *action)
{
  if (_mesh && shouldGLRender(action))
  {
    SoState*  state = action->getState();

    SoMaterialBundle mb(action);
    //SoTextureCoordinateBundle tb(action, true, false);

    SbBool needNormals = !mb.isColorOnly()/* || tb.isFunction()*/;
    mb.sendFirst();  // make sure we have the correct material

    drawFaces(needNormals);

    // Disable caching for this node
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
  }
}

void SoFCMeshNode::drawFaces(SbBool needNormals)
{
  // Use the data structure directly and not through MeshFacetIterator as this
  // class is quite slowly (at least for rendering)
  const MeshCore::MeshPointArray& rPoints = _mesh->getMesh().GetPoints();
  const MeshCore::MeshFacetArray& rFacets = _mesh->getMesh().GetFacets();

  if (needNormals)
  {
    glBegin(GL_TRIANGLES);
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it )
    {
      const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
      const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
      const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

      // Calculate the normal n = (v1-v0)x(v2-v0)
      float n[3];
      n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
      n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
      n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);
      
      glNormal(n);
      glVertex(v0);
      glVertex(v1);
      glVertex(v2);
    }
    glEnd();
  }
  else 
  {
    glBegin(GL_TRIANGLES);
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it )
    {
      glVertex(rPoints[it->_aulPoints[0]]);
      glVertex(rPoints[it->_aulPoints[1]]);
      glVertex(rPoints[it->_aulPoints[2]]);
    }
    glEnd();
  }
}

void SoFCMeshNode::generatePrimitives(SoAction* _action)
{
  if (_mesh)
  {
    // Use the data structure directly and not through MeshFacetIterator as this
    // class is quite slowly (at least for rendering)
    const MeshCore::MeshPointArray& rPoints = _mesh->getMesh().GetPoints();
    const MeshCore::MeshFacetArray& rFacets = _mesh->getMesh().GetFacets();

    // Create the information when moving over or picking into the scene
    SoPrimitiveVertex vertex;
    SoPointDetail pointDetail;
    SoFaceDetail faceDetail;

    vertex.setDetail(&pointDetail);

    beginShape(_action, TRIANGLES, &faceDetail);
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it )
    {
      const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
      const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
      const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

      // Calculate the normal n = (v1-v0)x(v2-v0)
      SbVec3f n;
      n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
      n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
      n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);

      // Set the normal
      vertex.setNormal(n);

      // Vertex 0
      pointDetail.setCoordinateIndex(it->_aulPoints[0]);
      vertex.setPoint(sbvec3f(v0));
      shapeVertex(&vertex);

      // Vertex 1
      pointDetail.setCoordinateIndex(it->_aulPoints[1]);
      vertex.setPoint(sbvec3f(v1));
      shapeVertex(&vertex);

      // Vertex 2
      pointDetail.setCoordinateIndex(it->_aulPoints[2]);
      vertex.setPoint(sbvec3f(v2));
      shapeVertex(&vertex);

      // Increment for the next face
      faceDetail.incFaceIndex();
    }

    endShape();
  }
}

SoDetail * SoFCMeshNode::createTriangleDetail(SoRayPickAction * action,
                                              const SoPrimitiveVertex * v1,
                                              const SoPrimitiveVertex * v2,
                                              const SoPrimitiveVertex * v3,
                                              SoPickedPoint * pp)
{
  SoDetail* detail = inherited::createTriangleDetail(action, v1, v2, v3, pp);
  return detail;
}

void SoFCMeshNode::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
  // Get the bbox directly from the mesh kernel
  if (_mesh && _mesh->getMesh().CountPoints() > 0) {
    const Base::BoundBox3f& cBox = _mesh->getMesh().GetBoundBox();
    box.setBounds(SbVec3f(cBox.MinX,cBox.MinY,cBox.MinZ),
		              SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MaxZ));
    Base::Vector3f mid = cBox.CalcCenter();
    center.setValue(mid.x,mid.y,mid.z);
  }
  else {
    box.setBounds(SbVec3f(0,0,0), SbVec3f(0,0,0));
    center.setValue(0.0f,0.0f,0.0f);
  }
}

void SoFCMeshNode::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  if (_mesh)
  {
    if (!this->shouldPrimitiveCount(action)) return;
    action->addNumTriangles(_mesh->getMesh().CountFacets());
  }
}
