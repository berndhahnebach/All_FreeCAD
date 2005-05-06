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


#include "PreCompiled.h"
#ifndef _PreComp_
#endif


#include "Delaunay.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>


//using namespace Mesh;



const Vector3 Vector3::ZERO(0,0,0);
const Vector3 Vector3::UNIT_X(1,0,0);
const Vector3 Vector3::UNIT_Y(0,1,0);
const Vector3 Vector3::UNIT_Z(0,0,1);

#ifdef FC_OS_WIN32
const Real INFINITY = DBL_MAX;
#endif


//----------------------------------------------------------------------------
Vector3::Vector3 ()
{
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}
//----------------------------------------------------------------------------
Vector3::Vector3 (Real fX, Real fY, Real fZ)
{
    x = fX;
    y = fY;
    z = fZ;
}
//----------------------------------------------------------------------------
Vector3::Vector3 (Real afCoordinate[3])
{
    x = afCoordinate[0];
    y = afCoordinate[1];
    z = afCoordinate[2];
}
//----------------------------------------------------------------------------
Vector3::Vector3 (const Vector3& rkVector)
{
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
}
//----------------------------------------------------------------------------
Real& Vector3::operator[] (int i) const
{
    // assert:  0 <= i < 2; x, y, and z are packed into 3*sizeof(Real)
    //          bytes
    return (Real&) *(&x + i);
}
//----------------------------------------------------------------------------
Vector3::operator Real* ()
{
    return &x;
}
//----------------------------------------------------------------------------
Vector3& Vector3::operator= (const Vector3& rkVector)
{
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    return *this;
}
//----------------------------------------------------------------------------
bool Vector3::operator== (const Vector3& rkVector) const
{
    return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
}
//----------------------------------------------------------------------------
bool Vector3::operator!= (const Vector3& rkVector) const
{
    return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
}
//----------------------------------------------------------------------------
Vector3 Vector3::operator+ (const Vector3& rkVector) const
{
    Vector3 kSum;
    kSum.x = x + rkVector.x;
    kSum.y = y + rkVector.y;
    kSum.z = z + rkVector.z;
    return kSum;
}
//----------------------------------------------------------------------------
Vector3 Vector3::operator- (const Vector3& rkVector) const
{
    Vector3 kDiff;
    kDiff.x = x - rkVector.x;
    kDiff.y = y - rkVector.y;
    kDiff.z = z - rkVector.z;
    return kDiff;
}
//----------------------------------------------------------------------------
Vector3 Vector3::operator* (Real fScalar) const
{
    Vector3 kProd;
    kProd.x = fScalar*x;
    kProd.y = fScalar*y;
    kProd.z = fScalar*z;
    return kProd;
}
//----------------------------------------------------------------------------
Vector3 Vector3::operator- () const
{
    Vector3 kNeg;
    kNeg.x = -x;
    kNeg.y = -y;
    kNeg.z = -z;
    return kNeg;
}
//----------------------------------------------------------------------------
Vector3 operator* (Real fScalar, const Vector3& rkVector)
{
    Vector3 kProd;
    kProd.x = fScalar*rkVector.x;
    kProd.y = fScalar*rkVector.y;
    kProd.z = fScalar*rkVector.z;
    return kProd;
}
//----------------------------------------------------------------------------
Vector3& Vector3::operator+= (const Vector3& rkVector)
{
    x += rkVector.x;
    y += rkVector.y;
    z += rkVector.z;
    return *this;
}
//----------------------------------------------------------------------------
Vector3& Vector3::operator-= (const Vector3& rkVector)
{
    x -= rkVector.x;
    y -= rkVector.y;
    z -= rkVector.z;
    return *this;
}
//----------------------------------------------------------------------------
Vector3& Vector3::operator*= (Real fScalar)
{
    x *= fScalar;
    y *= fScalar;
    z *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
Real Vector3::SquaredLength () const
{
    return x*x + y*y + z*z;
}
//----------------------------------------------------------------------------
Real Vector3::Dot (const Vector3& rkVector) const
{
    return x*rkVector.x + y*rkVector.y + z*rkVector.z;
}
//----------------------------------------------------------------------------
Vector3 Vector3::operator/ (Real fScalar) const
{
    Vector3 kQuot;

    if ( fScalar != 0.0 )
    {
        Real fInvScalar = 1.0/fScalar;
        kQuot.x = fInvScalar*x;
        kQuot.y = fInvScalar*y;
        kQuot.z = fInvScalar*z;
        return kQuot;
    }
    else
    {
        return Vector3(INFINITY,INFINITY,
            INFINITY);
    }
}
//----------------------------------------------------------------------------
Vector3& Vector3::operator/= (Real fScalar)
{
    if ( fScalar != 0.0 )
    {
        Real fInvScalar = 1.0/fScalar;
        x *= fInvScalar;
        y *= fInvScalar;
        z *= fInvScalar;
    }
    else
    {
        x = INFINITY;
        y = INFINITY;
        z = INFINITY;
    }

    return *this;
}
//----------------------------------------------------------------------------
Real Vector3::Length () const
{
    return sqrt(x*x + y*y + z*z);
}
//----------------------------------------------------------------------------
Real Vector3::Unitize (Real fTolerance)
{
    Real fLength = Length();

    if ( fLength > fTolerance )
    {
        Real fInvLength = 1.0/fLength;
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    }
    else
    {
        fLength = 0.0;
    }

    return fLength;
}
//----------------------------------------------------------------------------
Vector3 Vector3::Cross (const Vector3& rkVector) const
{
    Vector3 kCross;

    kCross.x = y*rkVector.z - z*rkVector.y;
    kCross.y = z*rkVector.x - x*rkVector.z;
    kCross.z = x*rkVector.y - y*rkVector.x;

    return kCross;
}
//----------------------------------------------------------------------------
Vector3 Vector3::UnitCross (const Vector3& rkVector) const
{
    Vector3 kCross;

    kCross.x = y*rkVector.z - z*rkVector.y;
    kCross.y = z*rkVector.x - x*rkVector.z;
    kCross.z = x*rkVector.y - y*rkVector.x;
    kCross.Unitize();

    return kCross;
}
//----------------------------------------------------------------------------
void Vector3::Orthonormalize (Vector3 akVector[3])
{
    // If the input vectors are v0, v1, and v2, then the Gram-Schmidt
    // orthonormalization produces vectors u0, u1, and u2 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // compute u0
    akVector[0].Unitize();

    // compute u1
    Real fDot0 = akVector[0].Dot(akVector[1]); 
    akVector[1] -= fDot0*akVector[0];
    akVector[1].Unitize();

    // compute u2
    Real fDot1 = akVector[1].Dot(akVector[2]);
    fDot0 = akVector[0].Dot(akVector[2]);
    akVector[2] -= fDot0*akVector[0] + fDot1*akVector[1];
    akVector[2].Unitize();
}
//----------------------------------------------------------------------------
void Vector3::GenerateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
    Vector3& rkW, bool bUnitLengthW)
{
    if ( !bUnitLengthW )
        rkW.Unitize();

    if ( fabs(rkW.x) >= fabs(rkW.y)
    &&   fabs(rkW.x) >= fabs(rkW.z) )
    {
        rkU.x = -rkW.y;
        rkU.y = +rkW.x;
        rkU.z = 0.0;
    }
    else
    {
        rkU.x = 0.0;
        rkU.y = +rkW.z;
        rkU.z = -rkW.y;
    }

    rkU.Unitize();
    rkV = rkW.Cross(rkU);
}
//----------------------------------------------------------------------------




