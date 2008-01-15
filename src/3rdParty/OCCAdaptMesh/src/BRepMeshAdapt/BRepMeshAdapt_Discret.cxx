// File:	BRepMeshAdapt_Discret.cxx
// Created:	Tue Feb 27 16:39:53 1996
// Author:	Laurent PAINNOT
//		<lpa@nonox>


#include <BRepMeshAdapt_Discret.ixx>

#include <BRepMeshAdapt_Array1OfVertexOfDelaun.hxx>
#include <BRepMeshAdapt_ListIteratorOfListOfVertex.hxx>
#include <BRepMeshAdapt_GeomTool.hxx>
#include <MeshShape_ListIteratorOfListOfSurfacePoint.hxx>
#include <MeshShape_CurvatureType.hxx>
#include <MeshShape_Couple.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopAbs.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
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
#include <TColgp_Array1OfPnt2d.hxx>
#include <Precision.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <Geom2d_Curve.hxx>

#include <MeshShape_DataMapOfShapeListOfTransient.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <MeshShape_DataMapIteratorOfDataMapOfShapeListOfTransient.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <BRepMeshAdapt_ShapeTool.hxx>



static Standard_Integer LIMITE_TRIANGULATION = 1048576;



//=======================================================================
//function : BRepMeshAdapt_Discret
//purpose  : 
//=======================================================================
BRepMeshAdapt_Discret::BRepMeshAdapt_Discret(const Standard_Real    defle,
				   const Standard_Real    angl,
				   const Standard_Boolean ws,
				   const Standard_Boolean inshape,
				   const Standard_Boolean relative,
				   const Standard_Boolean shapetrigu): 
  angle(angl), deflection(defle),WithShare(ws),nbDomains(0),  nbLocat(0), 
 myrelative(relative), myshapetrigu(shapetrigu), myinshape(inshape)
{
  structure=new BRepMeshAdapt_DataStructureOfDelaun();
}


//=======================================================================
//function : BRepMeshAdapt_Discret
//purpose  : 
//=======================================================================

BRepMeshAdapt_Discret::BRepMeshAdapt_Discret(const Standard_Real    defle,
				   const TopoDS_Shape&    shape,
				   const Standard_Real    angl,
				   const Standard_Boolean ws,
				   const Standard_Boolean inshape,
				   const Standard_Boolean relative,
				   const Standard_Boolean shapetrigu): 
 angle(angl), deflection(defle),WithShare(ws),nbDomains(0),  nbLocat(0), 
 myrelative(relative), myshapetrigu(shapetrigu), myinshape(inshape)
{
  structure=new BRepMeshAdapt_DataStructureOfDelaun();
  Add(shape);
}


//=======================================================================
//function : BRepMeshAdapt_Discret
//purpose  : 
//=======================================================================

BRepMeshAdapt_Discret::BRepMeshAdapt_Discret(const Standard_Real    defle,
				   const TopoDS_Face&     face,
				   const Standard_Real    angl,
				   const Standard_Boolean ws,
				   const Standard_Boolean inshape,
				   const Standard_Boolean relative,
				   const Standard_Boolean shapetrigu): 
  angle(angl), deflection(defle),WithShare(ws),  nbDomains(0), nbLocat(0),
 myrelative(relative), myshapetrigu(shapetrigu), myinshape(inshape)
{
  structure=new BRepMeshAdapt_DataStructureOfDelaun();
  Add(face);

}


//=======================================================================
//function : Add(shape)
//purpose  : 
//=======================================================================
void BRepMeshAdapt_Discret::Add(const TopoDS_Shape& shape)
{
  TopExp_Explorer ex;
  for (ex.Init(shape, TopAbs_FACE); ex.More(); ex.Next()) {
    Add(TopoDS::Face(ex.Current()));
  }
}


