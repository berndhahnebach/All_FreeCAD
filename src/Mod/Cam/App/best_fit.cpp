/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *	 Human Rezai <human@mytum.de>                                    * 
 *   Mohamad Najib Muhammad Noor <najib_bean@yahoo.co.uk>                  *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#include "best_fit.h"


best_fit::best_fit(const MeshCore::MeshKernel &mesh, TopoDS_Shape &cad)
:m_Mesh(mesh),m_Cad(cad)
{
	m_normals.resize(m_Mesh.CountPoints());
}

best_fit::best_fit(const MeshCore::MeshKernel &mesh)
:m_Mesh(mesh)
{
	m_normals.resize(m_Mesh.CountPoints());
}

best_fit::best_fit(TopoDS_Shape &cad)
:m_Cad(cad)
{
}

best_fit::~best_fit()
{
}

bool best_fit::RotMat(Base::Matrix4D &M, double degree, int axis)
{
	M.unity();
	degree = PI*degree/360;  // trafo bogenmaß

	switch(axis)
	{
		case 1:  M[1][1]=cos(degree); M[1][2]=sin(degree); M[2][1]=sin(degree); M[2][2]=cos(degree); 
		case 2:  M[0][0]=cos(degree); M[0][2]=sin(degree); M[2][0]=sin(degree); M[2][2]=cos(degree); 
		case 3:  M[0][0]=cos(degree); M[0][1]=sin(degree); M[1][0]=sin(degree); M[1][1]=cos(degree);
		default: throw Base::Exception("second input value differs from 1,2,3 (x,y,z)");
	}

	return true;
}

bool best_fit::TransMat(Base::Matrix4D &M, double trans, int axis)
{
	M.unity();

	switch(axis)
	{
		case 1:  M[0][3] = trans;
		case 2:  M[1][3] = trans;
		case 3:  M[2][3] = trans;
		default: throw Base::Exception("second input value differs from 1,2,3 (x,y,z)");
	}

	return true;
}

bool best_fit::MeshFit_Coarse()
{
	GProp_GProps prop;
	GProp_PrincipalProps pprop;
	BRepGProp SurfProp;
	
    //Base::Vector3f pnt(0.0,0.0,0.0);
	//Base::Vector3f x,y,z;
	//Base::Builder3D log3d_mesh, log3d_cad;
	//gp_Pnt orig;
    
	gp_Vec v1,v2,v3,vec; // Hauptachsenrichtungen
	gp_Trsf trafo;
	
	SurfProp.SurfaceProperties(m_Cad, prop);
	pprop = prop.PrincipalProperties();	
	v1 = pprop.FirstAxisOfInertia();
	v2 = pprop.SecondAxisOfInertia();
	v3 = pprop.ThirdAxisOfInertia();

	//orig  = prop.CentreOfMass();

	Base::Matrix4D M;

	M[0][0] = v1.X(); M[0][1] = v2.X(); M[0][2] = v3.X(); M[0][3] = 0.0f;
	M[1][0] = v1.Y(); M[1][1] = v2.Y(); M[1][2] = v3.Y(); M[1][3] = 0.0f;
    M[2][0] = v1.Z(); M[2][1] = v2.Z(); M[2][2] = v3.Z(); M[2][3] = 0.0f;
    M[3][0] = 0.0f;   M[3][1] = 0.0f;   M[3][2] = 0.0f;   M[3][3] = 1.0f;
	
	M.inverse();

	MeshCore::MeshEigensystem pca(m_Mesh);  
	pca.Evaluate();
	Base::Matrix4D T1 =  pca.Transform();

	m_Mesh.Transform(M*T1);
	
	/* 
	// plot CAD -> local coordinate system
	
	v1.SetX(M[0][0]);v1.SetY(M[1][0]);v1.SetZ(M[2][0]);
	v2.SetX(M[0][1]);v2.SetY(M[1][1]);v2.SetZ(M[2][1]);
	v3.SetX(M[0][2]);v3.SetY(M[1][2]);v3.SetZ(M[2][2]);
	
	x.x = 500*v1.X()+orig.X();x.y = 500*v1.Y()+orig.Y();x.z = 500*v1.Z()+orig.Z();
	y.x = 500*v2.X()+orig.X();y.y = 500*v2.Y()+orig.Y();y.z = 500*v2.Z()+orig.Z();
	z.x = 500*v3.X()+orig.X();z.y = 500*v3.Y()+orig.Y();z.z = 500*v3.Z()+orig.Z();

	pnt.x = orig.X();pnt.y = orig.Y();pnt.z = orig.Z();
	
	log3d_cad.addSingleLine(pnt,x,3,0,0,0);log3d_cad.addSingleLine(pnt,y,3,1,0,0);log3d_cad.addSingleLine(pnt,z,3,1,1,1);
	log3d_cad.saveToFile("c:/CAD_CoordSys.iv");
		
	// plot Mesh -> local coordinate system
	
	T1.inverse();
	
	v1.SetX(T1[0][0]);v1.SetY(T1[1][0]);v1.SetZ(T1[2][0]);
	v2.SetX(T1[0][1]);v2.SetY(T1[1][1]);v2.SetZ(T1[2][1]);
	v3.SetX(T1[0][2]);v3.SetY(T1[1][2]);v3.SetZ(T1[2][2]);

	orig.SetX(T1[0][3]);orig.SetY(T1[1][3]);orig.SetZ(T1[2][3]);

    x.x = 500*v1.X()+orig.X();x.y = 500*v1.Y()+orig.Y();x.z = 500*v1.Z()+orig.Z();
	y.x = 500*v2.X()+orig.X();y.y = 500*v2.Y()+orig.Y();y.z = 500*v2.Z()+orig.Z();
	z.x = 500*v3.X()+orig.X();z.y = 500*v3.Y()+orig.Y();z.z = 500*v3.Z()+orig.Z();

	pnt.x = orig.X();pnt.y = orig.Y();pnt.z = orig.Z();
	
	log3d_mesh.addSingleLine(pnt,x,3,0,0,0);log3d_mesh.addSingleLine(pnt,y,3,1,0,0);log3d_mesh.addSingleLine(pnt,z,3,1,1,1);
	log3d_mesh.saveToFile("c:/Mesh_CoordSys.iv");
	*/
	
	return true;
}

