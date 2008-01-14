#include "PreCompiled.h"
#include "ShapeTriangulator.h"
#include <Base/Builder3D.h>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <Mod/Mesh/App/Core/Builder.h>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


ShapeTriangulator::ShapeTriangulator(const TopoDS_Shape& aShape)
:m_Shape(aShape)
{
	m_EdgeStruct.clear();
	m_FacePointsMap.clear();
}

ShapeTriangulator::~ShapeTriangulator()
{
}

bool ShapeTriangulator::FillUpFacePointsMap()
{
	TopExp_Explorer anExplorer;
	for(anExplorer.Init(m_Shape,TopAbs_FACE);anExplorer.More();anExplorer.Next())
	{
		//Get Current Face
		TopoDS_Face aFace;
		aFace = TopoDS::Face(anExplorer.Current());
		Handle_Poly_Triangulation Tr;
		TopLoc_Location theLocation;
		Tr = BRep_Tool::Triangulation(aFace, theLocation);
		const TColgp_Array1OfPnt& Nodes3d = Tr->Nodes(); // 3d nodes for the points
		const TColgp_Array1OfPnt2d& Nodes2d = Tr->UVNodes(); //2d nodes for the points
		//Now fill up map FacePointsMap
		int j=Nodes3d.Length();
		j=Nodes2d.Length();
		std::pair<gp_Pnt,gp_Pnt2d> aPointPair;
		std::map<gp_Pnt,gp_Pnt2d,gp_PntLess> aPointMap;
		aPointMap.clear();
		for(int i=1;i<=Nodes3d.Length();++i)
		{
			aPointPair.first = Nodes3d(i); 
			aPointPair.second = Nodes2d(i);
			aPointMap.insert(aPointPair);
		}
		std::pair<TopoDS_Face,std::map<gp_Pnt,gp_Pnt2d,gp_PntLess> > aFacePointPair;
		aFacePointPair.first = aFace; aFacePointPair.second = aPointMap;
		m_FacePointsMap.insert(aFacePointPair);
	}	
		
	return true;
}

bool ShapeTriangulator::GenerateFreeCADMesh()
{
	
	TopExp_Explorer anExplorer;
	TopoDS_Face aFace;
	TopLoc_Location aLocation;
	MeshCore::MeshBuilder aMeshBuilder(m_Mesh);
	aMeshBuilder.Initialize(1000);
	Base::Vector3f Points[3];
	for(anExplorer.Init(m_Shape,TopAbs_FACE);anExplorer.More();anExplorer.Next())
	{
		aFace = TopoDS::Face(anExplorer.Current());
		// takes the triangulation of the face aFace:
		Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace,aLocation);
		// takes the array of nodes for this triangulation:
		const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
		MeshCore::MeshPointArray(aNodes.Length());
		// takes the array of triangles for this triangulation:
		const Poly_Array1OfTriangle& triangles = aTr->Triangles();
		MeshCore::MeshFacetArray aFacetArray(triangles.Length());
		// create array of node points in absolute coordinate system
		TColgp_Array1OfPnt aPoints(1, aNodes.Length());
		for( Standard_Integer i = 1; i <= aNodes.Length(); i++)
			aPoints(i) = aNodes(i).Transformed(aLocation);
		// Takes the node points of each triangle of this triangulation.
		// takes a number of triangles:
		Standard_Integer nnn = aTr->NbTriangles();
		Standard_Integer nt,n1,n2,n3;
		for( nt = 1 ; nt <= nnn ; nt++)
		{
			// takes the node indices of each triangle in n1,n2,n3:
			triangles(nt).Get(n1,n2,n3);
			// takes the node points:
			gp_Pnt aPnt1 = aPoints(n1);
			Points[0].Set(float(aPnt1.X()),float(aPnt1.Y()),float(aPnt1.Z()));
			gp_Pnt aPnt2 = aPoints(n2);
			Points[1].Set(aPnt2.X(),aPnt2.Y(),aPnt2.Z());
			gp_Pnt aPnt3 = aPoints(n3);
			Points[2].Set(aPnt3.X(),aPnt3.Y(),aPnt3.Z());
			MeshCore::MeshGeomFacet Facet(Points[0],Points[1],Points[2]);
			// give the occ mesh faces to the internal mesh structure of freecad
			aMeshBuilder.AddFacet(Facet);
			//aPointArray[(nt-1)*3] = Points[0];aPointArray[((nt-1)*3)+1] = Points[1];aPointArray[((nt-1)*3)+2] = Points[2];
		}
	}
	aMeshBuilder.Finish();
	return true;
}

