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
# include <Inventor/SoOutput.h>
# include <Inventor/SoPrimitiveVertex.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/actions/SoGetPrimitiveCountAction.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/bundles/SoMaterialBundle.h>
# include <Inventor/bundles/SoTextureCoordinateBundle.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/details/SoPointDetail.h>
# include <Inventor/elements/SoGLCacheContextElement.h>
# include <Inventor/elements/SoLightModelElement.h>
# include <Inventor/misc/SoState.h>
#endif

#include "SoFCMeshNode.h"
#include <Gui/SoFCInteractiveElement.h>
#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/App/MeshFeature.h>

using namespace MeshGui;

/**
 * class SoFCMeshNode
 * \brief The SoFCMeshNode class is designed to render huge meshes.
 *
 * The SoFCMeshNode is an Inventor shape node that is designed to render huge meshes. If the mesh exceeds a certain number of triangles
 * and the user does some intersections (e.g. moving, rotating, zooming, spinning, etc.) with the mesh then the GLRender() method renders 
 * only the gravity points of a subset of the triangles.
 * If there is no user interaction with the mesh then all triangles are rendered.
 * The limit of maximum allowed triangles can be specified in \a MaximumTriangles, the default value is set to 500.000.
 *
 * The GLRender() method checks the status of the SoFCInteractiveElement to decide to be in interactive mode or not.
 * To take advantage of this facility the client programmer must set the status of the SoFCInteractiveElement to \a true
 * if there is a user interation and set the status to \a false if not. This can be done e.g. in the actualRedraw() method of
 * the viewer.
 * \author Werner Mayer
 */

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

SoFCMeshNode::SoFCMeshNode(const Mesh::Feature* mesh) : MaximumTriangles(500000),_mesh(mesh)
{
  SO_NODE_CONSTRUCTOR(SoFCMeshNode);
  SO_NODE_ADD_FIELD(point, (0.0f, 0.0f, 0.0f));
  SO_NODE_ADD_FIELD(coordIndex,(0));
}

void SoFCMeshNode::notify(SoNotList * node)
{
  SoShape::notify(node);
}

/**
 * Sets the mesh.
 */
void SoFCMeshNode::setMesh(const Mesh::Feature* mesh)
{ 
  _mesh = mesh; 
}

/**
 * Either renders the complete mesh or only a subset of the points.
 */
void SoFCMeshNode::GLRender(SoGLRenderAction *action)
{
  if (_mesh && shouldGLRender(action))
  {
    SoState*  state = action->getState();

    SbBool mode = Gui::SoFCInteractiveElement::get(state);

    SoMaterialBundle mb(action);
    //SoTextureCoordinateBundle tb(action, true, false);

    SbBool needNormals = !mb.isColorOnly()/* || tb.isFunction()*/;
    mb.sendFirst();  // make sure we have the correct material

    if ( mode == false || countTriangles() <= this->MaximumTriangles )
      drawFaces(needNormals);
    else
      drawPoints(needNormals);

    // Disable caching for this node
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
  }
}

/**
 * Renders the triangles of the complete mesh.
 */
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

/**
 * Renders the gravity points of a subset of triangles.
 */
