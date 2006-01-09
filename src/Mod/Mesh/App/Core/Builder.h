/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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

#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#ifndef _PreComp_
# include <set>
# include <vector>
#endif

#include "MeshKernel.h"

namespace Base
{
  class Vector3D;
}

using Base::Vector3D;

namespace MeshCore
{
class MeshKernel;
class MeshPoint;
class MeshGeomFacet;

class AppMeshExport MeshBuilder
{
  /**
 * Class for creating the mesh structure by adding facets. Building the structure needs 3 steps:
 * 1. initializing  
 * 2. adding the facets
 * 3. finishing
 * \code
 * // Sample Code for building a mesh structure
 * MeshBuilder builder(someMeshReference);
 * builder.Initialize(numberOfFacets);
 * ...
 * for (...)
 *   builder.AddFacet(...);
 * ...
 * builder.Finish();
 * \endcode
 * @author Berthold Grupp
 */

private:
  /** @name Helper class */
  //@{
	class Edge
	{
		public:
		unsigned long	pt1, pt2, facetIdx;

		Edge (unsigned long p1, unsigned long p2, unsigned long idx)
		{
			facetIdx = idx;
			if (p1 > p2)
			{
				pt1 = p2;
				pt2 = p1;
			}
			else
			{
				pt1 = p1;
				pt2 = p2;
			}
		}

		bool operator < (const Edge &e) const
		{
			return (pt1 == e.pt1) ? (pt2 < e.pt2) : (pt1 < e.pt1);
		}

		bool operator > (const Edge &e) const
		{
			return (pt1 == e.pt1) ? (pt2 > e.pt2) : (pt1 > e.pt1);
		}

		bool operator == (const Edge &e) const
		{
			return (pt1 == e.pt1) && (pt2 == e.pt2);
		}
	};
  //@}

	MeshKernel& _meshKernel;

	std::set<MeshPoint>	_points;
	std::set<Edge>			_edges;
	unsigned long				_ptIdx, _facetIdx;

public:
	MeshBuilder(MeshKernel &rclM);
	~MeshBuilder(void);

  /** Initialize the class. Must be done before adding facets */
	void Initialize (long ctFacets);

  /** adding facets */
	void AddFacet (const MeshGeomFacet& facet);
	void AddFacet (const Vector3D& pt1, const Vector3D& pt2, const Vector3D& pt3, const Vector3D& normal);
	void AddFacet (Vector3D* facetPoints);

  /** finish building the mesh structure. Must be done after adding facets */
	void Finish ();

  friend class MeshKernel;
};

} // namespace MeshCore

#endif 
