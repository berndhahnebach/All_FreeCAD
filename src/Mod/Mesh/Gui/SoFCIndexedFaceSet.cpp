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
# ifdef FC_OS_MACOSX
# include <OpenGL/gl.h>
# else
# include <GL/gl.h>
# endif
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/bundles/SoMaterialBundle.h>
# include <Inventor/elements/SoCoordinateElement.h>
# include <Inventor/elements/SoGLCoordinateElement.h>
# include <Inventor/elements/SoMaterialBindingElement.h>
#endif

#include <Gui/SoFCInteractiveElement.h>
#include "SoFCIndexedFaceSet.h"

using namespace MeshGui;


SO_NODE_SOURCE(SoFCIndexedFaceSet);

void SoFCIndexedFaceSet::initClass()
{
    SO_NODE_INIT_CLASS(SoFCIndexedFaceSet, SoIndexedFaceSet, "IndexedFaceSet");
}

SoFCIndexedFaceSet::SoFCIndexedFaceSet() : renderTriangleLimit(100000)
{
    SO_NODE_CONSTRUCTOR(SoFCIndexedFaceSet);
    setName(SoFCIndexedFaceSet::getClassTypeId().getName());
}

/**
 * Either renders the complete mesh or only a subset of the points.
 */
void SoFCIndexedFaceSet::GLRender(SoGLRenderAction *action)
{
    if (this->coordIndex.getNum() < 3)
        return;

    if (!this->shouldGLRender(action))
        return;

    SoState * state = action->getState();
    SbBool mode = Gui::SoFCInteractiveElement::get(state);

    unsigned int num = this->coordIndex.getNum()/4;
    if (mode == false || num <= this->renderTriangleLimit) {
        inherited::GLRender(action);
    }
    else {
        SoMaterialBindingElement::Binding matbind =
            SoMaterialBindingElement::get(state);
        SbBool overall = (matbind == SoMaterialBindingElement::OVERALL);

        const SoCoordinateElement * coords;
        const SbVec3f * normals;
        const int32_t * cindices;
        int numindices;
        const int32_t * nindices;
        const int32_t * tindices;
        const int32_t * mindices;
        SbBool normalCacheUsed;

        SoMaterialBundle mb(action);

        SoTextureCoordinateBundle tb(action, TRUE, FALSE);
        SbBool sendNormals = !mb.isColorOnly() || tb.isFunction();

        this->getVertexData(state, coords, normals, cindices,
                            nindices, tindices, mindices, numindices,
                            sendNormals, normalCacheUsed);

        mb.sendFirst(); // make sure we have the correct material

        drawCoords(static_cast<const SoGLCoordinateElement*>(coords), cindices, numindices,
                   normals, nindices, &mb, mindices, overall, &tb, tindices);
        // Disable caching for this node
        SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
}

void SoFCIndexedFaceSet::drawCoords(const SoGLCoordinateElement * const vertexlist,
                                    const int32_t *vertexindices,
                                    int numindices,
                                    const SbVec3f *normals,
                                    const int32_t *normalindices,
                                    SoMaterialBundle *materials,
                                    const int32_t *matindices,
                                    const SbBool overall,
                                    const SoTextureCoordinateBundle * const texcoords,
                                    const int32_t *texindices)
{
    const SbVec3f * coords3d = 0;
    coords3d = vertexlist->getArrayPtr3();

    int mod = numindices/(4*this->renderTriangleLimit)+1;
    float size = std::min<float>((float)mod,3.0f);
    glPointSize(size);

    int ct=0;
    const int32_t *viptr = vertexindices;
    int32_t v1, v2, v3;
    SbVec3f dummynormal(0,0,1);
    const SbVec3f *currnormal = &dummynormal;
    if (normals) currnormal = normals;

    glBegin(GL_POINTS);
    for (int index=0; index<numindices; ct++) {
        if (ct%mod==0) {
            v1 = *viptr++; index++;
            if (!overall)
                materials->send(v1, TRUE);
            if (normals)
                currnormal = &normals[*normalindices++];
            glNormal3fv((const GLfloat*)currnormal);
            glVertex3fv((const GLfloat*)(coords3d + v1));

            v2 = *viptr++; index++;
            if (!overall)
                materials->send(v2, TRUE);
            if (normals)
                currnormal = &normals[*normalindices++];
            glNormal3fv((const GLfloat*)currnormal);
            glVertex3fv((const GLfloat*)(coords3d + v2));

            v3 = *viptr++; index++;
            if (!overall)
                materials->send(v3, TRUE);
            if (normals)
                currnormal = &normals[*normalindices++];
            glNormal3fv((const GLfloat*)currnormal);
            glVertex3fv((const GLfloat*)(coords3d + v3));
        }
        else {
            viptr++; index++; normalindices++;
            viptr++; index++; normalindices++;
            viptr++; index++; normalindices++;
        }

        viptr++; index++; normalindices++;
    }
    glEnd();
}
