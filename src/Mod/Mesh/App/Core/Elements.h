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


#ifndef __MESH_ELEMENTS_HXX__
#define __MESH_ELEMENTS_HXX__

#include <vector>

#include "Definitions.h"
#include "BoundBox.h"  // Added by ClassView


namespace Mesh {

class Vector3D;
class MeshHelpEdge;
class MeshPoint;
class AppMeshExport MeshHelpEdge
{
    public:
    inline bool operator == (const MeshHelpEdge &rclEdge) const;

    public:
    unsigned long   _ulIndex[2];  // point indicies
};


class AppMeshExport MeshIndexEdge
{
    public:
    unsigned long  _ulFacetIndex;  // Facet Index
    public:
    unsigned short _ausCorner[2];  // Eckpunkt-Indizies des Facet
};


// stellt ein Punkt in der Mesh-Datenstruktur dar. Erbt von der Klasse Vector3D   
// und hat zusätzlich noch einen Flagstatus und freiverwendbare Properties. Die    
// Flags werden über die Methoden Set-/Reset-/Is-Flag modifiziert. Auf die         
// Properties ist wahlfreier Zugriff möglich. (siehe auf Text Properties)
// Ein Punkt kann einen (temporär) ungültigen Zustand haben (z.B. beim Löschen von 
// mehreren Punkte), darf aber im allgemeinen nicht gesetzt sein; d.h. immer nur   
// innerhalb eines Mesh-internen Alogrihtmus verwendbar.
class MeshPoint: public Vector3D
{
    public:
    enum TFlagType {INVALID=1, VISIT=2, SEGMENT=4, MARKED=8, REV1=16, REV2=32, TMP0=64, TMP1=128};

    public:
    void SetFlag (TFlagType tF)
    { _ucFlag |= (unsigned char)(tF); }
    public:
    void ResetFlag (TFlagType tF)
    { _ucFlag &= ~(unsigned char)(tF); }
    public:
    bool IsFlag (TFlagType tF) const
    { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF);  }
    public:
    MeshPoint (void) : _ucFlag(0), _ulProp(0),_ulProbeProp(0)
    {}
    // -------------------------------------------------------------------
    // DES: loescht das Facet an der Pos. pIter aus dem Array. Die Nachbar-
    //   Indizies der anderen Facets werden entspr. angepasst.
    // PAR: pIter:  zu loeschendes Facet
    // RET: void
    // --------------------------------------------------------------------
    public:
    inline MeshPoint (const Vector3D &rclPt);

    public:
    inline MeshPoint (const MeshPoint &rclPt);

    public:
    ~MeshPoint (void)
    {}

    public:
    inline MeshPoint& operator = (const MeshPoint &rclPt);

    // compare operator
    public:
    inline bool operator == (const MeshPoint &rclPt) const;
    public:
    inline bool operator == (const Vector3D &rclV) const;
    friend DataStream& operator << (DataStream &rclOut, const MeshPoint &rclPt)
    {
      return rclOut << rclPt.x << rclPt.y << rclPt.z;
    }
    friend DataStream& operator >> (DataStream &rclIn, MeshPoint &rclPt)
    {
      return rclIn >> rclPt.x >> rclPt.y >> rclPt.z;
    }

    public:
    inline bool operator < (const MeshPoint &rclPt) const;

    public:
    void ResetInvalid (void)
    { ResetFlag(INVALID); }

    public:
    void  SetInvalid (void)
    { SetFlag(INVALID); }
    public:
    bool IsValid (void) const
    { return !IsFlag(INVALID); }
    // Flag, kann mit den Methoden XXXFlag modifizert werden.
    public:
    unsigned char _ucFlag;
    public:
    union 
    {
      unsigned long   _ulProp;
      float    _fProp;
      void    *_pProp;
    };
    unsigned long		_ulProbeProp;
};

