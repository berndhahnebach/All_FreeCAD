/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#include "PreCompiled.h"
#include <math_Gauss.hxx>
#include <math_Householder.hxx>
#include <Geom_BSplineSurface.hxx>

#include <Mod/Mesh/App/Core/Approximation.h>
#include <Base/Sequencer.h>
#include <Base/Tools2D.h>

#include "ApproxSurface.h"

using namespace Reen;

// SplineBasisfunction

SplineBasisfunction::SplineBasisfunction(int iSize)
: _vKnotVector(0,iSize-1)
{
}

SplineBasisfunction::SplineBasisfunction
                        (TColStd_Array1OfReal& vKnots, 
                         TColStd_Array1OfInteger& vMults, 
                         int iSize,
                         int iOrder)
: _vKnotVector(0,iSize-1)
{
  int sum = 0;
  for (int h=vMults.Lower(); h<=vMults.Upper(); h++)
    sum += vMults(h);

  if (vKnots.Length() != vMults.Length() || iSize != sum)
  {
    // Werfe Exception
    Standard_ConstructionError::Raise("BSplineBasis");
  }

  int k=0;
  for (int i=vMults.Lower(); i<=vMults.Upper(); i++)
  {
    for (int j=0; j<vMults(i); j++)
    {
      _vKnotVector(k) = vKnots(i);
      k++;
    }
  }

  _iOrder = iOrder;
}

SplineBasisfunction::SplineBasisfunction(TColStd_Array1OfReal& vKnots, int iOrder)
: _vKnotVector(0,vKnots.Length()-1)
{
  _vKnotVector = vKnots;
  _iOrder = iOrder;
}

SplineBasisfunction::~SplineBasisfunction()
{
}

void SplineBasisfunction::SetKnots(TColStd_Array1OfReal& vKnots, int iOrder)
{
  if (_vKnotVector.Length() != vKnots.Length())
        Standard_RangeError::Raise("BSplineBasis");

  _vKnotVector = vKnots;
  _iOrder = iOrder;
}

void SplineBasisfunction::SetKnots(TColStd_Array1OfReal& vKnots, TColStd_Array1OfInteger& vMults, 
                              int iOrder)
{
  int sum = 0;
  for (int h=vMults.Lower(); h<=vMults.Upper(); h++)
    sum += vMults(h);

  if (vKnots.Length() != vMults.Length() || _vKnotVector.Length() != sum)
  {
    // Werfe Exception
    Standard_RangeError::Raise("BSplineBasis");
  }
  int k=0;
  for (int i=vMults.Lower(); i<=vMults.Upper(); i++)
  {
    for (int j=0; j<vMults(i); j++)
    {
      _vKnotVector(k) = vKnots(i);
      k++;
    }
  }

  _iOrder = iOrder;
}

////////////////////////////////////////// BSplineBasis

BSplineBasis::BSplineBasis(int iSize)
: SplineBasisfunction(iSize)
{
}

BSplineBasis::BSplineBasis(TColStd_Array1OfReal& vKnots, TColStd_Array1OfInteger& vMults, int iSize, 
                             int iOrder)
: SplineBasisfunction(vKnots, vMults, iSize, iOrder)
{
}

BSplineBasis::BSplineBasis(TColStd_Array1OfReal& vKnots, int iOrder)
: SplineBasisfunction(vKnots, iOrder)
{
}

BSplineBasis::~BSplineBasis()
{
}

int BSplineBasis::FindSpan(double fParam)
{
  int n = _vKnotVector.Length()-_iOrder-1;
  if (fParam == _vKnotVector(n+1))
    return n;

  int low = _iOrder-1;
  int high = n+1;
  int mid = (low+high)/2; //Bin�rsuche

  while (fParam < _vKnotVector(mid) || fParam>= _vKnotVector(mid+1))
  {
    if (fParam < _vKnotVector(mid))
      high = mid;
    else
      low = mid;
    mid = (low+high)/2;
  }

  return mid;
}

void BSplineBasis::AllBasisFunctions(double fParam, TColStd_Array1OfReal& vFuncVals)
{
  if (vFuncVals.Length() != _iOrder)
    Standard_RangeError::Raise("BSplineBasis");

  int iIndex = FindSpan(fParam);

  TColStd_Array1OfReal zaehler_left(1,_iOrder-1);
  TColStd_Array1OfReal zaehler_right(1,_iOrder-1);
  vFuncVals(0) = 1.0f;

  for (int j=1; j<_iOrder; j++)
  {
    zaehler_left(j)  = fParam - _vKnotVector(iIndex+1-j);
    zaehler_right(j) = _vKnotVector(iIndex+j) - fParam;
    double saved = 0.0f;
    for (int r=0; r<j; r++)
    {
      double tmp = vFuncVals(r)/(zaehler_right(r+1) + zaehler_left(j-r));
      vFuncVals(r) = saved + zaehler_right(r+1)*tmp;
      saved = zaehler_left(j-r)*tmp;
    }
    
    vFuncVals(j) = saved;
  }
}

double BSplineBasis::BasisFunction(int iIndex, double fParam)
{
  int m = _vKnotVector.Length()-1;
  int p = _iOrder-1;
  double saved;
  TColStd_Array1OfReal N(0,p);

  if ((iIndex == 0 && fParam == _vKnotVector(0)) || 
      (iIndex == m-p-1 && fParam == _vKnotVector(m)))
  {
    return 1.0f;
  }

  if (fParam < _vKnotVector(iIndex) || fParam >= _vKnotVector(iIndex+p+1))
  {
    return 0.0f;
  }

  for (int j=0; j<=p; j++)
  {
    if (fParam >= _vKnotVector(iIndex+j) && fParam < _vKnotVector(iIndex+j+1))
      N(j) = 1.0f;
    else
      N(j) = 0.0f;
  }

  for (int k=1; k<=p; k++)
  {
    if (N(0) == 0.0f)
      saved = 0.0f;
    else
      saved = ((fParam - _vKnotVector(iIndex))*N(0)) / (_vKnotVector(iIndex+k) - _vKnotVector(iIndex));
    
    for (int j=0; j<p-k+1; j++)
    {
      double Tleft = _vKnotVector(iIndex+j+1);
      double Tright = _vKnotVector(iIndex+j+k+1);
     
      if (N(j+1) == 0.0)
      {
        N(j) = saved;
        saved = 0.0;
      }
      else 
      {
        double tmp = N(j+1)/(Tright-Tleft);
        N(j) = saved + (Tright - fParam)*tmp;
        saved = (fParam-Tleft)*tmp;
      }
    }
  }

  return N(0);
}