void SoFCMeshNode::drawPoints(SbBool needNormals)
{
  // Use the data structure directly and not through MeshFacetIterator as this
  // class is quite slowly (at least for rendering)
  const MeshCore::MeshPointArray& rPoints = _mesh->getMesh().GetPoints();
  const MeshCore::MeshFacetArray& rFacets = _mesh->getMesh().GetFacets();
  int mod = rFacets.size()/MaximumTriangles+1;

  float size = std::min<float>((float)mod,3.0f);
  glPointSize(size);

  if (needNormals)
  {
    glBegin(GL_POINTS);
    int ct=0;
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it, ct++ )
    {
      if ( ct%mod==0 ) {
        const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
        const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
        const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];

        // Calculate the normal n = (v1-v0)x(v2-v0)
        float n[3];
        n[0] = (v1.y-v0.y)*(v2.z-v0.z)-(v1.z-v0.z)*(v2.y-v0.y);
        n[1] = (v1.z-v0.z)*(v2.x-v0.x)-(v1.x-v0.x)*(v2.z-v0.z);
        n[2] = (v1.x-v0.x)*(v2.y-v0.y)-(v1.y-v0.y)*(v2.x-v0.x);
      
        // Calculate the center point p=(v0+v1+v2)/3
        float p[3];
        p[0] = (v0.x+v1.x+v2.x)/3.0f;
        p[1] = (v0.y+v1.y+v2.y)/3.0f;
        p[2] = (v0.z+v1.z+v2.z)/3.0f;
        glNormal3fv(n);
        glVertex3fv(p);
      }
    }
    glEnd();
  }
  else 
  {
    glBegin(GL_POINTS);
    int ct=0;
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); ++it, ct++ )
    {
      if (ct%mod==0) {
        const MeshCore::MeshPoint& v0 = rPoints[it->_aulPoints[0]];
        const MeshCore::MeshPoint& v1 = rPoints[it->_aulPoints[1]];
        const MeshCore::MeshPoint& v2 = rPoints[it->_aulPoints[2]];
        // Calculate the center point p=(v0+v1+v2)/3
        float p[3];
        p[0] = (v0.x+v1.x+v2.x)/3.0f;
        p[1] = (v0.y+v1.y+v2.y)/3.0f;
        p[2] = (v0.z+v1.z+v2.z)/3.0f;
        glVertex3fv(p);
      }
    }
    glEnd();
  }
}

/** Sets the point indices, the geometric points and the normal for each triangle.
 * If the number of triangles exceeds \a MaximumTriangles then only a triangulation of
 * the bounding box is filled in instead. This is due to performance issues.
 * \see createTriangleDetail().
 */