// Stellt eine Dreieckskante in der Mesh-Datenstruktur dar. Ein Kante indiziert    
// jeweils auf ein Dreieck in dem Facetarray. Zusätzlich ich die Kantenfolgennummer
// (siehe Beschreibung MeshFacet) des indizierten Dreiecks abgelegt. Beide Daten  
// werden in einem unsigned long (unsigned long) durch Binärverkettung abgelegt. Dabei    
// entsprechen die 30 oberen Bits dem Dreichsindex in dem Facetarry, die 2 unteren 
// Bits der Kantenfolgennummer. Folgendes Beispiel mag dies verdeutlichen.
// 
// Eine Kante indiziert das Dreieck mit dem Index 3 und der Kantenfolgennummer 1.  
// Der Wert wird dann generiert durch
// 3 << 2 | 1  ===>  0,1 Bit = Kantenfolgenummer  2-32 Bit = Index
// 
// Es stehen verschiedene Methoden zum Setzen und Auslesen des                     
// Index,Kantenfolgenummer zu Verfügung.
// Ausserdem hat eine Kante noch einen Flagstatus der über die üblichen Methode    
// modifiziert werden kann.
class MeshEdge
{
    public:
    enum TFlagType {INVALID=1, VISIT=2, SEGMENT=4, MARKED=8, REV1=16, REV2=32, TMP0=64, TMP1=128};
    public:
    void SetFlag (TFlagType tF)
    { _ucFlag |= (unsigned char)(tF); }
    public:
    void ResetFlag (TFlagType tF)
    { _ucFlag &= ~(unsigned char)(tF); }
    public:
    bool IsFlag (TFlagType tF) const
    { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF); }

    public:
    void ResetInvalid (void)
    { ResetFlag(INVALID); }

    public:
    void  SetInvalid (void)
    { SetFlag(INVALID); }

    public:
    bool IsValid (void) const
    { return !IsFlag(INVALID); }
 
    public:
    MeshEdge (void) : _ulIndex(0) 
    {}

    public:
    MeshEdge (const MeshEdge &rclE) : _ulIndex(rclE._ulIndex), _ucFlag(rclE._ucFlag)
    {}

    public:
    MeshEdge (unsigned long ulI) : _ulIndex(ulI) 
    {}
    public:
    MeshEdge (unsigned long ulFInd, unsigned long ulSide) : _ulIndex((ulFInd << 2) | (ulSide & 3)), _ucFlag(0)
    {}
   
    public:
    inline MeshEdge& operator = (const MeshEdge &rclE);

    // operators
    public:
    unsigned long operator () (void)
    { return _ulIndex; }
    public:
    bool operator < (const MeshEdge &rclM) const
    { return _ulIndex < rclM._ulIndex; }
    public:
    bool operator > (const MeshEdge &rclM) const
    { return _ulIndex > rclM._ulIndex; }
    public:
    bool operator == (const MeshEdge &rclM) const
    { return _ulIndex == rclM._ulIndex; }
    friend DataStream& operator << (DataStream &rclOut, const MeshEdge &rclE)
    {
      return rclOut << rclE._ulIndex;
    }
    friend DataStream& operator >> (DataStream &rclIn, MeshEdge &rclE)
    {
      return rclIn >> rclE._ulIndex;
    }
    // set index and edge-number
    public:
    void Set (unsigned long ulFInd, unsigned long ulSide)
    { _ulIndex = (ulFInd << 2) | (ulSide & 3); }
    // return the facet index
    public:
    unsigned long  Index (void) const
    { return _ulIndex >> 2; }
    // return the edge-number of the facet
    public:
    unsigned long  Side (void) const
    { return _ulIndex & 3; }
    // index and edge-number
    public:
    unsigned long  _ulIndex;
    // Flag, wird über die Methoden Set-/Reset-/Is-Flag modifiziert
    public:
    unsigned char _ucFlag;
};

class AppMeshExport MeshGeomEdge
{
    public:
    MeshGeomEdge (void) : _bBorder(FALSE) 
    {}

    public:
    Vector3D _aclPoints[2];  // Eckpunkte
    public:
    bool     _bBorder;       // TRUE: Grenzkante

    public:
    unsigned char _ucFlag;

    public:
    bool IsFlag (MeshEdge::TFlagType tF) const
    { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF); }

};

