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


#ifndef MESH_ITERATOR_H
#define MESH_ITERATOR_H

#include "MeshKernel.h"
#include "Elements.h"

namespace Mesh {

class MeshKernel;
class MeshGeomFacet;
class MeshPoint;
class MeshGeomEdge;
class MeshIndexEdge;
class MeshHelpEdge;

/**
 * The MeshFacetIterator allows to iterate over the facets that
 * hold the topology of the mesh and provides access to their
 * geometric information.
 */
class MeshFacetIterator
{
public:
  /** @name Construction */
  //@{
  /// construction
  inline MeshFacetIterator (const MeshKernel &rclM);
  /// construction
  inline MeshFacetIterator (const MeshKernel &rclM, unsigned long ulPos);
  /// construction
  inline MeshFacetIterator (const MeshFacetIterator &rclI);
  //@}

  /** @name Access methods */
  //@{
  /// Access to the element the iterator points to.
  const MeshGeomFacet& operator*(void)
  { return Dereference(); }
  /// Access to the element the iterator points to.
  const MeshGeomFacet* operator->(void)
  { return &Dereference(); }
  /// Increments the iterator. It points then to the next element if the
  /// end is not reached.
  const MeshFacetIterator& operator ++ (void)
  { _clIter++; return *this; }
  /// Decrements the iterator. It points then to the previous element if the beginning
  /// is not reached.
  const MeshFacetIterator& operator -- (void)
  { _clIter--; return *this; }
  /// Increments the iterator by \a k positions.
  const MeshFacetIterator& operator += (int k)
  { _clIter += k; return *this; }
  /// Decrements the iterator by \a k positions.
  const MeshFacetIterator& operator -= (int k)
  { _clIter -= k; return *this; }
  /// Assignment.
  inline MeshFacetIterator& operator = (const MeshFacetIterator &rpI);
  /// Compares if this iterator points to a lower element than the other one.
  bool operator < (const MeshFacetIterator &rclI) const
  { return _clIter < rclI._clIter; }
  /// Compares if this iterator points to a higher element than the other one.
  bool operator > (const MeshFacetIterator &rclI) const
  { return _clIter > rclI._clIter; }
  /// Checks if the iterators points to the same element.
  bool operator == (const MeshFacetIterator &rclI) const
  { return _clIter == rclI._clIter; }
  /// Sets the iterator to the beginning of the array.
  void Begin (void)
  { _clIter = _rclMesh._aclFacetArray.begin(); }
  /// Sets the iterator to the end of the array.
  void End (void)
  { _clIter = _rclMesh._aclFacetArray.end(); }
  /// Returns the current position of the iterator in the array.
  unsigned long Position (void) const
  { return _clIter - _rclMesh._aclFacetArray.begin(); }
  /// Checks if the end is already reached.
  bool EndReached (void) const
  { return !(_clIter < _rclMesh._aclFacetArray.end()); }
  /// Sets the iterator to the beginning of the array.
  void  Init (void)
  { Begin(); }
  /// Checks if the end is not yet reached.
  bool More (void) const
  { return !EndReached(); }
  /// Increments the iterator.
  void  Next (void)
  { operator++(); }
  /// Sets the iterator to a given position.
  inline bool Set (unsigned long ulIndex);
  /// Returns the topologic facet.
  inline MeshFacet GetIndicies (void) const
  { return *_clIter; }
  /// Returns iterators pointing to the current facet's neighbours.
  inline void GetNeighbours (MeshFacetIterator &rclN0, MeshFacetIterator &rclN1, MeshFacetIterator &rclN2) const;
  /// Sets the iterator to the current facet's neighbour of the side \a usN.
  inline void SetToNeighbour (unsigned short usN);
  /// Retruns the property information to the current facet.
  inline unsigned long GetProperty (void) const;
  /// Checks if the iterator points to a valid element inside the array.
  inline bool IsValid (void) const
  { return (_clIter >= _rclMesh._aclFacetArray.begin()) && (_clIter < _rclMesh._aclFacetArray.end()); }
  //@}

protected:
  inline const MeshGeomFacet& Dereference (void);

protected:
  const MeshKernel&     _rclMesh;
  const MeshFacetArray& _rclFAry;
  const MeshPointArray& _rclPAry;
  MeshFacetArray::_TConstIterator _clIter;
  MeshGeomFacet _clFacet;

