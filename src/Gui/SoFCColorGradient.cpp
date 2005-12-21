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
# include <qstring.h>
#endif

#include "SoFCColorGradient.h"

#include <Inventor/nodes/SoText2.h>

using namespace Gui;

SO_NODE_SOURCE(SoFCColorGradient);

/*!
  Constructor.
*/
SoFCColorGradient::SoFCColorGradient() : _fPosX(4.0f), _fPosY(4.0f), _bOutInvisible(false)
{
  SO_NODE_CONSTRUCTOR(SoFCColorGradient);
  _cColGrad.setStyle(App::ColorGradient::FLOW);
  coords = new SoCoordinate3;
  coords->ref();
  labels = new SoSeparator;
  labels->ref();

  setColorModel( App::ColorGradient::TRIA );
}

/*!
  Destructor.
*/
SoFCColorGradient::~SoFCColorGradient()
{
  //delete THIS;
  coords->unref();
  labels->unref();
}

// doc from parent
void SoFCColorGradient::initClass(void)
{
  SO_NODE_INIT_CLASS(SoFCColorGradient,SoSeparator,"Separator");
}

void SoFCColorGradient::setMarkerLabel( const SoMFString& label )
{
  labels->removeAllChildren();

  int num = label.getNum();
  if ( num > 1 )
  {
    float fStep = 8.0f / ((float)num-1);
    SoTransform* trans = new SoTransform;
    trans->translation.setValue(_fPosX+0.1f,_fPosY-0.05f+fStep,0.0f);
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

void SoFCColorGradient::setViewerSize( const SbVec2s& size )
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

  _fPosX = fMaxX;
  _fPosY = fMaxY;

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

  // set the vertices spanning the faces for the color gradient
  int ct = coords->point.getNum()/2;
  for ( int j=0; j<ct; j++ )
  {
    float w = (float)j/(float)(ct-1);
    float fPosY = (1.0f-w)*fMaxY + w*fMinY;
  	coords->point.set1Value(2*j, fMinX, fPosY, 0.0f);
  	coords->point.set1Value(2*j+1, fMaxX, fPosY, 0.0f);
  }
}

void SoFCColorGradient::setRange( float fMin, float fMax, int prec )
{
  _cColGrad.setRange(fMin, fMax);

  SoMFString label;
  QString s;
  int ticks = 9;

  // the middle of the bar is zero
  if ( fMin < 0.0f && fMax > 0.0f && _cColGrad.getStyle() == App::ColorGradient::ZERO_BASED )
  {
    if ( ticks % 2 == 0) ticks++;
    int half = ticks / 2;
    for (int j=0; j<half+1; j++)
    {
      float w = (float)j/((float)half);
      float fValue = (1.0f-w)*fMax;
      label.set1Value(j, s.setNum(fValue, 'f', prec).latin1() );
    }
    for (int k=half+1; k<ticks; k++)
    {
      float w = (float)(k-half+1)/((float)(ticks-half));
      float fValue = w*fMin;
      label.set1Value(k, s.setNum(fValue, 'f', prec).latin1() );
    }
  }
  else // either not zero based or 0 is not in between [fMin,fMax]
  {
    for (int j=0; j<ticks; j++)
    {
      float w = (float)j/((float)ticks-1.0f);
      float fValue = (1.0f-w)*fMax+w*fMin;
      label.set1Value(j, s.setNum(fValue, 'f', prec).latin1() );
    }
  }

  setMarkerLabel( label );
}

void SoFCColorGradient::setColorModel( App::ColorGradient::TColorModel tModel )
{
  _cColGrad.setColorModel( tModel );
  rebuild();
}

void SoFCColorGradient::setColorStyle (App::ColorGradient::TStyle tStyle)
{
  _cColGrad.setStyle( tStyle );
  rebuild();
}

void SoFCColorGradient::rebuild()
{
  App::ColorModel model = _cColGrad.getColorModel();
  int uCtColors = (int)model._usColors;

  // don't know why the parameter range isn't between [-1,+1]
  float fMinX=  4.0f, fMaxX=4.5f;
  float fMinY= -4.0f, fMaxY=4.0f;
  coords->point.setNum(2*uCtColors);
  for ( int i=0; i<uCtColors; i++ )
  {
    float w = (float)i/(float)(uCtColors-1);
    float fPosY = (1.0f-w)*fMaxY + w*fMinY;
  	coords->point.set1Value(2*i, fMinX, fPosY, 0.0f);
  	coords->point.set1Value(2*i+1, fMaxX, fPosY, 0.0f);
  }

  // for uCtColors colors we need 2*(uCtColors-1) facets and therefore an array with
  // 8*(uCtColors-1) face indices
  SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
  faceset->coordIndex.setNum(8*(uCtColors-1));
  for ( int j=0; j<uCtColors-1; j++ )
  {
    faceset->coordIndex.set1Value(8*j,   2*j);
    faceset->coordIndex.set1Value(8*j+1, 2*j+3);
    faceset->coordIndex.set1Value(8*j+2, 2*j+1);
    faceset->coordIndex.set1Value(8*j+3, SO_END_FACE_INDEX);
    faceset->coordIndex.set1Value(8*j+4, 2*j);
    faceset->coordIndex.set1Value(8*j+5, 2*j+2);
    faceset->coordIndex.set1Value(8*j+6, 2*j+3);
    faceset->coordIndex.set1Value(8*j+7, SO_END_FACE_INDEX);
  }

  SoMaterial* mat = new SoMaterial;
  mat->transparency = 0.3f;
  mat->diffuseColor.setNum(2*uCtColors);
  for ( int k=0; k<uCtColors; k++ )
  {
    App::Color col = model._pclColors[uCtColors-k-1];
    mat->diffuseColor.set1Value(2*k, col.r, col.g, col.b);
    mat->diffuseColor.set1Value(2*k+1, col.r, col.g, col.b);
  }

  SoMaterialBinding* matBinding = new SoMaterialBinding;
  matBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;

  // first clear the children
  if ( getNumChildren() > 0 )
    removeAllChildren();
  addChild(labels);
	addChild(coords);
  addChild(mat);
  addChild(matBinding);
	addChild(faceset);
}

bool SoFCColorGradient::isVisible (float fVal) const
{
  if (_bOutInvisible == true)
  {
    float fMin, fMax;
    _cColGrad.getRange(fMin, fMax);
    if ((fVal > fMax) || (fVal < fMin))
      return false;
    else
      return true;
  }

  return true;
}
