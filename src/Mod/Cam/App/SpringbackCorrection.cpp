/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>          *
 *   Human Rezai <Human@web.de>                                            *
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

#include "SpringbackCorrection.h"
#include "best_fit.h"


#include <Mod/Mesh/App/Core/Builder.h>
#include <Mod/Mesh/App/WildMagic4/Wm4MeshCurvature.h>

#include <BRepMeshAdapt.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <Base/Builder3D.h>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRep_Tool.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepExtrema_DistShapeShape.hxx>



///*********BINDINGS********/
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/atlas/clapack.hpp>

using namespace boost::numeric::bindings;

using namespace boost::numeric;



SpringbackCorrection::SpringbackCorrection(const TopoDS_Shape& aShape)
        :m_Shape(aShape)
{
    m_EdgeStruct.clear();
    best_fit::Tesselate_Shape(m_Shape,m_Mesh,0.1); // Basistriangulierung des ganzen Shapes
}

SpringbackCorrection::SpringbackCorrection(const TopoDS_Shape& aShape, const MeshCore::MeshKernel& aMesh)
        :m_Shape(aShape),m_Mesh(aMesh)
{
    m_EdgeStruct.clear();
    EdgeMap.clear();
    MeshMap.clear();
    //Nur aktuell
    m_Mesh.Clear();
    //Remove any existing Triangulation on the Shape
    BRepTools::Clean(m_Shape);
    best_fit::Tesselate_Shape(m_Shape,m_Mesh,0.1); // Basistriangulierung des ganzen Shapes
}


SpringbackCorrection::SpringbackCorrection(const TopoDS_Shape& aShape, const MeshCore::MeshKernel& CadMesh, const MeshCore::MeshKernel& Mesh)
        :m_Mesh(CadMesh), m_Mesh2Fit(Mesh)
{
    m_EdgeStruct.clear();
}

SpringbackCorrection::~SpringbackCorrection()
{
}


#include <Poly_Polygon3d.hxx>
#include <Handle_Poly_Polygon3d.hxx>
bool SpringbackCorrection::CalcCurv()
{
    Base::Builder3D logo;
    BRep_Builder VertexBuild;
    BRepExtrema_DistShapeShape pnt2edge;
    pnt2edge.SetDeflection(0.1);

    TopExp_Explorer aExpFace;
    TopExp_Explorer aExpEdge;
    TopoDS_Vertex V;

    MeshPnt aMeshStruct;
    MeshCore::MeshPointIterator pntIt(m_Mesh);
    MeshCore::MeshKernel FaceMesh;
    MeshCore::MeshPointArray MeshPnts;
    MeshCore::MeshPointArray MeshPntsCopy;
    MeshCore::MeshFacetArray MeshFacets;
    MeshCore::MeshFacetIterator facetIt(m_Mesh);

    std::pair<Base::Vector3f, MeshPnt> inp;
    std::vector<int> MeanVec;
    std::vector<FacePnt> FacePntVector;
    BRep_Tool aBRep;
    TopLoc_Location aloc;
    Base::Vector3f mP;
    gp_Pnt e_pnt, m_pnt;
    double dist, dist_tmp, distSum, revSum;;
    int siz, n;

    std::map<TopoDS_Edge, std::vector<double>, Edge_Less>::iterator edge_it;
    std::map<Base::Vector3f,MeshPnt,MeshPntLess >::iterator meshIt;
    std::vector<MeshCore::MeshPoint>::iterator pIt;

    m_CurvPos.resize(m_Mesh.CountPoints());
    m_CurvNeg.resize(m_Mesh.CountPoints());
    m_MeshStruct.resize(m_Mesh.CountPoints());
    MeanVec.resize(m_Mesh.CountPoints(), 0);

    // F�lle Map mit Punkten (Key) und deren, zur Basistriangulierung korrespondierenden, Indizes
    MeshPnts = m_Mesh.GetPoints();

    for (unsigned int i=0; i<MeshPnts.size(); ++i)
    {
        aMeshStruct.pnt = MeshPnts[i]    ;        // stores point
        aMeshStruct.index = i;   // stores index
        aMeshStruct.maxCurv = 1e+3;
        aMeshStruct.minCurv = -1e+3;
        inp.first  = aMeshStruct.pnt;
        inp.second = aMeshStruct;
        MeshMap.insert(inp);
    }

    // explores all faces  ------------  Hauptschleife
    for (aExpFace.Init(m_Shape,TopAbs_FACE);aExpFace.More();aExpFace.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
        TransferFaceTriangulationtoFreeCAD(aFace, FaceMesh);
        MeshPnts.clear();
        MeshPnts = FaceMesh.GetPoints();
        MeshPntsCopy = MeshPnts;
        n = MeshPnts.size();
        TopLoc_Location aLocation;
        // berechne normalen -> m_MeshStruct
        Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace,aLocation);
        const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
        // create array of node points in absolute coordinate system
        TColgp_Array1OfPnt aPoints(1, aNodes.Length());
        for ( Standard_Integer i = 1; i <= aNodes.Length(); i++)
            aPoints(i) = aNodes(i).Transformed(aLocation);

        const TColgp_Array1OfPnt2d& aUVNodes = aTr->UVNodes();


        BRepAdaptor_Surface aSurface(aFace);
        Base::Vector3f Pnt1, Pnt2;
        gp_Pnt2d par;
        gp_Pnt P;
        gp_Vec D1U, D1V;
        for (int i=1; i<n+1; ++i)
        {
            par = aUVNodes.Value(i);
            aSurface.D1(par.X(),par.Y(),P,D1U,D1V);
            P = aPoints(i);
            Pnt1.x = P.X();
            Pnt1.y = P.Y();
            Pnt1.z = P.Z();
            meshIt = MeshMap.find(Pnt1);
            //if (meshIt == MeshMap.end())
            //   cout << "error";

            D1U.Cross(D1V);
            D1U.Normalize();
            if (aFace.Orientation() == TopAbs_FORWARD) D1U.Scale(-1.0);

            //Pnt2.Set(Pnt1.x+D1U.X(),Pnt1.y+D1U.Y(),Pnt1.z+D1U.Z());
            //logo.addSingleArrow(Pnt1, Pnt2);

            m_MeshStruct[((*meshIt).second).index].normal.x = D1U.X();
            m_MeshStruct[((*meshIt).second).index].normal.y = D1U.Y();
            m_MeshStruct[((*meshIt).second).index].normal.z = D1U.Z();
        }

        // get Inner Mesh Points
        int innerpoints = GetBoundary(FaceMesh, MeshPnts);



        FacePntVector.resize(innerpoints);  // stores inner-points and sourrounding edges-distances

        // explores the edges of the face ----------------------
        for (aExpEdge.Init(aFace,TopAbs_EDGE);aExpEdge.More();aExpEdge.Next())
        {
            TopoDS_Edge aEdge = TopoDS::Edge(aExpEdge.Current());

            edge_it = EdgeMap.find(aEdge);
            if (edge_it == EdgeMap.end())
                cout << "error";

            pnt2edge.LoadS1(aEdge);

            int counter = 0;

            for (pIt = MeshPnts.begin(); pIt != MeshPnts.end(); ++pIt)
            {
                //check if there is no boundary
                if (pIt->_ulProp == 0)
                {
                    m_pnt.SetCoord(pIt->x, pIt->y, pIt->z);
                    VertexBuild.MakeVertex(V,m_pnt,0.001);
                    pnt2edge.LoadS2(V);
                    pnt2edge.Perform();

                    if (pnt2edge.IsDone() == false)
                    {
                        throw Base::Exception("couldn't perform distance calculation pnt2edge \n");
                    }

                    dist = pnt2edge.Value();

                    FacePntVector[counter].pnt = *pIt;
                    FacePntVector[counter].distances.push_back(dist);
                    FacePntVector[counter].MinEdgeOff.push_back((*edge_it).second[0]);
                    FacePntVector[counter].MaxEdgeOff.push_back((*edge_it).second[1]);
                    ++counter;
                }

            }

            // Knoten auf der EDGE die EdgeOffset-Werte zuweisen

            // Edge -> Polygon -> Nodes
            Handle(Poly_Polygon3D) polyg = BRep_Tool::Polygon3D(aEdge,aloc);

            if (polyg.IsNull())
            {
                cout << "error";
            }

            TColStd_Array1OfInteger IndArr(1,polyg->NbNodes());
            //IndArr = polyg->Nodes();

            //for (int k=0; k<polyg->Nodes().Upper(); ++k)
            //         {
            //             e_pnt = aPoints(IndArr.Value(k+1));

            //             mP.x = e_pnt.X();
            //             mP.y = e_pnt.Y();
            //             mP.z = e_pnt.Z();

            //             meshIt = MeshMap.find(mP);
            //             //if (meshIt == MeshMap.end())
            //              //   cout << "error";

            //             ++MeanVec[meshIt->second.index];

            //             // nehme stets den minimalen wert
            //             if (((*edge_it).second[0])>((*meshIt).second).minCurv)
            //                 ((*meshIt).second).minCurv = (*edge_it).second[0];

            //             if ((*edge_it).second[1]<((*meshIt).second).maxCurv)
            //                 ((*meshIt).second).maxCurv = (*edge_it).second[1];
            //         }
        }

        // Knoten INNERHALB eines Faces die Offset-Werte zuweisen
        for (unsigned int k=0; k<FacePntVector.size(); ++k)
        {
            meshIt = MeshMap.find(FacePntVector[k].pnt);

            distSum = 0.0;
            for (int l=0; l<FacePntVector[k].distances.size(); ++l)
                distSum +=  FacePntVector[k].distances[l];

            revSum = 0.0;
            for (int l=0; l<FacePntVector[k].distances.size(); ++l)
            {
                revSum += distSum/FacePntVector[k].distances[l];

                if (((*meshIt).second).minCurv == -1e+3)
                    ((*meshIt).second).minCurv = 0.0;

                if (((*meshIt).second).maxCurv == 1e+3)
                    ((*meshIt).second).maxCurv = 0.0;

                ((*meshIt).second).minCurv += distSum*(FacePntVector[k].MinEdgeOff[l])/FacePntVector[k].distances[l];
                ((*meshIt).second).maxCurv += distSum*(FacePntVector[k].MaxEdgeOff[l])/FacePntVector[k].distances[l];

            }

            ((*meshIt).second).minCurv /= revSum;
            ((*meshIt).second).maxCurv /= revSum;

        }
        FacePntVector.clear();
    }



    Base::Builder3D log,log3d;
    Base::Vector3f pa,pb;
    char text[10];
    int w;

    // �bergebe kr�mmungswerte an m_MeshStruct
    for (meshIt = MeshMap.begin(); meshIt != MeshMap.end(); ++meshIt)
    {
        w = meshIt->second.index;
        m_MeshStruct[w].pnt = (*meshIt).first;

        m_MeshStruct[w].minCurv = (((*meshIt).second).minCurv);
        m_MeshStruct[w].maxCurv = (((*meshIt).second).maxCurv);

        // Ausgabe
        snprintf(text,10,"%f",m_MeshStruct[w].minCurv);
        log.addText(m_MeshStruct[w].pnt.x, m_MeshStruct[w].pnt.y, m_MeshStruct[w].pnt.z,text);

        if (MeanVec[w] == 1)
            log3d.addSinglePoint(m_MeshStruct[w].pnt,4,0,0,0); // innere punkte - > schwarz
        else if (MeanVec[w] == 2)
            log3d.addSinglePoint(m_MeshStruct[w].pnt,4,1,1,1); // edge-punkte   - > wei�
        else
            log3d.addSinglePoint(m_MeshStruct[w].pnt,4,1,0,0); // eck-punkte    - > rot

        //m_MeshStruct[w].minCurv = (((*meshIt).second).minCurv)/MeanVec[w];
        //m_MeshStruct[w].maxCurv = (((*meshIt).second).maxCurv)/MeanVec[w];

        //if(m_MeshStruct[w].maxCurv < 100 || fabs(m_MeshStruct[w].minCurv) < 100)
        //{
        // if(m_MeshStruct[w].maxCurv > fabs(m_MeshStruct[w].minCurv))
        // {
        //  //snprintf(text,10,"%f",m_MeshStruct[w].minCurv);
        //  m_MeshStruct[w].normal.Scale(m_MeshStruct[w].minCurv,m_MeshStruct[w].minCurv,m_MeshStruct[w].minCurv);
        //  logo.addSingleArrow(m_MeshStruct[w].pnt, m_MeshStruct[w].pnt+m_MeshStruct[w].normal);
        // }
        // else
        // {
        //  //snprintf(text,4,"%f",m_MeshStruct[w].maxCurv);
        //  m_MeshStruct[w].normal.Scale(m_MeshStruct[w].maxCurv,m_MeshStruct[w].maxCurv,m_MeshStruct[w].maxCurv);
        //  logo.addSingleArrow(m_MeshStruct[w].pnt, m_MeshStruct[w].pnt+m_MeshStruct[w].normal);
        // }
        //}

        /*if(m_MeshStruct[w].maxCurv < 100)
        {
         m_MeshStruct[w].normal.Scale(0.5*m_MeshStruct[w].maxCurv,0.5*m_MeshStruct[w].maxCurv,0.5*m_MeshStruct[w].maxCurv);
         logo.addSingleArrow(m_MeshStruct[w].pnt, m_MeshStruct[w].pnt+m_MeshStruct[w].normal);
        }*/

        //snprintf(text,10,"%f",m_MeshStruct[w].maxCurv);
        //snprintf(text,10,"%i",w);


        /*if(m_MeshStruct[w].maxCurv > fabs(m_MeshStruct[w].minCurv))
        {
         snprintf(text,10,"%f",m_MeshStruct[w].minCurv);
         
        }
        else
        {
         snprintf(text,10,"%f",m_MeshStruct[w].maxCurv);
        }*/

    }

    log.saveToFile("c:/printCurv.iv");
    log3d.saveToFile("c:/triPnts.iv");
    logo.saveToFile("c:/NormalsCurv.iv");

    return true;
}