//=======================================================================
//function : Add(face)
//purpose  : 
//=======================================================================
void BRepMeshAdapt_Discret::Add(const TopoDS_Face& face)
{
  nbDomains++;
  Domains.Bind(nbDomains, face);
  structure->NewDomain(nbDomains);
  BRepAdaptor_Surface  BS(face, Standard_False);
  Handle(BRepAdaptor_HSurface) gFace = new BRepAdaptor_HSurface(BS);

  myVeloc.Clear();
  gp_Pnt2d uvFirst, uvLast;

  TopExp_Explorer ex;
  for (ex.Init(face, TopAbs_EDGE); ex.More(); ex.Next()) {
    BRep_Tool::UVPoints(TopoDS::Edge(ex.Current()), face, uvFirst, uvLast);
    myVeloc.Append(uvFirst);
    myVeloc.Append(uvLast);
  }

  TopAbs_Orientation orFace = face.Orientation();
  Handle(Poly_Triangulation) T;
  TopLoc_Location l;

  if (!WithShare) {          
    vertices.Clear();
    edges.Clear();
  }

  mylistver.Clear();
  myvemap.Clear();
  mylocation2d.Clear();
  internaledges.Clear();

  Standard_Integer i, j;//,i1, i2, k;
  i = 1;

  Standard_Real defedge, defface;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
  Standard_Integer nbEdge = 0;
  defface = 0.;

  for (ex.Init(face, TopAbs_EDGE); ex.More(); ex.Next()) {
    const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
    if (myrelative) {
      nbEdge++;
      Bnd_Box B;
      BndLib_Add3dCurve::Add(BRepAdaptor_Curve(edge),0.,B);
      B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      dx = aXmax-aXmin;
      dy = aYmax-aYmin;
      dz = aZmax-aZmin;
      defedge = dx;
      if (defedge < dy) defedge = dy;
      if (defedge < dz) defedge = dz;
      defedge = defedge * deflection;
      defface = defface + defedge;
    }
    else defedge = deflection;
    Add(edge, face, gFace, defedge);
  }

  if (myrelative && nbEdge != 0) defface = defface / nbEdge;
  else                           defface = deflection;

  T = BRep_Tool::Triangulation(face, l);


  if (mylistver.Extent() ==  0) return;

  if (!myshapetrigu || T.IsNull()) {
    BRepMeshAdapt_Array1OfVertexOfDelaun tabvert(1, mylistver.Extent());
    BRepMeshAdapt_ListIteratorOfListOfVertex itVer(mylistver);
    
    Standard_Real xCur, yCur;
    Standard_Real maxX, minX, maxY, minY;
    minX=minY=1.e100;
    maxX=maxY=-1.e100;
    
    Standard_Integer ipn = 0;
    for (; itVer.More(); itVer.Next()) {
      ipn++;
      xCur=itVer.Value().Coord().X();
      yCur=itVer.Value().Coord().Y();
      minX=Min(xCur, minX);
      maxX=Max(xCur, maxX);
      minY=Min(yCur, minY);
      maxY=Max(yCur, maxY);
      tabvert(ipn) = itVer.Value();
    }
    myumin = minX;
    myumax = maxX;
    myvmin = minY;
    myvmax = maxY;
    
    
    BRepMeshAdapt_Delaun trigu(structure, tabvert, orFace==TopAbs_FORWARD);

    Standard_Boolean isaline;
    isaline = ((myumax-myumin)<1.e-05) || ((myvmax-myvmin)<1.e-05);
    
    if (!isaline && structure->ElemOfDomain(nbDomains).Extent() > 0) {
      TColStd_ListOfInteger badTri, nulTri;
      
      if (Control(gFace, defface, mylistver, badTri, nulTri, Standard_True)) {
	
	mylistver.Clear();
	InternalVertices(trigu, gFace, tabvert, mylistver, defface);
	if (mylistver.Extent() > 0) {
	  BRepMeshAdapt_Array1OfVertexOfDelaun verttab(1, mylistver.Extent());
	  BRepMeshAdapt_ListIteratorOfListOfVertex itVer(mylistver);
	  ipn = 1;
	  for (; itVer.More(); itVer.Next())
	    verttab(ipn++) = itVer.Value();
	  trigu.AddVertices(verttab);
	}
      }
      
      Standard_Integer nbPassage = 0;
      
      while (mylistver.Extent() > 0 && nbPassage <= 10) {
	nbPassage++;
	mylistver.Clear();
	nulTri.Clear();
	badTri.Clear();
	Control(gFace, defface, mylistver, badTri, nulTri, Standard_False);
	
	if (mylistver.Extent() > 0) {
	  BRepMeshAdapt_Array1OfVertexOfDelaun verttab(1, mylistver.Extent());
	  BRepMeshAdapt_ListIteratorOfListOfVertex itVer(mylistver);
	  ipn = 1;
	  for (; itVer.More(); itVer.Next())
	    verttab(ipn++) = itVer.Value();
	  trigu.AddVertices(verttab);
	}
      }
    }
    structure->ClearDeleted();
    
    if (myinshape && T.IsNull()) AddInShape(face, defface);
  }

  else {
    // recup des triangles pour les mettre dans la structure.
    const TColgp_Array1OfPnt& Nodes = T->Nodes();
    const TColgp_Array1OfPnt2d& UVNodes = T->UVNodes();
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    Standard_Integer nt, ed1, ed2, ed3, tri;
#ifdef DEB
    Standard_Integer nbNodes = 
#endif
      Nodes.Length();
    Standard_Integer nbTriangles = T->NbTriangles();
    BRepMeshAdapt_Vertex v1, v2, v3;
    Standard_Integer iv1, iv2, iv3;
    Standard_Integer k, niver, nFree;//, nInternal;
    Standard_Integer t[3], n[3];


    Poly_Connect pc(T);

    // count the free edges
    nFree = 0;
    TColStd_MapOfInteger vFree;
    for (i = 1; i <= nbTriangles; i++) {
      pc.Triangles(i,t[0],t[1],t[2]);
      triangles(i).Get(n[0], n[1], n[2]);
      for (j = 0; j < 3; j++) {
	if (t[j] == 0)  {
	  k = (j+1) % 3;
	  if (!vFree.Contains(n[j])) vFree.Add(n[j]);
	  if (!vFree.Contains(n[k])) vFree.Add(n[k]);
	  nFree++;
	}
      }
    }

    TColStd_DataMapOfIntegerInteger IDMAP;
    Standard_Integer index;


    // parmi ces noeuds exterieurs, trouver leur indice dans la structure.
    TColStd_MapIteratorOfMapOfInteger it;

      // iteration sur vemap;
    for (i = 1; i <= myvemap.Extent(); i++) {
      index = myvemap.FindKey(i);

      for (it.Initialize(vFree); it.More(); it.Next()) {
	const gp_Pnt2d& P2d = UVNodes(it.Key());
	if (Vertex(index).Coord().IsEqual(P2d.Coord(), Precision::PConfusion())) {
	  IDMAP.Bind(it.Key(), index);
	  vFree.Remove(it.Key());
	  break;
	}
      }
    }
    
    gp_Pnt P1, P2, P3;

    for (nt = 1; nt <= nbTriangles; nt++) {
      triangles(nt).Get(n[0], n[1], n[2]);
      if (!l.IsIdentity()) {
	P1 = Nodes(n[0]).Transformed(l.Transformation());
	P2 = Nodes(n[1]).Transformed(l.Transformation());
	P3 = Nodes(n[2]).Transformed(l.Transformation());
      }
      else {
	P1 = Nodes(n[0]);
	P2 = Nodes(n[1]);
	P3 = Nodes(n[2]);
      }
      const gp_Pnt2d& P12d = UVNodes(n[0]);
      const gp_Pnt2d& P22d = UVNodes(n[1]);
      const gp_Pnt2d& P32d = UVNodes(n[2]);


      if (!IDMAP.IsBound(n[0])) {
	nbLocat++;
	v1.Initialize(P12d.XY(), nbDomains, nbLocat, MeshDS_OnSurface);
	Location3d.Bind(nbLocat, P1);
	iv1 = structure->AddNode(v1);
	IDMAP.Bind(n[0], iv1);
      }
      else {
	iv1 = IDMAP.Find(n[0]);
      }

      if (!IDMAP.IsBound(n[1])) {
	nbLocat++;
	v2.Initialize(P22d.XY(), nbDomains, nbLocat, MeshDS_OnSurface);
	Location3d.Bind(nbLocat, P2);
	iv2 = structure->AddNode(v2);
	IDMAP.Bind(n[1], iv2);
      }
      else {
	iv2 = IDMAP.Find(n[1]);
      }

      if (!IDMAP.IsBound(n[2])) {
	nbLocat++;
	v3.Initialize(P32d.XY(), nbDomains, nbLocat, MeshDS_OnSurface);
	Location3d.Bind(nbLocat, P3);
	iv3 = structure->AddNode(v3);
	IDMAP.Bind(n[2], iv3);
      }
      else {
	iv3 = IDMAP.Find(n[2]);
      }

      if (face.Orientation() == TopAbs_REVERSED) {
	niver=iv2;
	iv2=iv3;
	iv3=niver;
      }

      ed1=structure->AddLink(BRepMeshAdapt_Edge(iv1,iv2,MeshDS_Free, nbDomains));
      ed2=structure->AddLink(BRepMeshAdapt_Edge(iv2,iv3,MeshDS_Free, nbDomains));
      ed3=structure->AddLink(BRepMeshAdapt_Edge(iv3,iv1,MeshDS_Free, nbDomains));
      tri = structure->AddElement(BRepMeshAdapt_Triangle(Abs(ed1), Abs(ed2), Abs(ed3), 
						    (ed1>0), (ed2>0), (ed3>0), 
						    MeshDS_Free, nbDomains));

    }
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::Add(const TopoDS_Edge&                  edge, 
			   const TopoDS_Face&                  face, 
			   const Handle(BRepAdaptor_HSurface)& gFace,
			   const Standard_Real                 defedge)
{
 if (!myshapetrigu || !Update(edge, face, defedge)) {
  Standard_Real wFirst, wLast, wCur;
  Standard_Integer ip1, ip2=0, ipn, iv1, iv2, ivl, isv, isv1, isvl;
  BRepMeshAdapt_Vertex v1, v2, vl;
  gp_Pnt2d uv, uvFirst, uvLast;
  gp_Pnt   l3d;
  gp_XY theUV;
// Standard_Real tol, dist;
  TopoDS_Vertex pBegin, pEnd;
  TopAbs_Orientation orEdge = edge.Orientation();
  BRep_Tool::Range(edge, face, wFirst, wLast);
  BRep_Tool::UVPoints(edge, face, uvFirst, uvLast);
  Standard_Boolean sameUV = uvFirst.IsEqual(uvLast, Precision::PConfusion());
  TopExp::Vertices(edge, pBegin, pEnd);
  Standard_Real otherdefedge = defedge/2.;

  Handle(Poly_PolygonOnTriangulation) Poly, P2;

  if (!edges.IsBound(edge)) {
    if (!vertices.IsBound(pBegin)) {
      if (sameUV && vertices.IsBound(pEnd)) {
	ip1= vertices.Find(pEnd);
      }
      else {
	nbLocat++;
	ip1 = nbLocat;
	Location3d.Bind(ip1, BRep_Tool::Pnt(pBegin));
	vertices.Bind(pBegin, ip1);
      }
    } 
    else ip1 = vertices.Find(pBegin);

    theUV = FindUV(pBegin, uvFirst, ip1);

    v1.Initialize(theUV, nbDomains, ip1, MeshDS_Frontier);
    iv1=structure->IndexOf(v1);
    if (iv1 == 0) {
      iv1 = structure->AddNode(v1);
      mylistver.Append(v1);
    }
    
    if (!sameUV) {
      if (!vertices.IsBound(pEnd)) {
	nbLocat++;
	ipn = nbLocat;
	Location3d.Bind(ipn, BRep_Tool::Pnt(pEnd));
	vertices.Bind(pEnd, ipn);
      }
      else ipn = vertices.Find(pEnd);
      
      theUV = FindUV(pEnd, uvLast, ipn);
      
      vl.Initialize(theUV, nbDomains, ipn, MeshDS_Frontier);
      ivl=structure->IndexOf(vl);
      if (ivl == 0) {
	ivl = structure->AddNode(vl);
	mylistver.Append(vl);
      }
    }
    else {ivl=iv1; vl=v1;}
    
    isv1 = myvemap.FindIndex(iv1);
    if (isv1 == 0) isv1 = myvemap.Add(iv1);
    isvl = myvemap.FindIndex(ivl);
    if (isvl == 0) isvl = myvemap.Add(ivl);

    if (!BRep_Tool::Degenerated(edge)) {
	
      if (orEdge == TopAbs_INTERNAL) otherdefedge = otherdefedge/2.;
      
      BRepAdaptor_Curve cons;
      Standard_Real pf, pl;
      TopLoc_Location L;
      const Handle(Geom_Curve) CC = BRep_Tool::Curve(edge, L, pf, pl);
      if (!CC.IsNull()) cons.Initialize(edge);
      else              cons.Initialize(edge, face);

      BRepMeshAdapt_GeomTool GT(cons, wFirst, wLast, angle, otherdefedge);
      
      // Creation des polygones sur triangulation:
      Standard_Integer nbnodes = GT.NbPoints();
      TColStd_Array1OfInteger Nodes(1, nbnodes);
      TColStd_Array1OfInteger NodInStruct(1, nbnodes);
      TColStd_Array1OfReal Param(1, nbnodes);
      
      // traitement du 1er point:
      Nodes(1) = isv1;
      NodInStruct(1) = ip1;
      Param(1) = wFirst;
      
      Standard_Integer pp;
      for (pp = 2; pp < GT.NbPoints(); pp++) {
	nbLocat++;
	ip2 = nbLocat;
	GT.Value(cons, gFace, pp, wCur, l3d, uv);
	Location3d.Bind(ip2, l3d);
	v2.Initialize(uv.Coord(), nbDomains, ip2, MeshDS_OnCurve);
	iv2=structure->AddNode(v2);
	mylistver.Append(v2);
	
	isv = myvemap.FindIndex(iv2);
	if (isv == 0) isv = myvemap.Add(iv2);
	Nodes(pp) = isv;
	NodInStruct(pp) = ip2;
	Param(pp) = wCur;
	
	if (orEdge == TopAbs_FORWARD)
	  structure->AddLink(BRepMeshAdapt_Edge(iv1, iv2, MeshDS_Frontier, nbDomains));
	else if (orEdge == TopAbs_REVERSED)
	  structure->AddLink(BRepMeshAdapt_Edge(iv2, iv1, MeshDS_Frontier, nbDomains));
	else if (orEdge == TopAbs_INTERNAL)
	  structure->AddLink(BRepMeshAdapt_Edge(iv1, iv2, MeshDS_Fixed, nbDomains));
	iv1 = iv2;
      }
      
      // dernier point:
      Nodes(nbnodes) = isvl;
      NodInStruct(nbnodes) = ipn;
      Param(nbnodes) = wLast;
      
      Poly = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2   = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }
    else {
      // creation du nouveau:
      TColStd_Array1OfInteger Nodes(1, 2), NodInStruct(1, 2);
      TColStd_Array1OfReal Param(1, 2);
      
      Nodes(1) = isv1;
      NodInStruct(1) = ip1;
      Param(1) = wFirst;
      
      Nodes(2) = isvl;
      NodInStruct(2) = ip2;
      Param(2) = wLast;
      
      Poly = new Poly_PolygonOnTriangulation(Nodes, Param);
      P2   = new Poly_PolygonOnTriangulation(NodInStruct, Param);
    }
    
    P2->Deflection(otherdefedge);
    TColStd_ListOfTransient L;
    L.Append(P2);
    edges.Bind(edge, L);
    
    if (iv1 != ivl) {
      if (orEdge == TopAbs_FORWARD)
	structure->AddLink(BRepMeshAdapt_Edge(iv1, ivl, MeshDS_Frontier, nbDomains));
      else if (orEdge == TopAbs_REVERSED)
	structure->AddLink(BRepMeshAdapt_Edge(ivl, iv1, MeshDS_Frontier, nbDomains));
      else if (orEdge == TopAbs_INTERNAL)
	structure->AddLink(BRepMeshAdapt_Edge(iv1, ivl, MeshDS_Fixed, nbDomains));
    }
  }
  
  
  // Si on a déjà vu cette Edge et qu'elle n'est pas dégénérée on reprend
  // les points du polygon calculés à la première rencontre :
  
  else  {
    if (sameUV) {
      if (vertices.IsBound(pBegin)) 
	ip1 = vertices.Find(pBegin);
      else 
	ip1 = vertices.Find(pEnd);
      ip2 = ip1;
    }
    else {
      ip1 = vertices.Find(pBegin);
      ip2 = vertices.Find(pEnd);
    }
    
    theUV = FindUV(pBegin, uvFirst, ip1);
    
    v1.Initialize(theUV, nbDomains, ip1, MeshDS_Frontier);
    iv1 = structure->IndexOf(v1);
    if (iv1 == 0) {
      iv1=structure->AddNode(v1);
      mylistver.Append(v1);
    }
    
    theUV = FindUV(pEnd, uvLast, ip2);

    vl.Initialize(theUV, nbDomains, ip2, MeshDS_Frontier);
    ivl=structure->IndexOf(vl);
    if (ivl == 0) {
      ivl = structure->AddNode(vl);
      mylistver.Append(vl);
    }
    
    // recuperation du polygone:
    const TColStd_ListOfTransient& L = edges.Find(edge);
    const Handle(Poly_PolygonOnTriangulation)& P = 
      *(Handle(Poly_PolygonOnTriangulation)*)&(L.First());
    const TColStd_Array1OfInteger& NOD = P->Nodes();
    Handle(TColStd_HArray1OfReal) Par = P->Parameters();

    if (!BRep_Tool::Degenerated(edge)) {
      // creation du nouveau:
      Standard_Integer nbnodes = NOD.Length();
      TColStd_Array1OfInteger Nodes(1, nbnodes);
      
      isv1 = myvemap.FindIndex(iv1);
      if (isv1 == 0) isv1 = myvemap.Add(iv1);
      Nodes(1) = isv1;
      
      if (nbnodes > 2) {
//	Standard_Real pf, pl;
	Handle(Geom2d_Curve) C2d;
//	C2d = BRep_Tool::CurveOnSurface(edge, face, pf, pl);
	
	Standard_Real wCur, wCurFound, deltaW;
	Standard_Real wFold = Par->Value(Par->Lower());
	Standard_Real wLold = Par->Value(Par->Upper());
	Standard_Real wKoef = 1.;
	if ((wFold != wFirst || wLold != wLast) && wLold != wFold) {
	  wKoef = (wLast - wFirst) / (wLold - wFold);
	}
	
	wCur      = wFirst;
	wCurFound = wFirst;
	BRepAdaptor_Curve cons(edge, face);
	
	for (Standard_Integer i = 2; i <= nbnodes-1; i++) {
	  deltaW =  wCur;
	  Standard_Real monw = Par->Value(i);
	  wCur   = (wFirst+(monw-wFold)*wKoef);
	  deltaW =  wCur-deltaW;
	  BRepMeshAdapt_ShapeTool::Locate(cons,
				     wCurFound+deltaW,
				     wCurFound, 
				     Location3d(NOD(i)),
				     uv);
	  

// utiliser cette ligne uniquement pour les pieces sameparameter.
//	  uv = C2d->Value(Par->Value(i));

	  v2.Initialize(uv.Coord(), nbDomains, NOD(i), MeshDS_OnCurve);
	  
	  iv2 = structure->IndexOf(v2);
	  if (iv2 == 0) {
	    iv2 = structure->AddNode(v2);
	    mylistver.Append(v2);
	  }
	  
	  isv = myvemap.FindIndex(iv2);
	  if (isv == 0) isv = myvemap.Add(iv2);
	  Nodes(i) = isv;
	  
	  if (orEdge==TopAbs_FORWARD)
	    structure->AddLink(BRepMeshAdapt_Edge(iv1, iv2, MeshDS_Frontier, nbDomains));
	  else if (orEdge == TopAbs_REVERSED)
	    structure->AddLink(BRepMeshAdapt_Edge(iv2, iv1, MeshDS_Frontier, nbDomains));
	  else if (orEdge == TopAbs_INTERNAL)
	    structure->AddLink(BRepMeshAdapt_Edge(iv1, iv2, MeshDS_Fixed, nbDomains));
	  
	  iv1 = iv2;
	}
      }
      isvl = myvemap.FindIndex(ivl);
      if (isvl == 0) isvl = myvemap.Add(ivl);
      Nodes(nbnodes) = isvl;
      
      Poly = new Poly_PolygonOnTriangulation(Nodes, Par->Array1());
    }
    else {
      // creation du nouveau:
      TColStd_Array1OfInteger Nodes(1, 2);
      
      isv1 = myvemap.FindIndex(iv1);
      if (isv1 == 0) isv1 = myvemap.Add(iv1);
      Nodes(1) = isv1;
      
      isvl = myvemap.FindIndex(ivl);
      if (isvl == 0) isvl = myvemap.Add(ivl);
      Nodes(2) = isvl;
      
      Poly = new Poly_PolygonOnTriangulation(Nodes, Par->Array1());
    }
    
    if (iv1 != ivl) {
      if (orEdge == TopAbs_FORWARD) 
	structure->AddLink(BRepMeshAdapt_Edge(iv1, ivl, MeshDS_Frontier, nbDomains));
      else if (orEdge == TopAbs_REVERSED)
	structure->AddLink(BRepMeshAdapt_Edge(ivl, iv1, MeshDS_Frontier, nbDomains));
      else if (orEdge == TopAbs_INTERNAL)
	structure->AddLink(BRepMeshAdapt_Edge(iv1, ivl, MeshDS_Fixed, nbDomains));
    }
  }

  Poly->Deflection(defedge);
  if (internaledges.IsBound(edge)) {
    TColStd_ListOfTransient& L = internaledges.ChangeFind(edge);
    L.Append(Poly);
  }
  else {
    TColStd_ListOfTransient L1;
    L1.Append(Poly);
    internaledges.Bind(edge, L1);
  }
 }
}


//=======================================================================
//function : Update(edge)
//purpose  :
//=======================================================================
Standard_Boolean BRepMeshAdapt_Discret::Update(const TopoDS_Edge&  edge,
					  const TopoDS_Face&  face,
					  const Standard_Real defedge)
{

  TopLoc_Location l;
  Standard_Integer i = 1;
  Handle(Poly_Triangulation) T, TNull;
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;
  Standard_Boolean found = Standard_False;
//  Standard_Real tol;
  do {
    BRep_Tool::PolygonOnTriangulation(edge,Poly,T,l,i);
    i++;
    if (!found && !T.IsNull() && T->HasUVNodes() && 
	!Poly.IsNull() && Poly->HasParameters()) { 
      if (Poly->Deflection() <= (defedge*1.001)) {
	const TColgp_Array1OfPnt& Nodes        = T->Nodes();
	const TColStd_Array1OfInteger& Indices = Poly->Nodes();
	Handle(TColStd_HArray1OfReal) Param    = Poly->Parameters();
	Handle(Poly_PolygonOnTriangulation) P1, P2;

	TopAbs_Orientation orEdge = edge.Orientation();
	Standard_Integer nbNodes = Indices.Length();
	Standard_Integer ip1, ip2, ipl,  iv1, iv2, ivl;//,iv;
	Standard_Integer isv1, isv, isvl;
//	Standard_Real w;
	TopoDS_Vertex pBegin, pEnd;
	BRepMeshAdapt_Vertex v, v1, vl;
//	gp_Pnt P;
	gp_Pnt2d uv, uvFirst, uvLast;
	gp_XY theUV;
	Standard_Real wFirst, wLast;
	BRep_Tool::Range(edge, face, wFirst, wLast);

	BRep_Tool::UVPoints(edge, face, uvFirst, uvLast);

	Standard_Boolean sameUV = uvFirst.IsEqual(uvLast, Precision::PConfusion());
	TopExp::Vertices(edge,pBegin,pEnd);

	if (!edges.IsBound(edge)) {
	  if (!vertices.IsBound(pBegin)) {
	    if (sameUV && vertices.IsBound(pEnd)) {
	      ip1 = vertices.Find(pEnd);
	    }
	    else {
	      nbLocat++;
	      ip1 = nbLocat;
	      Location3d.Bind(ip1,Nodes(Indices(1)).Transformed(l.Transformation()));
	      vertices.Bind(pBegin,ip1);
	    }
	  } 
	  else {
	    ip1 = vertices.Find(pBegin);
	  }

	  theUV = FindUV(pBegin, uvFirst, ip1);

	  v1.Initialize(theUV,nbDomains,ip1,MeshDS_Frontier);
	  iv1 = structure->IndexOf(v1);
	  if (iv1 == 0) {
	    iv1 = structure->AddNode(v1);
	    mylistver.Append(v1);
	  }
	  
	  if (!sameUV) {
	    if (!vertices.IsBound(pEnd)) {
	      nbLocat++;
	      ipl = nbLocat;
	      Location3d.Bind(ipl,Nodes(Indices(nbNodes)).Transformed(l.Transformation()));
	      vertices.Bind(pEnd,ipl);
	    }
	    else {
	      ipl = vertices.Find(pEnd);
	    }

	    theUV = FindUV(pEnd, uvLast, ipl);

	    vl.Initialize(theUV,nbDomains,ipl,MeshDS_Frontier);
	    ivl = structure->IndexOf(vl);
	    if (ivl == 0) {
	      ivl = structure->AddNode(vl);
	      mylistver.Append(vl);
	    }
	  }
	  else {
	    ivl = iv1;
	    vl  = v1;
	  }
	  
	  Standard_Integer nbnodes = Indices.Length();
	  TColStd_Array1OfInteger NewNodes(1, nbnodes);
	  TColStd_Array1OfInteger NewNodInStruct(1, nbnodes);
	  
	  // traitement du 1er point:
	  isv1 = myvemap.FindIndex(iv1);
	  if (isv1 == 0) isv1 = myvemap.Add(iv1);
	  NewNodes(1) = isv1;
	  NewNodInStruct(1) = ip1;
	  
	  Handle(Geom2d_Curve) C2d;
//	  Standard_Real pf, pl;
//	  C2d = BRep_Tool::CurveOnSurface(edge, face, pf, pl);

	  Standard_Real wCur, wCurFound, deltaW;
	  Standard_Real wFold = Param->Value(Param->Lower());
	  Standard_Real wLold = Param->Value(Param->Upper());
	  Standard_Real wKoef = 1.;
	  if ((wFold != wFirst || wLold != wLast) && wLold != wFold) {
	    wKoef = (wLast - wFirst) / (wLold - wFold);
	  }
	  
	  wCur      = wFirst;
	  wCurFound = wFirst;
	  BRepAdaptor_Curve cons(edge, face);

	  for (i = 2; i <= Indices.Length()-1; i++) {
	    nbLocat++;
	    ip2 = nbLocat;
	    gp_Pnt P3d = (Nodes(Indices(i)).Transformed(l.Transformation()));
	    Location3d.Bind(ip2, P3d);


	    deltaW =  wCur;
	    Standard_Real monw = Param->Value(i);
	    wCur   = (wFirst+(monw-wFold)*wKoef);
	    deltaW =  wCur-deltaW;
	    BRepMeshAdapt_ShapeTool::Locate(cons,
				       wCurFound+deltaW,
				       wCurFound, 
				       P3d, uv);
	    

// utiliser cette ligne uniquement pour les pieces sameparameter.
//	    uv = C2d->Value(Param->Value(i));
	    v.Initialize(uv.Coord(), nbDomains, ip2, MeshDS_Frontier);
	    iv2 = structure->AddNode(v);
	    mylistver.Append(v);
	    
	    isv = myvemap.FindIndex(iv2);
	    if (isv == 0) isv = myvemap.Add(iv2);
	    NewNodes(i) = isv;
	    NewNodInStruct(i) = ip2;
	    
	    if (orEdge == TopAbs_FORWARD)
	      structure->AddLink(BRepMeshAdapt_Edge(iv1,iv2,MeshDS_Frontier,nbDomains));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMeshAdapt_Edge(iv2,iv1,MeshDS_Frontier,nbDomains));
	    else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMeshAdapt_Edge(iv1,iv2,MeshDS_Fixed,nbDomains));
	    iv1 = iv2;
	  }
	  // dernier point:
	  isvl = myvemap.FindIndex(ivl);
	  if (isvl == 0) isvl = myvemap.Add(ivl);
	  NewNodes(nbnodes) = isvl;
	  NewNodInStruct(nbnodes) = ipl;
	  
	  if (iv1 != ivl) {
	    if (orEdge == TopAbs_FORWARD)
	      structure->AddLink(BRepMeshAdapt_Edge(iv1,ivl,MeshDS_Frontier,nbDomains));
	    else if (orEdge == TopAbs_REVERSED)
	      structure->AddLink(BRepMeshAdapt_Edge(ivl,iv1,MeshDS_Frontier,nbDomains));
	    else if (orEdge == TopAbs_INTERNAL)
	      structure->AddLink(BRepMeshAdapt_Edge(iv1,ivl,MeshDS_Fixed,nbDomains));
	  }
	  
	  P1 = new Poly_PolygonOnTriangulation(NewNodes, Param->Array1());
	  P1->Deflection(defedge);
	  if (internaledges.IsBound(edge)) {
	    TColStd_ListOfTransient& L = internaledges.ChangeFind(edge);
	    L.Append(P1);
	  }
	  else {
	    TColStd_ListOfTransient L1;
	    L1.Append(P1);
	    internaledges.Bind(edge, L1);
	  }
	  
	  P2 = new Poly_PolygonOnTriangulation(NewNodInStruct, Param->Array1());
	  P2->Deflection(defedge);
	  TColStd_ListOfTransient L;
	  L.Append(P2);
	  edges.Bind(edge, L);
	

	  found = Standard_True;
	}
      }
      else {
	BRep_Builder B;
	B.UpdateEdge(edge,NullPoly,T,l);
	B.UpdateFace(face,TNull);
      }
    }
    else if (!T.IsNull() && !T->HasUVNodes()) {
      BRep_Builder B;
      B.UpdateEdge(edge,NullPoly,T,l);
      B.UpdateFace(face,TNull);
    }
    
  } while (!Poly.IsNull() && !found);

  return found;
}



