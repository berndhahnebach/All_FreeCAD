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
# include <Inventor/details/SoLineDetail.h>
# include <Inventor/details/SoPointDetail.h>
# include <Inventor/elements/SoGLCacheContextElement.h>
# include <Inventor/elements/SoLazyElement.h>
# include <Inventor/elements/SoLightModelElement.h>
# include <Inventor/misc/SoState.h>
#endif

#include <Gui/SoFCInteractiveElement.h>
#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/MeshIO.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include "SoFCMeshFaceSet.h"
#include "SoFCMeshVertex.h"

using namespace MeshGui;


// Defines all required member variables and functions for a
// single-value field
SO_SFIELD_SOURCE(SoSFMeshFacetArray, MeshCore::MeshFacetArray*, MeshCore::MeshFacetArray*);


void SoSFMeshFacetArray::initClass()
{
   // This macro takes the name of the class and the name of the
   // parent class
   SO_SFIELD_INIT_CLASS(SoSFMeshFacetArray, SoSField);
}

void SoSFMeshFacetArray::setValue(const MeshCore::MeshFacetArray& p)
{
  SoSFMeshFacetArray::setValue(const_cast<MeshCore::MeshFacetArray*>(&p));
}

// This reads the value of a field from a file. It returns FALSE if the value could not be read
// successfully.
SbBool SoSFMeshFacetArray::readValue(SoInput *in)
{
  // This macro is convenient for reading with error detection.
#define READ_VAL(val) \
  if (!in->read(val)) { \
    SoReadError::post(in, "Premature end of file"); \
    return FALSE; \
  }

  value = new MeshCore::MeshFacetArray();

  // ** Binary format ******************************************************
  if (in->isBinary()) {
    int numtoread;
    READ_VAL(numtoread);

    // Sanity checking on the value, to avoid barfing on corrupt
    // files.
    if (numtoread < 0) {
      SoReadError::post(in, "invalid number of values in field: %d",
                        numtoread);
      return FALSE;
    }

    value->resize(numtoread);
    if (!this->readBinaryValues(in, numtoread)) { return FALSE; }
  }

  // ** ASCII format *******************************************************
  else {
    char c;
    READ_VAL(c);
    if (c == '[') {
      unsigned long currentidx = 0;

      READ_VAL(c);
      if (c == ']') {
        // Zero values -- done. :^)
      }
      else {
        in->putBack(c);

        while (TRUE) {
          // makeRoom() makes sure the allocation strategy is decent.
          if (currentidx >= value->size()) value->resize(currentidx + 1);

          if (!this->read1Value(in, currentidx++)) return FALSE;

          READ_VAL(c);
          if (c == ',') { READ_VAL(c); } // Treat trailing comma as whitespace.

          // That was the last array element, we're done.
          if (c == ']') { break; }

          if (c == '}') {
            SoReadError::post(in, "Premature end of array, got '%c'", c);
            return FALSE;
          }

          in->putBack(c);
        }
      }

      // Fit array to number of items.
      value->resize(currentidx);
    }
    else {
      in->putBack(c);
      value->resize(1);
      if (!this->read1Value(in, 0)) return FALSE;
    }
  }

#undef READ_VAL

  // We need to trigger the notification chain here, as this function
  // can be used on a node in a scene graph in any state -- not only
  // during initial scene graph import.
  this->valueChanged();
  
  return TRUE;
}

SbBool SoSFMeshFacetArray::readBinaryValues(SoInput * in, unsigned long numarg)
{
  assert(in->isBinary());
  assert(numarg >= 0);

  for (unsigned long i=0; i < numarg; i++) if (!this->read1Value(in, i)) return FALSE;
  return TRUE;
}

SbBool SoSFMeshFacetArray::read1Value(SoInput * in, unsigned long idx)
{
  assert(idx < value->size());
  MeshCore::MeshFacet& v = (*value)[idx];
  int32_t p0, p1, p2;
  int32_t n0, n1, n2;
  SbBool ret = (in->read(p0) && in->read(p1) && in->read(p2));
  ret &= (in->read(n0) && in->read(n1) && in->read(n2));
  if ( ret ) {
    v._aulPoints[0] = p0;
    v._aulPoints[1] = p1;
    v._aulPoints[2] = p2;
    v._aulNeighbours[0] = n0;
    v._aulNeighbours[1] = n1;
    v._aulNeighbours[2] = n2;
  }

  return ret;
}

int SoSFMeshFacetArray::getNumValuesPerLine() const
{
  return 1;
}

