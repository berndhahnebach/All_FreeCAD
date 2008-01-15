// File:	BRepMeshAdapt_EdgeDiscret.cxx
// Created:	Thu Jan  7 17:20:40 1999
// Author:	Laurent PAINNOT
//		<lpa@penox.paris1.matra-dtv.fr>


#include <BRepMeshAdapt_EdgeDiscret.ixx>
#include <BRepMeshAdapt_ListIteratorOfListOfSurfaceGrid.hxx>
#include <BRepMeshAdapt_SurfaceGrid.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopoDS_Face.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Precision.hxx>


//=======================================================================
//function : BRepMeshAdapt_EdgeDiscret
//purpose  : 
//=======================================================================

BRepMeshAdapt_EdgeDiscret::BRepMeshAdapt_EdgeDiscret()
{
}


//=======================================================================
//function : BRepMeshAdapt_EdgeDiscret
//purpose  : 
//=======================================================================

BRepMeshAdapt_EdgeDiscret::BRepMeshAdapt_EdgeDiscret(const TopoDS_Edge&                edge,
					   const BRepMeshAdapt_ListOfSurfaceGrid& L,
					   const Standard_Real               def,
					   const Standard_Real               ang)
{
  myparameters.Clear();
  BRepMeshAdapt_ListIteratorOfListOfSurfaceGrid It;
  Standard_Real first, last;
  Standard_Real tolconf = Precision::Confusion();
  Standard_Real tol = tolconf;
  Standard_Integer i, j;
  Standard_Real Vmin, Vmax, Umin, Umax, U;
  TColStd_SequenceOfReal TheParameters;
  

  BRepAdaptor_Curve cons;
  cons.Initialize(edge);

  for (It.Initialize(L); It.More(); It.Next()) {
    const Handle(BRepMeshAdapt_SurfaceGrid)& SG = It.Value();
    const TopoDS_Face& face = SG->Face();
    if (!BRep_Tool::SameParameter(edge)) {
      cons.Initialize(edge, face);
    }
    else {
      cons.Initialize(edge);
    }
    break;
  }

  Standard_Integer nbpmin = 2;
  first = cons.FirstParameter();
  last = cons.LastParameter();
  GCPnts_TangentialDeflection GT(cons, first, last, ang/2., def, nbpmin);

  for (Standard_Integer pp = 1; pp <= GT.NbPoints(); pp++) {
    TheParameters.Append(GT.Parameter(pp));
  }


  for (It.Initialize(L); It.More(); It.Next()) {
    const Handle(BRepMeshAdapt_SurfaceGrid)& SG = It.Value();
    SG->Bounds(Umin, Umax, Vmin, Vmax);
    Umin = Umin - 0.1*(Umax-Umin);
    Umax = Umax + 0.1*(Umax-Umin);
    Vmin = Vmin - 0.1*(Vmax-Vmin);
    Vmax = Vmax + 0.1*(Vmax-Vmin);
    const TColStd_SequenceOfReal& UParam = SG->UParameters();
    const TColStd_SequenceOfReal& VParam = SG->VParameters();
    const TopoDS_Face& face = SG->Face();

    // Intersections de la pcurve de l'edge avec le reseau d'isos.
    Handle(Geom2d_Curve) pc = BRep_Tool::CurveOnSurface(edge, face, first, last);
    Geom2dAdaptor_Curve GC(pc, first, last), GL;
    Handle(Geom2d_Line) L;
    gp_Dir2d DU(0, 1);
    gp_Pnt2d P;
    P.SetY(0);

    // Reseau d'isos U
    for (i = 1; i <= UParam.Length(); i++) {
      P.SetX(UParam.Value(i));
      L = new Geom2d_Line(P, DU);
      GL.Load(L);
      Geom2dInt_GInter Inter(GC, GL, tolconf, tol);
      
      if (Inter.IsDone()) {
	if (!Inter.IsEmpty()) {
	  for (j = 1; j <= Inter.NbPoints(); j++) {
	    const IntRes2d_IntersectionPoint& IP = Inter.Point(j);
#ifdef DEB
	    const gp_Pnt2d& Pi = 
#endif
	      IP.Value();
	    U = IP.ParamOnFirst();
	    TheParameters.Append(U);
	  }
	}
      }
    }

    // Reseau d'isos V
    P.SetX(0);
    gp_Dir2d DV(1, 0);
    for (i = 1; i <= VParam.Length(); i++) {
      P.SetY(VParam.Value(i));
      L = new Geom2d_Line(P, DV);
      GL.Load(L);
      Geom2dInt_GInter Inter(GC, GL, tolconf, tol);
      
      if (Inter.IsDone()) {
	if (!Inter.IsEmpty()) {
	  for (j = 1; j <= Inter.NbPoints(); j++) {
	    const IntRes2d_IntersectionPoint& IP = Inter.Point(j);
#ifdef DEB
	    const gp_Pnt2d& Pi = 
#endif
	      IP.Value();
	    U = IP.ParamOnFirst();
	    TheParameters.Append(U);
	  }
	}
      }
    }
  }


  if (TheParameters.Length() >= 1) {
    SortParameters(TheParameters);
  }

}


//=======================================================================
//function : SortParameters
//purpose  : 
//=======================================================================

void BRepMeshAdapt_EdgeDiscret::SortParameters(const TColStd_SequenceOfReal& Param)
{
  Standard_Integer i, j;
  Standard_Real U, tol = 1.e-2;
  TColStd_SequenceOfReal P1;
  P1.Append(Param(1));

  for (i = 2; i <= Param.Length(); i++) {
    U = Param.Value(i);
    Standard_Boolean ajout = Standard_True;

    for (j = 1; j <= P1.Length(); j++) {
      if (Abs((U-P1.Value(j))) < tol) {
	ajout = Standard_False;
	break;
      }
    }
    if (ajout) {
      P1.Append(U);
    }
  }
#ifndef DEB
  Standard_Integer index =0;
#else
  Standard_Integer index;
#endif
  while (!P1.IsEmpty()) {
    Standard_Real CC = 1.e308;
    for (i = 1; i <= P1.Length(); i++) {
      if (P1.Value(i) < CC) {
	CC = P1.Value(i);
	index = i;
      }
    }
    P1.Remove(index);
    myparameters.Append(CC);
  }
/*
  cout << endl;
  cout << "==> au final :"<< endl;
  for (i = 1; i <= myparameters.Length(); i++) {
    U = myparameters.Value(i);
    cout <<"parametre "<<i<<" = "<< U << endl;
  }
  cout << endl;
*/
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

const TColStd_SequenceOfReal& BRepMeshAdapt_EdgeDiscret::Parameters() const
{
  return myparameters;
}

