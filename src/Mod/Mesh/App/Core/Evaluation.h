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


#ifndef MESH_EVALUATION_H
#define MESH_EVALUATION_H

#ifndef _PreComp_
# include <list>
# include <math.h>
#endif

#include "MeshKernel.h"

namespace Mesh {

/**
 * The MeshEvaluation class checks the mesh kernel for correctness with respect to a
 * certain criterion, such as manifoldness, self-intersections, etc.
 * @see MeshEvalTopology
 * @see MeshEvalGeometry
 * The class itself is abstract, hence the method Evaluate() must be implemented 
 * by subclasses.
 */
class MeshEvaluation
{
public:
  enum State { Invalid, Fixed, Valid };

  MeshEvaluation (MeshKernel &rclB) : _rclMesh(rclB), _bIsValid(false) {}
  virtual ~MeshEvaluation () {}

  /**
   * Invokes Evaluate() to check for correctness. If \a fixup is true and if Evaluate() returns false then
   * Fixup() gets also invoked. If the evaluation was successful \a Valid is returned, if the evaluation failed
   * but the errors could be fixed -- in case \a fixup is \a true -- \a Fixed is returned, in all other cases \a
   * Invalid is returned.
   * @note: \a Fixed does not necessarily mean \a Valid. It is possible to have fixed some errors but the mesh
   * is still invalid. If you want to know if Fixed() was successful you might run Validate() twice.
   */
  State Validate ( bool fixup=false )
  {
    _bIsValid = false;
    if ( Evaluate() )
    {
      _bIsValid = true;
      return Valid;
    }
    else if ( fixup )
    {
      if ( Fixup() )
        return Fixed;
      else
        return Invalid;
    }
    else
    {
      return Invalid;
    }
  };

  bool IsValid() const
  {
    return _bIsValid;
  }

protected:
  /**
   * Evaluates the mesh kernel with respect to certain criteria. Must be reimplemented by every 
   * subclass. This pure virtual function returns false if the mesh kernel is invalid according
   * to this criterion and true if the mesh kernel is correct. 
   */
  virtual bool Evaluate () = 0;
  /**
   * This function attempts to change the mesh kernel to be valid according to the checked 
   * criterion: True is returned if the errors could be fixed, false otherwise. The default 
   * implementation does nothing and returns false.
   */
  virtual bool Fixup()
  {
    return false;
  }

protected:
  MeshKernel& _rclMesh; /**< Mesh kernel */

private:
  bool _bIsValid; /**< Holds the result of Evakuate(). */
};

// ----------------------------------------------------

/**
 * The MeshEvalNormals class checks the mesh kernel for consistent facet normals.
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalNormals : public MeshEvaluation
{
public:
  MeshEvalNormals (MeshKernel& rclM);
  ~MeshEvalNormals();

protected:
  bool Evaluate ();
  bool Fixup();
};

// ----------------------------------------------------

/**
 * The MeshEvalSolid class checks if the mesh represents a solid.
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalSolid : public MeshEvaluation
{
public:
  MeshEvalSolid (MeshKernel& rclM);
  ~MeshEvalSolid();

protected:
  bool Evaluate ();
  bool Fixup();
};

// ----------------------------------------------------

/**
 * The MeshEvalTopology class checks for topologic correctness, i.e
 * that the mesh must not contain non-manifolds. E.g. an edge is regarded as
 * non-manifold if it is shared by more than two facets.
 * @note This check does not necessarily cover any degenerations.
 */
class AppMeshExport MeshEvalTopology : public MeshEvaluation
{
public:
  MeshEvalTopology (MeshKernel &rclB) : MeshEvaluation(rclB) {}
  virtual ~MeshEvalTopology () {}

  void GetEdgeManifolds (std::vector<unsigned long> &raclEdgeIndList) const;
  void GetFacetManifolds (std::vector<unsigned long> &raclFacetIndList) const;
  unsigned long CountManifolds() const;

protected:
  std::vector<std::list<unsigned long> >   _aclManifoldList;
  virtual bool Evaluate ();
};

// ----------------------------------------------------

/**
 * The MeshEvalSingleFacet class checks a special case of non-manifold edges as follows.
 * If an edge is shared by more than two facets and if all further facets causing this non-
 * manifold have only their neighbour facets set at this edge, i.e. they have no neighbours
 * at their other edges.
 * Such facets can just be removed from the mesh.
 */
class AppMeshExport MeshEvalSingleFacet : public MeshEvalTopology
{
public:
  MeshEvalSingleFacet (MeshKernel &rclB) : MeshEvalTopology(rclB) {}
  virtual ~MeshEvalSingleFacet () {}

protected:
  bool Evaluate ();
  bool Fixup();
};

// ----------------------------------------------------

/**
 * The MeshEvalSelfIntersection class checks the mesh for self intersection.
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalSelfIntersection : public MeshEvaluation
{
public:
  MeshEvalSelfIntersection (MeshKernel &rclB) : MeshEvaluation(rclB) {}
  virtual ~MeshEvalSelfIntersection () {}

protected:
  bool Evaluate ();
};

// ----------------------------------------------------

/**
 * The MeshEvalNeighbourhood class checks if the neighbourhood among the facets is
 * set correctly.
 * @author Werner Mayer
 */
class AppMeshExport MeshEvalNeighbourhood : public MeshEvaluation
{
public:
  MeshEvalNeighbourhood (MeshKernel &rclB) : MeshEvaluation(rclB) {}
  ~MeshEvalNeighbourhood () {}

protected:
  bool Evaluate ();
  bool Fixup();
};

// ----------------------------------------------------

/**
 * The MeshEigensystem class actually does not try to check for or fix errors but
 * it provides methods to calculate the mesh's local coordinate system with the center
 * of gravity as origin.
 * The local coordinate system is computed this way that u has minimum and w has maximum 
 * expansion. The local coordinate system is right-handed.
 * @author Werner Mayer
 */
class AppMeshExport MeshEigensystem : public MeshEvaluation
{
public:
  MeshEigensystem (MeshKernel &rclB);
  virtual ~MeshEigensystem () {}

  /** Returns the transformation matrix. */
  Matrix4D Transform() const;
  /**
   * Returns the expansions in \a u, \a v and \a w of the bounding box.
   */
  Vector3D GetBoundings() const;

protected:
  bool Evaluate();
  /** 
   * Does not fix errors but apply the transformation matrix to the mesh object.
   */
  bool Fixup();
  /** 
   * Calculates the local coordinate system defined by \a u, \av, \a w and \a c. 
   */
  void CalculateLocalSystem();

private:
  Vector3D _cU, _cV, _cW, _cC; /**< Vectors that define the local coordinate system. */
  float _fU, _fV, _fW; /**< Expansion in \a u, \a v, and \a w direction of the transformed mesh. */
};

} // namespace Mesh

#endif // MESH_EVALUATION_H