//=======================================================================
//function : InternalVertices
//purpose  : 
//=======================================================================
void BRepMeshAdapt_Discret::InternalVertices
(BRepMeshAdapt_Delaun&                    trigu,
 const Handle(BRepAdaptor_HSurface)& caro,
 BRepMeshAdapt_Array1OfVertexOfDelaun&    FrontierV,
 BRepMeshAdapt_ListOfVertex&              InternalV,
 const Standard_Real                 defface)
{
  if (FrontierV.Length()>0) {
    gp_Pnt p3d;

    BRepMeshAdapt_GeomTool::D0(caro, myumin, myvmin, p3d);
    MeshShape_SurfacePoint P1(myumin, myvmin, p3d.XYZ());
    BRepMeshAdapt_GeomTool::D0(caro, myumax, myvmin, p3d);
    MeshShape_SurfacePoint P2(myumax, myvmin, p3d.XYZ());
    BRepMeshAdapt_GeomTool::D0(caro, myumax, myvmax, p3d);
    MeshShape_SurfacePoint P3(myumax, myvmax, p3d.XYZ());
    BRepMeshAdapt_GeomTool::D0(caro, myumin, myvmax, p3d);
    MeshShape_SurfacePoint P4(myumin, myvmax, p3d.XYZ());
    MeshShape_ListOfSurfacePoint pntsOnSurf;

    mymap.Clear();
    QuadCut  (caro, P1, P2, P3, P4, pntsOnSurf, defface);

    BRepMeshAdapt_Vertex newV;
    MeshShape_ListIteratorOfListOfSurfacePoint itP(pntsOnSurf);

    for (; itP.More(); itP.Next()) {
      newV.Initialize(itP.Value().UV(), nbDomains, 0, MeshDS_OnSurface);
      if (trigu.TriangleContaining(newV)!=0) {
	nbLocat++;
	Location3d.Bind(nbLocat, itP.Value().Coord());
	newV.Initialize(itP.Value().UV(), nbDomains, nbLocat,
			MeshDS_OnSurface);
	InternalV.Append(newV);
      }
    }
  }
}