class CRSGeoFacetModel;
class MeshFacet
{
    public:
    enum TFlagType {INVALID=1, VISIT=2, SEGMENT=4, MARKED=8, REV1=16, REV2=32, TMP0=64, TMP1=128};
    public:
    void SetFlag (TFlagType tF)
    { _ucFlag |= (unsigned char)(tF); }
    public:
    void ResetFlag (TFlagType tF)
    { _ucFlag &= ~(unsigned char)(tF); }
    public:
    bool IsFlag (TFlagType tF) const
    { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF); }
    public:
    inline MeshFacet (void);

    public:
    inline MeshFacet(const MeshFacet &rclF);

    public:
    ~MeshFacet (void)
    {}

    public:
    inline MeshFacet& operator = (const MeshFacet &rclF);

    friend DataStream& operator << (DataStream &rclOut, const MeshFacet &rclF)
    {
      return rclOut << rclF._aulNeighbours[0] << rclF._aulNeighbours[1] << rclF._aulNeighbours[2] <<
                       rclF._aulPoints[0]     << rclF._aulPoints[1]     << rclF._aulPoints[2] <<
                       rclF._ucFlag;
    }
    friend DataStream& operator >> (DataStream &rclIn, MeshFacet &rclF)
    {
      return rclIn >> rclF._aulNeighbours[0] >> rclF._aulNeighbours[1] >> rclF._aulNeighbours[2] >>
                      rclF._aulPoints[0]     >> rclF._aulPoints[1]     >> rclF._aulPoints[2] >>
                      rclF._ucFlag;
    }
    // getters, setters
    public:
    inline void GetEdge (unsigned short usSide, MeshHelpEdge &rclEdge) const;
    // ermittelt die Kantenfolgenummer des Facets dessen Kante gemeinsam ist dem       
    // übergebenen Nachbar(als Index).

    // ermittelt die Kantenfolgenummer des Facets dessen Kante gemeinsam ist dem       
    // übergebenen Nachbar(als Index).
    public:
    inline std::pair<unsigned long, unsigned long> GetEdge (unsigned short usSide) const;

    public:
    inline unsigned short Side (unsigned long ulNIndex) const;

    // ermittelt Kantennummer aus den Eckpunkten(-indizes)
    public:
    inline unsigned short Side (unsigned long ulP0, unsigned long P1) const;

    // transpose point index
    public:
    inline void Transpose (unsigned long ulOrig, unsigned long ulNew);
    // decrement index
    public:
    inline void Decrement (unsigned long ulIndex);
    // replace the neighbour index
    public:
    inline void ReplaceNeighbour (unsigned long ulOrig, unsigned long ulNew);
    // Abfrage, ob das Facets an der entspr. Kante (nach Kantenfolgenummer) ein        
    // Nachbar besitzt.
    public:
    bool HasNeighbour (unsigned short usSide) const
    { return (_aulNeighbours[usSide] != ULONG_MAX); }

    public:
    void ResetInvalid (void)
    { ResetFlag(INVALID); }

    // Dreieck ungueltig setzen, bzw Abfrage. Sollte nur innerhalb eines Algorithmus   
    // verwendet werden. (z.B. beim gleichzeitigen Löschen von mehreren Facets). Die   
    // Datenstruktur darf nach aussen keine ungültigen Facets enthalten.

    public:
    void  SetInvalid (void)
    { SetFlag(INVALID); }
    // siehe SetInvalid Methode
    public:
    bool IsValid (void) const
    { return !IsFlag(INVALID); }
    // umdrehen der Normal (genauer Umlaufrichtung). Die Kantenarray muss nach         
    // umdrehen von Normalen neu aufgebaut werden, bzw. muss die entspr. Kante         
    // korrigiert werden.
    public:
    void FlipNormal (void)
    {
      std::swap(_aulPoints[1], _aulPoints[2]);
      std::swap(_aulNeighbours[0], _aulNeighbours[2]);
    }

    public:
    unsigned char _ucFlag;
    public:
    union 
    {
      unsigned long   _ulProp;
      float    _fProp;
      unsigned short  _usProp;
      void    *_pProp;
    };
    unsigned long				_ulProbeProp;
	CRSGeoFacetModel * _pclGeoFacetModel;
    public:
    unsigned long      _aulPoints[3];     // Indizies Eckpunkte
    public:
    unsigned long      _aulNeighbours[3]; // Indizies Nachbar-Facets

};


