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


#ifndef MESH_APPROXIMATION_H
#define MESH_APPROXIMATION_H

#ifndef _PreComp_
# include <Wm3Vector3.h>
# include <Wm3QuadricSurface.h>
# include <Wm3Eigen.h>
# include <list>
# include <set>
# include <vector>
#endif

#include <Base/Vector3D.h>
#include <Base/Matrix.h>
using Base::Vector3D;
using Base::Matrix4D;

namespace Mesh {


/**
 * Abstract base class for approximation of a geometry to a given set of points.
 */
class AppMeshExport MeshPointFit
{
public:
  /**
   * Construction
   */
  MeshPointFit() : _bIsFitted( false ) { }

  /**
   * Destroys the object and frees any allocated resources.
   */
  virtual ~MeshPointFit(){ Clear(); }

  /**
   * Add points for the fit algorithm.
   */
  void AddPoint( const std::vector<Vector3D> &rvPointVect );

  /**
   * Add points for the fit algorithm.
   */
  void AddPoint( const std::set<Vector3D> &rsPointSet );

  /**
   * Add points for the fit algorithm.
   */
  void AddPoint( const std::list<Vector3D> &rsPointList );

  /**
   * Add points for the fit algorithm.
   */
  void AddPoint( const Vector3D &rcVector );

  /**
   * Returns the center of gravity of the current added points.
   * @return Vector3D
   */
  Vector3D GetGravity() const;

  /**
   * Determines the number of the current added points.
   * @return Number of points
   */
  unsigned long CountPoints() const;

  /**
   * Deletes the inserted points and frees any allocated resources.
   */
  void Clear();

  /**
   * Returns the result of the last fit.
   * @return float Quality of the last fit.
   */
  float GetLastResult() const;

  /**
   * Pure virtual function to fit the geometry to the given points. This function 
   * must be implemented by every subclass.
   */
  virtual float Fit() = 0;

  /**
   * Returns true if Fit() has been called for the current set of points, false otherwise.
   */
  bool Done() const;

protected:
  /**
   * Converts point from Wm3::Vector3 to Vector3D.
   */
  static void Convert( const Wm3::Vector3<float>&, Vector3D&);
  /**
   * Converts point from Vector3D to Wm3::Vector3.
   */
  static void Convert( const Vector3D&, Wm3::Vector3<float>&);

  /**
   * Creates a vector of Wm3::Vector3 elements.
   */
  void GetMgcVectorArray( std::vector< Wm3::Vector3<float> >& rcPts ) const;

  std::list< Vector3D > _vPoints; /**< Holds the points for the fit algorithm.  */
  bool _bIsFitted; /**< Flag, whether the fit has been called. */
  float _fLastResult; /**< Stores the last result of the fit */
};

// -------------------------------------------------------------------------------

/**
 * Approximation of a plane into a given set of points.
 */
class AppMeshExport MeshPlaneFit : public MeshPointFit
{
public:
  /**
   * Construction
   */
  MeshPlaneFit(){};
  /**
   * Destruction
   */
  virtual ~MeshPlaneFit(){};

  /**
   * Returns the normal of the fitted plane. If Fit() has not been called the null vector is
   * returned.
   */
  Vector3D GetNormal() const;

  virtual float Fit();
  /** 
   * Returns the distance from the point \a rcPoint to the fitted plane. If Fit() has not been
   * called FLOAT_MAX is returned.
   */ 
  float GetDistanceToPlane( Vector3D const &rcPoint ) const;
  /**
   * Returns the standard deviation from the points to the fitted plane. If Fit() has not been
   * called FLOAT_MAX is returned.
   */
  float GetStdDeviation() const;
  /**
   * Returns the standard deviation from the points to the fitted plane with respect to the orientation
   * of the plane's normal. If Fit() has not been called FLOAT_MAX is returned.
   */
  float GetSignedStdDeviation() const;
  /**
   * Projects the points onto the fitted plane.
   */
  void ProjectToPlane ();

protected:
  Vector3D _vBase; /**< Base vector of the plane. */
  Vector3D _vNormal; /**< Normal of the plane. */
};

// -------------------------------------------------------------------------------

/**
 * Approximation of a quadratic surface into a given set of points. The implicit form of the surface
 * is defined by F(x,y,z) = a * x^2 + b * y^2 + c * z^2 + 
 *                       2d * x * y + 2e * x * z + 2f * y * z +
 *                          g * x + h * y + * i * z + k 
 *                        = 0
 * Depending on the parameters (a,..,k) this surface defines a sphere, ellipsoid, cylinder, cone
 * and so on.
 */
class AppMeshExport MeshQuadraticFit : public MeshPointFit
{
public:
  /**
   * Construction
   */
  MeshQuadraticFit() : MeshPointFit() {};
  