//=======================================================================
//function : Control
//purpose  : 
//=======================================================================
Standard_Boolean BRepMeshAdapt_Discret::Control
  (const Handle(BRepAdaptor_HSurface)& caro,
   const Standard_Real                 defface,
   BRepMeshAdapt_ListOfVertex&              InternalV,
   TColStd_ListOfInteger&              badTriangles,
   TColStd_ListOfInteger&              nulTriangles,
   const Standard_Boolean              isfirst)
{
  Standard_Boolean isInsertedOnEdge, allDegenerated = Standard_True;
  Standard_Integer v1, v2, v3, e1, e2, e3;
  Standard_Boolean o1, o2, o3;
  gp_XY mi2d1, mi2d2, mi2d3, mi2dtri, vec2d1;
  gp_XYZ vecEd1, vecEd2, vecEd3;
  gp_Pnt pOnE1, pOnE2, pOnE3, pOnTri;
  BRepMeshAdapt_Vertex midV;
  Standard_Real dipo, dm, dv, d1, d2, d3;
  Standard_Real defl1, defl2, defl3, defTri;
  TColStd_MapOfInteger usedEdges;

  InternalV.Clear();
  badTriangles.Clear();

  TColStd_MapIteratorOfMapOfInteger triDom(structure->ElemOfDomain(nbDomains));

  for (; triDom.More(); triDom.Next()) {
    const BRepMeshAdapt_Triangle& curTri=Triangle(triDom.Key());
    if (curTri.Movability()!=MeshDS_Deleted) {
      curTri.Edges(e1, e2, e3, o1, o2, o3);
      const BRepMeshAdapt_Edge& edg1=Edge(e1);
      const BRepMeshAdapt_Edge& edg2=Edge(e2);
      const BRepMeshAdapt_Edge& edg3=Edge(e3);

      if (usedEdges.Contains(e1) && 
	  usedEdges.Contains(e2) && usedEdges.Contains(e3)) break;

      if (o1) {
	v1=edg1.FirstNode();
	v2=edg1.LastNode();
      }
      else {
	v1=edg1.LastNode();
	v2=edg1.FirstNode();
      }
      if (o2)
	v3=edg2.LastNode();
      else
	v3=edg2.FirstNode();

      const BRepMeshAdapt_Vertex& vert1=Vertex(v1);
      const BRepMeshAdapt_Vertex& vert2=Vertex(v2);
      const BRepMeshAdapt_Vertex& vert3=Vertex(v3);

      const gp_XY& xy1 = vert1.Coord();
      const gp_XY& xy2 = vert2.Coord();
      const gp_XY& xy3 = vert3.Coord();

      mi2d1.SetCoord((xy2.X()+xy3.X())/2., 
		     (xy2.Y()+xy3.Y())/2.);
      mi2d2.SetCoord((xy1.X()+xy3.X())/2., 
		     (xy1.Y()+xy3.Y())/2.);
      mi2d3.SetCoord((xy1.X()+xy2.X())/2., 
		     (xy1.Y()+xy2.Y())/2.);

      // Check for null triangle in 2d
      vec2d1=mi2d1-xy1;
      dv=Abs((xy2-xy1)^(vec2d1));
      dm=Abs((mi2d2-xy1)^(vec2d1));
      if (dm==0. || dv==0.)
      {
	nulTriangles.Append(triDom.Key());
        continue;
      }

      mi2dtri.SetCoord((xy1.X()+xy2.X()+xy3.X())/3.,  
                       (xy1.Y()+xy2.Y()+xy3.Y())/3.);
      BRepMeshAdapt_GeomTool::D0(caro, mi2dtri.X(), mi2dtri.Y(), pOnTri);

      const gp_XYZ& p1=Location3d(vert1.Location3d()).Coord();
      const gp_XYZ& p2=Location3d(vert2.Location3d()).Coord();
      const gp_XYZ& p3=Location3d(vert3.Location3d()).Coord();

      vecEd1=p2-p1;
      vecEd2=p3-p2;
      vecEd3=p1-p3;
      d1=vecEd1.SquareModulus();
      d2=vecEd2.SquareModulus();
      d3=vecEd3.SquareModulus();

      if (d1==0. || d2==0. || d3==0.)
      {
        nulTriangles.Append(triDom.Key());
        continue;
      }

      allDegenerated = Standard_False;

      gp_XYZ equa(vecEd1^vecEd2);
      dv=equa.Modulus();
      if (dv == 0.)
      {
        nulTriangles.Append(triDom.Key());
        continue;
      }

      isInsertedOnEdge = Standard_False;

      equa.SetCoord(equa.X()/dv, equa.Y()/dv, equa.Z()/dv);
      dipo=equa*p1;

      defTri=Abs((equa*pOnTri.XYZ())-dipo);
      BRepMeshAdapt_GeomTool::D0(caro, mi2d3.X(), mi2d3.Y(), pOnE1);
      defl1=Abs((equa*pOnE1.XYZ())-dipo);
      defTri=Max(defTri, defl1);
      BRepMeshAdapt_GeomTool::D0(caro, mi2d1.X(), mi2d1.Y(), pOnE2);
      defl2=Abs((equa*pOnE2.XYZ())-dipo);
      defTri=Max(defTri, defl2);
      BRepMeshAdapt_GeomTool::D0(caro, mi2d2.X(), mi2d2.Y(), pOnE3);
      defl3=Abs((equa*pOnE3.XYZ())-dipo);
      defTri=Max(defTri, defl3);

      if (defTri>defface)
      {
        d1 = sqrt(d1);
        d2 = sqrt(d2);
        d3 = sqrt(d3);
        dm=(d1+d2+d3)/3.5;

        if (d1>dm && defl1>defface && edg1.Movability()==MeshDS_Free)
        {
          isInsertedOnEdge = Standard_True;
          if (usedEdges.Add(e1))
          {
            if (isfirst) return Standard_True;
            nbLocat++;	
            Location3d.Bind(nbLocat, pOnE1);
            midV.Initialize(mi2d3, nbDomains, nbLocat, MeshDS_OnSurface);
            InternalV.Append(midV);
          }
        }

        if (d2>dm && defl2>defface && edg2.Movability()==MeshDS_Free)
        {
          isInsertedOnEdge = Standard_True;
          if (usedEdges.Add(e2))
          {
            if (isfirst) return Standard_True;
            nbLocat++;
            Location3d.Bind(nbLocat, pOnE2);
            midV.Initialize(mi2d1, nbDomains, nbLocat, MeshDS_OnSurface);
            InternalV.Append(midV);
          }
        }

        if (d3>dm && defl3>defface && edg3.Movability()==MeshDS_Free)
        {
          isInsertedOnEdge = Standard_True;
          if (usedEdges.Add(e3))
          {
            if (isfirst) return Standard_True;
            nbLocat++;
            Location3d.Bind(nbLocat, pOnE3);
            midV.Initialize(mi2d2, nbDomains, nbLocat, MeshDS_OnSurface);
            InternalV.Append(midV);
          }
        }

        if (!isInsertedOnEdge)
        {
          if (isfirst) return Standard_True;
          nbLocat++;
          Location3d.Bind(nbLocat, pOnTri);
          midV.Initialize(mi2dtri, nbDomains, nbLocat, MeshDS_OnSurface);
          InternalV.Append(midV);
        }

        badTriangles.Append(triDom.Key());
      }
    }
  }

  return (allDegenerated || !InternalV.IsEmpty());
}


