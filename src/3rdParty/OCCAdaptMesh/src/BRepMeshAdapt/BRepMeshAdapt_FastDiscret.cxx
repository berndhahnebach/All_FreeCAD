// File:	BRepMeshAdapt_FastDiscret.cxx
// Created:	Tue Feb 27 16:39:53 1996
// Author:	Laurent PAINNOT
//		<lpa@nonox>


#include <BRepMeshAdapt_FastDiscret.ixx>
#include <BRepUtils.h>
#include <BRepMeshAdapt_Array1OfVertexOfDelaun.hxx>
#include <BRepMeshAdapt_ListIteratorOfListOfVertex.hxx>
#include <BRepMeshAdapt_GeomTool.hxx>
#include <BRepMeshAdapt_ListOfXY.hxx>
#include <BRepMeshAdapt_ListIteratorOfListOfXY.hxx>
#include <MeshShape_ListIteratorOfListOfSurfacePoint.hxx>
#include <MeshShape_CurvatureType.hxx>
#include <MeshShape_ListOfSurfacePoint.hxx>
#include <MeshShape_Couple.hxx>
#include <MeshShape_MapOfCouple.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopAbs.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfXY.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <Geom2d_Curve.hxx>

#include <MeshShape_DataMapOfShapeListOfTransient.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <MeshShape_DataMapIteratorOfDataMapOfShapeListOfTransient.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <BRepMeshAdapt_ShapeTool.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Sphere.hxx>
#include <gp_Circ.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <BRepMeshAdapt_DataMapOfCouplePnt.hxx>
#include <Standard_ErrorHandler.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <BRepMeshAdapt_Classifier.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <BRepBndLib.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_TVertex.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <TopExp_Explorer.hxx>

#include <meshtriangleAdapt.hxx>
#include <TriangleAdapt.hxx>

#include <meshchronoAdapt.hxx>

//#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333
//#define TWOTHIRD 0.666666666666666666666666666666666666666666666666666666666666

//--ofv:
static Standard_Real FUN_CalcAverageDUV(TColStd_Array1OfReal& P, const Standard_Integer PLen)
{
  Standard_Integer i, j, n = 0;
  Standard_Real p, result = 0.;

  for(i = 1; i <= PLen; i++)
  {
    // Sort
    for(j = i + 1; j <= PLen; j++)
    {
      if(P(i) > P(j))
      {
        p = P(i);
        P(i) = P(j);
        P(j) = p;
      }
    }
    // Accumulate
    if (i != 1)
    {
      p = Abs(P(i) - P(i-1));
      if(p > 1.e-7)
      {
        result += p;
        n++;
      }
    }
  }
  return (n? (result / (Standard_Real) n) : -1.);
}
//--ofv.

// #include <stdio.h>
// #include <stdlib.h>
// Standard_Integer DumpOfTriangulation(const char* filename,
// 				     const char* writemode,
// 				     const Handle(Poly_Triangulation)& T,
// 				     Standard_Integer uvnodes = 1,
// 				     Standard_Integer triangles = 1,
// 				     Standard_Integer nodes = 1)
// {
//   FILE * out = fopen(filename,writemode);
//   if(uvnodes == 1)
//     {
//       fprintf(out,"UV-Nodes:\n");
//       const TColgp_Array1OfPnt2d& Nodes = T->UVNodes();
//       for(Standard_Integer iNodes = 1; iNodes <= Nodes.Length(); iNodes++)
// 	{
// 	  const gp_Pnt2d & p2 = Nodes.Value(iNodes);
// 	  Standard_Real X = p2.X();
// 	  Standard_Real Y = p2.Y();
// 	  fprintf(out,"[%3d] - X: %lf  Y: %lf\n",iNodes,X,Y);
// 	}
//       fprintf(out,"\n");
//     }
//   if(triangles == 1)
//     {
//       fprintf(out,"Triangles:\n");
//       const Poly_Array1OfTriangle& Trgls = T->Triangles();
//       for(Standard_Integer iTrgls = 1; iTrgls <= Trgls.Length(); iTrgls++)
// 	{
// 	  const Poly_Triangle& Trgl = Trgls.Value(iTrgls);
// 	  Standard_Integer N1 = Trgl.Value(1);
// 	  Standard_Integer N2 = Trgl.Value(2);
// 	  Standard_Integer N3 = Trgl.Value(3);
// 	  fprintf(out,"[%3d] - %2d  %2d  %2d\n",iTrgls,N1,N2,N3);
// 	}
//       fprintf(out,"\n");
//     }
//   if(nodes == 1)
//     {
//       fprintf(out,"3D Nodes:\n");
//       const TColgp_Array1OfPnt& Nodes3d = T->Nodes();
//       for(Standard_Integer iNodes3d = 1; iNodes3d <= Nodes3d.Length(); iNodes3d++)
// 	{
// 	  const gp_Pnt& p = Nodes3d.Value(iNodes3d);
// 	  Standard_Real X = p.X();
// 	  Standard_Real Y = p.Y();
// 	  Standard_Real Z = p.Z();
// 	  fprintf(out,"[%3d] - X: %lf  Y: %lf  Z: %lf\n",iNodes3d,X,Y,Z);
// 	}
//       fprintf(out,"\n");
//     }
//   fclose(out);
//   return 1;
// }

// Standard_Integer DataToTriangulate(const char* filename,
// 				   const char* writemode,
// 				   const TColgp_Array1OfPnt2d& tab,
// 				   const TColStd_Array1OfInteger& ind)
// {
//   if(tab.Length() != ind.Length()) return 0;
//   FILE * out = fopen(filename,writemode);
//   fprintf(out,"Triangulation data:\n");
//   for(Standard_Integer index = 1; index <= tab.Length(); index++)
//     {
//       const gp_Pnt2d& p2d = tab.Value(index);
//       Standard_Real X = p2d.X();
//       Standard_Real Y = p2d.Y();
//       Standard_Integer ip2d = ind.Value(index);
//       fprintf(out,"[%3d] - X: %lf  Y: %lf\n",ip2d,X,Y);
//     }
//   fclose(out);
//   return 1;
// }


/*static*/ inline Standard_Real MaxFaceTol (const TopoDS_Face& theFace)
{
  Standard_Real T, TMax = BRep_Tool::Tolerance(theFace);
  TopExp_Explorer Ex;

  for (Ex.Init(theFace,TopAbs_EDGE); Ex.More(); Ex.Next())
  {
    T = BRep_Tool::Tolerance(TopoDS::Edge(Ex.Current()));
    if (T > TMax) TMax = T;
  }

  for (Ex.Init(theFace,TopAbs_VERTEX); Ex.More(); Ex.Next())
  {
    T = BRep_Tool::Tolerance(TopoDS::Vertex(Ex.Current()));
    if (T > TMax) TMax = T;
  }

  return TMax;
}


//=======================================================================
//function : BRepMeshAdapt_FastDiscret
//purpose  : 
//=======================================================================

BRepMeshAdapt_FastDiscret::BRepMeshAdapt_FastDiscret(const Standard_Real    defle,
					   const Standard_Real    angl,
					   const Bnd_Box&         Box,
					   const Standard_Boolean withShare,
//					   const Standard_Boolean inshape,
					   const Standard_Boolean ,
					   const Standard_Boolean relative,
//					   const Standard_Boolean shapetrigu):
					   const Standard_Boolean )
: myDeflection(defle), myAngle(angl), mySharedMode(withShare), myRelative(relative),
  myNbIterations(11), myNbDomains(0), myNbLocations(0), mydtotale(0.0)
{
  if (relative)
  {
    Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
    Box.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
    mydtotale = TXmax-TXmin;
    const Standard_Real dy = TYmax-TYmin;
    const Standard_Real dz = TZmax-TZmin;
    if (dy > mydtotale) mydtotale = dy;
    if (dz > mydtotale) mydtotale = dz;
  }
#ifdef DEB_MESH_CHRONO
  MESH_CHRONO_INIT();
#endif
  //mynottriangulated.Clear();
}


//=======================================================================
//function : BRepMeshAdapt_FastDiscret
//purpose  : 
//=======================================================================

BRepMeshAdapt_FastDiscret::BRepMeshAdapt_FastDiscret(const Standard_Real    defle,
					   const TopoDS_Shape&    shape,
					   const Bnd_Box&         Box,
					   const Standard_Real    angl,
					   const Standard_Boolean withShare,
//					   const Standard_Boolean inshape,
					   const Standard_Boolean ,
					   const Standard_Boolean relative,
//					   const Standard_Boolean shapetrigu):
					   const Standard_Boolean )
: myDeflection(defle), myAngle(angl), mySharedMode(withShare), myRelative(relative),
  myNbIterations(11), myNbDomains(0), myNbLocations(0), mydtotale(0.0)
{
  if (relative)
  {
    Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
    Box.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
    mydtotale = TXmax-TXmin;
    const Standard_Real dy = TYmax-TYmin;
    const Standard_Real dz = TZmax-TZmin;
    if (dy > mydtotale) mydtotale = dy;
    if (dz > mydtotale) mydtotale = dz;
  }
#ifdef DEB_MESH_CHRONO
  MESH_CHRONO_INIT();
#endif
  //mynottriangulated.Clear();

  Add(shape);
}


//=======================================================================
//function : Add(shape)
//purpose  : 
//=======================================================================

void BRepMeshAdapt_FastDiscret::Add(const TopoDS_Shape& shape)
{
  TopExp_Explorer ex(shape, TopAbs_FACE);
  for (; ex.More(); ex.Next())
  {
    Add(TopoDS::Face(ex.Current()));
  }
}


//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================

#define MESH_FAILURE(theface)        \
{                                    \
  myfacestate = BRepMeshAdapt_Failure;    \
  mynottriangulated.Append(theface); \
  return;                            \
}

