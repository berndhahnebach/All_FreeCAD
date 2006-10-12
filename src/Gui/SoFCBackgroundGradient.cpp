/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <Inventor/SbViewportRegion.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDirectionalLight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoTransform.h>
#endif

#include "SoFCBackgroundGradient.h"

#include <Inventor/nodes/SoText2.h>
#define new DEBUG_CLIENTBLOCK
using namespace Gui;

SO_NODE_SOURCE(SoFCBackgroundGradient);

/*!
  Constructor.
*/
SoFCBackgroundGradient::SoFCBackgroundGradient()
{
  SO_NODE_CONSTRUCTOR(SoFCBackgroundGradient);

  // points
  SbVec3f* vertices = new SbVec3f[9];
  float fMinX= -0.5f, fMaxX=0.5f, fAvgX=0.0f;
  float fMinY= -0.5f, fMaxY=0.5f, fAvgY=0.0f;
  vertices[0].setValue( fMinX, fMaxY, 0.0f);
  vertices[1].setValue( fAvgX, fMaxY, 0.0f);
  vertices[2].setValue( fMaxX, fMaxY, 0.0f);
  vertices[3].setValue( fMinX, fAvgY, 0.0f);
  vertices[4].setValue( fAvgX, fAvgY, 0.0f);
  vertices[5].setValue( fMaxX, fAvgY, 0.0f);
  vertices[6].setValue( fMinX, fMinY, 0.0f);
  vertices[7].setValue( fAvgX, fMinY, 0.0f);
  vertices[8].setValue( fMaxX, fMinY, 0.0f);

  coords = new SoCoordinate3;
  coords->ref();
	coords->point.setValues(0,9, vertices);

  float colors[9][3] =
  {  
    { 0.5f, 0.5f, 0.8f}, { 0.5f, 0.5f, 0.8f}, { 0.5f, 0.5f, 0.8f}, 
    { 0.7f, 0.7f, 0.9f}, { 0.7f, 0.7f, 0.9f}, { 0.7f, 0.7f, 0.9f}, 
    { 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f, 1.0f}, 
  };

  SoMaterial* mat = new SoMaterial;
  mat->diffuseColor.setValues(0, 9, colors);
  mat->transparency = 0.0f;

  SoMaterialBinding* matBinding = new SoMaterialBinding;
  matBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;

  // face indices
  int32_t face_idx[32]=
  {
    0,3,1,SO_END_FACE_INDEX, 4,1,3,SO_END_FACE_INDEX,
    1,4,2,SO_END_FACE_INDEX, 5,2,4,SO_END_FACE_INDEX,
    3,6,4,SO_END_FACE_INDEX, 7,4,6,SO_END_FACE_INDEX,
    4,7,5,SO_END_FACE_INDEX, 8,5,7,SO_END_FACE_INDEX,
  };

  SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
	faceset->coordIndex.setValues(0,32,(const int32_t*) face_idx);

	addChild(new SoDirectionalLight);
	addChild(coords);
  addChild(mat);
  addChild(matBinding);
	addChild(faceset);
  delete [] vertices;
}

/*!
  Destructor.
*/
SoFCBackgroundGradient::~SoFCBackgroundGradient()
{
  //delete THIS;
  coords->unref();
}

// doc from parent
void SoFCBackgroundGradient::initClass(void)
{
  SO_NODE_INIT_CLASS(SoFCBackgroundGradient,SoSeparator,"Separator");
}

void SoFCBackgroundGradient::GLRenderBelowPath ( SoGLRenderAction *action )
{
  const SbViewportRegion& vp = action->getViewportRegion();
  const SbVec2s&  size = vp.getWindowSize();
  if ( size != viewsize )
  {
    setViewportSize(size);
    viewsize = size;
  }

  inherited::GLRenderBelowPath(action);
}

void SoFCBackgroundGradient::setViewportSize( const SbVec2s& size )
{
  float fRatio = ((float)size[0])/((float)size[1]);
  float fMinX= -0.5f, fMaxX=0.5f, fAvgX=0.0f;
  float fMinY= -0.5f, fMaxY=0.5f, fAvgY=0.0f;

  if ( fRatio > 1.0f )
  {
    fMinX = - 0.5f * fRatio;
    fMaxX =   0.5f * fRatio;
  }
  else if ( fRatio < 1.0f )
  {
    fMinX = - 0.5f / fRatio;
    fMaxX =   0.5f / fRatio;
    fMinY = - 0.5f / fRatio;
    fMaxY =   0.5f / fRatio;
  }

  SbVec3f* vertices = new SbVec3f[9];
  vertices[0].setValue( fMinX, fMaxY, 0.0f );
  vertices[1].setValue( fAvgX, fMaxY, 0.0f );
  vertices[2].setValue( fMaxX, fMaxY, 0.0f );
  vertices[3].setValue( fMinX, fAvgY, 0.0f );
  vertices[4].setValue( fAvgX, fAvgY, 0.0f );
  vertices[5].setValue( fMaxX, fAvgY, 0.0f );
  vertices[6].setValue( fMinX, fMinY, 0.0f );
  vertices[7].setValue( fAvgX, fMinY, 0.0f );
  vertices[8].setValue( fMaxX, fMinY, 0.0f );
	coords->point.setValues(0,9, vertices);
  delete [] vertices;
}

void SoFCBackgroundGradient::setColorGradient( const SbColor& fromColor, const SbColor& toColor )
{
  SoNode* child = getChild( 2 );
  if ( child->getTypeId() == SoMaterial::getClassTypeId() )
  {
    const float* rgb1 = fromColor.getValue();
    const float* rgb2 = toColor.getValue();
    SbColor mid( 0.5f*(rgb1[0]+rgb2[0]), 0.5f*(rgb1[1]+rgb2[1]), 0.5f*(rgb1[2]+rgb2[2]) );
    SoMaterial* mat = reinterpret_cast<SoMaterial*>(child);
    mat->diffuseColor.set1Value( 0, fromColor );
    mat->diffuseColor.set1Value( 1, fromColor );
    mat->diffuseColor.set1Value( 2, fromColor );
    mat->diffuseColor.set1Value( 3, mid );
    mat->diffuseColor.set1Value( 4, mid );
    mat->diffuseColor.set1Value( 5, mid );
    mat->diffuseColor.set1Value( 6, toColor );
    mat->diffuseColor.set1Value( 7, toColor );
    mat->diffuseColor.set1Value( 8, toColor );
  }
}
