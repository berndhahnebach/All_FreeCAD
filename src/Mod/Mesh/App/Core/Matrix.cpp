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


#include "PreCompiled.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Matrix.h"

using namespace Mesh;

Matrix4D::Matrix4D (void)
{
  (*this).Unit();
}

Matrix4D::Matrix4D (const Matrix4D& rclMtrx)
{
  (*this) = rclMtrx;
}

void Matrix4D::Unit (void)
{
  short iz, is;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++)
      dMtrx4D[iz][is] = 0;
  
  dMtrx4D[0][0] = 1;
  dMtrx4D[1][1] = 1;
  dMtrx4D[2][2] = 1;
  dMtrx4D[3][3] = 1;
}

void Matrix4D::SetMoveX (float fMove)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[0][3] = fMove;
  (*this) *= clMat;
}

void Matrix4D::SetMoveY (float fMove)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[1][3] = fMove;
  (*this) *= clMat;
}

void Matrix4D::SetMoveZ (float fMove)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[2][3] = fMove;
  (*this) *= clMat;
}

void Matrix4D::SetMove (const Vector3D& rclVct)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[0][3] = rclVct.x;
  clMat.dMtrx4D[1][3] = rclVct.y;
  clMat.dMtrx4D[2][3] = rclVct.z;
  (*this) *= clMat;
}

void Matrix4D::SetScaleX (float fScale)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[0][0] = fScale;
  
  (*this) *= clMat;
}

void Matrix4D::SetScaleY (float fScale)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[1][1] = fScale;
  (*this) *= clMat;
}

void Matrix4D::SetScaleZ (float fScale)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[2][2] = fScale;
  (*this) *= clMat;
}

void Matrix4D::SetScale (const Vector3D& rclVct)
{
  Matrix4D clMat;

  clMat.Unit();
  clMat.dMtrx4D[0][0] = rclVct.x;
  clMat.dMtrx4D[1][1] = rclVct.y;
  clMat.dMtrx4D[2][2] = rclVct.z;
  (*this) *= clMat;
}

void Matrix4D::SetRotX (float fAngle)
{
  Matrix4D clMat;
  float fsin, fcos;
 
  fsin = (float)sin (fAngle);
  fcos = (float)cos (fAngle);
  clMat.dMtrx4D[1][1] =  fcos;  clMat.dMtrx4D[2][2] = fcos;
  clMat.dMtrx4D[1][2] = -fsin;  clMat.dMtrx4D[2][1] = fsin;
  
  (*this) *= clMat;
}

void Matrix4D::SetRotY (float fAngle)
{
  Matrix4D clMat;
  float fsin, fcos;
 
  fsin = (float)sin (fAngle);
  fcos = (float)cos (fAngle);
  clMat.dMtrx4D[0][0] =  fcos;  clMat.dMtrx4D[2][2] = fcos;
  clMat.dMtrx4D[2][0] = -fsin;  clMat.dMtrx4D[0][2] = fsin;
  
  (*this) *= clMat;
}

void Matrix4D::SetRotZ (float fAngle)
{
  Matrix4D clMat;
  float fsin, fcos;
 
  fsin = (float)sin (fAngle);
  fcos = (float)cos (fAngle);
  clMat.dMtrx4D[0][0] =  fcos;  clMat.dMtrx4D[1][1] = fcos;
  clMat.dMtrx4D[0][1] = -fsin;  clMat.dMtrx4D[1][0] = fsin;
  
  (*this) *= clMat;
}

