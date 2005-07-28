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


#ifndef _CurveProjector_h_
#define _CurveProjector_h_

#ifndef _PreComp_
# include <gts.h>
#endif

#include "Core/Vector3D.h"


class TopoDS_Edge;
class TopoDS_Shape;

namespace Mesh
{

class MeshWithProperty;
class MeshKernel;

/** The mesh algorithems container class
 */
class AppMeshExport CurveProjector
{
public:

  void cutByShape(const TopoDS_Shape &aShape, MeshWithProperty* pMesh); 

  /// helper to discredicice a Edge...
  void GetSampledCurves( const TopoDS_Edge& aEdge, std::vector<Vector3D>& rclPoints, unsigned long ulNbOfPoints = 30);

  struct FaceSplitEdge
  {
    unsigned long ulFaceIndex;
    Vector3D p1,p2;
  };

  void projectCurve( MeshWithProperty* pMesh,
                                       const TopoDS_Edge& aEdge,
                                       const std::vector<Vector3D> &rclPoints, 
                                       std::vector<FaceSplitEdge> &vSplitEdges);

  bool projectPointToMesh(MeshKernel &MeshK,const Vector3D &Pnt,Vector3D &Rslt,unsigned long &FaceIndex);

protected:
  const TopoDS_Shape &aShape;

};



} // namespace Mesh
#endif 
