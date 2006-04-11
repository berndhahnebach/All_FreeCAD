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


#ifndef MESH_TOPOALGORITHM_H
#define MESH_TOPOALGORITHM_H

#ifndef _PreComp_
# include <map>
# include <vector>
#endif

#include "Definitions.h"
#include "Iterator.h"
#include "MeshKernel.h"
#include "Elements.h"
#include "Visitor.h"
#include "Algorithm.h"

#include <Base/Vector3D.h>
#include <Base/Sequencer.h>

using Base::Vector3D;


namespace MeshCore {

class MeshKernel;
class MeshFacet;

/**
 * Most of the provided methods don't modify the data structure directly but the changes are buffered in an array of facets.
 * To perform the changes on the data structure 'Commit' must be called.
 * E.g. if you want to insert a node to any triangles you have to do the following:
 * \code
 * MeshTopoAlgorithm cTopAlg(...);
 * cTopAlg.InsertNode(0, point1);
 * cTopAlg.InsertNode(4, point2);
 * cTopAlg.InsertNode(5, point4);
 * 
 * // the data structure is not yet modified...
 * 
 * cTopAlg.Commit(); // now the data structure is modified
 * \endcode
 *
 * All methods with the prefix 'Direct' modify the structure directly. 
 * @author Werner Mayer
 */
class AppMeshExport MeshTopoAlgorithm
{
public:
  // construction/destruction
  MeshTopoAlgorithm (MeshKernel &rclM);
  virtual ~MeshTopoAlgorithm (void);

public:
  /** 
   * Applies the changes.
   */
  void Commit ();
  /**
   * Discard all changes.
   */
  void Discard();
  /**
   * Tries to make a more beautiful mesh.
   */
  void OptimizeTopology();
  /**
   * Inserts a new vertex in the given triangle.
   * The given point must lie inside the triangle not outside or on an edge.
   */
  void InsertNode(unsigned long ulFacetPos, const Vector3D&  rclPoint);
  /**
   * If the given point references three facets only (the point must not be a border point, 
   * this is not checked) then each pair of the three facets must be neighbours.
   * The three facets are replaced by one facet. 
   * This operation is exactly the inverse operation of InsertNode().
   */
  bool InverseInsertNode(unsigned long ulPointPos);
  /**
   * Swaps the common edge of the facet and its neighbour.
   * You have to make sure that the two triangles build a convex
   * polygon and the facets should lie in the same plane..
   */
  void SwapEdge(unsigned long ulFacetPos, int iSide);
  /**
   * Insert a new edge. Each concerned triangle is broken into two
   * new triangles.
   */
  void SplitEdge(unsigned long ulFacetPos, int iSide, const Vector3D& rclPoint);
  /**
   * Splits the facet with index \a ulFacetPos into several facets. The points \a rP1 and \a
   * rP2 must lie at two different edges of the facets.
   * @note In this algorithm the neighbour facets of \a ulFacetIndex are not modified. So the
   * caller has to take care that SplitFacet() gets invoked for these facets, too; otherwise
   * the topology of the mesh gets broken after the call of Commit()
   */
  void SplitFacet(unsigned long ulFacetPos, const Vector3D& rP1, const Vector3D& rP2);
  /**
   * Insert a new edge. Each concerned triangle is broken into two
   * new triangles. Unlike SplitEdge this method allows to split a facet several
   * times, e.g you can call this method several times for the same facet.
   *
   * Attention!: 
   * the specified side of the facet must be an open edge because the algorithm 
   * does not check for neighbours if it is not an open edge.
   */
  bool MultiSplitOpenEdge(unsigned long ulFacetPos, int iSide, const Vector3D& rclPoint, float fTol = FLOAT_EPS);
  /**
   * Removes a vertex and retriangulates the arising hole.
   */
  bool CollapseNode(unsigned long ulPointPos);
  /**
   * Removes the triangles to the given edge (p0 <=> p1) and fills the
   * arising hole by tighting the remaining facets.
   */
  bool CollapseEdge(unsigned long ulEdgeP0, unsigned long ulEdgeP1);
  /**
   * Inserts several new facets.
   */
  void InsertFacets(const std::vector<MeshGeomFacet>& raclFacets);
  /**
   * Harmonizes the normals. The changes are done immediately.
   */
  void HarmonizeNormals (void);
  /** 
   * Flips the normals. The changes are done immediately.
   */
  void FlipNormals (void);

