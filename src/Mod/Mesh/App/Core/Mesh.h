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


#ifndef __MESH_MESH_HXX__
#define __MESH_MESH_HXX__

#include <assert.h>
#include "BoundBox.h"
#include "Elements.h"
#include "Helpers.h"

namespace Mesh {

class AppMeshExport MeshFacetIterator;
class AppMeshExport MeshEdgeIterator;
class AppMeshExport MeshPointIterator;
class AppMeshExport MeshGeomFacet;
class AppMeshExport Matrix4D;
class AppMeshExport DataStream;
class AppMeshExport MeshFacetIterator;
class AppMeshExport MeshEdgeIterator;
class AppMeshExport MeshPointIterator;
class AppMeshExport Vector3D;
class AppMeshExport MeshFacet;
class AppMeshExport MeshHelpEdge;
class AppMeshExport MeshFacetFunc;
class AppMeshExport CMeshDelaunay;
class AppMeshExport MeshSTL;
class AppMeshExport CMeshSearchNeighbours ;

class AppMeshExport MeshKernel
{
   public:
   MeshKernel (void);

   public:
    MeshKernel (const MeshKernel &rclMesh);
    public:
    virtual ~MeshKernel (void)
    {}
    // transformieren der Datenstruktur mit einer Transformations-Matrix
    public:
    virtual void operator *= (const Matrix4D &rclMat);
    
    // binäres streamen der Daten
    public:
    virtual void SaveStream (DataStream &rclOut);
    
    // binäres streamen der Daten
    public:
    virtual void RestoreStream (DataStream &rclIn);
    // -------------------------------------------------------------------
    // DES: Ueberprueft, ob der Punkt keinem anderen Facet mehr zugeordnet ist
    //   und loescht in im diesem Falle. Die Punkt-Indizies der Facets werden
    //   entsprechend angepasst.
    // PAR: ulIndex:         Index im Punkt-Array
    //      ulFacetIndex:    Index des quasi geloeschten Facet, wird nicht betrachtet
    //      bOnlySetInvalid: Der Punkt wird nicht geloescht, sondern nur invalid       
    // gesetzt                       Default-Wert: FALSE
    // RET: void:
    // --------------------------------------------------------------------
    public:
    unsigned long CountFacets (void) const
    { return (unsigned long)(_aclFacetArray.size()); }
    // ermittelt die Anzahl der Kanten
    public:
    unsigned long CountEdges (void) const
    { return (unsigned long)(_aclEdgeArray.size()); }
    // ermittelt die Anzahl der Punkte
    public:
    unsigned long CountPoints (void) const
    { return (unsigned long)(_aclPointArray.size()); }
    // ermittelt die BoundingBox
    public:
    BoundBox3D GetBoundBox (void) const
    { return const_cast<BoundBox3D&>(_clBoundBox); }

    public:
    void RecalcBoundBox (void);

    // gibt den Punkt an dem übergebenen Index zurück. Die Methode ist nicht sehr      
    // effektiv und  sollte nur für sporadische Zugriffe von Aussen verwendet werden.
    public:
    inline MeshPoint GetPoint (unsigned long ulIndex) const;
    // gibt die Kante an dem übergebenen Index zurück. Die Methode ist nicht sehr      
    // effektiv und  sollte nur für sporadische Zugriffe von Aussen verwendet werden.
    public:
    inline MeshGeomEdge GetEdge (unsigned long ulPos) const;
    // gibt das Facet an dem übergebenen Index zurück. Die Methode ist nicht sehr      
    // effektiv und  sollte nur für sporadische Zugriffe von Aussen verwendet werden.
    public:
    inline MeshGeomFacet GetFacet (unsigned long ulIndex) const;
    // -------------------------------------------------------------------
    // DES: hinzufuegen eines neuen Facets in die Mesh-Struktur
    // PAR: rclSFacet: Facet
    // RET: MeshKernel&:    Referenz diese Objekt
    // --------------------------------------------------------------------

    public:
    // gibt die Nachbarn zu einem Facet an
    inline void GetFacetNeighbours (unsigned long ulIndex, unsigned long &rulNIdx0, unsigned long &rulNIdx1, unsigned long &rulNIdx2);

