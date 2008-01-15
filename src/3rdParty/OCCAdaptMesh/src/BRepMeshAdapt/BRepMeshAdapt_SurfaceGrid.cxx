// File:	BRepMeshAdapt_SurfaceGrid.cxx
// Created:	Wed Jan  6 11:17:53 1999
// Author:	Laurent PAINNOT
//		<lpa@penox.paris1.matra-dtv.fr>


#include <BRepMeshAdapt_SurfaceGrid.ixx>

#include <ElSLib.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <Geom_Curve.hxx>



//=======================================================================
//function : BRepMeshAdapt_SurfaceGrid
//purpose  : 
//=======================================================================

BRepMeshAdapt_SurfaceGrid::BRepMeshAdapt_SurfaceGrid(const TopoDS_Face&  F,
					   const Standard_Real defface,
					   const Standard_Real angle):
					   myDeflection(defface),
					   myangle(angle),
					   myEquilateral(Standard_False),
					   myIsDimensionSet(Standard_False),
					   myState(BRepMeshAdapt_NoError)
{
  myFace = F;
  BRepAdaptor_Surface S(F);
  myumin = S.FirstUParameter();
  myumax = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvmax = S.LastVParameter();
  mySurface = new BRepAdaptor_HSurface(S);
  SetDelta();
  Perform();
}

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================