bool best_fit::ShapeFit_Coarse()
{
	GProp_GProps prop;
	GProp_PrincipalProps pprop;
	BRepGProp SurfProp;

    gp_Vec vec; // Hauptachsenrichtungen
	gp_Pnt orig;
	gp_Trsf trafo;
	
	SurfProp.SurfaceProperties(m_Cad, prop);
	pprop = prop.PrincipalProperties();
	orig  = prop.CentreOfMass();

	// CAD-Mesh -> zurück zum Ursprung
	vec.SetX(-orig.X());
	vec.SetY(-orig.Y());
	vec.SetZ(-orig.Z());

	trafo.SetTranslation(vec);
	BRepBuilderAPI_Transform trsf(trafo);
    
	trsf.Perform(m_Cad);
    m_Cad = trsf.Shape();
		
	return true;
}

bool best_fit::mesh_curvature()
{
	// get all points
	const MeshCore::MeshKernel& rMesh = m_Mesh;
    std::vector< Wm4::Vector3<float> > aPnts;
	std::vector<Base::Vector3f> aPnts_tmp;
	MeshCore::MeshPointIterator cPIt( rMesh );
    for ( cPIt.Init(); cPIt.More(); cPIt.Next() ) {
        Wm4::Vector3<float> cP( cPIt->x, cPIt->y, cPIt->z );
		Base::Vector3f cP_tmp( cPIt->x, cPIt->y, cPIt->z );
        aPnts.push_back( cP );
		aPnts_tmp.push_back(cP_tmp);
    }

    // get all point connections
    std::vector<int> aIdx;
	const std::vector<MeshCore::MeshFacet>& raFts = rMesh.GetFacets();
	for ( std::vector<MeshCore::MeshFacet>::const_iterator jt = raFts.begin(); jt != raFts.end(); ++jt ) {
        for (int i=0; i<3; i++) {
            aIdx.push_back( (int)jt->_aulPoints[i] );
        }
    }

    // compute vertex based curvatures
    Wm4::MeshCurvature<float> meshCurv(rMesh.CountPoints(), &(aPnts[0]), rMesh.CountFacets(), &(aIdx[0]));

    // get curvature information now
    const Wm4::Vector3<float>* aMaxCurvDir = meshCurv.GetMaxDirections();
    const Wm4::Vector3<float>* aMinCurvDir = meshCurv.GetMinDirections();
    const float* aMaxCurv = meshCurv.GetMaxCurvatures();
    const float* aMinCurv = meshCurv.GetMinCurvatures();

	double maxCurv = 0.0;
	double avgCurv = 0.0;

	std::vector<double> pntCurv(rMesh.CountPoints());
	Base::Builder3D log3d;

    for ( unsigned long i=0; i<rMesh.CountPoints(); i++ ) {

		pntCurv[i] = max(aMaxCurv[i], -aMinCurv[i]);


		if( pntCurv[i] > maxCurv )
			maxCurv = pntCurv[i];

		avgCurv += pntCurv[i];
    }

	avgCurv /= rMesh.CountPoints();

	double TOL = 0.02*(maxCurv - avgCurv);

	for (unsigned long i=0; i<rMesh.CountPoints(); i++ ) {

		if( pntCurv[i] < TOL )
		{
			m_pntInd.push_back(i);
			log3d.addSinglePoint(aPnts_tmp[i],1,0,0,0);
		}
	
	}

	log3d.saveToFile("c:/minCurvature.iv");
	return true;
}

