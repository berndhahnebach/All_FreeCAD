/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# ifdef FC_OS_MACOSX
# include <OpenGL/gl.h>
# else
# include <GL/gl.h>
# endif
# include <cfloat>
# include <algorithm>
# include <Inventor/actions/SoCallbackAction.h>
# include <Inventor/actions/SoGetBoundingBoxAction.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/bundles/SoMaterialBundle.h>
# include <Inventor/bundles/SoTextureCoordinateBundle.h>
# include <Inventor/elements/SoCoordinateElement.h>
# include <Inventor/elements/SoGLCacheContextElement.h>
# include <Inventor/misc/SoState.h>
#endif

#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>

#include "SoTextLabel.h"

using namespace Gui;


SO_NODE_SOURCE(SoTextLabel);

void SoTextLabel::initClass()
{
    SO_NODE_INIT_CLASS(SoTextLabel, SoText2, "Text2");
}

SoTextLabel::SoTextLabel()
{
    SO_NODE_CONSTRUCTOR(SoTextLabel);
}

/**
 * Renders the label.
 */
void SoTextLabel::GLRender(SoGLRenderAction *action)
{
    if (!this->shouldGLRender(action)) return;

    SoState * state = action->getState();

    state->push();
    SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);

    SoMaterialBundle mb(action);
    mb.sendFirst();
    SbVec3f nilpoint(0.0f, 0.0f, 0.0f);
    const SbMatrix & mat = SoModelMatrixElement::get(state);
    const SbViewVolume & vv = SoViewVolumeElement::get(state);
    const SbMatrix & projmatrix = (mat * SoViewingMatrixElement::get(state) *
                                   SoProjectionMatrixElement::get(state));
    const SbViewportRegion & vp = SoViewportRegionElement::get(state);
    SbVec2s vpsize = vp.getViewportSizePixels();

    projmatrix.multVecMatrix(nilpoint, nilpoint);
    nilpoint[0] = (nilpoint[0] + 1.0f) * 0.5f * vpsize[0];
    nilpoint[1] = (nilpoint[1] + 1.0f) * 0.5f * vpsize[1];      
 
    // Set new state.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, vpsize[0], 0, vpsize[1], -1.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    float fontsize = SoFontSizeElement::get(state);
    float xpos = nilpoint[0];      // to get rid of compiler warning..
    float ypos = nilpoint[1];
    int ix=0, iy=0;

    // get the current glColor
    float ftglColor[4];
    glGetFloatv(GL_CURRENT_COLOR, ftglColor);

    unsigned char red   = (unsigned char) (ftglColor[0] * 255.0f);
    unsigned char green = (unsigned char) (ftglColor[1] * 255.0f);
    unsigned char blue  = (unsigned char) (ftglColor[2] * 255.0f);

    state->push();
    
    // disable textures for all units
    SoGLTextureEnabledElement::set(state, this, FALSE);
    SoGLTexture3EnabledElement::set(state, this, FALSE);

    glPushAttrib(GL_ENABLE_BIT | GL_PIXEL_MODE_BIT | GL_COLOR_BUFFER_BIT);
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    SbBool didenableblend = FALSE;

    glColor3f(1.0f, 0.447059f, 0.337255f);
    glBegin(GL_QUADS);
    glVertex2i(0,0);
    glVertex2i(100,0);
    glVertex2i(100,100);
    glVertex2i(0,100);
    glEnd();



    // pop old state
    glPopClientAttrib();
    glPopAttrib();
    state->pop();
      
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    // Pop old GL matrix state.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  
  state->pop();





    //if (!this->shouldGLRender(action)) return;

    inherited::GLRender(action);
}

/**
 * Sets the bounding box of the mesh to \a box and its center to \a center.
 */
void SoTextLabel::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
{
    inherited::computeBBox(action, box, center);

    myBox = box;
    myCenter = center;
}