void BRepMeshAdapt_FastDiscret::Add(const TopoDS_Face& theface)
{
  // Modified by mpv Dec 25 2006 OCC14376 - first launch this algo with normal precision
  static int anIncreasedPrecision = Standard_False;

  TopLoc_Location ll;
  Handle(Geom_Surface) SS = BRep_Tool::Surface(theface, ll);
  if (SS.IsNull()) return;

#ifdef DEB_MESH_CHRONO
  chTotal.Start();
#endif
  GeomAbs_SurfaceType thetype;
  

#ifndef DEB_MESH
  try
  {
    OCC_CATCH_SIGNALS
#endif
    TopoDS_Face face = theface;
    BRepTools::Update(face);
    face.Orientation(TopAbs_FORWARD);
    myNbDomains++;
    BRepAdaptor_Surface  BS(face, Standard_False);
    Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);
    thetype = BS.GetType();

#ifdef DEB_MESH_CHRONO
    MESH_CHRONO_TSTART(thetype);
#endif

    // Clear local data
    myvemap.Clear();
    mylocation2d.Clear();
    internaledges.Clear();

    // Clear structure for better performance
    structure.Clear();

    Standard_Integer i = 1;//, i1, i2, j, k;
    BRepTools_WireExplorer WireExplorer;
    TopExp_Explorer FaceExplorer, ex;

    Standard_Real defedge, defface;
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
    Standard_Integer nbEdge = 0;

    Standard_Real maxdef = 2.* MaxFaceTol(theface);

    if (myRelative) defface = 0.0;
    else defface = Max(myDeflection, maxdef);
    //else defface = myDeflection;
    
    Standard_Real cdef;//, cang;
    Standard_Real savangle = myAngle;
    
    nbEdge = 0;
    for (ex.Init(face, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
      nbEdge++;
      if (!mapdefle.IsBound(edge)) {
	if (myRelative) {
	  if (edges.IsBound(edge)) {
	    const TColStd_ListOfTransient& L = edges.Find(edge);
	    const Handle(Poly_PolygonOnTriangulation)& P = 
	      *(Handle(Poly_PolygonOnTriangulation)*)&(L.First());
	    defedge = P->Deflection();
	  }
	  else {
	    Bnd_Box B;
	    BRepBndLib::Add(edge, B);
	    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	    dx = aXmax-aXmin;
	    dy = aYmax-aYmin;
	    dz = aZmax-aZmin;
	    defedge = dx;
	    if (defedge < dy) defedge = dy;
	    if (defedge < dz) defedge = dz;
	    // ajustement par rapport a la taille totale:
	    cdef = mydtotale/(2*defedge);
	    if (cdef < 0.5) cdef = 0.5;
	    if (cdef > 2.) cdef = 2.;
	    defedge = cdef * defedge * myDeflection;
	    myAngle = savangle * cdef;

	  }
	  defface = defface + defedge;
	  defface = Max(maxdef, defface);
	}
	else defedge = myDeflection;

	defedge = Max(maxdef, defedge);
	defedge = Max(1.e-05 , defedge);
	mapdefle.Bind(edge, defedge);
      }
      else {
	defedge = mapdefle(edge);
	if (myRelative) {defface = defface + defedge; defface = Max(maxdef, defface);}
      }
      Add(edge, face, gFace, defedge);
      myAngle = savangle;
    }
    
    if (nbEdge == 0 || myvemap.Extent() < 3)
    {
#ifdef DEB_MESH_CHRONO
      MESH_CHRONO_TSTOP(thetype);
#endif
      MESH_FAILURE(theface);
    }
    
    if (myRelative)  defface = defface / nbEdge;
    else             defface = myDeflection;

    if (mySharedMode) defface = Max(maxdef, defface);

    TopLoc_Location l;
    Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(face, l);
    if (T.IsNull())
    {
#ifdef DEB_MESH_CHRONO
      chMaillEdges.Start();
#endif
      Standard_Real xCur, yCur;
      Standard_Real maxX, minX, maxY, minY;
      minX=minY=1.e100;
      maxX=maxY=-1.e100; 
      
      Standard_Integer nbVertices = myvemap.Extent();
      gp_XY p2d;
      BRepMeshAdapt_Vertex v;
#ifdef DEB_MESH_CHRONO
      D0Edges += nbVertices;
#endif

      // Define UV boundaries of face
      for (i = 1; i <= nbVertices; i++)
      {
	v = structure.FindKey(myvemap.FindKey(i));
	xCur=v.Coord().X();
	yCur=v.Coord().Y();
	minX=Min(xCur, minX);
	maxX=Max(xCur, maxX);
	minY=Min(yCur, minY);
	maxY=Max(yCur, maxY);
      }
      myumin = minX;
      myumax = maxX;
      myvmin = minY;
      myvmax = maxY;

      const Standard_Real umin = BS.FirstUParameter();
      const Standard_Real umax = BS.LastUParameter();
      const Standard_Real vmin = BS.FirstVParameter();
      const Standard_Real vmax = BS.LastVParameter();

      if (myumin < umin || myumax > umax)
      {
	if (BS.IsUPeriodic())
        {
	  if ((myumax - myumin) > (umax - umin))
          {
	    myumax = myumin + (umax - umin);
	  }
	}
        else
        {
          if (umin > myumin) myumin = umin;
          if (umax < myumax) myumax = umax;
	}
      }

      if (myvmin < vmin || myvmax > vmax)
      {
	if (BS.IsVPeriodic())
        {
	  if ((myvmax - myvmin) > (vmax - vmin))
          {
	    myvmax = myvmin + (vmax - vmin);
	  }
	}
        else
        {
          if (vmin > myvmin) myvmin = vmin;
          if (vmax < myvmax) myvmax = vmax;
	}
      }

      // verif rapide de la validite des bornes calculees. Si fausse,
      // surement un pb de pcurve.

      if (thetype == GeomAbs_BezierSurface &&
          (myumin < -0.5 || myumax > 1.5 || myvmin < -0.5 || myvmax > 1.5))
      {
#ifdef DEB_MESH_CHRONO
        cout <<"WARNING: face "<< myNbDomains <<" ( "<<(long unsigned) ((void*) &(*theface.TShape())) 
          <<" ) non triangulee, pcurves surement fausses."<<endl;
        chMaillEdges.Stop();
        MESH_CHRONO_TSTOP(thetype);
#endif
        MESH_FAILURE(theface);
      }

      const Standard_Boolean infini = (Precision::IsInfinite(myumin) ||
                                       Precision::IsInfinite(myumax) ||
                                       Precision::IsInfinite(myvmin) ||
                                       Precision::IsInfinite(myvmax));

      const Standard_Boolean isaline = ((myumax-myumin)<1.e-05 ||
                                        (myvmax-myvmin)<1.e-05);

      if (isaline || infini)
      {
#ifdef DEB_MESH_CHRONO
	chMaillEdges.Stop();
        MESH_CHRONO_TSTOP(thetype);
#endif
        MESH_FAILURE(theface);
      }

      deltaX = 1.0;
      deltaY = 1.0;

      const Standard_Real tolclass = Precision::PConfusion(); //0.03*Max(myumax-myumin, myvmax-myvmin);

	
      BRepMeshAdapt_Classifier* classifier = 
	new BRepMeshAdapt_Classifier(face, tolclass,	internaledges, myvemap, 
				structure, myumin, myumax, myvmin, myvmax, myTriangleParameters);

      myfacestate = classifier->State();
      
      if (myfacestate == BRepMeshAdapt_SelfIntersectingWire)
      {
	Standard_Integer nbmaill = 0;
	Standard_Real eps = Precision::Confusion();
	while (nbmaill < 5 && myfacestate != BRepMeshAdapt_ReMesh)
        {
	  nbmaill++;
	  myvemap.Clear();
	  mylocation2d.Clear();
	  internaledges.Clear();
	  
	  for (ex.Init(face, TopAbs_EDGE); ex.More(); ex.Next())
          {
	    const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
	    if (edges.IsBound(edge))
            {
	      edges.UnBind(edge);
	      internaledges.UnBind(edge);
	    }
	  }

	  for (ex.Init(face, TopAbs_EDGE); ex.More(); ex.Next())
          {
	    const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
	    defedge = mapdefle(edge) / 3.;
	    defedge = Max(defedge, eps);
	    mapdefle.Bind(edge, defedge);
	    Add(edge, face, gFace, defedge);
	  }
	  
	  delete classifier;

	  classifier = new BRepMeshAdapt_Classifier(face, tolclass, internaledges, myvemap,
					       structure, myumin, myumax, myvmin, myvmax,myTriangleParameters);

	  if (classifier->State() == BRepMeshAdapt_NoError)
          {
	    myfacestate = BRepMeshAdapt_ReMesh;
	  }
	  nbVertices = myvemap.Extent();
	}
      }
      
      if (myfacestate != BRepMeshAdapt_NoError && myfacestate != BRepMeshAdapt_ReMesh)
      {
#ifdef DEB_MESH_CHRONO
	chMaillEdges.Stop();
        MESH_CHRONO_TSTOP(thetype);
	if (myfacestate == BRepMeshAdapt_OpenWire ) {
	  cout <<"WARNING, face "<< myNbDomains <<" ( "<<(long unsigned)((void*) &(*theface.TShape()))
	    <<" ) : non triangulee, wire ouvert."<<endl;
	}
	else {
	  cout <<"WARNING, face "<< myNbDomains <<" ( "<<(long unsigned)((void*) &(*theface.TShape()))
	    <<" ) : non triangulee, chevauchement de wire."<<endl;
	}
#endif
	mynottriangulated.Append(theface);
	delete classifier;
	return;
      }
      
#ifdef DEB_MESH_CHRONO
      if (myfacestate == BRepMeshAdapt_ReMesh)
      {
	cout <<"WARNING, face "<< myNbDomains <<" ( "<<(long unsigned)((void*) &(*theface.TShape()))
	  <<" ) : chevauchement de wire rectifie."<<endl;
      }
#endif
      
      // essai de determination de la longueur vraie:
      // akm (bug OCC16) : We must calculate these measures in non-singular
      //     parts of face. Let's try to compute average value of three
      //     (umin, (umin+umax)/2, umax), and respectively for v.
      //                 vvvvv
      Standard_Real longu = 0.0, longv = 0.0; //, last , first;
      gp_Pnt P11, P12, P21, P22, P31, P32;
      
      Standard_Real du = (myumax-myumin)/20;
      Standard_Real dv = (myvmax-myvmin)/20;
      Standard_Real dfuave=(myumin+myumax)/2, dfucur;
      Standard_Real dfvave=(myvmin+myvmax)/2, dfvcur;
      // U loop
      BS.D0 (myumin, myvmin, P11);
      BS.D0 (myumin, dfvave, P21);
      BS.D0 (myumin, myvmax, P31);
      for (i=0, dfucur=myumin; i <= 20; i++, dfucur+=du)
      {
	BS.D0 (dfucur, myvmin, P12);
	BS.D0 (dfucur, dfvave, P22);
	BS.D0 (dfucur, myvmax, P32);
	longu += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
	P11 = P12;
	P21 = P22;
	P31 = P32;
      }
      // V loop
      BS.D0(myumin, myvmin, P11);
      BS.D0(dfuave, myvmin, P21);
      BS.D0(myumax, myvmin, P31);
      for (i=0, dfvcur=myvmin; i <= 20; i++, dfvcur+=dv)
      {
	BS.D0 (myumin, dfvcur, P12);
	BS.D0 (dfuave, dfvcur, P22);
	BS.D0 (myumax, dfvcur, P32);
	longv += ( P11.Distance(P12) + P21.Distance(P22) + P31.Distance(P32) );
	P11 = P12;
	P21 = P22;
	P31 = P32;
      }
      longu /= 3.;
      longv /= 3.;
      // akm (bug OCC16) ^^^^^
      
      if (longu <= 1.e-16 || longv <= 1.e-16)
      {   //sisi, ca s'est vu!!
#ifdef DEB_MESH_CHRONO
	chMaillEdges.Stop();
        MESH_CHRONO_TSTOP(thetype);
	cout <<"WARNING: la face "<<myNbDomains<<" n'est pas triangulee."<<endl;
#endif
	delete classifier;
        MESH_FAILURE(theface);
      }
      
      if (thetype == GeomAbs_Torus)
      {
	gp_Torus Tor = BS.Torus();
	Standard_Real r = Tor.MinorRadius(), R = Tor.MajorRadius();
	Standard_Real Du, Dv;//, pasu, pasv;
	
	Dv = Max(1.0e0 - (defface/r),0.0e0) ;
	Standard_Real oldDv = 2.0 * ACos (Dv);
	oldDv = Min(oldDv, myAngle);
	Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
	Dv = oldDv;

	Standard_Integer nbV = Max((Standard_Integer)((myvmax-myvmin)/Dv), 2);
	Dv = (myvmax-myvmin)/(nbV+1);
	
	Standard_Real ru = R + r;
	if (ru > 1.e-16)
        {
	  Du = Max(1.0e0 - (defface/ru),0.0e0);
	  Du  = (2.0 * ACos (Du));
	  Du = Min(Du, myAngle);
	  Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
	  Du = Du * Min(oldDv, Du) / aa;
	}
	else Du = Dv;     
	
	Standard_Integer nbU = Max((Standard_Integer)((myumax-myumin)/Du), 2);
	nbU = Max(nbU, (Standard_Integer)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
	
	Du = (myumax-myumin)/(nbU+1);
	//-- On choisit DeltaX et DeltaY de facon a ce qu on ne saute pas 
	//-- de points sur la grille
	deltaX = Du;
	deltaY = Dv;
      }
      else if (thetype == GeomAbs_Cylinder)
      {
        deltaX = 0.01;//0.001
	deltaY = 1.0;
      }
      else
      {
	deltaX = (myumax-myumin)/longu;
	deltaY = (myvmax-myvmin)/longv;
      }
    
      TColgp_Array1OfPnt2d tabP(1, nbVertices);
      TColStd_Array1OfInteger Ind(1, nbVertices);

      // Check the necessity to fill the map of parameters
      const Standard_Boolean useUVParam = (thetype == GeomAbs_Torus ||
                                           thetype == GeomAbs_BezierSurface ||
                                           thetype == GeomAbs_BSplineSurface);

      myUParam.Clear(); myVParam.Clear();

      for (i = 1; i <= nbVertices; i++)
      {
	v = structure.FindKey(myvemap.FindKey(i));
	p2d = v.Coord();
        if (useUVParam)
        {
          myUParam.Add(p2d.X());
          myVParam.Add(p2d.Y());
        }
	tabP(i).SetCoord((p2d.X()-minX)/deltaX,(p2d.Y()-minY)/deltaY);
	Ind(i) = v.Location3d();
      }
      
      // trouvons les trous.
      TColgp_SequenceOfPnt2d lpnt;
      FindHoles(theface,(thetype == GeomAbs_Plane),lpnt);

      const TColgp_SequenceOfPnt2d& holes = classifier->Holes();
      for (i = 1; i <= holes.Length(); i++)
      {
	gp_Pnt2d pph((holes(i).X()-myumin)/deltaX,
                     (holes(i).Y()-myvmin)/deltaY);
	lpnt.Append(pph);
      }
      
#ifdef DEB_MESH_CHRONO
      chMaillEdges.Stop();
      chEtuInter.Start();
#endif
      
      Standard_Boolean rajout;

      switch (thetype)
      {
        case GeomAbs_Plane:
          rajout = !classifier->NaturalRestriction();
          break;
        case GeomAbs_Sphere:
        case GeomAbs_Torus:
          rajout = Standard_True;
          break;
        default:
          rajout = Standard_False;
      }

      BRepMeshAdapt_ListOfVertex mylistver;
      Handle(Poly_Triangulation) Tr;

      if (!rajout)
      {
        // First pass - triangulate boundary nodes only
        Tr = Triangulate(gFace, defface, tabP, Ind, mylistver, lpnt, Standard_True, anIncreasedPrecision);
        rajout = Tr.IsNull();
      }

      if (rajout)
      {
        // Calculate internal nodes
        InternalVertices(gFace, mylistver, defface, *classifier);
        // Second pass - triangulate all nodes
        Tr = Triangulate(gFace, defface, tabP, Ind, mylistver, lpnt, Standard_False, anIncreasedPrecision);
      }

#ifdef DEB_MESH_CHRONO
      chEtuInter.Stop();
#endif

      delete classifier;

      if (Tr.IsNull() || (Tr->NbNodes() < myvemap.Extent()))
      {
#ifdef DEB_MESH_CHRONO
        MESH_CHRONO_TSTOP(thetype);
	cout <<"WARNING: la face "<<myNbDomains<<" n'est pas triangulee."<<endl;
#endif
        MESH_FAILURE(theface);
      }

#ifdef DEB_MESH_CHRONO
#ifdef DEB
      if (AffichTriangles)
      {
        Handle(Poly_Triangulation) TD;
        TD = new Poly_Triangulation(Tr->Nodes(), Tr->UVNodes(), Tr->Triangles());
        Handle(DrawTrSurf_Triangulation2D) DTD = new DrawTrSurf_Triangulation2D(TD);
        char* name = new char[100];
        sprintf(name, "tr_%i", myNbDomains);
        Draw::Set(name, DTD);
        delete name;
      }
#endif
      chStock.Start();
#endif

      // stockage de la triangulation dans la BRep.
      l = face.Location();
      if (!l.IsIdentity())
      {
        TColgp_Array1OfPnt& Nodes = Tr->ChangeNodes();
        gp_Trsf tr = l.Transformation();
        tr.Invert();
        for (i = Nodes.Lower(); i <= Nodes.Upper(); i++)
          Nodes(i).Transform(tr);
      }

      BRep_Builder B;
      B.UpdateFace(face, Tr);

      // mise en place des polygones sur triangulation dans la face:
      MeshShape_DataMapIteratorOfDataMapOfShapeListOfTransient It(internaledges);

      for (; It.More(); It.Next())
      {
        const Handle(Poly_PolygonOnTriangulation)& NOD1 = 
          *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().First()));
        if (It.Value().Extent() == 1)
        {
          B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, Tr, l);
        }
        else
        {
          const Handle(Poly_PolygonOnTriangulation)& NOD2 = 
            *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().Last()));
          B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, NOD2, Tr, l);
        }
      }
      