bool best_fit::Tesselate_Face(TopoDS_Face &aface, MeshCore::MeshKernel &mesh, float deflection)
{
	Base::Builder3D aBuild;
  	MeshCore::MeshBuilder builder(mesh);
	builder.Initialize(1000);
	Base::Vector3f Points[3];

	// removes all the triangulations of the faces ,
	// and all the polygons on the triangulations of the edges:
	BRepTools::Clean(aface);

	// adds a triangulation of the shape aShape with the deflection aDeflection:
	//BRepMesh_IncrementalMesh Mesh(pcShape->getShape(),aDeflection);

	BRepMesh::Mesh(aface,deflection);


	 TopLoc_Location aLocation;
	  // takes the triangulation of the face aFace:
	 Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aface,aLocation);
	 if(!aTr.IsNull()) // if this triangulation is not NULL
	 { 
		// takes the array of nodes for this triangulation:
		const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
		// takes the array of triangles for this triangulation:
		const Poly_Array1OfTriangle& triangles = aTr->Triangles();
		// create array of node points in absolute coordinate system
		TColgp_Array1OfPnt aPoints(1, aNodes.Length());
		for( Standard_Integer i = 1; i < aNodes.Length()+1; i++)
		  aPoints(i) = aNodes(i).Transformed(aLocation);
		// Takes the node points of each triangle of this triangulation.
		// takes a number of triangles:
		Standard_Integer nnn = aTr->NbTriangles();
		Standard_Integer nt,n1,n2,n3;
		for( nt = 1 ; nt < nnn+1 ; nt++)
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
		  // give the occ faces to the internal mesh structure of freecad
		  MeshCore::MeshGeomFacet Face(Points[0],Points[1],Points[2]);
		  builder.AddFacet(Face);
		} 

	  }
	  // if the triangulation of only one face is not possible to get
	  else
	  {
		  throw Base::Exception("Empty face triangulation\n");
	  }

	// finish FreeCAD Mesh Builder and exit with new mesh
	builder.Finish();
	return true;
}



bool best_fit::thinning()
{	
	std::list< std::vector <Base::Vector3f> > BoundariesPoints;

	MeshCore::MeshKernel           mesh;
	MeshCore::MeshGeomFacet        t_face;
	MeshCore::MeshBuilder          builder(mesh);
	MeshCore::MeshAlgorithm        algo(m_Mesh);
	MeshCore::MeshPointIterator    p_it(m_Mesh);
	MeshCore::MeshRefPointToFacets rf2pt(m_Mesh);

	Base::Vector3f facePnts[3];
	
	algo.GetMeshBorders(m_boundInd);
	//algo.GetMeshBorders(BoundariesPoints);
	//builder.Initialize(m_boundInd.size()-3);

	std::list< std::vector <unsigned long> >::iterator bInd = m_boundInd.begin();

	//for(unsigned int i=0; i<m_boundInd.size(); ++i)
	//{
	//	const std::set<MeshCore::MeshFacetArray::_TConstIterator>& faceSet = rf2pt[**bInd];
	//	++bInd;

	//	 for (std::set<MeshCore::MeshFacetArray::_TConstIterator>::const_iterator it = faceSet.begin(); it != faceSet.end(); ++it) 
	//	 {
	//		// Zweimal derefernzieren, um an das MeshFacet zu kommen
	//		t_face = m_Mesh.GetFacet(**it);
	//		facePnts = t_face._aclPoints;

	//		MeshCore::MeshGeomFacet Face(facePnts[0],facePnts[1],facePnts[2]);
	//		Face.CalcNormal();
	//		
	//		builder.AddFacet(Face);
	//	}
	//}
	//builder.Finish();

	//m_Mesh = mesh;

	return true;
}

bool best_fit::Tesselate_Shape(TopoDS_Shape &shape, MeshCore::MeshKernel &mesh, float deflection)
{
	Base::Builder3D aBuild;
	
  	MeshCore::MeshBuilder builder(mesh);
	builder.Initialize(1000);
	Base::Vector3f Points[3];

	// removes all the triangulations of the faces ,
	// and all the polygons on the triangulations of the edges:
	BRepTools::Clean(shape);

	// adds a triangulation of the shape aShape with the deflection aDeflection:
	//BRepMesh_IncrementalMesh Mesh(pcShape->getShape(),aDeflection);

	BRepMesh::Mesh(shape,deflection);
	TopExp_Explorer aExpFace;
	for(aExpFace.Init(shape,TopAbs_FACE);aExpFace.More();aExpFace.Next())
	{  
	  TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
	  TopLoc_Location aLocation;
	  // takes the triangulation of the face aFace:
	  Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace,aLocation);
	  if(!aTr.IsNull()) // if this triangulation is not NULL
	  { 
		// takes the array of nodes for this triangulation:
		const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
		// takes the array of triangles for this triangulation:
		const Poly_Array1OfTriangle& triangles = aTr->Triangles();
		// create array of node points in absolute coordinate system
		TColgp_Array1OfPnt aPoints(1, aNodes.Length());
		for( Standard_Integer i = 1; i < aNodes.Length()+1; i++)
		  aPoints(i) = aNodes(i).Transformed(aLocation);
		// Takes the node points of each triangle of this triangulation.
		// takes a number of triangles:
		Standard_Integer nnn = aTr->NbTriangles();
		Standard_Integer nt,n1,n2,n3;
		for( nt = 1 ; nt < nnn+1 ; nt++)
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
		  // give the occ faces to the internal mesh structure of freecad
		  MeshCore::MeshGeomFacet Face(Points[0],Points[1],Points[2]);
		  builder.AddFacet(Face);

		} 

	  }
	  // if the triangulation of only one face is not possible to get
	  else
	  {
		  throw Base::Exception("Empty face triangulation\n");
	  }
	}
	// finish FreeCAD Mesh Builder and exit with new mesh
	builder.Finish();

	return true;
}