class AppMeshExport MeshGeomFacet 
{
    public:
    MeshGeomFacet (void) : _bNormalCalculated(false)
    {}
    public:
    ~MeshGeomFacet (void)
    {}
    // -------------------------------------------------------------------
    // DES: Ueberprueft, ob der Punkt dem Facet zugeordnet werden kann. Ein
    //   Punkt gehoert einem Facet wenn der Abstand zur Dreiecksflaeche kleiner
    //   fDistance ist und wenn sich der Punkt innerhalb des Dreiecks befindet.
    // PAR: rclPoint:  Punkt
    //      fDistance: max. Abstand zur Dreiecksflaeche
    // RET: bool: TRUE, wenn Punkt zum Dreieck gehoert
    // --------------------------------------------------------------------
    public:
    bool IsPointOf (const Vector3D &rclPoint, float fDistance) const;
    // distance between point and facet-plane
    public:
    inline float Distance (const Vector3D &rclPoint) const;
    // -------------------------------------------------------------------
    // DES: Ausdehnen des Facets
    // PAR: fDist: neuer Kanten-Abstand
    // RET: void:
    // --------------------------------------------------------------------
    public:
    void Enlarge (float fDist);
    // calculate normal form points
    public:
    inline void CalcNormal (void);
    // arrange normal
    public:
    inline void ArrangeNormal (const Vector3D &rclN);

    // Umlaufrichtung der Eckpunkte an Normal anpassen
    public:
    inline void AdjustCirculationDirection (void);

    public:
      void NormalInvalid (void) { _bNormalCalculated = false; }

    public:
    bool IsFlag (MeshFacet::TFlagType tF) const
    { return (_ucFlag & (unsigned char)(tF)) == (unsigned char)(tF); }

    public:
    inline Vector3D GetGravityPoint (void) const;

    public:
    inline Vector3D GetNormal (void) const;
    inline void      SetNormal (const Vector3D &rclNormal);

    protected:
    Vector3D  _clNormal;
  
    protected:
    bool  _bNormalCalculated;

    public:
    Vector3D  _aclPoints[3];
      public:
      inline BoundBox3D GetBoundBox (void) const;

    public:
    unsigned char _ucFlag;

    union 
    {
      unsigned long   _ulProp;
      float    _fProp;
      void    *_pProp;
    };
};


// verwaltet alle Punkte in der Mesh-Datenstruktur
typedef  std::vector<MeshPoint>  TMeshPointArray;
class MeshPointArray: public TMeshPointArray
{
  public:
    typedef std::vector<MeshPoint>::iterator        _TIterator;
    typedef std::vector<MeshPoint>::const_iterator  _TConstIterator;

    // setzen eines Flags für alle Punkte
    public:
    void SetFlag (MeshPoint::TFlagType tF);
    
    // rücksetzen eines Flags für alle Punkte
    public:
    void ResetFlag (MeshPoint::TFlagType tF);

     public:
     void ResetInvalid (void);
    
     public:
     void SetProperty (unsigned long ulVal);

    // constructors, destructor
    public:
    MeshPointArray (void)
    {}

    public:
    MeshPointArray (unsigned long ulSize) : TMeshPointArray(ulSize)
    {}
    public:
    ~MeshPointArray (void)
    {}

    public:
    MeshPointArray& operator = (const MeshPointArray &rclPAry);