void BRepMeshAdapt_SurfaceGrid::SetDelta()
{
  GeomAbs_SurfaceType thetype = mySurface->GetType();
  mydeltaX = 1.;
  mydeltaY = 1.;
  
  // essai de determination de la longueur vraie:
  Standard_Real  longu = 0.0, longv = 0.0;//first, last;
  Standard_Integer i;
  gp_Pnt P1, P2;
  
  Standard_Real du = (myumax-myumin)/20;
  Standard_Real isov = (myvmin+myvmax)*0.5;
  mySurface->D0(myumin, isov, P1);
  for (i = 0; i <= 20; i++) {
    mySurface->D0(myumin + i*du, isov, P2);
    longu += P1.Distance(P2);
    P1 = P2;
  }
  Standard_Real dv = (myvmax-myvmin)/20;
  Standard_Real isou = (myumin+myumax)*0.5;
  mySurface->D0(isou, myvmin, P1);
  for (i = 0; i <= 20; i++) {
    mySurface->D0(isou, myvmin + i*dv, P2);
    longv += P1.Distance(P2);
	P1 = P2;
  }
  
  if (longu <= 1.e-16 || longv <= 1.e-16) {   //sisi, ca s'est vu!!
    // error
    myState = BRepMeshAdapt_Failure;
  }


  if (thetype == GeomAbs_Torus) {
    gp_Torus T = mySurface->Torus();
    Standard_Real r = T.MinorRadius(), R = T.MajorRadius();
    Standard_Real Du, Dv;//, pasu, pasv;
    
    Dv = Max(1.0e0 - (myDeflection/r),0.0e0) ;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    oldDv = Min(oldDv, myangle);
    Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
    Dv = oldDv;
//#ifndef DEB
    Standard_Integer nbV = (Standard_Integer) Max(2., (myvmax-myvmin)/Dv);
//#else
//    Standard_Integer nbV = Max(2., (myvmax-myvmin)/Dv);
//#endif
    Dv = (myvmax-myvmin)/(nbV+1);
    
    Standard_Real ru = R + r;
    if (ru > 1.e-16) {
      Du = Max(1.0e0 - (myDeflection/ru),0.0e0);
      Du  = (2.0 * ACos (Du));
      Du = Min(Du, myangle);
      Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
      Du = Du * Min(oldDv, Du) / aa;
    }
    else Du = Dv;     
//#ifndef DEB
    Standard_Integer nbU = (Standard_Integer) Max((myumax-myumin)/Du, 2.);
//#else    
//    Standard_Integer nbU = Max((myumax-myumin)/Du, 2.);
//#endif
    nbU = Max(nbU , (int)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
    
    Du = (myumax-myumin)/(nbU+1);
    //-- On choisit DeltaX et DeltaY de facon a ce qu on ne saute pas 
    //-- de points sur la grille
    mydeltaX = Dv;
    mydeltaY = Du;
    
    
  }
  else if (thetype == GeomAbs_Cylinder) {       
      //mydeltaX = 0.001;
      mydeltaX = 0.01;
      mydeltaY = 1.;
  }
  else {       
    mydeltaX = (myumax-myumin)/longu;
    mydeltaY = (myvmax-myvmin)/longv;
  }
    

}


//=======================================================================
//function : PerformGrid
//purpose  : 
//=======================================================================

void BRepMeshAdapt_SurfaceGrid::Perform()
{
  //gp_Pnt2d p2d;
  //gp_Pnt p3d;
  //Standard_Boolean OK;
  //Standard_Integer nbLocat = 0;

  // travail suivant le type de surface

  GeomAbs_SurfaceType thetype = mySurface->GetType();
  Standard_Real theangle = 0.35;

  if (thetype == GeomAbs_Plane) {
    // rajout d'un seul point au milieu.
    
//    Standard_Real U=(myumin+myumax)/2.;
//    Standard_Real V=(myvmin+myvmax)/2.;
//    myUParameters.Append(U);
//    myVParameters.Append(V);
/*
    p2d.SetCoord(U, V);
    OK =  (classifier.Perform(p2d) == TopAbs_IN);
    if (OK) {
      BS.D0(U, V, p3d);
      nbLocat++;
      myPoints.Append(p3d);
      myParameters.Append(p2d);
      myUParameters.Append(U);
      myVParameters.Append(V);
    }
*/
  }
  else if (thetype == GeomAbs_Sphere) {
    gp_Sphere S = mySurface->Sphere();
    Standard_Real R = S.Radius();

    Standard_Real ru, Du, Dv, pasu, pasv;
    Dv = Max(1.0e0 - (myDeflection/R),0.0e0) ;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    Dv  =  0.333333333333333 * oldDv;
//#ifndef DEB
    Standard_Integer nbV = (Standard_Integer) ( (myvmax-myvmin)/Dv );
//#else
//    Standard_Integer nbV = (myvmax-myvmin)/Dv;
//#endif
    Dv = (myvmax-myvmin)/(nbV+1);

    ru = R*cos(myvmax);
    if (ru > 1.e-16) {
      Du = Max(1.0e0 - (myDeflection/ru),0.0e0);
      Du  = (2.0 * ACos (Du));
      Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
      Du = Du * oldDv / aa;
    }
    else Du = Dv;
//#ifndef DEB
    Standard_Integer nbU = (Standard_Integer) ((myumax-myumin)/Du);
//#else
//    Standard_Integer nbU = (myumax-myumin)/Du;
//#endif
    Du = (myumax-myumin)/(nbU+1);

    Standard_Real pasvmax = myvmax-Dv*0.5, pasumax = myumax-Du*0.5;
    for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
      myUParameters.Append(pasu);
    }
    for (pasv = myvmin + Dv; pasv < pasvmax; pasv += Dv) {
      myVParameters.Append(pasv);
    }
/*
      for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
	p2d.SetCoord(pasu, pasv);
	OK =  (classifier.Perform(p2d) == TopAbs_IN);
	if (OK) {
	  ElSLib::D0(pasu, pasv, S, p3d);
	  nbLocat++;
	  myPoints.Append(p3d);
	  myParameters.Append(p2d);
	}
      }
*/
  }
  else if (thetype == GeomAbs_Cylinder) {
    gp_Cylinder S = mySurface->Cylinder();
    Standard_Real R = S.Radius();

    Standard_Real Du, Dv, pasu, pasv;
    Du = Max(1.0e0 - (myDeflection/R),0.0e0);
    Du  = (2.0 * ACos (Du));
    Du = Min(Du, myangle);
//#ifndef DEB
    Standard_Integer nbU = (Standard_Integer) ( (myumax-myumin)/Du );
    Standard_Integer nbV = (Standard_Integer) ( nbU*(myvmax-myvmin)/((myumax-myumin)*R) );
//#else
//    Standard_Integer nbU = (myumax-myumin)/Du;
//    Standard_Integer nbV = nbU*(myvmax-myvmin)/((myumax-myumin)*R);
//#endif
    nbV = Min(nbV, 100*nbU);
    Du = (myumax-myumin)/(nbU+1);
    Dv = (myvmax-myvmin)/(nbV+1);

    Standard_Real pasvmax = myvmax-Dv*0.5, pasumax = myumax-Du*0.5;
    for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
      myUParameters.Append(pasu);
    }
    for (pasv = myvmin + Dv; pasv < pasvmax; pasv += Dv) {
      myVParameters.Append(pasv);
    }
/*
      for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
	p2d.SetCoord(pasu, pasv);
	OK = (classifier.Perform(p2d) == TopAbs_IN);
	if (OK) {
	  ElSLib::D0(pasu, pasv, S, p3d);
          nbLocat++;
	  myPoints.Append(p3d);
	  myParameters.Append(p2d);
	}
      }
*/
  }
  else if (thetype == GeomAbs_Cone) {
    Standard_Real R, RefR, SAng;
    gp_Cone C = mySurface->Cone();
    RefR = C.RefRadius();
    SAng = C.SemiAngle();
    R = Max(Abs(RefR+myvmin*Sin(SAng)), Abs(RefR+myvmax*Sin(SAng)));
    Standard_Real Du, Dv, pasu, pasv;
    Du = Max(1.0e0 - (myDeflection/R),0.0e0);
    Du  = (2.0 * ACos (Du));
//#ifndef DEB
    Standard_Integer nbU = (Standard_Integer) ( (myumax-myumin)/Du );
    Standard_Integer nbV = (Standard_Integer) ( nbU*(myvmax-myvmin)/((myumax-myumin)*R) );
//#else
//    Standard_Integer nbU = (myumax-myumin)/Du;
//    Standard_Integer nbV = nbU*(myvmax-myvmin)/((myumax-myumin)*R);
//#endif
    Du = (myumax-myumin)/(nbU+1);
    Dv = (myvmax-myvmin)/(nbV+1);

    Standard_Real pasvmax = myvmax-Dv*0.5, pasumax = myumax-Du*0.5;
    
    for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
      myUParameters.Append(pasu);
    }
    for (pasv = myvmin + Dv; pasv < pasvmax; pasv += Dv) {
      myVParameters.Append(pasv);
    }