#ifdef DEB_MESH_CHRONO
      chStock.Stop();
#endif
    }

#ifdef DEB_MESH_CHRONO
    MESH_CHRONO_TSTOP(thetype);
#endif
    
#ifndef DEB_MESH
  }
  catch(Standard_Failure)
  {
    BRepMeshAdapt_FreeTriangulation();
#ifdef DEB_MESH_CHRONO
    MESH_CHRONO_TSTOP(thetype);
#endif
    // Modified by mpv Dec 25 2006 OCC14376 - increase precision: may be this slower algo will work
    if (!anIncreasedPrecision) {
      anIncreasedPrecision = Standard_True;
      Add(theface);
      anIncreasedPrecision = Standard_False;
      return;
    }
    MESH_FAILURE(theface);
  }
#endif // DEB_MESH
}


//=======================================================================
//function : FindHoles
//purpose  : 
//=======================================================================

void BRepMeshAdapt_FastDiscret::FindHoles(const TopoDS_Face& aFace,
                                     const Standard_Boolean isPlanar,
                                     TColgp_SequenceOfPnt2d& aHoles)
{
  // check if there is vertex shared by more than two edges.
  TopTools_IndexedDataMapOfShapeListOfShape ancestors;
  TopExp::MapShapesAndAncestors (aFace, TopAbs_VERTEX, TopAbs_EDGE, ancestors);
      
  TopTools_ListOfShape StrangeV; 
  TopTools_ListIteratorOfListOfShape Iter;
  Standard_Integer i;

  // for each vertices check the number of representation in UV space
  for (i = 1; i <= ancestors.Extent(); i++)
  {
    const TopTools_ListOfShape& aList = ancestors.FindFromIndex(i);
    if ( aList.Extent() < 3 ) continue;

    TopoDS_Vertex aVertex = TopoDS::Vertex(ancestors.FindKey(i));
    Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &aVertex.TShape());
    if (!TV->Points().Extent()) continue;

    // the vertex is shared by three or more edges and the number of representation is not null
    // we check the nature of the junction:
    // the vertex must be internal to one of edges and there must be at least 2 edges
    // (not seam and not degenerated) having this vertex at end
    Standard_Integer cint = 0, cpt = 0;
    for (Iter.Initialize(aList); Iter.More(); Iter.Next())
    {
      TopoDS_Edge aEdge = TopoDS::Edge(Iter.Value());
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(aEdge,V1,V2);
      if (aVertex.IsSame(V1) || aVertex.IsSame(V2))
      {
	// this vertex is the end of this edge
	if(!BRep_Tool::IsClosed(aEdge,aFace) && !BRep_Tool::Degenerated(aEdge))
	  // the edge is not a Seam and not Degenerated
	  cpt++;
      }
      else
      {
	// check that this vertex is internal to this edge
	Standard_Boolean isInt = Standard_False;
	TopoDS_Iterator it(aEdge);
	for (; it.More(); it.Next())
	{
	  if (aVertex.IsSame(it.Value()))
	  {
	    if (it.Value().Orientation() == TopAbs_INTERNAL)
	      isInt = Standard_True;
	    else
	    {
	      isInt = Standard_False;
	      break;
	    }
	  }
	}
	if (isInt)
	  cint++;
      }
    }
    if (cint > 0 && cpt >= 2)
    {
      // we found a strange vertex 
      StrangeV.Append(aVertex);
    }
  }

  // if there is strange verticies we must analyse wire 
  if (StrangeV.Extent() == 0) return;

  TopExp_Explorer VertexExplorer;
  TopTools_IndexedMapOfShape Edges;

  // for each strange vertex we check the wires
  for(Iter.Initialize(StrangeV); Iter.More(); Iter.Next())
  {
    TopoDS_Edge Ecurrent;
    TopoDS_Vertex V1, V2, Vcurrent, VS = TopoDS::Vertex(Iter.Value());
    TopTools_ListOfShape LEdges; 
    TopTools_ListIteratorOfListOfShape Iter2;
    Standard_Boolean IsClosed = Standard_False, IsFound = Standard_False;

    TopTools_IndexedDataMapOfShapeListOfShape ancestors2;
    TopExp::MapShapesAndAncestors (aFace, TopAbs_VERTEX, TopAbs_WIRE, ancestors2);
    TopoDS_Wire W1, W2;

    Vcurrent = VS;
    const TopTools_ListOfShape&  aListWire = ancestors2.FindFromKey(Vcurrent);

    // if there is already two wires we take them
    if ((aListWire.Extent() >= 2) &&
        (!(TopoDS::Wire(aListWire.First())).IsSame(TopoDS::Wire(aListWire.Last()))))
    {
      W1 = TopoDS::Wire(aListWire.First());
      W2 = TopoDS::Wire(aListWire.Last());
    }
    // else we must analyse the wire to find the hole
    else
    {
      W1 = TopoDS::Wire(aListWire.First());
      BRepLib_MakeWire MW;
      Standard_Integer cpt, nbE = 0;

      while(!IsFound)
      {
        const TopTools_ListOfShape& aList = ancestors.FindFromKey(Vcurrent);
        nbE = aList.Extent();
        cpt = 0;

        for(Iter2.Initialize(aList); Iter2.More(); Iter2.Next())
        {
          Ecurrent = TopoDS::Edge(Iter2.Value());
          if (Edges.Contains(Ecurrent)) cpt++;
          else
          {
            TopExp::Vertices(Ecurrent,V1,V2);
            const TopTools_ListOfShape& aListV1 = ancestors.FindFromKey(V1);
            const TopTools_ListOfShape& aListV2 = ancestors.FindFromKey(V2);

            Edges.Add(Ecurrent);
            if (aListV1.Extent()<=2 || aListV2.Extent()<=2)
            {
              // if the edges have two "normal" vertex we take it
              LEdges.Append(Ecurrent);
              if(V1.IsSame(Vcurrent)) { Vcurrent = V2; }
              else { Vcurrent = V1; }
              if(Vcurrent.IsSame(VS))
              {
                IsFound = Standard_True; 
              }
            }
            else
            {
              // else we clear LEdges and restart from the begining
              LEdges.Clear();
              Vcurrent = VS;
            }
            break;
          }
        }
        // we have explore all edges of the vertex
        if (cpt == nbE) IsFound = Standard_True;
      }
      // we building the two wires W1 and W2
      BRepTools_Substitution BSub;
      TopTools_ListOfShape EmptyList;

      for(Iter2.Initialize(LEdges); Iter2.More(); Iter2.Next())
      {
        MW.Add(TopoDS::Edge(Iter2.Value()));
        BSub.Substitute(TopoDS::Edge(Iter2.Value()),EmptyList);
      }

      BSub.Build(W1);

      if (BSub.IsCopied(W1))
      {
        W1 = TopoDS::Wire(BSub.Copy(W1).First());
        W2 = MW;

        TopoDS_Vertex Va,Vb;

        TopExp::Vertices(W2,V1,V2);
        TopExp::Vertices(W1,Va,Vb);
        if ((V1.IsSame(V2)) && (Va.IsSame(Vb))) IsClosed = Standard_True;
      }  
    }

    if (!(W1.IsNull()) && !(W2.IsNull()) && IsClosed)
    {
      BRepLib_MakeFace Face1(W1,isPlanar);
      TopoDS_Face F1 = Face1.Face();

      // we take a vertex on W2 which is not a strange vertex
      TopoDS_Vertex VertexOnW2;
      for(VertexExplorer.Init(W2,TopAbs_VERTEX); VertexExplorer.More(); VertexExplorer.Next())
      {
        VertexOnW2 = TopoDS::Vertex(VertexExplorer.Current());
        if (!VertexOnW2.IsSame(TopoDS::Vertex(Iter.Value()))) break;
      }

      // we classify the vertex 
      gp_Pnt2d point =  BRep_Tool::Parameters(VertexOnW2,aFace);

      BRepTopAdaptor_FClass2d classifieur(F1,0);
      const TopAbs_State etat = classifieur.Perform(point);

      TopoDS_Face FaceTrou;
      if (etat == TopAbs_IN) {
        // the wire 2 is a hole
        BRepLib_MakeFace Face2(W2,isPlanar);
        FaceTrou = Face2.Face();
      }  
      else if (etat == TopAbs_OUT) {
        // the wire 1 is a hole
        FaceTrou = F1;
      }
      if (!FaceTrou.IsNull())
      {
        // find a point in the hole  
        Standard_Real u1,u2,v1,v2;
        BRepTools::UVBounds(FaceTrou,u1,u2,v1,v2);
        const Standard_Real inc = (u2-u1)/3.0;

        BRepTopAdaptor_FClass2d clas(FaceTrou,0);

        gp_Pnt2d pnt(u1 + inc,0.5*(v1+v2));
        while (pnt.X() < u2)
        {
          const TopAbs_State etatpnt = clas.Perform(pnt);
          if (etatpnt == TopAbs_IN)
          {
            // point found 
            pnt.SetX((pnt.X()-myumin)/deltaX);
            pnt.SetY((pnt.Y()-myvmin)/deltaY);
            // append it in the list of holes for Triangle
            aHoles.Append(pnt);
            break;
          }
          pnt.SetX(pnt.X()+inc);
        }
      }
    }
  }	
}


