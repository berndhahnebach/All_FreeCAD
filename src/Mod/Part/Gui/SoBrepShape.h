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

#ifndef PARTGUI_SOBREPSHAPE_H
#define PARTGUI_SOBREPSHAPE_H

#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSubField.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/elements/SoReplacedElement.h>

class SoGLCoordinateElement;
class SoTextureCoordinateBundle;

namespace PartGui {

class AppPartGuiExport SoBrepFaceSet : public SoIndexedFaceSet {
    typedef SoIndexedFaceSet inherited;

    SO_NODE_HEADER(SoBrepFaceSet);

public:
    static void initClass();
    SoBrepFaceSet();

    SoMFInt32 partIndex;

protected:
    virtual ~SoBrepFaceSet() {};
    virtual void GLRender(SoGLRenderAction *action);
    virtual SoDetail * createTriangleDetail(
        SoRayPickAction * action,
        const SoPrimitiveVertex * v1,
        const SoPrimitiveVertex * v2,
        const SoPrimitiveVertex * v3,
        SoPickedPoint * pp);

private:
    enum Binding {
        OVERALL = 0,
        PER_PART,
        PER_PART_INDEXED,
        PER_FACE,
        PER_FACE_INDEXED,
        PER_VERTEX,
        PER_VERTEX_INDEXED,
        NONE = OVERALL
    };
    Binding findMaterialBinding(SoState * const state) const;
    Binding findNormalBinding(SoState * const state) const;
    void renderShape(const SoGLCoordinateElement * const vertexlist,
                     const int32_t *vertexindices,
                     int num_vertexindices,
                     const int32_t *partindices,
                     int num_partindices,
                     const SbVec3f *normals,
                     const int32_t *normindices,
                     SoMaterialBundle *const materials,
                     const int32_t *matindices,
                     SoTextureCoordinateBundle * const texcoords,
                     const int32_t *texindices,
                     const int nbind,
                     const int mbind,
                     const int texture);
};

// ---------------------------------------------------------------------

class AppPartGuiExport SoBrepEdgeSet : public SoIndexedLineSet {
    typedef SoIndexedLineSet inherited;

    SO_NODE_HEADER(SoBrepEdgeSet);

public:
    static void initClass();
    SoBrepEdgeSet();

protected:
    virtual ~SoBrepEdgeSet() {};
    virtual void GLRender(SoGLRenderAction *action);
    virtual SoDetail * createLineSegmentDetail(
        SoRayPickAction *action,
        const SoPrimitiveVertex *v1,
        const SoPrimitiveVertex *v2,
        SoPickedPoint *pp);
};

class AppPartGuiExport SoBrepEdgeHighlight : public SoIndexedLineSet {
    typedef SoIndexedLineSet inherited;

    SO_NODE_HEADER(SoBrepEdgeHighlight);

public:
    static void initClass();
    SoBrepEdgeHighlight();

    SoMFInt32 edgeIndex;
    SoSFNode edgeNode;

protected:
    virtual ~SoBrepEdgeHighlight() {};
    virtual void GLRender(SoGLRenderAction *action);
    virtual SoDetail * createLineSegmentDetail(
        SoRayPickAction *action,
        const SoPrimitiveVertex *v1,
        const SoPrimitiveVertex *v2,
        SoPickedPoint *pp);
    virtual void doAction(SoAction* action); 

private:
    SbBool _doDraw;
};

} // namespace PartGui


#endif // PARTGUI_SOBREPSHAPE_H

