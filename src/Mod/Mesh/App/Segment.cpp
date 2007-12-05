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

#include <Base/Builder3D.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/PyCXX/Objects.hxx>

#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Iterator.h"
#include "Core/Info.h"
#include "Core/TopoAlgorithm.h"
#include "Core/Evaluation.h"
#include "Core/Degeneration.h"
#include "Core/SetOperations.h"

#include "Segment.h"
#include "Mesh.h"
#include "MeshPy.h"

using namespace Mesh;

Segment::Segment(MeshObject* mesh) : _mesh(mesh)
{
}

Segment::Segment(MeshObject* mesh, const std::vector<unsigned long>& inds) : _mesh(mesh), _indices(inds)
{
}

void Segment::addIndices(const std::vector<unsigned long>& inds)
{
    _indices.insert(_indices.end(), inds.begin(), inds.end());
}

const std::vector<unsigned long>& Segment::getIndices() const
{
    return _indices;
}

// ----------------------------------------------------------------------------

Segment::FacetIter::FacetIter(const Segment* segm, std::vector<unsigned long>::const_iterator it)
  : _segment(segm), _it(it)
{
    const MeshCore::MeshKernel& kernel = _segment->_mesh->getKernel();
    this->_f_it = kernel.GetFacets().begin();
}

Segment::FacetIter::FacetIter(const Segment::FacetIter& fi)
  : _segment(fi._segment), _f_it(fi._f_it), _it(fi._it)
{
}

Segment::FacetIter::~FacetIter()
{
}

Segment::FacetIter& Segment::FacetIter::operator=(const Segment::FacetIter& fi)
{
    this->_segment = fi._segment;
    this->_f_it    = fi._f_it;
    this->_it      = fi._it;
    return *this;
}

const MeshCore::MeshFacet& Segment::FacetIter::operator*() const
{
    return *(_f_it + *_it);
}

const MeshCore::MeshFacet* Segment::FacetIter::operator->() const
{
    return &(*(_f_it + *_it));
}

bool Segment::FacetIter::operator==(const Segment::FacetIter& fi) const
{
    return (this->_segment == fi._segment) && (this->_it == fi._it);
}

bool Segment::FacetIter::operator!=(const Segment::FacetIter& fi) const 
{
    return !operator==(fi);
}

Segment::FacetIter& Segment::FacetIter::operator++()
{
    ++(this->_it);
    return *this;
}

Segment::FacetIter& Segment::FacetIter::operator--()
{
    --(this->_it);
    return *this;
}
