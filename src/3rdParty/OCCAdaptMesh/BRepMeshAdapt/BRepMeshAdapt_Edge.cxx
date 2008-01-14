// File:	BRepMeshAdapt_Edge.cxx
// Created:	Thu Sep 23 13:39:58 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>
  
#include <BRepMeshAdapt_Edge.ixx>
  
BRepMeshAdapt_Edge::BRepMeshAdapt_Edge(const Standard_Integer vDebut,
			     const Standard_Integer vFin,
			     const MeshDS_DegreeOfFreedom canMove,
			     const Standard_Integer domain)
     : myFirstNode(vDebut), myLastNode(vFin), myMovability(canMove),
       myDomain(domain)
{}

void  BRepMeshAdapt_Edge::SetMovability(const MeshDS_DegreeOfFreedom Move)
{
  myMovability =Move;
}

Standard_Integer  BRepMeshAdapt_Edge::HashCode(const Standard_Integer Upper)const 
{
  return ::HashCode(myFirstNode+myLastNode, Upper);
}

Standard_Boolean  BRepMeshAdapt_Edge::IsEqual(const BRepMeshAdapt_Edge& Other)const 
{
  if (myMovability==MeshDS_Deleted || Other.myMovability==MeshDS_Deleted)
    return Standard_False;
  return (myFirstNode==Other.myFirstNode && myLastNode==Other.myLastNode) ||
    (myFirstNode==Other.myLastNode && myLastNode==Other.myFirstNode);
}


Standard_Boolean  BRepMeshAdapt_Edge::SameOrientation
  (const BRepMeshAdapt_Edge& Other)const 
{
  return (myFirstNode==Other.myFirstNode && myLastNode==Other.myLastNode);
}