    public:
    std::vector<MeshPoint>& GetPoints (void) { return _aclPointArray; }
    std::vector<MeshFacet>& GetFacets (void) { return _aclFacetArray; }

    public:
    MeshKernel& operator += (const MeshGeomFacet &rclSFacet);
    MeshKernel& operator += (const std::vector<MeshGeomFacet> &rclVAry);

	public:
		bool AddFacet(const MeshGeomFacet &rclSFacet);
		bool AddFacet(const std::vector<MeshGeomFacet> &rclVAry);
    // -------------------------------------------------------------------
    // DES: einfuegen von Facets
    // PAR: rclVAry: Facets, !!! dieses Array wird geloescht
    // RET: MeshKernel&: Referenz
    // --------------------------------------------------------------------
    public:
    MeshKernel& operator = (std::vector<MeshGeomFacet> &rclVAry);
    // -------------------------------------------------------------------
    // DES: Zuweisungs-Operator
    // PAR: rclMesh: zugewiesendes Objekt
    // RET: MeshKernel&: diese Instanz
    // --------------------------------------------------------------------

    protected:
    void Assign (MeshPointBuilder &rclMap);

    public:
    MeshKernel& operator = (const MeshKernel &rclMesh);
    // -------------------------------------------------------------------
    // DES: loeschen eines Facets. Das Loeschen eines Facets erfordert foldende
    //   Schritte: 
    //   - Nachbar-Index der Nachbar-Facets auf das geloeschte Facet ungueltig
    //     setzen. 
    //   - Wenn Nachbar-Facet existiert evt. Kanten-Index auf dieses Facet umbiegen.
    //     Wenn kein Nachbar-Facet existiert: Kante loeschen.
    //   - Indizies der Nachbar-Facets aller Facets geg. korrigieren.
    //   - Indizies der Kanten geg. korrigieren
    //   - Falls keine Nachbar-Facets existieren, Eckpunkte daraufhin ueberpruefen,
    //     ob sie geloescht werden koennen.
    // PAR: clIter:  Facet-Iterator, zeigt auf Facet das geloescht werden soll
    // RET: bool:   TRUE, wenn geloescht
    // --------------------------------------------------------------------
    public:
    bool DeleteFacet (const MeshFacetIterator &rclIter);
    // -------------------------------------------------------------------
    // DES: loeschen einer Kante
    // PAR: clIter:  Kanten-Iterator: zeigt auf die Kante die geloescht werden
    //   soll.
    //   Das Loeschen einer Kante erfordert folgende Schritte:
    //   - Loeschen der Facet(s), die der Kante zugeordnet sind.
    // RET: bool:   TRUE, wenn geloescht
    // --------------------------------------------------------------------
    public:
    bool DeleteEdge (const MeshEdgeIterator &rclIter);
    // -------------------------------------------------------------------
    // DES: loeschen eines Punkts. 
    // PAR: clIter:  Punkt-Iterator: zeigt auf den Punkt der geloescht werden
    //   soll. Das Loschen eines Punktes geschieht nach folgenden Schritte:
    //   - Feststellen der Facets, die dem Punkt zugeordnet sind.
    //   - Loeschen aller zugeordneten Facets
    // RET: bool:   TRUE, wenn geloescht
    // --------------------------------------------------------------------
    public:
    bool DeletePoint (const MeshPointIterator &rclIter);
    // -------------------------------------------------------------------
    // DES: ermittelt alle Facets die zu dem Punkt geheoren
    // PAR: rclIter:  Punkt-Iterator
    // RET: std::vector<unsigned long>: Array von Facet-Indizies
    // --------------------------------------------------------------------
    public:
    std::vector<unsigned long> HasFacets (const MeshPointIterator &rclIter) const;
    // translate one point
    public:
    inline void MovePoint (unsigned long ulPtIndex, const Vector3D &rclTrans);

    public:
    inline void GetFacetPoints (unsigned long ulFaIndex, unsigned long &rclP0, unsigned long &rclP1, unsigned long &rclP2) const;
    // -------------------------------------------------------------------
    // DES: alles Loeschen
    // PAR:  void
    // RET:  void:
    // --------------------------------------------------------------------
    public:
    void Clear (void);
 