bool best_fit::Comp_Normals()
{
	Base::Builder3D log3d;
	Base::Vector3f normal,local_normal;
	Base::Vector3f origPoint;

	//const MeshCore::MeshFacetArray& Facets = mesh.GetFacets();
	//const MeshCore::MeshPointArray& Points = mesh.GetPoints();
	MeshCore::MeshPointIterator    p_it(m_Mesh);
	MeshCore::MeshFacetIterator    f_it(m_Mesh);
	MeshCore::MeshRefPointToFacets rf2pt(m_Mesh);
	MeshCore::MeshGeomFacet        t_face;

	int NumOfPoints = rf2pt.size();

	//int NumOfPoints = mesh.CountPoints();
	//float fArea = 0.0f;

	for (p_it.Begin(); !p_it.EndReached(); ++p_it)
	{
		 // Satz von Dreiecken zu jedem Punkt
		 const std::set<MeshCore::MeshFacetArray::_TConstIterator>& faceSet = rf2pt[p_it.Position()];
		 float fArea = 0.0;
		 normal.Set(0.0,0.0,0.0);	
         
		 // Iteriere über die Dreiecke zu jedem Punkt
		 for (std::set<MeshCore::MeshFacetArray::_TConstIterator>::const_iterator it = faceSet.begin(); it != faceSet.end(); ++it) 
		 {
			// Zweimal derefernzieren, um an das MeshFacet zu kommen und dem Kernel uebergeben, dass er ein MeshGeomFacet liefert
			t_face = m_Mesh.GetFacet(**it);
			// Flaecheninhalt aufsummieren
			float local_Area = t_face.Area();
			local_normal = t_face.GetNormal();
			if(local_normal.z < 0)
			{
				local_normal = local_normal * (-1);
			}

			fArea  = fArea  + local_Area;
			normal = normal + local_normal;
             
		 }

		 m_normals[p_it.Position()] = normal;

		 origPoint.x = p_it->x;
		 origPoint.y = p_it->y;
		 origPoint.z = p_it->z;

		 log3d.addSingleArrow(origPoint,origPoint+normal,1,0,0,0);
	}

	log3d.saveToFile("c:/normals.iv");

	return true;
}

double best_fit::Comp_Error(MeshCore::MeshKernel &mesh)
{
	Base::Builder3D log3d;
	double err_avg = 0.0;
	double err_max = 0.0;

	MeshCore::MeshFacetGrid aFacetGrid(m_CadMesh);
	MeshCore::MeshAlgorithm malg(m_CadMesh);
	MeshCore::MeshAlgorithm malg2(m_CadMesh);

	Base::Vector3f origPoint, projPoint, distVec;
	Base::Vector3f normal;
	unsigned long facetIndex;

	const MeshCore::MeshFacetArray& Facets = mesh.GetFacets();
	const MeshCore::MeshPointArray& Points = mesh.GetPoints();
	MeshCore::MeshPointIterator p_it(mesh);

	int NumOfPoints = Points.size();

	int i = 0;
	
	for (unsigned int i=0; i<m_pntInd.size(); ++i)
	{     
		p_it.Set(m_pntInd[i]); 

		origPoint.x = p_it->x;
		origPoint.y = p_it->y;
		origPoint.z = p_it->z;

		normal = m_normals[p_it.Position()];

		if(!malg.NearestFacetOnRay(origPoint,normal,aFacetGrid,projPoint,facetIndex))   // gridoptimiert
			malg2.NearestFacetOnRay(origPoint,normal,projPoint,facetIndex);

		log3d.addSingleArrow(origPoint,projPoint);

		distVec = projPoint-origPoint;
		 
		if(err_max < distVec.Length())
			err_max = distVec.Length();

		err_avg += distVec.Length();
	}	

	log3d.saveToFile("c:/projection.iv");

	return err_avg/NumOfPoints;
}

