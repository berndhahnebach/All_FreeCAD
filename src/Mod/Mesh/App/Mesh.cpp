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
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Writer.h>
#include <Base/Reader.h>

#include "Core/MeshKernel.h"
#include "Core/MeshIO.h"
#include "Core/Iterator.h"

#include "Mesh.h"
#include "MeshPy.h"

using namespace Mesh;


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

void MeshObject::ApplyTransform( const Base::Matrix4D& rclTrf )
{
  _Mtrx = rclTrf * _Mtrx;
}

void MeshObject::SetTransform( const Base::Matrix4D& rclTrf )
{
  _Mtrx = rclTrf;
}

