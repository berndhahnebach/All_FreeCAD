// File:	BRepMeshAdapt_IncrementalMesh.cxx
// Created:	Tue Jun 20 10:34:51 1995
// Author:	Stagiaire Alain JOURDAIN
//		<ajo@phobox>


#include <BRepMeshAdapt_IncrementalMesh.ixx>
#include <BRepMeshAdapt_Edge.hxx>
#include <BRepMeshAdapt_Vertex.hxx>
#include <BRepMeshAdapt_Triangle.hxx>

#include <Bnd_Box.hxx>
#include <BRepLib.hxx>
#include <BRepBndLib.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Connect.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <BRepMeshAdapt.hxx>
#include <TopExp.hxx>
#include <GCPnts_TangentialDeflection.hxx>


//=======================================================================
//function : BRepMeshAdapt_IncrementalMesh
//purpose  : 
//=======================================================================
BRepMeshAdapt_IncrementalMesh::
BRepMeshAdapt_IncrementalMesh() :
myModified(Standard_False)
{
  mymapedge.Clear();
  myancestors.Clear();
}


//=======================================================================
//function : BRepMeshAdapt_IncrementalMesh
//purpose  : 
//=======================================================================

BRepMeshAdapt_IncrementalMesh::
BRepMeshAdapt_IncrementalMesh(const TopoDS_Shape& S,
			 const Standard_Real D,
			 const Standard_Boolean Rel,
			 const Standard_Real Ang) :
			 myDeflection(D),
			 myAngle(Ang),
			 myRelative(Rel),
			 myModified(Standard_False)
{
  mymapedge.Clear();
  myshape = S;
  myancestors.Clear();
  Bnd_Box B;
  BRepBndLib::Add(myshape, B);
  myBox = B;
  myMesh = new BRepMeshAdapt_FastDiscret(myDeflection,
				    myAngle,
				    B,
				    Standard_True,
				    Standard_True,
				    Rel,
				    Standard_True);
  Update(S);
}


//=======================================================================
//function : BRepMeshAdapt_IncrementalMesh //Own Constructor to fill param structure
//purpose  : 
//=======================================================================

BRepMeshAdapt_IncrementalMesh::BRepMeshAdapt_IncrementalMesh(
						const TopoDS_Shape& S,
                        const Standard_Real D,
                        const TriangleAdapt_Parameters& params,
                        const Standard_Boolean Rel,
                        const Standard_Real Ang) 
						:
						myDeflection(D),
						myAngle(Ang),
						myRelative(Rel),
						myModified(Standard_False)
{
   mymapedge.Clear();
   myshape = S;
   myancestors.Clear();
   Bnd_Box B;
   BRepBndLib::Add(myshape, B);
   myBox = B;
   myMesh = new BRepMeshAdapt_FastDiscret(
										myDeflection,
										myAngle,
										B,
										Standard_True,
										Standard_True,
										Rel,
										Standard_True);

   myMesh->SetTriangleParameters(params);
   Update(S);
}

//=======================================================================
//function : SetDeflection
//purpose  : 
//=======================================================================

void BRepMeshAdapt_IncrementalMesh::SetDeflection(const Standard_Real D)
{
  myDeflection = D;
  myRelative = Standard_False;
  myModified = Standard_False;
  myMesh = new BRepMeshAdapt_FastDiscret
    (myDeflection, myAngle, myBox,
     Standard_True, Standard_True, myRelative,Standard_True);
}

//=======================================================================
//function : SetAngle
//purpose  : 
//=======================================================================

void BRepMeshAdapt_IncrementalMesh::SetAngle(const Standard_Real Ang)
{
  myAngle = Ang;
  myModified = Standard_False;
  myMesh = new BRepMeshAdapt_FastDiscret
    (myDeflection, myAngle, myBox,
     Standard_True, Standard_True, myRelative,Standard_True);
}

//=======================================================================
//function : SetRatio
//purpose  : 
//=======================================================================

void BRepMeshAdapt_IncrementalMesh::SetRatio(const Standard_Real D)
{
  myDeflection = D;
  myRelative = Standard_True;
  myModified = Standard_False;
  myMesh = new BRepMeshAdapt_FastDiscret
    (myDeflection, myAngle, myBox,
     Standard_True, Standard_True, myRelative,Standard_True);
}

//=======================================================================
//function : IsModified
//purpose  : 
//=======================================================================

Standard_Boolean BRepMeshAdapt_IncrementalMesh::IsModified() const
{
  return myModified;
}

//=======================================================================
//function : Update(shape)
//purpose  : Builds the incremental mesh of the shape
//=======================================================================
void BRepMeshAdapt_IncrementalMesh::Update(const TopoDS_Shape& S)
				  
