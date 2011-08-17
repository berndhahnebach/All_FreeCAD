/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <float.h>
# include <algorithm>
# include <Inventor/actions/SoCallbackAction.h>
# include <Inventor/actions/SoGetBoundingBoxAction.h>
# include <Inventor/actions/SoGetPrimitiveCountAction.h>
# include <Inventor/actions/SoGLRenderAction.h>
# include <Inventor/actions/SoPickAction.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/bundles/SoMaterialBundle.h>
# include <Inventor/bundles/SoTextureCoordinateBundle.h>
# include <Inventor/elements/SoCoordinateElement.h>
# include <Inventor/elements/SoGLCoordinateElement.h>
# include <Inventor/elements/SoGLCacheContextElement.h>
# include <Inventor/errors/SoReadError.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/details/SoLineDetail.h>
# include <Inventor/misc/SoState.h>
#endif

#include "SoBrepShape.h"

using namespace PartGui;


SO_NODE_SOURCE(SoBrepFaceSet);

void SoBrepFaceSet::initClass()
{
    SO_NODE_INIT_CLASS(SoBrepFaceSet, SoIndexedFaceSet, "IndexedFaceSet");
}

SoBrepFaceSet::SoBrepFaceSet()
{
    SO_NODE_CONSTRUCTOR(SoBrepFaceSet);
    SO_NODE_ADD_FIELD(partIndex, (-1));
}

/**
 * Renders the control points.
 */
void SoBrepFaceSet::GLRender(SoGLRenderAction *action)
{
    if (this->coordIndex.getNum() < 3)
        return;

    if (!this->shouldGLRender(action))
        return;

    SoState * state = action->getState();

    Binding mbind = this->findMaterialBinding(state);
    Binding nbind = this->findNormalBinding(state);

    const SoCoordinateElement * coords;
    const SbVec3f * normals;
    const int32_t * cindices;
    int numindices;
    const int32_t * nindices;
    const int32_t * tindices;
    const int32_t * mindices;
    const int32_t * pindices;
    int numparts;
    SbBool doTextures;
    SbBool normalCacheUsed;

    SoMaterialBundle mb(action);

    SoTextureCoordinateBundle tb(action, TRUE, FALSE);
    doTextures = tb.needCoordinates();
    SbBool sendNormals = !mb.isColorOnly() || tb.isFunction();

    this->getVertexData(state, coords, normals, cindices,
                        nindices, tindices, mindices, numindices,
                        sendNormals, normalCacheUsed);

    mb.sendFirst(); // make sure we have the correct material

    // just in case someone forgot
    if (!mindices) mindices = cindices;
    if (!nindices) nindices = cindices;
    pindices = this->partIndex.getValues(0);
    numparts = this->partIndex.getNum();
    renderShape(static_cast<const SoGLCoordinateElement*>(coords), cindices, numindices,
        pindices, numparts, normals, nindices, &mb, mindices, &tb, tindices, nbind, mbind, doTextures?1:0);
    // Disable caching for this node
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
}