//=======================================================================
//function : Plan
//purpose  : 
//=======================================================================
static Standard_Boolean Plan(const MeshShape_SurfacePoint& P1,
			     const MeshShape_SurfacePoint& P2,
			     const MeshShape_SurfacePoint& P3,
			     gp_XYZ&        eqPlan,
			     Standard_Real& diPolr)
{
  gp_XYZ v1(P2.Coord()-P1.Coord());
  if (v1.Modulus()>Precision::Confusion()) {
    gp_XYZ v2(P3.Coord()-P2.Coord());
    if (v2.Modulus()>Precision::Confusion()) {
      gp_XYZ v3(P1.Coord()-P3.Coord());
      if (v3.Modulus()>Precision::Confusion()) {
	eqPlan=v1^v2;
	if (eqPlan.Modulus()>0.) {
	  eqPlan.Normalize();
	  diPolr=P1.Coord()*eqPlan;
	  return Standard_True;
	}
      }
    }
  }
  diPolr=0.;
  eqPlan.SetCoord(0., 0., 0.);
  return Standard_False;
}


//=======================================================================
//function : Uindex
//purpose  : 
//=======================================================================

Standard_Integer BRepMeshAdapt_Discret::Uindex(const MeshShape_SurfacePoint& P) const
{
//#ifndef DEB 
  return (Standard_Integer) Round((P.UV().X()-myumin)/(myumax-myumin)*LIMITE_TRIANGULATION);
//#else
//  return Round((P.UV().X()-myumin)/(myumax-myumin)*LIMITE_TRIANGULATION);
//#endif
}