bool best_fit::Coarse_correction()
{
	double error = 0.0;
	double error_tmp;
	int n = m_normals.size();
	Base::Matrix4D M_z,M_x,M_y, M, M_fin;

	// 180°- rotation um z-Achse
	M_z[0][0] = cos(PI); M_z[0][1] = -sin(PI); M_z[0][2] = 0.0f;     M_z[0][3] = 0.0f;
	M_z[1][0] = sin(PI); M_z[1][1] =  cos(PI); M_z[1][2] = 0.0f;     M_z[1][3] = 0.0f;
    M_z[2][0] = 0.0f;    M_z[2][1] =  0.0f;    M_z[2][2] = 1.0f;     M_z[2][3] = 0.0f;
    M_z[3][0] = 0.0f;    M_z[3][1] =  0.0f;    M_z[3][2] = 0.0f;     M_z[3][3] = 1.0f;

	// 180°- rotation um y-Achse
	M_y[0][0] = cos(PI); M_y[0][1] = 0.0f;     M_y[0][2] = -sin(PI); M_y[0][3] = 0.0f;
	M_y[1][0] = 0.0f;    M_y[1][1] = 1.0f;     M_y[1][2] = 0.0f;     M_y[1][3] = 0.0f;
    M_y[2][0] = sin(PI); M_y[2][1] = 0.0f;     M_y[2][2] = cos(PI);  M_y[2][3] = 0.0f;
    M_y[3][0] = 0.0f;    M_y[3][1] = 0.0f;     M_y[3][2] = 0.0f;     M_y[3][3] = 1.0f;

	// 180°- rotation um x-Achse
	M_x[0][0] = 1.0f;    M_x[0][1] = 0.0f;     M_x[0][2] = 0.0f;     M_x[0][3] = 0.0f;
	M_x[1][0] = 0.0f;    M_x[1][1] = cos(PI);  M_x[1][2] = sin(PI);  M_x[1][3] = 0.0f;
    M_x[2][0] = 0.0f;    M_x[2][1] = sin(PI);  M_x[2][2] = cos(PI);  M_x[2][3] = 0.0f;
    M_x[3][0] = 0.0f;    M_x[3][1] = 0.0f;     M_x[3][2] = 0.0f;     M_x[3][3] = 1.0f;

	

	MeshCore::MeshKernel tmp_mesh = m_Mesh;
	
	
	error = best_fit::Comp_Error(tmp_mesh);  // ausgangsfehler
	
	M = M_x;
	tmp_mesh.Transform(M);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}



	M = M_y;
	tmp_mesh = m_Mesh;
	tmp_mesh.Transform(M);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}


	M = M_z;
	tmp_mesh = m_Mesh;
	tmp_mesh.Transform(M_z);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}


	M = M_x*M_y;
	tmp_mesh = m_Mesh;
	tmp_mesh.Transform(M);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}

	M = M_x*M_z;
	tmp_mesh = m_Mesh;
	tmp_mesh.Transform(M);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}


	M = M_y*M_z;
	tmp_mesh = m_Mesh;
	tmp_mesh.Transform(M);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}


	M = M_x*M_y*M_z;
	tmp_mesh = m_Mesh;
	tmp_mesh.Transform(M);

	for(int i=0; i<n;++i)
	{
		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	}

	error_tmp = best_fit::Comp_Error(tmp_mesh);

	if(error_tmp < error)
	{
		error = error_tmp;
		M_fin = M;
	}

	m_Mesh.Transform(M_fin);
	error_tmp = best_fit::Comp_Error(m_Mesh);

	return true;
}