void BSplineBasis::DerivativesOfBasisFunction(int iIndex, int iMaxDer, double fParam,
                                               TColStd_Array1OfReal& Derivat)
{
  int iMax = iMaxDer;
  if (Derivat.Length() != iMax+1)
    Standard_RangeError::Raise("BSplineBasis");
  //k-te Ableitungen (k>Grad) sind Null 
  if (iMax >= _iOrder)
  {
   for (int i=_iOrder; i<=iMaxDer; i++)
     Derivat(i) = 0.0f;
     iMax = _iOrder - 1;
  }

  TColStd_Array1OfReal ND(0, iMax);
  int p = _iOrder-1;
  math_Matrix N(0,p,0,p);
  double saved;

  // falls Wert au�erhalb Intervall, dann Funktionswert und alle Ableitungen gleich Null
  if (fParam < _vKnotVector(iIndex) || fParam >= _vKnotVector(iIndex+p+1))
  {
    for (int k=0; k<=iMax; k++)
      Derivat(k) = 0.0f;
    return;
  }

  // Berechne die Basisfunktionen der Ordnung 1
  for (int j=0; j<_iOrder; j++)
  {
    if (fParam >= _vKnotVector(iIndex+j) && fParam < _vKnotVector(iIndex+j+1))
      N(j,0) = 1.0f;
    else 
      N(j,0) = 0.0f;
  }

  // Berechne Dreieckstabelle der Funktionswerte
  for (int k=1; k<_iOrder; k++)
  {
    if (N(0,k-1) == 0.0f)
      saved = 0.0f;
    else 
      saved = ((fParam - _vKnotVector(iIndex))*N(0,k-1))/(_vKnotVector(iIndex+k)-_vKnotVector(iIndex));
    for (int j=0; j<p-k+1; j++)
    {
      double Tleft = _vKnotVector(iIndex+j+1);
      double Tright = _vKnotVector(iIndex+j+k+1);

      if (N(j+1,k-1) == 0.0)
      {
        N(j,k) = saved;
        saved = 0.0f;
      }
      else
      {
        double tmp = N(j+1,k-1)/(Tright-Tleft);
        N(j,k) = saved + (Tright-fParam)*tmp;
        saved = (fParam-Tleft)*tmp;
      }
    }
  }

  // Funktionswert
  Derivat(0) = N(0,p);
  // Berechne aus der Dreieckstabelle die Ableitungen
  for (int k=1; k<=iMax; k++)
  {
    for (int j=0; j<=k; j++)
    {
      // Lade (p-k)-te Spalte
      ND(j) = N(j,p-k);
    }

    for (int jj=1; jj<=k; jj++)
    {
      if (ND(0) == 0.0f)
        saved = 0.0f;
      else
        saved = ND(0)/(_vKnotVector(iIndex+p-k+jj) - _vKnotVector(iIndex));

      for (int j=0; j<k-jj+1; j++)
      {
        double Tleft = _vKnotVector(iIndex+j+1);
        double Tright = _vKnotVector(iIndex+j+p-k+jj+1);
        if (ND(j+1) == 0.0f)
        {
          ND(j) = (p-k+jj)*saved;
          saved = 0.0f;
        }
        else
        {
          double tmp = ND(j+1)/(Tright-Tleft);
          ND(j) = (p-k+jj)*(saved-tmp);
          saved = tmp;
        }
      }
    }

    Derivat(k) = ND(0); //k-te Ableitung
  }

  return;
}

double BSplineBasis::DerivativeOfBasisFunction(int iIndex, int iMaxDer, double fParam)
{
  int iMax = iMaxDer;

  // Funktionswert (0-te Ableitung)
  if (iMax == 0)
    return BasisFunction(iIndex, fParam);

  //k-te Ableitungen (k>Grad) sind Null 
  if (iMax >= _iOrder)
  {
    return 0.0f;
  }

  TColStd_Array1OfReal ND(0, iMax);
  int p = _iOrder-1;
  math_Matrix N(0,p,0,p);
  double saved;

  // falls Wert au�erhalb Intervall, dann Funktionswert und Ableitungen gleich Null
  if (fParam < _vKnotVector(iIndex) || fParam >= _vKnotVector(iIndex+p+1))
  {
    return 0.0f;
  }

  // Berechne die Basisfunktionen der Ordnung 1
  for (int j=0; j<_iOrder; j++)
  {
    if (fParam >= _vKnotVector(iIndex+j) && fParam < _vKnotVector(iIndex+j+1))
      N(j,0) = 1.0f;
    else 
      N(j,0) = 0.0f;
  }

  // Berechne Dreieckstabelle der Funktionswerte
  for (int k=1; k<_iOrder; k++)
  {
    if (N(0,k-1) == 0.0f)
      saved = 0.0f;
    else 
      saved = ((fParam - _vKnotVector(iIndex))*N(0,k-1))/(_vKnotVector(iIndex+k)-_vKnotVector(iIndex));
    for (int j=0; j<p-k+1; j++)
    {
      double Tleft = _vKnotVector(iIndex+j+1);
      double Tright = _vKnotVector(iIndex+j+k+1);

      if (N(j+1,k-1) == 0.0)
      {
        N(j,k) = saved;
        saved = 0.0f;
      }
      else
      {
        double tmp = N(j+1,k-1)/(Tright-Tleft);
        N(j,k) = saved + (Tright-fParam)*tmp;
        saved = (fParam-Tleft)*tmp;
      }
    }
  }

  // Berechne aus der Dreieckstabelle die Ableitungen
  for (int j=0; j<=iMax; j++)
  {
    // Lade (p-iMax)-te Spalte
    ND(j) = N(j,p-iMax);
  }

  for (int jj=1; jj<=iMax; jj++)
  {
    if (ND(0) == 0.0f)
      saved = 0.0f;
    else
      saved = ND(0)/(_vKnotVector(iIndex+p-iMax+jj) - _vKnotVector(iIndex));

    for (int j=0; j<iMax-jj+1; j++)
    {
      double Tleft = _vKnotVector(iIndex+j+1);
      double Tright = _vKnotVector(iIndex+j+p-iMax+jj+1);
      if (ND(j+1) == 0.0f)
      {
        ND(j) = (p-iMax+jj)*saved;
        saved = 0.0f;
      }
      else
      {
        double tmp = ND(j+1)/(Tright-Tleft);
        ND(j) = (p-iMax+jj)*(saved-tmp);
        saved = tmp;
      }
    }
  }

  return ND(0); //iMax-te Ableitung
}