    public:
    inline bool IsBorder (const MeshEdgeArray::_TIterator pPEdge) const;

    public:
    virtual void DeleteFacets (const std::vector<unsigned long> &raulFacets);

    public:
    virtual void DeletePoints (const std::vector<unsigned long> &raulPoints);

    public:
    virtual bool IsValid (void) const
    { return _bValid; }

    // anpassen der Umlaufrichtung an die Normale
    protected:
    inline void AdjustNormal (MeshFacet &rclFacet, const Vector3D &rclNormal);

    // berechnen eine normierten Normale des übergebenen Facet
    protected:
    inline Vector3D GetNormal (const MeshFacet &rclFacet) const;
    // -------------------------------------------------------------------
    // DES: Ueberprueft, ob der Punkt keinem anderen Facet mehr zugeordnet ist
    //   und loescht in im diesem Falle. Die Punkt-Indizies der Facets werden
    //   entsprechend angepasst.
    // PAR: ulIndex:         Index im Punkt-Array
    //      ulFacetIndex:    Index des quasi geloeschten Facet, wird nicht betrachtet
    //      bOnlySetInvalid: Der Punkt wird nicht geloescht, sondern nur invalid       
    // gesetzt                       Default-Wert: FALSE
    // RET: void:
    // --------------------------------------------------------------------
    protected:
    MeshPointArray  _aclPointArray;
    protected:
    MeshEdgeArray   _aclEdgeArray;
    protected:
    MeshFacetArray  _aclFacetArray;
    protected:
    BoundBox3D      _clBoundBox;

    protected:
    bool             _bValid;
    // -------------------------------------------------------------------
    // DES: Einfuegen neuer Kanten, bzw. updaten der vorhandenen. Alle Kanten
    //   des Facets werden ueberprueft auf Existenz. Bei Bedarf wird neue Kante
    //   eingefuegt. Die neuen Kanten werden entspr. dem Index in das RSsortierte
    //   Array eingefuegt. Gleichzeitig werden die Nachbar-Indicies der Facets
    //   neu gesetzt.
    // PAR:  rclFacet:     Facet in:  ohne gueltigte Nachbar-Indizies
    //                           out: gueltigte Nachbar-Indizies
    //       ulFacetIndex: Index des Facets im Facet-Array
    // RET:  void:
    // --------------------------------------------------------------------
    protected:
    void AddEdge (MeshFacet &rclFacet, unsigned long ulFacetIndex);
    // -------------------------------------------------------------------
    // DES: Ueberprueft, ob der Punkt keinem anderen Facet mehr zugeordnet ist
    //   und loescht in im diesem Falle. Die Punkt-Indizies der Facets werden
    //   entsprechend angepasst.
    // PAR: ulIndex:         Index im Punkt-Array
    //      ulFacetIndex:    Index des quasi geloeschten Facet, wird nicht betrachtet
    //      bOnlySetInvalid: Der Punkt wird nicht geloescht, sondern nur invalid       
    // gesetzt                       Default-Wert: FALSE
    // RET: void:
    // --------------------------------------------------------------------
    protected:
    inline unsigned long FindEdge (const MeshHelpEdge &rclEdge) const;
    // -------------------------------------------------------------------
    // DES: loescht bzw. umbelegt alle Kanten die aus das Facet indizieren.
    //   Wenn kein Nachbar-Facet existiert wird die Kante geloescht.
    //   Umbelegen heisst: Kanten die auf das Facet indizieren werden auf das
    //   Nachbar-Facet indiziert.
    // PAR: ulFacetIndex: zu loeschendes Facet
    // RET: void:
    // --------------------------------------------------------------------

    // -------------------------------------------------------------------
    // DES: loescht bzw. umbelegt alle Kanten die aus das Facet indizieren.
    //   Wenn kein Nachbar-Facet existiert wird die Kante geloescht.
    //   Umbelegen heisst: Kanten die auf das Facet indizieren werden auf das
    //   Nachbar-Facet indiziert.
    // PAR: ulFacetIndex: zu loeschendes Facet
    // RET: void:
    // --------------------------------------------------------------------
    protected:
    MeshEdgeArray::_TConstIterator FindEdge (unsigned long ulFacet, unsigned short usSide) const ;

