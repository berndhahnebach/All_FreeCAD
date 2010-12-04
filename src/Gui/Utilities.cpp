/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <Inventor/SbTesselator.h>
#endif
#include "Utilities.h"

using namespace Gui;


ViewVolumeProjection::ViewVolumeProjection (const SbViewVolume &vv)
  : viewVolume(vv)
{
}

Base::Vector3f ViewVolumeProjection::operator()(const Base::Vector3f &pt) const
{
    SbVec3f pt3d(pt.x,pt.y,pt.z);
    viewVolume.projectToScreen(pt3d,pt3d);
    return Base::Vector3f(pt3d[0],pt3d[1],pt3d[2]);
}

Base::Vector3f ViewVolumeProjection::inverse (const Base::Vector3f &pt) const
{
#if 1
    SbVec3f pt3d(2.0f*pt.x-1.0f, 2.0f*pt.y-1.0f, 2.0f*pt.z-1.0f);
    viewVolume.getMatrix().inverse().multVecMatrix(pt3d, pt3d);
#elif 1
    SbLine line; SbVec3f pt3d;
    SbPlane distPlane = viewVolume.getPlane(viewVolume.getNearDist());
    viewVolume.projectPointToLine(SbVec2f(pt.x,pt.x), line);
    distPlane.intersect(line, pt3d);
#else
    SbVec3f pt3d = viewVolume.getPlanePoint(viewVolume.getNearDist(), SbVec2f(pt.x,pt.y));
#endif
    return Base::Vector3f(pt3d[0],pt3d[1],pt3d[2]);
}

Base::Matrix4D ViewVolumeProjection::getProjectionMatrix () const
{
    // Inventor stores the transposed matrix
    Base::Matrix4D mat;
    SbMatrix affine, proj;
    viewVolume.getMatrices(affine, proj);
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++)
            mat[i][j] = proj[j][i];
    }

    return mat;
}

// ----------------------------------------------------------------------------

void Tessellator::tessCB(void * v0, void * v1, void * v2, void * cbdata)
{
    int * vtx0 = (int *)v0; 
    int * vtx1 = (int *)v1; 
    int * vtx2 = (int *)v2;

    std::vector<int>* array = (std::vector<int> *)cbdata;
    array->push_back(*vtx0);
    array->push_back(*vtx1);
    array->push_back(*vtx2);
    array->push_back(-1);
}

Tessellator::Tessellator(const std::vector<SbVec2f>& poly) : polygon(poly)
{
}

std::vector<int> Tessellator::tessellate() const
{
    std::vector<int> indices(polygon.size());
    std::vector<int> face_indices;

    SbTesselator tessellator(tessCB, &face_indices);
    tessellator.beginPolygon();

    int index = 0;
    for (std::vector<SbVec2f>::const_iterator it = polygon.begin(); it != polygon.end(); ++it, index++) {
        indices[index] = index;
        tessellator.addVertex(SbVec3f((*it)[0], (*it)[1], 0.0f), &(indices[index]));
    }

    // run the triangulation now
    tessellator.endPolygon();
    return face_indices;
}