double BSplineBasis::GetIntegralOfProductOfBSplines(int iIdx1, int iIdx2, int iOrd1, int iOrd2)
{
  int iMax = CalcSize(iOrd1, iOrd2);
  double dIntegral=0.0f;
  double fMin, fMax;

  TColStd_Array1OfReal vRoots(0,iMax), vWeights(0,iMax);
  GenerateRootsAndWeights(vRoots, vWeights);

  /*Berechne das Integral*/
  // Integrationsbereich
  int iBegin=0; int iEnd=0;
  FindIntegrationArea(iIdx1, iIdx2, iBegin, iEnd);

  for (int j=iBegin; j<iEnd; j++)
  {
    fMax = _vKnotVector(j+1);
    fMin = _vKnotVector(j);

    if (fMax > fMin)
    {
      for (int i=0; i<=iMax; i++)
      {
        double fParam = 0.5*(vRoots(i)+1)*(fMax-fMin)+fMin;
        dIntegral += 0.5*(fMax-fMin)*vWeights(i) * 
          DerivativeOfBasisFunction(iIdx1, iOrd1, fParam) * 
          DerivativeOfBasisFunction(iIdx2, iOrd2, fParam);
      }
    }
  }

  return dIntegral;
}

void BSplineBasis::GenerateRootsAndWeights(TColStd_Array1OfReal& vRoots, TColStd_Array1OfReal& vWeights)
{
  int iSize = vRoots.Length();

  //Nullstellen der Legendre-Polynome und die zugeh. Gewichte
  if (iSize == 1)
  {
    vRoots(0) = 0.0f; vWeights(0) = 2.0f;
  }
  else if (iSize == 2)
  {
    vRoots(0) = 0.57735f;   vWeights(0) = 1.0f;
    vRoots(1) = -vRoots(0); vWeights(1) = vWeights(0);
  }
  else if (iSize == 4)
  {
    vRoots(0) = 0.33998f;   vWeights(0) = 0.65214f;
    vRoots(1) = 0.86113f;   vWeights(1) = 0.34785f;
    vRoots(2) = -vRoots(0); vWeights(2) = vWeights(0);
    vRoots(3) = -vRoots(1); vWeights(3) = vWeights(1);
  }
  else if (iSize == 6)
  {
    vRoots(0) = 0.23861f;   vWeights(0) = 0.46791f;
    vRoots(1) = 0.66120f;   vWeights(1) = 0.36076f;
    vRoots(2) = 0.93246f;   vWeights(2) = 0.17132f;
    vRoots(3) = -vRoots(0); vWeights(3) = vWeights(0);
    vRoots(4) = -vRoots(1); vWeights(4) = vWeights(1);
    vRoots(5) = -vRoots(2); vWeights(5) = vWeights(2);
  }
  else if (iSize == 8)
  {
    vRoots(0) = 0.18343f;   vWeights(0) = 0.36268f;
    vRoots(1) = 0.52553f;   vWeights(1) = 0.31370f;
    vRoots(2) = 0.79666f;   vWeights(2) = 0.22238f;
    vRoots(3) = 0.96028f;   vWeights(3) = 0.10122f;
    vRoots(4) = -vRoots(0); vWeights(4) = vWeights(0);
    vRoots(5) = -vRoots(1); vWeights(5) = vWeights(1);
    vRoots(6) = -vRoots(2); vWeights(6) = vWeights(2);
    vRoots(7) = -vRoots(3); vWeights(7) = vWeights(3);
  } 
  else if (iSize == 10)
  {
    vRoots(0) = 0.14887f;   vWeights(0) = 0.29552f;
    vRoots(1) = 0.43339f;   vWeights(1) = 0.26926f;
    vRoots(2) = 0.67940f;   vWeights(2) = 0.21908f;
    vRoots(3) = 0.86506f;   vWeights(3) = 0.14945f;
    vRoots(4) = 0.97390f;   vWeights(4) = 0.06667f;
    vRoots(5) = -vRoots(0); vWeights(5) = vWeights(0);
    vRoots(6) = -vRoots(1); vWeights(6) = vWeights(1);
    vRoots(7) = -vRoots(2); vWeights(7) = vWeights(2);
    vRoots(8) = -vRoots(3); vWeights(8) = vWeights(3);
    vRoots(9) = -vRoots(4); vWeights(9) = vWeights(4);
  }
  else
  {
    vRoots(0) = 0.12523f;   vWeights(0) = 0.24914f;
    vRoots(1) = 0.36783f;   vWeights(1) = 0.23349f;
    vRoots(2) = 0.58731f;   vWeights(2) = 0.20316f;
    vRoots(3) = 0.76990f;   vWeights(3) = 0.16007f;
    vRoots(4) = 0.90411f;   vWeights(4) = 0.10693f;
    vRoots(5) = 0.98156f;   vWeights(5) = 0.04717f;
    vRoots(6) = -vRoots(0); vWeights(6) = vWeights(0);
    vRoots(7) = -vRoots(1); vWeights(7) = vWeights(1);
    vRoots(8) = -vRoots(2); vWeights(8) = vWeights(2);
    vRoots(9) = -vRoots(3); vWeights(9) = vWeights(3);
    vRoots(10)= -vRoots(4); vWeights(10)= vWeights(4);
    vRoots(11)= -vRoots(5); vWeights(11)= vWeights(5);
  }
}