  /**
   * Destruction
   */
  virtual ~MeshQuadraticFit(){};
  
  /**
   * Übertragen der Quadric-Koeffizienten
   * @param iCoeff Nummer des Koeffizienten (0..9)
   * @return float Wert des Koeffizienten
   */
  float GetCoeff( unsigned long ulIndex ) const;
  
  /**
   * Übertragen der Koeffizientan als Referenz
   * auf das interne Array
   * @return const float& Referenz auf das float-Array
   */
  const float& GetCoeffArray() const;
  
  /**
   * Aufruf des Fit-Algorithmus
   * @retrun float Qualität des Fits.
   */
  float Fit();

  void CalcZValues( float x, float y, float &dZ1, float &dZ2 ) const;

  /**
   * Berechnen der Krümmungswerte der Quadric in einem bestimmten Punkt.
   * @param x X-Koordinate
   * @param y Y-Koordinate
   * @param z Z-Koordinate
   * @param rfCurv0 1. Hauptkrümmung
   * @param rfCurv1 2. Hauptkrümmung
   * @param rkDir0  Richtung der 1. Hauptkrümmung
   * @param rkDir1  Richtung der 2. Hauptkrümmung
   * @return bool Fehlerfreie Ausfürhung = true, ansonsten false
   */
  bool GetCurvatureInfo( float x, float y, float z,
                         float &rfCurv0, float &rfCurv1,
                         Vector3D &rkDir0, Vector3D &rkDir1, float &dDistance );

  bool GetCurvatureInfo( float x, float y, float z,
                                    float &rfCurv0, float &rfcurv1 );
  /**
   * Aufstellen der Formanmatrix A und Berechnen der Eigenwerte.
   * @param dLambda1 Eigenwert 1
   * @param dLambda2 Eigenwert 2
   * @param dLambda3 Eigenwert 3
   * @param clEV1    Eigenvektor 1
   * @param clEV2    Eigenvektor 2
   * @param clEV3    Eigenvektor 3
   */
  void CalcEigenValues( float &dLambda1, float &dLambda2, float &dLambda3,
                        Vector3D &clEV1, Vector3D &clEV2, Vector3D &clEV3 ) const;

protected:
  float _fCoeff[ 10 ];  /**< Ziel der Koeffizienten aus dem Fit */
};

// -------------------------------------------------------------------------------

/**
 * Dies ist ein 2,5D-Ansatz, bei dem zunächst die Ausgleichsebene der Punktmenge (P_i = (x,y,z), i=1,...,n) 
 * bestimmt wird. Danach wird eine Parametrisierung der Datenpunkte errechnet. Die Datenpunkte 
 * werden somit bzgl. des lokalen Systems der Ebene dargestellt (P_i = (u,v,w)). 
 * Durch diese transformierten Punkte wird nun eine quadratische Funktion
 *      w = f(u,v) = a*u^2 + b*v^2 + c*u*v + d*u + e*v + f
 * berechnet.
 * Dieser Ansatz wurde als Alternative für den 3D-Ansatz mit Quadriken entwickelt, da bei
 * Quadriken in (vor allem) ebenen Bereichen recht seltsame Artefakte auftreten.
 */
class AppMeshExport MeshSurfaceFit : public MeshPlaneFit
{
public:
  /**
   * Construction
   */
  MeshSurfaceFit();
  
  /**
   * Destruction
   */
  virtual ~MeshSurfaceFit(){};
 