{
  myModified = Standard_False;
  TopExp_Explorer ex;

  Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
  Standard_Real dx, dy, dz;
  
  myBox.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
  dx = TXmax-TXmin;
  dy = TYmax-TYmin;
  dz = TZmax-TZmin;
  mydtotale = dx;
  if (dy > mydtotale) mydtotale = dy;
  if (dz > mydtotale) mydtotale = dz;
  
  for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next()) {
    Update(TopoDS::Edge(ex.Current()));
  }

  TopTools_ListOfShape LF;
  BRepLib::ReverseSortFaces(S,LF);

  TopTools_ListIteratorOfListOfShape it;
  for (it.Initialize(LF); it.More(); it.Next()) {
    Update(TopoDS::Face(it.Value()));
  }

 
  // maillage des edges non contenues dans les faces :
  Standard_Real f, l, defedge;
  Standard_Integer i, nbNodes;
  TopLoc_Location L;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  ex.Init(S ,TopAbs_EDGE, TopAbs_FACE);

  while (ex.More()) {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    if (myRelative) {
      Bnd_Box B;
      BRepBndLib::Add(E, B);
      B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      dx = aXmax-aXmin;
      dy = aYmax-aYmin;
      dz = aZmax-aZmin;
      defedge = dx;
      if (defedge < dy) defedge = dy;
      if (defedge < dz) defedge = dz;
      // ajustement par rapport a la taille totale:
      Standard_Real cdef = mydtotale/(2*defedge);
      if (cdef < 0.5) cdef = 0.5;
      if (cdef > 2.) cdef = 2.;
      defedge = cdef * defedge * myDeflection;
    }
    else defedge = myDeflection;
    
    Handle(Poly_Polygon3D) P3D = BRep_Tool::Polygon3D(E, L);
    Standard_Boolean maill = Standard_False;
    if (P3D.IsNull()) {
      maill = Standard_True;
    }
    else if (P3D->Deflection() > 1.1*defedge) {
      maill = Standard_True;
    }
    if (maill) {
      BRepAdaptor_Curve C(E);
      f = C.FirstParameter();
      l = C.LastParameter();
      
      GCPnts_TangentialDeflection TD(C, f, l, myAngle, defedge, 2);
      nbNodes = TD.NbPoints();
      
      TColgp_Array1OfPnt Nodes(1, nbNodes);
      TColStd_Array1OfReal UVNodes(1, nbNodes);
      for ( i = 1; i <= nbNodes; i++) {
	Nodes(i) = TD.Value(i);
	UVNodes(i) = TD.Parameter(i);
      }
      
      BRep_Builder B;
      Handle(Poly_Polygon3D) P = new Poly_Polygon3D(Nodes, UVNodes);
      P->Deflection(myDeflection);
      B.UpdateEdge(E, P);
    }

    ex.Next();
  }

}    


//=======================================================================
//function : Update(edge)
//purpose  : Locate a correct discretisation if it exists
//           Set no one otherwise
//=======================================================================
void BRepMeshAdapt_IncrementalMesh::Update(const TopoDS_Edge& E)
{
  TopLoc_Location l;
  Standard_Integer i = 1;
  Handle(Poly_Triangulation) T, TNull;
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;
  Standard_Boolean found = Standard_False;
  Standard_Real defedge;
  BRep_Builder B;
  Standard_Boolean defined = Standard_False;
  
  do {
    BRep_Tool::PolygonOnTriangulation(E, Poly, T, l, i);
    i++;
    if (!T.IsNull() && !Poly.IsNull()) {
      if (!defined) {
	if (myRelative) {
	  Bnd_Box B;
	  BRepBndLib::Add(E, B);
	  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
	  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	  dx = aXmax-aXmin;
	  dy = aYmax-aYmin;
	  dz = aZmax-aZmin;
	  defedge = dx;
	  if (defedge < dy) defedge = dy;
	  if (defedge < dz) defedge = dz;
	  // ajustement par rapport a la taille totale:
	  Standard_Real cdef = mydtotale/(2*defedge);
	  if (cdef < 0.5) cdef = 0.5;
	  if (cdef > 2.) cdef = 2.;
	  defedge = cdef * defedge * myDeflection;
	}
	else defedge = myDeflection;
	mymapedge.Bind(E, defedge);
	defined = Standard_True;
      }
      if ((!myRelative && Poly->Deflection() <= 1.1*defedge) ||
	  (myRelative && Poly->Deflection() <= 1.1*defedge)) 
	found = Standard_True;
      else {
	myModified = Standard_True;
	B.UpdateEdge(E, NullPoly, T, l);
      }
    }
  } while (!Poly.IsNull());

  if (!found) myMap.Add(E);
}