bool ShapeTriangulator::ConnectOCCtoFreeCAD()
{
	//Connect the FreeCAD Indizes to the OCC TopoStructure. 
	//Afterwards we are able to get the FreeCAD Indizes for every edge and every Face
	//Make sure that the FreeCAD Mesh is generated the right way bevore invoking this function
	MeshCore::MeshPointArray aPointArray = m_Mesh.GetPoints();
	TopExp_Explorer anExplorer;
	TopoDS_Face aFace;
	TopLoc_Location aLocation;
	for(anExplorer.Init(m_Shape,TopAbs_FACE);anExplorer.More();anExplorer.Next())
	{
		aFace = TopoDS::Face(anExplorer.Current());
		// takes the triangulation of the face aFace:
		Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace,aLocation);
		// takes the array of nodes for this triangulation:
		const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
		// takes the array of triangles for this triangulation:
		const Poly_Array1OfTriangle& triangles = aTr->Triangles();
		// create array of node points in absolute coordinate system
		TColgp_Array1OfPnt aPoints(1, aNodes.Length());
		for( Standard_Integer i = 1; i <= aNodes.Length(); i++)
			aPoints(i) = aNodes(i).Transformed(aLocation);
		// Takes the node points of each triangle of this triangulation.
		// takes a number of triangles:
		Standard_Integer nnn = aTr->NbTriangles();
		Standard_Integer nt,n1,n2,n3;
		//for( nt = 1 ; nt <= nnn ; nt++)
		//{
		//	// takes the node indices of each triangle in n1,n2,n3:
		//	triangles(nt).Get(n1,n2,n3);
		//	// takes the node points:
		//	gp_Pnt aPnt1 = aPoints(n1);
		//	Points[0].Set(float(aPnt1.X()),float(aPnt1.Y()),float(aPnt1.Z()));
		//	gp_Pnt aPnt2 = aPoints(n2);
		//	Points[1].Set(aPnt2.X(),aPnt2.Y(),aPnt2.Z());
		//	gp_Pnt aPnt3 = aPoints(n3);
		//	Points[2].Set(aPnt3.X(),aPnt3.Y(),aPnt3.Z());
		//	MeshCore::MeshGeomFacet Facet(Points[0],Points[1],Points[2]);
		//	// give the occ mesh faces to the internal mesh structure of freecad
		//	aMeshBuilder.AddFacet(Facet);
		//	//aPointArray[(nt-1)*3] = Points[0];aPointArray[((nt-1)*3)+1] = Points[1];aPointArray[((nt-1)*3)+2] = Points[2];
		//}
	}

	return true;
}

