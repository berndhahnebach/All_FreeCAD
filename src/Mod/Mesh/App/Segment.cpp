/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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


#include "Core/MeshKernel.h"

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

const Segment& Segment::operator = (const Segment& s)
{
    // Do not copy the MeshObject pointer
    if (this != &s)
        this->_indices = s._indices;
    return *this;
}

bool Segment::operator == (const Segment& s) const
{
    return this->_indices == s._indices;
}

// ----------------------------------------------------------------------------

Segment::FacetIter::FacetIter(const Segment* segm, std::vector<unsigned long>::const_iterator it)
  : _segment(segm), _f_it(segm->_mesh->getKernel()), _it(it)
{
    this->_f_it.Set(0);
    this->_f_it.Transform(_segment->_mesh->getTransform());
    this->_facet.Mesh = _segment->_mesh;
}

Segment::FacetIter::FacetIter(const Segment::FacetIter& fi)
  : _segment(fi._segment), _facet(fi._facet), _f_it(fi._f_it), _it(fi._it)
{
}

Segment::FacetIter::~FacetIter()
{
}

Segment::FacetIter& Segment::FacetIter::operator=(const Segment::FacetIter& fi)
{
    this->_segment = fi._segment;
    this->_facet   = fi._facet;
    this->_f_it    = fi._f_it;
    this->_it      = fi._it;
    return *this;
}

void Segment::FacetIter::dereference()
{
    this->_f_it.Set(*_it);
    this->_facet.MeshCore::MeshGeomFacet::operator = (*_f_it);
    this->_facet.Index = *_it;
    const MeshCore::MeshFacet& face = _f_it.GetReference();
    for (int i=0; i<3;i++) {
        this->_facet.PIndex[i] = face._aulPoints[i];
        this->_facet.NIndex[i] = face._aulNeighbours[i];
    }
}

Facet& Segment::FacetIter::operator*()
{
    dereference();
    return this->_facet;
}

Facet* Segment::FacetIter::operator->()
{
    dereference();
    return &(this->_facet);
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

// ----------------------------------------------------------------------------

Segment::ConstFacetIter::ConstFacetIter(const Segment* segm, std::vector<unsigned long>::const_iterator it)
  : _segment(segm), _f_it(segm->_mesh->getKernel()), _it(it)
{
    this->_f_it.Set(0);
    this->_f_it.Transform(_segment->_mesh->getTransform());
    this->_facet.Mesh = _segment->_mesh;
}

Segment::ConstFacetIter::ConstFacetIter(const Segment::ConstFacetIter& fi)
  : _segment(fi._segment), _facet(fi._facet), _f_it(fi._f_it), _it(fi._it)
{
}

Segment::ConstFacetIter::~ConstFacetIter()
{
}

Segment::ConstFacetIter& Segment::ConstFacetIter::operator=(const Segment::ConstFacetIter& fi)
{
    this->_segment = fi._segment;
    this->_facet   = fi._facet;
    this->_f_it    = fi._f_it;
    this->_it      = fi._it;
    return *this;
}

void Segment::ConstFacetIter::dereference()
{
    this->_f_it.Set(*_it);
    this->_facet.MeshCore::MeshGeomFacet::operator = (*_f_it);
    this->_facet.Index = *_it;
    const MeshCore::MeshFacet& face = _f_it.GetReference();
    for (int i=0; i<3;i++) {
        this->_facet.PIndex[i] = face._aulPoints[i];
        this->_facet.NIndex[i] = face._aulNeighbours[i];
    }
}

const Facet& Segment::ConstFacetIter::operator*() const
{
    const_cast<ConstFacetIter*>(this)->dereference();
    return this->_facet;
}

const Facet* Segment::ConstFacetIter::operator->() const
{
    const_cast<ConstFacetIter*>(this)->dereference();
    return &(this->_facet);
}

bool Segment::ConstFacetIter::operator==(const Segment::ConstFacetIter& fi) const
{
    return (this->_segment == fi._segment) && (this->_it == fi._it);
}

bool Segment::ConstFacetIter::operator!=(const Segment::ConstFacetIter& fi) const 
{
    return !operator==(fi);
}

Segment::ConstFacetIter& Segment::ConstFacetIter::operator++()
{
    ++(this->_it);
    return *this;
}

Segment::ConstFacetIter& Segment::ConstFacetIter::operator--()
{
    --(this->_it);
    return *this;
}