void BSplineBasis::FindIntegrationArea(int iIdx1, int iIdx2, int& iBegin, int& iEnd)
{
  // nach Index ordnen
  if (iIdx2 < iIdx1)
  {
    int tmp=iIdx1;
    iIdx1 = iIdx2;
    iIdx2 = tmp;
  }

  iBegin = iIdx2;
  iEnd   = iIdx1+_iOrder;
  if (iEnd == _vKnotVector.Upper())
    iEnd -= 1;
}

int BSplineBasis::CalcSize(int r, int s)
{
  int iMaxDegree = 2*(_iOrder-1)-r-s;

  if (iMaxDegree < 0)
    return 0;
  else if (iMaxDegree < 4)
    return 1;
  else if (iMaxDegree < 8)
    return 3;
  else if (iMaxDegree < 12)
    return 5;
  else if (iMaxDegree < 16)
    return 7;
  else if (iMaxDegree < 20)
    return 9;
  else
    return 11;
}

/////////////////// ParameterCorrection

ParameterCorrection::ParameterCorrection(unsigned short usUOrder, unsigned short usVOrder, 
                             unsigned short usUCtrlpoints, unsigned short usVCtrlpoints)
    : _usUOrder(usUOrder),
      _usVOrder(usVOrder),
      _usUCtrlpoints(usUCtrlpoints),
      _usVCtrlpoints(usVCtrlpoints),
      _vCtrlPntsOfSurf(0,usUCtrlpoints-1,0,usVCtrlpoints-1),
      _vUKnots(0,usUCtrlpoints-usUOrder+1),
      _vVKnots(0,usVCtrlpoints-usVOrder+1),
      _vUMults(0,usUCtrlpoints-usUOrder+1),
      _vVMults(0,usVCtrlpoints-usVOrder+1)
{
  _bGetUVDir = false;
  _bSmoothing = false;
  _fSmoothInfluence = 0.0f;
}

void ParameterCorrection::CalcEigenvectors()
{
    MeshCore::PlaneFit planeFit;
    //for (it = aclPoints.begin(); it!=aclPoints.end(); ++it)
    //    planeFit.AddPoint(*it);
    for (int i=_pvcPoints->Lower(); i<=_pvcPoints->Upper(); i++) {
        planeFit.AddPoint(Base::Vector3f(
            (float)(*_pvcPoints)(i).X(),
            (float)(*_pvcPoints)(i).Y(),
            (float)(*_pvcPoints)(i).Z()));
    }


    planeFit.Fit();
    _clU = planeFit.GetDirU();
    _clV = planeFit.GetDirV();
    _clW = planeFit.GetNormal();
}

bool ParameterCorrection::DoInitialParameterCorrection(float fSizeFactor)
{
  // falls Richtungen nicht vorgegeben, selber berechnen
  if (_bGetUVDir == false)
    CalcEigenvectors();
  if (!GetUVParameters(fSizeFactor))
    return false;
  if (_bSmoothing)
  {
    if (!SolveWithSmoothing(_fSmoothInfluence))
      return false;
  }
  else
  {
    if (!SolveWithoutSmoothing())
      return false;
  }

  return true;
}

bool ParameterCorrection::GetUVParameters(float fSizeFactor)
{
  // Eigenvektoren als neue Basis
  Base::Vector3f e[3];
  e[0] = _clU;
  e[1] = _clV;
  e[2] = _clW;
  
  //kanonische Basis des R^3
  Base::Vector3f b[3];
  b[0]=Base::Vector3f(1.0f,0.0f,0.0f); b[1]=Base::Vector3f(0.0f,1.0f,0.0f);b[2]=Base::Vector3f(0.0f,0.0f,1.0f);
  // Erzeuge ein Rechtssystem aus den orthogonalen Eigenvektoren
  if ((e[0]%e[1])*e[2] < 0)
  {
    Base::Vector3f tmp = e[0];
    e[0] = e[1];
    e[1] = tmp;
  }

  // Nun erzeuge die transpon. Rotationsmatrix
  Wm4::Matrix3f clRotMatTrans;
  for (int i=0; i<3; i++)
  {
    for (int j=0; j<3; j++)
    {
      clRotMatTrans[i][j] = b[j]*e[i];
    }
  }

  std::vector<Base::Vector2D> vcProjPts;
  Base::BoundBox2D clBBox;

  // Berechne die Koordinaten der transf. Punkte und projiz. diese auf die x,y-Ebene des neuen
  // Koordinatensystems
  for (int ii=_pvcPoints->Lower(); ii<=_pvcPoints->Upper(); ii++)
  {
      Wm4::Vector3f clProjPnt = clRotMatTrans * ( Wm4::Vector3f(
          (float)(*_pvcPoints)(ii).X(),
          (float)(*_pvcPoints)(ii).Y(),
          (float)(*_pvcPoints)(ii).Z()));
      vcProjPts.push_back(Base::Vector2D(clProjPnt.X(), clProjPnt.Y()));
    clBBox &= (Base::Vector2D(clProjPnt.X(), clProjPnt.Y()));
  }

  if ((clBBox.fMaxX == clBBox.fMinX) || (clBBox.fMaxY == clBBox.fMinY))
    return false;
  float tx = fSizeFactor*clBBox.fMinX-(fSizeFactor-1.0f)*clBBox.fMaxX;
  float ty = fSizeFactor*clBBox.fMinY-(fSizeFactor-1.0f)*clBBox.fMaxY;
  float fDeltaX = (2*fSizeFactor-1.0f)*(clBBox.fMaxX - clBBox.fMinX);
  float fDeltaY = (2*fSizeFactor-1.0f)*(clBBox.fMaxY - clBBox.fMinY);

  // Berechne die u,v-Parameter mit u,v aus [0,1]
  _pvcUVParam->Init(gp_Pnt2d(0.0f, 0.0f));
  int ii=0;
  if (clBBox.fMaxX - clBBox.fMinX >= clBBox.fMaxY - clBBox.fMinY)
      for (std::vector<Base::Vector2D>::iterator It2=vcProjPts.begin(); It2!=vcProjPts.end(); It2++)
    {
      (*_pvcUVParam)(ii) = gp_Pnt2d((It2->fX-tx)/fDeltaX, (It2->fY-ty)/fDeltaY);
      ii++;
    }
  else
      for (std::vector<Base::Vector2D>::iterator It2=vcProjPts.begin(); It2!=vcProjPts.end(); It2++)
    {
      (*_pvcUVParam)(ii) = gp_Pnt2d((It2->fY-ty)/fDeltaY, (It2->fX-tx)/fDeltaX);
      ii++;
    }

  return true;
}