//=======================================================================
//function : Triangulate
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) BRepMeshAdapt_FastDiscret::Triangulate
       (const Handle(BRepAdaptor_HSurface)& caro,
        const Standard_Real                 defface,
        const TColgp_Array1OfPnt2d&         tabP,
        const TColStd_Array1OfInteger&      tabI,
        const BRepMeshAdapt_ListOfVertex&        tabV,
        const TColgp_SequenceOfPnt2d&       tabH,
        const Standard_Boolean              isfirst,
	const Standard_Boolean              aIncreasePrecision)
{
  Handle(Poly_Triangulation) theMesh;

  Standard_Integer i, nbtotal = tabP.Length() + tabV.Extent();

  // Initialize Triangle data structures
  triangleAdapt_InitBehavior(myTriangleParameters._minAngle,myTriangleParameters._maxArea);
  if (aIncreasePrecision) triangleAdapt_IncreasePrecision();
  triangleAdapt_InitMesh(nbtotal);

  // Add boundary nodes
  for (i = tabP.Lower(); i <= tabP.Upper(); i++)
  {
    const gp_Pnt2d& pnt = tabP(i);
    triangleAdapt_AddPointIni(pnt.X(),pnt.Y(),tabI(i));
  }

  if (!isfirst)
  {
    // Add internal nodes
    BRepMeshAdapt_ListIteratorOfListOfVertex IT(tabV);
    for (; IT.More(); IT.Next())
    {
      const BRepMeshAdapt_Vertex& v = IT.Value();
      const gp_XY& pnt = v.Coord();
      triangleAdapt_AddPointIni(pnt.X(),pnt.Y(),v.Location3d());
    }
  }

  // Triangulate nodes
  if (!triangleAdapt_Triangulate()) return theMesh;

  // Add boundary links
  Standard_Boolean isFirstEdge = Standard_True;
  MeshShape_DataMapIteratorOfDataMapOfShapeListOfTransient It;
  for (It.Initialize(internaledges); It.More(); It.Next())
  {
    const Handle(Poly_PolygonOnTriangulation)& NOD1 =
      *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().First()));
    if (It.Value().Extent() == 1)
    {
      BRepMeshAdapt_AddEdge(NOD1->Nodes(), (TopoDS::Edge(It.Key())).Orientation(), isFirstEdge);
    }
    else
    {
      const Handle(Poly_PolygonOnTriangulation)& NOD2 =
        *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().Last()));
      BRepMeshAdapt_AddEdge(NOD1->Nodes(), TopAbs_FORWARD, isFirstEdge);
      BRepMeshAdapt_AddEdge(NOD2->Nodes(), TopAbs_REVERSED, Standard_False);
    }
    isFirstEdge = Standard_False;
  }

  // Add control points
  const Standard_Real curdef = Control(caro, defface, isfirst);

  if (!isfirst || (0.0 <= curdef && curdef <= defface))
  {
    if (!tabH.IsEmpty()) BRepMeshAdapt_InsertHoles(tabH);

    triangleAdapt_FinishTriangulation();

    // Read mesh size
    Standard_Integer nbnodes, nbtriangles;
    triangleAdapt_GetMeshSize(&nbnodes,&nbtriangles);
    if (nbnodes > 0 && nbtriangles > 0)
    {
      theMesh = new Poly_Triangulation(nbnodes, nbtriangles, Standard_True);

      // Read nodes information
      TColgp_Array1OfPnt& theNodes = theMesh->ChangeNodes();
      TColgp_Array1OfPnt2d& theNodes2d = theMesh->ChangeUVNodes();
      Standard_Real x, y;
      Standard_Integer pindex, pnumber = 1;
      triangleAdapt_InitPoints();
     
	  Standard_Integer type;
      while (triangleAdapt_NextPoint(pnumber,&x,&y,&pindex,&type))
      {
          theNodes2d(pnumber).SetCoord(x*deltaX + myumin, y*deltaY + myvmin);
          //theNodes(pnumber) = Location3d(pindex);
          gp_Pnt pnt3d;
          if ( type == 0 /*INPUTVERTEX */)
          {
			pnt3d = Location3d(pindex);
          }
          else
          {
            pnt3d = caro->Value(x*deltaX + myumin, y*deltaY + myvmin);
//          myNbLocations++;
//          Location3d.Bind(myNbLocations, pnt3d);
          }
          theNodes(pnumber) = pnt3d;
          pnumber++;
       }



  //    while (triangleAdapt_NextPoint(pnumber,&x,&y,&pindex))
  //    {
  //      theNodes2d(pnumber).SetCoord(x*deltaX + myumin, y*deltaY + myvmin);
  //      //changed for the new mesh algo
		////theNodes(pnumber) = Location3d(pindex);
		//gp_Pnt pnt3d = caro->Value(x*deltaX + myumin, y*deltaY + myvmin);
  //      theNodes(pnumber) = pnt3d;
  //      pnumber++;
  //    }

      // Read triangles information
      Poly_Array1OfTriangle& theTriangles = theMesh->ChangeTriangles();
      Standard_Integer p1, p2, p3, m1, m2, m3, tnumber = 1;
      triangleAdapt_InitTriangles();
      while (triangleAdapt_NextTriangle(&p1,&p2,&p3,&m1,&m2,&m3))
      {
        theTriangles(tnumber++) = Poly_Triangle(p1,p2,p3);
      }

      theMesh->Deflection((curdef < 0.0)? defface : curdef);
    }
  }

  BRepMeshAdapt_FreeTriangulation();

  return theMesh;
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepMeshAdapt_FastDiscret::Add(const TopoDS_Edge&                  edge, 
			       const TopoDS_Face&                  face, 
			       const Handle(BRepAdaptor_HSurface)& gFace,
			       const Standard_Real                 defedge)
{
  const TopAbs_Orientation orEdge = edge.Orientation();
  if (orEdge == TopAbs_EXTERNAL) return;

  Standard_Real ppf, ppl;
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(edge, face, ppf, ppl);
  
  if (C2d.IsNull()) return; //OCC316(APO)

  const Standard_Boolean isEdgeBound = edges.IsBound(edge);

#ifdef DEB_MESH_CHRONO
  chEdges.Start();
#endif

  if (!isEdgeBound)
  {
    if (Update(edge, face, gFace, defedge))
    {
#ifdef DEB_MESH_CHRONO
      chEdges.Stop();
#endif
      return;
    }
  }

  TopoDS_Vertex pBegin, pEnd;
  TopExp::Vertices(edge, pBegin, pEnd);
  if (pBegin.IsNull() || pEnd.IsNull())
  {
#ifdef DEB_MESH_CHRONO
    chEdges.Stop();
#endif
    return;
  }

  Standard_Real wFirst, wLast;
  BRep_Tool::Range(edge, face, wFirst, wLast);

  gp_Pnt2d uvFirst, uvLast;
  BRep_Tool::UVPoints(edge, face, uvFirst, uvLast);

  const Standard_Boolean sameUV = uvFirst.IsEqual(uvLast, Precision::PConfusion());

  //Controle vertice tolerances

  gp_Pnt pFirst = gFace->Value(uvFirst.X(), uvFirst.Y());
  gp_Pnt pLast = gFace->Value(uvLast.X(), uvLast.Y());

  Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
				    pLast.Distance(BRep_Tool::Pnt(pEnd)));

  if(mindist < BRep_Tool::Tolerance(pBegin) ||
     mindist < BRep_Tool::Tolerance(pEnd) ) mindist = defedge;

  // controle des edges degenerees non codees.

  Standard_Boolean degener = BRep_Tool::Degenerated(edge);

  if (!degener)
  {
    if (pBegin.IsSame(pEnd))
    {
      // calcul de la longueur de l'edge en 3d
      Standard_Real longueur = 0.0;
      Standard_Real du = (wLast-wFirst)/20;
      gp_Pnt P1, P2;
      BRepAdaptor_Curve BC(edge);
      BC.D0(wFirst, P1);
      Standard_Real tolV = BRep_Tool::Tolerance(pBegin);
      Standard_Real tolV2 = 1.2*tolV;
      for (Standard_Integer l = 1; l <= 20; l++) {
	BC.D0(wFirst + l*du, P2);
	longueur += P1.Distance(P2);
	if (longueur > tolV2) break;
	P1 = P2;
      }

      if (longueur < tolV2)
      {
#ifdef DEB_MESH_CHRONO
        cout <<"WARNING: edge "<<(long unsigned)((void*) &(*edge.TShape())) <<" potentiellement degeneree."<< endl;
        cout <<"tolerance du vertex: "<< tolV <<"  longueur de l'edge: "<<longueur <<endl;
#endif
	degener = Standard_True;
      }
    }
  }

  // Correct UV points
  if (sameUV)
  {
    // 1. est-ce vraiment sameUV sans etre denegere
    gp_Pnt2d uvF, uvL;
    C2d->D0(ppf, uvF);
    C2d->D0(ppl, uvL);
    if (!uvFirst.IsEqual(uvF, Precision::PConfusion()))
    {
      uvFirst = uvF;
#ifdef DEB_MESH_CHRONO
      cout <<"WARNING: incorrect edge on face "<< (long unsigned)((void*) &(*face.TShape()))<<endl;
#endif
    }
    if (!uvLast.IsEqual(uvL, Precision::PConfusion()))
    {
      uvLast = uvL;
#ifdef DEB_MESH_CHRONO
      cout <<"WARNING: incorrect edge on face "<< (long unsigned)((void*) &(*face.TShape()))<<endl;
#endif
    }
  }

#ifdef DEB_MESH_CHRONO
  chAdd11.Start();
#endif

  gp_XY theUV;

  // Process first vertex
  Standard_Integer ipf;
  if (vertices.IsBound(pBegin))
  {
    ipf = vertices.Find(pBegin);
  }
  else
  {
    if (sameUV && vertices.IsBound(pEnd))
    {
      ipf = vertices.Find(pEnd);
    }
    else
    {
      myNbLocations++;
      Location3d.Bind(myNbLocations, BRep_Tool::Pnt(pBegin));
      ipf = myNbLocations;
    }
    vertices.Bind(pBegin,ipf);
  }
  theUV = FindUV(pBegin, uvFirst, ipf, gFace, mindist);
  BRepMeshAdapt_Vertex vf(theUV, myNbDomains, ipf, MeshDS_Frontier);
  const Standard_Integer isvf = myvemap.Add(structure.Add(vf));

  // Process last vertex
  Standard_Integer ipl;
  if (pEnd.IsSame(pBegin))
  {
    ipl = ipf;
  }
  else
  {
    if (vertices.IsBound(pEnd))
    {
      ipl = vertices.Find(pEnd);
    }
    else
    {
      if (sameUV/* && vertices.IsBound(pBegin)*/)
      {
        ipl = ipf;
      }
      else
      {
        myNbLocations++;
        Location3d.Bind(myNbLocations, BRep_Tool::Pnt(pEnd));
        ipl = myNbLocations;
      }
      vertices.Bind(pEnd,ipl);
    }
  }
  theUV = FindUV(pEnd, uvLast, ipl, gFace, mindist);
  BRepMeshAdapt_Vertex vl(theUV, myNbDomains, ipl, MeshDS_Frontier);
  const Standard_Integer isvl = myvemap.Add(structure.Add(vl));