  // friends
  friend class MeshKernel;
};

class AppMeshExport MeshPointIterator
{
public:
  /** @name Construction */
  //@{
  inline MeshPointIterator (const MeshKernel &rclM);
  inline MeshPointIterator (const MeshKernel &rclM, unsigned long ulPos);
  inline MeshPointIterator (const MeshPointIterator &rclI);
  //@}
  
  /** @name Access methods */
  //@{
  /// Access to the element the iterator points to.
  const MeshPoint& operator*(void) const
  { return Dereference(); }
  /// Access to the element the iterator points to.
  const MeshPoint* operator->(void) const
  { return &Dereference(); }
  /// Increments the iterator. It points then to the next element if the
  /// end is not reached.
  const MeshPointIterator& operator ++ (void)
        { _clIter++; return *this; }
  /// Decrements the iterator. It points then to the previous element if the beginning
  /// is not reached.
  const MeshPointIterator& operator -- (void)
        { _clIter--; return *this; }
  /// Assignment.
  inline MeshPointIterator& operator = (const MeshPointIterator &rpI);
  /// Compares if this iterator points to a lower element than the other one.
  bool operator < (const MeshPointIterator &rclI) const
  { return _clIter < rclI._clIter; }
  /// Compares if this iterator points to a higher element than the other one.
  bool operator > (const MeshPointIterator &rclI) const
  { return _clIter > rclI._clIter; }
  /// Checks if the iterators points to the same element.
  bool operator == (const MeshPointIterator &rclI) const
  { return _clIter == rclI._clIter; }
  /// Sets the iterator to the beginning of the array.
  void Begin (void)
  { _clIter = _rclMesh._aclPointArray.begin(); }
  /// Sets the iterator to the end of the array.
  void End (void)
  { _clIter = _rclMesh._aclPointArray.end(); }
  /// Returns the current position of the iterator in the array.
  unsigned long Position (void) const
  { return _clIter - _rclMesh._aclPointArray.begin(); }
  /// Checks if the end is already reached.
  bool EndReached (void) const
  { return !(_clIter < _rclMesh._aclPointArray.end()); }
  /// Sets the iterator to the beginning of the array.
  void  Init (void)
  { Begin(); }
  /// Checks if the end is not yet reached.
  bool More (void) const
  { return !EndReached(); }
  /// Increments the iterator.
  void  Next (void)
  { operator++(); }
  /// Sets the iterator to a given position.
  inline bool Set (unsigned long ulIndex);
  /// Checks if the iterator points to a valid element inside the array.
  inline bool IsValid (void) const
  { return (_clIter >= _rclMesh._aclPointArray.begin()) && (_clIter < _rclMesh._aclPointArray.end()); }
  //@}

protected:
  inline const MeshPoint& Dereference (void) const
  { return *_clIter; }

protected:
  const MeshKernel& _rclMesh;
  MeshPoint _clFacet;
  MeshPointArray::_TConstIterator _clIter;

  // friends
  friend class MeshKernel;
};

#ifdef Use_EdgeList
class AppMeshExport MeshEdgeIterator
{
public:
  /** @name Construction */
  //@{
  inline MeshEdgeIterator (const MeshKernel &rclM);
  inline MeshEdgeIterator (const MeshKernel &rclM,  unsigned long ulPos);
  inline MeshEdgeIterator (const MeshEdgeIterator &rclI);
  //@}