  bool GetCurvatureInfo( float x, float y, float z, float &rfCurv0, float &rfCurv1,
                         Vector3D &rkDir0, Vector3D &rkDir1, float &dDistance );

  bool GetCurvatureInfo( float x, float y, float z, float &rfCurv0, float &rfcurv1 );

  float Fit();

protected:
  float SurfaceFit();
  float _fCoeff[ 10 ];  /**< Ziel der Koeffizienten aus dem Fit */
};

// -------------------------------------------------------------------------------

/**
 * Hilfs-Klasse für den Quadric-Fit. Beinhaltet die 
 * partiellen Ableitungen der Quadric und dient zur
 * Berechnung der Quadrik-Eigenschaften.
 */
class FunctionContainer
{
public:

  /**
   * Die MGC-Algorithmen arbeiten mit Funktionen dieses
   * Types
   */
  typedef float (*Function)(float,float,float);

  /**
   * Der parametrisierte Konstruktor. Erwartet ein Array
   * mit den Quadric-Koeffizienten.
   * @param pKoef Zeiger auf die Quadric-Parameter
   *        (float [10])
   */
  FunctionContainer( const float *pKoef )
  {
    Assign( pKoef );
/*
    Function oF;     
    Function aoDF[3];
    Function aoD2F[6];

    oF = &F;
    aoDF[0] = &Fx;
    aoDF[1] = &Fy;
    aoDF[2] = &Fz;
    aoD2F[0] = &Fxx;
    aoD2F[1] = &Fxy;
    aoD2F[2] = &Fxz;
    aoD2F[3] = &Fyy;
    aoD2F[4] = &Fyz;
    aoD2F[5] = &Fzz;

    pImplSurf = new Wm3::QuadricSurface<float>( oF, aoDF, aoD2F );*/

    pImplSurf = new Wm3::QuadricSurface<float>( dKoeff );
  }

  /**
   * Übernehmen der Quadric-Parameter
   * @param pKoef Zeiger auf die Quadric-Parameter
   *        (doube [10])
   */
  void Assign( const float *pKoef )
  {
    for( long ct=0; ct < 10; ct++ )
      dKoeff[ ct ] = pKoef[ ct ];
  }

  /**
   * Destruktor. Löscht die ImpicitSurface Klasse
   * der MGC-Bibliothek wieder
   */
  virtual ~FunctionContainer(){ delete pImplSurf; }

  /** 
   * Zugriff auf die Koeffizienten der Quadric
   * @param idx Index des Parameters
   * @return float& Der Koeffizient
   */
  float& operator[](int idx){ return dKoeff[ idx ]; }

  /**
   * Redirector auf eine Methode der MGC Bibliothek. Ermittelt
   * die Hauptkrümmungen und ihre Richtungen im angegebenen Punkt.
   * @param x X-Koordinate
   * @param y Y-Koordinate
   * @param z Z-Koordinate
   * @param rfCurv0 1. Hauptkrümmung
   * @param rfCurv1 2. Hauptkrümmung
   * @param rkDir0  Richtung der 1. Hauptkrümmung
   * @param rkDir1  Richtung der 2. Hauptkrümmung
   * @param dDistance Ergebnis das die Entfernung des Punktes von der Quadrik angibt.
   * @return bool Fehlerfreie Ausfürhung = true, ansonsten false
   */
  bool CurvatureInfo( float x, float y, float z, 
                      float &rfCurv0, float &rfCurv1,
                      Wm3::Vector3<float> &rkDir0,  Wm3::Vector3<float> &rkDir1, float &dDistance )
  {
    return pImplSurf->ComputePrincipalCurvatureInfo( Wm3::Vector3<float>(x, y, z),rfCurv0, rfCurv1, rkDir0, rkDir1 );
  }

  Vector3D GetGradient( float x, float y, float z ) const
  {
    Wm3::Vector3<float> grad = pImplSurf->GetGradient( Wm3::Vector3<float>(x, y, z) );
    return Vector3D( grad.X(), grad.Y(), grad.Z() );
  }