void SoFCMeshNode::generatePrimitives(SoAction* action)
{
  if (_mesh)
  {
    // Use the data structure directly and not through MeshFacetIterator as this
    // class is quite slowly (at least for rendering)
    const MeshCore::MeshPointArray& rPoints = _mesh->getMesh().GetPoints();
    const MeshCore::MeshFacetArray& rFacets = _mesh->getMesh().GetFacets();

    // In case we have too many triangles we just fill in some dummy triangles that
    // define the bounding box
    if ( this->MaximumTriangles < rFacets.size() ) {
      const Base::BoundBox3f& cBox = _mesh->getMesh().GetBoundBox();
      int triangles[36] = {
        0,1,2,0,2,3,
        0,1,5,0,5,4,
        0,4,7,0,7,3,
        6,7,4,6,4,5,
        6,2,3,6,3,7,
        6,1,2,6,5,1
      };
      SbVec3f points[8] = {
        SbVec3f(cBox.MinX,cBox.MinY,cBox.MinZ),
        SbVec3f(cBox.MaxX,cBox.MinY,cBox.MinZ),
        SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MinZ),
        SbVec3f(cBox.MinX,cBox.MaxY,cBox.MinZ),
        SbVec3f(cBox.MinX,cBox.MinY,cBox.MaxZ),
        SbVec3f(cBox.MaxX,cBox.MinY,cBox.MaxZ),
        SbVec3f(cBox.MaxX,cBox.MaxY,cBox.MaxZ),
        SbVec3f(cBox.MinX,cBox.MaxY,cBox.MaxZ)
      };

      SoPrimitiveVertex vertex;
      beginShape(action, TRIANGLES, 0);
      int i=0;
      while ( i<36 ) 
      {
        if (i<6)
          vertex.setNormal(SbVec3f(0.0f,0.0f,-1.0f));
        else if (i<12)
          vertex.setNormal(SbVec3f(0.0f,-1.0f,0.0f));
        else if (i<18)
          vertex.setNormal(SbVec3f(-1.0f,0.0f,0.0f));
        else if (i<24)
          vertex.setNormal(SbVec3f(0.0f,0.0f,1.0f));
        else if (i<30)
          vertex.setNormal(SbVec3f(0.0f,1.0f,0.0f));
        else
          vertex.setNormal(SbVec3f(1.0f,0.0f,0.0f));
        vertex.setPoint( points[triangles[i++]] );
        shapeVertex(&vertex);
        vertex.setPoint( points[triangles[i++]] );
        shapeVertex(&vertex);
        vertex.setPoint( points[triangles[i++]] );
        shapeVertex(&vertex);
      }
      endShape();
    } else {
      // Create the information when moving over or picking into the scene
      SoPrimitiveVertex vertex;
      SoPointDetail pointDetail;
      SoFaceDetail faceDetail;

      vertex.setDetail(&pointDetail);

      beginShape(action, TRIANGLES, &faceDetail);
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
}

/**
 * If the number of triangles exceeds \a MaximumTriangles 0 is returned. This means that the client programmer needs to implement itself to get the
 * index of the picked triangle. If the number of triangles doesn't exceed \a MaximumTriangles SoShape::createTriangleDetail() gets called.
 * Against the default OpenInventor implementation which returns 0 as well Coin3d fills in the point and face indices.
 */
SoDetail * SoFCMeshNode::createTriangleDetail(SoRayPickAction * action,
                                              const SoPrimitiveVertex * v1,
                                              const SoPrimitiveVertex * v2,
                                              const SoPrimitiveVertex * v3,
                                              SoPickedPoint * pp)
{
  if ( this->MaximumTriangles < countTriangles() ) {
    return 0;
  } else {
    SoDetail* detail = inherited::createTriangleDetail(action, v1, v2, v3, pp);
    return detail;
  }
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoFCMeshNode::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
  // Get the bbox directly from the mesh kernel
  if (countTriangles() > 0) {
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

/**
 * Adds the number of the triangles to the \a SoGetPrimitiveCountAction.
 */
void SoFCMeshNode::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  if (!this->shouldPrimitiveCount(action)) return;
  action->addNumTriangles(countTriangles());
}

/**
 * Counts the number of triangles. If a mesh is not set yet it returns 0.
 */
unsigned int SoFCMeshNode::countTriangles() const
{
  return (_mesh ? _mesh->getMesh().CountFacets() : 0);
}

/**
 * Writes out the mesh node.
 */
void SoFCMeshNode::write( SoWriteAction* action ) 
{ 
  SoOutput * out = action->getOutput();

  if (out->getStage() == SoOutput::COUNT_REFS) {
    this->addWriteReference(out, FALSE);
  }
  else if (out->getStage() == SoOutput::WRITE) {
    const MeshCore::MeshPointArray& rPoints = _mesh->getMesh().GetPoints();
    const MeshCore::MeshFacetArray& rFacets = _mesh->getMesh().GetFacets();
    if (this->writeHeader(out, FALSE, FALSE)) return;
    point.setNum(rPoints.size());
    unsigned int pos=0;
    for (MeshCore::MeshPointArray::_TConstIterator cP=rPoints.begin(); cP!=rPoints.end(); ++cP)
      point.set1Value(pos++,cP->x,cP->y,cP->z);
    coordIndex.setNum(3*rFacets.size());
    pos=0;
    for (MeshCore::MeshFacetArray::_TConstIterator cF=rFacets.begin(); cF!=rFacets.end(); ++cF){
      coordIndex.set1Value(pos++,cF->_aulPoints[0]);
      coordIndex.set1Value(pos++,cF->_aulPoints[1]);
      coordIndex.set1Value(pos++,cF->_aulPoints[2]);
    }
    this->getFieldData()->write(out, this);
    this->writeFooter(out);
    point.deleteValues(0);
    coordIndex.deleteValues(0);
  }
}

/**
 * Reads in the mesh node from the input stream.
 */
SbBool SoFCMeshNode::readInstance( SoInput* in, unsigned short  flags )
{
  SbBool ret = inherited::readInstance(in, flags);

  MeshCore::MeshPointArray cPoints;
  cPoints.resize(point.getNum());
  for (int i=0; i<point.getNum(); ++i) {
    const SbVec3f& pt = point[i];
    cPoints[i].Set(pt[0],pt[1],pt[2]);
  }

  MeshCore::MeshFacetArray cFacets;
  cFacets.resize(coordIndex.getNum()/3);
  unsigned long k=0;
  for (int j=0; j<coordIndex.getNum(); ++k) {
    cFacets[k]._aulPoints[0] = coordIndex[j++];
    cFacets[k]._aulPoints[1] = coordIndex[j++];
    cFacets[k]._aulPoints[2] = coordIndex[j++];
  }

  point.deleteValues(0);
  coordIndex.deleteValues(0);

  MeshCore::MeshKernel* kernel = new MeshCore::MeshKernel;
  kernel->Adopt(cPoints, cFacets, true);

  Mesh::Feature* mesh = new Mesh::Feature;
  mesh->Mesh.setValue(*kernel);
  _mesh=mesh;
  return ret;
}