  /** @name Access methods */
  //@{
  /// Access to the element the iterator points to.
  const MeshGeomEdge& operator*(void)
  { return Dereference(); }
  /// Access to the element the iterator points to.
  const MeshGeomEdge* operator->(void)
  { return &Dereference(); }
  /// Increments the iterator. It points then to the next element if the
  /// end is not reached.
  const MeshEdgeIterator& operator ++ (void)
  { _clIter++; return *this; }
  /// Decrements the iterator. It points then to the previous element if the beginning
  /// is not reached.
  const MeshEdgeIterator& operator -- (void)
  { _clIter--; return *this; }
  /// Assignment.
  inline MeshEdgeIterator& operator = (const MeshEdgeIterator &rpI);
  /// Compares if this iterator points to a lower element than the other one.
  bool operator < (const MeshEdgeIterator &rclI) const
  { return _clIter < rclI._clIter; }
  /// Compares if this iterator points to a higher element than the other one.
  bool operator > (const MeshEdgeIterator &rclI) const
  { return _clIter > rclI._clIter; }
  /// Checks if the iterators points to the same element.
  bool operator == (const MeshEdgeIterator &rclI) const
  { return _clIter == rclI._clIter; }
  /// Sets the iterator to the beginning of the array.
  void Begin (void)
  { _clIter = _rclMesh._aclEdgeArray.begin(); }
  /// Sets the iterator to the end of the array.
  void End (void)
  { _clIter = _rclMesh._aclEdgeArray.end(); }
  /// Returns the current position of the iterator in the array.
  unsigned long Position (void) const
  { return _clIter - _rclMesh._aclEdgeArray.begin(); }
  /// Checks if the end is already reached.
  bool EndReached (void) const
  { return !(_clIter < _rclMesh._aclEdgeArray.end()); }
  /// Sets the iterator to the beginning of the array.
  void  Init (void)
  { Begin(); }
  /// Checks if the end is not yet reached.
  bool More (void) const
  { return !EndReached(); }
  /// Increments the iterator.
  void  Next (void)
  { operator++(); }
  /// Sets the iterator to a given position.
  inline bool Set (unsigned long ulIndex);
  /// Determines the indices of the element.
  inline MeshIndexEdge& GetIndicies (void);
  /// Determines the indices of the corners of the edge
  inline MeshHelpEdge& GetPointIndicies (void);
  /// Checks if the iterator points to a valid element inside the array.
  inline bool IsValid (void) const
  { return (_clIter >= _rclMesh._aclEdgeArray.begin()) && (_clIter < _rclMesh._aclEdgeArray.end()); }
  //@}

protected:
  inline const MeshGeomEdge& Dereference (void);

protected:
  const MeshKernel& _rclMesh;
  MeshGeomEdge      _clEdge;
  MeshHelpEdge      _clHelpEdge;
  MeshIndexEdge     _clIndexEdge;
  MeshEdgeArray::_TConstIterator _clIter;

  // friends
  friend class MeshKernel;
};
#endif


class MeshFastFacetIterator
{
public:
  inline MeshFastFacetIterator (const MeshKernel &rclM);
  virtual ~MeshFastFacetIterator () {}

  void Init (void) { _clIter = _rclFAry.begin(); }
  inline void Next (void);
  bool More (void) { return _clIter != _rclFAry.end(); }