    protected:
    void CheckAndCorrectEdge (unsigned long ulFacetIndex);
    // -------------------------------------------------------------------
    // DES: loescht alle ungueltige Punkte und Facets und korrigiert 
    //   gleichzeitig die Punkt- und Nachbar-Indizies
    // PAR: void:
    // RET: void:
    // --------------------------------------------------------------------
    protected:
    void RemoveInvalids (bool bWithEdgeCorrect = true, bool bWithEdgeDelete = false);
    // -------------------------------------------------------------------
    // DES: Ueberprueft, ob der Punkt keinem anderen Facet mehr zugeordnet ist
    //   und loescht in im diesem Falle. Die Punkt-Indizies der Facets werden
    //   entsprechend angepasst.
    // PAR: ulIndex:         Index im Punkt-Array
    //      ulFacetIndex:    Index des quasi geloeschten Facet, wird nicht betrachtet
    //      bOnlySetInvalid: Der Punkt wird nicht geloescht, sondern nur invalid       
    // gesetzt                       Default-Wert: FALSE
    // RET: void:
    // --------------------------------------------------------------------
    protected:
    void ErasePoint (unsigned long ulIndex, unsigned long ulFacetIndex,
                                bool bOnlySetInvalid = FALSE);

    // Neuaufbau des Kantenarrays.
    protected:
    void RebuildEdgeArray (void);

    // neugenerierung der Nachbarn-Indizes der Facets
    protected:
    void RebuildNeighbours (void);

