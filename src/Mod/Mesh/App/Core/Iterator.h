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


#ifndef __MESH_ITERATOR_HXX__
#define __MESH_ITERATOR_HXX__

#include "MeshKernel.h"
#include "Elements.h"

namespace Mesh {

class MeshKernel;
class MeshGeomFacet;
class MeshPoint;
class MeshGeomEdge;
class MeshIndexEdge;
class MeshHelpEdge;

class MeshFacetIterator
{
  public:

    // constructor
    public:
    inline MeshFacetIterator (const MeshKernel &rclM);
    
    public:
    inline MeshFacetIterator (const MeshKernel &rclM, unsigned long ulPos);
    
    public:
    inline MeshFacetIterator (const MeshFacetIterator &rclI);
    
    public:
    const MeshGeomFacet& operator*(void)
    { return Dereference(); }

    public:
    const MeshGeomFacet* operator->(void)
    { return &Dereference(); }

    public:
    const MeshFacetIterator& operator ++ (void)
    { _clIter++; return *this; }

    public:
    const MeshFacetIterator& operator -- (void)
    { _clIter--; return *this; }

    public:
    const MeshFacetIterator& operator += (int k)
    { _clIter += k; return *this; }

    public:
    const MeshFacetIterator& operator -= (int k)
    { _clIter -= k; return *this; }

 
    public:
    inline MeshFacetIterator& operator = (const MeshFacetIterator &rpI);

    public:
    bool operator < (const MeshFacetIterator &rclI) const
    { return _clIter < rclI._clIter; }

    public:
    bool operator > (const MeshFacetIterator &rclI) const
    { return _clIter > rclI._clIter; }
    public:
    bool operator == (const MeshFacetIterator &rclI) const
    { return _clIter == rclI._clIter; }

    public:
    void Begin (void)
    { _clIter = _rclMesh._aclFacetArray.begin(); }

    public:
    void End (void)
    { _clIter = _rclMesh._aclFacetArray.end(); }

    public:
    unsigned long Position (void) const
    { return _clIter - _rclMesh._aclFacetArray.begin(); }

    public:
    bool EndReached (void) const
    { return !(_clIter < _rclMesh._aclFacetArray.end()); }
  
    public:
    void  Init (void)
    { Begin(); }
    public:
    bool More (void) const
    { return !EndReached(); }
    public:
    void  Next (void)
    { operator++(); }

    public:
    inline bool Set (unsigned long ulIndex);

    public:
    inline MeshFacet GetIndicies (void) const
    { return *_clIter; }
    public:
    inline void GetNeighbours (MeshFacetIterator &rclN0, MeshFacetIterator &rclN1, MeshFacetIterator &rclN2) const;

    public:
    inline void SetToNeighbour (unsigned short usN);

    public:
    inline unsigned long GetProperty (void) const;

    public:
    inline bool IsValid (void) const
    { return (_clIter >= _rclMesh._aclFacetArray.begin()) && (_clIter < _rclMesh._aclFacetArray.end()); }

    protected:
    inline const MeshGeomFacet& Dereference (void);
 
    protected:
    const MeshKernel &_rclMesh;

    protected:
    const MeshFacetArray             &_rclFAry;
    const MeshPointArray             &_rclPAry;

    protected:
    MeshFacetArray::_TConstIterator     _clIter;
    protected:
    MeshGeomFacet                     _clFacet;

  friend class MeshKernel;
};

class AppMeshExport MeshPointIterator
{
  public:

    public:
    inline MeshPointIterator (const MeshKernel &rclM);
    
    public:
    inline MeshPointIterator (const MeshKernel &rclM, unsigned long ulPos);
    
    public:
    inline MeshPointIterator (const MeshPointIterator &rclI);
    
    public:
    const MeshPoint& operator*(void) const
    { return Dereference(); }

    public:
    const MeshPoint* operator->(void) const
    { return &Dereference(); }

    public:
    const MeshPointIterator& operator ++ (void)
          { _clIter++; return *this; }

    public:
    const MeshPointIterator& operator -- (void)
          { _clIter--; return *this; }

    public:
    inline MeshPointIterator& operator = (const MeshPointIterator &rpI);

    public:
    bool operator < (const MeshPointIterator &rclI) const
    { return _clIter < rclI._clIter; }

    public:
    bool operator > (const MeshPointIterator &rclI) const
    { return _clIter > rclI._clIter; }
    
    public:
    bool operator == (const MeshPointIterator &rclI) const
    { return _clIter == rclI._clIter; }

    public:
    void Begin (void)
    { _clIter = _rclMesh._aclPointArray.begin(); }

    public:
    void End (void)
    { _clIter = _rclMesh._aclPointArray.end(); }

    public:
    unsigned long Position (void) const
    { return _clIter - _rclMesh._aclPointArray.begin(); }