Real Delaunay3D::ms_fEpsilon = 0.00001;
Real Delaunay3D::ms_fRange = 10.0;
int Delaunay3D::ms_iTSize = 75;
int Delaunay3D::ms_iQuantityFactor = 16;


Real SymmetricRandom ()
{
    return 2.0*Real(rand())/Real(RAND_MAX) - 1.0;
}


//---------------------------------------------------------------------------
Delaunay3D::Delaunay3D (int iVertexQuantity, Vector3* akVertex)
{
    assert( iVertexQuantity >= 4 && akVertex );
    m_akTetrahedron = 0;

    m_bOwner = true;

    m_iVertexQuantity = iVertexQuantity;
    m_akVertex = akVertex;

    // Make a copy of the input vertices.  These will be modified.  The
    // extra four slots are required for temporary storage.
    Vector3* akPoint = new Vector3[m_iVertexQuantity+4];
    memcpy(akPoint,akVertex,m_iVertexQuantity*sizeof(Vector3));

    // compute the axis-aligned bounding box of the vertices
    m_fXMin = akPoint[0].x;
    m_fXMax = m_fXMin;
    m_fYMin = akPoint[0].y;
    m_fYMax = m_fYMin;
    m_fZMin = akPoint[0].z;
    m_fZMax = m_fZMin;

    int i;
    for (i = 1; i < m_iVertexQuantity; i++)
    {
        Real fValue = akPoint[i].x;
        if ( m_fXMax < fValue )
            m_fXMax = fValue;
        if ( m_fXMin > fValue )
            m_fXMin = fValue;

        fValue = akPoint[i].y;
        if ( m_fYMax < fValue )
            m_fYMax = fValue;
        if ( m_fYMin > fValue )
            m_fYMin = fValue;

        fValue = akPoint[i].z;
        if ( m_fZMax < fValue )
            m_fZMax = fValue;
        if ( m_fZMin > fValue )
            m_fZMin = fValue;
    }

    m_fXRange = m_fXMax-m_fXMin;
    m_fYRange = m_fYMax-m_fYMin;
    m_fZRange = m_fZMax-m_fZMin;

    // need to scale the data later to do a correct tetrahedron count
    Real fMaxRange = m_fXRange;
    if ( fMaxRange < m_fYRange )
        fMaxRange = m_fYRange;
    if ( fMaxRange < m_fZRange )
        fMaxRange = m_fZRange;
    Real fMaxRangeCube = fMaxRange*fMaxRange*fMaxRange;

    // Tweak the points by very small random numbers to help avoid
    // cosphericities in the vertices.
    Real fAmplitude = 0.5*ms_fEpsilon*fMaxRange;
    for (i = 0; i < m_iVertexQuantity; i++) 
    {
        akPoint[i].x += fAmplitude*SymmetricRandom();
        akPoint[i].y += fAmplitude*SymmetricRandom();
        akPoint[i].z += fAmplitude*SymmetricRandom();
    }

    Real aafWork[4][3] =
    {
        { 8.0*ms_fRange, -ms_fRange, -ms_fRange },
        { -ms_fRange, 8.0*ms_fRange, -ms_fRange },
        { -ms_fRange, -ms_fRange, 8.0*ms_fRange },
        { -ms_fRange, -ms_fRange, -ms_fRange }
    };

    for (i = 0; i < 4; i++)
    {
        akPoint[m_iVertexQuantity+i].x = m_fXMin+m_fXRange*aafWork[i][0];
        akPoint[m_iVertexQuantity+i].y = m_fYMin+m_fYRange*aafWork[i][1];
        akPoint[m_iVertexQuantity+i].z = m_fZMin+m_fZRange*aafWork[i][2];
    }

    int i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i11, aiII[3];
    Real fTmp;

    int iSixTSize = 6*ms_iTSize;
    int** aaiTmp = new int*[iSixTSize+1];
    aaiTmp[0] = new int[3*(iSixTSize+1)];
    for (i0 = 1; i0 < iSixTSize+1; i0++)
        aaiTmp[i0] = aaiTmp[0] + 3*i0;

    // Estimate of how many tetrahedrons there can be.  Since theoretically
    // the number is O(N^2), this could be quite large.  You may need to
    // increase the quantity factor if a call to this function fails.
    i1 = ms_iQuantityFactor*m_iVertexQuantity;

    int* aaID = new int[i1];
    for (i0 = 0; i0 < i1; i0++)
        aaID[i0] = i0;

    int** aaiA3S = new int*[i1];
    aaiA3S[0] = new int[4*i1];
    for (i0 = 1; i0 < i1; i0++)
        aaiA3S[i0] = aaiA3S[0] + 4*i0;
    aaiA3S[0][0] = m_iVertexQuantity;
    aaiA3S[0][1] = m_iVertexQuantity+1;
    aaiA3S[0][2] = m_iVertexQuantity+2;
    aaiA3S[0][3] = m_iVertexQuantity+3;

    // circumscribed centers and radii
    Real** aafCCR = new Real*[i1];
    aafCCR[0] = new Real[4*i1];
    for (i0 = 1; i0 < i1; i0++)
        aafCCR[i0] = aafCCR[0] + 4*i0;
    aafCCR[0][0] = 0.0;
    aafCCR[0][1] = 0.0;
    aafCCR[0][2] = 0.0;
    aafCCR[0][3] = INFINITY;

    int iTetraQuantity = 1;  // number of tetrahedra
    i4 = 1;

    // compute tetrahedralization
    for (i0 = 0; i0 < m_iVertexQuantity; i0++)
    {  
        i1 = i7 = -1;
        i9 = 0;
        for (i11 = 0; i11 < iTetraQuantity; i11++)
        {  
            i1++;
            while ( aaiA3S[i1][0] < 0 ) 
                i1++;
            fTmp = aafCCR[i1][3];
            for (i2 = 0; i2 < 3; i2++)
            {
                Real fZ = akPoint[i0][i2]-aafCCR[i1][i2];
                fTmp -= fZ*fZ;
                if ( fTmp < 0.0 ) 
                    goto Corner3;
            }
            i9--;
            i4--;
            aaID[i4] = i1;
            for (i2 = 0; i2 < 4; i2++)
            {
                aiII[0] = 0;
                if ( aiII[0] == i2 ) 
                    aiII[0]++;
                for (i3 = 1; i3 < 3; i3++)
                {
                    aiII[i3] = aiII[i3-1] + 1;
                    if ( aiII[i3] == i2 ) 
                        aiII[i3]++;
                }
                if ( i7 > 2 )
                {
                    i8 = i7;
                    for (i3 = 0; i3 <= i8; i3++)
                    {
                        for (i5 = 0; i5 < 3; i5++)
                        {
                            if ( aaiA3S[i1][aiII[i5]] != aaiTmp[i3][i5] ) 
                                goto Corner1;
                        }
                        for (i6 = 0; i6 < 3; i6++)
                            aaiTmp[i3][i6] = aaiTmp[i8][i6];
                        i7--;
                        goto Corner2;
Corner1:;
                    }
                }
                if ( ++i7 > iSixTSize )
                {
                    // Temporary storage exceeded.  Increase ms_iTSize and
                    // call the constructor again.
                    assert( false );
                    goto ExitDelaunay;
                }
                for (i3 = 0; i3 < 3; i3++) 
                    aaiTmp[i7][i3] = aaiA3S[i1][aiII[i3]];
Corner2:;
            }
            aaiA3S[i1][0] = -1;
Corner3:;
        }

        for (i1 = 0; i1 <= i7; i1++)
        {
            for (i2 = 0; i2 < 3; i2++)
                for (aafWork[3][i2] = 0, i3 = 0; i3 < 3; i3++)
                {
                    aafWork[i3][i2] = akPoint[aaiTmp[i1][i2]][i3] -
                        akPoint[i0][i3];
                    aafWork[3][i2] += 0.5*aafWork[i3][i2]*(
                        akPoint[aaiTmp[i1][i2]][i3] + akPoint[i0][i3]);
                }

            fTmp = (aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                aafWork[1][1]));
            assert( fTmp != 0.0 );
            fTmp = 1.0/fTmp;
            aafCCR[aaID[i4]][0] = ((aafWork[3][0]*(aafWork[1][1]*
                aafWork[2][2] - aafWork[1][2]*aafWork[2][1])) -
                (aafWork[1][0]*(aafWork[3][1]*aafWork[2][2] - aafWork[3][2]*
                aafWork[2][1])) + (aafWork[2][0]*(aafWork[3][1]*
                aafWork[1][2] - aafWork[3][2]*aafWork[1][1])))*fTmp;
            aafCCR[aaID[i4]][1] = ((aafWork[0][0]*(aafWork[3][1]*
                aafWork[2][2] - aafWork[3][2]*aafWork[2][1])) -
                (aafWork[3][0]*(aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*
                aafWork[2][1])) + (aafWork[2][0]*(aafWork[0][1]*
                aafWork[3][2] - aafWork[0][2]*aafWork[3][1])))*fTmp;
            aafCCR[aaID[i4]][2] = ((aafWork[0][0]*(aafWork[1][1]*
                aafWork[3][2] - aafWork[1][2]*aafWork[3][1])) -
                (aafWork[1][0]*(aafWork[0][1]*aafWork[3][2] - aafWork[0][2]*
                aafWork[3][1])) + (aafWork[3][0]*(aafWork[0][1]*
                aafWork[1][2] - aafWork[0][2]*aafWork[1][1])))*fTmp;

            for (aafCCR[aaID[i4]][3] = 0, i2 = 0; i2 < 3; i2++) 
            {
                Real fZ = akPoint[i0][i2] - aafCCR[aaID[i4]][i2];
                aafCCR[aaID[i4]][3] += fZ*fZ;
                aaiA3S[aaID[i4]][i2] = aaiTmp[i1][i2];
            }

            aaiA3S[aaID[i4]][3] = i0;
            i4++;
            i9++;
        }
        iTetraQuantity += i9;
    }

    // count the number of tetrahedra
    m_iTetrahedronQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTetraQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;
        if ( aaiA3S[i0][0] < m_iVertexQuantity )
        {
            for (i1 = 0; i1 < 3; i1++)
            {
                for (i2 = 0; i2 < 3; i2++) 
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][3]][i2];
                }
            }

            fTmp = ((aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                aafWork[1][1])));

            if ( fabs(fTmp) > ms_fEpsilon*fMaxRangeCube )
                m_iTetrahedronQuantity++;
        }
    }

    // create the tetrahedra
    m_akTetrahedron = new Tetrahedron[m_iTetrahedronQuantity];
    m_iTetrahedronQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTetraQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;
        if ( aaiA3S[i0][0] < m_iVertexQuantity )
        {
            for (i1 = 0; i1 < 3; i1++)
            {
                for (i2 = 0; i2 < 3; i2++) 
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][3]][i2];
                }
            }

            fTmp = ((aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2]-aafWork[0][2]*
                aafWork[1][1])));

            if ( fabs(fTmp) > ms_fEpsilon*fMaxRangeCube )
            {
                int iDelta = fTmp < 0.0 ? 1 : 0;
                Tetrahedron& rkTetra =
                    m_akTetrahedron[m_iTetrahedronQuantity];
                rkTetra.m_aiVertex[0] = aaiA3S[i0][0];
                rkTetra.m_aiVertex[1] = aaiA3S[i0][1];
                rkTetra.m_aiVertex[2] = aaiA3S[i0][2+iDelta];
                rkTetra.m_aiVertex[3] = aaiA3S[i0][3-iDelta];
                m_iTetrahedronQuantity++;
            }
        }
    }