void SoBrepFaceSet::renderShape(const SoGLCoordinateElement * const vertexlist,
                                const int32_t *vertexindices,
                                int numindices,
                                const int32_t *partindices,
                                int num_partindices,
                                const SbVec3f *normals,
                                const int32_t *normalindices,
                                SoMaterialBundle *const materials,
                                const int32_t *matindices,
                                SoTextureCoordinateBundle * const texcoords,
                                const int32_t *texindices,
                                const int nbind,
                                const int mbind,
                                const int texture)
{
    int texidx = 0;

    const SbVec3f * coords3d = NULL;
    coords3d = vertexlist->getArrayPtr3();

    const int32_t *viptr = vertexindices;
    const int32_t *vistartptr = vertexindices;
    const int32_t *viendptr = viptr + numindices;
    const int32_t *piptr = partindices;
    const int32_t *piendptr = piptr + num_partindices;
    int32_t v1, v2, v3, v4, pi;
    SbVec3f dummynormal(0,0,1);
    int numverts = vertexlist->getNum();

    const SbVec3f *currnormal = &dummynormal;
    if (normals) currnormal = normals;

    int matnr = 0;
    int trinr = 0;
    pi = piptr < piendptr ? *piptr++ : -1;

    glBegin(GL_TRIANGLES);
    while (viptr + 2 < viendptr) {
        v1 = *viptr++;
        v2 = *viptr++;
        v3 = *viptr++;

        // This test is for robustness upon buggy data sets
        if (v1 < 0 || v2 < 0 || v3 < 0 ||
            v1 >= numverts || v2 >= numverts || v3 >= numverts) {
            break;
        }
        v4 = viptr < viendptr ? *viptr++ : -1;

        /* vertex 1 *********************************************************/
        if (mbind == PER_PART) {
            if (trinr == 0)
                materials->send(matnr++, TRUE);
        }
        else if (mbind == PER_PART_INDEXED) {
            if (trinr == 0)
                materials->send(*matindices++, TRUE);
        }
        else if (mbind == PER_VERTEX || mbind == PER_FACE) {
            materials->send(matnr++, TRUE);
        }
        else if (mbind == PER_VERTEX_INDEXED || mbind == PER_FACE_INDEXED) {
            materials->send(*matindices++, TRUE);
        }

        if (nbind == PER_VERTEX || nbind == PER_FACE) {
            currnormal = normals++;
            glNormal3fv((const GLfloat*)currnormal);
        }
        else if (nbind == PER_VERTEX_INDEXED || nbind == PER_FACE_INDEXED) {
            currnormal = &normals[*normalindices++];
            glNormal3fv((const GLfloat*)currnormal);
        }

        if (texture) {
            texcoords->send(texindices ? *texindices++ : texidx++,
                        vertexlist->get3(v1),
                        *currnormal);
        }

        glVertex3fv((const GLfloat*) (coords3d + v1));

        /* vertex 2 *********************************************************/
        if (mbind == PER_VERTEX)
            materials->send(matnr++, TRUE);
        else if (mbind == PER_VERTEX_INDEXED)
            materials->send(*matindices++, TRUE);

        if (nbind == PER_VERTEX) {
            currnormal = normals++;
            glNormal3fv((const GLfloat*)currnormal);
        }
        else if (nbind == PER_VERTEX_INDEXED) {
            currnormal = &normals[*normalindices++];
            glNormal3fv((const GLfloat*)currnormal);
        }

        if (texture) {
            texcoords->send(texindices ? *texindices++ : texidx++,
                            vertexlist->get3(v2),
                            *currnormal);
        }

        glVertex3fv((const GLfloat*) (coords3d + v2));

        /* vertex 3 *********************************************************/
        if (mbind == PER_VERTEX)
            materials->send(matnr++, TRUE);
        else if (mbind == PER_VERTEX_INDEXED)
            materials->send(*matindices++, TRUE);

        if (nbind == PER_VERTEX) {
            currnormal = normals++;
            glNormal3fv((const GLfloat*)currnormal);
        }
        else if (nbind == PER_VERTEX_INDEXED) {
            currnormal = &normals[*normalindices++];
            glNormal3fv((const GLfloat*)currnormal);
        }

        if (texture) {
            texcoords->send(texindices ? *texindices++ : texidx++,
                            vertexlist->get3(v3),
                            *currnormal);
        }

        glVertex3fv((const GLfloat*) (coords3d + v3));

        if (mbind == PER_VERTEX_INDEXED)
            matindices++;

        if (nbind == PER_VERTEX_INDEXED)
            normalindices++;

        if (texture && texindices) {
            texindices++;
        }

        trinr++;
        if (pi == trinr) {
            pi = piptr < piendptr ? *piptr++ : -1;
            trinr = 0;
        }
    }
    glEnd();
}

SoDetail * SoBrepFaceSet::createTriangleDetail(SoRayPickAction * action,
                                               const SoPrimitiveVertex * v1,
                                               const SoPrimitiveVertex * v2,
                                               const SoPrimitiveVertex * v3,
                                               SoPickedPoint * pp)
{
    SoDetail* detail = inherited::createTriangleDetail(action, v1, v2, v3, pp);
    const int32_t * indices = this->partIndex.getValues(0);
    int num = this->partIndex.getNum();
    if (indices) {
        SoFaceDetail* face_detail = static_cast<SoFaceDetail*>(detail);
        int index = face_detail->getFaceIndex();
        int count = 0;
        for (int i=0; i<num; i++) {
            count += indices[i];
            if (index < count) {
                face_detail->setPartIndex(i);
                break;
            }
        }
    }
    return detail;
}