  Matrix4D GetHessian( float x, float y, float z ) const
  {
    Wm3::Matrix3<float> hess = pImplSurf->GetHessian( Wm3::Vector3<float>(x, y, z) );
    Matrix4D cMat; cMat.Unit();
    cMat[0][0] = hess[0][0]; cMat[0][1] = hess[0][1]; cMat[0][2] = hess[0][2];
    cMat[1][0] = hess[1][0]; cMat[1][1] = hess[1][1]; cMat[1][2] = hess[1][2];
    cMat[2][0] = hess[2][0]; cMat[2][1] = hess[2][1]; cMat[2][2] = hess[2][2];
    return cMat;
  }

  bool CurvatureInfo( float x, float y, float z,
                      float &rfCurv0, float &rfCurv1 )
  {
    float dQuot = Fz(x,y,z);
    float zx = - ( Fx(x,y,z) / dQuot );
    float zy = - ( Fy(x,y,z) / dQuot );
    
    float zxx = - ( 2.0 * ( dKoeff[5] + dKoeff[6] * zx * zx + dKoeff[8] * zx ) ) / dQuot;
    float zyy = - ( 2.0 * ( dKoeff[5] + dKoeff[6] * zy * zy + dKoeff[9] * zy ) ) / dQuot;
    float zxy = - ( dKoeff[6] * zx * zy + dKoeff[7] + dKoeff[8] * zy + dKoeff[9] * zx ) / dQuot;

    float dNen = 1 + zx*zx + zy*zy;
    float dNenSqrt = sqrt( dNen );
    float K = ( zxx * zyy - zxy * zxy ) / ( dNen * dNen );
    float H = 0.5 * ( ( 1.0+zx*zx - 2*zx*zy*zxy + (1.0+zy*zy)*zxx ) / ( dNenSqrt * dNenSqrt * dNenSqrt ) ) ;

    float dDiscr = sqrt(fabs(H*H-K));
    rfCurv0 = H - dDiscr;
    rfCurv1 = H + dDiscr;

    return true;
  }

  //+++++++++ Quadric +++++++++++++++++++++++++++++++++++++++
  static float F  ( float x, float y, float z ) 
  {
    return ( dKoeff[0] + dKoeff[1]*x + dKoeff[2]*y + dKoeff[3]*z +
             dKoeff[4]*x*x + dKoeff[5]*y*y + dKoeff[6]*z*z +
             dKoeff[7]*x*y + dKoeff[8]*x*z + dKoeff[9]*y*z );
  }
  
  //+++++++++ 1. derivations ++++++++++++++++++++++++++++++++
  static float Fx ( float x, float y, float z )
  {
    return( dKoeff[1] + 2.0*dKoeff[4]*x + dKoeff[7]*y + dKoeff[8]*z );
  }
  static float Fy ( float x, float y, float z ) 
  {
    return( dKoeff[2] + 2.0*dKoeff[5]*y + dKoeff[7]*x + dKoeff[9]*z );
  }
  static float Fz ( float x, float y, float z ) 
  {
    return( dKoeff[3] + 2.0*dKoeff[6]*z + dKoeff[8]*x + dKoeff[9]*y );
  }

  //+++++++++ 2. derivations ++++++++++++++++++++++++++++++++
  static float Fxx( float x, float y, float z ) 
  {
    return( 2.0*dKoeff[4] );
  }
  static float Fxy( float x, float y, float z ) 
  {
    return( dKoeff[7] );
  }
  static float Fxz( float x, float y, float z ) 
  {
    return( dKoeff[8] );
  }
  static float Fyy( float x, float y, float z ) 
  {
    return( 2.0*dKoeff[5] );
  }
  static float Fyz( float x, float y, float z ) 
  {
    return( dKoeff[9] );
  }
  static float Fzz( float x, float y, float z ) 
  {
    return( 2.0*dKoeff[6] );
  }
   
protected:
  static float dKoeff[ 10 ];     /**< Koeffizienten der Quadric */
  Wm3::ImplicitSurface<float> *pImplSurf;  /**< Zugriff auf die MGC-Bibliothek */

private:
  /**
   * Private construction.
   */
  FunctionContainer(){};
};

} // namespace Mesh

#endif // MESH_APPROXIMATION_H