    // -------------------------------------------------------------------
    // DES: sucht den zugehoerigen Index des Punkts. Zwei Punkte sind gleich,
    //   wenn sich der Abstand den minimalen Wert (s. definitions.hxx) 
    //   unterschreitet
    // PAR:  rclPoint:  Punkt
    // RET:  unsigned long:    Index, ULONG_MAX wenn kein Index gefunden
    // --------------------------------------------------------------------
    public:
    unsigned long Get (const MeshPoint &rclPoint);
    // -------------------------------------------------------------------
    // DES: gibt den Index des Punktes zurueck, bzw. fuegt einer neuer Punkt
    //   an und gibt dessen Index an. Zwei Punkte sind gleich wenn der Abstand
    //   den minimalen Abstand (s. definitions.hxx) unterschreitet
    // PAR:  rclPoint:  Punkt
    // RET:  unsigned long:    Index
    // --------------------------------------------------------------------
    public:
    unsigned long GetOrAddIndex (const MeshPoint &rclPoint);
};

// verwalten der Kanten in der Mesh-Datenstruktur
typedef std::vector<MeshEdge> TMeshEdgeArray;

class MeshEdgeArray: public TMeshEdgeArray
{
  public:
    typedef std::vector<MeshEdge>::iterator        _TIterator;
    typedef std::vector<MeshEdge>::const_iterator  _TConstIterator;

    // setzen eines Flags für alle Kanten
    public:
    void SetFlag (MeshEdge::TFlagType tF);
    
    // rücksetzen eines Flags für alle Kanten
    public:
    void ResetFlag (MeshEdge::TFlagType tF);
    
    // constructors, destructor
    public:
    MeshEdgeArray (void)
    {}
    
    public:
    MeshEdgeArray (unsigned long ulSize) : TMeshEdgeArray(ulSize)
    {}

    public:
    ~MeshEdgeArray (void)
    {}

    public:
    MeshEdgeArray& operator = (const MeshEdgeArray &rclEAry);

    // add new edge: ulIndex := facet-index  ulSide := edge-number of facet
    public:
    inline void Add (unsigned long ulFacetIndex, unsigned long ulSide);

    // search for an edge 
    public:
    inline unsigned long Find (unsigned long ulIndex, unsigned long ulSide) const;
 
    // -------------------------------------------------------------------
    // DES: Passt die Facet-Indizies eines entfernten Facet der Kanten an.
    //   D.h. alle nachfolgende Indizies werden um 1 dekrementiert.
    // PAR: ulIndex:  Facet-Index der entfernt wurde
    // RET: void
    // --------------------------------------------------------------------
    public:
    void AdjustIndex (unsigned long ulIndex);

  friend class MeshEdgeIterator;
};

// Array zum verwalten der Facets in der Mesh-Datenstruktur
typedef std::vector<MeshFacet>  TMeshFacetArray;

class MeshFacetArray: public TMeshFacetArray
{
  public:
    typedef std::vector<MeshFacet>::iterator        _TIterator;
    typedef std::vector<MeshFacet>::const_iterator  _TConstIterator;

    // globales Setzen eines Flags für alle Facets
    public:
    void SetFlag (MeshFacet::TFlagType tF);
    
    // Zurücksetzen eines Flags für alle Facets
    public:
    void ResetFlag (MeshFacet::TFlagType tF);

     public:
     void ResetInvalid (void);

     public:
     void SetProperty (unsigned long ulVal);
    
    public:
    MeshFacetArray (void)
    {}
    public:
    MeshFacetArray (unsigned long ulSize) : TMeshFacetArray(ulSize)
    {}

    public:
    ~MeshFacetArray (void)
    {}

    public:
    MeshFacetArray& operator = (const MeshFacetArray &rclFAry);

    // -------------------------------------------------------------------
    // DES: loescht das Facet an der Pos. pIter aus dem Array. Die Nachbar-
    //   Indizies der anderen Facets werden entspr. angepasst.
    // PAR: pIter:  zu loeschendes Facet
    // RET: void
    // --------------------------------------------------------------------
    public:
    void Erase (_TIterator pIter);

    // -------------------------------------------------------------------
    // DES: prueft und vertauscht evt. die Punkt-Indizies der Facets
    // PAR: ulOrig: zu korrigierender Punkt-Index
    //      ulNew:  neuer Punkt-Index
    // RET: void
    // --------------------------------------------------------------------
    public:
    void TransposeIndicies (unsigned long ulOrig, unsigned long ulNew);
    // -------------------------------------------------------------------
    // DES: dekrementiert alle Punkt-Indizies die groesser ulIndex sind
    // PAR: ulIndex:  Index
    // RET: void
    // --------------------------------------------------------------------
    public:
    void DecrementIndicies (unsigned long ulIndex);
    
    
};