//=======================================================================
//function : Vindex
//purpose  : 
//=======================================================================

Standard_Integer BRepMeshAdapt_Discret::Vindex(const MeshShape_SurfacePoint& P) const
{
//#ifndef DEB
  return (Standard_Integer) Round((P.UV().Y()-myvmin)/(myvmax-myvmin)*LIMITE_TRIANGULATION);
//#else
//  return Round((P.UV().Y()-myvmin)/(myvmax-myvmin)*LIMITE_TRIANGULATION);
//#endif
}


//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::Append(MeshShape_ListOfSurfacePoint& pntsOnSurf,
			      const MeshShape_SurfacePoint& P)
{
  Standard_Integer IU = Uindex(P), IV = Vindex(P);

  // pas d append dans le cas d une frontiere.
  if ((IU != 0)                    && 
      (IV != 0)                    && 
      (IU != LIMITE_TRIANGULATION) && 
      (IV != LIMITE_TRIANGULATION)) {
    MeshShape_Couple C(IU, IV);
    if (!mymap.Contains(C)) {
      mymap.Add(C);
      pntsOnSurf.Append(P);
    }
  }
}


//=======================================================================
//function : QuadCut
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::QuadCut 
  (const Handle(BRepAdaptor_HSurface)& caro,
   const MeshShape_SurfacePoint&       P1,
   const MeshShape_SurfacePoint&       P2,
   const MeshShape_SurfacePoint&       P3,
   const MeshShape_SurfacePoint&       P4,
   MeshShape_ListOfSurfacePoint&       pntsOnSurf,
   const Standard_Real                 defface)
{

  Standard_Real tolerance=defface;
  
  Standard_Real Umin=P1.UV().X();
  Standard_Real Umax=P3.UV().X();
  Standard_Real Vmin=P1.UV().Y();
  Standard_Real Vmax=P3.UV().Y();
  Standard_Real dU=(Umax-Umin)/3.;
  Standard_Real dV=(Vmax-Vmin)/3.;
  Standard_Real dU4=(Umax-Umin)/4.;
  Standard_Real dV4=(Vmax-Vmin)/4.;
  Standard_Real U1234=(Umin+Umax)/2.;
  Standard_Real V1234=(Vmin+Vmax)/2.;

  gp_Pnt pOnSurf;

  BRepMeshAdapt_GeomTool::D0(caro, U1234, V1234, pOnSurf);
  MeshShape_SurfacePoint p1234(U1234, V1234, pOnSurf.XYZ());

  if (dU>Precision::PConfusion() && dV>Precision::PConfusion()) {
    BRepMeshAdapt_GeomTool::D0(caro, U1234, Vmin, pOnSurf);
    MeshShape_SurfacePoint p12(U1234, Vmin, pOnSurf.XYZ());
    BRepMeshAdapt_GeomTool::D0(caro, Umax, V1234, pOnSurf);
    MeshShape_SurfacePoint p23(Umax ,V1234 ,pOnSurf.XYZ());
    BRepMeshAdapt_GeomTool::D0(caro, U1234, Vmax, pOnSurf);
    MeshShape_SurfacePoint p34(U1234 ,Vmax ,pOnSurf.XYZ());
    BRepMeshAdapt_GeomTool::D0(caro, Umin, V1234, pOnSurf);
    MeshShape_SurfacePoint p14(Umin ,V1234 ,pOnSurf.XYZ());
    
    Standard_Real    dpol;
    Standard_Real    U, V, defle=0.;
    Standard_Boolean nulU=Standard_False;
    Standard_Boolean cutUmin=Standard_False;
    Standard_Boolean cutUmax=Standard_False;
    Standard_Boolean nulV=Standard_False;
    Standard_Boolean cutVmin=Standard_False;
    Standard_Boolean cutVmax=Standard_False;

    gp_XYZ           equaPlan1;
    if (Plan(P1, P2, p1234, equaPlan1, dpol)) {
      for (U=Umin+dU; U<Umax; U+=dU) {
	BRepMeshAdapt_GeomTool::D0(caro, U, V1234, pOnSurf);
        defle=Abs((pOnSurf.XYZ()*equaPlan1)-dpol);
        if (defle>=tolerance) {
	  cutUmin=Standard_True;
	  break;
	}
      }
      if (!cutUmin) {
	for (U=Umin+dU; U<Umax; U+=dU) {
	  BRepMeshAdapt_GeomTool::D0(caro, U, Vmin, pOnSurf);
	  defle=Abs((pOnSurf.XYZ()*equaPlan1)-dpol);
	  if (defle>=tolerance) {
	    cutUmin=Standard_True;
	    break;
	  }
	}
      }
      defle=Abs((p12.Coord()*equaPlan1)-dpol);
      if (defle>=tolerance) {
	cutUmin=Standard_True;
      }
      U = Umin+dU;
      V = Vmin+dV4;
      BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
      defle=Abs((pOnSurf.XYZ()*equaPlan1)-dpol);
      if (defle>=tolerance) {
	cutVmin=Standard_True;
      }
      U = Umax-dU;
      V = Vmin+dV4;
      BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
      defle=Abs((pOnSurf.XYZ()*equaPlan1)-dpol);
      if (defle>=tolerance) {
	cutVmax=Standard_True;
      }
    }
    else {
      if ((P1.Coord()-p12.Coord()).Modulus()>Precision::Confusion())
        cutUmin=Standard_True;
      else
        nulU=Standard_True;
    }
    
    gp_XYZ           equaPlan2;
    if (Plan(P3, P4, p1234, equaPlan2, dpol)) {
      for (U=Umin+dU; U<Umax; U+=dU) {
	BRepMeshAdapt_GeomTool::D0(caro, U, V1234, pOnSurf);
        defle=Abs((pOnSurf.XYZ()*equaPlan2)-dpol);
        if (defle>=tolerance) {
	  cutUmax=Standard_True;
	  break;
	}
      }
      if (!cutUmax) {
	for (U=Umin+dU; U<Umax; U+=dU) {
	  BRepMeshAdapt_GeomTool::D0(caro, U, Vmax, pOnSurf);
	  defle=Abs((pOnSurf.XYZ()*equaPlan2)-dpol);
	  if (defle>=tolerance) {
	    cutUmax=Standard_True;
	    break;
	  }
        }
	if (!cutUmax) {
	  defle=Abs((p34.Coord()*equaPlan2)-dpol);
	  if (defle>=tolerance) {
	    cutUmax=Standard_True;
	  }
	}
      }
      if (!cutVmin) {
	U = Umin+dU;
	V = Vmax-dV4;
	BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
	defle=Abs((pOnSurf.XYZ()*equaPlan2)-dpol);
	if (defle>=tolerance) {
	  cutVmin=Standard_True;
	}
      }
      if (!cutVmax) {
	U = Umax-dU;
	V = Vmax-dV4;
	BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
	defle=Abs((pOnSurf.XYZ()*equaPlan2)-dpol);
	if (defle>=tolerance) {
	  cutVmax=Standard_True;
	}
      }
    }
    else {
      if ((P3.Coord()-p34.Coord()).Modulus()>Precision::Confusion())
        cutUmax=Standard_True;
      else if (nulU) {
        cutVmin=Standard_True;
        cutVmax=Standard_True;
      }
    }
    
    gp_XYZ           equaPlan3;
    if (Plan(P4, P1, p1234, equaPlan3, dpol)) {
      for (V=Vmin+dV; V<Vmax; V+=dV) {
	BRepMeshAdapt_GeomTool::D0(caro, U1234, V, pOnSurf);
        defle=Abs((pOnSurf.XYZ()*equaPlan3)-dpol);
        if (defle>=tolerance) {
	  cutVmin=Standard_True;
	  break;
	}
        }
      if (!cutVmin) {
	for (V=Vmin+dV; V<Vmax; V+=dV) {
	  BRepMeshAdapt_GeomTool::D0(caro, Umin, V, pOnSurf);
	  defle=Abs((pOnSurf.XYZ()*equaPlan3)-dpol);
	  if (defle>=tolerance) {
	    cutVmin=Standard_True;
	    break;
	  }
	}
	if (!cutVmin) {
	  defle=Abs((p14.Coord()*equaPlan3)-dpol);
	  if (defle>=tolerance) {
	    cutVmin=Standard_True;
	  }
	}
      }
      if (!cutUmin) {
	U = Umin+dU4;
	V = Vmin+dV;
	BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
	defle=Abs((pOnSurf.XYZ()*equaPlan3)-dpol);
	if (defle>=tolerance) {
	  cutUmin=Standard_True;
	}
      }
      if (!cutUmax) {
	U = Umin+dU4;
	V = Vmax-dV;
	BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
	defle=Abs((pOnSurf.XYZ()*equaPlan3)-dpol);
	if (defle>=tolerance) {
	  cutUmax=Standard_True;
	}
      }
    }
    else {
      if ((P1.Coord()-p14.Coord()).Modulus()>Precision::Confusion())
	cutVmin=Standard_True;
      else 
	nulV=Standard_True;
    }
      
    gp_XYZ           equaPlan4;
    if (Plan(P2, P3, p1234, equaPlan4, dpol)) {
      for (V=Vmin+dV; V<Vmax; V+=dV) {
	BRepMeshAdapt_GeomTool::D0(caro, U1234, V, pOnSurf);
        defle=Abs((pOnSurf.XYZ()*equaPlan4)-dpol);
        if (defle>=tolerance) {
	  cutVmax=Standard_True;
	  break;
	}
      }
      if (!cutVmax) {
	for (V=Vmin+dV; V<Vmax; V+=dV) {
	  BRepMeshAdapt_GeomTool::D0(caro, Umax, V, pOnSurf);
	  defle=Abs((pOnSurf.XYZ()*equaPlan4)-dpol);
	  if (defle>=tolerance) {
	    cutVmax=Standard_True;
	    break;
	  }
	}
	if (!cutVmax) {
	  defle=Abs((p23.Coord()*equaPlan4)-dpol);
	  if (defle>=tolerance) {
	    cutVmax=Standard_True;
	  }
	}
      }
      if (!cutUmin) {
	U=Umax-dU4;
	V=Vmin+dV; 
	BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
	defle=Abs((pOnSurf.XYZ()*equaPlan4)-dpol);
	if (defle>=tolerance) {
	  cutUmin=Standard_True;
	}
      }
      if (!cutUmax) {
	U=Umax-dU4;
	V=Vmax-dV;
	BRepMeshAdapt_GeomTool::D0(caro, U, V, pOnSurf);
	defle=Abs((pOnSurf.XYZ()*equaPlan4)-dpol);
	if (defle>=tolerance) {
	  cutUmax=Standard_True;
	}
      }
    }
    else {
      if ((P2.Coord()-p23.Coord()).Modulus()>Precision::Confusion())
	cutVmax=Standard_True;
      else if (nulV) {
	cutUmin=Standard_True;
	cutUmax=Standard_True;
      }
    }
     
//  Le découpage est fonction des valeurs de <cut*> :
//
// V^             
//  |   +---+---+          cutUmin correspond au découpage entre a et b
//  |   | c | d |          cutUmax correspond au découpage entre c et d
//  |   +---+---+          cutVmin correspond au découpage entre a et c
//  |   | a | b |          cutVmax correspond au découpage entre b et d
//  |   +---+---+
//  +--------------->
//                  U
//
//  a  : QuadCut(caro, P1, p12, p1234, p14, pntsOnSurf);
//  b  : QuadCut(caro, p12, P2, p23, p1234, pntsOnSurf);
//  c  : QuadCut(caro, p14, p1234, p34, P4, pntsOnSurf);
//  d  : QuadCut(caro, p1234, p23, P3, p34, pntsOnSurf);
//  ac : QuadCut(caro, P1, p12, p34, P4, pntsOnSurf);
//  bd : QuadCut(caro, p12, P2, P3, p34, pntsOnSurf);
//  ab : QuadCut(caro, P1, P2, p23, p14, pntsOnSurf);
//  cd : QuadCut(caro, p14, p23, P3, P4, pntsOnSurf);

    if (cutUmin && cutUmax) {
      if (cutVmin && cutVmax) {
        QuadCut(caro, P1, p12, p1234, p14, pntsOnSurf, defface);  // a 
        QuadCut(caro, p12, P2, p23, p1234, pntsOnSurf, defface);  // b
        QuadCut(caro, p14, p1234, p34, P4, pntsOnSurf, defface);  // c
        QuadCut(caro, p1234, p23, P3, p34, pntsOnSurf, defface);  // d
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p23);
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p14);
	Append(pntsOnSurf, p1234);
      }
      else if (cutVmin) {
        QuadCut(caro, P1, p12, p1234, p14, pntsOnSurf, defface);  // a
        QuadCut(caro, p14, p1234, p34, P4, pntsOnSurf, defface);  // c
        QuadCut(caro, p12, P2, P3, p34, pntsOnSurf, defface);     // bd
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p14);
	Append(pntsOnSurf, p1234);
      }
      else if (cutVmax) {
        QuadCut(caro, P1, p12, p34, P4, pntsOnSurf, defface);     // ac
        QuadCut(caro, p12, P2, p23, p1234, pntsOnSurf, defface);  // b
        QuadCut(caro, p1234, p23, P3, p34, pntsOnSurf, defface);  // d
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p23);
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p1234);
      }
      else {
        QuadCut(caro, P1, p12, p34, P4, pntsOnSurf, defface);     // ac
        QuadCut(caro, p12, P2, P3, p34, pntsOnSurf, defface);     // bd
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p34);
      }
    }
    else if (cutUmin) {
      if (cutVmin && cutVmax) {
        QuadCut(caro, P1, p12, p1234, p14, pntsOnSurf, defface);  // a
        QuadCut(caro, p12, P2, p23, p1234, pntsOnSurf, defface);  // b
        QuadCut(caro, p14, p23, P3, P4, pntsOnSurf, defface);     // cd
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p23);
	Append(pntsOnSurf, p14);
	Append(pntsOnSurf, p1234);
      }
      else if (cutVmin) {
        QuadCut(caro, P1, p12, p1234, p14, pntsOnSurf, defface);  // a
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p1234);
      }
      else if (cutVmax)  {
        QuadCut(caro, p12, P2, p23, p1234, pntsOnSurf, defface);  // b
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p23);
	Append(pntsOnSurf, p1234);
      }
      else {
        QuadCut(caro, P1, p12, p34, P4, pntsOnSurf, defface);     // ac
        QuadCut(caro, p12, P2, P3, p34, pntsOnSurf, defface);     // bd
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p34);
      }
    }
    else if (cutUmax) {
      if (cutVmin && cutVmax) {
        QuadCut(caro, P1, P2, p23, p14, pntsOnSurf, defface);     // ab
        QuadCut(caro, p14, p1234, p34, P4, pntsOnSurf, defface);  // c
        QuadCut(caro, p1234, p23, P3, p34, pntsOnSurf, defface);  // d
	Append(pntsOnSurf, p23);
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p14);
	Append(pntsOnSurf, p1234);
      }
      else if (cutVmin) {
        QuadCut(caro, p14, p1234, p34, P4, pntsOnSurf, defface);  // c
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p14);
	Append(pntsOnSurf, p1234);
      }
      else if (cutVmax) {
        QuadCut(caro, p1234, p23, P3, p34, pntsOnSurf, defface);  // d
	Append(pntsOnSurf, p23);
	Append(pntsOnSurf, p34);
	Append(pntsOnSurf, p1234);
      }
      else {
        QuadCut(caro, P1, p12, p34, P4, pntsOnSurf, defface);     // ac
        QuadCut(caro, p12, P2, P3, p34, pntsOnSurf, defface);     // bd
	Append(pntsOnSurf, p12);
	Append(pntsOnSurf, p34);
      }
    }
    else if (cutVmin || cutVmax) {
      QuadCut(caro, P1, P2, p23, p14, pntsOnSurf, defface);       // ab
      QuadCut(caro, p14, p23, P3, P4, pntsOnSurf, defface);       // cd
      Append(pntsOnSurf, p23);
      Append(pntsOnSurf, p14);
    }
    else {
      Append(pntsOnSurf, p1234);
    }
  }
  else {
    Append(pntsOnSurf, p1234);
  }
}