/*      for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
	p2d.SetCoord(pasu, pasv);
	OK = (classifier.Perform(p2d) == TopAbs_IN);
	if (OK) {
	  ElSLib::D0(pasu, pasv, C, p3d);
          nbLocat++;
	  myPoints.Append(p3d);
	  myParameters.Append(p2d);
	}
      }
*/
  }
  else if (thetype == GeomAbs_Torus) {
    gp_Torus T = mySurface->Torus();
    Standard_Integer i;//, j;
    //Standard_Real U1, U2, V1, V2;
    //Standard_Real pp, pasu, pasv;


//    Standard_Real dd = 0.01*(myumax-myumin);
    //Standard_Boolean insert;

    //    dd=(myumax-myumin)/10000.0;

    //Standard_Integer ParamULength, ParamVLength;
    
    Standard_Real r = T.MinorRadius(), R = T.MajorRadius();
    Standard_Real Du, Dv;
    
    Dv = Max(1.0e0 - (myDeflection/r),0.0e0) ;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    oldDv = Min(oldDv, myangle);
    Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
    Dv = oldDv;
//#ifndef DEB
    Standard_Integer nbV = (Standard_Integer) Max(2., (myvmax-myvmin)/Dv);
//#else
//    Standard_Integer nbV = Max(2., (myvmax-myvmin)/Dv);
//#endif
    Dv = (myvmax-myvmin)/(nbV+1);
    
    Standard_Real ru = R + r;
    if (ru > 1.e-16) {
      Du = Max(1.0e0 - (myDeflection/ru),0.0e0);
      Du  = (2.0 * ACos (Du));
      Du = Min(Du, myangle);
      Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
      Du = Du * Min(oldDv, Du) / aa;
    }
    else Du = Dv;     
    
//#ifndef DEB    
    Standard_Integer nbU = (Standard_Integer) Max((myumax-myumin)/Du, 2.);
//#else
//    Standard_Integer nbU = Max((myumax-myumin)/Du, 2.);
//#endif
    nbU = Max(nbU , (int)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
    
    Du = (myumax-myumin)/(nbU+1);
    //-- On choisit DeltaX et DeltaY de facon a ce qu on ne saute pas 
    //-- de points sur la grille
    
    for (i = 0; i <= nbU; i++) {
      myUParameters.Append(myumin + i* Du);
    }
    for (i = 0; i <= nbV; i++) {
      myVParameters.Append(myvmin + i* Dv);
    }
/*    
    Standard_Integer Lu = myUParameters.Length(), Lv = myVParameters.Length();
    Standard_Real umin = myumin+mydeltaY*0.1;
    Standard_Real vmin = myvmin+mydeltaX*0.1;
    Standard_Real umax = myumax-mydeltaY*0.1;
    Standard_Real vmax = myvmax-mydeltaX*0.1;
    
    for (i = 1; i <= Lu; i++) {
      pasu = myUParameters.Value(i);
      if (pasu >= umin && pasu < umax) {
	for (j = 1; j <= Lv; j++) {
	  pasv = myVParameters.Value(j);
	  if (pasv >= vmin && pasv < vmax) {
	    p2d.SetCoord(pasu, pasv);
	    OK = (classifier.Perform(p2d) == TopAbs_IN);
	    if (OK) {
	      ElSLib::D0(pasu, pasv, T, p3d);
	      nbLocat++;
	      myPoints.Append(p3d);
	      myParameters.Append(p2d);
	    }
	  }
	}
      }
    }
*/
  }
  else if (thetype == GeomAbs_BezierSurface || 
	   thetype == GeomAbs_BSplineSurface) {


    const BRepAdaptor_Surface& BS = *(BRepAdaptor_Surface*)&(mySurface->Surface());

    Standard_Integer i, j, nbpointsU = 10, nbpointsV = 10;
    Handle(Geom_Surface) B;


    if (thetype == GeomAbs_BezierSurface) {
      B = BS.Bezier();
      nbpointsU = (*((Handle(Geom_BezierSurface)*)&B))->UDegree();
      nbpointsV = (*((Handle(Geom_BezierSurface)*)&B))->VDegree();
    }
    else {
      B = BS.BSpline();
      nbpointsU = (*((Handle(Geom_BSplineSurface)*)&B))->UDegree();
      nbpointsV = (*((Handle(Geom_BSplineSurface)*)&B))->VDegree();
    }

    TColStd_SequenceOfReal ParamU, ParamV;
    Standard_Real u, v, du, dv;
    Standard_Integer iu, iv;
    //Standard_Real u1, v1, u2, v2;
    Standard_Real f, l;
    Adaptor3d_IsoCurve tabu[10], tabv[10];


    //Standard_Real pp, pasu, pasv;
    Standard_Real U1, V1, U2, V2, VControl;
    gp_Pnt P1, P2, PControl;

    //Standard_Real dd = 0.02*(myumax-myumin);
    //Standard_Boolean insert;
    Standard_Integer ParamULength=2, ParamVLength=2;
    
    du = (myumax-myumin) / (nbpointsU+1); 
    dv = (myvmax-myvmin) / (nbpointsV+1);

    for (iu = 1; iu <= nbpointsU; iu++) {
      u = myumin + iu*du;
      tabu[iu-1].Load(mySurface);
      tabu[iu-1].Load(GeomAbs_IsoU, u);
    }

    for (iv = 1; iv <= nbpointsV; iv++) {
      v = myvmin + iv*dv;
      tabv[iv-1].Load(mySurface);
      tabv[iv-1].Load(GeomAbs_IsoV, v);
    }
    // gp_Pnt P;
    //gp_Pnt2d P2d;

    Standard_Integer imax = 1, MaxV = 0;
    
    GCPnts_TangentialDeflection* tabGU = new GCPnts_TangentialDeflection[nbpointsU];

    for (i = 0; i <= nbpointsU-1; i++) {
      f = Max(myvmin, tabu[i].FirstParameter());
      l = Min(myvmax, tabu[i].LastParameter());
      GCPnts_TangentialDeflection thedeflection(tabu[i], f, l, theangle, 0.7*myDeflection, 2);
      tabGU[i] = thedeflection;
      if (tabGU[i].NbPoints() > MaxV) {
	MaxV = tabGU[i].NbPoints();
	imax = i;
      }
    }
    
    // recuperation du tableau de parametres V:
    Standard_Integer NV = tabGU[imax].NbPoints();
    for (i = 1; i <= NV; i++) {
      ParamV.Append(tabGU[imax].Parameter(i));
    }
    delete [] tabGU;

    imax = 1;
    Standard_Integer MaxU = 0;

    GCPnts_TangentialDeflection* tabGV = new GCPnts_TangentialDeflection[nbpointsV];

    for (i = 0; i <= nbpointsV-1; i++) {
      f = Max(myumin, tabv[i].FirstParameter());
      l = Min(myumax, tabv[i].LastParameter());
      GCPnts_TangentialDeflection thedeflection2(tabv[i], f, l, theangle, 0.7*myDeflection, 2);
      tabGV[i] = thedeflection2;
      if (tabGV[i].NbPoints() > MaxU) {
	MaxU = tabGV[i].NbPoints();
	imax = i;
      }
    }
    
    // recuperation du tableau de parametres U:
    Standard_Integer NU = tabGV[imax].NbPoints();
    for (i = 1; i <= NU; i++) {
      ParamU.Append(tabGV[imax].Parameter(i));
    }
    delete [] tabGV;
    
    if (ParamU.Length() == 2) {
      ParamU.InsertAfter(1, (myumax+myumin)*0.5);
    }
    if (ParamV.Length() == 2) {
      ParamV.InsertAfter(1, (myvmax+myvmin)*0.5);
    }
    

    Adaptor3d_IsoCurve IsoV;
    IsoV.Load(mySurface);

    // controle des isos U et insertion eventuelle:

    TColGeom_SequenceOfCurve tabIsoU;

    for (i = 1; i <= ParamULength; i++) {
      u = ParamU.Value(i);
      Handle(Geom_Curve) IsoU = B->UIso(u);
      tabIsoU.Append(IsoU);
      V1 = ParamV.Value(1);
      P1 = IsoU->Value(V1);

      for (j = 2; j <= ParamVLength; j++) {
	V2 = ParamV.Value(j);
	P2 = IsoU->Value(V2);
	VControl = (V1+V2)/2.;
	PControl = IsoU->Value(VControl);
	if (P1.SquareDistance(P2) > 1.e-5) {
	  gp_Lin L (P1, gp_Dir(gp_Vec(P1, P2)));
	  if (L.Distance(PControl) > myDeflection) {
	    // insertion 
	    ParamV.InsertBefore(j, VControl);
	    ParamVLength++;
	    j--;
	  }
	  else {
	    V1 = V2;
	    P1 = P2;
	  }
	}
	else {
	  V1 = V2;
	  P1 = P2;
	}
      }
    }


    Standard_Real UControl;
    TColGeom_SequenceOfCurve tabIsoV; //(1, ParamV.Length());

    for (i = 1; i <= ParamVLength; i++) {
      v = ParamV.Value(i);
      Handle(Geom_Curve) IsoV = B->VIso(v);
      tabIsoV.Append(IsoV);
      U1 = ParamU.Value(1);
      P1 = IsoV->Value(U1);
      for (j = 2; j <= ParamULength; j++) {
	U2 = ParamU.Value(j);
	P2 = IsoV->Value(U2);
	UControl = (U1+U2)/2.;
	PControl = IsoV->Value(UControl);
	if (P1.SquareDistance(P2) > 1.e-5) {
	  gp_Lin L (P1, gp_Dir(gp_Vec(P1, P2)));
	  if (L.Distance(PControl) > myDeflection) {
	    // insertion 
	    ParamU.InsertBefore(j, UControl);
	    ParamULength++;
	    Handle(Geom_Curve) iso = B->UIso(UControl);
	    tabIsoU.InsertBefore(j, iso);
	    j--;
	  }
	  else {
	    U1 = U2;
	    P1 = P2;
	  }
	}
	else {
	  U1 = U2;
	  P1 = P2;
	}
      }
    }

    ParamULength = ParamU.Length();
    ParamVLength = ParamV.Length();    
    for (j = 1; j <= ParamULength; j++) {
      myUParameters.Append(ParamU.Value(j));
    }
    for (j = 1; j <= ParamVLength; j++) {
      myVParameters.Append(ParamV.Value(j));
    }

/*

    gp_Pnt P3, P4, PMID;
    gp_XYZ vecEd1, vecEd2, equa;
    Standard_Real dipo, def1, def2;
    Standard_Real utri, vtri;
    Standard_Integer k;
    Handle(Geom_Curve) IsoV1, IsoV2, IsoU1, IsoU2, IsoVControl;

    for (i = 2; i < ParamVLength; i++) {
      v = ParamV.Value(i);
      myVParameters.Append(v);
      const Handle(Geom_Curve)& IsoV = tabIsoV(i);
      for (j = 2; j < ParamULength; j++) {
	u = ParamU.Value(j);
	p2d.SetCoord(u, v);
	OK = (classifier.Perform(p2d) == TopAbs_IN);
	if (OK) {
	  p3d = IsoV->Value(u);
	  nbLocat++;
	  myPoints.Append(p3d);
	  myParameters.Append(p2d);
	}
      }
    } 
*/
  }


  else {
#ifdef DEB
    const BRepAdaptor_Surface& BS = *(BRepAdaptor_Surface*)&
#endif
                                                            (mySurface->Surface());
    Standard_Integer i, j, nbpointsU = 10, nbpointsV = 10;
    Adaptor3d_IsoCurve tabu[10], tabv[10];

    TColStd_SequenceOfReal ParamU, ParamV;
    Standard_Real u, v, du, dv;
    Standard_Integer iu, iv;
    //Standard_Real u1, v1, u2, v2;
    Standard_Real f, l;


    du = (myumax-myumin) / (nbpointsU+1); dv = (myvmax-myvmin) / (nbpointsV+1);


    for (iu = 1; iu <= nbpointsU; iu++) {
      u = myumin + iu*du;
      tabu[iu-1].Load(mySurface);
      tabu[iu-1].Load(GeomAbs_IsoU, u);
    }

    for (iv = 1; iv <= nbpointsV; iv++) {
      v = myvmin + iv*dv;
      tabv[iv-1].Load(mySurface);
      tabv[iv-1].Load(GeomAbs_IsoV, v);
    }
    //gp_Pnt P;
    //gp_Pnt2d P2d;

    Standard_Integer imax = 1, MaxV = 0;
    
    GCPnts_TangentialDeflection* tabGU = new GCPnts_TangentialDeflection[nbpointsU];

    for (i = 0; i <= nbpointsU-1; i++) {
      f = Max(myvmin, tabu[i].FirstParameter());
      l = Min(myvmax, tabu[i].LastParameter());
      GCPnts_TangentialDeflection thedeflection3(tabu[i], f, l, theangle, 0.7*myDeflection, 2);
      tabGU[i] = thedeflection3; 
      if (tabGU[i].NbPoints() > MaxV) {
	MaxV = tabGU[i].NbPoints();
	imax = i;
      }
    }
    
    // recuperation du tableau de parametres V:
    Standard_Integer NV = tabGU[imax].NbPoints();
    for (i = 1; i <= NV; i++) {
      ParamV.Append(tabGU[imax].Parameter(i));
    }
    delete [] tabGU;

    imax = 1;
    Standard_Integer MaxU = 0;

    GCPnts_TangentialDeflection* tabGV = new GCPnts_TangentialDeflection[nbpointsV];

    for (i = 0; i <= nbpointsV-1; i++) {
      f = Max(myumin, tabv[i].FirstParameter());
      l = Min(myumax, tabv[i].LastParameter());
      GCPnts_TangentialDeflection thedeflection4(tabv[i], f, l, theangle, 0.7*myDeflection, 2);
      tabGV[i] = thedeflection4;
      if (tabGV[i].NbPoints() > MaxU) {
	MaxU = tabGV[i].NbPoints();
	imax = i;
      }
    }
    
    // recuperation du tableau de parametres U:
    Standard_Integer NU = tabGV[imax].NbPoints();
    for (i = 1; i <= NU; i++) {
      ParamU.Append(tabGV[imax].Parameter(i));
    }
    delete [] tabGV;
    
    if (ParamU.Length() == 2) {
      ParamU.InsertAfter(1, (myumax+myumin)*0.5);
    }
    if (ParamV.Length() == 2) {
      ParamV.InsertAfter(1, (myvmax+myvmin)*0.5);
    }
    
    //gp_Pnt P1, P2, PControl;
    //Standard_Real U1, U2, V1, V2, VControl;

    Adaptor3d_IsoCurve IsoV;
    IsoV.Load(mySurface);

    Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
    
    for (j = 2; j < Lu; j++) {
      myUParameters.Append(ParamU.Value(j));
    }
    for (i = 2; i < Lv; i++) {
      v = ParamV.Value(i);
      myVParameters.Append(v);
    }
/*
      IsoV.Load(GeomAbs_IsoV, v);
      for (j = 2; j < Lu; j++) {
	u = ParamU.Value(j);
	p2d.SetCoord(u, v);
	OK = (classifier.Perform(p2d) == TopAbs_IN);
	if (OK) {
	  p3d = IsoV.Value(u);
	  nbLocat++;
	  myPoints.Append(p3d);
	  myParameters.Append(p2d);
	}
      }
    } 
*/
  }



}
					   