SoBrepFaceSet::Binding
SoBrepFaceSet::findMaterialBinding(SoState * const state) const
{
    Binding binding = OVERALL;
    SoMaterialBindingElement::Binding matbind =
        SoMaterialBindingElement::get(state);

    switch (matbind) {
    case SoMaterialBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoMaterialBindingElement::PER_VERTEX:
        binding = PER_VERTEX;
        break;
    case SoMaterialBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoMaterialBindingElement::PER_PART:
        binding = PER_PART;
        break;
    case SoMaterialBindingElement::PER_FACE:
        binding = PER_FACE;
        break;
    case SoMaterialBindingElement::PER_PART_INDEXED:
        binding = PER_PART_INDEXED;
        break;
    case SoMaterialBindingElement::PER_FACE_INDEXED:
        binding = PER_FACE_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}

SoBrepFaceSet::Binding
SoBrepFaceSet::findNormalBinding(SoState * const state) const
{
    Binding binding = PER_VERTEX_INDEXED;
    SoNormalBindingElement::Binding normbind =
        (SoNormalBindingElement::Binding) SoNormalBindingElement::get(state);

    switch (normbind) {
    case SoNormalBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoNormalBindingElement::PER_VERTEX:
        binding = PER_VERTEX;
        break;
    case SoNormalBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoNormalBindingElement::PER_PART:
        binding = PER_PART;
        break;
    case SoNormalBindingElement::PER_FACE:
        binding = PER_FACE;
        break;
    case SoNormalBindingElement::PER_PART_INDEXED:
        binding = PER_PART_INDEXED;
        break;
    case SoNormalBindingElement::PER_FACE_INDEXED:
        binding = PER_FACE_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}

// ---------------------------------------------------------------------

SO_NODE_SOURCE(SoBrepEdgeSet);

void SoBrepEdgeSet::initClass()
{
    SO_NODE_INIT_CLASS(SoBrepEdgeSet, SoIndexedLineSet, "IndexedLineSet");
}

SoBrepEdgeSet::SoBrepEdgeSet()
{
    SO_NODE_CONSTRUCTOR(SoBrepEdgeSet);
    SO_NODE_ADD_FIELD(partIndex, (-1));
}

/**
 * Renders the control points.
 */
void SoBrepEdgeSet::GLRender(SoGLRenderAction *action)
{
    if (this->coordIndex.getNum() < 2)
        return;

    if (!this->shouldGLRender(action))
        return;

    SoState * state = action->getState();

    Binding mbind = this->findMaterialBinding(state);
    Binding nbind = this->findNormalBinding(state);

    const SoCoordinateElement * coords;
    const SbVec3f * normals;
    const int32_t * cindices;
    int numindices;
    const int32_t * nindices;
    const int32_t * tindices;
    const int32_t * mindices;
    const int32_t * pindices;
    int numparts;
    SbBool doTextures;
    SbBool normalCacheUsed;

    SoMaterialBundle mb(action);

    SoTextureCoordinateBundle tb(action, TRUE, FALSE);
    doTextures = tb.needCoordinates();
    SbBool sendNormals = !mb.isColorOnly() || tb.isFunction();

    this->getVertexData(state, coords, normals, cindices,
                        nindices, tindices, mindices, numindices,
                        sendNormals, normalCacheUsed);

    mb.sendFirst(); // make sure we have the correct material

    // just in case someone forgot
    if (!mindices) mindices = cindices;
    if (!nindices) nindices = cindices;
    pindices = this->partIndex.getValues(0);
    numparts = this->partIndex.getNum();
    //renderShape(static_cast<const SoGLCoordinateElement*>(coords), cindices, numindices,
    //    pindices, numparts, normals, nindices, &mb, mindices, &tb, tindices, nbind, mbind, doTextures?1:0);
    // Disable caching for this node
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DONT_AUTO_CACHE);
}

SoDetail * SoBrepEdgeSet::createLineSegmentDetail(SoRayPickAction * action,
                                                  const SoPrimitiveVertex * v1,
                                                  const SoPrimitiveVertex * v2,
                                                  SoPickedPoint * pp)
{
    SoDetail* detail = inherited::createLineSegmentDetail(action, v1, v2, pp);
    const int32_t * indices = this->partIndex.getValues(0);
    int num = this->partIndex.getNum();
    if (indices) {
        SoLineDetail* line_detail = static_cast<SoLineDetail*>(detail);
        int index = line_detail->getLineIndex();
        int count = 0;
        for (int i=0; i<num; i++) {
            count += indices[i];
            if (index < count) {
                line_detail->setPartIndex(i);
                break;
            }
        }
    }
    return detail;
}

SoBrepEdgeSet::Binding
SoBrepEdgeSet::findMaterialBinding(SoState * const state) const
{
    Binding binding = OVERALL;
    SoMaterialBindingElement::Binding matbind =
        SoMaterialBindingElement::get(state);

    switch (matbind) {
    case SoMaterialBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoMaterialBindingElement::PER_VERTEX:
        binding = PER_VERTEX;
        break;
    case SoMaterialBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoMaterialBindingElement::PER_PART:
        binding = PER_PART;
        break;
    case SoMaterialBindingElement::PER_PART_INDEXED:
        binding = PER_PART_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}

SoBrepEdgeSet::Binding
SoBrepEdgeSet::findNormalBinding(SoState * const state) const
{
    Binding binding = PER_VERTEX_INDEXED;
    SoNormalBindingElement::Binding normbind =
        (SoNormalBindingElement::Binding) SoNormalBindingElement::get(state);

    switch (normbind) {
    case SoNormalBindingElement::OVERALL:
        binding = OVERALL;
        break;
    case SoNormalBindingElement::PER_VERTEX:
        binding = PER_VERTEX;
        break;
    case SoNormalBindingElement::PER_VERTEX_INDEXED:
        binding = PER_VERTEX_INDEXED;
        break;
    case SoNormalBindingElement::PER_PART:
        binding = PER_PART;
        break;
    case SoNormalBindingElement::PER_PART_INDEXED:
        binding = PER_PART_INDEXED;
        break;
    default:
        break;
    }
    return binding;
}
