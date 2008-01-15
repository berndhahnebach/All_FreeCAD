// File:	BRepMeshAdapt_Vertex.cxx
// Created:	Thu Sep 23 12:46:51 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>

#include <BRepMeshAdapt_Vertex.ixx>
#include <Precision.hxx>


BRepMeshAdapt_Vertex::BRepMeshAdapt_Vertex()
     : myDomain(0), myLocation(0), myMovability(MeshDS_Free)
{}

BRepMeshAdapt_Vertex::BRepMeshAdapt_Vertex(const gp_XY& UV,
				 const Standard_Integer Dom,
				 const Standard_Integer Locat3d,
				 const MeshDS_DegreeOfFreedom Move)
     : myUV(UV), myDomain(Dom), myLocation(Locat3d), myMovability(Move)
{}

BRepMeshAdapt_Vertex::BRepMeshAdapt_Vertex(const Standard_Real U,
				 const Standard_Real V,
				 const Standard_Integer Dom,
				 const MeshDS_DegreeOfFreedom Move)
     : myUV(U, V), myDomain(Dom), myLocation(0), myMovability(Move)
{}

void  BRepMeshAdapt_Vertex::Initialize(const gp_XY& UV,
				  const Standard_Integer Dom,
				  const Standard_Integer Locat3d,
				  const MeshDS_DegreeOfFreedom Move)
{
  myUV=UV;
  myDomain=Dom;
  myLocation=Locat3d;
  myMovability=Move;
}

void  BRepMeshAdapt_Vertex::SetMovability(const MeshDS_DegreeOfFreedom Move)
{
  myMovability=Move;
}

//=======================================================================
//function : HashCode IsEqual 
//purpose  : Services for Map
//=======================================================================
Standard_Integer  BRepMeshAdapt_Vertex::HashCode(const Standard_Integer Upper)const
{
  // 20/02/02 akm (OCC179) return ::HashCode(myDomain*myLocation, Upper);
  return ::HashCode (Floor(1e5*myUV.X())*Floor(1e5*myUV.Y()), Upper);
}

Standard_Boolean  BRepMeshAdapt_Vertex::IsEqual(const BRepMeshAdapt_Vertex& Other)const
{
  if (myMovability!=MeshDS_Deleted && Other.myMovability!=MeshDS_Deleted &&
      myDomain==Other.myDomain) // 20/02/02 akm && myLocation==Other.myLocation)
    return (myUV.IsEqual(Other.myUV, Precision::PConfusion()));
  return Standard_False;
}

