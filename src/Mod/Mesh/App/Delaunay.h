/***************************************************************************
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

#ifndef DELAUNY_H
#define DELAUNY_H


typedef double Real;

#include <assert.h>

class Vector3 
{
public:
    // construction
    Vector3 ();
    Vector3 (Real fX, Real fY, Real fZ);
    Vector3 (Real afCoordinate[3]);
    Vector3 (const Vector3& rkVector);

    // member access (allows V.x or V[0], V.y or V[1], V.z or V[2])
    Real x, y, z;
    Real& operator[] (int i) const;
    operator Real* ();

    // assignment and comparison
    Vector3& operator= (const Vector3& rkVector);
    bool operator== (const Vector3& rkVector) const;
    bool operator!= (const Vector3& rkVector) const;

    // arithmetic operations
    Vector3 operator+ (const Vector3& rkVector) const;
    Vector3 operator- (const Vector3& rkVector) const;
    Vector3 operator* (Real fScalar) const;
    Vector3 operator/ (Real fScalar) const;
    Vector3 operator- () const;
    friend Vector3 operator* (Real fScalar, const Vector3& rkVector);

    // arithmetic updates
    Vector3& operator+= (const Vector3& rkVector);
    Vector3& operator-= (const Vector3& rkVector);
    Vector3& operator*= (Real fScalar);
    Vector3& operator/= (Real fScalar);

    // vector operations
    Real Length () const;
    Real SquaredLength () const;
    Real Dot (const Vector3& rkVector) const;
    Real Unitize (Real fTolerance = 1e-06);
    Vector3 Cross (const Vector3& rkVector) const;
    Vector3 UnitCross (const Vector3& rkVector) const;

    // Gram-Schmidt orthonormalization.
    static void Orthonormalize (Vector3 akVector[3]);

    // Input W must be initialize to a nonzero vector, output is {U,V,W}
    // an orthonormal basis.  A hint is provided about whether or not W
    // is already unit length.
    static void GenerateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
        Vector3& rkW, bool bUnitLengthW = true);

    // special points
    static const Vector3 ZERO;
    static const Vector3 UNIT_X;
    static const Vector3 UNIT_Y;
    static const Vector3 UNIT_Z;
};


class Delaunay3D
{
public:
    // Construction and destruction.  In the first constructor,
    // Delaunay3D accepts ownership of the input array and will
    // delete it during destruction.  The second constructor is designed to
    // allow sharing of the network (note that the reference argument is not
    // passed as 'const').  Any other network that shares this object will
    // not delete the data in this object.
    Delaunay3D (int iVertexQuantity, Vector3* akVertex);
    Delaunay3D (Delaunay3D& rkNet);
    virtual ~Delaunay3D ();
    bool IsOwner () const;

    // vertices
    int GetVertexQuantity () const;
    const Vector3& GetVertex (int i) const;
    const Vector3* GetVertices () const;
    Real GetXMin () const;
    Real GetXMax () const;
    Real GetXRange () const;
    Real GetYMin () const;
    Real GetYMax () const;
    Real GetYRange () const;
    Real GetZMin () const;
    Real GetZMax () const;
    Real GetZRange () const;

    // tetrahedra
    class Tetrahedron
    {
    public:
        int m_aiVertex[4];
    };

    int GetTetrahedronQuantity () const;
    Tetrahedron& GetTetrahedron (int i);
    const Tetrahedron& GetTetrahedron (int i) const;
    Tetrahedron* GetTetrahedrons ();
    const Tetrahedron* GetTetrahedrons () const;

    // tweaking parameters
    static Real& Epsilon ();     // default = 0.00001
    static Real& Range ();       // default = 10.0
    static int& TSize ();           // default = 75
    static int& QuantityFactor ();  // default = 16

protected:
    // for sharing
    bool m_bOwner;

    // vertices
    int m_iVertexQuantity;
    Vector3* m_akVertex;
    Real m_fXMin, m_fXMax, m_fXRange;
    Real m_fYMin, m_fYMax, m_fYRange;
    Real m_fZMin, m_fZMax, m_fZRange;

    // tetrahedra
    int m_iTetrahedronQuantity;
    Tetrahedron* m_akTetrahedron;

    // tweaking parameters
    static Real ms_fEpsilon;
    static Real ms_fRange;
    static int ms_iTSize;
    static int ms_iQuantityFactor;
};

//----------------------------------------------------------------------------
inline bool Delaunay3D::IsOwner () const
{
    return m_bOwner;
}
//----------------------------------------------------------------------------
inline int Delaunay3D::GetVertexQuantity () const
{
    return m_iVertexQuantity;
}
//----------------------------------------------------------------------------
inline const Vector3& Delaunay3D::GetVertex (int i) const
{
    assert( 0 <= i && i < m_iVertexQuantity );
    return m_akVertex[i];
}
//----------------------------------------------------------------------------
inline const Vector3* Delaunay3D::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetXRange () const
{
    return m_fXRange;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetYMin () const
{
    return m_fYMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetYMax () const
{
    return m_fYMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetYRange () const
{
    return m_fYRange;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetZMin () const
{
    return m_fZMin;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetZMax () const
{
    return m_fZMax;
}
//----------------------------------------------------------------------------
inline Real Delaunay3D::GetZRange () const
{
    return m_fZRange;
}
//----------------------------------------------------------------------------
inline int Delaunay3D::GetTetrahedronQuantity () const
{
    return m_iTetrahedronQuantity;
}
//----------------------------------------------------------------------------
inline Delaunay3D::Tetrahedron&
Delaunay3D::GetTetrahedron (int i)
{
    assert( 0 <= i && i < m_iTetrahedronQuantity );
    return m_akTetrahedron[i];
}
//----------------------------------------------------------------------------
inline const Delaunay3D::Tetrahedron&
Delaunay3D::GetTetrahedron (int i) const
{
    assert( 0 <= i && i < m_iTetrahedronQuantity );
    return m_akTetrahedron[i];
}
//----------------------------------------------------------------------------
inline Delaunay3D::Tetrahedron*
Delaunay3D::GetTetrahedrons ()
{
    return m_akTetrahedron;
}
//----------------------------------------------------------------------------
inline const Delaunay3D::Tetrahedron*
Delaunay3D::GetTetrahedrons () const
{
    return m_akTetrahedron;
}
//----------------------------------------------------------------------------
inline Real& Delaunay3D::Epsilon ()
{
    return ms_fEpsilon;
}
//----------------------------------------------------------------------------
inline Real& Delaunay3D::Range ()
{
    return ms_fRange;
}
//----------------------------------------------------------------------------
inline int& Delaunay3D::TSize ()
{
    return ms_iTSize;
}
//----------------------------------------------------------------------------
inline int& Delaunay3D::QuantityFactor ()
{
    return ms_iQuantityFactor;
}
//----------------------------------------------------------------------------


#endif