void Matrix4D::SetRotLine (const Vector3D& rclVct, float fAngle)
{
  // **** Algorithmus wurde aus einem Mathebuch entnohmen 
  Matrix4D  clMA, clMB, clMC, clMRot;
  Vector3D  clRotAxis(rclVct);
  short iz, is;
  float fcos, fsin;

  // alle Eintraege auf "0" gesetzt
  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++)  {
      clMA.dMtrx4D[iz][is] = 0;
      clMB.dMtrx4D[iz][is] = 0;
      clMC.dMtrx4D[iz][is] = 0;
      }

  // ** normieren der Rotationsachse 
  clRotAxis.Normalize();
  
  // ** Aufstellen der Rotaionsmatrix (Formel aus einem Mathebuch) */
  fcos = (float)cos(fAngle);
  fsin = (float)sin(fAngle);
  
  clMA.dMtrx4D[0][0] = (1-fcos) * clRotAxis.x * clRotAxis.x;
  clMA.dMtrx4D[0][1] = (1-fcos) * clRotAxis.x * clRotAxis.y;
  clMA.dMtrx4D[0][2] = (1-fcos) * clRotAxis.x * clRotAxis.z;
  clMA.dMtrx4D[1][0] = (1-fcos) * clRotAxis.x * clRotAxis.y;
  clMA.dMtrx4D[1][1] = (1-fcos) * clRotAxis.y * clRotAxis.y;
  clMA.dMtrx4D[1][2] = (1-fcos) * clRotAxis.y * clRotAxis.z;
  clMA.dMtrx4D[2][0] = (1-fcos) * clRotAxis.x * clRotAxis.z;
  clMA.dMtrx4D[2][1] = (1-fcos) * clRotAxis.y * clRotAxis.z;
  clMA.dMtrx4D[2][2] = (1-fcos) * clRotAxis.z * clRotAxis.z;

  clMB.dMtrx4D[0][0] = fcos;
  clMB.dMtrx4D[1][1] = fcos;
  clMB.dMtrx4D[2][2] = fcos;

  clMC.dMtrx4D[0][1] = -fsin * clRotAxis.z;
  clMC.dMtrx4D[0][2] =  fsin * clRotAxis.y;
  clMC.dMtrx4D[1][0] =  fsin * clRotAxis.z;
  clMC.dMtrx4D[1][2] = -fsin * clRotAxis.x;
  clMC.dMtrx4D[2][0] = -fsin * clRotAxis.y;
  clMC.dMtrx4D[2][1] =  fsin * clRotAxis.x;

  for (iz = 0; iz < 3; iz++)
    for (is = 0; is < 3; is++)
      clMRot.dMtrx4D[iz][is] = clMA.dMtrx4D[iz][is]  + clMB.dMtrx4D[iz][is] +
                               clMC.dMtrx4D[iz][is];
  (*this) *= clMRot;
}

void Matrix4D::SetRotLine   (const Vector3D& rclBase, const Vector3D& rclDir, float fAngle)
{
  Matrix4D  clMT, clMRot, clMInvT, clM;
  Vector3D clBase(rclBase);
  
  clMT.SetMove(clBase);            // Translation
  clMInvT.SetMove(clBase *= (-1.0f));  // inverse Translation
  clMRot.SetRotLine(rclDir, fAngle);

  clM = clMRot * clMInvT;
  clM = clMT * clM; 
  (*this) *= clM;  
}

void Matrix4D::Inverse (void)
{
  Matrix4D clInvTrlMat, clInvRotMat;
  short  iz, is;

  /**** Herausnehmen und Inversion der TranslationsMatrix
	aus der TransformationMatrix                      ****/
  for (iz = 0 ;iz < 3; iz++)
    clInvTrlMat.dMtrx4D[iz][3] = -dMtrx4D[iz][3];

  /**** Herausnehmen und Inversion der RotationsMatrix
	aus der TransformationMatrix                      ****/
  for (iz = 0 ;iz < 3; iz++)
    for (is = 0 ;is < 3; is++)
      clInvRotMat.dMtrx4D[iz][is] = dMtrx4D[is][iz];
      
  /**** inv(M) = inv(Mtrl * Mrot) = inv(Mrot) * inv(Mtrl) ****/
  (*this) = clInvRotMat * clInvTrlMat;
}

typedef  double * Matrix;

void Matrix_gauss(Matrix a, Matrix b)
{
  int ipiv[4], indxr[4], indxc[4];
  int i,j,k,l,ll;
  int irow=0, icol=0;
  double big, pivinv;
  double dum;
  for (j = 0; j < 4; j++)
    ipiv[j] = 0;
  for (i = 0; i < 4; i++) {
    big = 0;
    for (j = 0; j < 4; j++) {
      if (ipiv[j] != 1) {
	for (k = 0; k < 4; k++) {
	  if (ipiv[k] == 0) {
	    if (fabs(a[4*j+k]) >= big) {
	      big = fabs(a[4*j+k]);
	      irow = j;
	      icol = k;
	    }
	  } else if (ipiv[k] > 1)
	    return;  /* Singular matrix */
	}
      }
    }
    ipiv[icol] = ipiv[icol]+1;
    if (irow != icol) {
      for (l = 0; l < 4; l++) {
	dum = a[4*irow+l];
	a[4*irow+l] = a[4*icol+l];
	a[4*icol+l] = dum;
      }
      for (l = 0; l < 4; l++) {
	dum = b[4*irow+l];
	b[4*irow+l] = b[4*icol+l];
	b[4*icol+l] = dum;
      }
    }
    indxr[i] = irow;
    indxc[i] = icol;
    if (a[4*icol+icol] == 0) return;
    pivinv = 1.0/a[4*icol+icol];
    a[4*icol+icol] = 1.0;
    for (l = 0; l < 4; l++)
      a[4*icol+l] = a[4*icol+l]*pivinv;
    for (l = 0; l < 4; l++)
      b[4*icol+l] = b[4*icol+l]*pivinv;
    for (ll = 0; ll < 4; ll++) {
      if (ll != icol) {
	dum = a[4*ll+icol];
	a[4*ll+icol] = 0;
	for (l = 0; l < 4; l++)
	  a[4*ll+l] = a[4*ll+l] - a[4*icol+l]*dum;
	for (l = 0; l < 4; l++)
	  b[4*ll+l] = b[4*ll+l] - b[4*icol+l]*dum;
      }
    }
  }
  for (l = 3; l >= 0; l--) {
    if (indxr[l] != indxc[l]) {
      for (k = 0; k < 4; k++) {
	dum = a[4*k+indxr[l]];
	a[4*k+indxr[l]] = a[4*k+indxc[l]];
	a[4*k+indxc[l]] = dum;
      }
    }
  }
}
/* ------------------------------------------------------------------------
   Matrix_identity(Matrix a)

   Puts an identity matrix in matrix a
   ------------------------------------------------------------------------ */