void ParameterCorrection::SetUVW(const Base::Vector3f& clU, const Base::Vector3f& clV, const Base::Vector3f& clW, bool bUseDir)
{
  _clU = clU;
  _clV = clV;
  _clW = clW;
  _bGetUVDir = bUseDir;
}

void ParameterCorrection::GetUVW(Base::Vector3f& clU, Base::Vector3f& clV, Base::Vector3f& clW) const
{
  clU = _clU;
  clV = _clV;
  clW = _clW;
}

Base::Vector3f ParameterCorrection::GetGravityPoint() const
{
  unsigned long ulSize = _pvcPoints->Length();
  float x=0.0f, y=0.0f, z=0.0f;
  for (int i=_pvcPoints->Lower(); i<=_pvcPoints->Upper(); i++)
  {
    x += (float)(*_pvcPoints)(i).X();
    y += (float)(*_pvcPoints)(i).Y();
    z += (float)(*_pvcPoints)(i).Z();
  }

  return Base::Vector3f(float(x/ulSize), float(y/ulSize), float(z/ulSize));
}

Handle(Geom_BSplineSurface) ParameterCorrection::CreateSurface(const TColgp_Array1OfPnt& points, 
                                                                unsigned short usIter,
                                                                bool  bParaCor,
                                                                float fSizeFactor)
{
  if (_pvcPoints != NULL)
  {
    delete _pvcPoints;
    _pvcPoints = NULL;
    delete _pvcUVParam;
    _pvcUVParam = NULL;
  }
  _pvcPoints = new TColgp_Array1OfPnt(points.Lower(), points.Upper());
  *_pvcPoints = points;
  _pvcUVParam = new TColgp_Array1OfPnt2d(points.Lower(), points.Upper());

  if (_usUCtrlpoints*_usVCtrlpoints > _pvcPoints->Length())
    return NULL; //LGS unterbestimmt
  if(!DoInitialParameterCorrection(fSizeFactor))
    return NULL;

  if (bParaCor)
    DoParameterCorrection(usIter);

  return new Geom_BSplineSurface
                      (_vCtrlPntsOfSurf,
                       _vUKnots, 
                       _vVKnots,
                       _vUMults,
                       _vVMults,
                       _usUOrder-1,
                       _usVOrder-1);
}

void ParameterCorrection::EnableSmoothing(bool bSmooth, float fSmoothInfl)
{
  _bSmoothing = bSmooth;
  _fSmoothInfluence = fSmoothInfl;
}

/////////////////// BSplineParameterCorrection


BSplineParameterCorrection::BSplineParameterCorrection(unsigned short usUOrder, unsigned short usVOrder, 
                             unsigned short usUCtrlpoints, unsigned short usVCtrlpoints)
    : ParameterCorrection
        (usUOrder, usVOrder, usUCtrlpoints, usVCtrlpoints),
      _clUSpline(usUCtrlpoints+usUOrder),
      _clVSpline(usVCtrlpoints+usVOrder),
      _clSmoothMatrix
          (0,usUCtrlpoints*usVCtrlpoints-1,
           0,usUCtrlpoints*usVCtrlpoints-1),
      _clFirstMatrix
          (0,usUCtrlpoints*usVCtrlpoints-1,
           0,usUCtrlpoints*usVCtrlpoints-1),
      _clSecondMatrix
          (0,usUCtrlpoints*usVCtrlpoints-1,
           0,usUCtrlpoints*usVCtrlpoints-1),
      _clThirdMatrix
          (0,usUCtrlpoints*usVCtrlpoints-1,
           0,usUCtrlpoints*usVCtrlpoints-1)
{
  Init();
}

