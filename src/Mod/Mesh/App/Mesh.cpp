/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
# include <sstream>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Writer.h>
#include <Base/Reader.h>

#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Iterator.h"
#include "Core/Info.h"
#include "Core/TopoAlgorithm.h"

#include "Mesh.h"
#include "MeshPy.h"

using namespace Mesh;

MeshObject::MeshObject(MeshCore::MeshKernel *Kernel, const Base::Matrix4D &Mtrx)
  : ComplexGeoData(Mtrx),_pcKernel(Kernel)
{
}

MeshObject::MeshObject()
  : _pcKernel(new MeshCore::MeshKernel())
{
}

MeshObject::~MeshObject()
{
}

std::string MeshObject::representation() const
{
    std::stringstream str;
    MeshCore::MeshInfo info(*_pcKernel);
    info.GeneralInformation(str);
    return str.str();
}

unsigned long MeshObject::countPoints() const
{
    return _pcKernel->CountPoints();
}

unsigned long MeshObject::countFacets() const
{
    return _pcKernel->CountFacets();
}

unsigned long MeshObject::countEdges () const
{
    return _pcKernel->CountEdges();
}

unsigned long MeshObject::countComponents() const
{
    std::vector<std::vector<unsigned long> > segments;
    MeshCore::MeshComponents comp(*_pcKernel);
    comp.SearchForComponents(MeshCore::MeshComponents::OverEdge,segments);
    return segments.size();
}

void MeshObject::save(const char* file) const
{
    MeshCore::MeshOutput aWriter(*_pcKernel);
    aWriter.SaveAny(file);
}

void MeshObject::save(std::ostream& out) const
{
    _pcKernel->Write(out);
}

void MeshObject::load(const char* file)
{
    MeshCore::MeshInput aReader(*_pcKernel);
    aReader.LoadAny(file);
}

void MeshObject::load(std::istream& in)
{
    _pcKernel->Read(in);
}

MeshCore::MeshFacetIterator MeshObject::FacetIterator() const
{
    MeshCore::MeshFacetIterator it = _pcKernel->FacetIterator();
    it.Transform(_Mtrx);
    return it;
}

MeshCore::MeshPointIterator MeshObject::PointIterator() const
{
    MeshCore::MeshPointIterator it = _pcKernel->PointIterator();
    it.Transform(_Mtrx);
    return it;
}

void MeshObject::clear(void)
{
    _pcKernel->Clear();
    setTransform(Base::Matrix4D());
}

void MeshObject::flipNormals()
{
    MeshCore::MeshTopoAlgorithm alg(*_pcKernel);
    alg.FlipNormals();
}

void MeshObject::harmonizeNormals()
{
    MeshCore::MeshTopoAlgorithm alg(*_pcKernel);
    alg.HarmonizeNormals();
}

void MeshObject::collapseFacets(const std::vector<unsigned long>& facets)
{
    MeshCore::MeshTopoAlgorithm alg(*_pcKernel);
    for (std::vector<unsigned long>::const_iterator it = facets.begin(); it != facets.end(); ++it) {
        alg.CollapseFacet(*it);
    }
}

void MeshObject::removeComponents(unsigned long count)
{
    MeshCore::MeshTopoAlgorithm(*_pcKernel).RemoveComponents(count);
}

void MeshObject::fillupHoles(unsigned long length)
{
    MeshCore::MeshTopoAlgorithm topalg(*_pcKernel);
    topalg.FillupHoles(length);
}