// -------------------------------------------------------------------
// DES: loescht das Facet an der Pos. pIter aus dem Array. Die Nachbar-
//   Indizies der anderen Facets werden entspr. angepasst.
// PAR: pIter:  zu loeschendes Facet
// RET: void
// --------------------------------------------------------------------
inline MeshPoint::MeshPoint (const Vector3D &rclPt)
: Vector3D(rclPt),
  _ucFlag(0),
  _ulProp(0),
  _ulProbeProp(0)
{
}

inline MeshPoint::MeshPoint (const MeshPoint &rclPt)
: Vector3D(rclPt),
  _ucFlag(rclPt._ucFlag),
  _ulProp(rclPt._ulProp),
  _ulProbeProp(rclPt._ulProbeProp)
{
}

inline MeshPoint& MeshPoint::operator = (const MeshPoint &rclPt)
{
  Vector3D::operator=(rclPt);
  _ucFlag = rclPt._ucFlag;
  _ulProp = rclPt._ulProp;
  _ulProbeProp = rclPt._ulProbeProp;
  return *this;
}

// compare operator
inline bool MeshPoint::operator == (const MeshPoint &rclPt) const
{
  return DistanceP2(*this, rclPt) < MeshDefinitions::_fMinPointDistanceP2;
}

inline bool MeshPoint::operator == (const Vector3D &rclV) const
{
  return DistanceP2(*this, rclV) < MeshDefinitions::_fMinPointDistanceP2;
}

inline bool MeshPoint::operator < (const MeshPoint &rclPt) const
{
  if (fabs(x - rclPt.x) < MeshDefinitions::_fMinPointDistanceD1)
  {
    if (fabs(y - rclPt.y) < MeshDefinitions::_fMinPointDistanceD1)
    {
      if (fabs(z - rclPt.z) < MeshDefinitions::_fMinPointDistanceD1)
        return FALSE;
      else
        return z < rclPt.z;
    }
    else
      return y < rclPt.y;
  }
  else
    return x < rclPt.x;
}

inline MeshEdge& MeshEdge::operator = (const MeshEdge &rclE)
{
  _ulIndex = rclE._ulIndex;
  _ucFlag  = rclE._ucFlag;
  return *this;
}

inline bool MeshHelpEdge::operator == (const MeshHelpEdge &rclEdge) const
{
  return (((_ulIndex[0] == rclEdge._ulIndex[0]) && (_ulIndex[1] == rclEdge._ulIndex[1])) ||
          ((_ulIndex[0] == rclEdge._ulIndex[1]) && (_ulIndex[1] == rclEdge._ulIndex[0])));
}

// distance between point and facet-plane
inline float MeshGeomFacet::Distance (const Vector3D &rclPoint) const
{
  Vector3D clNorm(_clNormal);
  clNorm.Normalize();
  return float(fabs(rclPoint.DistanceToPlane(_aclPoints[0], clNorm)));
}

// calculate normal form points
inline void MeshGeomFacet::CalcNormal (void)
{
  _clNormal = (_aclPoints[1] - _aclPoints[0]) % (_aclPoints[2] - _aclPoints[0]);
  _clNormal.Normalize();
  _bNormalCalculated = true;
}

inline Vector3D MeshGeomFacet::GetNormal (void) const
{
  if (_bNormalCalculated == false)
    const_cast<MeshGeomFacet*>(this)->CalcNormal();

  return _clNormal;
}

inline void MeshGeomFacet::SetNormal (const Vector3D &rclNormal)
{
  _clNormal = rclNormal;
  _bNormalCalculated = true;
}

// arrange normal
inline void MeshGeomFacet::ArrangeNormal (const Vector3D &rclN)
{
  if ((rclN * _clNormal) < 0.0f)
    _clNormal = -_clNormal;
}