#ifdef DEB_MESH_CHRONO
  chAdd11.Stop();
#endif
  
  Standard_Real otherdefedge = 0.5*defedge;
  gp_Pnt2d uv;
  BRepMeshAdapt_Vertex v2;
  gp_Pnt   P3d;

  Handle(Poly_PolygonOnTriangulation) P1;

  if (!isEdgeBound)
  {
#ifdef DEB_MESH_CHRONO
    chAdd12.Start();
#endif
    Handle(Poly_PolygonOnTriangulation) P2;

    if (degener)
    {
      // creation du nouveau:
      TColStd_Array1OfInteger Nodes(1, 2), NodInStruct(1, 2);
      TColStd_Array1OfReal Param(1, 2);
      
      NodInStruct(1) = ipf;
      Nodes(1) = isvf;
      Param(1) = wFirst;
      
      NodInStruct(2) = ipl;
      Nodes(2) = isvl;
      Param(2) = wLast;

      P1 = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2 = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }
    else
    {
      if (orEdge == TopAbs_INTERNAL) otherdefedge *= 0.5;
      
      BRepAdaptor_Curve cons;
      if (BRep_Tool::SameParameter(edge))
      {
	cons.Initialize(edge);
      }
      else
      {
	cons.Initialize(edge, face);
      }

#ifdef DEB_MESH_CHRONO
      chUnif.Start();
#endif
      TopLoc_Location L;
      Standard_Integer nbpmin = 4;

	  if ( myTriangleParameters._minNbPntsPerEdgeLine > 0 && cons.GetType() == GeomAbs_Line )
      {
         nbpmin = myTriangleParameters._minNbPntsPerEdgeLine;
      }
      if ( myTriangleParameters._minNbPntsPerEdgeOther > 0 && cons.GetType() != GeomAbs_Line )
      {
         nbpmin = myTriangleParameters._minNbPntsPerEdgeOther;
      }
      //determine the nb of points on the edge
      if ( myTriangleParameters._minEdgeSplit > 0 )
      {
         nbpmin = std::max(myTriangleParameters._minEdgeSplit,nbpmin);
      }
      else if ( myTriangleParameters._maxTriangleSideSize > 0 )
      {
		//edge length :
		double edgeLength = BRepUtils::GetLength(edge);
		int val = (int)ceil(edgeLength / myTriangleParameters._maxTriangleSideSize);
		nbpmin = std::max(val,nbpmin);
      }

//    if (cons.GetType() != GeomAbs_Line || thetype != GeomAbs_Plane) nbpmin = 4;
      if (cons.GetType() == GeomAbs_Circle) nbpmin = 4; //OCC287
      BRepMeshAdapt_GeomTool GT(cons, wFirst, wLast, 0.5*myAngle, otherdefedge, nbpmin);
#ifdef DEB_MESH_CHRONO
      D0Unif += GT.NbPoints();
      chUnif.Stop();
#endif      
      // Creation des polygones sur triangulation:
      const Standard_Integer nbnodes = GT.NbPoints();
      TColStd_Array1OfInteger Nodes(1, nbnodes);
      TColStd_Array1OfInteger NodInStruct(1, nbnodes);
      TColStd_Array1OfReal Param(1, nbnodes);
      
      // traitement du 1er point:
      Nodes(1) = isvf;
      NodInStruct(1) = ipf;
      Param(1) = wFirst;
      
      // dernier point:
      Nodes(nbnodes) = isvl;
      NodInStruct(nbnodes) = ipl;
      Param(nbnodes) = wLast;

      Standard_Real puv;
      Standard_Integer i;
      for (i = 2; i < GT.NbPoints(); i++)
      {
        // Record 3d point
	GT.Value(cons, gFace, i, puv, P3d, uv);
	myNbLocations++;
	Location3d.Bind(myNbLocations, P3d);
	NodInStruct(i) = myNbLocations;
        // Record 2d point
	v2.Initialize(uv.Coord(), myNbDomains, myNbLocations, MeshDS_OnCurve);
	Nodes(i) = myvemap.Add(structure.Add(v2));
	Param(i) = puv;
      }

      P1 = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2 = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }

    P2->Deflection(otherdefedge);
    TColStd_ListOfTransient L;
    L.Append(P2);
    edges.Bind(edge, L);

#ifdef DEB_MESH_CHRONO
    chAdd12.Stop();
#endif
  }
  // Si on a dÝjÒ vu cette Edge et qu'elle n'est pas dÝgÝnÝrÝe on reprend
  // les points du polygone calculÝs Ò la premiÛre rencontre :
  else
  {
#ifdef DEB_MESH_CHRONO
    chAdd2.Start();
#endif
    if (degener)
    {
      // Create 2d polygon for degenerated edge
      TColStd_Array1OfInteger Nodes(1, 2);
      TColStd_Array1OfReal PPar(1, 2);
      
      Nodes(1) = isvf;
      PPar(1) = wFirst;
      
      Nodes(2) = isvl;
      PPar(2) = wLast;
      
      P1 = new Poly_PolygonOnTriangulation(Nodes, PPar);
    }
    else
    {
      // recuperation du polygone:
      const TColStd_ListOfTransient& L = edges.Find(edge);
      const Handle(Poly_PolygonOnTriangulation)& P = 
        *(Handle(Poly_PolygonOnTriangulation)*)&(L.First());
      const TColStd_Array1OfInteger& NOD = P->Nodes();
      Handle(TColStd_HArray1OfReal) Par = P->Parameters();

      // creation du nouveau:
      const Standard_Integer nbnodes = NOD.Length();
      TColStd_Array1OfInteger Nodes(1, nbnodes);
      TColStd_Array1OfReal PPar(1, nbnodes);
      
      Nodes(1) = isvf;
      PPar(1) = wFirst;
      
      Nodes(nbnodes) = isvl;
      PPar(nbnodes) = wLast;
      
      if (nbnodes > 2)
      {
        Standard_Integer i;
	if (BRep_Tool::SameParameter(edge))
        {
	  for (i = 2; i < nbnodes; i++)
          {
            const Standard_Real puv = Par->Value(i);
	    C2d->D0(puv, uv);
	    v2.Initialize(uv.Coord(), myNbDomains, NOD(i), MeshDS_OnCurve);
	    Nodes(i) = myvemap.Add(structure.Add(v2));
	    PPar(i) = puv;
	  }
	}
        else
        {
	  const Standard_Real wFold = Par->Value(Par->Lower());
	  const Standard_Real wLold = Par->Value(Par->Upper());

	  Standard_Real wKoef = 1.;
	  if ((wFold != wFirst || wLold != wLast) && wLold != wFold)
          {
	    wKoef = (wLast - wFirst) / (wLold - wFold);
	  }
	  
	  BRepAdaptor_Curve cons(edge, face);
	  Extrema_LocateExtPC pcos;
	  pcos.Initialize(cons, cons.FirstParameter(), 
			  cons.LastParameter(), Precision::PConfusion());

	  Standard_Real wPrev;
	  Standard_Real wCur      = wFirst;
	  Standard_Real wCurFound = wFirst;
	  for (i = 2; i < nbnodes; i++)
          {
	    P3d = Location3d(NOD(i));
            // Record 2d point
	    wPrev = wCur;
	    wCur  = wFirst + wKoef*(Par->Value(i) - wFold);
            wCurFound += (wCur - wPrev);
	    pcos.Perform(P3d, wCurFound);
	    if (pcos.IsDone()) wCurFound = pcos.Point().Parameter();
	    C2d->D0(wCurFound, uv);
	    v2.Initialize(uv.Coord(), myNbDomains, NOD(i), MeshDS_OnCurve);
	    Nodes(i) = myvemap.Add(structure.Add(v2));
	    PPar(i) = wCurFound;
	  }
	}
      }

      P1 = new Poly_PolygonOnTriangulation(Nodes, PPar);
    }
#ifdef DEB_MESH_CHRONO
    chAdd2.Stop();
#endif
  }

  P1->Deflection(defedge);
  if (internaledges.IsBound(edge))
  {
    TColStd_ListOfTransient& L = internaledges.ChangeFind(edge);
    if (orEdge == TopAbs_REVERSED)
      L.Append(P1);
    else
      L.Prepend(P1);
  }
  else
  {
    TColStd_ListOfTransient L1;
    L1.Append(P1);
    internaledges.Bind(edge, L1);
  }
#ifdef DEB_MESH_CHRONO
  chEdges.Stop();
#endif
}


//=======================================================================
//function : Update(edge)
//purpose  :
//=======================================================================