void BSplineParameterCorrection::Init()
{
  // Initialisierungen
  _pvcUVParam       = NULL;
  _pvcPoints        = NULL;
  _clFirstMatrix.Init(0.0f);
  _clSecondMatrix.Init(0.0f);
  _clThirdMatrix.Init(0.0f);
  _clSmoothMatrix.Init(0.0f);
  
  /* Berechne die Knotenvektoren */
  unsigned short usUMax = _usUCtrlpoints-_usUOrder+1;
  unsigned short usVMax = _usVCtrlpoints-_usVOrder+1;

  // Knotenvektor f�r die CAS.CADE-Klasse
  // u-Richtung
  for (int i=0;i<=usUMax; i++)
  {
    _vUKnots(i) = ((float)i) / ((float)usUMax);
    _vUMults(i) = 1;
  }
  _vUMults(0) = _usUOrder;
  _vUMults(usUMax) = _usUOrder;
  // v-Richtung
  for (int i=0; i<=usVMax; i++)
  {
    _vVKnots(i) = ((float)i) / ((float)usVMax);
    _vVMults(i) = 1;
  }
  _vVMults(0) = _usVOrder;
  _vVMults(usVMax) = _usVOrder;

  // Setzen der B-Spline-Basisfunktionen
  _clUSpline.SetKnots(_vUKnots, _vUMults, _usUOrder);
  _clVSpline.SetKnots(_vVKnots, _vVMults, _usVOrder);
}

void BSplineParameterCorrection::SetUKnots(const std::vector<float>& afKnots)
{
  if (afKnots.size() != (unsigned long)(_usUCtrlpoints+_usUOrder))
    return;

  unsigned short usUMax = _usUCtrlpoints-_usUOrder+1;

  // Knotenvektor f�r die CAS.CADE-Klasse
  // u-Richtung
  for (int i=1;i<usUMax; i++)
  {
    _vUKnots(i) = afKnots[_usUOrder+i-1];
    _vUMults(i) = 1;
  }

  // Setzen der B-Spline-Basisfunktionen
  _clUSpline.SetKnots(_vUKnots, _vUMults, _usUOrder);
}

void BSplineParameterCorrection::SetVKnots(const std::vector<float>& afKnots)
{
  if (afKnots.size() != (unsigned long)(_usVCtrlpoints+_usVOrder))
    return;

  unsigned short usVMax = _usVCtrlpoints-_usVOrder+1;

  // Knotenvektor f�r die CAS.CADE-Klasse
  // v-Richtung
  for (int i=1; i<usVMax; i++)
  {
    _vVKnots(i) = afKnots[_usVOrder+i-1];
    _vVMults(i) = 1;
  }

  // Setzen der B-Spline-Basisfunktionen
  _clVSpline.SetKnots(_vVKnots, _vVMults, _usVOrder);
}

void BSplineParameterCorrection::DoParameterCorrection(unsigned short usIter)
{
  int i=0;
  float fMaxDiff=0.0f, fMaxScalar=1.0f;
  float fWeight = _fSmoothInfluence;

  Base::SequencerLauncher seq("Calc surface...", usIter*_pvcPoints->Length());

  do
  {
    fMaxScalar = 1.0f;
    fMaxDiff   = 0.0f;

    Geom_BSplineSurface* pclBSplineSurf = new Geom_BSplineSurface
                        (_vCtrlPntsOfSurf,
                         _vUKnots, 
                         _vVKnots,
                         _vUMults,
                         _vVMults,
                         _usUOrder-1,
                         _usVOrder-1);

    for (int ii=_pvcPoints->Lower();ii <=_pvcPoints->Upper();ii++)
    {  

      double fDeltaU, fDeltaV, fU, fV;
      gp_Vec P((*_pvcPoints)(ii).X(), (*_pvcPoints)(ii).Y(), (*_pvcPoints)(ii).Z());
      gp_Pnt PntX;
      gp_Vec Xu, Xv, Xuv, Xuu, Xvv;
      //Berechne die ersten beiden Ableitungen und Punkt an der Stelle (u,v)
      pclBSplineSurf->D2((*_pvcUVParam)(ii).X(), (*_pvcUVParam)(ii).Y(), PntX, Xu, Xv, Xuu, Xvv, Xuv);
      gp_Vec X(PntX.X(), PntX.Y(), PntX.Z());
      gp_Vec ErrorVec = X - P;

      // Berechne Xu x Xv die Normale in X(u,v)
      gp_Dir clNormal = Xu ^ Xv;

      //Pr�fe, ob X = P
      if (!(X.IsEqual(P,0.001,0.001)))
      {
        ErrorVec.Normalize();
        if(fabs(clNormal*ErrorVec) < fMaxScalar)
          fMaxScalar = (float)fabs(clNormal*ErrorVec);
      }

      fDeltaU =  ( (P-X) * Xu ) / ( (P-X)*Xuu - Xu*Xu );
      if (fabs(fDeltaU) < FLOAT_EPS)
        fDeltaU = 0.0f;
      fDeltaV =  ( (P-X) * Xv ) / ( (P-X)*Xvv - Xv*Xv );
      if (fabs(fDeltaV) < FLOAT_EPS)
        fDeltaV = 0.0f;

      //Ersetze die alten u/v-Werte durch die neuen
      fU = (*_pvcUVParam)(ii).X() - fDeltaU;
      fV = (*_pvcUVParam)(ii).Y() - fDeltaV;
      if (fU <= 1.0f && fU >= 0.0f &&
          fV <= 1.0f && fV >= 0.0f)
      {
        (*_pvcUVParam)(ii).SetX(fU);
        (*_pvcUVParam)(ii).SetY(fV);
        fMaxDiff = std::max<float>(float(fabs(fDeltaU)), fMaxDiff);
        fMaxDiff = std::max<float>(float(fabs(fDeltaV)), fMaxDiff);
      }

      seq.next();
    }

    if (_bSmoothing)
    {
      fWeight *= 0.5f;
      SolveWithSmoothing(fWeight);
    }
    else
      SolveWithoutSmoothing();

    i++;
  }
  while(i<usIter && fMaxDiff > FLOAT_EPS && fMaxScalar < 0.99);
}