  Vector3D _afPoints[3];

protected:
  const MeshKernel&     _rclMesh;
  const MeshFacetArray& _rclFAry;
  const MeshPointArray& _rclPAry;
  MeshFacetArray::_TConstIterator _clIter;

private:
  MeshFastFacetIterator (const MeshFastFacetIterator&);
  void operator = (const MeshFastFacetIterator&);
};

inline MeshFastFacetIterator::MeshFastFacetIterator (const MeshKernel &rclM)
: _rclMesh(rclM),
  _rclFAry(rclM._aclFacetArray),
  _rclPAry(rclM._aclPointArray),
  _clIter(_rclFAry.begin())
{
}

inline void MeshFastFacetIterator::Next (void)
{
  const unsigned long *paulPt = _clIter->_aulPoints;
  Vector3D *pfPt = _afPoints;
  *(pfPt++)      = _rclPAry[*(paulPt++)];
  *(pfPt++)      = _rclPAry[*(paulPt++)];
  *pfPt          = _rclPAry[*paulPt];
}

inline MeshFacetIterator::MeshFacetIterator (const MeshKernel &rclM)
: _rclMesh(rclM),
  _rclFAry(rclM._aclFacetArray),
  _rclPAry(rclM._aclPointArray),
  _clIter(rclM._aclFacetArray.begin())
{
}

inline MeshFacetIterator::MeshFacetIterator (const MeshKernel &rclM, unsigned long ulPos)
: _rclMesh(rclM),
  _rclFAry(rclM._aclFacetArray),
  _rclPAry(rclM._aclPointArray),
  _clIter(rclM._aclFacetArray.begin() + ulPos)
{
}

inline MeshFacetIterator::MeshFacetIterator (const MeshFacetIterator &rclI)
: _rclMesh(rclI._rclMesh),
  _rclFAry(rclI._rclFAry),
  _rclPAry(rclI._rclPAry),
  _clIter(rclI._clIter)
{
}

inline const MeshGeomFacet& MeshFacetIterator::Dereference (void)
{
  MeshFacet rclF             = *_clIter;
  const unsigned long *paulPt        = &(_clIter->_aulPoints[0]);
  Vector3D  *pclPt = _clFacet._aclPoints;
  *(pclPt++)       = _rclPAry[*(paulPt++)];
  *(pclPt++)       = _rclPAry[*(paulPt++)];
  *pclPt           = _rclPAry[*paulPt];
  _clFacet._ulProp = rclF._ulProp;
  _clFacet._ucFlag = rclF._ucFlag;
  _clFacet.NormalInvalid();
  return _clFacet;
}

inline bool MeshFacetIterator::Set (unsigned long ulIndex)
{
  if (ulIndex < _rclMesh._aclFacetArray.size())
  {
    _clIter    = _rclMesh._aclFacetArray.begin() + ulIndex;
    return true;
  }
  else
  {
    _clIter    = _rclMesh._aclFacetArray.end();
    return false;
  }
}

inline MeshFacetIterator& MeshFacetIterator::operator = (const MeshFacetIterator &rpI)
{
  _clIter  = rpI._clIter;
  // dirty flat copy of reference
  memcpy((void*)&_rclMesh, &rpI._rclMesh, sizeof(&_rclMesh));
  return *this;
}

inline unsigned long MeshFacetIterator::GetProperty (void) const
{
  return _clIter->_ulProp;
}

inline void MeshFacetIterator::GetNeighbours (MeshFacetIterator &rclN0, MeshFacetIterator &rclN1, MeshFacetIterator &rclN2) const
{
  if (_clIter->_aulNeighbours[0] != ULONG_MAX)
    rclN0.Set(_clIter->_aulNeighbours[0]);
  else
    rclN0.End();

  if (_clIter->_aulNeighbours[1] != ULONG_MAX)
    rclN1.Set(_clIter->_aulNeighbours[1]);
  else
    rclN1.End();

  if (_clIter->_aulNeighbours[2] != ULONG_MAX)
    rclN2.Set(_clIter->_aulNeighbours[2]);
  else
    rclN2.End();
}

inline void MeshFacetIterator::SetToNeighbour (unsigned short usN)
{ 
  if (_clIter->_aulNeighbours[usN] != ULONG_MAX)
    _clIter = _rclMesh._aclFacetArray.begin() + _clIter->_aulNeighbours[usN];
  else
    End();
}

inline MeshPointIterator::MeshPointIterator (const MeshKernel &rclM)
: _rclMesh(rclM)
{
  _clIter = _rclMesh._aclPointArray.begin();
}

inline MeshPointIterator::MeshPointIterator (const MeshKernel &rclM, unsigned long ulPos)
: _rclMesh(rclM)
{
  _clIter = _rclMesh._aclPointArray.begin() + ulPos;
}

inline MeshPointIterator::MeshPointIterator (const MeshPointIterator &rclI)
: _rclMesh(rclI._rclMesh),
  _clIter(rclI._clIter)
{
}

inline bool MeshPointIterator::Set (unsigned long ulIndex)
{
  if (ulIndex < _rclMesh._aclPointArray.size())
  {
    _clIter = _rclMesh._aclPointArray.begin() + ulIndex;
    return true;
  }
  else
  {
    _clIter = _rclMesh._aclPointArray.end();
    return false;
  }
}

inline MeshPointIterator& MeshPointIterator::operator = (const MeshPointIterator &rpI)
{
  _clIter  = rpI._clIter;
  // dirty flat copy of reference
  memcpy((void*)&_rclMesh, &rpI._rclMesh, sizeof(&_rclMesh));
  return *this;
}

#ifdef Use_EdgeList
inline MeshEdgeIterator::MeshEdgeIterator (const MeshKernel &rclM)
: _rclMesh(rclM)
{
  _clIter = _rclMesh._aclEdgeArray.begin();
}

inline MeshEdgeIterator::MeshEdgeIterator (const MeshKernel &rclM,  unsigned long ulPos)
: _rclMesh(rclM)
{
  _clIter = _rclMesh._aclEdgeArray.begin() + ulPos;
}

inline MeshEdgeIterator::MeshEdgeIterator (const MeshEdgeIterator &rclI)
: _rclMesh(rclI._rclMesh),
  _clIter(rclI._clIter)
{
}

inline const MeshGeomEdge& MeshEdgeIterator::Dereference (void)
{ 
  MeshHelpEdge clH;
  unsigned short usSide  = (unsigned short)((*_clIter).Side());
  unsigned long  ulIndex = (*_clIter).Index();
  _rclMesh._aclFacetArray[ulIndex].GetEdge(usSide, clH);
  _clEdge._aclPoints[0] = _rclMesh._aclPointArray[clH._ulIndex[0]];
  _clEdge._aclPoints[1] = _rclMesh._aclPointArray[clH._ulIndex[1]];
  _clEdge._bBorder      = _rclMesh._aclFacetArray[ulIndex]._aulNeighbours[usSide] == ULONG_MAX;
  _clEdge._ucFlag       = _clIter->_ucFlag;
  return _clEdge;
}

inline MeshIndexEdge& MeshEdgeIterator::GetIndicies (void)
{
  unsigned short usSide = (unsigned short)((*_clIter).Side());
  _clIndexEdge._ulFacetIndex = (*_clIter).Index();
  _clIndexEdge._ausCorner[0] = usSide;
  _clIndexEdge._ausCorner[1] = (usSide + 1) % 3;
  return _clIndexEdge;
}

inline MeshHelpEdge& MeshEdgeIterator::GetPointIndicies (void)
{
  unsigned short usSide        = (unsigned short)((*_clIter).Side());
  unsigned long  ulFacetIndex  = (*_clIter).Index();
  _clHelpEdge._ulIndex[0] = _rclMesh._aclFacetArray[ulFacetIndex]._aulPoints[usSide];
  _clHelpEdge._ulIndex[1] = _rclMesh._aclFacetArray[ulFacetIndex]._aulPoints[(usSide+1)%3];
  return _clHelpEdge;
}

inline bool MeshEdgeIterator::Set (unsigned long ulIndex)
{
  if (ulIndex < _rclMesh._aclEdgeArray.size())
  {
    _clIter = _rclMesh._aclEdgeArray.begin() + ulIndex;
    return true;
  }
  else
  {
    _clIter = _rclMesh._aclEdgeArray.end();
    return false;
  }
}

inline MeshEdgeIterator& MeshEdgeIterator::operator = (const MeshEdgeIterator &rpI)
{
  _clIter  = rpI._clIter;
  // dirty flat copy of reference
  memcpy((void*)&_rclMesh, &rpI._rclMesh, sizeof(&_rclMesh));
  return *this;
}
#endif


} // namespace Mesh


#endif // MESH_ITERATOR_H 