//=======================================================================
//function : AddInShape
//purpose  : 
//=======================================================================
void BRepMeshAdapt_Discret::AddInShape(const TopoDS_Face&  face,
				  const Standard_Real defface)
{
//  gp_Pnt Pt;

  TColStd_MapIteratorOfMapOfInteger it;

  Standard_Integer e1, e2, e3, nTri;
  Standard_Integer v1, v2, v3, iv1, iv2, iv3;
  Standard_Integer i, index;
  Standard_Boolean o1, o2, o3;
  TopAbs_Orientation orFace = face.Orientation();

  const TColStd_MapOfInteger& TriMap = structure->ElemOfDomain(nbDomains);
  it.Initialize(TriMap);
    
  nTri = TriMap.Extent();

  if (nTri != 0) {
    
    Poly_Array1OfTriangle Tri(1, nTri);
    
    i = 1;
    
    for (; it.More(); it.Next()) {
      structure->GetElement(it.Key()).Edges(e1, e2, e3, o1, o2, o3);
      
      const BRepMeshAdapt_Edge& ve1 = structure->GetLink(e1);
      if (o1) { v1 = ve1.FirstNode(); v2 = ve1.LastNode(); }
      else    { v1 = ve1.LastNode() ; v2 = ve1.FirstNode();}
      
      const BRepMeshAdapt_Edge& ve2 = structure->GetLink(e2);
      if (o2) { v2 = ve2.FirstNode(); v3 = ve2.LastNode(); }
      else    { v2 = ve2.LastNode() ; v3 = ve2.FirstNode();}
      
      iv1 = myvemap.FindIndex(v1);
      if (iv1 == 0) iv1 = myvemap.Add(v1);
      iv2 = myvemap.FindIndex(v2);
      if (iv2 == 0) iv2 = myvemap.Add(v2);
      iv3 = myvemap.FindIndex(v3);
      if (iv3 == 0) iv3 = myvemap.Add(v3);
      
      if (orFace == TopAbs_REVERSED) Tri(i++).Set(iv1, iv3, iv2);
      else Tri(i++).Set(iv1, iv2, iv3);
    }
    
    Standard_Integer nbVertices = myvemap.Extent();
    Handle(Poly_Triangulation) T = new Poly_Triangulation(nbVertices, nTri, Standard_True);
    Poly_Array1OfTriangle& Trian = T->ChangeTriangles();
    Trian = Tri;
    TColgp_Array1OfPnt&  Nodes = T->ChangeNodes();
    TColgp_Array1OfPnt2d& Nodes2d = T->ChangeUVNodes();
    
    for (i = 1; i <= nbVertices; i++) {
      index = myvemap.FindKey(i);
      Nodes(i) = Pnt(index);
      Nodes2d(i).SetXY(Vertex(index).Coord());
    }
    
    T->Deflection(defface);
    
    // stockage de la triangulation dans la BRep.
    BRep_Builder B;
    TopLoc_Location l = face.Location();
    if (!l.IsIdentity()) {
      gp_Trsf tr = l.Transformation();
      tr.Invert();
      for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) 
	Nodes(i).Transform(tr);
    }
    B.UpdateFace(face, T);

    // mise en place des polygones sur triangulation dans la face:
    MeshShape_DataMapIteratorOfDataMapOfShapeListOfTransient It(internaledges);

    for (; It.More(); It.Next()) {
      if (It.Value().Extent() == 1) {
	const Handle(Poly_PolygonOnTriangulation)& NOD = 
	  *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().First()));
	B.UpdateEdge(TopoDS::Edge(It.Key()), NOD, T, l);
      }
      else {
	const Handle(Poly_PolygonOnTriangulation)& NOD1 = 
	  *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().First()));
	const Handle(Poly_PolygonOnTriangulation)& NOD2 = 
	  *((Handle(Poly_PolygonOnTriangulation)*)&(It.Value().Last()));
	B.UpdateEdge(TopoDS::Edge(It.Key()), NOD1, NOD2, T, l);
      }
    }
  }
}


