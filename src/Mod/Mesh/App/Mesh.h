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


#ifndef _Mesh_h_
#define _Mesh_h_

#include <vector>
#include <list>
#include <set>
#include <string>
#include <map>

#include <Base/Matrix.h>
#include <Base/Vector3D.h>

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>
#include <App/ComplexGeoData.h>

#include "Core/MeshKernel.h"


namespace Mesh
{
class AppMeshExport MeshObject: public Data::ComplexGeoData
{
public:
  MeshObject(MeshCore::MeshKernel *Kernel, const Base::Matrix4D &Mtrx )
    : _pcKernel(Kernel),ComplexGeoData(Mtrx){}

  MeshObject();

  /** Returns an iterator object to go over all facets. */
  MeshCore::MeshFacetIterator FacetIterator() const;
  /** Returns an iterator object to go over all points. */
  MeshCore::MeshPointIterator PointIterator() const;

  MeshCore::MeshKernel& getKernel(void){return *_pcKernel;}
  const MeshCore::MeshKernel& getKernel(void) const {return *_pcKernel;}

  /// clears the Mesh
  void clear(void);

protected:
  MeshCore::MeshKernel* _pcKernel;
};



} // namespace Mesh

#endif // _Mesh_h_

