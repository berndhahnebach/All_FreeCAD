// File:	MeshAlgoAdapt_CircleTool.cxx
// Created:	Tue Jun 15 19:10:25 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>

#include <MeshAlgoAdapt_CircleTool.ixx>
#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <Precision.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <MeshAlgoAdapt_Circ.hxx>
#include <MeshAlgoAdapt_DataMapIteratorOfDataMapOfIntegerCirc.hxx>

const gp_XY SortingDirection(M_SQRT1_2, M_SQRT1_2);

Standard_Boolean chronoTriangulation=Standard_False;

Standard_Integer MeshAlgoAdapt_debug=0;

//=======================================================================
//function : MeshAlgoAdapt_CircleTool
//purpose  : 
//=======================================================================
MeshAlgoAdapt_CircleTool::MeshAlgoAdapt_CircleTool()
     : Tolerance(Precision::PConfusion()), isValid(Standard_False)
{
  Tolerance=Tolerance*Tolerance;
}

//=======================================================================
//function : MeshAlgoAdapt_CircleTool
//purpose  : 
//=======================================================================
MeshAlgoAdapt_CircleTool::MeshAlgoAdapt_CircleTool(const Standard_Integer nbComp)
     : Tolerance(Precision::PConfusion()),Components(nbComp*2), 
       isValid(Standard_False)
{
  Tolerance=Tolerance*Tolerance;
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void MeshAlgoAdapt_CircleTool::Initialize(const Standard_Integer nbComp)
{
  Components.Clear();
  Components.ReSize(nbComp*2);
  SearchList.Clear();
  Tolerance=Precision::PConfusion();
  Tolerance=Tolerance*Tolerance;
  isValid=Standard_False;
}


//=======================================================================
//function : Valid
//purpose  : 
//=======================================================================
void  MeshAlgoAdapt_CircleTool::Valid()
{
  SearchList.Clear();
  isValid=Standard_True;
}

//=======================================================================
//function : unValid
//purpose  : 
//=======================================================================
void  MeshAlgoAdapt_CircleTool::unValid()
{
  SearchList.Clear();
  MeshAlgoAdapt_DataMapIteratorOfDataMapOfIntegerCirc circs(Components);
  for (; circs.More(); circs.Next()) 
    SearchList.Add(circs.Key());
  isValid=Standard_False;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void  MeshAlgoAdapt_CircleTool::Add(const gp_Circ2d& theCirc,
			       const Standard_Integer theIndex)
{
  Components.Bind(theIndex, MeshAlgoAdapt_Circ(theCirc.Location().Coord(), 
					  theCirc.Radius()));
  if (!isValid) SearchList.Add(theIndex);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
Standard_Boolean MeshAlgoAdapt_CircleTool::Add(const gp_XY& p1,
					  const gp_XY& p2,
					  const gp_XY& p3,
					  const Standard_Integer theIndex)
{
  gp_XY m1((p1.X()+p2.X())/2., (p1.Y()+p2.Y())/2.);
  gp_XY m2((p2.X()+p3.X())/2., (p2.Y()+p3.Y())/2.);
  gp_XY m3((p3.X()+p1.X())/2., (p3.Y()+p1.Y())/2.);
  Standard_Real dx=m1.X()-m2.X();
  Standard_Real dy=m1.Y()-m2.Y();
  Standard_Real d12=(dx*dx)+(dy*dy);
  dx=m2.X()-m3.X();
  dy=m2.Y()-m3.Y();
  Standard_Real d23=(dx*dx)+(dy*dy);
  dx=m3.X()-m1.X();
  dy=m3.Y()-m1.Y();
  Standard_Real d31=(dx*dx)+(dy*dy);
  gp_Lin2d L1, L2;

  if (d12>d23 && d12>d31) {
    dy=p2.Y()-p1.Y();
    dx=p1.X()-p2.X();
    if (dy!=0. || dx!=0.) {
      L1.SetLocation(m1);
      L1.SetDirection(gp_Dir2d(dy, dx));
    }
    else return Standard_False;

    dy=p3.Y()-p2.Y();
    dx=p2.X()-p3.X();
    if (dy!=0. || dx!=0.) {
      L2.SetLocation(m2);
      L2.SetDirection(gp_Dir2d(dy, dx));
    }
    else return Standard_False;
  }
  else {
    if (d23>d31) {
      dy=p3.Y()-p2.Y();
      dx=p2.X()-p3.X();
      if (dy!=0. || dx!=0.) {
	L1.SetLocation(m2);
	L1.SetDirection(gp_Dir2d(dy, dx));
      }
      else return Standard_False;

      dy=p1.Y()-p3.Y();
      dx=p3.X()-p1.X();
      if (dy!=0. || dx!=0.) {
	L2.SetLocation(m3);
	L2.SetDirection(gp_Dir2d(dy, dx));
      }
      else return Standard_False;
    }
    else {
      dy=p1.Y()-p3.Y();
      dx=p3.X()-p1.X();
      if (dy!=0. || dx!=0.) {
	L1.SetLocation(m3);
	L1.SetDirection(gp_Dir2d(dy, dx));
      }
      else return Standard_False;

      dy=p2.Y()-p1.Y();
      dx=p1.X()-p2.X();
      if (dy!=0. || dx!=0.) {
	L2.SetLocation(m1);
	L2.SetDirection(gp_Dir2d(dy, dx));
      }
      else return Standard_False;
    }
  }

  IntAna2d_AnaIntersection Intp(L1,L2);
  if (Intp.IsDone()) {
    if (!Intp.IsEmpty()) {
      const gp_XY& pInt = Intp.Point(1).Value().Coord();
      dx=p1.X()-pInt.X();
      dy=p1.Y()-pInt.Y();
      Components.Bind(theIndex, MeshAlgoAdapt_Circ(pInt, Sqrt(dx*dx+dy*dy)));
      if (!isValid) SearchList.Add(theIndex);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void  MeshAlgoAdapt_CircleTool::Delete(const Standard_Integer theIndex)
{
  Components.UnBind(theIndex);
  if (!isValid) SearchList.Remove(theIndex);
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
TColStd_ListOfInteger&  MeshAlgoAdapt_CircleTool::Select(const gp_XY& thePnt)
{
  Standard_Real dx,dy;
  lastSelect.Clear();
  
  if (isValid) {
    MeshAlgoAdapt_DataMapIteratorOfDataMapOfIntegerCirc circs(Components);
    for (; circs.More(); circs.Next()) {
      const MeshAlgoAdapt_Circ& refCi=circs.Value();
      dx=thePnt.X()-refCi.Location().X();
      dy=thePnt.Y()-refCi.Location().Y();
      if ((dx*dx+dy*dy)-(refCi.Radius()*refCi.Radius()) <= Tolerance)
	lastSelect.Append(circs.Key());
    }
  }
  else {
    TColStd_MapIteratorOfMapOfInteger circ(SearchList);
    TColStd_ListOfInteger unReachable;
    for (; circ.More(); circ.Next()) {
      MeshAlgoAdapt_Circ& refCi=Components.ChangeFind(circ.Key());
      dx=thePnt.X()-refCi.Location().X();
      dy=thePnt.Y()-refCi.Location().Y();
      if ((dx*dx+dy*dy)-(refCi.Radius()*refCi.Radius()) <= Tolerance)
	lastSelect.Append(circ.Key());
      else if (((thePnt*SortingDirection)-(refCi.Location()*SortingDirection)) 
	       > refCi.Radius()) {
	unReachable.Append(circ.Key());
      }
    }
    for (; !unReachable.IsEmpty() ; unReachable.RemoveFirst())
      SearchList.Remove(unReachable.First());
  }
  return lastSelect;
}