bool best_fit::Fit_iter()
{
	double check = 0;
	double TOL = 0.4;

	double err = 0.0;
	double err_tmp = 0;
	double alph = 0;
	double alph_step = 1;
	double traf = 0;
	double traf_step = 5;
	double step = 5*PI/360;
	double ref  = -20*PI/360;


	int c = 0;
	int pos = 0;
	int neg = 0;
	int fl = 1;

	int n = m_Mesh.CountPoints();

    MeshCore::MeshKernel tmp_mesh = m_Mesh;
	MeshCore::MeshKernel tmp_mesh2;

	Base::Matrix4D Mx,My,Mz,Mx_ref,My_ref,Mz_ref, M_fin,M;
	
	M.unity();
	Mx.unity();
	My.unity();
	Mz.unity();
	M_fin.unity();

	// start fehler
	err = best_fit::Comp_Error(m_Mesh);

	// rotation
    // Rotation um die z-achse
	Mz[0][0] =  cos(step);
	Mz[0][1] = -sin(step);
	Mz[1][0] =  sin(step);
	Mz[1][1] =  cos(step);

    // Rotation um die y-achse
	My[0][0] =  cos(step);
    My[0][2] = -sin(step);
	My[2][0] =  sin(step);
	My[2][2] =  cos(step);

    // Rotation um die x-achse
	Mx[1][1] =  cos(step);
    Mx[1][2] = -sin(step);
	Mx[2][1] =  sin(step);
	Mx[2][2] =  cos(step);

	
	Mz_ref[0][0] =  cos(ref);
    Mz_ref[0][1] = -sin(ref);
	Mz_ref[1][0] =  sin(ref);
	Mz_ref[1][1] =  cos(ref);


	My_ref[0][0] =  cos(ref);
    My_ref[0][2] = -sin(ref);
	My_ref[2][0] =  sin(ref);
	My_ref[2][2] =  cos(ref);


	Mx_ref[1][1] =  cos(ref);
    Mx_ref[1][2] = -sin(ref);
	Mx_ref[2][1] =  sin(ref);
	Mx_ref[2][2] =  cos(ref);


	M = Mx_ref*My_ref*Mz_ref;
	tmp_mesh.Transform(M);  // rotiere zur referenzposition (-20°, -20°, -20°)

	for (unsigned int i=0; i<m_pntInd.size(); ++i)
	{
		//p_it.Set(m_pntInd[i]); 
		m_normals[m_pntInd[i]].x = M[0][0]*m_normals[m_pntInd[i]].x + M[0][1]*m_normals[m_pntInd[i]].y + M[0][2]*m_normals[m_pntInd[i]].z;
		m_normals[m_pntInd[i]].y = M[1][0]*m_normals[m_pntInd[i]].x + M[1][1]*m_normals[m_pntInd[i]].y + M[1][2]*m_normals[m_pntInd[i]].z;
		m_normals[m_pntInd[i]].z = M[2][0]*m_normals[m_pntInd[i]].x + M[2][1]*m_normals[m_pntInd[i]].y + M[2][2]*m_normals[m_pntInd[i]].z;
	}
	
	err = best_fit::Comp_Error(tmp_mesh);


	std::vector<Base::Vector3f> normals_tmp = m_normals;
	M.unity();

	tmp_mesh2 = tmp_mesh;

	for(int n1=0; n1<9; ++n1)
	{
		for(int m1=0; m1<n1; ++m1)
			M *= Mx;

		for(int n2=0; n2<9; ++n2)
	    {
			for(int m2=0; m2<n2; ++m2)
				M *= My;

			for(int n3=0; n3<9; ++n3)
			{
				for(int m3=0; m3<n3; ++m3)
					M *= Mz;

				tmp_mesh.Transform(M);
				
				for (unsigned int i=0; i<m_pntInd.size(); ++i)
				{
					//p_it.Set(m_pntInd[i]); 
					m_normals[m_pntInd[i]].x = M[0][0]*m_normals[m_pntInd[i]].x + M[0][1]*m_normals[m_pntInd[i]].y + M[0][2]*m_normals[m_pntInd[i]].z;
					m_normals[m_pntInd[i]].y = M[1][0]*m_normals[m_pntInd[i]].x + M[1][1]*m_normals[m_pntInd[i]].y + M[1][2]*m_normals[m_pntInd[i]].z;
					m_normals[m_pntInd[i]].z = M[2][0]*m_normals[m_pntInd[i]].x + M[2][1]*m_normals[m_pntInd[i]].y + M[2][2]*m_normals[m_pntInd[i]].z;
				}

				err_tmp = best_fit::Comp_Error(tmp_mesh);

				if(err_tmp < err)
					M_fin = M;

				M.unity();
				m_normals = normals_tmp;
			}
		}
	}
		
	m_Mesh.Transform(M_fin);

	


	//for(int i=0; i<3; ++i)
	//{
    
		
	//// translation fit
	//
	//// z-translation

	//cout << "z-translation" << endl;

	//while(traf_step > TOL)
	//{
	//	M[2][3] = fl*traf_step; // z-translation-element

	//	tmp_mesh.Transform(M);
	//	err_tmp = best_fit::Comp_Error(tmp_mesh);		
	//	
	//	if(err_tmp <= err)
	//	{
	//		err = err_tmp;
	//		cout << "avg_err: " << err << endl;
	//		check  += fl*traf_step;
	//	}
	//	else
	//	{
	//		if(fl == -1)
	//		{
	//			// zurück als es noch besser war
	//			M[2][3] = traf_step;
	//			tmp_mesh.Transform(M);

	//			traf_step /= 2;
	//			fl = 1;
	//		}
	//		else
	//		    fl = -1;				
	//	}
	//}

	//cout << "z-translation: " << check << endl;

	//fl = 1;
	//traf = 0;
	//traf_step = step;
	//M[2][3] = 0.0f;
	//check = 0;
	//// end z-translation


	//// y-translation

	//cout << "y-translation" << endl;

	//while(traf_step > TOL)
	//{
	//	M[1][3] = fl*traf_step; // y-translation-element

	//	tmp_mesh.Transform(M);
	//	err_tmp = best_fit::Comp_Error(tmp_mesh);		
	//	
	//	if(err_tmp <= err)
	//	{
	//		err = err_tmp;
	//		cout << "avg_err: " << err << endl;
	//		check  += fl*traf_step;
	//	}
	//	else
	//	{				
	//		if(fl == -1)
	//		{
	//			M[1][3] = traf_step;
	//			tmp_mesh.Transform(M);
	//			traf_step /= 2;
	//			fl = 1;
	//		}
	//		else
	//		    fl = -1;				
	//	}
	//}

	//cout << "y-translation: " << check << endl;
	//fl = 1;
	//traf = 0;
	//traf_step = step;
	//M[1][3] = 0.0f;
	//check = 0;
	//// end y-translation


	//// x-translation

	//cout << "x-translation" << endl;

	//while(traf_step > TOL)
	//{
	//	M[0][3] = fl*traf_step; // x-translation-element

	//	tmp_mesh.Transform(M);
	//	err_tmp = best_fit::Comp_Error(tmp_mesh);
	//	
	//	if(err_tmp <= err)
	//	{
	//		err = err_tmp;
	//		cout << "avg_err: " << err << endl;
	//		check  += fl*traf_step;
	//	}
	//	else
	//	{				
	//		if(fl == -1)
	//		{
	//			M[0][3] = traf_step;
	//			tmp_mesh.Transform(M);
	//			traf_step /= 2;
	//			fl = 1;
	//		}
	//		else
	//		    fl = -1;				
	//	}
	//}

	//cout << "x-translation: " << check << endl;
	//fl = 1;
	//traf = 0;
	//traf_step = step;
	//M[0][3] = 0.0f;
	//check = 0;
	//// end x-translation

 //   // end translation fit


 //   
	//
	//// rotation fit

	//// z-rotation
	//cout << "z-rotation " << check << endl;
	//while(traf_step > TOL)
	//{
	//	traf = fl*PI*traf_step/360;

	//	// Rotation um die z-achse
	//	M[0][0] =  cos(traf);
	//    M[0][1] = -sin(traf);
	//	M[1][0] =  sin(traf);
	//	M[1][1] =  cos(traf);
	//	
	//	tmp_mesh.Transform(M);

	//	for(int i=0; i<n;++i)
	//	{
	//		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
	//		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
	//		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	//    }

	//	err_tmp = best_fit::Comp_Error(tmp_mesh);
	//	
	//	if(err_tmp < err)
	//	{
	//		err = err_tmp;
	//		cout << "avg_err: " << err << endl;
	//		check  += fl*traf_step;
	//	}
	//	else
	//	{				
	//		if(fl == -1)
	//		{
	//			M[0][0] =  cos(-traf);
	//			M[0][1] = -sin(-traf);
	//			M[1][0] =  sin(-traf);
	//			M[1][1] =  cos(-traf);
	//			tmp_mesh.Transform(M);

	//			for(int i=0; i<n;++i)
	//			{
	//				m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
	//				m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
	//				m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	//			}

	//			traf_step /= 2;
	//			fl = 1;
	//		}
	//		else
	//		    fl = -1;				
	//	}
	//}

	//cout << "z-rotation: " << check << endl;
	//fl = 1;
	//traf = 0;
	//traf_step = step;
	//M.unity();
	//check = 0;
	//// end z-rotation

	//// y-rotation
	//cout << "y-rotation " << check << endl;
	//while(traf_step > TOL)
	//{
	//	traf = fl*PI*traf_step/360;

	//	// Rotation um die z-achse
	//	M[0][0] =  cos(traf);
	//    M[0][2] = -sin(traf);
	//	M[2][0] =  sin(traf);
	//	M[2][2] =  cos(traf);
	//	
	//	tmp_mesh.Transform(M);

	//	for(int i=0; i<n;++i)
	//	{
	//		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
	//		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
	//		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	//    }

	//	err_tmp = best_fit::Comp_Error(tmp_mesh);
	//	
	//	if(err_tmp < err)
	//	{
	//		err = err_tmp;
	//		cout << "avg_err: " << err << endl;
	//		check  += fl*traf_step;
	//	}
	//	else
	//	{				
	//		if(fl == -1)
	//		{
	//			M[0][0] =  cos(-traf);
	//			M[0][2] = -sin(-traf);
	//			M[2][0] =  sin(-traf);
	//			M[2][2] =  cos(-traf);
	//			tmp_mesh.Transform(M);

	//			for(int i=0; i<n;++i)
	//			{
	//				m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
	//				m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
	//				m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	//			}

	//			traf_step /= 2;
	//			fl = 1;
	//		}
	//		else
	//		    fl = -1;				
	//	}
	//}

	//cout << "y-rotation: " << check << endl;
	//fl = 1;
	//traf = 0;
	//traf_step = step;
	//M.unity();
	//check = 0;
	//// end y-rotation

	//// x-rotation
	//cout << "x-rotation: " << check << endl;
	//while(traf_step > TOL)
	//{
	//	traf = fl*PI*traf_step/360;

	//	// Rotation um die z-achse
	//	M[1][1] =  cos(traf);
	//    M[1][2] = -sin(traf);
	//	M[2][1] =  sin(traf);
	//	M[2][2] =  cos(traf);
	//	
	//	tmp_mesh.Transform(M);

	//	for(int i=0; i<n;++i)
	//	{
	//		m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
	//		m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
	//		m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	//    }

	//	err_tmp = best_fit::Comp_Error(tmp_mesh);
	//	
	//	if(err_tmp < err)
	//	{
	//		err = err_tmp;
	//		cout << "avg_err: " << err << endl;
	//		check  += fl*traf_step;
	//	}
	//	else
	//	{				
	//		if(fl == -1)
	//		{
	//			M[1][1] =  cos(-traf);
	//			M[1][2] = -sin(-traf);
	//			M[2][1] =  sin(-traf);
	//			M[2][2] =  cos(-traf);
	//			tmp_mesh.Transform(M);

	//			for(int i=0; i<n;++i)
	//			{
	//				m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
	//				m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
	//				m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	//			}

	//			traf_step /= 2;
	//			fl = 1;
	//		}
	//		else
	//		    fl = -1;				
	//	}
	//}

	//cout << "x-rotation: " << check << endl;
	//fl = 1;
	//traf = 0;
	//traf_step = step;
	//M.unity();

	//check = 0;
	//// end x-rotation

 //   // end rotation fit

    //m_Mesh = tmp_mesh;

	return true;
}