bool BSplineParameterCorrection::SolveWithoutSmoothing()
{
  unsigned long ulSize = _pvcPoints->Length();
  math_Matrix M  (0, ulSize-1, 0,_usUCtrlpoints*_usVCtrlpoints-1);
  math_Matrix Xx (0, _usUCtrlpoints*_usVCtrlpoints-1,0,0);
  math_Matrix Xy (0, _usUCtrlpoints*_usVCtrlpoints-1,0,0);
  math_Matrix Xz (0, _usUCtrlpoints*_usVCtrlpoints-1,0,0);
  math_Vector bx (0, ulSize-1);
  math_Vector by (0, ulSize-1);
  math_Vector bz (0, ulSize-1);

  //Bestimmung der Koeffizientenmatrix des �berbestimmten LGS
  for (unsigned long i=0; i<ulSize; i++)
  {
    float fU = (float)(*_pvcUVParam)(i).X();
    float fV = (float)(*_pvcUVParam)(i).Y();
    unsigned long ulIdx=0;

    for (unsigned short j=0; j<_usUCtrlpoints; j++)
    {
      for (unsigned short k=0; k<_usVCtrlpoints; k++)
      {
        M(i,ulIdx) = _clUSpline.BasisFunction(j,fU)*_clVSpline.BasisFunction(k,fV);
        ulIdx++;
      }
    }
  }

  //Bestimmen der rechten Seite
  for (int ii=_pvcPoints->Lower(); ii<=_pvcPoints->Upper(); ii++)
  {
    bx(ii) = (*_pvcPoints)(ii).X(); by(ii) = (*_pvcPoints)(ii).Y(); bz(ii) = (*_pvcPoints)(ii).Z();
  }

  // L�se das �berbest. LGS mit der Householder-Transformation
  math_Householder hhX(M,bx);
  math_Householder hhY(M,by);
  math_Householder hhZ(M,bz);

  if (!(hhX.IsDone() && hhY.IsDone() && hhZ.IsDone()))
    //LGS konnte nicht gel�st werden
    return false;
  Xx = hhX.AllValues();
  Xy = hhY.AllValues();
  Xz = hhZ.AllValues();

  unsigned long ulIdx=0;
  for (unsigned short j=0;j<_usUCtrlpoints;j++)
  {
    for (unsigned short k=0;k<_usVCtrlpoints;k++)
    {
      _vCtrlPntsOfSurf(j,k) = gp_Pnt(Xx(ulIdx,0),Xy(ulIdx,0),Xz(ulIdx,0));
      ulIdx++;
    }
  }

  return true;
}

bool BSplineParameterCorrection::SolveWithSmoothing(float fWeight)
{
  unsigned long ulSize = _pvcPoints->Length();
  unsigned long ulDim  = _usUCtrlpoints*_usVCtrlpoints;
  math_Matrix M  (0, ulSize-1, 0,_usUCtrlpoints*_usVCtrlpoints-1);
  math_Matrix MTM(0, _usUCtrlpoints*_usVCtrlpoints-1, 
                  0, _usUCtrlpoints*_usVCtrlpoints-1);
  math_Vector Xx (0, _usUCtrlpoints*_usVCtrlpoints-1);
  math_Vector Xy (0, _usUCtrlpoints*_usVCtrlpoints-1);
  math_Vector Xz (0, _usUCtrlpoints*_usVCtrlpoints-1);
  math_Vector bx (0, ulSize-1);
  math_Vector by (0, ulSize-1);
  math_Vector bz (0, ulSize-1);
  math_Vector Mbx(0, _usUCtrlpoints*_usVCtrlpoints-1);
  math_Vector Mby(0, _usUCtrlpoints*_usVCtrlpoints-1);
  math_Vector Mbz(0, _usUCtrlpoints*_usVCtrlpoints-1);

  //Bestimmung der Koeffizientenmatrix des �berbestimmten LGS
  for (unsigned long i=0; i<ulSize; i++)
  {
    float fU = (float)(*_pvcUVParam)(i).X();
    float fV = (float)(*_pvcUVParam)(i).Y();
    unsigned long ulIdx=0;

    for (unsigned short j=0; j<_usUCtrlpoints; j++)
    {
      for (unsigned short k=0; k<_usVCtrlpoints; k++)
      {
        M(i,ulIdx) = _clUSpline.BasisFunction(j,fU)*_clVSpline.BasisFunction(k,fV);
        ulIdx++;
      }
    }
  }
  //Das Produkt aus ihrer Transformierten und ihr selbst ergibt die quadratische Systemmatrix
  for (unsigned long m=0; m<ulDim; m++)
  {
    for (unsigned long n=m; n<ulDim; n++)
    {
      MTM(m,n)=MTM(n,m)=M.Col(m)*M.Col(n);
    }
  }

  //Bestimmen der rechten Seite
  for (int ii=_pvcPoints->Lower(); ii<=_pvcPoints->Upper(); ii++)
  {
    bx(ii) = (*_pvcPoints)(ii).X(); by(ii) = (*_pvcPoints)(ii).Y(); bz(ii) = (*_pvcPoints)(ii).Z();
  }
  for (unsigned long i=0; i<ulDim; i++)
  {
    Mbx(i) = M.Col(i) * bx;
    Mby(i) = M.Col(i) * by;
    Mbz(i) = M.Col(i) * bz;
  }

  // L�se das LGS mit der LU-Zerlegung
  math_Gauss mgGaussX(MTM+fWeight*_clSmoothMatrix);
  math_Gauss mgGaussY(MTM+fWeight*_clSmoothMatrix);
  math_Gauss mgGaussZ(MTM+fWeight*_clSmoothMatrix);

  mgGaussX.Solve(Mbx,Xx);
  if (!mgGaussX.IsDone())
    return false;

  mgGaussY.Solve(Mby,Xy);
  if (!mgGaussY.IsDone())
    return false;

  mgGaussZ.Solve(Mbz,Xz);
  if (!mgGaussZ.IsDone())
    return false;

  unsigned long ulIdx=0;
  for (unsigned short j=0;j<_usUCtrlpoints;j++)
  {
    for (unsigned short k=0;k<_usVCtrlpoints;k++)
    {
      _vCtrlPntsOfSurf(j,k) = gp_Pnt(Xx(ulIdx),Xy(ulIdx),Xz(ulIdx));
      ulIdx++;
    }
  }

  return true;
}