bool SpringbackCorrection::Init()
{
    double max = cMin;
    TopTools_IndexedDataMapOfShapeListOfShape anIndex;
    anIndex.Clear();
    TopExp aMap;
    aMap.MapShapesAndAncestors(m_Shape,TopAbs_EDGE,TopAbs_FACE,anIndex);
    TopExp_Explorer anExplorer, anExplorer2;
    TopoDS_Face aFace;
    EdgeStruct tempEdgeStruct;
    std::vector<EdgeStruct> EdgeVec;
    MeshCore::MeshKernel FaceMesh;
    std::vector<double> curvature(2);
    std::pair<TopoDS_Edge, std::vector<double> > aPair;

    float maxOffset, minOffset;

    for (anExplorer.Init(m_Shape,TopAbs_EDGE);anExplorer.More();anExplorer.Next())
    {
        tempEdgeStruct.aFace.clear();
        tempEdgeStruct.MaxOffset = 1e+10;
        tempEdgeStruct.MinOffset = -1e+10;
        tempEdgeStruct.anEdge = TopoDS::Edge(anExplorer.Current()); // store current edge

        const TopTools_ListOfShape& aFaceList = anIndex.FindFromKey(tempEdgeStruct.anEdge);
        TopTools_ListIteratorOfListOfShape aListIterator;

        for (aListIterator.Initialize(aFaceList);aListIterator.More(); aListIterator.Next())
        {
            // store corresponding faces
            aFace = TopoDS::Face(aListIterator.Value());
            tempEdgeStruct.aFace.push_back(aFace);
            TransferFaceTriangulationtoFreeCAD(aFace, FaceMesh);
            curvature = MeshCurvature(FaceMesh);

            if (aFace.Orientation() == TopAbs_REVERSED)
            {
                maxOffset = -curvature[1];
                minOffset = -curvature[0];
            }
            else
            {
                maxOffset = curvature[0];
                minOffset = curvature[1];
            }

            if (maxOffset < tempEdgeStruct.MaxOffset)
                tempEdgeStruct.MaxOffset = maxOffset;

            if (minOffset > tempEdgeStruct.MinOffset)
                tempEdgeStruct.MinOffset = minOffset;
        }


        if (tempEdgeStruct.MaxOffset > max)
            curvature[1] = tempEdgeStruct.MaxOffset;
        else
            curvature[1] = cMin;

        if (-tempEdgeStruct.MinOffset > max)
            curvature[0] = tempEdgeStruct.MinOffset;
        else
            curvature[0] = -cMin;

        aPair.first = tempEdgeStruct.anEdge;
        aPair.second = curvature;
        EdgeMap.insert(aPair);

    }
    return true;
}