// This writes the value of a field to a file.
void SoSFMeshFacetArray::writeValue(SoOutput *out) const
{
  if (out->isBinary()) {
    this->writeBinaryValues(out);
    return;
  }

  const unsigned long count = value->size();
  if ((count > 1) || (count == 0)) out->write("[ ");

  out->incrementIndent();

  for (unsigned long i=0; i < count; i++) {
    this->write1Value(out, i);

    if (i != count-1) {
      if (((i+1) % this->getNumValuesPerLine()) == 0) {
        out->write(",\n");
        out->indent();
        // for alignment
        out->write("  ");
      }
      else {
        out->write(", ");
      }
    }
  }
  if ((count > 1) || (count == 0)) out->write(" ]");

  out->decrementIndent();
}

void SoSFMeshFacetArray::writeBinaryValues(SoOutput * out) const
{
  assert(out->isBinary());

  const unsigned int count = (unsigned int)value->size();
  out->write(count);
  for (unsigned int i=0; i < count; i++) this->write1Value(out, i);
}

void SoSFMeshFacetArray::write1Value(SoOutput * out, unsigned long idx) const
{
  const MeshCore::MeshFacet& v = (*value)[idx];
  out->write((int32_t)(v._aulPoints[0]));
  if (!out->isBinary()) out->write(' ');
  out->write((int32_t)(v._aulPoints[1]));
  if (!out->isBinary()) out->write(' ');
  out->write((int32_t)(v._aulPoints[2]));
  if (!out->isBinary()) out->write(' ');
  out->write((int32_t)(v._aulNeighbours[0]));
  if (!out->isBinary()) out->write(' ');
  out->write((int32_t)(v._aulNeighbours[1]));
  if (!out->isBinary()) out->write(' ');
  out->write((int32_t)(v._aulNeighbours[2]));
}

// -------------------------------------------------------

SO_ELEMENT_SOURCE(SoFCMeshFacetElement);

void SoFCMeshFacetElement::initClass()
{
   SO_ELEMENT_INIT_CLASS(SoFCMeshFacetElement, inherited);
}

void SoFCMeshFacetElement::init(SoState * state)
{
  inherited::init(state);
  this->coordIndex = 0;
}

SoFCMeshFacetElement::~SoFCMeshFacetElement()
{
}

void SoFCMeshFacetElement::set(SoState * const state, SoNode * const node, const MeshCore::MeshFacetArray * const coords)
{
  SoFCMeshFacetElement * elem = (SoFCMeshFacetElement *)
    SoReplacedElement::getElement(state, classStackIndex, node);
  if (elem) {
    elem->coordIndex = coords;
    elem->nodeId = node->getNodeId();
  }
}

const MeshCore::MeshFacetArray * SoFCMeshFacetElement::get(SoState * const state)
{
  return SoFCMeshFacetElement::getInstance(state)->coordIndex;
}

const SoFCMeshFacetElement * SoFCMeshFacetElement::getInstance(SoState * state)
{
  return (const SoFCMeshFacetElement *) SoElement::getConstElement(state, classStackIndex);
}

void SoFCMeshFacetElement::print(FILE * /* file */) const
{
}

// -------------------------------------------------------

SO_NODE_SOURCE(SoFCMeshFacet);

/*!
  Constructor.
*/
SoFCMeshFacet::SoFCMeshFacet(void)
{
  SO_NODE_CONSTRUCTOR(SoFCMeshFacet);

  SO_NODE_ADD_FIELD(coordIndex, (0));
}

/*!
  Destructor.
*/
SoFCMeshFacet::~SoFCMeshFacet()
{
}

// Doc from superclass.
void SoFCMeshFacet::initClass(void)
{
  SO_NODE_INIT_CLASS(SoFCMeshFacet, SoNode, "Node");

  SO_ENABLE(SoGetBoundingBoxAction, SoFCMeshFacetElement);
  SO_ENABLE(SoGLRenderAction, SoFCMeshFacetElement);
  SO_ENABLE(SoPickAction, SoFCMeshFacetElement);
  SO_ENABLE(SoCallbackAction, SoFCMeshFacetElement);
  SO_ENABLE(SoGetPrimitiveCountAction, SoFCMeshFacetElement);
}

// Doc from superclass.
void SoFCMeshFacet::doAction(SoAction * action)
{
  SoFCMeshFacetElement::set(action->getState(), this, coordIndex.getValue());
}

// Doc from superclass.
void SoFCMeshFacet::GLRender(SoGLRenderAction * action)
{
  SoFCMeshFacet::doAction(action);
}

