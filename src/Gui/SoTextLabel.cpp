/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QFontMetrics>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/bundles/SoMaterialBundle.h>
# include <Inventor/elements/SoLazyElement.h>
# include <Inventor/misc/SoState.h>
#endif

#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoCullElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>

#include "SoTextLabel.h"

using namespace Gui;

/*!
\code

s="""
  #Inventor V2.1 ascii

  Annotation {
    Translation { translation 4 0 0 }
    FontStyle {
        size 20
        style BOLD
    }
    BaseColor {
        rgb 0.0 0.0 0.0
    }


    SoTextLabel { string ["Text label", "Second line"] backgroundColor 1.0 0.447059 0.337255}
  }
"""

App.ActiveDocument.addObject("App::InventorObject","iv").Buffer=s

\encode
*/

SO_NODE_SOURCE(SoTextLabel);

void SoTextLabel::initClass()
{
    SO_NODE_INIT_CLASS(SoTextLabel, SoText2, "Text2");
}

SoTextLabel::SoTextLabel()
{
    SO_NODE_CONSTRUCTOR(SoTextLabel);
    SO_NODE_ADD_FIELD(backgroundColor, (SbVec3f(1.0f,1.0f,1.0f)));
    SO_NODE_ADD_FIELD(background, (TRUE));
    SO_NODE_ADD_FIELD(frameSize, (10.0f));
}

/**
 * Renders the label.
 */
void SoTextLabel::GLRender(SoGLRenderAction *action)
{
    if (!this->shouldGLRender(action)) return;

    // only draw text without background
    if (!this->background.getValue()) {
        inherited::GLRender(action);
        return;
    }

    SoState * state = action->getState();

    state->push();
    SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);

    SbBox3f box;
    SbVec3f center;
    this->computeBBox(action, box, center);

    if (!SoCullElement::cullTest(state, box, TRUE)) {
        SoMaterialBundle mb(action);
        mb.sendFirst();
        const SbMatrix & mat = SoModelMatrixElement::get(state);
        //const SbViewVolume & vv = SoViewVolumeElement::get(state);
        const SbMatrix & projmatrix = (mat * SoViewingMatrixElement::get(state) *
                                       SoProjectionMatrixElement::get(state));
        const SbViewportRegion & vp = SoViewportRegionElement::get(state);
        SbVec2s vpsize = vp.getViewportSizePixels();

        // font stuff
        float space = this->spacing.getValue();
        float fontsize = SoFontSizeElement::get(state);
        SbName fontname = SoFontNameElement::get(state);
        int lines = this->string.getNum();

        // get left bottom corner of the label
        SbVec3f nilpoint(0.0f, 0.0f, 0.0f);
        projmatrix.multVecMatrix(nilpoint, nilpoint);
        nilpoint[0] = (nilpoint[0] + 1.0f) * 0.5f * vpsize[0];
        nilpoint[1] = (nilpoint[1] + 1.0f) * 0.5f * vpsize[1];

#if 1
        // Unfortunately, the size of the label is stored in the pimpl class of
        // SoText2 which cannot be accessed directly. However, there is a trick
        // to get the required information: set model, viewing and projection
        // matrix to the identity matrix and also view volume to some default
        // values. SoText2::computeBBox() then calls SoText2P::getQuad which
        // returns the sizes in form of the bounding box. These values can be
        // reverse-engineered to get width and height.
        state->push();
        SoModelMatrixElement::set(state,this,SbMatrix::identity());
        SoViewingMatrixElement::set(state,this,SbMatrix::identity());
        SoProjectionMatrixElement::set(state,this,SbMatrix::identity());
        SbViewVolume vv;
        vv.ortho(-1,1,-1,1,-1,1);
        SoViewVolumeElement::set(state,this,vv);

        SbBox3f box;
        SbVec3f center;
        this->computeBBox(action, box, center);
        state->pop();

        float xmin,ymin,zmin,xmax,ymax,zmax;
        box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
        SbVec3f v0(xmin,ymax,zmax);
        SbVec3f v1(xmax,ymax,zmax);
        SbVec3f v2(xmax,ymin,zmax);
        SbVec3f v3(xmin,ymin,zmax);
        vv.projectToScreen(v0,v0);
        vv.projectToScreen(v1,v1);
        vv.projectToScreen(v2,v2);
        vv.projectToScreen(v3,v3);

        float width,height;
        width  = (v1[0]-v0[0])*vpsize[0];
        height = (v1[1]-v3[1])*vpsize[1];
        switch (this->justification.getValue()) {
        case SoText2::RIGHT:
            nilpoint[0] -= width;
            break;
        case SoText2::CENTER:
            nilpoint[0] -= 0.5f*width;
            break;
        default:
            break;
        }

        if (lines > 1) {
            nilpoint[1] -= (float(lines-1)/(float)lines*height);
        }
#else
        // Unfortunately, the required size (in pixels) is stored in a non-accessible way
        // in the subclass SoText2. Thus, we try to get a satisfactory solution with Qt 
        // methods.
        // The font name is of the form "family:style". If 'style' is given it can be
        // 'Bold', 'Italic' or 'Bold Italic'.
        QFont font;
        QString fn = QString::fromAscii(fontname.getString());
        int pos = fn.indexOf(QLatin1Char(':'));
        if (pos > -1) {
            if (fn.indexOf(QLatin1String("Bold"),pos,Qt::CaseInsensitive) > pos)
                font.setBold(true);
            if (fn.indexOf(QLatin1String("Italic"),pos,Qt::CaseInsensitive) > pos)
                font.setItalic(true);
            fn = fn.left(pos);
        }
        font.setFamily(fn);
        font.setPixelSize((int)fontsize);
        QFontMetrics fm(font);

        float width = 0.0f;
        float height = 0.75f*fontsize*lines + (lines-1)*space;//fm.height();
        float hh=0;
        for (int i = 0; i < lines; i++) {
            SbString str = this->string[i];
            float w = fm.width(QLatin1String(this->string[i].getString()));
            width = std::max<float>(width, w);
            hh = fm.height();
        }

        if (lines > 1) {
            nilpoint[1] -= ((lines-1)*fontsize*0.75f+space);
        }
#endif

        SbVec3f toppoint = nilpoint;
        toppoint[0] += width;
        toppoint[1] += height;

        // Set new state.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, vpsize[0], 0, vpsize[1], -1.0f, 1.0f);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);

        state->push();

        // disable textures for all units
        SoGLTextureEnabledElement::set(state, this, FALSE);
        SoGLTexture3EnabledElement::set(state, this, FALSE);

        glPushAttrib(GL_ENABLE_BIT | GL_PIXEL_MODE_BIT | GL_COLOR_BUFFER_BIT);
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

        // color and frame size
        SbColor color = this->backgroundColor.getValue();
        float fs = this->frameSize.getValue();

        // draw background
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_QUADS);
        glVertex3f(nilpoint[0]-fs,nilpoint[1]-fs,0.0f);
        glVertex3f(toppoint[0]+fs,nilpoint[1]-fs,0.0f);
        glVertex3f(toppoint[0]+fs,toppoint[1]+fs,0.0f);
        glVertex3f(nilpoint[0]-fs,toppoint[1]+fs,0.0f);
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
    }

    state->pop();

    inherited::GLRender(action);
}
