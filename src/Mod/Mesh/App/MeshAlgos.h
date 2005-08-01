/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


#ifndef _MeshAlgos_h_
#define _MeshAlgos_h_

#ifndef _PreComp_
# include <gts.h>
# include <vector>
#endif

#include <Base/Vector3D.h>
using Base::Vector3D;

#include "CurveProjector.h"

class TopoDS_Edge;
class TopoDS_Shape;

namespace Mesh
{

class MeshWithProperty;
class MeshKernel;

/** The mesh algorithems container class
 */
class AppMeshExport MeshAlgos
{
public:
  /** Load a Mesh
   */
  static MeshWithProperty* Load(const char *FileName);

  /** write a Mesh as binery STL
   */
  static void writeBin(MeshWithProperty* Mesh,const char *FileName);

  /** write a Mesh as ASCII STL
   */
  static void writeAscii(MeshWithProperty* Mesh,const char *FileName);

  /** reads a Mesh format if supported
   */
  static void read(MeshWithProperty* Mesh,const char *FileName);

  /** Calculate per Vertex normales and adds the Normal property bag
  */
  static void calcVertexNormales(MeshWithProperty* Mesh);

  /** Calculate per vertex curvate and adds the curvature property bag
  */
  static void calcVertexCurvature(MeshWithProperty* Mesh);

  /** Calculate per face curvate and adds the curvature property bag
  */
  static void calcFaceCurvature(MeshWithProperty* Mesh);

  /** Calculate per Vertex normales and adds the Normal property bag
  */
  static void offset(MeshWithProperty* Mesh, float fSize);

  /** Coarsen the mesh
  */
  static void MeshAlgos::coarsen(MeshWithProperty* Mesh, float f);

  /** makes a boolean add
   * The int Type stears the boolean oberation: 0=add;1=intersection;2=diff
  */
  static MeshWithProperty* MeshAlgos::boolean(MeshWithProperty* Mesh1, MeshWithProperty* Mesh2, MeshWithProperty* pResult, int Type=0);

  /** Creates a GTS Surface from a MeshKernel
  */
  static GtsSurface* createGTSSurface(MeshWithProperty* Mesh);

  /** Creates a GTS Surface from a MeshKernel
  */
  static void MeshAlgos::fillMeshFromGTSSurface(MeshWithProperty* pMesh, GtsSurface* pSurface);

  static void cutByShape(const TopoDS_Shape &aShape,const MeshWithProperty* pMesh,MeshWithProperty* pToolMesh);

  /// helper to discredicice a Edge...
  static void MeshAlgos::GetSampledCurves( const TopoDS_Edge& aEdge, std::vector<Vector3D>& rclPoints, unsigned long ulNbOfPoints = 30);

  /*
  struct FaceSplitEdge
  {
    unsigned long ulFaceIndex;
    Vector3D p1,p2;
  };

  static void projectCurve( MeshWithProperty* pMesh,
                                       const TopoDS_Edge& aEdge,
                                       const std::vector<Vector3D> &rclPoints, 
                                       std::vector<FaceSplitEdge> &vSplitEdges);
*/
  static void cutByCurve(MeshWithProperty* pMesh,const std::vector<CurveProjector::FaceSplitEdge> &vSplitEdges);
/*
  static bool projectPointToMesh(MeshKernel &MeshK,const Vector3D &Pnt,Vector3D &Rslt,unsigned long &FaceIndex);
*/


};



} // namespace Mesh
#endif 