  friend class MeshPointIterator;
  friend class MeshEdgeIterator;
  friend class MeshFacetIterator;
  friend class MeshFastFacetIterator;
  friend class MeshInventor;
  friend class MeshSTL;
};


/*-------------------------------------------------------------------
DES: Ueberprueft, ob der Punkt keinem anderen Facet mehr zugeordnet ist
  und loescht in im diesem Falle. Die Punkt-Indizies der Facets werden
  entsprechend angepasst.
PAR: ulIndex:         Index im Punkt-Array
     ulFacetIndex:    Index des quasi geloeschten Facet, wird nicht betrachtet
     bOnlySetInvalid: Der Punkt wird nicht geloescht, sondern nur invalid       
gesetzt                       Default-Wert: FALSE
RET: void:
--------------------------------------------------------------------*/
class BoundBox3D;
class MeshPoint;
class AppMeshExport MeshGeomEdge;
class MeshGeomFacet;
class Matrix4D;
class DataStream;
class MeshFacetIterator;
class AppMeshExport MeshEdgeIterator;
class AppMeshExport MeshPointIterator;
class Vector3D;
class MeshFacet;
class AppMeshExport MeshHelpEdge;
#ifdef OVERREAD_FOLLOW
#include "boundbox.hxx"
#include "elements.hxx"
#endif
inline unsigned long MeshKernel::FindEdge (const MeshHelpEdge &rclEdge) const
{
  unsigned long           i, ulCt  = _aclEdgeArray.size();
  MeshHelpEdge    clEdge;
  const MeshEdge *pclEdge;

  for (i = 0; i < ulCt; i++)
  {
    pclEdge = &_aclEdgeArray[i];
    _aclFacetArray[pclEdge->Index()].GetEdge((unsigned short)(pclEdge->Side()), clEdge);
    if (clEdge == rclEdge)
      return i;
  }

  return ULONG_MAX;
}

// gibt den Punkt an dem übergebenen Index zurück. Die Methode ist nicht sehr      
// effektiv und  sollte nur für sporadische Zugriffe von Aussen verwendet werden.
inline MeshPoint MeshKernel::GetPoint (unsigned long ulIndex) const
{
  assert(ulIndex < _aclPointArray.size());
  return _aclPointArray[ulIndex];
}

// gibt die Kante an dem übergebenen Index zurück. Die Methode ist nicht sehr      
// effektiv und  sollte nur für sporadische Zugriffe von Aussen verwendet werden.
inline MeshGeomEdge MeshKernel::GetEdge (unsigned long ulPos) const
{
  assert(ulPos < _aclEdgeArray.size());

  MeshHelpEdge   clH;
  MeshGeomEdge clEdge;
  unsigned short         usSide  = (unsigned short)(_aclEdgeArray[ulPos].Side());
  unsigned long          ulIndex = _aclEdgeArray[ulPos].Index();

  _aclFacetArray[ulIndex].GetEdge(usSide, clH);
  clEdge._aclPoints[0] = _aclPointArray[clH._ulIndex[0]];
  clEdge._aclPoints[1] = _aclPointArray[clH._ulIndex[1]];
  clEdge._bBorder = _aclFacetArray[ulIndex]._aulNeighbours[usSide] == ULONG_MAX;
  return clEdge;
}

// gibt das Facet an dem übergebenen Index zurück. Die Methode ist nicht sehr      
// effektiv und  sollte nur für sporadische Zugriffe von Aussen verwendet werden.
inline MeshGeomFacet MeshKernel::GetFacet (unsigned long ulIndex) const
{
  assert(ulIndex < _aclFacetArray.size());

  const MeshFacet *pclF = &_aclFacetArray[ulIndex];
  MeshGeomFacet  clFacet;

  clFacet._aclPoints[0] = _aclPointArray[pclF->_aulPoints[0]];
  clFacet._aclPoints[1] = _aclPointArray[pclF->_aulPoints[1]];
  clFacet._aclPoints[2] = _aclPointArray[pclF->_aulPoints[2]];
  clFacet._ulProp       = pclF->_ulProp;
  clFacet._ucFlag       = pclF->_ucFlag;
  clFacet.CalcNormal();
  return clFacet;
}

// gibt die Nachbarn zu einem Facet an
inline void MeshKernel::GetFacetNeighbours (unsigned long ulIndex, unsigned long &rulNIdx0, unsigned long &rulNIdx1, unsigned long &rulNIdx2)
{
  assert(ulIndex < _aclFacetArray.size());

  rulNIdx0 = _aclFacetArray[ulIndex]._aulNeighbours[0];
  rulNIdx1 = _aclFacetArray[ulIndex]._aulNeighbours[1];
  rulNIdx2 = _aclFacetArray[ulIndex]._aulNeighbours[2];
}


/*translate one point*/
inline void MeshKernel::MovePoint (unsigned long ulPtIndex, const Vector3D &rclTrans)
{
  _aclPointArray[ulPtIndex] += rclTrans;
}

// anpassen der Umlaufrichtung an die Normale
inline void MeshKernel::AdjustNormal (MeshFacet &rclFacet, const Vector3D &rclNormal)
{
  Vector3D clN = (_aclPointArray[rclFacet._aulPoints[1]] - _aclPointArray[rclFacet._aulPoints[0]]) %
                  (_aclPointArray[rclFacet._aulPoints[2]] - _aclPointArray[rclFacet._aulPoints[0]]);
  if ((clN * rclNormal) < 0.0f)
  {
    rclFacet.FlipNormal();
  }
}

// berechnen eine normierten Normale des übergebenen Facet
inline Vector3D MeshKernel::GetNormal (const MeshFacet &rclFacet) const
{
  Vector3D clN = (_aclPointArray[rclFacet._aulPoints[1]] - _aclPointArray[rclFacet._aulPoints[0]]) %
                  (_aclPointArray[rclFacet._aulPoints[2]] - _aclPointArray[rclFacet._aulPoints[0]]);
  clN.Normalize();
  return clN;
}

inline void MeshKernel::GetFacetPoints (unsigned long ulFaIndex, unsigned long &rclP0, unsigned long &rclP1, unsigned long &rclP2) const
{
  assert(ulFaIndex < _aclFacetArray.size());
  const MeshFacet rclFacet = _aclFacetArray[ulFaIndex];
  rclP0 = rclFacet._aulPoints[0];  
  rclP1 = rclFacet._aulPoints[1];  
  rclP2 = rclFacet._aulPoints[2];  
}

inline bool MeshKernel::IsBorder (const MeshEdgeArray::_TIterator pPEdge) const
{
  return _aclFacetArray[pPEdge->Index()]._aulNeighbours[pPEdge->Side()] == ULONG_MAX;
}

} // namespace Mesh

#endif
