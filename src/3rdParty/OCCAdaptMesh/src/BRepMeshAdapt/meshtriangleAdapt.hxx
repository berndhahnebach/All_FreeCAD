// File:	meshtriangleAdapt.hxx
// Created:	Tue Jun 11 17:40:33 1996
// Author:	Open CASCADE Support
//		<support@opencascade.com>
// Copyright:    Open CASCADE 1996, 2003

#ifndef _meshtriangleAdapt_HeaderFile
#define _meshtriangleAdapt_HeaderFile

#include <TColgp_Array1OfPnt2d.hxx>
#include <TriangleAdapt_Parameters.h>
#include <TColStd_HArray1OfInteger.hxx>
#include <Poly_Triangulation.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <MeshShape_DataMapOfIntegerPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>


Standard_Boolean BRepMeshAdapt_AddPoint(const gp_Pnt2d& aPoint,
                                   const Standard_Integer anIndex);

void BRepMeshAdapt_AddEdge(const TColStd_Array1OfInteger& aNodes,
                      const TopAbs_Orientation&      anOrient,
                      const Standard_Boolean isFirst = Standard_True);

void BRepMeshAdapt_InsertHoles(const TColgp_SequenceOfPnt2d& aHoles);

Standard_Boolean BRepMeshAdapt_Triangulate(const TColgp_Array1OfPnt2d& aNodes,
                                      const TColStd_Array1OfInteger& anIndices,
									  const TriangleAdapt_Parameters& parameters);

Handle(Poly_Triangulation) BRepMeshAdapt_FinishTriangulation(Handle(TColStd_HArray1OfInteger)& anIndices,
                                                        const TopAbs_Orientation          anOrient);

Handle(Poly_Triangulation) BRepMeshAdapt_GetTriangulation(Handle(TColStd_HArray1OfInteger)& anIndices,
                                                     const TopAbs_Orientation          anOrient);

void BRepMeshAdapt_FreeTriangulation();

#endif // _meshtriangleAdapt_HeaderFile
