// File:	BRepMeshAdaptAdapt.cxx
// Created:	Wed Aug  7 10:19:24 1996
// Author:	Laurent PAINNOT
//		<lpa@penox.paris1.matra-dtv.fr>


#include <BRepMeshAdapt.ixx>
#include <BRepMeshAdapt_IncrementalMesh.hxx>
#include <TopoDS_Shape.hxx>


void BRepMeshAdapt::Mesh(const TopoDS_Shape& S,
		    const Standard_Real d)
{
  BRepMeshAdapt_IncrementalMesh M(S, d);

}


void BRepMeshAdapt::Mesh(const TopoDS_Shape& S,
						 const Standard_Real d,
						 const TriangleAdapt_Parameters& params)
{
   BRepMeshAdapt_IncrementalMesh M(S, d, params);
}