void Matrix_identity (Matrix a)
{
  int i;
  for (i = 0; i < 16; i++) a[i] = 0;
  a[0] = 1;
  a[5] = 1;
  a[10] = 1;
  a[15] = 1;
}

/* ------------------------------------------------------------------------
   Matrix_invert(Matrix a, Matrix inva)

   Inverts Matrix a and places the result in inva.
   Relies on the Gaussian Elimination code above. (See Numerical recipes).
   ------------------------------------------------------------------------ */
void Matrix_invert (Matrix a, Matrix inva)
{

  double  temp[16];
  int     i;

  for (i = 0; i < 16; i++)
    temp[i] = a[i];
  Matrix_identity(inva);
  Matrix_gauss(temp,inva);
}

void Matrix4D::InverseGauss (void)
{
  double matrix        [16]; 
  double inversematrix [16] = { 1 ,0 ,0 ,0 ,
								                0 ,1 ,0 ,0 ,
								                0 ,0 ,1 ,0 ,
								                0 ,0 ,0 ,1 }; 
  GetGLMatrix(matrix);

//  Matrix_invert(matrix, inversematrix);
  Matrix_gauss(matrix,inversematrix);

  SetGLMatrix(inversematrix);
}

DataStream & Matrix4D::SaveData (DataStream& ofs)
{
  ofs << dMtrx4D[0][0] << dMtrx4D[0][1]
      << dMtrx4D[0][2] << dMtrx4D[0][3]
      << dMtrx4D[1][0] << dMtrx4D[1][1]
      << dMtrx4D[1][2] << dMtrx4D[1][3]
      << dMtrx4D[2][0] << dMtrx4D[2][1]
      << dMtrx4D[2][2] << dMtrx4D[2][3]
      << dMtrx4D[3][0] << dMtrx4D[3][1]
      << dMtrx4D[3][2] << dMtrx4D[3][3];
  return ofs;
}

DataStream & Matrix4D::LoadData (DataStream& ifs)
{
  ifs >> dMtrx4D[0][0] >> dMtrx4D[0][1]
      >> dMtrx4D[0][2] >> dMtrx4D[0][3]
      >> dMtrx4D[1][0] >> dMtrx4D[1][1]
      >> dMtrx4D[1][2] >> dMtrx4D[1][3]
      >> dMtrx4D[2][0] >> dMtrx4D[2][1]
      >> dMtrx4D[2][2] >> dMtrx4D[2][3]
      >> dMtrx4D[3][0] >> dMtrx4D[3][1]
      >> dMtrx4D[3][2] >> dMtrx4D[3][3];
 
  return ifs;
}

void Matrix4D::GetGLMatrix (double dMtrx[16]) const
{
  short iz, is;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++)
      dMtrx[ iz + 4*is ] = dMtrx4D[iz][is];
}

void Matrix4D::SetGLMatrix (const double dMtrx[16])
{
  short iz, is;

  for (iz = 0; iz < 4; iz++)
    for (is = 0; is < 4; is++)
      dMtrx4D[iz][is] = dMtrx[ iz + 4*is ];
}

unsigned long Matrix4D::GetMemSpace (void)
{
  return sizeof(Matrix4D);
}

void Matrix4D::Print (void) const
{
  short i;

  for (i = 0; i < 4; i++)
    printf("%9.3f %9.3f %9.3f %9.3f\n", dMtrx4D[i][0], dMtrx4D[i][1], dMtrx4D[i][2], dMtrx4D[i][3]);
}

void Matrix4D::Transpose (void)
{
  double  dNew[4][4];

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
      dNew[j][i] = dMtrx4D[i][j];
  }

  memcpy(dMtrx4D, dNew, sizeof(dMtrx4D));
}