bool ShapeTriangulator::Init()
{
	//checken vom Mesh Index-Zeugs
	Base::Builder3D aBuilder;
	BRepTools::Clean(m_Shape);
	BRepMesh::Mesh(m_Shape,1);
	FillUpFacePointsMap();
	GenerateFreeCADMesh();
	ConnectOCCtoFreeCAD();

	TopTools_IndexedDataMapOfShapeListOfShape anIndex;
	anIndex.Clear();
	TopExp aMap;
	aMap.MapShapesAndAncestors(m_Shape,TopAbs_EDGE,TopAbs_FACE,anIndex);
	TopExp_Explorer anExplorer;
	










	EdgeStruct tempEdgeStruct;
	std::vector<TopoDS_Face>::iterator aFaceIterator;
	//store all Edges and corresponding faces and the weighted normals for Start and EndPoint
	for(anExplorer.Init(m_Shape,TopAbs_EDGE);anExplorer.More();anExplorer.Next())
	{
		tempEdgeStruct.aFace.clear();
		tempEdgeStruct.anEdge = TopoDS::Edge(anExplorer.Current());
		const TopTools_ListOfShape& aFaceList = anIndex.FindFromKey(tempEdgeStruct.anEdge);
		TopTools_ListIteratorOfListOfShape aListIterator(aFaceList);
		for(aListIterator.Initialize(aFaceList);aListIterator.More();aListIterator.Next())
		{tempEdgeStruct.aFace.push_back(TopoDS::Face(aListIterator.Value()));}
		
		aFaceIterator = tempEdgeStruct.aFace.begin();
		gp_Pnt2d first,last; 
		gp_Vec U,V,NormalOldFirst(0,0,0),NormalOldLast(0,0,0);
		gp_Pnt tempPoint;
		
		Standard_Integer n1, n2, n3;//Triangle Indizes
		for(aFaceIterator = tempEdgeStruct.aFace.begin();aFaceIterator != tempEdgeStruct.aFace.end();aFaceIterator++)
		{
			Handle_Poly_Triangulation Tr;
			//Zunächst einmal das Polygon der Edge in einer Map speichern
			Handle_Poly_Polygon3D P3D;
			
			TopLoc_Location theLocation;
			Tr = BRep_Tool::Triangulation(*aFaceIterator, theLocation);
			const TColgp_Array1OfPnt& Nodes3d = Tr->Nodes(); // 3d nodes for the points
			
			const Poly_Array1OfTriangle& theTriangles = Tr->Triangles();
			int nbt = Tr->NbTriangles();
			//for (int cpt = 1; cpt <= nbt; cpt++)
			//{// loop on the triangles ; the iterator is : cpt
			//// get the indexes of the points for current triangle in the Nodes3d list;
			//	theTriangles(cpt).Get(n1, n2, n3);
			//	//cout<<n1<<","<<n2<<","<<n3<<endl;
			//	//aBuilder.addSinglePoint(Nodes3d.Value(n1).X(),Nodes3d.Value(n1).Y(),Nodes3d.Value(n1).Z());
			//	//aBuilder.addSinglePoint(Nodes3d.Value(n2).X(),Nodes3d.Value(n2).Y(),Nodes3d.Value(n2).Z());
			//	//aBuilder.addSinglePoint(Nodes3d.Value(n3).X(),Nodes3d.Value(n3).Y(),Nodes3d.Value(n3).Z());
			//}
			//char test[50];
			//for(int z=1;z<=PolyNodes.Length();++z)
			//{
			//	snprintf(test,50,"%d",PolyNodes.Value(z));
			//	aBuilder.addText(Nodes3d.Value(PolyNodes.Value(z)).X(),Nodes3d.Value(PolyNodes.Value(z)).Y(),Nodes3d.Value(PolyNodes.Value(z)).Z(),test,0,0,0);
			//	cout << PolyNodes.Value(z) <<endl;
			//}
			//cout <<endl;

			/*BRep_Tool::UVPoints(tempEdgeStruct.anEdge,*aFaceIterator,first,last);
			BRepAdaptor_Surface aAdaptorSurface(*aFaceIterator);
			aAdaptorSurface.D1(first.X(),first.Y(),tempPoint,U,V);
			U.Cross(V); U.Normalize();
			NormalOldFirst.Add(U);
			aAdaptorSurface.D1(last.X(),last.Y(),tempPoint,U,V);
			U.Cross(V); U.Normalize();
			NormalOldLast.Add(U);
			++i;*/
		}
		//tempEdgeStruct.NormStartP = NormalOldFirst / i;
		//tempEdgeStruct.NormEndP = NormalOldLast / i;
		////Not yet implemented is the check of the edge direction !!!!!
		//m_EdgeStruct.push_back(tempEdgeStruct);
	}
	aBuilder.saveToFile("c:/output_log.iv");
	return true;
}