//=======================================================================
//function : output
//purpose  : 
//=======================================================================
Standard_Integer BRepMeshAdapt_Discret::NbTriangles() const
{
  return structure->NbElements();
}

//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================

const BRepMeshAdapt_Triangle& BRepMeshAdapt_Discret::Triangle
  (const Standard_Integer Index) const
{
  return structure->GetElement(Index);
}

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer BRepMeshAdapt_Discret::NbEdges() const
{
  return structure->NbLinks();
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const BRepMeshAdapt_Edge& BRepMeshAdapt_Discret::Edge(const Standard_Integer Index) const
{
  return structure->GetLink(Index);
}

//=======================================================================
//function : NbVertices
//purpose  : 
//=======================================================================

Standard_Integer BRepMeshAdapt_Discret::NbVertices() const
{
  return structure->NbNodes();
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const BRepMeshAdapt_Vertex& BRepMeshAdapt_Discret::Vertex
  (const Standard_Integer Index) const
{
  return structure->GetNode(Index);
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& BRepMeshAdapt_Discret::Pnt(const Standard_Integer Index) const
{
  return Location3d(structure->GetNode(Index).Location3d());
}

//=======================================================================
//function : NbPoint3d
//purpose  : 
//=======================================================================

Standard_Integer BRepMeshAdapt_Discret::NbPoint3d() const
{
  return nbLocat;
}

//=======================================================================
//function : Point3d
//purpose  : 
//=======================================================================

const gp_Pnt& BRepMeshAdapt_Discret::Point3d(const Standard_Integer Index) const
{
  return Location3d(Index);
}

//=======================================================================
//function : Normal
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::Normal(const Standard_Integer Index,
				 gp_Pnt& Pon, gp_Dir& Nor) const
{
  const BRepMeshAdapt_Vertex& v=structure->GetNode(Index);
  BRepMeshAdapt_GeomTool::Normal(new BRepAdaptor_HSurface(Domains(v.Domain())), 
		   v.Coord().X(), v.Coord().Y(), Pon, Nor);
}

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

Handle(BRepMeshAdapt_DataStructureOfDelaun) BRepMeshAdapt_Discret::Result() const
{
  return structure;
}


//=======================================================================
//function : NbDomains
//purpose  : 
//=======================================================================

Standard_Integer BRepMeshAdapt_Discret::NbDomains() const 
{ return nbDomains;}

//=======================================================================
//function : DomainFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepMeshAdapt_Discret::DomainFace(const Standard_Integer Index) const 
{ 
  return Domains(Index); 
}

//=======================================================================
//function : VerticesOfDomain
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::VerticesOfDomain(const Standard_Integer Index,
					TColStd_MapOfInteger&  Indices) const 
{ 
  Indices.Clear();
  
  // recup de la map des edges.
  const TColStd_MapOfInteger& edmap = structure->LinkOfDomain(Index);

  // iterateur sur les edges.
  TColStd_MapIteratorOfMapOfInteger iter(edmap);
  Standard_Integer ind_edge;
  for (iter.Reset(); iter.More(); iter.Next()) {
    ind_edge = iter.Key();
    const BRepMeshAdapt_Edge& Ed = Edge(ind_edge);
    Indices.Add(Ed.FirstNode());
    Indices.Add(Ed.LastNode());
  }
}



//=======================================================================
//function : EdgesOfDomain
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::EdgesOfDomain(const Standard_Integer Index,
				     TColStd_MapOfInteger&  Indices) const 
{ 
  Indices = structure->LinkOfDomain(Index);
}


//=======================================================================
//function : TrianglesOfDomain
//purpose  : 
//=======================================================================

void BRepMeshAdapt_Discret::TrianglesOfDomain(const Standard_Integer Index,
				     TColStd_MapOfInteger&  Indices) const 
{ 
  Indices = structure->ElemOfDomain(Index);
}



//=======================================================================
//function : GetDeflection
//purpose  : 
//=======================================================================

Standard_Real BRepMeshAdapt_Discret::GetDeflection() const
{
  return deflection;
}


//=======================================================================
//function : GetAngle
//purpose  : 
//=======================================================================

Standard_Real BRepMeshAdapt_Discret::GetAngle() const
{
  return angle;
}


//=======================================================================
//function : FindUV
//purpose  : 
//=======================================================================

gp_XY BRepMeshAdapt_Discret::FindUV(const TopoDS_Vertex&   V,
			       const gp_Pnt2d&        XY, 
			       const Standard_Integer ip) 
{
  gp_XY theUV;
  Standard_Real tol = 10*BRep_Tool::Tolerance(V);

  if (mylocation2d.IsBound(ip)) {
    theUV = mylocation2d.Find(ip);
    if (!theUV.IsEqual(XY.Coord(), tol)) {
      theUV = XY.Coord();
    }
    
  }
  else {
    theUV = XY.Coord();
    mylocation2d.Bind(ip, theUV);
  }
  return theUV;
}