//=======================================================================
//function : SetEquilateral
//purpose  : 
//=======================================================================

void BRepMeshAdapt_SurfaceGrid::SetEquilateral()
{
  myEquilateral=Standard_True;
}



//=======================================================================
//function : SetEdgeDimension
//purpose  : 
//=======================================================================

void BRepMeshAdapt_SurfaceGrid::SetEdgeDimension(const Standard_Real D)
{
  myDimension = D;
}

//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

const TColgp_SequenceOfPnt& BRepMeshAdapt_SurfaceGrid::Points() const
{
  return myPoints;
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

const TColgp_SequenceOfPnt2d& BRepMeshAdapt_SurfaceGrid::Parameters() const
{
  return myParameters;
}


//=======================================================================
//function : DeltaX
//purpose  : 
//=======================================================================

Standard_Real BRepMeshAdapt_SurfaceGrid::DeltaX() const
{
  return mydeltaX;
}

//=======================================================================
//function : DeltaY
//purpose  : 
//=======================================================================

Standard_Real BRepMeshAdapt_SurfaceGrid::DeltaY() const
{
  return mydeltaY;
}



//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepMeshAdapt_Status BRepMeshAdapt_SurfaceGrid::Error() const
{
  return myState;
}

//=======================================================================
//function : UParameters
//purpose  : 
//=======================================================================

const TColStd_SequenceOfReal& BRepMeshAdapt_SurfaceGrid::UParameters() const
{
  return myUParameters;
}


//=======================================================================
//function : VParameters
//purpose  : 
//=======================================================================

const TColStd_SequenceOfReal& BRepMeshAdapt_SurfaceGrid::VParameters() const
{
  return myVParameters;
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

TopoDS_Face BRepMeshAdapt_SurfaceGrid::Face() const
{
  return myFace;
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(BRepAdaptor_HSurface) BRepMeshAdapt_SurfaceGrid::Surface() const
{
  return mySurface;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepMeshAdapt_SurfaceGrid::Perform(const BRepMeshAdapt_Classifier& classifier)
{
  myParameters.Clear();
  myPoints.Clear();

  Standard_Integer i, j;
  Standard_Boolean OK;
  Standard_Real pasu, pasv;
  Standard_Integer Lu = myUParameters.Length(), Lv = myVParameters.Length();
  Standard_Real umin = myumin+mydeltaY*0.1;
  Standard_Real vmin = myvmin+mydeltaX*0.1;
  Standard_Real umax = myumax-mydeltaY*0.1;
  Standard_Real vmax = myvmax-mydeltaX*0.1;
  gp_Pnt2d p2d;
  gp_Pnt p3d;
  
  for (i = 1; i <= Lu; i++) {
    pasu = myUParameters.Value(i);
    if (pasu >= umin && pasu < umax) {
      for (j = 1; j <= Lv; j++) {
	pasv = myVParameters.Value(j);
	if (pasv >= vmin && pasv < vmax) {
	  p2d.SetCoord(pasu, pasv);
	  OK = (classifier.Perform(p2d) == TopAbs_IN);
	  if (OK) {
	    mySurface->D0(pasu, pasv, p3d);
	    myPoints.Append(p3d);
	    myParameters.Append(p2d);
	  }
	}
      }
    }
  }

}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void BRepMeshAdapt_SurfaceGrid::Bounds(Standard_Real& Umin,
				  Standard_Real& Umax,
				  Standard_Real& Vmin,
				  Standard_Real& Vmax) const
{
  Umin = myumin;
  Umax = myumax;
  Vmin = myvmin;
  Vmax = myvmax;
}