inline Vector3D MeshGeomFacet::GetGravityPoint (void) const
{
  return (1.0f / 3.0f) * (_aclPoints[0] + _aclPoints[1] + _aclPoints[2]);
}

// Umlaufrichtung der Eckpunkte an Normal ausrichten (Normal bereits berechnet)
inline void MeshGeomFacet::AdjustCirculationDirection (void)
{
  Vector3D clN = (_aclPoints[1] - _aclPoints[0]) % (_aclPoints[2] - _aclPoints[0]);
  if ((clN * _clNormal) < 0.0f)
    std::swap(_aclPoints[1], _aclPoints[2]);
}

inline BoundBox3D MeshGeomFacet::GetBoundBox (void) const
{
  return BoundBox3D(_aclPoints, 3);
}

inline MeshFacet::MeshFacet (void)
: _ucFlag(0),
  _ulProp(0),
  _ulProbeProp(0),
  _pclGeoFacetModel(NULL)
{
  memset(_aulNeighbours, ULONG_MAX, sizeof(ULONG_MAX) * 3);
  memset(_aulPoints, ULONG_MAX, sizeof(ULONG_MAX) * 3);
}

inline MeshFacet::MeshFacet(const MeshFacet &rclF)
: _ucFlag(rclF._ucFlag),
  _ulProp(rclF._ulProp),
  _ulProbeProp(rclF._ulProbeProp),
  _pclGeoFacetModel(rclF._pclGeoFacetModel)
{
  _aulPoints[0] = rclF._aulPoints[0];
  _aulPoints[1] = rclF._aulPoints[1];
  _aulPoints[2] = rclF._aulPoints[2];

  _aulNeighbours[0] = rclF._aulNeighbours[0];
  _aulNeighbours[1] = rclF._aulNeighbours[1];
  _aulNeighbours[2] = rclF._aulNeighbours[2];
}

inline MeshFacet& MeshFacet::operator = (const MeshFacet &rclF)
{
  _ucFlag          = rclF._ucFlag;
  _ulProp          = rclF._ulProp;
  _ulProbeProp     = rclF._ulProbeProp;
  _pclGeoFacetModel= rclF._pclGeoFacetModel;

  _aulPoints[0]    = rclF._aulPoints[0];
  _aulPoints[1]    = rclF._aulPoints[1];
  _aulPoints[2]    = rclF._aulPoints[2];

  _aulNeighbours[0] = rclF._aulNeighbours[0];
  _aulNeighbours[1] = rclF._aulNeighbours[1];
  _aulNeighbours[2] = rclF._aulNeighbours[2];

  return *this;
}

// getters, setters
inline void MeshFacet::GetEdge (unsigned short usSide, MeshHelpEdge &rclEdge) const
{
  rclEdge._ulIndex[0] = _aulPoints[usSide];
  rclEdge._ulIndex[1] = _aulPoints[(usSide+1) % 3];
}

// ermittelt die Kantenfolgenummer des Facets dessen Kante gemeinsam ist dem       
// übergebenen Nachbar(als Index).
inline std::pair<unsigned long, unsigned long> MeshFacet::GetEdge (unsigned short usSide) const
{
  return std::pair<unsigned long, unsigned long>(_aulPoints[usSide], _aulPoints[(usSide+1)%3]);
}

// transpose point index
inline void MeshFacet::Transpose (unsigned long ulOrig, unsigned long ulNew)
{
  if (_aulPoints[0] == ulOrig)
    _aulPoints[0] = ulNew;
  else if (_aulPoints[1] == ulOrig)
    _aulPoints[1] = ulNew;
  else if (_aulPoints[2] == ulOrig)
    _aulPoints[2] = ulNew;
}

// decrement index
inline void MeshFacet::Decrement (unsigned long ulIndex)
{
  if (_aulPoints[0] > ulIndex) _aulPoints[0]--;
  if (_aulPoints[1] > ulIndex) _aulPoints[1]--;
  if (_aulPoints[2] > ulIndex) _aulPoints[2]--;
}