ExitDelaunay:
    delete[] aaiTmp[0];
    delete[] aaiTmp;
    delete[] aaID;
    delete[] aaiA3S[0];
    delete[] aaiA3S;
    delete[] aafCCR[0];
    delete[] aafCCR;
    delete[] akPoint;
}
//---------------------------------------------------------------------------
Delaunay3D::Delaunay3D (Delaunay3D& rkNet)
{
    m_bOwner = false;

    m_iVertexQuantity = rkNet.m_iVertexQuantity;
    m_akVertex = rkNet.m_akVertex;
    m_fXMin = rkNet.m_fXMin;
    m_fXMax = rkNet.m_fXMax;
    m_fXRange = rkNet.m_fXRange;
    m_fYMin = rkNet.m_fYMin;
    m_fYMax = rkNet.m_fYMax;
    m_fYRange = rkNet.m_fYRange;
    m_fZMin = rkNet.m_fZMin;
    m_fZMax = rkNet.m_fZMax;
    m_fZRange = rkNet.m_fZRange;
    m_iTetrahedronQuantity = rkNet.m_iTetrahedronQuantity;
    m_akTetrahedron = rkNet.m_akTetrahedron;
}
//---------------------------------------------------------------------------
Delaunay3D::~Delaunay3D ()
{
    if ( m_bOwner )
    {
        delete[] m_akVertex;
        delete[] m_akTetrahedron;
    }
}
//----------------------------------------------------------------------------