// Doc from superclass.
void SoFCMeshFacet::callback(SoCallbackAction * action)
{
  SoFCMeshFacet::doAction(action);
}

// Doc from superclass.
void SoFCMeshFacet::pick(SoPickAction * action)
{
  SoFCMeshFacet::doAction(action);
}

// Doc from superclass.
void SoFCMeshFacet::getBoundingBox(SoGetBoundingBoxAction * action)
{
  SoFCMeshFacet::doAction(action);
}

// Doc from superclass.
void SoFCMeshFacet::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  SoFCMeshFacet::doAction(action);
}

// -------------------------------------------------------

/**
 * class SoFCMeshFaceSet
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

SO_NODE_SOURCE(SoFCMeshFaceSet);

void SoFCMeshFaceSet::initClass()
{
  SO_NODE_INIT_CLASS(SoFCMeshFaceSet, SoShape, "Shape");
}

SoFCMeshFaceSet::SoFCMeshFaceSet() : MaximumTriangles(500000)
{
  SO_NODE_CONSTRUCTOR(SoFCMeshFaceSet);
}

void SoFCMeshFaceSet::notify(SoNotList * node)
{
  SoShape::notify(node);
}

/**
 * Either renders the complete mesh or only a subset of the points.
 */
void SoFCMeshFaceSet::GLRender(SoGLRenderAction *action)
{
  if (shouldGLRender(action))
  {
    SoState*  state = action->getState();

    SbBool mode = Gui::SoFCInteractiveElement::get(state);
    const MeshCore::MeshPointArray * coords = SoFCMeshVertexElement::get(state);
    const MeshCore::MeshFacetArray * index = SoFCMeshFacetElement::get(state);

    SoMaterialBundle mb(action);
    //SoTextureCoordinateBundle tb(action, true, false);

    SbBool needNormals = !mb.isColorOnly()/* || tb.isFunction()*/;
    mb.sendFirst();  // make sure we have the correct material

    if ( mode == false || index->size() <= this->MaximumTriangles )
      drawFaces(coords, index, needNormals);
    else
      drawPoints(coords, index, needNormals);

    // Disable caching for this node
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
  }
}

/**
 * Renders the triangles of the complete mesh.
 */
void SoFCMeshFaceSet::drawFaces(const MeshCore::MeshPointArray * rPoints, 
                                const MeshCore::MeshFacetArray* rFacets, SbBool needNormals) const
{
  if (needNormals)
  {
    glBegin(GL_TRIANGLES);
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it )
    {
      const MeshCore::MeshPoint& v0 = (*rPoints)[it->_aulPoints[0]];
      const MeshCore::MeshPoint& v1 = (*rPoints)[it->_aulPoints[1]];
      const MeshCore::MeshPoint& v2 = (*rPoints)[it->_aulPoints[2]];

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
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it )
    {
      glVertex((*rPoints)[it->_aulPoints[0]]);
      glVertex((*rPoints)[it->_aulPoints[1]]);
      glVertex((*rPoints)[it->_aulPoints[2]]);
    }
    glEnd();
  }
}

/**
 * Renders the gravity points of a subset of triangles.
 */