bool best_fit::test()
{
	double check = 0;
	double TOL = 0.4;

	double err = 0.0;
	double err_tmp = 0;
	double alph = 0;
	double alph_step = 1;
	double traf = 0;
	double traf_step = 8;
	double step = 8;


	int c = 0;
	int pos = 0;
	int neg = 0;
	int fl = 1;

	int n = m_Mesh.CountPoints();

    MeshCore::MeshKernel tmp_mesh = m_Mesh;

	Base::Matrix4D M;
	M.unity();


	// start fehler
	err = best_fit::Comp_Error(m_Mesh);

	while(traf_step > TOL)
	{
		traf = fl*PI*traf_step/360;

		// Rotation um die z-achse
		M[0][0] =  cos(traf);
	    M[0][1] = -sin(traf);
		M[1][0] =  sin(traf);
		M[1][1] =  cos(traf);
		
		tmp_mesh.Transform(M);

		for(int i=0; i<n;++i)
		{
			m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
			m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
			m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
	    }

		err_tmp = best_fit::Comp_Error(tmp_mesh);
		
		if(err_tmp < err)
		{
			err = err_tmp;
		}
		else
		{				
			if(fl == -1)
			{
				M[0][0] =  cos(-traf);
				M[0][1] = -sin(-traf);
				M[1][0] =  sin(-traf);
				M[1][1] =  cos(-traf);
				tmp_mesh.Transform(M);

				for(int i=0; i<n;++i)
				{
					m_normals[i].x = M[0][0]*m_normals[i].x + M[0][1]*m_normals[i].y + M[0][2]*m_normals[i].z;
					m_normals[i].y = M[1][0]*m_normals[i].x + M[1][1]*m_normals[i].y + M[1][2]*m_normals[i].z;
					m_normals[i].z = M[2][0]*m_normals[i].x + M[2][1]*m_normals[i].y + M[2][2]*m_normals[i].z;
				}

				traf_step /= 2;
				fl = 1;
			}
			else
			    fl = -1;				
		}
	}


	m_Mesh = tmp_mesh;

	return true;
}






















