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
#endif

#include "Smoothing.h"
#include "MeshKernel.h"
#include "Algorithm.h"
#include "Elements.h"
#include "Iterator.h"
#include "Approximation.h"


using namespace MeshCore;


AbstractSmoothing::AbstractSmoothing(MeshKernel& m) : kernel(m)
{
}

AbstractSmoothing::~AbstractSmoothing()
{
}

void AbstractSmoothing::initialize(Component comp, Continuity cont)
{
    this->component = comp;
    this->continuity = cont;
}

MeshSmoothing::MeshSmoothing(MeshKernel& m)
  : AbstractSmoothing(m)
{
}

MeshSmoothing::~MeshSmoothing()
{
}

void MeshSmoothing::Smooth(unsigned int iterations)
{
    MeshCore::MeshPoint center;
    MeshCore::MeshPointArray PointArray = kernel.GetPoints();

    MeshCore::MeshPointIterator v_it(kernel);
    MeshCore::MeshRefPointToPoints vv_it(kernel);
    std::set<MeshCore::MeshPointArray::_TConstIterator> cv;// circulator
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator cv_it;

    for (unsigned int i=0; i<iterations; i++) {
        Base::Vector3f N, L;
        for (v_it.Begin(); v_it.More(); v_it.Next()) {
            MeshCore::PlaneFit pf;
            pf.AddPoint(*v_it);
            center = *v_it;
            cv = vv_it[v_it.Position()];
            if (cv.size() < 3)
                continue;

            for (cv_it = cv.begin(); cv_it !=cv.end(); ++cv_it) {
                pf.AddPoint((*cv_it)[0]);
                center += (*cv_it)[0];
            }

            float scale = 1.0f/((float)cv.size()+1.0f);
            center.Scale(scale,scale,scale);

            // get the mean plane of the current vertex with the surrounding vertices
            pf.Fit();
            N = pf.GetNormal();
            N.Normalize();

            // look in which direction we should move the vertex
            L.Set(v_it->x - center.x, v_it->y - center.y, v_it->z - center.z);
            if (N*L < 0.0)
                N.Scale(-1.0, -1.0, -1.0);

            // maximum value to move is distance to mean plane
            float d = std::min<float>((float)fabs(this->tolerance),(float)fabs(N*L));
            N.Scale(d,d,d);

            PointArray[v_it.Position()].Set(v_it->x - N.x, v_it->y - N.y, v_it->z - N.z);
        }

        // assign values without affecting iterators
        unsigned long count = kernel.CountPoints();
        for (unsigned long idx = 0; idx < count; idx++) {
            kernel.SetPoint(idx, PointArray[idx]);
        }
    }
}

LaplaceSmoothing::LaplaceSmoothing(MeshKernel& m)
  : AbstractSmoothing(m)
{
}

LaplaceSmoothing::~LaplaceSmoothing()
{
}

void LaplaceSmoothing::Smooth(unsigned int iterations)
{
}

JacobiLaplaceSmoothing::JacobiLaplaceSmoothing(MeshKernel& m)
  : LaplaceSmoothing(m)
{
}

JacobiLaplaceSmoothing::~JacobiLaplaceSmoothing()
{
}

void JacobiLaplaceSmoothing::Smooth(unsigned int iterations)
{
}
