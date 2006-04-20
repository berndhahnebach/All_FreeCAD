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
# include <Inventor/misc/SoState.h>
# include <Inventor/elements/SoLightModelElement.h>
# include <Inventor/bundles/SoMaterialBundle.h>
#endif

#include "SoFCMeshNode.h"
#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/App/Core/Iterator.h>
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

void SoFCMeshNode::setMesh(const Mesh::Feature* mesh)
{ 
  _mesh = mesh; 
}

void SoFCMeshNode::GLRender(SoGLRenderAction *action)
{
  if (_mesh && shouldGLRender(action))
  {
    SoState*  state = action->getState();
    SbBool send_normals = (SoLightModelElement::get(state) !=
			   SoLightModelElement::BASE_COLOR);

    SoMaterialBundle mb(action);
    mb.sendFirst();

    drawFaces(send_normals?true:false);
  }
}

void SoFCMeshNode::drawFaces(SbBool send_normals)
{
  MeshCore::MeshFacetIterator it(_mesh->getMesh());

  if (send_normals)
  {
    glBegin(GL_TRIANGLES);
    for (it.Init(); it.More(); it.Next())
    {
      glNormal(it->GetNormal());
      glVertex(it->_aclPoints[0]);
      glVertex(it->_aclPoints[1]);
      glVertex(it->_aclPoints[2]);
    }
    glEnd();
  }
  else 
  {
    glBegin(GL_TRIANGLES);
    for (it.Init(); it.More(); it.Next())
    {
      glVertex(it->_aclPoints[0]);
      glVertex(it->_aclPoints[1]);
      glVertex(it->_aclPoints[2]);
    }
    glEnd();
  }
}

void SoFCMeshNode::generatePrimitives(SoAction* _action)
{
  if (_mesh)
  {
    MeshCore::MeshFacetIterator it(_mesh->getMesh());
    SoPrimitiveVertex pv;
    beginShape(_action, TRIANGLES);

    for (it.Init(); it.More(); it.Next())
    {
      pv.setNormal(sbvec3f(it->GetNormal()));
			   
      pv.setPoint(sbvec3f(it->_aclPoints[0]));
      shapeVertex(&pv);

      pv.setPoint(sbvec3f(it->_aclPoints[1]));
      shapeVertex(&pv);

      pv.setPoint(sbvec3f(it->_aclPoints[2]));
      shapeVertex(&pv);
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
  return 0;
}

void SoFCMeshNode::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
  if (_mesh && _mesh->getMesh().CountPoints() > 0) {
    const Base::BoundBox3f& cBox = _mesh->getMesh().GetBoundBox();
    box.setBounds(SbVec3f(cBox.MinX,cBox.MinY,cBox.MinZ),
		              SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MaxZ));
  }
  else {
    box.setBounds(SbVec3f(0,0,0), SbVec3f(0,0,0));
  }
}