//=======================================================================
//function : Update(face)
//purpose  : If the face is not correctly triangulated, or if one of its
//           edges is to be discretisated correctly, the triangulation
//           of this face is built.
//=======================================================================
void  BRepMeshAdapt_IncrementalMesh::Update(const TopoDS_Face& F)
{
  TopLoc_Location l;
  Handle(Geom_Surface) SS = BRep_Tool::Surface(F, l);
  if (SS.IsNull()) return;

  //Standard_Integer i;
  Standard_Boolean WillBeTriangulated = Standard_False;
  Handle(Poly_Triangulation) T, TNull;
  T = BRep_Tool::Triangulation(F, l);
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;

  BRep_Builder B;
  TopExp_Explorer ex;
  
  Standard_Real defedge, defface;
  Standard_Integer nbEdge = 0;
  if (myRelative) {
    defface = 0.;
    
    for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
      nbEdge++;
      if (mymapedge.IsBound(edge)) {
	defedge = mymapedge(edge);
      }
      else {
	Bnd_Box B;
	BRepBndLib::Add(edge, B);
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
	B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	dx = aXmax-aXmin;
	dy = aYmax-aYmin;
	dz = aZmax-aZmin;
	defedge = dx;
	if (defedge < dy) defedge = dy;
	if (defedge < dz) defedge = dz;
	// ajustement par rapport a la taille totale:
	Standard_Real cdef = mydtotale/(2*defedge);
	if (cdef < 0.5) cdef = 0.5;
	if (cdef > 2.) cdef = 2.;
	defedge = cdef * defedge * myDeflection;
      }
      defface = defface + defedge;
    }
    if (nbEdge != 0) defface = defface / nbEdge;
    else             defface = myDeflection;
  }
  else
    defface = myDeflection;

  if (!T.IsNull()) {
    if ((!myRelative && T->Deflection() <= 1.1*defface) ||
	(myRelative && T->Deflection() <= 1.1*defface)) {
      for (ex.Init(F, TopAbs_EDGE);
	   ex.More();
	   ex.Next()) {
	const TopoDS_Shape& E = ex.Current();
	Poly = BRep_Tool::PolygonOnTriangulation(TopoDS::Edge(E), T, l);
	if (Poly.IsNull() || myMap.Contains(E)) {
	  WillBeTriangulated = Standard_True;
	  // cas un peu special. la triangulation est bonne, mais
	  // l'edge n'a pas de representation polygonalisee sur celle-ci.
	  break;
	}
      }
    } 
    else WillBeTriangulated = Standard_True;
  }

  if (WillBeTriangulated || T.IsNull()) {
    myModified = Standard_True;
    if (!T.IsNull()) {
      for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next()) {
	B.UpdateEdge(TopoDS::Edge(ex.Current()), NullPoly, T, l);
	myMap.Remove(ex.Current());
      }
      B.UpdateFace(F, TNull);
    }
    myMesh->Add(F);
    if (myMesh->CurrentFaceStatus() == BRepMeshAdapt_ReMesh) {
#ifdef DEB_MESH
      cout << " face remaillee + finement que prevu."<< endl;
#endif

      Standard_Integer index;

      myancestors.Clear();

      TopTools_ListOfShape empty;
      TopExp_Explorer exf, exs, exe;
      
      for (exf.Init(myshape, TopAbs_FACE); exf.More(); exf.Next()) {
	TopoDS_Face F2 = TopoDS::Face(exf.Current());
	
	for (exs.Init(F2, TopAbs_EDGE); exs.More(); exs.Next()) {
	  TopoDS_Edge E2 = TopoDS::Edge(exs.Current());
	  
	  for (exe.Init(F, TopAbs_EDGE); exe.More(); exe.Next()) {
	    TopoDS_Edge E = TopoDS::Edge(exe.Current());
	    if (E2.IsPartner(E)) {
	      index = myancestors.FindIndex(E);
	      if (index == 0) index = myancestors.Add(E,empty);
	      myancestors(index).Append(F2);
	    }
	  }
	}
      }

      TopTools_MapOfShape MShape;
      MShape.Add(F);

      for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next()) {
	const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
	index = myancestors.FindIndex(edge);
	if (index != 0) {
	  const TopTools_ListOfShape& L = myancestors.FindFromKey(edge);
	  
	  TopTools_ListIteratorOfListOfShape it(L);
	  
	  for (; it.More(); it.Next()) {
	    TopoDS_Face F2 = TopoDS::Face(it.Value());
	    if (!MShape.Contains(F2)) {
	      MShape.Add(F2);
	      T = BRep_Tool::Triangulation(F2, l);	  
	      if (!T.IsNull()) {
#ifdef DEB_MESH
		cout <<"triangulation a refaire" <<endl;
#endif
		for (exe.Init(F2, TopAbs_EDGE); exe.More(); exe.Next()) {
		  TopoDS_Edge E2 = TopoDS::Edge(exe.Current());
		  B.UpdateEdge(E2, NullPoly, T, l);
		}
		B.UpdateFace(F2, TNull);
		myMesh->Add(F2);
	      }
	    }
	  }
	}
      }

    }
  }
}