Standard_Boolean BRepMeshAdapt_FastDiscret::Update(const TopoDS_Edge&  edge,
					      const TopoDS_Face&  face,
					      const Handle(BRepAdaptor_HSurface)& gFace,
					      const Standard_Real defedge)
{
#ifdef DEB_MESH_CHRONO    
  chUpdate.Start();
#endif

  TopLoc_Location l;
  Handle(Poly_Triangulation) T, TNull;
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;

  Standard_Integer i = 1;
  Standard_Boolean found = Standard_False;
  do
  {
    BRep_Tool::PolygonOnTriangulation(edge,Poly,T,l,i);
    i++;
    if (!found && !T.IsNull() && T->HasUVNodes() && 
	!Poly.IsNull() && Poly->HasParameters())
    {
      if (Poly->Deflection() <= 1.1*defedge)
      {
        // Get range on 2d curve
	Standard_Real wFirst, wLast;
	BRep_Tool::Range(edge, face, wFirst, wLast);

        // Get end points on 2d curve
	gp_Pnt2d uvFirst, uvLast;
	BRep_Tool::UVPoints(edge, face, uvFirst, uvLast);

        // Get vertices
	TopoDS_Vertex pBegin, pEnd;
	TopExp::Vertices(edge,pBegin,pEnd);

	const Standard_Boolean sameUV =
          uvFirst.IsEqual(uvLast, Precision::PConfusion());

	//Controle vertice tolerances

	gp_Pnt pFirst = gFace->Value(uvFirst.X(), uvFirst.Y());
	gp_Pnt pLast  = gFace->Value(uvLast.X(), uvLast.Y());
	
	Standard_Real mindist = 10. * Max(pFirst.Distance(BRep_Tool::Pnt(pBegin)), 
					  pLast.Distance(BRep_Tool::Pnt(pEnd)));

	if (mindist < BRep_Tool::Tolerance(pBegin) ||
	    mindist < BRep_Tool::Tolerance(pEnd) ) mindist = defedge;

	if (sameUV)
        {
	  // 1. est-ce vraiment sameUV sans etre denegere
	  Standard_Real ppf, ppl;
	  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(edge, face, ppf, ppl);
	  gp_Pnt2d uvF, uvL;
	  C2d->D0(ppf, uvF);
	  C2d->D0(ppl, uvL);
	  if (!uvFirst.IsEqual(uvF, Precision::PConfusion())) {
	    uvFirst = uvF;
#ifdef DEB_MESH_CHRONO
	    cout <<"WARNING: incorrect edge on face "<< myNbDomains <<"."<<endl;
#endif
	  }
	  if (!uvLast.IsEqual(uvL, Precision::PConfusion())) {
	    uvLast = uvL; 
#ifdef DEB_MESH_CHRONO
	    cout <<"WARNING: incorrect edge on face "<< myNbDomains <<"."<<endl;
#endif
	  }
	}

	const TColgp_Array1OfPnt&      Nodes   = T->Nodes();
	const TColStd_Array1OfInteger& Indices = Poly->Nodes();
	Handle(TColStd_HArray1OfReal)  Param   = Poly->Parameters();

	const Standard_Integer nbnodes = Indices.Length();
	TColStd_Array1OfInteger NewNodes(1, nbnodes);
        TColStd_Array1OfInteger NewNodInStruct(1, nbnodes);

        gp_Pnt P3d;
	gp_XY theUV;

        // Process first vertex
        Standard_Integer ipf;
	if (vertices.IsBound(pBegin))
        {
	  ipf = vertices.Find(pBegin);
	}
        else
        {
	  if (sameUV && vertices.IsBound(pEnd))
          {
	    ipf = vertices.Find(pEnd);
	  }
	  else
          {
	    P3d = Nodes(Indices(1));
	    if (!l.IsIdentity())
              P3d.Transform(l.Transformation());
	    myNbLocations++;
	    Location3d.Bind(myNbLocations,P3d);
	    ipf = myNbLocations;
	  }
          vertices.Bind(pBegin,ipf);
	} 
	NewNodInStruct(1) = ipf;
	theUV = FindUV(pBegin, uvFirst, ipf, gFace, mindist);
	BRepMeshAdapt_Vertex vf(theUV,myNbDomains,ipf,MeshDS_Frontier);
	NewNodes(1) = myvemap.Add(structure.Add(vf));

        // Process last vertex
        Standard_Integer ipl;
        if (pEnd.IsSame(pBegin))
        {
          ipl = ipf;
        }
        else
        {
          if (vertices.IsBound(pEnd))
          {
            ipl = vertices.Find(pEnd);
          }
          else
          {
            if (sameUV /*&& vertices.IsBound(pBegin)*/)
            {
              ipl = ipf;
            }
            else
            {
              myNbLocations++;
              Location3d.Bind(myNbLocations,Nodes(Indices(nbnodes)).Transformed(l.Transformation()));
              ipl = myNbLocations;
            }
            vertices.Bind(pEnd,ipl);
          }
        }
	NewNodInStruct(nbnodes) = ipl;
	theUV = FindUV(pEnd, uvLast, ipl, gFace, mindist);
	BRepMeshAdapt_Vertex vl(theUV,myNbDomains,ipl,MeshDS_Frontier);
	NewNodes(nbnodes) = myvemap.Add(structure.Add(vl));
	
	gp_Pnt2d uv;
	BRepMeshAdapt_Vertex v;

	Standard_Real pf, pl;
	Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(edge, face, pf, pl);
	
	if (BRep_Tool::SameParameter(edge))
        {
	  for (i = 2; i < Indices.Length(); i++)
          {
            // Record 3d point
	    P3d = Nodes(Indices(i));
	    if (!l.IsIdentity())
              P3d.Transform(l.Transformation());
	    myNbLocations++;
	    Location3d.Bind(myNbLocations, P3d);
	    NewNodInStruct(i) = myNbLocations;
            // Record 2d point
	    uv = C2d->Value(Param->Value(i));
	    v.Initialize(uv.Coord(), myNbDomains, myNbLocations, MeshDS_Frontier);
	    NewNodes(i) = myvemap.Add(structure.Add(v));
	  }
	}
        else
        {
	  const Standard_Real wFold = Param->Value(Param->Lower());
	  const Standard_Real wLold = Param->Value(Param->Upper());

	  Standard_Real wKoef = 1.;
	  if ((wFold != wFirst || wLold != wLast) && wLold != wFold)
          {
	    wKoef = (wLast - wFirst) / (wLold - wFold);
	  }
	  
	  BRepAdaptor_Curve cons(edge, face);
	  Extrema_LocateExtPC pcos;
	  pcos.Initialize(cons, cons.FirstParameter(), 
			  cons.LastParameter(), Precision::PConfusion());

	  Standard_Real wPrev;
	  Standard_Real wCur      = wFirst;
	  Standard_Real wCurFound = wFirst;
	  for (i = 2; i < Indices.Length(); i++)
          {
            // Record 3d point
	    P3d = Nodes(Indices(i));
	    if (!l.IsIdentity())
              P3d.Transform(l.Transformation());
	    myNbLocations++;
	    Location3d.Bind(myNbLocations, P3d);
	    NewNodInStruct(i) = myNbLocations;
            // Record 2d point
	    wPrev = wCur;
	    wCur  = wFirst + wKoef*(Param->Value(i) - wFold);
            wCurFound += (wCur - wPrev);
	    pcos.Perform(P3d, wCurFound);
	    if (pcos.IsDone()) wCurFound = pcos.Point().Parameter();
	    C2d->D0(wCurFound, uv);
	    v.Initialize(uv.Coord(), myNbDomains, myNbLocations, MeshDS_Frontier);
	    NewNodes(i) = myvemap.Add(structure.Add(v));
	  }
	}

	Handle(Poly_PolygonOnTriangulation) P1 =
          new Poly_PolygonOnTriangulation(NewNodes, Param->Array1());
	P1->Deflection(defedge);
	if (internaledges.IsBound(edge))
        {
	  TColStd_ListOfTransient& L = internaledges.ChangeFind(edge);
          if (edge.Orientation() == TopAbs_REVERSED)
            L.Append(P1);
          else
            L.Prepend(P1);
	}
	else
        {
	  TColStd_ListOfTransient L1;
	  L1.Append(P1);
	  internaledges.Bind(edge, L1);
	}

	Handle(Poly_PolygonOnTriangulation) P2 =
          new Poly_PolygonOnTriangulation(NewNodInStruct, Param->Array1());
	P2->Deflection(defedge);
	TColStd_ListOfTransient L;
	L.Append(P2);
	edges.Bind(edge, L);

	found = Standard_True;
      }
      else
      {
	BRep_Builder B;
	B.UpdateEdge(edge,NullPoly,T,l);
	B.UpdateFace(face,TNull);
      }
    }
    else if (!T.IsNull() && !T->HasUVNodes())
    {
      BRep_Builder B;
      B.UpdateEdge(edge,NullPoly,T,l);
      B.UpdateFace(face,TNull);
    }
  }
  while (!Poly.IsNull()/* && !found*/);

#ifdef DEB_MESH_CHRONO
  chUpdate.Stop();
#endif
  return found;
}


//=======================================================================
//function : InternalVertices
//purpose  : 
//=======================================================================