  /** @name Direct manipulation 
   * All these methods manipulate the mesh structure directly. 
   */
  //@{
  /**
   * Splits the facet at position \a ulFacetPos in the mesh array into two facets. The point \a rP must lie on the 
   * edge with at \a side of the given facet. The adjacent facet that shares the same edge where \a rP lies on gets 
   * broken, too.
   * If the number of facets is less than \a ulFacetPos+1 or if \a rP is coincident with a corner point nothing happens.
   */
  void DirectSplitFacet(unsigned long ulFacetPos, unsigned short side, const Vector3D& rP);
  /**
   * Does basically the same as DirectSplitFacet() unless that the facet at position \a ulFacetPos has no adjacent
   * facet at its \a side.
   */
  void DirectSplitFacetWithOpenEdge(unsigned long ulFacetPos, unsigned short side, const Vector3D& rP);
  /**
   * Removes the degenerated facet at position \a index from the mesh structure. A facet is degenerated if its corner
   * points are collinear.
   */
  void DirectRemoveDegenerated(unsigned long index);
  /**
   * Removes the corrupted facet at position \a index from the mesh structure. A facet is corrupted if the indices of its corner
   * points are not all different.
   */
  void DirectRemoveCorrupted(unsigned long index);
  //@}

private:
  void RefPointToFacet();
  /**
   * Checks if the polygon is convex
   */
  bool IsConvexPolygon(const std::vector<unsigned long>& raulPoly);
  /**
   * Returns the circumjacent polygon the the point.
   */
  bool GetPolygonOfNeighbours(unsigned long ulPointPos, std::vector<unsigned long>& raulPoly);
  /**
   * Retriangulates the polygon.
   */
  bool TriangulatePolygon(const std::vector<unsigned long>& raulPoly, std::vector<MeshGeomFacet>& raclFacets);
  /** @deprecated
   * The facet \a ulFacetInd is rearranged so that the neighbour at side \a iSide becomes neighbour 0. 
   */
  void RotateFacet(unsigned long ulFacetInd, int iSide);
  /** @deprecated
   * Inserts the edges associated to the facet \a ulFacetPos if needed. This method
   * is safe if the facet has already associated edges.
   */

  void ClearFlag (void)
  { _rclMesh._aclFacetArray.ResetFlag(MeshFacet::INVALID); }

  void SetFlag (unsigned long ulPos)
  { _rclMesh._aclFacetArray[ulPos].SetFlag(MeshFacet::INVALID); }

  void ResetFlag (unsigned long ulPos)
  { _rclMesh._aclFacetArray[ulPos].ResetFlag(MeshFacet::INVALID); }

  bool IsFlag (unsigned long ulPos) const
  { return _rclMesh._aclFacetArray[ulPos].IsFlag(MeshFacet::INVALID); }

private:
  MeshKernel& _rclMesh;
  MeshRefPointToFacets * _pclRefPt2Fac;
  std::vector<MeshGeomFacet> _aclNewFacets;
  std::map<unsigned long, std::vector<MeshGeomFacet> > _aclMultiSplitted;
};

/**
 * The MeshHarmonizer class adjusts the orientation of all facets to the
 * orientation of a start facet.
 */
class AppMeshExport MeshHarmonizer: virtual public MeshFacetVisitor
{
public:
  MeshHarmonizer( std::vector<unsigned long>& uIndices ) : _uIndices(uIndices) {}
  /** Adjusts the orientation of facet \a rclFacet to that one of \a rclFrom. */
  bool Visit (const MeshFacet &rclFacet, const MeshFacet &rclFrom, unsigned long ulFInd, unsigned long)
  {
    unsigned long i, j;
    Base::Sequencer().next();
  
    // Normale an Vorgaenger-Facet angleichen => Umlaufrichtung gegenseitig
    for (i = 0; i < 2; i++)
    {
      for (j = 0; j < 3; j++)
      {
        if (rclFrom._aulPoints[i] == rclFacet._aulPoints[j])
        {  // gemeinsamer Punkt
          if ((rclFrom._aulPoints[i+1]     == rclFacet._aulPoints[(j+1)%3]) ||
              (rclFrom._aulPoints[(i+2)%3] == rclFacet._aulPoints[(j+2)%3]))
          {
            _uIndices.push_back(ulFInd);
          } 
          return true;
        }
      }
    }
    return true;
  }

private:
  std::vector<unsigned long>& _uIndices;
};

/**
 * The MeshComponents class searches for topologic independent "isles" of the 
 * given mesh structure. 
 *
 * @author Werner Mayer
 */
class AppMeshExport MeshComponents
{
public:
  enum TMode {OverEdge, OverPoint};

  MeshComponents( const MeshKernel& rclMesh );
  ~MeshComponents();

  /**
	 * Searches for 'isles' of the mesh. If \a tMode is \a OverEdge then facets sharing the same edge are
   * regarded as connected, if \a tMode is \a OverPoint then facets sharing a common point are regarded
   * as connected.
   */ 
  void SearchForComponents(TMode tMode, std::vector<std::vector<unsigned long> >& aclT) const;

  /**
   * Does basically the same as the method above escept that only the faces in \a aSegment are regarded.
   */
  void SearchForComponents(TMode tMode, const std::vector<unsigned long>& aSegment, std::vector<std::vector<unsigned long> >& aclT) const;

protected:
  // for sorting of elements
  struct CNofFacetsCompare : public std::binary_function<const std::vector<unsigned long>&, 
                                                         const std::vector<unsigned long>&, bool>
  {
    bool operator () (const std::vector<unsigned long> &rclC1, const std::vector<unsigned long> &rclC2)
    {
      return rclC1.size() > rclC2.size();
    }
  };
protected:
	const MeshKernel& _rclMesh;
};

} // namespace MeshCore

#endif // MESH_TOPOALGORITHM_H