    public:
    bool EndReached (void) const
    { return !(_clIter < _rclMesh._aclPointArray.end()); }

    public:
    void  Init (void)
    { Begin(); }
    public:
    bool More (void) const
    { return !EndReached(); }
    public:
    void  Next (void)
    { operator++(); }

    // constructor
    public:
    inline bool Set (unsigned long ulIndex);

    public:
    inline bool IsValid (void) const
    { return (_clIter >= _rclMesh._aclPointArray.begin()) && (_clIter < _rclMesh._aclPointArray.end()); }

    protected:
    inline const MeshPoint& Dereference (void) const
    { return *_clIter; }
 
    protected:
    const MeshKernel &_rclMesh;

    protected:
    MeshPoint                      _clFacet;
    protected:
    MeshPointArray::_TConstIterator     _clIter;

  friend class MeshKernel;
};

class AppMeshExport MeshEdgeIterator
{
  public:

    public:
    inline MeshEdgeIterator (const MeshKernel &rclM);
    
    public:
    inline MeshEdgeIterator (const MeshKernel &rclM,  unsigned long ulPos);
    
    public:
    inline MeshEdgeIterator (const MeshEdgeIterator &rclI);
    
    public:
    const MeshGeomEdge& operator*(void)
    { return Dereference(); }

    public:
    const MeshGeomEdge* operator->(void)
    { return &Dereference(); }

    public:
    const MeshEdgeIterator& operator ++ (void)
    { _clIter++; return *this; }

    public:
    const MeshEdgeIterator& operator -- (void)
    { _clIter--; return *this; }

    public:
    inline MeshEdgeIterator& operator = (const MeshEdgeIterator &rpI);

    public:
    bool operator < (const MeshEdgeIterator &rclI) const
    { return _clIter < rclI._clIter; }

    public:
    bool operator > (const MeshEdgeIterator &rclI) const
    { return _clIter > rclI._clIter; }
    public:
    bool operator == (const MeshEdgeIterator &rclI) const
    { return _clIter == rclI._clIter; }

    public:
    void Begin (void)
    { _clIter = _rclMesh._aclEdgeArray.begin(); }

    public:
    void End (void)
    { _clIter = _rclMesh._aclEdgeArray.end(); }

    public:
    unsigned long Position (void) const
    { return _clIter - _rclMesh._aclEdgeArray.begin(); }

    public:
    bool EndReached (void) const
    { return !(_clIter < _rclMesh._aclEdgeArray.end()); }

    public:
    void  Init (void)
    { Begin(); }
    public:
    bool More (void) const
    { return !EndReached(); }
    public:
    void  Next (void)
    { operator++(); }

    public:
    inline bool Set (unsigned long ulIndex);

    public:
    inline MeshIndexEdge& GetIndicies (void);

    public:
    inline MeshHelpEdge& GetPointIndicies (void);

    public:
    inline bool IsValid (void) const
    { return (_clIter >= _rclMesh._aclEdgeArray.begin()) && (_clIter < _rclMesh._aclEdgeArray.end()); }

    // constructor
    protected:
    inline const MeshGeomEdge& Dereference (void);
 
    protected:
    const MeshKernel  &_rclMesh;

    protected:
    MeshGeomEdge                _clEdge;
    protected:
    MeshHelpEdge                  _clHelpEdge;
    protected:
    MeshIndexEdge                 _clIndexEdge;
    protected:
    MeshEdgeArray::_TConstIterator     _clIter;

  friend class MeshKernel;
};


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
    const MeshKernel                   &_rclMesh;
    const MeshFacetArray             &_rclFAry;
    const MeshPointArray             &_rclPAry;
    MeshFacetArray::_TConstIterator   _clIter;

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
  Vector3D    *pfPt   = _afPoints;
  *(pfPt++)            = _rclPAry[*(paulPt++)];
  *(pfPt++)            = _rclPAry[*(paulPt++)];
  *pfPt                = _rclPAry[*paulPt];
}

class AppMeshExport MeshKernel;
class MeshGeomFacet;
class MeshFacet;
class MeshPoint;
class AppMeshExport MeshGeomEdge;
class AppMeshExport MeshIndexEdge;
class AppMeshExport MeshHelpEdge;
#include "Elements.h"
// constructor
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
  Vector3D    *pclPt         = _clFacet._aclPoints;
  *(pclPt++)                  = _rclPAry[*(paulPt++)];
  *(pclPt++)                  = _rclPAry[*(paulPt++)];
  *pclPt                      = _rclPAry[*paulPt];
  _clFacet._ulProp            = rclF._ulProp;
  _clFacet._ucFlag            = rclF._ucFlag;
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

// constructor
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

} // namespace Mesh


#endif