// replace the neighbour index
inline void MeshFacet::ReplaceNeighbour (unsigned long ulOrig, unsigned long ulNew)
{
  if (_aulNeighbours[0] == ulOrig)
    _aulNeighbours[0] = ulNew;
  else if (_aulNeighbours[1] == ulOrig)
    _aulNeighbours[1] = ulNew;
  else if (_aulNeighbours[2] == ulOrig)
    _aulNeighbours[2] = ulNew;
}

// ermittelt die Kantenfolgenummer des Facets dessen Kante gemeinsam ist dem       
// übergebenen Nachbar(als Index).
inline unsigned short MeshFacet::Side (unsigned long ulNIndex) const
{
  if (_aulNeighbours[0] == ulNIndex)
    return 0;
  else if (_aulNeighbours[1] == ulNIndex)
    return 1;
  else if (_aulNeighbours[2] == ulNIndex)
    return 2;
  else
    return USHRT_MAX;
}

// ermittelt die Kantenfolgenummer aus den Eckpunktindizes
inline unsigned short MeshFacet::Side (unsigned long ulP0, unsigned long ulP1) const
{
  if (_aulPoints[0] == ulP0)
  {
    if (_aulPoints[1] == ulP1)
      return 0;  // Kante 0-1 ==> 0
    else if (_aulPoints[2] == ulP1)
      return 2;  // Kante 0-2 ==> 2
  }
  else if (_aulPoints[1] == ulP0)
  {
    if (_aulPoints[0] == ulP1)
      return 0; // Kante 1-0 ==> 0
    else if (_aulPoints[2] == ulP1)
      return 1; // Kante 1-2 ==> 1
  }
  else if (_aulPoints[2] == ulP0)
  {
    if (_aulPoints[0] == ulP1)
      return 2; // Kante 2-0 ==> 2
    else if (_aulPoints[1] == ulP1)
      return 1; // Kante 2-1 ==> 1
  }
  
  return USHRT_MAX;
}


// add new edge: ulIndex := facet-index  ulSide := edge-number of facet
inline void MeshEdgeArray::Add (unsigned long ulFacetIndex, unsigned long ulSide)
{
  MeshEdge  clEdge(ulFacetIndex, ulSide);
  insert(std::lower_bound(begin(), end(), clEdge), clEdge);  
}

// search for an edge 
inline unsigned long MeshEdgeArray::Find (unsigned long ulIndex, unsigned long ulSide) const
{
  std::vector<MeshEdge>::const_iterator pIter;
  unsigned long ulInd = (ulIndex << 2) | (ulSide & 3);
  pIter = begin();
  pIter = std::upper_bound(begin(), end(), MeshEdge(ulInd));
  if (pIter > begin())
    pIter--;
  if (pIter->_ulIndex == ulInd)
    return pIter - begin();
  else
    return ULONG_MAX;
}

class Vector3D;
class AppMeshExport MeshHelpEdge;
class BoundBox3D;
class MeshPoint;
#ifdef OVERREAD_FOLLOW
#include "cvector3.hxx"
#endif
template <class TCLASS>
class MeshIsFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
    public:
    bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
    { return rclElem.IsFlag(tFlag); }
};

// Abfrage auf Flagstatus
// Binäre STL-Funktionsobjekt zur Verwendung in generischen STL-Funktionen
template <class TCLASS>
class MeshIsNotFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
    public:
    bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
    { return !rclElem.IsFlag(tFlag); }
};

// Setzen eines Flag
// Binäre STL-Funktionsobjekt zur Verwendung in generischen STL-Funktionen
template <class TCLASS>
class MeshSetFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
    public:
    bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
    { rclElem.SetFlag(tFlag); return true; }
};

// Zurücksetzen eines Flags
// Binäre STL-Funktionsobjekt zur Verwendung in generischen STL-Funktionen
template <class TCLASS>
class MeshResetFlag: public std::binary_function<TCLASS, typename TCLASS::TFlagType, bool>
{
    public:
    bool operator () (TCLASS rclElem, typename TCLASS::TFlagType tFlag) const
    { rclElem.ResetFlag(tFlag); return true; }
};


} // namespace Mesh

#endif