void BSplineParameterCorrection::CalcSmoothingTerms(bool bRecalc, float fFirst, float fSecond, float fThird)
{
  if (bRecalc)
  {
    Base::SequencerLauncher seq("Initializing...", 3 * _usUCtrlpoints * _usUCtrlpoints * _usVCtrlpoints * _usVCtrlpoints);
    CalcFirstSmoothMatrix(seq);
    CalcSecondSmoothMatrix(seq);
    CalcThirdSmoothMatrix(seq);
  }

  _clSmoothMatrix = fFirst  * _clFirstMatrix  + 
                    fSecond * _clSecondMatrix +
                    fThird  * _clThirdMatrix  ;
}

void BSplineParameterCorrection::CalcFirstSmoothMatrix(Base::SequencerLauncher& seq)
{
  unsigned long m=0;
  for (unsigned long k=0; k<_usUCtrlpoints; k++)
  {
    for (unsigned long l=0; l<_usVCtrlpoints; l++)
    {
      unsigned long n=0;

      for (unsigned short i=0; i<_usUCtrlpoints; i++)
      {
        for (unsigned short j=0; j<_usVCtrlpoints; j++)
        {
          _clFirstMatrix(m,n) =   _clUSpline.GetIntegralOfProductOfBSplines(i,k,1,1) *
                                  _clVSpline.GetIntegralOfProductOfBSplines(j,l,0,0) +
                                  _clUSpline.GetIntegralOfProductOfBSplines(i,k,0,0) *
                                  _clVSpline.GetIntegralOfProductOfBSplines(j,l,1,1);
          seq.next();
          n++;
        }
      }
      m++;
    }
  }
}

void BSplineParameterCorrection::CalcSecondSmoothMatrix(Base::SequencerLauncher& seq)
{
  unsigned long m=0;
  for (unsigned long k=0; k<_usUCtrlpoints; k++)
  {
    for (unsigned long l=0; l<_usVCtrlpoints; l++)
    {
      unsigned long n=0;

      for (unsigned short i=0; i<_usUCtrlpoints; i++)
      {
        for (unsigned short j=0; j<_usVCtrlpoints; j++)
        {
          _clSecondMatrix(m,n) =  _clUSpline.GetIntegralOfProductOfBSplines(i,k,2,2) *
                                  _clVSpline.GetIntegralOfProductOfBSplines(j,l,0,0) +
                                2*_clUSpline.GetIntegralOfProductOfBSplines(i,k,1,1) *
                                  _clVSpline.GetIntegralOfProductOfBSplines(j,l,1,1) +
                                  _clUSpline.GetIntegralOfProductOfBSplines(i,k,0,0) *
                                  _clVSpline.GetIntegralOfProductOfBSplines(j,l,2,2);
          seq.next();
          n++;
        }
      }
      m++;
    }
  }
}

void BSplineParameterCorrection::CalcThirdSmoothMatrix(Base::SequencerLauncher& seq)
{
  unsigned long m=0;
  for (unsigned long k=0; k<_usUCtrlpoints; k++)
  {
    for (unsigned long l=0; l<_usVCtrlpoints; l++)
    {
      unsigned long n=0;

      for (unsigned short i=0; i<_usUCtrlpoints; i++)
      {
        for (unsigned short j=0; j<_usVCtrlpoints; j++)
        {
          _clThirdMatrix(m,n) = _clUSpline.GetIntegralOfProductOfBSplines(i,k,3,3) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,0,0) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,3,1) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,0,2) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,1,3) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,2,0) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,1,1) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,2,2) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,2,2) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,1,1) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,0,2) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,3,1) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,2,0) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,1,3) +
                                _clUSpline.GetIntegralOfProductOfBSplines(i,k,0,0) *
                                _clVSpline.GetIntegralOfProductOfBSplines(j,l,3,3) ;
          seq.next();
          n++;
        }
      }
      m++;
    }
  }
}

void BSplineParameterCorrection::EnableSmoothing(bool bSmooth, float fSmoothInfl)
{
    EnableSmoothing(bSmooth, fSmoothInfl, 1.0f, 0.0f, 0.0f);
}

void BSplineParameterCorrection::EnableSmoothing(bool bSmooth, float fSmoothInfl, 
                                                 float fFirst, float fSec, float fThird)
{
    if (_bSmoothing && bSmooth)
        CalcSmoothingTerms(false, fFirst, fSec, fThird);
    else if (bSmooth)
        CalcSmoothingTerms(true, fFirst, fSec, fThird);

    ParameterCorrection::EnableSmoothing(bSmooth, fSmoothInfl);
}

const math_Matrix& BSplineParameterCorrection::GetFirstSmoothMatrix() const
{
    return _clFirstMatrix;
}

const math_Matrix& BSplineParameterCorrection::GetSecondSmoothMatrix() const
{
    return _clSecondMatrix;
}

const math_Matrix& BSplineParameterCorrection::GetThirdSmoothMatrix() const
{
    return _clThirdMatrix;
}

void BSplineParameterCorrection::SetFirstSmoothMatrix(const math_Matrix& rclMat)
{
    _clFirstMatrix = rclMat;
}

void BSplineParameterCorrection::SetSecondSmoothMatrix(const math_Matrix& rclMat)
{
    _clSecondMatrix = rclMat;
}

void BSplineParameterCorrection::SetThirdSmoothMatrix(const math_Matrix& rclMat)
{
    _clThirdMatrix = rclMat;
}