void BRepMeshAdapt_FastDiscret::InternalVertices(const Handle(BRepAdaptor_HSurface)& caro,
					    BRepMeshAdapt_ListOfVertex&              InternalV,
					    const Standard_Real                 defface,
					    const BRepMeshAdapt_Classifier&          classifier)
{
#ifdef DEB_MESH_CHRONO
  chInternal.Start();
#endif

  BRepMeshAdapt_Vertex newV;
  gp_Pnt2d p2d;
  gp_Pnt p3d;
  MeshShape_ListOfSurfacePoint pntsOnSurf;
  
  // travail suivant le type de surface

  const BRepAdaptor_Surface& BS = *(BRepAdaptor_Surface*)&(caro->Surface());
  GeomAbs_SurfaceType thetype = caro->GetType();

  if (thetype == GeomAbs_Plane && !classifier.NaturalRestriction())
  {
    // rajout d'un seul point au milieu.
    const Standard_Real U = 0.5*(myumin+myumax);
    const Standard_Real V = 0.5*(myvmin+myvmax);
    if (classifier.Perform(gp_Pnt2d(U, V)) == TopAbs_IN)
    {
      // Record 3d point
#ifdef DEB_MESH_CHRONO
      D0Internal++;
#endif
      BRepMeshAdapt_GeomTool::D0(caro, U, V, p3d);
      myNbLocations++;
      Location3d.Bind(myNbLocations, p3d);
      // Record 2d point
      p2d.SetCoord((U-myumin)/deltaX, (V-myvmin)/deltaY);
      newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
      InternalV.Append(newV);//BRepMeshAdapt_AddPoint(p2d, myNbLocations);
    }
  }
  else if (thetype == GeomAbs_Sphere)
  {
    gp_Sphere S = BS.Sphere();
    const Standard_Real R = S.Radius();

    // Calculate parameters for iteration in V direction
    Standard_Real Dv = 1.0 - (defface/R);
    if (Dv < 0.0) Dv = 0.0;
    Standard_Real oldDv = 2.0 * ACos (Dv);
    Dv  =  .7 * oldDv; //.7 ~= sqrt(2.) - Dv is hypotenuse of triangle when oldDv is legs
    const Standard_Real sv = myvmax - myvmin;
    Dv = sv/((Standard_Integer)(sv/Dv) + 1);
    const Standard_Real pasvmax = myvmax-Dv*0.5;

    //Du can be defined from relation: 2*r*Sin(Du/2) = 2*R*Sin(Dv/2), r = R*Cos(v)
    //here approximate relation r*Du = R*Dv is used
    
    Standard_Real Du, pasu, pasv; //, ru;
    const Standard_Real su = myumax-myumin;
    Standard_Boolean Shift = Standard_False;
    for (pasv = myvmin + Dv; pasv < pasvmax; pasv += Dv)
    {
      // Calculate parameters for iteration in U direction
      // 1.-.365*pasv*pasv is simple approximation of Cos(pasv)
      // with condition that it gives ~.1 when pasv = pi/2
      Du = Dv/(1.-.365*pasv*pasv);
      Du = su/((Standard_Integer)(su/Du) + 1);
      Shift = !Shift;
      const Standard_Real d = (Shift)? Du*.5 : 0.;
      const Standard_Real pasumax = myumax-Du*0.5 + d;
      for (pasu = myumin + Du - d; pasu < pasumax; pasu += Du)
      {
	if (classifier.Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
#ifdef DEB_MESH_CHRONO
	  D0Internal++;
#endif
	  ElSLib::D0(pasu, pasv, S, p3d);
	  myNbLocations++;
	  Location3d.Bind(myNbLocations, p3d);
          // Record 2d point
	  p2d.SetCoord((pasu-myumin)/deltaX, (pasv-myvmin)/deltaY);
	  newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
	  InternalV.Append(newV);
	}
      }
    }
  }
  else if (thetype == GeomAbs_Cylinder)
  {
    gp_Cylinder S = BS.Cylinder();
    const Standard_Real R = S.Radius();

    // Calculate parameters for iteration in U direction
    Standard_Real Du = 1.0 - (defface/R);
    if (Du < 0.0) Du = 0.0;
    Du = 2.0 * ACos (Du);
    if (Du > myAngle) Du = myAngle;
    const Standard_Real su = myumax - myumin;
    const Standard_Integer nbU = (Standard_Integer)(su/Du);
    Du = su/(nbU+1);

    // Calculate parameters for iteration in V direction
    const Standard_Real sv = myvmax - myvmin;
    Standard_Integer nbV = (Standard_Integer)( nbU*sv/(su*R) );
    nbV = Min(nbV, 100*nbU);
    Standard_Real Dv = sv/(nbV+1);

    Standard_Real pasu, pasv, pasvmax = myvmax-Dv*0.5, pasumax = myumax-Du*0.5;
    for (pasv = myvmin + Dv; pasv < pasvmax; pasv += Dv) {
      for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
	if (classifier.Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
#ifdef DEB_MESH_CHRONO
	  D0Internal++;
#endif
	  ElSLib::D0(pasu, pasv, S, p3d);
          myNbLocations++;
          Location3d.Bind(myNbLocations, p3d);
          // Record 2d point
	  p2d.SetCoord((pasu-myumin)/deltaX, (pasv-myvmin)/deltaY);
	  newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
          InternalV.Append(newV);//BRepMeshAdapt_AddPoint(p2d, myNbLocations);
	}
      }
    }
  }
  else if (thetype == GeomAbs_Cone)
  {
    Standard_Real R, RefR, SAng;
    gp_Cone C = BS.Cone();
    RefR = C.RefRadius();
    SAng = C.SemiAngle();
    R = Max(Abs(RefR+myvmin*Sin(SAng)), Abs(RefR+myvmax*Sin(SAng)));
    Standard_Real Du, Dv, pasu, pasv;
    Du = Max(1.0e0 - (defface/R),0.0e0);
    Du  = (2.0 * ACos (Du));
//    Du = Min(Du, myAngle);
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
    for (pasv = myvmin + Dv; pasv < pasvmax; pasv += Dv) {
      for (pasu = myumin + Du; pasu < pasumax; pasu += Du) {
	if (classifier.Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
        {
          // Record 3d point
#ifdef DEB_MESH_CHRONO
          D0Internal++;
#endif
	  ElSLib::D0(pasu, pasv, C, p3d);
          myNbLocations++;
          Location3d.Bind(myNbLocations, p3d);
          // Record 2d point
          p2d.SetCoord((pasu-myumin)/deltaX, (pasv-myvmin)/deltaY);
	  newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
          InternalV.Append(newV);//BRepMeshAdapt_AddPoint(p2d, myNbLocations);
	}
      }
    }
  }
  else if (thetype == GeomAbs_Torus)
  {
    gp_Torus T = BS.Torus();

    Standard_Boolean insert;
    Standard_Integer i, j, ParamULength, ParamVLength;
    Standard_Real pp, pasu, pasv;
    Standard_Real r = T.MinorRadius(), R = T.MajorRadius();

    TColStd_SequenceOfReal ParamU, ParamV;

    if (R < r)
    {
      // comme on recupere les points des edges.
      // dans ce cas, les points ne sont pas representatifs.
            
      Standard_Real Du, Dv;//, pasu, pasv;
      Dv = Max(1.0e0 - (defface/r),0.0e0) ;
      Standard_Real oldDv = 2.0 * ACos (Dv);
      oldDv = Min(oldDv, myAngle);
      Dv  =  0.9*oldDv; //TWOTHIRD * oldDv;
      Dv = oldDv;

      Standard_Integer nbV = Max((Standard_Integer)((myvmax-myvmin)/Dv), 2);
      Dv = (myvmax-myvmin)/(nbV+1);
      Standard_Real ru = R + r;
      if (ru > 1.e-16)
      {
        Du  = 2.0 * ACos(Max(1.0 - (defface/ru),0.0));
        if (myAngle < Du) Du = myAngle;
        const Standard_Real aa = sqrt(Du*Du + oldDv*oldDv);
        Du *= Min(oldDv, Du) / aa;
      }
      else Du = Dv;     

      Standard_Integer nbU = Max((Standard_Integer)((myumax-myumin)/Du), 2);
      nbU = Max(nbU , (int)(nbV*(myumax-myumin)*R/((myvmax-myvmin)*r)/5.));
      Du = (myumax-myumin)/(nbU+1);
      //-- On choisit DeltaX et DeltaY de facon a ce qu on ne saute pas 
      //-- de points sur la grille
      for (i = 0; i <= nbU; i++) ParamU.Append(myumin + i* Du);
    }//R<r
    else //U if R > r
    {
      //--ofv: U
      // Number of mapped U parameters
      const Standard_Integer LenU = myUParam.Extent();
      // Fill array of U parameters
      TColStd_Array1OfReal Up(1,LenU);
      for (j = 1; j <= LenU; j++) Up(j) = myUParam(j);
      // Calculate DU, sort array of parameters
      const Standard_Real aDU = FUN_CalcAverageDUV(Up,LenU);
      Standard_Real dUstd = Abs(myumax -  myumin) / (Standard_Real) LenU;
      if (aDU > dUstd) dUstd = aDU;
      // Add U parameters
      for (j = 1; j <= LenU; j++)
      {
        pp = Up(j);
        insert = Standard_True;
        ParamULength = ParamU.Length();
        for (i = 1; i <= ParamULength && insert; i++)
        {
          insert = (Abs(ParamU.Value(i)-pp) > (0.5*dUstd));
        }
        if (insert) ParamU.Append(pp);
      }
    } // else R > r

    //--ofv: V
    // Number of mapped V parameters
    const Standard_Integer LenV = myVParam.Extent();
    // Fill array of V parameters
    TColStd_Array1OfReal Vp(1,LenV);
    for (j = 1; j <= LenV; j++) Vp(j) = myVParam(j);
    // Calculate DV, sort array of parameters
    const Standard_Real aDV = FUN_CalcAverageDUV(Vp,LenV);
    Standard_Real dVstd = Abs(myvmax -  myvmin) / (Standard_Real) LenV;
    if (aDV > dVstd) dVstd = aDV;
    // Add V parameters
    for (j = 1; j <= LenV; j++)
    {
      pp = Vp(j);
      insert = Standard_True;
      ParamVLength = ParamV.Length();
      for (i = 1; i <= ParamVLength && insert; i++)
      {
        insert = (Abs(ParamV.Value(i)-pp) > (dVstd*2./3.));
      }
      if (insert) ParamV.Append(pp);
    }

    Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
    Standard_Real umin = myumin+deltaY*0.1;
    Standard_Real vmin = myvmin+deltaX*0.1;
    Standard_Real umax = myumax-deltaY*0.1;
    Standard_Real vmax = myvmax-deltaX*0.1;

    for (i = 1; i <= Lu; i++)
    {
      pasu = ParamU.Value(i);
      if (pasu >= umin && pasu < umax)
      {
	for (j = 1; j <= Lv; j++)
        {
	  pasv = ParamV.Value(j);
	  if (pasv >= vmin && pasv < vmax)
          {
	    if (classifier.Perform(gp_Pnt2d(pasu, pasv)) == TopAbs_IN)
            {
              // Record 3d point
#ifdef DEB_MESH_CHRONO
	      D0Internal++;
#endif
	      ElSLib::D0(pasu, pasv, T, p3d);
	      myNbLocations++;
	      Location3d.Bind(myNbLocations, p3d);
              // Record 2d point
	      p2d.SetCoord((pasu-myumin)/deltaX, (pasv-myvmin)/deltaY);
	      newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
	      InternalV.Append(newV);
	    }
	  }
	}
      }
    }
  }
  else if (thetype == GeomAbs_BezierSurface || thetype == GeomAbs_BSplineSurface)
  {
    Standard_Integer i, j;
    Standard_Real pp;

    // Sort sequence of U parameters
    TColStd_SequenceOfReal ParamU;
    ParamU.Append(myumin); ParamU.Append(myumax);
    Standard_Integer ParamULength = 2;
    const Standard_Integer LenU = myUParam.Extent();
    const Standard_Real ddu = 0.02*(myumax-myumin);
    Standard_Real U1, U2;
    for (j = 1; j <= LenU; j++)
    {
      pp = myUParam(j);
      U1 = ParamU.Value(1);
      for (i = 2; i <= ParamULength; i++)
      {
	U2 = ParamU.Value(i);
	if (pp < U2)
        {
	  if ((U2-pp) < ddu)
          {
	    ParamU(i) = pp;
	  }
	  else if ((pp-U1) > ddu)
          {
	    ParamU.InsertBefore(i, pp);
	    ParamULength++;
	  }
	  else if (i != 2)
          {
	    ParamU(i-1) = pp;
	  }
	  break;
	}
        U1 = U2;
      }
    }
    
    // Sort sequence of V parameters
    TColStd_SequenceOfReal ParamV;
    ParamV.Append(myvmin); ParamV.Append(myvmax);
    Standard_Integer ParamVLength = 2;
    const Standard_Integer LenV = myVParam.Extent();
    const Standard_Real ddv = 0.02*(myvmax-myvmin);
    Standard_Real V1, V2;
    for (j = 1; j <= LenV; j++)
    {
      pp = myVParam(j);
      V1 = ParamV.Value(1);
      for (i = 2; i <= ParamVLength; i++)
      {
	V2 = ParamV.Value(i);
	if (pp < V2)
        {
	  if ((V2-pp) < ddv)
          {
	    ParamV(i) = pp;
	  }
	  else if ((pp-V1) > ddv)
          {
	    ParamV.InsertBefore(i, pp);
	    ParamVLength++;
	  }
	  else if (i != 2)
          {
	    ParamV(i-1) = pp;
	  }
	  break;
	}
        V1 = V2;
      }
    }

    // controle des isos U et insertion eventuelle:

    Handle(Geom_Surface) B;
    if (thetype == GeomAbs_BezierSurface) {
      B = BS.Bezier();
    }
    else {
      B = BS.BSpline();
    }

    gp_Pnt P1, P2, PControl;
    Standard_Real u, v, dist;

    // Insert V parameters by deflection criterion
    for (i = 1; i <= ParamULength; i++)
    {
      Handle(Geom_Curve) IsoU = B->UIso(ParamU.Value(i));
      V1 = ParamV.Value(1);
      P1 = IsoU->Value(V1);
      for (j = 2; j <= ParamVLength;)
      {
	V2 = ParamV.Value(j);
	P2 = IsoU->Value(V2);
	v = 0.5*(V1+V2);
	PControl = IsoU->Value(v);
	if (P1.SquareDistance(P2) > 1.e-5)
        {
	  gp_Lin L (P1, gp_Dir(gp_Vec(P1, P2)));
	  dist = L.Distance(PControl);
	}
	else
        {
	  dist = P1.Distance(PControl);
	}
        if (dist > defface)
        {
          // insertion 
          ParamV.InsertBefore(j, v);
          ParamVLength++;
        }
        else
        {
          V1 = V2;
          P1 = P2;
          j++;
        }
      }
    }

    for (i = 2; i < ParamVLength; i++)
    {
      v = ParamV.Value(i);
      Handle(Geom_Curve) IsoV = B->VIso(v);
      U1 = ParamU.Value(1);
      P1 = IsoV->Value(U1);
      for (j = 2; j <= ParamULength;)
      {
	U2 = ParamU.Value(j);
	P2 = IsoV->Value(U2);
	u = 0.5*(U1+U2);
	PControl = IsoV->Value(u);
	if (P1.SquareDistance(P2) > 1.e-5)
        {
	  gp_Lin L (P1, gp_Dir(gp_Vec(P1, P2)));
	  dist = L.Distance(PControl);
	}
	else
        {
	  dist = P1.Distance(PControl);
	}
        if (dist > defface)
        {
          // insertion 
          ParamU.InsertBefore(j, u);
          ParamULength++;
        }
        else
        {
          U1 = U2;
          P1 = P2;
          if (j < ParamULength)
          {
            // Classify intersection point
            if (classifier.Perform(gp_Pnt2d(U1, v)) == TopAbs_IN)
            {
              // Record 3d point
              myNbLocations++;
              Location3d.Bind(myNbLocations, P1);
              // Record 2d point
              p2d.SetCoord((U1-myumin)/deltaX, (v-myvmin)/deltaY);
              newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
              InternalV.Append(newV);
            }
          }
          j++;
        }
      }
    }
  }
  else
  {
    const Standard_Real theangle = 0.35;

    Standard_Integer i, j, nbpointsU = 10, nbpointsV = 10;
    Adaptor3d_IsoCurve tabu[10], tabv[10];

    TColStd_SequenceOfReal ParamU, ParamV;
    Standard_Real u, v, du, dv;
    Standard_Integer iu, iv;
    Standard_Real f, l;

    du = (myumax-myumin) / (nbpointsU+1); dv = (myvmax-myvmin) / (nbpointsV+1);

    for (iu = 1; iu <= nbpointsU; iu++) {
      u = myumin + iu*du;
      tabu[iu-1].Load(caro);
      tabu[iu-1].Load(GeomAbs_IsoU, u);
    }

    for (iv = 1; iv <= nbpointsV; iv++) {
      v = myvmin + iv*dv;
      tabv[iv-1].Load(caro);
      tabv[iv-1].Load(GeomAbs_IsoV, v);
    }

    Standard_Integer imax = 1, MaxV = 0;
    
    GCPnts_TangentialDeflection* tabGU = new GCPnts_TangentialDeflection[nbpointsU];

    for (i = 0; i <= nbpointsU-1; i++) {
      f = Max(myvmin, tabu[i].FirstParameter());
      l = Min(myvmax, tabu[i].LastParameter());
      GCPnts_TangentialDeflection theDeflection(tabu[i], f, l, theangle, 0.7*defface, 2);
      tabGU[i] = theDeflection;
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
      GCPnts_TangentialDeflection thedeflection2(tabv[i], f, l, theangle, 0.7*defface, 2);
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
    
    TColStd_SequenceOfReal InsertV, InsertU;
    gp_Pnt P1;

    Adaptor3d_IsoCurve IsoV;
    IsoV.Load(caro);

    Standard_Integer Lu = ParamU.Length(), Lv = ParamV.Length();
    
    for (i = 2; i < Lv; i++) {
      v = ParamV.Value(i);
      IsoV.Load(GeomAbs_IsoV, v);
      for (j = 2; j < Lu; j++) {
	u = ParamU.Value(j);
	if (classifier.Perform(gp_Pnt2d(u, v)) == TopAbs_IN)
        {
          // Record 3d point
	  P1 = IsoV.Value(u);
	  myNbLocations++;
          Location3d.Bind(myNbLocations, P1);
          // Record 2d point
	  p2d.SetCoord((u-myumin)/deltaX, (v-myvmin)/deltaY);
	  newV.Initialize(p2d.XY(), myNbDomains, myNbLocations, MeshDS_Free);
	  InternalV.Append(newV); //BRepMeshAdapt_AddPoint(p2d, myNbLocations);
	}
      }
    } 
  }
#ifdef DEB_MESH_CHRONO
  chInternal.Stop();
#endif
}


//=======================================================================
//function : Control
//purpose  : 
//=======================================================================

Standard_Real BRepMeshAdapt_FastDiscret::Control
  (const Handle(BRepAdaptor_HSurface)& caro,
   const Standard_Real                 defface,
   const Standard_Boolean              isfirst)
{
  //IMPORTANT: Constants used in calculations
  const Standard_Real MinimalArea2d = 1.e-9;
  const Standard_Real MinimalSqLength3d = 1.e-12;

  // Define the number of iterations
  const Standard_Integer nbPasses = (isfirst? 1 : myNbIterations);

  // Initialize stop condition
  Standard_Boolean allDegenerated = Standard_False;
  Standard_Integer nbInserted = 1;

  // Create map of links to skip already processed
  Standard_Integer nbnodes, nbtriangles;
  triangleAdapt_GetMeshSize(&nbnodes,&nbtriangles);
  if (nbnodes <= 0 || nbtriangles <= 0) return -1.0;
  MeshShape_MapOfCouple theCouples(3*nbtriangles);

#ifdef DEB_MESH_CHRONO
  chControl.Start();
  if (!isfirst) NbControls++;
#endif

  gp_XY mi2d;
  gp_XYZ vecEd1, vecEd2, vecEd3;
  gp_Pnt pDef;
  Standard_Real x, y, dv, defl, maxdef;
  Standard_Integer pass, i, nf, nl, n1, n2, n3, m1, m2, m3;
  BRepMeshAdapt_Vertex InsVertex;
  Standard_Boolean caninsert;

  // Perform refinement passes
  for (pass = 1; pass <= nbPasses && nbInserted && !allDegenerated; pass++)
  {
    // Reset stop condition
    allDegenerated = Standard_True;
    nbInserted = 0;
    maxdef = -1.0;

    // Do not insert nodes in last pass in non-SharedMode
    caninsert = (mySharedMode || pass < nbPasses);

    // Read mesh size
    triangleAdapt_GetMeshSize(&nbnodes,&nbtriangles);
    if (nbnodes <= 0 || nbtriangles <= 0) break;

    // Read current nodes
    TColgp_Array1OfPnt2d Nodes2d(1, nbnodes);
    TColgp_Array1OfPnt Nodes(1, nbnodes);
    TColStd_Array1OfInteger Indices(1, nbnodes);
    Standard_Integer nindex = 1;
	Standard_Integer dummy;
    triangleAdapt_InitPoints();
    while (triangleAdapt_NextPoint(nindex,&x,&y,&i,&dummy))
    {
      Nodes2d.SetValue(nindex,gp_Pnt2d(x*deltaX+myumin,y*deltaY+myvmin));
      Nodes.SetValue(nindex,Location3d(i));
      Indices.SetValue(nindex,i);
      nindex++;
    }

    // Iterate on current triangles
    BRepMeshAdapt_ListOfVertex InsVertices;
    triangleAdapt_InitTriangles();
    while (triangleAdapt_NextTriangle(&n1,&n2,&n3,&m1,&m2,&m3))
    {
      const gp_XYZ& p1 = Nodes(n1).XYZ();
      const gp_XYZ& p2 = Nodes(n2).XYZ();
      const gp_XYZ& p3 = Nodes(n3).XYZ();

      vecEd1 = p2 - p1;
      vecEd2 = p3 - p2;
      vecEd3 = p1 - p3;

      // Check for degenerated triangle
      if (vecEd1.SquareModulus() < MinimalSqLength3d ||
          vecEd2.SquareModulus() < MinimalSqLength3d ||
          vecEd3.SquareModulus() < MinimalSqLength3d) continue;

      allDegenerated = Standard_False;

      const gp_XY& xy1 = Nodes2d(n1).XY();
      const gp_XY& xy2 = Nodes2d(n2).XY();
      const gp_XY& xy3 = Nodes2d(n3).XY();

      // Check triangle area in 2d
      if (Abs((xy2-xy1)^(xy3-xy1)) < MinimalArea2d) continue;

      // Check triangle normal
      gp_XYZ normal(vecEd1^vecEd2);
      dv = normal.Modulus();
      if (dv == 0.) continue;
      normal /= dv;

      // Check deflection on triangle
#ifdef DEB_MESH_CHRONO
      D0Control++;
#endif
      mi2d = (xy1+xy2+xy3)/3.0;
      caro->D0(mi2d.X(), mi2d.Y(), pDef);
      defl = Abs(normal*(pDef.XYZ()-p1));
      if (defl > maxdef) maxdef = defl;
      if (defl > defface)
      {
        if (isfirst) break;
        if (caninsert)
        {
          // Record new vertex
          myNbLocations++;
          Location3d.Bind(myNbLocations,pDef);
          InsVertex.Initialize(mi2d,myNbDomains,myNbLocations,MeshDS_Free);
          InsVertices.Append(InsVertex);
        }
      }

      // Convert node numbers to 3d indices
      n1 = Indices(n1);
      n2 = Indices(n2);
      n3 = Indices(n3);
      
      if (!m2) // Not a boundary
      {
        // Check if this link was already processed
        if (n2 < n3) { nf = n2; nl = n3; } else { nf = n3; nl = n2; }
        if (theCouples.Add(MeshShape_Couple(nf,nl)))
        {
          // Check deflection on edge 1
#ifdef DEB_MESH_CHRONO
          D0Control++;
#endif
          mi2d = (xy2+xy3)*0.5;
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          defl = Abs(normal*(pDef.XYZ()-p1));
          if (defl > maxdef) maxdef = defl;
          if (defl > defface)
          {
            if (isfirst) break;
            if (caninsert)
            {
              // Record new vertex
              myNbLocations++;
              Location3d.Bind(myNbLocations,pDef);
              InsVertex.Initialize(mi2d,myNbDomains,myNbLocations,MeshDS_Free);
              InsVertices.Append(InsVertex);
            }
          }
        }
      }

      if (!m3) // Not a boundary
      {
        // Check if this link was already processed
        if (n1 < n3) { nf = n1; nl = n3; } else { nf = n3; nl = n1; }
        if (theCouples.Add(MeshShape_Couple(nf,nl)))
        {
          // Check deflection on edge 2
#ifdef DEB_MESH_CHRONO
          D0Control++;
#endif
          mi2d = (xy3+xy1)*0.5;
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          defl = Abs(normal*(pDef.XYZ()-p2));
          if (defl > maxdef) maxdef = defl;
          if (defl > defface)
          {
            if (isfirst) break;
            if (caninsert)
            {
              // Record new vertex
              myNbLocations++;
              Location3d.Bind(myNbLocations,pDef);
              InsVertex.Initialize(mi2d,myNbDomains,myNbLocations,MeshDS_Free);
              InsVertices.Append(InsVertex);
            }
          }
        }
      }

      if (!m1) // Not a boundary
      {
        // Check if this link was already processed
        if (n1 < n2) { nf = n1; nl = n2; } else { nf = n2; nl = n1; }
        if (theCouples.Add(MeshShape_Couple(nf,nl)))
        {
          // Check deflection on edge 3
#ifdef DEB_MESH_CHRONO
          D0Control++;
#endif
          mi2d = (xy1+xy2)*0.5;
          caro->D0(mi2d.X(), mi2d.Y(), pDef);
          defl = Abs(normal*(pDef.XYZ()-p3));
          if (defl > maxdef) maxdef = defl;
          if (defl > defface)
          {
            if (isfirst) break;
            if (caninsert)
            {
              // Record new vertex
              myNbLocations++;
              Location3d.Bind(myNbLocations,pDef);
              InsVertex.Initialize(mi2d,myNbDomains,myNbLocations,MeshDS_Free);
              InsVertices.Append(InsVertex);
            }
          }
        }
      }
    }

    if (!isfirst)
    {
      // Add inserted vertices
      BRepMeshAdapt_ListIteratorOfListOfVertex IT(InsVertices);
      for (; IT.More(); IT.Next())
      {
        const BRepMeshAdapt_Vertex& v = IT.Value();
        if (triangleAdapt_AddPoint((v.Coord().X()-myumin)/deltaX,
                              (v.Coord().Y()-myvmin)/deltaY,
                              v.Location3d()))
          nbInserted++;
      }
    }
  }

#ifdef DEB_MESH_CHRONO
  chControl.Stop();
#endif

  return maxdef;
}


//=======================================================================
//function : FindUV
//purpose  : 
//=======================================================================

gp_XY BRepMeshAdapt_FastDiscret::FindUV(const TopoDS_Vertex&   V,
				   const gp_Pnt2d&        XY, 
				   const Standard_Integer ip,
				   const Handle(BRepAdaptor_HSurface)& S,
				   const Standard_Real mindist) 
{
  gp_XY theUV;
  if (mylocation2d.IsBound(ip))
  {
    BRepMeshAdapt_ListOfXY& L = mylocation2d.ChangeFind(ip);
    theUV = L.First();
    if (L.Extent() != 1)
    {
      BRepMeshAdapt_ListIteratorOfListOfXY it(L);
      it.Next();
      Standard_Real dd, dmin = XY.Distance(gp_Pnt2d(theUV));
      for (; it.More(); it.Next())
      {
	dd = XY.Distance(gp_Pnt2d(it.Value()));
	if (dd < dmin)
        {
	  theUV = it.Value();
	  dmin = dd;
	}
      }
    }

    const Standard_Real tol = Min(2. * BRep_Tool::Tolerance(V), mindist);

    const Standard_Real Utol2d = .5 * (S->LastUParameter() - S->FirstUParameter());
    const Standard_Real Vtol2d = .5 * (S->LastVParameter() - S->FirstVParameter());

    const gp_Pnt p1 = S->Value(theUV.X(), theUV.Y());
    const gp_Pnt p2 = S->Value(XY.X(), XY.Y());

    if (Abs(theUV.X() - XY.X()) > Utol2d ||
        Abs(theUV.Y() - XY.Y()) > Vtol2d ||
	!p1.IsEqual(p2, tol))
    {
      theUV = XY.Coord();
      L.Append(theUV);
    }
  }
  else
  {
    theUV = XY.Coord();
    BRepMeshAdapt_ListOfXY L;
    L.Append(theUV);
    mylocation2d.Bind(ip, L);
  }
  return theUV;
}


//=======================================================================
//function : NotTriangulatedFaces
//purpose  : 
//=======================================================================

void BRepMeshAdapt_FastDiscret::NotTriangulatedFaces(TopTools_ListOfShape& L) const
{
  L = mynottriangulated;
}


//=======================================================================
//function : CurrentFaceStatus
//purpose  : 
//=======================================================================

BRepMeshAdapt_Status BRepMeshAdapt_FastDiscret::CurrentFaceStatus() const
{
  return myfacestate;
}