bool SpringbackCorrection::TransferFaceTriangulationtoFreeCAD(const TopoDS_Face& aFace, MeshCore::MeshKernel& FaceMesh)
{
    FaceMesh.Clear();
    MeshCore::MeshBuilder builder(FaceMesh);
    builder.Initialize(1000);
    Base::Vector3f Points[3];

    TopLoc_Location aLocation;
    // takes the triangulation of the face aFace:
    Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace,aLocation);
    if (!aTr.IsNull()) // if this triangulation is not NULL
    {
        // takes the array of nodes for this triangulation:
        const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
        // takes the array of triangles for this triangulation:
        const Poly_Array1OfTriangle& triangles = aTr->Triangles();
        // create array of node points in absolute coordinate system
        TColgp_Array1OfPnt aPoints(1, aNodes.Length());
        for ( Standard_Integer i = 1; i <= aNodes.Length(); i++)
            aPoints(i) = aNodes(i).Transformed(aLocation);
        // Takes the node points of each triangle of this triangulation.
        // takes a number of triangles:
        Standard_Integer nnn = aTr->NbTriangles();
        Standard_Integer nt,n1,n2,n3;
        for ( nt = 1 ; nt < nnn+1 ; nt++)
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


MeshCore::MeshKernel SpringbackCorrection::BuildMesh(Handle_Poly_Triangulation aTri, std::vector<Base::Vector3f> TrPoints)
{
    MeshCore::MeshKernel FaceMesh;
    MeshCore::MeshBuilder builder(FaceMesh);
    builder.Initialize(1000);
    Base::Vector3f Points[3];

    const Poly_Array1OfTriangle& triangles = aTri->Triangles();
    Standard_Integer nnn = aTri->NbTriangles();
    Standard_Integer nt,n1,n2,n3;

    for ( nt = 1 ; nt < nnn+1 ; nt++)
    {
        // takes the node indices of each triangle in n1,n2,n3:
        triangles(nt).Get(n1,n2,n3);
        // takes the node points:
        Points[0] = TrPoints[n1];
        Points[1] = TrPoints[n2];
        Points[2] = TrPoints[n3];
        // give the occ faces to the internal mesh structure of freecad
        MeshCore::MeshGeomFacet Face(Points[0],Points[1],Points[2]);
        builder.AddFacet(Face);
    }

    // finish FreeCAD Mesh Builder and exit with new mesh
    builder.Finish();

    return FaceMesh;
}

int SpringbackCorrection::GetBoundary(const MeshCore::MeshKernel &mesh, MeshCore::MeshPointArray &meshPnts)
{
    // get mesh-boundary
    // zweck: nur f�r die inneren punkte wird der curvature wert �ber die edge abst�nde berechnet
    std::list< std::vector <unsigned long> >  BoundariesIndex;
    std::list< std::vector <unsigned long> >::iterator bInd;


    MeshCore::MeshAlgorithm algo(mesh);
    algo.GetMeshBorders(BoundariesIndex);
    //Set all MeshPoints to Property 0
    for (unsigned int i=0;i<meshPnts.size();++i)
        meshPnts[i].SetProperty(0);

    //Set Boundary Points to 1
    int inner_points = 0;
    for (bInd = BoundariesIndex.begin(); bInd != BoundariesIndex.end(); ++bInd)
    {
        for (unsigned int i=0;i< bInd->size();++i)
        {
            meshPnts[(*bInd).at(i)].SetProperty(1);
        }

    }
    for (unsigned int i=0;i<meshPnts.size();++i)
    {
        if (meshPnts[i]._ulProp == 0)
            inner_points++;
    }

    return (inner_points);
}

bool SpringbackCorrection::SmoothCurvature()
{
    MeshCore::MeshPointIterator v_it(m_Mesh);
    MeshCore::MeshRefPointToPoints vv_it(m_Mesh);
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it;

    int n = m_MeshStruct.size();

    std::vector<double> SCMax(n);
    std::vector<double> SCMin(n);
    double maxCurv = 0.0, minCurv = 0.0;

    for (v_it.Begin(); v_it.More(); v_it.Next())
    {
        PntNei = vv_it[v_it.Position()];
        maxCurv += m_MeshStruct[v_it.Position()].maxCurv;
        minCurv += m_MeshStruct[v_it.Position()].minCurv;

        for (pnt_it = PntNei.begin(); pnt_it !=PntNei.end(); ++pnt_it)
        {
            maxCurv += m_MeshStruct[(*pnt_it)[0]._ulProp].maxCurv;
            minCurv += m_MeshStruct[(*pnt_it)[0]._ulProp].minCurv;
        }

        SCMax[v_it.Position()] = maxCurv/(PntNei.size()+1);
        SCMin[v_it.Position()] = minCurv/(PntNei.size()+1);

        maxCurv = 0.0;
        minCurv = 0.0;
    }

    for (int i=0; i<n; ++i)
    {
        m_MeshStruct[i].maxCurv = SCMax[i];
        m_MeshStruct[i].minCurv = SCMin[i];
    }

    return true;
}

bool SpringbackCorrection::SmoothMesh(MeshCore::MeshKernel &Mesh, double d_max)
{
    Base::Builder3D log;
    MeshCore::MeshKernel localMesh, Meshtmp;
    MeshCore::MeshPoint mpnt, spnt;
    MeshCore::MeshPointArray locPointArray, PointArray = Mesh.GetPoints();
    MeshCore::MeshFacetArray locFacetArray, FacetArray = Mesh.GetFacets();

    MeshCore::MeshPointIterator v_it(Mesh);
    MeshCore::MeshRefPointToPoints vv_it(Mesh);
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it;

    Base::Vector3f N, L, coor;
    char text[10];

    int n = m_MeshStruct.size();
    int g;

    for (v_it.Begin(); v_it.More(); v_it.Next())
    {

        g = v_it.Position();
        spnt.Set(0.0, 0.0, 0.0);
        locPointArray.push_back(*v_it);
        spnt += *v_it;
        PntNei = vv_it[(*v_it)._ulProp];

        if (PntNei.size() < 3)
            continue;

        for (pnt_it = PntNei.begin(); pnt_it !=PntNei.end(); ++pnt_it)
        {
            locPointArray.push_back((*pnt_it)[0]);
            spnt += (*pnt_it)[0];
        }

        spnt.Scale(1.0/(double(PntNei.size()) + 1.0), 1.0/(double(PntNei.size()) +1.0), 1.0/(double(PntNei.size()) +1.0));

        localMesh.Assign(locPointArray, locFacetArray);


        // need at least one facet to perform the PCA
        MeshCore::MeshGeomFacet face;
        face._aclPoints[0] = locPointArray[0];
        face._aclPoints[1] = locPointArray[1];
        face._aclPoints[2] = locPointArray[2];
        localMesh.AddFacet(face);

        MeshCore::MeshEigensystem pca(localMesh);
        pca.Evaluate();
        Base::Matrix4D T1 = pca.Transform();

        N.x = T1[0][0];
        N.y = T1[0][1];
        N.z = T1[0][2];
        L.Set(v_it->x - spnt.x, v_it->y - spnt.y, v_it->z - spnt.z);
        N.Normalize();


        if (N*L < 0.0)
            N.Scale(-1.0, -1.0, -1.0);

        if ((*v_it)._ulProp == 2286)
        {
            log.addSinglePoint(spnt,4,1,0,0);
            log.addSinglePoint(*v_it,4,0,0,0);
            for (int i=0; i<locPointArray.size(); ++i)
                log.addSinglePoint(locPointArray[i],4);

            for (int i=0; i<3; ++i)
            {
                coor.x = T1[i][0];
                coor.y = T1[i][1];
                coor.z = T1[i][2];

                coor.Normalize();

                log.addSingleArrow(*v_it, *v_it + coor);
            }

            /*log.addSingleArrow(*v_it, *v_it - N);
            snprintf(text,10,"%i",(*v_it)._ulProp);
            log.addText(v_it->x, v_it->y, v_it->z,text);*/
        }

        double d = d_max;
        if (fabs(N*L)< d_max) d = fabs(N*L);
        N.Scale(d,d,d);

        PointArray[v_it.Position()].Set(v_it->x - N.x, v_it->y - N.y, v_it->z - N.z);
        locPointArray.clear();
        localMesh = Meshtmp;
    }

    Mesh.Assign(PointArray, FacetArray);
    log.saveToFile("c:/checkNormals.iv");

    return true;
}

bool SpringbackCorrection::SmoothMesh(MeshCore::MeshKernel &Mesh, std::vector<int> ind, double d_max)
{
    Base::Builder3D log;
    MeshCore::MeshKernel localMesh, Meshtmp;
    MeshCore::MeshPoint mpnt, spnt;
    MeshCore::MeshPointArray locPointArray, PointArray = Mesh.GetPoints();
    MeshCore::MeshFacetArray locFacetArray, FacetArray = Mesh.GetFacets();

    MeshCore::MeshPointIterator v_it(Mesh);
    MeshCore::MeshRefPointToPoints vv_it(Mesh);
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it;

    Base::Vector3f N, L, coor;
    char text[10];

    int n = ind.size();
    int g;

    for (int i=0; i<n; ++i)
    {
        v_it.Set(ind[i]);
        spnt.Set(0.0, 0.0, 0.0);
        locPointArray.push_back(*v_it);
        spnt += *v_it;
        PntNei = vv_it[(*v_it)._ulProp];

        if (PntNei.size() < 3)
            continue;

        for (pnt_it = PntNei.begin(); pnt_it !=PntNei.end(); ++pnt_it)
        {
            locPointArray.push_back((*pnt_it)[0]);
            spnt += (*pnt_it)[0];
        }

        spnt.Scale(1.0/(double(PntNei.size()) + 1.0), 1.0/(double(PntNei.size()) +1.0), 1.0/(double(PntNei.size()) +1.0));

        localMesh.Assign(locPointArray, locFacetArray);


        // need at least one facet to perform the PCA
        MeshCore::MeshGeomFacet face;
        face._aclPoints[0] = locPointArray[0];
        face._aclPoints[1] = locPointArray[1];
        face._aclPoints[2] = locPointArray[2];
        localMesh.AddFacet(face);

        MeshCore::MeshEigensystem pca(localMesh);
        pca.Evaluate();
        Base::Matrix4D T1 = pca.Transform();

        N.x = T1[0][0];
        N.y = T1[0][1];
        N.z = T1[0][2];
        L.Set(v_it->x - spnt.x, v_it->y - spnt.y, v_it->z - spnt.z);
        N.Normalize();


        if (N*L < 0.0)
            N.Scale(-1.0, -1.0, -1.0);

        if ((*v_it)._ulProp == 2286)
        {
            log.addSinglePoint(spnt,4,1,0,0);
            log.addSinglePoint(*v_it,4,0,0,0);
            for (int i=0; i<locPointArray.size(); ++i)
                log.addSinglePoint(locPointArray[i],4);

            for (int i=0; i<3; ++i)
            {
                coor.x = T1[i][0];
                coor.y = T1[i][1];
                coor.z = T1[i][2];

                coor.Normalize();

                log.addSingleArrow(*v_it, *v_it + coor);
            }

            /*log.addSingleArrow(*v_it, *v_it - N);
            snprintf(text,10,"%i",(*v_it)._ulProp);
            log.addText(v_it->x, v_it->y, v_it->z,text);*/
        }

        double d = d_max;
        if (fabs(N*L)< d_max) d = fabs(N*L);
        N.Scale(d,d,d);

        PointArray[v_it.Position()].Set(v_it->x - N.x, v_it->y - N.y, v_it->z - N.z);
        locPointArray.clear();
        localMesh = Meshtmp;
    }

    Mesh.Assign(PointArray, FacetArray);
    log.saveToFile("c:/checkNormals.iv");

    return true;
}

bool SpringbackCorrection::GetFaceAng(MeshCore::MeshKernel &mesh, int deg_tol)
{
    Base::Vector3f normal, base;
    double deg;
    int n = mesh.CountFacets(), n1,n2,n3;
    bool b = true;

    for (int i=0; i<n; ++i)
    {
        MeshCore::MeshGeomFacet face = mesh.GetFacet(i);
        face.CalcNormal();
        normal = face.GetNormal();
        normal.Normalize();
        base = normal;
        base.z = 0.0;
        base.Normalize();
        deg = ((PI/2) - acos(normal*base))*180.0/PI;

        if (deg > deg_tol)
            b = false;

        m_FaceAng.push_back(((PI/2) - acos(normal*base))*180.0/PI);
    }

    return b;
}

std::vector<int> SpringbackCorrection::FaceCheck(MeshCore::MeshKernel mesh, int degree)
{
    std::vector<int> faceInd;
    double tol = cos((90.0 - double(degree))*PI/180.0);
    MeshCore::MeshFacetIterator fIt(mesh);
    MeshCore::MeshPointIterator mIt(mesh);
    Base::Vector3f normal, base;
    Base::Builder3D log;

    int n = mesh.CountFacets();
    unsigned long n1,n2,n3;

    for (int i=0; i<n; ++i)
    {
        MeshCore::MeshGeomFacet face = mesh.GetFacet(i);
        face.CalcNormal();
        normal = face.GetNormal();
        normal.Normalize();
        base = normal;
        base.z = 0.0;
        base.Normalize();
        double deg, lamda;

        if (normal*base > tol)
        {
            faceInd.push_back(i);
            deg = ((PI/2) - acos(normal*base))*180.0/PI;
            lamda = (degree - m_FaceAng[i])/(fabs(deg - m_FaceAng[i]));
            mesh.GetFacetPoints(i, n1,n2,n3);
            m_normals[n1].Scale(lamda, lamda, lamda);
            m_normals[n2].Scale(lamda, lamda, lamda);
            m_normals[n3].Scale(lamda, lamda, lamda);

            mIt.Set(n1);
            log.addSinglePoint(*mIt,4,1,0,0);
            mIt.Set(n2);
            log.addSinglePoint(*mIt,4,1,0,0);
            mIt.Set(n3);
            log.addSinglePoint(*mIt,4,1,0,0);
        }
    }

    log.saveToFile("c:/angles.iv");
    return faceInd;
}

bool SpringbackCorrection::Perform(int deg_Tol)
{
    MeshCore::MeshKernel RefMesh;
    best_fit befi(m_Mesh, m_Shape);

    GetFaceAng(m_Mesh, deg_Tol+1);
    Init();      // tesseliere shape -> Basistriangulierung CAD-Mesh
    CalcCurv();  // berechne Kr�mmungswerte �ber Edgekr�mmungen

    RefMesh = m_Mesh;  // �begebe CAD-Mesh vor der Verformung

    // speichere normalen seperat
    m_normals.clear();
    m_normals.resize(m_MeshStruct.size());

    for (int i=0; i<m_normals.size(); ++i)
    {
        m_normals[i] = m_MeshStruct[i].normal;
    }


    // �bergebe Normalen und CAD-Mesh f�r Fehlerberechnng
    befi.m_normals = m_normals;
    befi.m_CadMesh = m_Mesh;

    int n = m_Mesh.CountPoints();

    Base::Vector3f tr(0.0, 0.0, +5.0);
    for (int i=0; i<n; ++i)
        befi.m_CadMesh.MovePoint(i,tr);

    befi.CompTotalError();

    for (int i=0; i<n; ++i)
    {
        m_MeshStruct[i].error = befi.m_error[i];

        if (m_MeshStruct[i].error < 0)
        {
            if (-m_MeshStruct[i].error > (m_MeshStruct[i].maxCurv - 5.0))
                m_Offset.push_back(m_MeshStruct[i].maxCurv - 5.0);
            else
                m_Offset.push_back(-m_MeshStruct[i].error);
        }
        else
        {
            if (m_MeshStruct[i].error > (fabs(m_MeshStruct[i].minCurv + 5.0)))
                m_Offset.push_back(m_MeshStruct[i].minCurv + 5.0);
            else
                m_Offset.push_back(-m_MeshStruct[i].error);
        }
    }

    for (int i=0; i<n; ++i)
    {
        m_normals[i].Scale(m_Offset[i], m_Offset[i], m_Offset[i]);
    }

    MeshCore::MeshPointArray pntAr = befi.m_CadMesh.GetPoints();
    MeshCore::MeshFacetArray facAr = befi.m_CadMesh.GetFacets();

    for (int i=0; i<n; ++i)
    {
        pntAr[i].Set(pntAr[i].x + m_normals[i].x, pntAr[i].y + m_normals[i].y, pntAr[i].z + m_normals[i].z);
    }

    befi.m_CadMesh.Assign(pntAr, facAr);
    SmoothMesh(befi.m_CadMesh,befi.m_FailProj, 10.0);
    SmoothMesh(befi.m_CadMesh,befi.m_FailProj, 10.0);
    SmoothMesh(befi.m_CadMesh,befi.m_FailProj, 10.0);
    SmoothMesh(befi.m_CadMesh,befi.m_FailProj, 10.0);
    SmoothMesh(befi.m_CadMesh,befi.m_FailProj, 10.0);
    SmoothMesh(befi.m_CadMesh,befi.m_FailProj, 10.0);
    m_Mesh = befi.m_CadMesh;

    return true;
    SmoothMesh(befi.m_CadMesh, 10.0);
    SmoothMesh(befi.m_CadMesh, 10.0);

    bool b = true;

    while (b==true)
    {
        break;
        FaceCheck(befi.m_CadMesh,deg_Tol+1);
        b = GetFaceAng(m_Mesh, deg_Tol+1);

        MeshCore::MeshPointArray pntAr2 = RefMesh.GetPoints();
        MeshCore::MeshFacetArray facAr2 = RefMesh.GetFacets();


        for (int i=0; i<n; ++i)
        {
            pntAr2[i].Set(pntAr2[i].x + m_normals[i].x, pntAr2[i].y + m_normals[i].y, pntAr2[i].z + m_normals[i].z);
        }

        RefMesh.Assign(pntAr2, facAr2);
    }

    m_Mesh = befi.m_CadMesh;


    return true;
}

//
//bool SpringbackCorrection::Init()
//{
// Base::Builder3D log;
// Base::Vector3f  p1,p2;
// double curv;
// // Triangulate Shape (vorl�ufig)
// best_fit::Tesselate_Shape(m_Shape, m_Mesh, 1);
//
// MeshCurvature(m_Mesh);
//
// std::vector<Base::Vector3f> normals = best_fit::Comp_Normals(m_Mesh);
// MeshCore::MeshPointIterator p_it(m_Mesh);
// int i = 0;
//
// for(p_it.Begin(); p_it.More(); p_it.Next()){
//  normals[i].Scale(m_CurvMax[i], m_CurvMax[i], m_CurvMax[i]);
//  //p1.Set(PointArray[i].x,       PointArray[i].y,       PointArray[i].z);
//  p2.Set(p_it->x + normals[i].x,   p_it->y + normals[i].y,   p_it->z + normals[i].z);
//
//
//  if(m_CurvMax[i] < 100)
//  log.addSingleLine(*p_it, p2, 1 ,0, 0, 0 );
//
//  //cout << p_it->x << ", " << p_it->y << ", " << p_it->z << endl;
//  //cout <<normals[i].x << ", " << normals[i].y << ", " << normals[i].z << endl;
//  //cout << m_CurvMax[i] << endl;
//  ++i;
//
// }
//
// log.saveToFile("c:/servolation.iv");
//}

// return true;
bool SpringbackCorrection::GetCurvature(TopoDS_Face aFace)
{
    Base::Builder3D log;
    Handle_Geom_Surface geom_surface;
    geom_surface = BRep_Tool::Surface(aFace);
    gp_Pnt proPnt;
    double u_par,v_par,k1,k2;
    gp_Vec D1U,D1V,D2U,D2V,D2UV;
    Base::Vector3f p1,p2;

    MeshCore::MeshPointArray PointArray = m_Mesh.GetPoints();
    MeshCore::MeshFacetArray FacetArray = m_Mesh.GetFacets();

    MeshCore::MeshPointIterator p_it(m_Mesh);

    m_CurvMax.resize(PointArray.size());

    for (unsigned int i=0; i<PointArray.size(); ++i)
    {

        proPnt.SetX(PointArray[i].x);
        proPnt.SetY(PointArray[i].y);
        proPnt.SetZ(PointArray[i].z);

        GeomAPI_ProjectPointOnSurf aProjection(proPnt,geom_surface);
        aProjection.LowerDistanceParameters(u_par,v_par);

        // Berechne Kr�mmung
        geom_surface->D2(u_par,v_par,proPnt,D1U,D1V,D2U,D2V,D2UV);

        // erste & zweite Hauptkr�mmung
        k1 = D1U.CrossMagnitude(D2U) / D1U.Magnitude();
        k2 = D1V.CrossMagnitude(D2V) / D1V.Magnitude();

        /*if(k1>k2) PointArray[i].SetProperty(1/k1);
        else      PointArray[i].SetProperty(1/k2);*/

        if (abs(k1)>abs(k2))
        {
            m_CurvMax[i] = 1/k1;
        }
        else
            if (k2!=0)
                m_CurvMax[i] = 1/k2;
            else
                m_CurvMax[i] = 100;


        D1U.Cross(D1V);
        D1U.Normalize();
        D1U.Scale(m_CurvMax[i]);

        p1.x = proPnt.X() + D1U.X();
        p1.y = proPnt.Y() + D1U.Y();
        p1.z = proPnt.Z() + D1U.Z();

        p2.x = proPnt.X();
        p2.y = proPnt.Y();
        p2.z = proPnt.Z();

        log.addSingleLine(p2, p1, 1 ,0, 0, 0 );
    }


    MeshCore::MeshPointIterator v_it(m_Mesh);
    MeshCore::MeshRefPointToPoints vv_it(m_Mesh);
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei2;
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei3;
    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei4;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it1;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it2;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it3;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it4;
    std::vector<unsigned long> nei;
    double curv;

    std::vector<double> CurvCop = m_CurvMax;

    for (v_it.Begin(); v_it.More(); v_it.Next())
    {
        PntNei = vv_it[v_it.Position()];
        curv = m_CurvMax[v_it.Position()];

        for (pnt_it1 = PntNei.begin(); pnt_it1 !=PntNei.end(); ++pnt_it1)
        {
            if (m_CurvMax[(*pnt_it1)[0]._ulProp] < curv)
                curv = m_CurvMax[(*pnt_it1)[0]._ulProp];

            PntNei2 = vv_it[(*pnt_it1)[0]._ulProp];
            for (pnt_it2 = PntNei2.begin(); pnt_it2 !=PntNei2.end(); ++pnt_it2)
            {
                if (m_CurvMax[(*pnt_it2)[0]._ulProp] < curv)
                    curv = m_CurvMax[(*pnt_it2)[0]._ulProp];


                PntNei3 = vv_it[(*pnt_it2)[0]._ulProp];
                for (pnt_it3 = PntNei3.begin(); pnt_it3 !=PntNei3.end(); ++pnt_it3)
                {
                    if (m_CurvMax[(*pnt_it3)[0]._ulProp] < curv)
                        curv = m_CurvMax[(*pnt_it3)[0]._ulProp];

                    PntNei4 = vv_it[(*pnt_it3)[0]._ulProp];
                    for (pnt_it4 = PntNei4.begin(); pnt_it4 !=PntNei4.end(); ++pnt_it4)
                    {
                        if (m_CurvMax[(*pnt_it4)[0]._ulProp] < curv)
                            curv = m_CurvMax[(*pnt_it4)[0]._ulProp];
                    }
                }
            }
        }

        CurvCop[v_it.Position()] = curv;
    }

    m_CurvMax = CurvCop;


    return true;
}


// 1. Computes the curvature of the mesh at the verticies
// 2. Stores the minimum curvatures along the neighbours for all verticies in a vector
std::vector<double> SpringbackCorrection::MeshCurvature(const MeshCore::MeshKernel& mesh)
{
    // get all points
    std::vector<double> curv(2);
    //MeshCore::MeshKernel& rMesh = mesh;
    std::vector< Wm4::Vector3<float> > aPnts;
    std::vector<Base::Vector3f> aPnts_tmp;
    MeshCore::MeshPointIterator cPIt( mesh );
    for ( cPIt.Init(); cPIt.More(); cPIt.Next() )
    {
        Wm4::Vector3<float> cP( cPIt->x, cPIt->y, cPIt->z );
        Base::Vector3f cP_tmp( cPIt->x, cPIt->y, cPIt->z );
        aPnts.push_back( cP );
        aPnts_tmp.push_back(cP_tmp);
    }

    // get all point connections
    std::vector<int> anIdx;
    const std::vector<MeshCore::MeshFacet>& MeshFacetArray = mesh.GetFacets();
    for ( std::vector<MeshCore::MeshFacet>::const_iterator jt = MeshFacetArray.begin(); jt != MeshFacetArray.end(); ++jt )
    {
        for (int i=0; i<3; i++)
        {
            anIdx.push_back( (int)jt->_aulPoints[i] );
        }
    }

    // compute vertex based curvatures
    Wm4::MeshCurvature<float> meshCurv(mesh.CountPoints(), &(aPnts[0]), mesh.CountFacets(), &(anIdx[0]));

    // get curvature information now
    const Wm4::Vector3<float>* aMaxCurvDir = meshCurv.GetMaxDirections();
    const Wm4::Vector3<float>* aMinCurvDir = meshCurv.GetMinDirections();
    const float* aMaxCurv = meshCurv.GetMaxCurvatures();
    const float* aMinCurv = meshCurv.GetMinCurvatures();

    double maxCurv = 0.0;
    double avgCurv = 0.0;
    double tmp1 =  1e+10;
    double tmp2 = -1e+10;

    double mean_curvMax = 0.0, mean_curvMin = 0.0;
    int c1 = 0, c2 = 0;

    m_CurvPos.clear();
    m_CurvNeg.clear();
    m_CurvMax.clear();
    m_CurvPos.resize(mesh.CountPoints());
    m_CurvNeg.resize(mesh.CountPoints());
    m_CurvMax.resize(mesh.CountPoints());

    for ( unsigned long i=0; i<mesh.CountPoints(); i++ )
    {

        if (aMaxCurv[i] > 0) m_CurvPos[i] = 1 / aMaxCurv[i];
        else                 m_CurvPos[i] = 1e+10;

        if (aMinCurv[i] < 0) m_CurvNeg[i] = 1 / aMinCurv[i];
        else                 m_CurvNeg[i] = -1e+10;

        if (m_CurvPos[i] < tmp1)
            tmp1 = m_CurvPos[i];

        if (m_CurvNeg[i] > tmp2)
            tmp2 = m_CurvNeg[i];

        if (aMaxCurv[i] > 0)
        {
            ++c1;
            mean_curvMax += 1/aMaxCurv[i];
        }

        if (aMinCurv[i] < 0)
        {
            ++c2;
            mean_curvMin += 1/aMinCurv[i];
        }
    }

    if (c1==0)
        mean_curvMax = 1e+3;
    else
        mean_curvMax /= mesh.CountPoints();

    if (c2==0)
        mean_curvMin = -1e+3;
    else
        mean_curvMin /= mesh.CountPoints();

    curv[0] = tmp1;
    curv[1] = tmp2;

    return curv;
}

bool SpringbackCorrection::MirrorMesh(std::vector<double> error)
{
    // Flags: 0 - not yet checked
    //       1 - no correction applied, but neccessary
    //        2 - first correction applied and still neccessary
    //        3 - done successfully
    //        4 - done without success
    //        5 - boundary point

    int n = error.size();
    m_error = error;

    std::vector<int> InterPnts;
    std::vector<unsigned long> InterFace;
    std::vector<unsigned long> IFCopy;

    bool boo = true;

    std::vector<unsigned long> ind;
    std::vector<unsigned long> copy;

    MeshRef = m_Mesh;
    std::vector<Base::Vector3f> NormalsRef = m_normals;
    std::vector<Base::Vector3f> NormalOrig = m_normals;

    PointArray = MeshRef.GetPoints();
    MeshCore::MeshPointArray PntArray   = m_Mesh.GetPoints();
    MeshCore::MeshFacetArray FctArray   = m_Mesh.GetFacets();
    MeshCore::MeshFacetArray FacetArray = MeshRef.GetFacets();

    MeshCore::MeshFacetIterator f_it(MeshRef);
    MeshCore::MeshPointIterator v_it(MeshRef);
    MeshCore::MeshRefPointToPoints vv_it(MeshRef);
    MeshCore::MeshRefPointToFacets vf_it(MeshRef);

    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;
    std::set<MeshCore::MeshFacetArray::_TConstIterator> FacetNei;

    // Iterates in facets in form of MeshGeomFacets
    std::set<MeshCore::MeshFacetArray::_TConstIterator>::iterator face_it;
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it;

    std::vector<Base::Vector3f> NeiLoc;
    std::vector<Base::Vector3f> NorLoc;
    std::vector<Base::Vector3f> TmpPntVec;
    std::vector<unsigned long>  TmpIndVec;
    std::vector<unsigned long>  nei;

    bool k=true;
    int count = 0;

    while (k==true)
    {
        // ------ Self-Intersection Check + Correction ------------------------------------------------------

        if (count ==2) break;
        count++;

        Base::Builder3D log3d;
        Base::Builder3D loga;
        Base::Vector3f Pnt, NeiPnt, NeiPntNext, NeiPntPrev, TmpPnt, TmpNor;

        std::vector<Base::Vector3f> Neis;
        double minScale, tmp;
        bool sign;

        ublas::matrix<double> A(3, 3);
        ublas::matrix<double> b(1, 3);

        ind.clear();
        for (unsigned int i=0; i<m_Mesh.CountPoints(); ++i)
            ind.push_back(i);


        /* do correction for selected points ---------------------------------*/
        for (unsigned int j=0; j<ind.size(); ++j)
        {
            sign = (error[ind[j]] > 0);  // (+) -> true , (-) -> false

            MeshRef.Assign(PointArray, FacetArray);

            v_it.Set(ind[j]);

            Pnt      = *v_it;
            PntNei   = vv_it[ind[j]];
            FacetNei = vf_it[ind[j]];

            nei.clear();

            // falls boundary-point tue nichts bzw. gehe zum n�chsten punkt
            if (FacetNei.size() != PntNei.size())
            {
                PointArray[ind[j]]._ucFlag = 5;
                m_CurvMax[ind[j]] = abs(error[ind[j]]);   // vor�bergehend
                continue;
            }

            // ordne nachbarn
            ReorderNeighbourList(PntNei,FacetNei,nei,ind[j]);

            NeiLoc.clear();
            NorLoc.clear();
            NeiLoc.push_back(Pnt);
            NorLoc.push_back(NormalsRef[ind[j]]);

            for (unsigned int r=0; r<nei.size(); ++r)
            {
                v_it.Set(nei[r]);
                NeiLoc.push_back(*v_it);
                NorLoc.push_back(NormalsRef[nei[r]]);
            }


            if (count == 2)
            {
                if (sign ==true) minScale = -(1e+10);
                else      minScale =   1e+10;
                minScale = LocalCorrection(NeiLoc,NorLoc,sign,minScale);

                if ((abs(0.8*minScale) - abs(error[ind[j]])) > 0)
                {
                    PointArray[ind[j]]._ucFlag = 3;
                    m_CurvMax[ind[j]] = abs(minScale);
                    //cout << "t, ";
                    continue;
                }
                else
                {
                    PointArray[ind[j]]._ucFlag = 4;
                    m_CurvMax[ind[j]] = abs(minScale);
                    cout << "f, ";
                    continue;
                }
            }


            if (sign ==true) minScale = -(1e+10);
            else      minScale =   1e+10;

            minScale = LocalCorrection(NeiLoc,NorLoc,sign,minScale);

            if ((abs(0.8*minScale) - abs(error[ind[j]])) > 0)
            {
                PointArray[ind[j]]._ucFlag = 3;
                m_CurvMax[ind[j]] = abs(minScale);
                continue;
            }

            //---------------------------- 1. Korrektur -------------------------
            tmp = 0;
            std::vector<Base::Vector3f> ConvComb = FillConvex(NeiLoc,NorLoc,20);

            for (unsigned int l=0; l<ConvComb.size(); ++l)
            {
                if (sign ==true) minScale = -(1e+10);
                else      minScale =   1e+10;

                NeiLoc[0] = ConvComb[l];
                minScale = LocalCorrection(NeiLoc,NorLoc,sign,minScale);

                if (abs(minScale) > abs(tmp))
                {
                    tmp = minScale;
                    TmpPnt = NeiLoc[0];
                }
            }

            NeiLoc[0] = PointArray[ind[j]];
            cout << "new minScale: " << tmp << endl;

            if ((abs(0.8*tmp) - abs(error[ind[j]])) > 0)
            {
                PointArray[ind[j]].Set(TmpPnt.x, TmpPnt.y, TmpPnt.z);
                PointArray[ind[j]]._ucFlag = 3;
                m_CurvMax[ind[j]] = abs(tmp);
                continue;
            }
            else
            {
                PointArray[ind[j]]._ucFlag = 2;
                m_CurvMax[ind[j]] = abs(tmp);
            }

            //------------------------------- END -------------------------------

            // update
            for (unsigned int r=0; r<nei.size(); ++r)
            {
                v_it.Set(nei[r]);
                NeiLoc[r+1] = *v_it;
            }

            MeshRef.Assign(PointArray, FacetArray);

            //---------------------------- 2. Korrektur -------------------------
            tmp = GlobalCorrection(NeiLoc,NorLoc,sign,ind[j]);

            /*MeshRef.Assign(PointArray, FacetArray);
            m_Mesh = MeshRef;
            return true;*/

            cout << "2: " << tmp << endl;

            if ((abs(0.8*tmp) - abs(error[ind[j]])) > 0)
            {
                PointArray[ind[j]].Set(NeiLoc[0].x, NeiLoc[0].y, NeiLoc[0].z);
                NormalsRef[ind[j]].Set(NorLoc[0].x, NorLoc[0].y, NorLoc[0].z);
                PointArray[ind[j]]._ucFlag = 3;
                m_CurvMax[ind[j]] = abs(tmp);
                continue;
            }
            else
            {
                if (m_CurvMax[ind[j]] < abs(tmp))
                {
                    m_CurvMax[ind[j]] = abs(tmp);
                    PointArray[ind[j]].Set(NeiLoc[0].x, NeiLoc[0].y, NeiLoc[0].z);
                    NormalsRef[ind[j]].Set(NorLoc[0].x, NorLoc[0].y, NorLoc[0].z);
                }
                // flag bleibt 2
            }

            //------------------------------- END -------------------------------

        } // end for

        cout << "Replace Mesh..." << endl;
        PntArray   = PointArray;
        m_normals  = NormalsRef;
        m_Mesh = MeshRef;

    } // end while


    double ver, mx=100;
    for (unsigned int i=0; i<error.size(); ++i)
    {
        ver = m_CurvMax[i] / abs(error[i]);
        if (ver < mx)
            mx = ver;
    }

    /*cout << "skalierung: " << mx << endl;
    for(int i=0; i<error.size(); ++i)
     error[i] *= mx;*/



    // deform CAD-Mesh with regard to the scaled normals
    MeshCore::MeshPointIterator p_it(m_Mesh);
    Base::Builder3D log,log2;
    std::vector<int> IndexVec,NumVec, NumVecCopy;
    int c, lastNum, lastInd;

    // kritische punkte plus anzahl seiner kritischen nachbarn speichern
    //ind.clear();
    for (unsigned int j=0; j<ind.size(); ++j)
    {
        if (PointArray[ind[j]]._ucFlag == 3 || PointArray[ind[j]]._ucFlag == 5)
        {
            //ind.push_back(ind[j]);
            continue;
        }
        else
        {
            c=0;
            PntNei = vv_it[ind[j]];
            for (pnt_it = PntNei.begin(); pnt_it != PntNei.end(); ++pnt_it)
            {
                if (PointArray[(*pnt_it)->_ulProp]._ucFlag == 4)
                    c++;
            }

            IndexVec.push_back(ind[j]);
            NumVec.push_back(c);
        }
    }

    // normalenskalierung
    for (unsigned int j=0; j<ind.size(); ++j)
    {
        if ( ( abs(0.8*m_CurvMax[ind[j]]) - abs(error[ind[j]]) ) < 0)
        {
            if (error[ind[j]] < 0)
                m_normals[ind[j]].Scale(0.8*m_CurvMax[ind[j]], 0.8*m_CurvMax[ind[j]], 0.8*m_CurvMax[ind[j]]);
            else
                m_normals[ind[j]].Scale(-0.8*m_CurvMax[ind[j]], -0.8*m_CurvMax[ind[j]], -0.8*m_CurvMax[ind[j]]);
        }
        else
            m_normals[ind[j]].Scale(-error[ind[j]], -error[ind[j]], -error[ind[j]]);
    }


    for (unsigned int j=0; j<ind.size(); ++j)
    {
        log.addSingleLine(PointArray[ind[j]],PointArray[ind[j]] + m_normals[ind[j]],2,1,1,1);  // white arrows
        PntArray[ind[j]].Set(PntArray[ind[j]].x + m_normals[ind[j]].x,
                             PntArray[ind[j]].y + m_normals[ind[j]].y,
                             PntArray[ind[j]].z + m_normals[ind[j]].z);
    }

    m_Mesh.Assign(PntArray, FctArray);
    MeshRef = m_Mesh;
    log.saveToFile("c:/deformation.iv");


    // kritische punkte nach der anzahl seiner kritischen nachbarn sortieren
    NumVecCopy = NumVec;
    std::vector<int> SortNumVec(NumVec.size(), -1);
    std::vector<int> SortIndVec(NumVec.size(), -1);
    cout << NumVec.size() << endl;

    /*cout << "vor dem sortieren: ";
    for(int j=0; j<NumVec.size(); ++j)
    {
     cout << NumVec[j] << ", ";
    }
    cout << endl;*/

    for (unsigned int j=0; j<NumVecCopy.size(); ++j)
    {

        lastNum = NumVec.back();
        lastInd = IndexVec.back();

        NumVec.pop_back();
        IndexVec.pop_back();

        c=0;
        for (unsigned int i=0; i<NumVecCopy.size(); ++i)
        {
            if (NumVecCopy[i] < lastNum)
                ++c;
        }

        while (SortNumVec[c] == lastNum)
            ++c;

        SortNumVec[c] = lastNum;
        SortIndVec[c] = lastInd;
    }

    /*cout << "danach: ";
    for(int j=0; j<SortNumVec.size(); ++j)
    {
     cout << SortNumVec[j] << ", ";
    }
    cout << endl;*/

    NeiLoc.clear();
    NorLoc.clear();

    Base::Builder3D log1;

    MeshCore::MeshKernel mesh;
    MeshCore::MeshPoint mpnt;
    MeshCore::MeshPointArray mPointArray;
    MeshCore::MeshFacetArray mFacetArray;

    float w;

    for (unsigned int j=0; j<SortIndVec.size(); ++j)
    {
        Base::Vector3f bvec(0.0, 0.0, 0.0);
        NeiLoc.clear();
        NeiLoc.push_back(bvec);

        PntNei = vv_it[SortIndVec[j]];
        FacetNei = vf_it[SortIndVec[j]];
        nei.clear();

        // ordne nachbarn
        ReorderNeighbourList(PntNei,FacetNei,nei,SortIndVec[j]);

        mPointArray.clear();
        for (unsigned int i=0; i<nei.size(); ++i)
        {
            mpnt = PntArray[nei[i]];
            mPointArray.push_back(mpnt);
            //!!!!!!!!!!!!!!!!! NeiLoc.size() must be greater than 3!!!!!!!!!!!!!!!
            if (PntArray[nei[i]]._ucFlag != 4)
            {
                NeiLoc.push_back(PntArray[nei[i]]);
                log1.addSinglePoint(PntArray[nei[i]].x, PntArray[nei[i]].y, PntArray[nei[i]].z, 2,1,1,1);
                mPointArray.push_back(mpnt);
            }
            else
            {
                bvec += PntArray[nei[i]];
                log1.addSinglePoint(PntArray[nei[i]].x, PntArray[nei[i]].y, PntArray[nei[i]].z, 2,1,0,0);
            }
        }

        log1.saveToFile("c:/nachbarn.iv");

        mesh.Assign(mPointArray, mFacetArray);

        // need at least one facet to perform the PCA
        MeshCore::MeshGeomFacet face;
        mesh.AddFacet(face);

        // berechne hauptachsen
        MeshCore::MeshEigensystem pca(mesh);
        pca.Evaluate();
        Base::Matrix4D T1 = pca.Transform();
        std::vector<Base::Vector3f> v(3);

        v[0].x = T1[0][0];
        v[0].y = T1[0][1];
        v[0].z = T1[0][2];
        v[1].x = T1[1][0];
        v[1].y = T1[1][1];
        v[1].z = T1[1][2];
        v[2].x = T1[2][0];
        v[2].y = T1[2][1];
        v[2].z = T1[2][2];

        v[0].Normalize();
        v[1].Normalize();
        v[2].Normalize();

        double deg,temp=0;
        int s;

        for (int i=0; i<3; ++i)
        {
            deg = v[i]*NormalOrig[SortIndVec[j]];
            if (abs(deg)>temp)
            {
                temp = abs(deg);
                s=i;
            }
        }

        if (v[s].z < 0.0)
            v[s] *= -1;

        for (unsigned int i=0; i<NeiLoc.size(); ++i)
            NorLoc.push_back(v[s]);

        // gewichtung (im verh�ltnis 1:2)
        w = 1.0 /(float(NeiLoc.size()) + float(PntNei.size()) - 1.0);
        bvec.Scale(w,w,w);

        for (unsigned int i=1; i<NeiLoc.size(); ++i)
        {
            bvec.x = bvec.x + 2*w*NeiLoc[i].x;
            bvec.y = bvec.y + 2*w*NeiLoc[i].y;
            bvec.z = bvec.z + 2*w*NeiLoc[i].z;
        }

        NeiLoc[0] = bvec;
        log1.addSinglePoint(NeiLoc[0],2,0,1,0);
        std::vector<Base::Vector3f> Convs = FillConvex(NeiLoc, NorLoc, 20);

        for (unsigned int i=0; i<Convs.size(); ++i)
            log1.addSinglePoint(Convs[i],2,0,0,0);

        log1.saveToFile("c:/nachbarn.iv");

        PntArray[SortIndVec[j]].Set(Convs[0].x, Convs[0].y, Convs[0].z);
        PntArray[SortIndVec[j]]._ucFlag = 3;

    }

    m_Mesh.Assign(PntArray, FctArray);

    return true;


    for (p_it.Begin(); p_it.More(); p_it.Next())
    {
        if (PointArray[p_it.Position()]._ucFlag == 3)
        {
            log.addSingleLine(*p_it,*p_it + m_normals[p_it.Position()],2,1,1,1);  // red arrows
            PntArray[p_it.Position()].Set(PntArray[p_it.Position()].x + m_normals[p_it.Position()].x,
                                          PntArray[p_it.Position()].y + m_normals[p_it.Position()].y,
                                          PntArray[p_it.Position()].z + m_normals[p_it.Position()].z);
        }

        if (PointArray[p_it.Position()]._ucFlag == 4)
        {
            log.addSingleLine(*p_it,*p_it + m_normals[p_it.Position()],2,1,0,0);  // red arrows
            PntArray[p_it.Position()].Set(PntArray[p_it.Position()].x + m_normals[p_it.Position()].x,
                                          PntArray[p_it.Position()].y + m_normals[p_it.Position()].y,
                                          PntArray[p_it.Position()].z + m_normals[p_it.Position()].z);
        }
    }

    m_Mesh.Assign(PntArray, FctArray);
    log.saveToFile("c:/deformation.iv");

    return true;
}


double SpringbackCorrection::LocalCorrection(std::vector<Base::Vector3f> Neib ,
        std::vector<Base::Vector3f> Normal, bool sign, double minScale)
{
    Base::Vector3f Pnt, NeiPnt, NeiPntNext, NeiPntPrev;

    ublas::matrix<double> A(3, 3);
    ublas::matrix<double> b(1, 3);

    // 2. Spalte bleibt unver�ndert
    A(0,2) = -Normal[0].x;
    A(1,2) = -Normal[0].y;
    A(2,2) = -Normal[0].z;

    // forward
    for (unsigned int k=1; k<Neib.size(); ++k)
    {
        NeiPnt = Neib[k];

        if ((k+1) == Neib.size())
        {
            NeiPntNext = Neib[1];
        }
        else
        {
            NeiPntNext = Neib[k+1];
        }

        A(0,2) = -Normal[0].x;
        A(0,0) = Normal[k].x;
        A(0,1) = (NeiPntNext.x - NeiPnt.x);
        A(1,2) = -Normal[0].y;
        A(1,0) = Normal[k].y;
        A(1,1) = (NeiPntNext.y - NeiPnt.y);
        A(2,2) = -Normal[0].z;
        A(2,0) = Normal[k].z;
        A(2,1) = (NeiPntNext.z - NeiPnt.z);

        b(0,0) = Neib[0].x - NeiPnt.x;
        b(0,1) = Neib[0].y - NeiPnt.y;
        b(0,2) = Neib[0].z - NeiPnt.z;

        //cout << "A-Matrix: " << endl;
        //cout << A(0,0) << ", " << A(0,1) << ", " << A(0,2) << endl;
        //cout << A(1,0) << ", " << A(1,1) << ", " << A(1,2) << endl;
        //cout << A(2,0) << ", " << A(2,1) << ", " << A(2,2) << endl;
        //cout << endl;

        //cout << "b-Vector: " << endl;
        //cout << b(0,0) << ", " << b(0,1) << ", " << b(0,2) << endl;
        //cout << endl;

        atlas::gesv(A,b);

        //cout << "solution: " << endl;
        //cout << b(0,0) << ", " << b(0,1) << ", " << b(0,2) << endl;
        //cout << endl;

        if ((b(0,2)<=0) && (sign == true) && (b(0,1)>=0) && (b(0,1)<=1))
        {
            if (minScale < b(0,2))
                minScale = b(0,2);
        }
        else if (b(0,2)>=0 && sign == false && (b(0,1)>=0) && (b(0,1)<=1))
        {
            if (minScale > b(0,2))
                minScale = b(0,2);
        }
    }

    // backward
    for (unsigned int k=1; k<Neib.size(); ++k)
    {
        NeiPnt = Neib[k];

        if (k == 1)
        {
            NeiPntPrev = Neib[Neib.size()-1];
        }
        else
        {
            NeiPntPrev = Neib[k-1];
        }

        A(0,2) = -Normal[0].x;
        A(0,0) = Normal[k].x;
        A(0,1) = (NeiPntPrev.x - NeiPnt.x);
        A(1,2) = -Normal[0].y;
        A(1,0) = Normal[k].y;
        A(1,1) = (NeiPntPrev.y - NeiPnt.y);
        A(2,2) = -Normal[0].z;
        A(2,0) = Normal[k].z;
        A(2,1) = (NeiPntPrev.z - NeiPnt.z);

        b(0,0) = Neib[0].x - NeiPnt.x;
        b(0,1) = Neib[0].y - NeiPnt.y;
        b(0,2) = Neib[0].z - NeiPnt.z;

        /*cout << "A-Matrix: " << endl;
        cout << A(0,0) << ", " << A(0,1) << ", " << A(0,2) << endl;
        cout << A(1,0) << ", " << A(1,1) << ", " << A(1,2) << endl;
        cout << A(2,0) << ", " << A(2,1) << ", " << A(2,2) << endl;
        cout << endl;

        cout << "b-Vector: " << endl;
        cout << b(0,0) << ", " << b(0,1) << ", " << b(0,2) << endl;
        cout << endl;*/

        atlas::gesv(A,b);

        /*cout << "solution: " << endl;
        cout << b(0,0) << ", " << b(0,1) << ", " << b(0,2) << endl;
        cout << endl;*/

        if (b(0,2)<0 && sign == true && (b(0,1)>=0) && (b(0,1)<=1))
        {
            if (minScale < b(0,2))
                minScale = b(0,2);
        }
        else if (b(0,2)>0 && sign == false && (b(0,1)>=0) && (b(0,1)<=1))
        {
            if (minScale > b(0,2))
                minScale = b(0,2);
        }
    }

    return minScale;
}

std::vector<Base::Vector3f> SpringbackCorrection::FillConvex(std::vector<Base::Vector3f> Neib, std::vector<Base::Vector3f> Normals, int n)
{
    std::vector<Base::Vector3f> ConvComb;
    Base::Vector3f tmp;
    for (unsigned int i=1; i<Neib.size(); ++i)
    {
        for (int j=0; j<n; ++j)
        {
            //cout << "t, " ;
            tmp = Neib[i] - Neib[0];
            tmp.Scale(float(j)/float(n), float(j)/float(n), float(j)/float(n));
            if (InsideCheck(Neib[0] + tmp, Normals[0], Neib) == true)
                ConvComb.push_back(Neib[0] + tmp);
            else;
            //cout << "f, " ;
        }
    }
    cout << endl;
    return ConvComb;
}

bool SpringbackCorrection::InsideCheck(Base::Vector3f pnt, Base::Vector3f normal, std::vector<Base::Vector3f> Neib)
{
    Base::Vector3f Pnt, NeiPnt, NeiPntNext, Cross;

    ublas::matrix<double> A(3, 3);
    ublas::matrix<double> b(1, 3);

    bool b1 = true, b2;

    for (unsigned int k=1; k<Neib.size(); ++k)
    {
        NeiPnt = Neib[k];

        if ((k+1) == Neib.size())
        {
            NeiPntNext = Neib[1];
        }
        else
        {
            NeiPntNext = Neib[k+1];
        }

        Cross = normal%(NeiPntNext - NeiPnt);

        A(0,0) = normal.x;
        A(0,1) = (NeiPntNext.x - NeiPnt.x);
        A(0,2) = -Cross.x;
        A(1,0) = normal.y;
        A(1,1) = (NeiPntNext.y - NeiPnt.y);
        A(1,2) = -Cross.y;
        A(2,0) = normal.z;
        A(2,1) = (NeiPntNext.z - NeiPnt.z);
        A(2,2) = -Cross.z;

        b(0,0) = pnt.x - NeiPnt.x;
        b(0,1) = pnt.y - NeiPnt.y;
        b(0,2) = pnt.z - NeiPnt.z;

        atlas::gesv(A,b);

        if (b1==true)
        {
            if ( b(0,2) < 0)
            {
                b1 = false;
                b2 = true;
            }
            else if (b(0,2)>0)
            {
                b1 = false;
                b2 = false;
            }
        }

        if (b1==false)
        {
            if ((b2 == true)  && (b(0,2) > 0)) return false;
            if ((b2 == false) && (b(0,2) < 0)) return false;
        }
    }
    return true;
}

double SpringbackCorrection::GlobalCorrection(std::vector<Base::Vector3f> NeiLoc, std::vector<Base::Vector3f> NorLoc,
        bool sign, int ind)
{
    double minScale,mScRef,tmp = 0;
    Base::Vector3f TmpPnt, TmpNor;

    if (sign ==true) mScRef = -(1e+10);
    else      mScRef =   1e+10;

    minScale = mScRef;
    minScale = LocalCorrection(NeiLoc,NorLoc,sign,minScale);

    if ((abs(0.8*minScale) - abs(m_error[ind])) > 0)
    {
        PointArray[ind]._ucFlag = 3;
        m_CurvMax[ind] = abs(minScale);
        return minScale;
    }

    std::vector<Base::Vector3f> ConvComb = FillConvex(NeiLoc,NorLoc,20);

    // f�ge schwerpunkte der umgebenden faces hinzu
    std::set<MeshCore::MeshFacetArray::_TConstIterator> FacetNei;
    std::set<MeshCore::MeshFacetArray::_TConstIterator>::iterator face_it;
    MeshCore::MeshFacetIterator    f_it (MeshRef);
    MeshCore::MeshRefPointToFacets vf_it(MeshRef);

    FacetNei = vf_it[ind];

    for (face_it = FacetNei.begin(); face_it != FacetNei.end(); ++face_it)
    {
        TmpPnt.Set(0.0, 0.0, 0.0);
        TmpPnt += PointArray[(*face_it)->_aulPoints[0]];
        TmpPnt += PointArray[(*face_it)->_aulPoints[1]];
        TmpPnt += PointArray[(*face_it)->_aulPoints[2]];
        TmpPnt.Scale(1.0/3.0, 1.0/3.0, 1.0/3.0);
        ConvComb.push_back(TmpPnt);
    }

    /*Base::Builder3D logg;

    for(int i=0; i<NeiLoc.size(); ++i)
     logg.addSinglePoint(NeiLoc[i], 2,1,0,0);

    for(int i=0; i<ConvComb.size(); ++i)
     logg.addSinglePoint(ConvComb[i]);

    logg.saveToFile("c:/convex.iv");*/

    for (unsigned int i=0; i<ConvComb.size(); ++i)
    {
        for (unsigned int j=1; j<NorLoc.size()-1; ++j)
        {

            minScale  = mScRef;
            NeiLoc[0] = ConvComb[i];
            NorLoc[0] = NorLoc[j];
            minScale  = LocalCorrection(NeiLoc,NorLoc,sign,minScale);


            if (minScale > tmp)
            {
                tmp = minScale;
                TmpPnt = NeiLoc[0];
                TmpNor = NorLoc[0];
            }
        }
    }

    NeiLoc[0] = TmpPnt;
    NorLoc[0] = TmpNor;

    if ((abs(0.8*tmp) - abs(m_error[ind])) > 0)
    {
        PointArray[ind]._ucFlag = 3;
        m_CurvMax[ind] = abs(tmp);
    }
    else
    {
        PointArray[ind]._ucFlag = 4;
        m_CurvMax[ind] = abs(tmp);
    }

    return tmp;
}

/*! \brief Reorder the neighbour list

  This function will reorder the list in one-direction. Clockwise or counter clockwise is depending on the
  facet list and will not be checked by this function. (i.e the third vertex i.e vertex in first facet that
  is not the CurIndex or the first neighbour in pnt[Ok, I am also lost with this..., just debug and step to see what I mean...])
*/
void SpringbackCorrection::ReorderNeighbourList(std::set<MeshCore::MeshPointArray::_TConstIterator> &pnt,
        std::set<MeshCore::MeshFacetArray::_TConstIterator> &face, std::vector<unsigned long> &nei, unsigned long CurInd)
{
    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator pnt_it;
    std::set<MeshCore::MeshFacetArray::_TConstIterator>::iterator face_it;
    std::vector<unsigned long>::iterator vec_it;
    std::vector<unsigned long>::iterator ulong_it;
    unsigned long PrevIndex;

    if (face.size() != pnt.size())  //this means boundary point
    {
        for (pnt_it = pnt.begin(); pnt_it != pnt.end(); ++pnt_it)
        {
            int c = 0;
            for (face_it = face.begin(); face_it != face.end(); ++face_it)
            {
                if ((*pnt_it)[0]._ulProp == (*face_it)[0]._aulPoints[0])
                    ++c;
                if ((*pnt_it)[0]._ulProp == (*face_it)[0]._aulPoints[1])
                    ++c;
                if ((*pnt_it)[0]._ulProp == (*face_it)[0]._aulPoints[2])
                    ++c;
            }

            if (c==1)
                break;
        }
    }
    else
        pnt_it = pnt.begin();



    face_it = face.begin();

    nei.push_back((*pnt_it)->_ulProp);  //push back first neighbour
    vec_it = nei.begin();
    PrevIndex = nei[0];  //Initialize PrevIndex
    for (unsigned int i = 1; i < pnt.size(); i++) //Start
    {
        while (true)
        {
            std::vector<unsigned long> facetpnt;
            facetpnt.push_back((*face_it)->_aulPoints[0]); //push back into a vector for easier iteration
            facetpnt.push_back((*face_it)->_aulPoints[1]);
            facetpnt.push_back((*face_it)->_aulPoints[2]);
            if ((ulong_it = find(facetpnt.begin(), facetpnt.end(), PrevIndex)) == facetpnt.end())  //if PrevIndex not found
            {
                //in current facet
                if (face_it != face.end())
                    ++face_it; //next face please
                else
                    break;

                continue;
            }
            else
            {
                for (unsigned int k = 0 ; k < 3; k++)
                {
                    //If current facetpnt[k] is not yet in nei_list AND it is not the CurIndex
                    if (((vec_it = find(nei.begin(), nei.end(), facetpnt[k])) == nei.end()) && facetpnt[k] != CurInd)
                    {
                        face.erase(face_it);   //erase this face
                        nei.push_back(facetpnt[k]);  //push back the index
                        PrevIndex = facetpnt[k];   //this index is now PrevIndex
                        face_it = face.begin(); //reassign the iterator
                        break;   //end this for-loop
                    }
                }
                break;  //end this while loop
            }
        }
    }
}
