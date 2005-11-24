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
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoTransform.h>
#endif

#include "SoFCColorLegend.h"

#include <Inventor/nodes/SoText2.h>

using namespace Gui;

SO_NODE_SOURCE(SoFCColorLegend);

/*!
  Constructor.
*/
SoFCColorLegend::SoFCColorLegend()
{
  SO_NODE_CONSTRUCTOR(SoFCColorLegend);

  // don't know why the parameter range isn't between [-1,+1]
  SbVec3f* vertices = new SbVec3f[10];
  float fMinX=  4.0f, fMaxX=4.5f;
  float fMinY= -4.0f, fMaxY=4.0f;
  float fHigY = 0.75f*fMaxY+0.25f*fMinY;
  float fAvgY = 0.50f*fMaxY+0.50f*fMinY;
  float fLowY = 0.25f*fMaxY+0.75f*fMinY;
  vertices[0].setValue( fMinX, fMaxY, 0.0f);
  vertices[1].setValue( fMaxX, fMaxY, 0.0f);
  vertices[2].setValue( fMinX, fHigY, 0.0f);
  vertices[3].setValue( fMaxX, fHigY, 0.0f);
  vertices[4].setValue( fMinX, fAvgY, 0.0f);
  vertices[5].setValue( fMaxX, fAvgY, 0.0f);
  vertices[6].setValue( fMinX, fLowY, 0.0f);
  vertices[7].setValue( fMaxX, fLowY, 0.0f);
  vertices[8].setValue( fMinX, fMinY, 0.0f);
  vertices[9].setValue( fMaxX, fMinY, 0.0f);
  // face indices
  int32_t idx2[32]=
  {
    0,3,1,SO_END_FACE_INDEX,
    0,2,3,SO_END_FACE_INDEX,
    2,5,3,SO_END_FACE_INDEX,
    2,4,5,SO_END_FACE_INDEX,
    4,7,5,SO_END_FACE_INDEX,
    4,6,7,SO_END_FACE_INDEX,
    6,9,7,SO_END_FACE_INDEX,
    6,8,9,SO_END_FACE_INDEX,
  };

  coords = new SoCoordinate3;
  coords->ref();
	coords->point.setValues(0,10, vertices);

  float colors[10][3] =
  {  
    { 1, 0, 0}, { 1, 0, 0}, // red
    { 1, 1, 0}, { 1, 1, 0}, // yellow
    { 0, 1, 0}, { 0, 1 ,0}, // green
    { 0, 1 ,1}, { 0 ,1, 1}, // cyan
    { 0 ,0, 1}, { 0 ,0, 1}, // blue
  };

  SoMaterial* mat = new SoMaterial;
  mat->diffuseColor.setValues(0, 10, colors);
  mat->transparency = 0.3f;

  SoMaterialBinding* matBinding = new SoMaterialBinding;
  matBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;

  SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
	faceset->coordIndex.setValues(0,32,(const int*) idx2);

  labels = new SoSeparator;
  addChild(labels);
	addChild(coords);
  addChild(mat);
  addChild(matBinding);
	addChild(faceset);
  delete [] vertices;
}

/*!
  Destructor.
*/
SoFCColorLegend::~SoFCColorLegend()
{
  //delete THIS;
  coords->unref();
}

// doc from parent
void SoFCColorLegend::initClass(void)
{
  SO_NODE_INIT_CLASS(SoFCColorLegend,SoSeparator,"Separator");
}

void SoFCColorLegend::setMarkerLabel( const SoMFString& label )
{
  labels->removeAllChildren();

  int num = label.getNum();
  if ( num > 1 )
  {
    float fStep = 8.0f / ((float)num-1);
    SoTransform* trans = new SoTransform;
    trans->translation.setValue(4.6f,3.95f+fStep,0.0f);
    labels->addChild(trans);

    for ( int i=0; i<num; i++ )
    {
      SoTransform* trans = new SoTransform;
      SoBaseColor* color = new SoBaseColor;
      SoText2    * text2 = new SoText2;

      trans->translation.setValue(0,-fStep,0);
      color->rgb.setValue(0,0,0);
      text2->string.setValue( label[i] );
      labels->addChild(trans);
      labels->addChild(color);
      labels->addChild(text2);
    }
  }
}

void SoFCColorLegend::setViewerSize( const SbVec2s& size )
{
  float fRatio = ((float)size[0])/((float)size[1]);
  float fMinX=  4.0f, fMaxX=4.5f;
  float fMinY= -4.0f, fMaxY=4.0f;

  if ( fRatio > 1.0f )
  {
    fMinX = 4.0f * fRatio;
    fMaxX = fMinX+0.5f;
  }
  else if ( fRatio < 1.0f )
  {
    fMinY =  -4.0f / fRatio;
    fMaxY =   4.0f / fRatio;
  }

  float fHigY = 0.75f*fMaxY+0.25f*fMinY;
  float fAvgY = 0.50f*fMaxY+0.50f*fMinY;
  float fLowY = 0.25f*fMaxY+0.75f*fMinY;

  // search for the labels
  int num=0;
  for ( int i=0; i<labels->getNumChildren(); i++ )
  {
    if ( labels->getChild(i)->getTypeId() == SoTransform::getClassTypeId() )
      num++;
  }

  if ( num > 2 )
  {
    bool first=true;
    float fStep = (fMaxY-fMinY) / ((float)num-2);

    for ( int j=0; j<labels->getNumChildren(); j++ )
    {
      if ( labels->getChild(j)->getTypeId() == SoTransform::getClassTypeId() )
      {
        if ( first )
        {
          first = false;
          reinterpret_cast<SoTransform*>(labels->getChild(j))->translation.setValue(fMaxX+0.1f,fMaxY-0.05f+fStep,0.0f);
        }
        else
        {
          reinterpret_cast<SoTransform*>(labels->getChild(j))->translation.setValue(0,-fStep,0.0f);
        }
      }
    }
  }

  // search for the coordinates
  SoNode* node = getChild(1);
  if ( node && node->getTypeId() == SoCoordinate3::getClassTypeId() ) 
  {
    SoCoordinate3* coord = reinterpret_cast<SoCoordinate3*>(node);
    SbVec3f* vertices = new SbVec3f[10];
    vertices[0].setValue( fMinX, fMaxY, 0.0f);
    vertices[1].setValue( fMaxX, fMaxY, 0.0f);
    vertices[2].setValue( fMinX, fHigY, 0.0f);
    vertices[3].setValue( fMaxX, fHigY, 0.0f);
    vertices[4].setValue( fMinX, fAvgY, 0.0f);
    vertices[5].setValue( fMaxX, fAvgY, 0.0f);
    vertices[6].setValue( fMinX, fLowY, 0.0f);
    vertices[7].setValue( fMaxX, fLowY, 0.0f);
    vertices[8].setValue( fMinX, fMinY, 0.0f);
    vertices[9].setValue( fMaxX, fMinY, 0.0f);
	  coord->point.setValues(0,10, vertices);
    delete [] vertices;
  }
}