//
//bool best_fit::rotation_fit()
//{
//	
//	std::vector<Base::Vector3f> pnt_tmp(5);
//	std::vector<Base::Vector3f> pnt_ref;
//	
//	
//	std::vector<Base::Vector3f> pnt;
//	
//	double TOL = 0.001;
//
//	double err = 0;
//	double err_tmp = 0;
//	double alph = 0;
//	double alph_step = 1;
//	double traf = 0;
//	double traf_step = 1;
//
//	
//
//	int c = 0;
//	int pos = 0;
//	int neg = 0;
//	int fl = 1;
//
//	int NumOfPoints = pnt.size();
//
//	std::vector< std::vector<double> > R(3, std::vector<double>(3,0.0)); // Rotationsmatrix
//
//			// start fehler
//		for(int i=0; i<NumOfPoints; ++i)
//			err_tmp += *sqrt( (pnt[i][0] - pnt_ref[i][0])*(pnt[i][0] - pnt_ref[i][0]) + 
//			                            (pnt[i][1] - pnt_ref[i][1])*(pnt[i][1] - pnt_ref[i][1]) + 
//										(pnt[i][2] - pnt_ref[i][2])*(pnt[i][2] - pnt_ref[i][2]) );
//	
//		// z-rotation
//		while(traf_step > TOL)
//		{
//			traf += fl*traf_step*PI/360;
//
//			// Rotation um die z-achse
//			R[0][0] =  cos(traf);
//			R[0][1] = -sin(traf);
//			R[1][0] =  sin(traf);
//			R[1][1] =  cos(traf);
//			R[2][2] =  1;
//
//			err = 0;
//			for(int i=0; i<NumOfPoints; ++i)
//			{
//
//				pnt_tmp[i][0] = (R[0][0]*pnt[i][0] + R[0][1]*pnt[i][1] + R[0][2]*pnt[i][2]);
//				pnt_tmp[i][1] = (R[1][0]*pnt[i][0] + R[1][1]*pnt[i][1] + R[1][2]*pnt[i][2]);
//				pnt_tmp[i][2] = (R[2][0]*pnt[i][0] + R[2][1]*pnt[i][1] + R[2][2]*pnt[i][2]);
//
//				err +=*sqrt( (pnt_tmp[i][0] - pnt_ref[i][0])*(pnt_tmp[i][0] - pnt_ref[i][0]) + 
//			                            (pnt_tmp[i][1] - pnt_ref[i][1])*(pnt_tmp[i][1] - pnt_ref[i][1]) + 
//										(pnt_tmp[i][2] - pnt_ref[i][2])*(pnt_tmp[i][2] - pnt_ref[i][2]) );
//			}
//
//			traf -= fl*traf_step*PI/360;
//			
//			if(err < err_tmp)
//				traf += fl*traf_step*PI/360;
//			else
//			{				
//				if(fl == -1)
//				{
//					traf_step /= 2;
//					fl = 1;
//				}
//				else
//				    fl = -1;				
//			}
//
//		}
//
//		R[0][0] =  cos(traf);
//		R[0][1] = -sin(traf);
//		R[1][0] =  sin(traf);
//		R[1][1] =  cos(traf);
//		R[2][2] =  1;
//
//		for(int i=0; i<NumOfPoints; ++i)
//		{
//			pnt[i][0] = (R[0][0]*pnt[i][0] + R[0][1]*pnt[i][1] + R[0][2]*pnt[i][2]);
//			pnt[i][1] = (R[1][0]*pnt[i][0] + R[1][1]*pnt[i][1] + R[1][2]*pnt[i][2]);
//			pnt[i][2] = (R[2][0]*pnt[i][0] + R[2][1]*pnt[i][1] + R[2][2]*pnt[i][2]);
//		}
//
//		traf = 0;
//		traf_step = 1;
//		// end z-rotation
//	return true;
//}