void SoFCMeshFaceSet::drawPoints(const MeshCore::MeshPointArray * rPoints, 
                                 const MeshCore::MeshFacetArray* rFacets, SbBool needNormals) const
{
  int mod = rFacets->size()/MaximumTriangles+1;

  float size = std::min<float>((float)mod,3.0f);
  glPointSize(size);

  if (needNormals)
  {
    glBegin(GL_POINTS);
    int ct=0;
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it, ct++ )
    {
      if ( ct%mod==0 ) {
        const MeshCore::MeshPoint& v0 = (*rPoints)[it->_aulPoints[0]];
        const MeshCore::MeshPoint& v1 = (*rPoints)[it->_aulPoints[1]];
        const MeshCore::MeshPoint& v2 = (*rPoints)[it->_aulPoints[2]];

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
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it, ct++ )
    {
      if (ct%mod==0) {
        const MeshCore::MeshPoint& v0 = (*rPoints)[it->_aulPoints[0]];
        const MeshCore::MeshPoint& v1 = (*rPoints)[it->_aulPoints[1]];
        const MeshCore::MeshPoint& v2 = (*rPoints)[it->_aulPoints[2]];
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
 * a rough model is filled in instead. This is due to performance issues.
 * \see createTriangleDetail().
 */
void SoFCMeshFaceSet::generatePrimitives(SoAction* action)
{
  SoState*  state = action->getState();
  const MeshCore::MeshPointArray * rPoints = SoFCMeshVertexElement::get(state);
  const MeshCore::MeshFacetArray * rFacets = SoFCMeshFacetElement::get(state);

  // In case we have too many triangles we just create a rough model of the original mesh
  if ( this->MaximumTriangles < rFacets->size() ) {
  /*  //FIXME: We should notify this shape when the data has changed.
    //Just counting the number of triangles won't always work.
    if ( rFacets->size() != _ctPrimitives ) {
      _ctPrimitives = rFacets->size();
      createRoughModel(false);
    }
    SoPrimitiveVertex vertex;
    beginShape(action, TRIANGLES, 0);
    int i=0;
    while ( i<coordIndex.getNum() )
    {
      const SbVec3f& v0 = point[coordIndex[i++]]; 
      const SbVec3f& v1 = point[coordIndex[i++]]; 
      const SbVec3f& v2 = point[coordIndex[i++]]; 

      // Calculate the normal n = (v1-v0)x(v2-v0)
      SbVec3f n;
      n[0] = (v1[1]-v0[1])*(v2[2]-v0[2])-(v1[2]-v0[2])*(v2[1]-v0[1]);
      n[1] = (v1[2]-v0[2])*(v2[0]-v0[0])-(v1[0]-v0[0])*(v2[2]-v0[2]);
      n[2] = (v1[0]-v0[0])*(v2[1]-v0[1])-(v1[1]-v0[1])*(v2[0]-v0[0]);

      // Set the normal
      vertex.setNormal(n);

      vertex.setPoint( v0 );
      shapeVertex(&vertex);
      vertex.setPoint( v1 );
      shapeVertex(&vertex);
      vertex.setPoint( v2 );
      shapeVertex(&vertex);
    }
    endShape();*/
  } else {
    // Create the information when moving over or picking into the scene
    SoPrimitiveVertex vertex;
    SoPointDetail pointDetail;
    SoFaceDetail faceDetail;

    vertex.setDetail(&pointDetail);

    beginShape(action, TRIANGLES, &faceDetail);
    for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it )
    {
      const MeshCore::MeshPoint& v0 = (*rPoints)[it->_aulPoints[0]];
      const MeshCore::MeshPoint& v1 = (*rPoints)[it->_aulPoints[1]];
      const MeshCore::MeshPoint& v2 = (*rPoints)[it->_aulPoints[2]];

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

/**
 * If the number of triangles exceeds \a MaximumTriangles 0 is returned. This means that the client programmer needs to implement itself to get the
 * index of the picked triangle. If the number of triangles doesn't exceed \a MaximumTriangles SoShape::createTriangleDetail() gets called.
 * Against the default OpenInventor implementation which returns 0 as well Coin3d fills in the point and face indices.
 */
SoDetail * SoFCMeshFaceSet::createTriangleDetail(SoRayPickAction * action,
                                              const SoPrimitiveVertex * v1,
                                              const SoPrimitiveVertex * v2,
                                              const SoPrimitiveVertex * v3,
                                              SoPickedPoint * pp)
{
  if ( this->MaximumTriangles < countTriangles(action) ) {
    return 0;
  } else {
    SoDetail* detail = inherited::createTriangleDetail(action, v1, v2, v3, pp);
    return detail;
  }
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoFCMeshFaceSet::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
  SoState*  state = action->getState();
  const MeshCore::MeshPointArray * rPoints = SoFCMeshVertexElement::get(state);
  if (rPoints && rPoints->size() > 0) {
    Base::BoundBox3f cBox;
    for ( MeshCore::MeshPointArray::_TConstIterator it = rPoints->begin(); it != rPoints->end(); ++it )
      cBox &= (*it);
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
void SoFCMeshFaceSet::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  if (!this->shouldPrimitiveCount(action)) return;
  SoState*  state = action->getState();
  const MeshCore::MeshFacetArray * coordIndex = SoFCMeshFacetElement::get(state);
  action->addNumTriangles(coordIndex->size());
}

/**
 * Counts the number of triangles. If a mesh is not set yet it returns 0.
 */
unsigned int SoFCMeshFaceSet::countTriangles(SoAction * action) const
{
  SoState*  state = action->getState();
  const MeshCore::MeshFacetArray * coordIndex = SoFCMeshFacetElement::get(state);
  return coordIndex->size();
}

// -------------------------------------------------------

SO_NODE_SOURCE(SoFCMeshOpenEdgeSet);

void SoFCMeshOpenEdgeSet::initClass()
{
  SO_NODE_INIT_CLASS(SoFCMeshOpenEdgeSet, SoShape, "Shape");
}

SoFCMeshOpenEdgeSet::SoFCMeshOpenEdgeSet()
{
  SO_NODE_CONSTRUCTOR(SoFCMeshOpenEdgeSet);
}

/**
 * Renders the open edges only.
 */
void SoFCMeshOpenEdgeSet::GLRender(SoGLRenderAction *action)
{
  if (shouldGLRender(action))
  {
    SoState*  state = action->getState();
    const MeshCore::MeshPointArray * coords = SoFCMeshVertexElement::get(state);
    const MeshCore::MeshFacetArray * index = SoFCMeshFacetElement::get(state);

    SoMaterialBundle mb(action);
    SoTextureCoordinateBundle tb(action, TRUE, FALSE);
    SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);
    mb.sendFirst();  // make sure we have the correct material

    drawLines(coords,index);

    // Disable caching for this node
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
  }
}

/**
 * Renders the triangles of the complete mesh.
 */
void SoFCMeshOpenEdgeSet::drawLines(const MeshCore::MeshPointArray * rPoints, 
                                    const MeshCore::MeshFacetArray* rFacets) const
{
  // Use the data structure directly and not through MeshFacetIterator as this
  // class is quite slowly (at least for rendering)
  glBegin(GL_LINES);
  for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it ) {
    for ( int i=0; i<3; i++ ) {
      if ( it->_aulNeighbours[i] == ULONG_MAX ) {
        glVertex((*rPoints)[it->_aulPoints[i]]);
        glVertex((*rPoints)[it->_aulPoints[(i+1)%3]]);
      }
    }
  }

  glEnd();
}

void SoFCMeshOpenEdgeSet::generatePrimitives(SoAction* action)
{
  // do not create primitive information as an SoFCMeshFaceSet should already be used that delivers the information
  SoState*  state = action->getState();
  const MeshCore::MeshPointArray * rPoints = SoFCMeshVertexElement::get(state);
  const MeshCore::MeshFacetArray * rFacets = SoFCMeshFacetElement::get(state);

  // Create the information when moving over or picking into the scene
  SoPrimitiveVertex vertex;
  SoPointDetail pointDetail;
  SoLineDetail lineDetail;

  vertex.setDetail(&pointDetail);

  beginShape(action, LINES, &lineDetail);
  for ( MeshCore::MeshFacetArray::_TConstIterator it = rFacets->begin(); it != rFacets->end(); ++it )
  {
    for ( int i=0; i<3; i++ ) {
      if ( it->_aulNeighbours[i] == ULONG_MAX ) {
        const MeshCore::MeshPoint& v0 = (*rPoints)[it->_aulPoints[i]];
        const MeshCore::MeshPoint& v1 = (*rPoints)[it->_aulPoints[(i+1)%3]];

        // Vertex 0
        pointDetail.setCoordinateIndex(it->_aulPoints[i]);
        vertex.setPoint(sbvec3f(v0));
        shapeVertex(&vertex);

        // Vertex 1
        pointDetail.setCoordinateIndex(it->_aulPoints[(i+1)%3]);
        vertex.setPoint(sbvec3f(v1));
        shapeVertex(&vertex);

        // Increment for the next open edge
        lineDetail.incLineIndex();
      }
    }
  }

  endShape();
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoFCMeshOpenEdgeSet::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
  SoState*  state = action->getState();
  const MeshCore::MeshPointArray * rPoints = SoFCMeshVertexElement::get(state);
  if (rPoints && rPoints->size() > 0) {
    Base::BoundBox3f cBox;
    for ( MeshCore::MeshPointArray::_TConstIterator it = rPoints->begin(); it != rPoints->end(); ++it )
      cBox &= (*it);
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
void SoFCMeshOpenEdgeSet::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  if (!this->shouldPrimitiveCount(action)) return;
  SoState*  state = action->getState();
  const MeshCore::MeshFacetArray * rFaces = SoFCMeshFacetElement::get(state);
  
  // Count number of open edges first
  int ctEdges=0;
  for ( MeshCore::MeshFacetArray::_TConstIterator jt = rFaces->begin(); jt != rFaces->end(); ++jt ) {
    for ( int i=0; i<3; i++ ) {
      if ( jt->_aulNeighbours[i] == ULONG_MAX ) {
        ctEdges++;
      }
    }
  }

  action->addNumLines(ctEdges);
}
