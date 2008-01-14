/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
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

//Mesh Stuff
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/MeshPy.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshAlgos.h>
#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/App/Core/Grid.h>

//OCC Stuff
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepAlgo_Section.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Handle_Geom_Plane.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>

//Own Stuff
#include "cutting_tools.h"
#include "best_fit.h"



cutting_tools::cutting_tools(TopoDS_Shape &aShape, float pitch,bool mirrortobothsides)
        : m_Shape(aShape),
		m_aMeshAlgo(NULL),
		m_CAD_Mesh_Grid(NULL),
        m_mirrortobothsides(mirrortobothsides),
        m_pitch(pitch)
{
    m_ordered_cuts.clear();
	m_all_offset_cuts_high.clear();
	m_all_offset_cuts_low.clear();
	m_face_bboxes.clear();
	m_cad = false;
	m_CAD_Mesh.Clear();
	m_zl_wire_combination.clear();
	getShapeBB();
	fillFaceBBoxes();
	classifyShape();
	checkFlatLevel();
	initializeMeshStuff();
	//BRepBuilderAPI_Sewing aSewer;
	//aSewer.Add(m_Shape);
	//aSewer.
	//Everything should be initialised now
	
}


cutting_tools::~cutting_tools()
{
	delete m_aMeshAlgo;
	delete m_CAD_Mesh_Grid;
}

bool cutting_tools::getShapeBB()
{
	//Die Cascade-Bounding Box funktioniert nicht richtig
	//Es wird dort wohl ne BoundingBox um dasKontrollnetz gelegt
	//Deshalb wird jetzt kurz das Shape tesseliert und dann die Bounding Box direkt ausgelesen
	best_fit::Tesselate_Shape(m_Shape,m_CAD_Mesh,0.1);
	Base::BoundBox3f aBoundBox = m_CAD_Mesh.GetBoundBox();
	m_maxlevel = aBoundBox.MaxZ;
	m_minlevel = aBoundBox.MinZ;
	/*	Hier ist die alte OCC BoundingBox Funktion
	Bnd_Box currentBBox;
	Standard_Real XMin, YMin, ZMin, XMax, YMax, ZMax;
	BRepBndLib::Add(m_shape, currentBBox );
    currentBBox.SetGap(1000.0);
    currentBBox.Get(XMin, YMin, ZMin, XMax, YMax, ZMax);
	*/
	return true;
}

bool cutting_tools::fillFaceBBoxes()
{
	TopoDS_Face atopo_surface;
    TopExp_Explorer Explorer;
	MeshCore::MeshKernel aFaceMesh;
	Base::BoundBox3f aBoundBox;
    Explorer.Init(m_Shape,TopAbs_FACE);
    for (;Explorer.More();Explorer.Next())
    {	
		aFaceMesh.Clear();
		aBoundBox.Flush();
        atopo_surface = TopoDS::Face (Explorer.Current());
		best_fit::Tesselate_Face(atopo_surface,aFaceMesh,0.1);
		aBoundBox = aFaceMesh.GetBoundBox();
		aBoundBox.Enlarge(2.0);
		std::pair<TopoDS_Face,Base::BoundBox3f> tempPair;
		tempPair.first = atopo_surface;
		tempPair.second = aBoundBox;
		m_face_bboxes.push_back(tempPair);
	}
	return true;
}
//Hier ist die alte Version um die Bounding Box zu bestimmen
  //      aAdaptor_Surface.Initialize(atopo_surface);
  //      Standard_Real FirstUParameter, LastUParameter,FirstVParameter,LastVParameter;
  //      gp_Pnt aSurfacePoint;
  //	  FirstUParameter = aAdaptor_Surface.FirstUParameter();
  //      LastUParameter  = aAdaptor_Surface.LastUParameter();
  //      FirstVParameter = aAdaptor_Surface.FirstVParameter();
  //      LastVParameter = aAdaptor_Surface.LastVParameter();
		//float urange = LastUParameter - FirstUParameter;
		//float vrange = LastVParameter - FirstVParameter;
		////Jetzt ein 10x10 Grid pro Face machen und so die BBox bestimmen
		////Zunächst mal die BoundingBox auf Null setzen
		//Bnd_Box currentBBox;
		//
		//for(int i=0;i<10;++i)
		//{
		//	for(int j=0;j<10;++j)
		//	{
		//		aAdaptor_Surface.D0(FirstUParameter+((urange/10)*i),FirstVParameter+((vrange/10)*j),aSurfacePoint);
		//		currentBBox.Update(aSurfacePoint.X(),aSurfacePoint.Y(),aSurfacePoint.Z());
		//	}
		//}
		////Jetzt noch nen Rand drumherum bauen
		//currentBBox.SetGap(0.5);
		////Jetzt die aktuelle BoundingBox mit dem aktuellen Face in einen vector pushen
		//std::pair<TopoDS_Face,Bnd_Box> tempPair;
		//tempPair.first = atopo_surface;
		//tempPair.second = currentBBox;
		//m_face_bboxes.push_back(tempPair);
		////aVertex = BRepBuilderAPI_MakeVertex(aSurfacePoint); 
		////BRepBndLib::Add(aVertex, currentBBox );
		//currentBBox.SetVoid();


bool cutting_tools::checkPointinFaceBB(const gp_Pnt &aPnt,const Base::BoundBox3f &aBndBox)
{
	if((aPnt.X()>aBndBox.MinX) && (aPnt.X()<aBndBox.MaxX) && (aPnt.Y()>aBndBox.MinY) && (aPnt.Y()<aBndBox.MaxY) && (aPnt.Z()>aBndBox.MinZ) && (aPnt.Z()<aBndBox.MaxZ))
		return true;

	return false;
}


bool cutting_tools::initializeMeshStuff()
{
	m_CAD_Mesh_Grid = new MeshCore::MeshFacetGrid(m_CAD_Mesh);
	m_aMeshAlgo = new MeshCore::MeshAlgorithm(m_CAD_Mesh);
	return true;
}
bool cutting_tools::arrangecuts_ZLEVEL()
{
	//Zunächst wieder checken ob CAD oder nicht
	if(m_cad==false) 
    {
		//Cast um die Nachkommastellen wegzuschneiden
        int cutnumber = (int)fabs((m_maxlevel-m_minlevel)/m_pitch);
		//m_pitch leicht korrigieren um wirklich auf die letzte Ebene zu kommen
        m_pitch = fabs(m_maxlevel-m_minlevel)/cutnumber;
		//Jetzt die Schnitte machen. Die höchste Ebene fällt weg, da hier noch kein Blech gedrückt wird
        float z_level,z_level_corrected;
		TopoDS_Shape aCutShape;
		for (int i=1;i<=cutnumber;++i) 
		{
            //Jetzt schneiden (die oberste Ebene auslassen)
            z_level = m_maxlevel-(i*m_pitch);
			z_level_corrected = z_level;
			cut(z_level,m_minlevel,aCutShape,z_level_corrected);
			//cut_Mesh(z_level,m_minlevel,result,z_level_corrected);

			//Jetzt die resultierende Wire in einen Vector pushen
			std::pair<float,TopoDS_Shape> tempPair;
			tempPair.first = z_level_corrected;
			tempPair.second = aCutShape;
			m_ordered_cuts.push_back(tempPair);
		}
        return true;
    }
	//Wenn wir mehrere Faces haben oder eine CAD-Geometrie vorhanden ist
	else
	{
		//Erst mal schauen ob der maximale Wert vom Bauteil m_maxlevel mit dem höchsten flachen Bereich zusammenfällt
		if(m_zl_wire_combination.empty()) return false;
		//Wir benutzen rbegin, weil wir damit den ersten Z-Wert bekommen (wurde vom kleinsten zum größten geordnet)
		float test = m_maxlevel-m_zl_wire_combination.rbegin()->first;
		//Oberste Ebene vom Bauteil fällt mit der obersten flachen Ebene zusammen
		//-> Schnitte von der obersten flachen Ebene bis zur nächsten flachen Ebene oder bis ganz nach unten m_minlevel
		if (fabs(test) < 3)
		{
			float temp_max = m_zl_wire_combination.rbegin()->first;
			float temp_min;
			std::map<float,std::map<Base::BoundBox3f,TopoDS_Wire,BoundBox3f_Less> >::iterator zl_wire_it;
			//Wir holen uns jetzt den nächsten Z-Level raus. Wir müssen was kleineres 
			//wie den höchsten Wert nehmen sonst gibt er immer den höchsten Wert aus
			zl_wire_it = m_zl_wire_combination.upper_bound(temp_max-0.1);
			if(zl_wire_it->first == temp_max)
			{
				cout << "Tja, es gibt wohl nur eine flache Area";
				temp_min = m_minlevel;
			}
			//Wenn es mehrere flache Bereiche gibt muss ich nochmal weitermachen
			else
			{
				temp_min = zl_wire_it->first;
				cout << "Mehrere Areas erkannt";
			}
			//Jetzt schnippeln von temp_max bis temp_min
			int cutnumber = (int)fabs((temp_max-temp_min)/m_pitch);
			//m_pitch leicht korrigieren um wirklich auf die letzte Ebene zu kommen
			m_pitch = fabs(temp_max-temp_min)/cutnumber;
			//Jetzt die Schnitte machen. Die höchste Ebene fällt weg, da hier noch kein Blech gedrückt wird
			float z_level,z_level_corrected;
			TopoDS_Shape aCutShape;
			//Jetzt schneiden (die oberste Ebene auslassen)
			for (int i=1;i<=20;++i) 
			{
				z_level = temp_max-(i*m_pitch);
				z_level_corrected = z_level;
				//cut_Mesh(z_level,m_minlevel,result,z_level_corrected);
				//Jetzt die resultierenden Points in den vector schieben
				//std::pair<float,std::list<std::vector<Base::Vector3f> > > tempPair;
				//tempPair.first = z_level_corrected;
				//tempPair.second = result;
				//m_ordered_cuts.push_back(tempPair);
				cut(z_level,temp_min, aCutShape,z_level_corrected);
				//Jetzt die gefüllte Wire in den vector schieben
				std::pair<float,TopoDS_Shape> tempPair;
				tempPair.first = z_level_corrected;
				tempPair.second = aCutShape;
				m_ordered_cuts.push_back(tempPair);
			}
			return true;
		}
	}

	return false;
}



bool cutting_tools::checkFlatLevel()
{
	//Falls keine CAD-Geometrie da ist, gleich wieder rausspringen
	
	if(m_cad==false) return false;
    
	TopoDS_Face atopo_surface;
    BRepAdaptor_Surface aAdaptor_Surface;
    TopExp_Explorer Explorer;
    

    for (Explorer.Init(m_Shape,TopAbs_FACE);Explorer.More();Explorer.Next())
    {
        atopo_surface = TopoDS::Face (Explorer.Current());
        aAdaptor_Surface.Initialize(atopo_surface);
        Standard_Real FirstUParameter, LastUParameter,FirstVParameter,LastVParameter;
        gp_Pnt first,second,third;
        gp_Vec first_u,first_v,second_u,second_v,third_u,third_v, Norm_first,Norm_second,Norm_third,Norm_average;
        double u_middle,v_middle;
        /*
		Generate three random point on the surface to get the surface normal and decide wether its a
        planar face or not
		*/
        FirstUParameter = aAdaptor_Surface.FirstUParameter();
        LastUParameter  = aAdaptor_Surface.LastUParameter();
        FirstVParameter = aAdaptor_Surface.FirstVParameter();
        LastVParameter = aAdaptor_Surface.LastVParameter();
        u_middle = sqrt((FirstUParameter - LastUParameter)*(FirstUParameter - LastUParameter))/2;
        v_middle = sqrt((FirstVParameter - LastVParameter)*(FirstVParameter - LastVParameter))/2;
        aAdaptor_Surface.D1(sqrt((FirstUParameter-u_middle)*(FirstUParameter-u_middle))/2,sqrt((FirstVParameter-v_middle)*(FirstVParameter-v_middle))/2,first,first_u,first_v);
        aAdaptor_Surface.D1(sqrt((u_middle)*(u_middle))/2,sqrt((v_middle)*(v_middle))/2,second,second_u,second_v);
        aAdaptor_Surface.D1(sqrt((FirstUParameter+u_middle)*(FirstUParameter+u_middle))/2,sqrt((FirstVParameter+v_middle)*(FirstVParameter+v_middle))/2,third,third_u,third_v);
        //Get Surface normal as Cross-Product between two Vectors
        Norm_first = first_u.Crossed(first_v);
        Norm_first.Normalize();
        Norm_second = second_u.Crossed(second_v);
        Norm_second.Normalize();
        Norm_third = third_u.Crossed(third_v);
        Norm_third.Normalize();
        //Evaluate average normal vector
        Norm_average.SetX((Norm_first.X()+Norm_second.X()+Norm_third.X())/3);
        Norm_average.SetY((Norm_first.Y()+Norm_second.Y()+Norm_third.Y())/3);
        Norm_average.SetZ((Norm_first.Z()+Norm_second.Z()+Norm_third.Z())/3);
        Norm_average.Normalize();
        gp_Vec z_normal(0,0,1);
        gp_Vec z_normal_opposite(0,0,-1);
        if (Norm_average.IsEqual(z_normal,0.01,0.01) || Norm_average.IsEqual(z_normal_opposite,0.01,0.01))
        {
            cout << "Einen flachen Bereich gefunden";
			std::pair<float,std::map<Base::BoundBox3f,TopoDS_Wire,BoundBox3f_Less> >aTempPair;
			aTempPair.second.clear();
			std::pair<Base::BoundBox3f,TopoDS_Wire> aTempBBoxPair;
			
            //Z-Wert vom flachen Bereich in ein temporäres pair pushen
			aTempPair.first = ((first.Z()+second.Z()+third.Z())/3);
			//Jetzt durch das Face gehen und die Wires rausfiltern
			TopExp_Explorer Explore_Face;
            Explore_Face.Init(atopo_surface,TopAbs_WIRE);
			//If there is no Wire -> return
            if (!Explore_Face.More()) return false; 

			//Jetzt alle Wires in die map schieben
            for (Explore_Face.ReInit();Explore_Face.More();Explore_Face.Next())
            {
				aTempBBoxPair.first = getWireBBox(TopoDS::Wire(Explore_Face.Current()));
				aTempBBoxPair.second = TopoDS::Wire(Explore_Face.Current());
				aTempPair.second.insert(aTempBBoxPair);
				//aTempPair.first ist ja noch auf dem gleichen Z-Wert wie vorher, deswegen muss da nichts abgepasst werden
            }
			m_zl_wire_combination.insert(aTempPair);
		}
    }
    
    return true;
}

//bool cutting_tools::projectWireToSurface(const TopoDS_Wire &aWire,const TopoDS_Shape &aShape,std::vector<projectPointContainer> &aContainer);
//{
//	//make your wire looks like a curve to other algorithm and generate Points to offset the curve
//	aContainer.clear();
//	BRepAdaptor_CompCurve2 wireAdaptor(aWire);
//	GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.01);
//	int numberofpoints = aProp.NbPoints();
//	Standard_Real Umin,Vmin,lowestdistance;
//	TopoDS_Face atopo_surface,atopo_surface_shortest;
//	Handle_Geom_Surface geom_surface;
//
//	//Now project the points to the surface and get surface normal. 
//	for (int i=1;i<=numberofpoints;++i)
//	{
//		lowestdistance=200;
//		//Aktuellen Punkt holen
//		gp_Pnt currentPoint = aProp.Value(i);
//		projectPointContainer aTempContainer;
//		//checken auf welches Face wir projezieren könnnen
//		for(m_face_bb_it = m_face_bboxes.begin();m_face_bb_it!=m_face_bboxes.end();++m_face_bb_it)
//		{
//			//Wenn der aktuelle Punkt in der BBox enthalten ist, dann machen wir mit der Projection weiter
//			if(checkPointinFaceBB(aProp.Value(i),m_face_bb_it->second))
//			{
//				atopo_surface = m_face_bb_it->first;
//				geom_surface = BRep_Tool::Surface(atopo_surface);
//				GeomAPI_ProjectPointOnSurf aPPS(currentPoint,geom_surface,0.001);
//				//Wenn nichts projeziert werden kann, gehts gleich weiter zum nächsten Face bzw. der nächsten BBox
//				if (aPPS.NbPoints() == 0) continue;
//				//Jetzt muss das aktuelle Face gespeichert werden, da es eventuell das face ist, welches am nächsten ist
//				double length = aPPS.LowerDistance();
//				if(lowestdistance>length)
//				{
//					lowestdistance=length;
//					atopo_surface_shortest = atopo_surface;
//					aPPS.LowerDistanceParameters (Umin,Vmin);
//				}
//			}
//		}
//		gp_Vec Uvec,Vvec,normalVec;
//		geom_surface = BRep_Tool::Surface(atopo_surface_shortest);
//		//Das Face welches am nächsten ist in der temp-struct speichern
//		aTempContainer.face = atopo_surface_shortest;
//		geom_surface->D1(Umin,Vmin,aTempContainer.point,Uvec,Vvec);
//		//Jetzt den Normalenvector auf die Fläche ausrechnen
//		normalVec = Uvec;
//		normalVec.Cross(Vvec);
//		normalVec.Normalize(); 
//		//Jetzt ist die Normale berechnet und auch normalisiert
//		//Jetzt noch checken ob die Normale auch wirklich wie alle anderen auf die gleiche Seite zeigt.
//		//dazu nur checken ob der Z-Wert der Normale größer Null ist (dann im 1.und 2. Quadranten)
//		if(normalVec.Z()<0) normalVec.Multiply(-1.0);
//		//Mal kurz den Winkel zur Grund-Ebene ausrechnen
//		aTempContainer.normalvector = normalVec;
//		aContainer.push_back(aTempContainer);
//			
//	}
//	return true;
//}


TopoDS_Wire cutting_tools::ordercutShape(const TopoDS_Shape &aShape)
{
    //Bisher funktioniert das Ganze nur für Schnitte welche nur einmal rundherum laufen und noch nicht für Inseln
    TopExp_Explorer exploreShape;
    exploreShape.Init(aShape,TopAbs_EDGE);
    int k=0;
    for (; exploreShape.More(); exploreShape.Next()) //erstmal schauen wieviele Edges wir haben
    {
        k++;
    }
    //Jetzt die Edges alle in eine Wire packen (mit Add) und schauen ob ein Fehler kommt.
    //Wenn ja, dann die nächste Edge. Solange bis alle Edges drin sind.

    if (k<1)
    {
        exploreShape.ReInit();
		BRepBuilderAPI_MakeWire mkWire(TopoDS::Edge(exploreShape.Current()));
		return mkWire.Wire();
    }
    else //Nur wenn mehr als eine Edge vorhanden ist
    {
        BRepBuilderAPI_MakeWire mkWire; //WireContainer aufbauen
        std::vector<edge_container> listofedge_tmp,listofedge;
        std::vector<edge_container>::iterator it_edge;
        edge_container a_edge_container; //Definiert in der stuff.h
        listofedge.clear();
		listofedge_tmp.clear();
        exploreShape.ReInit();
        //Edge-Liste füllen
        for (; exploreShape.More(); exploreShape.Next())
        {
            a_edge_container.edge = TopoDS::Edge(exploreShape.Current());
            BRepAdaptor_Curve an_edge_adaptor(a_edge_container.edge);
            a_edge_container.firstPoint = an_edge_adaptor.Value(an_edge_adaptor.FirstParameter());
            a_edge_container.lastPoint = an_edge_adaptor.Value(an_edge_adaptor.LastParameter());
            listofedge.push_back(a_edge_container);
        }
        
        gp_Pnt lastpointoflastedge,firstpointoflastedge;
        while (listofedge.empty() == false )
        {
            listofedge_tmp.clear();
            for (it_edge = listofedge.begin();it_edge<(listofedge.end());++it_edge)
            {
                mkWire.Add((*it_edge).edge);
                if (mkWire.IsDone())
                {
                    lastpointoflastedge = (*it_edge).lastPoint;
                    firstpointoflastedge = (*it_edge).firstPoint;
                }
                else
                {
                    //Abstände ausrechnen
                    double abstand1=sqrt((*it_edge).firstPoint.SquareDistance(lastpointoflastedge));
                    double abstand2=sqrt((*it_edge).lastPoint.SquareDistance(lastpointoflastedge));
                    double abstand3=sqrt((*it_edge).firstPoint.SquareDistance(firstpointoflastedge));
                    double abstand4=sqrt((*it_edge).lastPoint.SquareDistance(firstpointoflastedge));
                    if (abstand1<0.5)
                    {
                        //Neue Edge erzeugen welche vom letzten Endpunkt zum aktuellen Startpunkt geht
                        BRepBuilderAPI_MakeEdge newedge(lastpointoflastedge,(*it_edge).firstPoint);
                        mkWire.Add(newedge);
                        mkWire.Add((*it_edge).edge);
                        lastpointoflastedge = (*it_edge).lastPoint;
                        firstpointoflastedge = (*it_edge).firstPoint;
                    }
                    else if (abstand2<0.5)
                    {
                        //Neue Edge erzeugen welche vom letzten Endpunkt zum aktuellen Startpunkt geht
                        BRepBuilderAPI_MakeEdge newedge(lastpointoflastedge,(*it_edge).lastPoint);
                        mkWire.Add(newedge);
                        mkWire.Add((*it_edge).edge); 
                        lastpointoflastedge = (*it_edge).lastPoint;
                        firstpointoflastedge = (*it_edge).firstPoint;

                    }

                    else if (abstand3<0.5)
                    {
                        //Neue Edge erzeugen welche vom letzten Endpunkt zum aktuellen Startpunkt geht
                        BRepBuilderAPI_MakeEdge newedge(firstpointoflastedge,(*it_edge).firstPoint);
                        mkWire.Add(newedge);
                        mkWire.Add((*it_edge).edge);
                        lastpointoflastedge = (*it_edge).lastPoint;
                        firstpointoflastedge = (*it_edge).firstPoint;
                    }
                    else if (abstand4<0.5)
                    {
                        //Neue Edge erzeugen welche vom letzten Endpunkt zum aktuellen Startpunkt geht
                        BRepBuilderAPI_MakeEdge newedge(firstpointoflastedge,(*it_edge).lastPoint);
                        mkWire.Add(newedge);         
                        mkWire.Add((*it_edge).edge);
                        lastpointoflastedge = (*it_edge).lastPoint;
                        firstpointoflastedge = (*it_edge).firstPoint;
                    }
                    else
                    {
                        listofedge_tmp.push_back(*it_edge);
                    }
                }
            }
            listofedge = listofedge_tmp;
        }
        return mkWire.Wire();
    }

}


//bool cutting_tools::OffsetWires_Standard(float radius) //Version wo nur in X,Y-Ebene verschoben wird
//{
//	Base::Builder3D build;
//	std::ofstream outfile;
//	outfile.open("c:/atest.out");
//
//
//	//Die ordered_cuts sind ein Vector wo für jede Ebene ein Pair existiert
//	for(m_ordered_cuts_it = m_ordered_cuts.begin();m_ordered_cuts_it!=m_ordered_cuts.end();++m_ordered_cuts_it)
//	{
//		float current_z_level = m_ordered_cuts_it->first;
//		std::vector<gp_Pnt> finalPoints;
//		finalPoints.clear();
//		Standard_Real Umin,Vmin,lowestdistance;
//		TopoDS_Face atopo_surface,atopo_surface_shortest;
//		Handle_Geom_Surface geom_surface;
//		int i=0;
//		for (;avector_it!=aPolyline_it->end();++avector_it)
//		{
//			i++;
//			lowestdistance=200;
//			//Aktuellen Punkt holen
//			gp_Pnt currentPoint(avector_it->x,avector_it->y,avector_it->z);
//			gp_Pnt nearest_Point;
//			//checken auf welches Face wir projezieren könnnen
//			for(m_face_bb_it = m_face_bboxes.begin();m_face_bb_it!=m_face_bboxes.end();++m_face_bb_it)
//			{
//				//Wenn der aktuelle Punkt in der BBox enthalten ist, dann machen wir mit der Projection weiter
//				if(checkPointinFaceBB(currentPoint,m_face_bb_it->second))
//				{
//					atopo_surface = m_face_bb_it->first;
//					geom_surface = BRep_Tool::Surface(atopo_surface);
//					GeomAPI_ProjectPointOnSurf aPPS(currentPoint,geom_surface,0.001);
//					//Wenn nichts projeziert werden kann, gehts gleich weiter zum nächsten Face bzw. der nächsten BBox
//					if (aPPS.NbPoints() == 0) continue;
//					//Jetzt muss das aktuelle Face gespeichert werden, da es eventuell das face ist, welches am nächsten ist
//					double length = aPPS.LowerDistance();
//					if(lowestdistance>length)
//					{
//						lowestdistance=length;
//						atopo_surface_shortest = atopo_surface;
//						//aPPS.LowerDistanceParameters (Umin,Vmin);
//						nearest_Point = aPPS.NearestPoint();
//					}
//				}
//			}
//			//Für eine saubere Projection auf der aktuellen Ebene wird jetzt der Richtungsvector 
//			//aus dem projezierten Punkt und dem Ursprungspunkt gebildet
//			//und dieser dann hergenommen um damit nochmal in dessen Richtung zu projezieren
//			gp_Vec aVec(currentPoint,nearest_Point);
//			aVec.Normalize();
//			aVec.SetZ(0.0);
//			gp_Dir aDir(aVec);
//			gp_Lin aLine(currentPoint,aDir);
//			IntCurvesFace_ShapeIntersector aFaceIntSect;
//			aFaceIntSect.Load(m_Shape, 0.001);
//			aFaceIntSect.PerformNearest(aLine,-RealLast(), +RealLast());
//			//Jetzt holen wir uns auf der Fläche den U und V Wert um dann D1 und D2 bestimmen zu können
//			gp_Pnt projectedPoint,OffsetPoint;
//			gp_Pnt testpoint,testpoint2;
//			float abstand,abstand_old;
//			int number = aFaceIntSect.NbPnt();
//			abstand_old = 100;
//			for(int k=0;k<number;++k)
//			{
//				testpoint = aFaceIntSect.Pnt(k+1);
//				abstand = testpoint.Distance(currentPoint);
//				if(abstand<abstand_old)
//				{
//					Umin = aFaceIntSect.UParameter(k+1);
//					Vmin = aFaceIntSect.VParameter(k+1);
//					atopo_surface_shortest = aFaceIntSect.Face(k+1);
//					abstand_old = abstand;
//				}
//			}	
//			gp_Vec Uvec,Vvec,normalVec,projPointVec,z_normale;
//			geom_surface = BRep_Tool::Surface(atopo_surface_shortest);
//			geom_surface->D1(Umin,Vmin,projectedPoint,Uvec,Vvec);
//			normalVec = Uvec;
//			normalVec.Cross(Vvec);
//			normalVec.Normalize(); 
//			//Jetzt ist die Surface-Normale berechnet und auch normalisiert
//			//Jetzt noch checken ob die Normale auch wirklich wie alle anderen auf die gleiche Seite zeigt.
//			//dazu nur checken ob der Z-Wert der Normale größer Null ist (dann im 1.und 2. Quadranten)
//			if(normalVec.Z()<0) normalVec.Multiply(-1.0);
//			//Jetzt die Normale auf die Radiuslänge verlängern
//			normalVec.Multiply(radius);
//			//Jetzt die Z-Komponente auf 0 setzen
//			normalVec.SetZ(0.0);
//			//float abstand = currentPoint.Distance(projectedPoint);
//			//if(abstand>0.2)
//			//{cout<<"error"<<endl;}
//			projPointVec.SetXYZ(projectedPoint.XYZ());
//			OffsetPoint.SetXYZ((projPointVec + normalVec).XYZ());
//			OffsetPoint.SetZ(projectedPoint.Z()+radius);//Den Radius noch dazu addieren
//			//Aktuellen OffsetPoint setzen
//			finalPoints.push_back(OffsetPoint);
//
//			//Base::Vector3f offsetPoint,projectPoint;
//			//offsetPoint.x=OffsetPoint.X();offsetPoint.y=OffsetPoint.Y();offsetPoint.z=OffsetPoint.Z();
//			//projectPoint.x=projectedPoint.X();projectPoint.y=projectedPoint.Y();projectPoint.z=projectedPoint.Z();
//			//build.addSingleArrow(projectPoint,offsetPoint);
//			build.addSinglePoint(OffsetPoint.X(),OffsetPoint.Y(),OffsetPoint.Z());
//			outfile << currentPoint.X() <<","<<currentPoint.Y()<<","<<currentPoint.Z()<<","<< projectedPoint.X() <<","<<projectedPoint.Y()<<","<<projectedPoint.Z()<<","<< OffsetPoint.X() <<","<<OffsetPoint.Y()<<","<<OffsetPoint.Z()<<","<<normalVec.X() <<","<<normalVec.Y()<<","<<normalVec.Z()<< std::endl;
//		}
//
//
//	//		outfile << projectedPoint.X() <<","<<projectedPoint.Y()<<","<<projectedPoint.Z()<<std::endl;
//			//Jetzt die aktuelle Kurve als BSpline interpolieren
//		//check for intersections due to wrong offseted points
//		checkPointIntersection(finalPoints);
//		std::vector<gp_Pnt> finalPointscorrected;
//		finalPointscorrected.clear();
//		checkPointDistance(finalPoints,finalPointscorrected);
//		Handle(TColgp_HArray1OfPnt) finalOffsetPoints = new TColgp_HArray1OfPnt(1, finalPointscorrected.size());
//		for(int t=0;t<finalPointscorrected.size();++t)
//		{
//			finalOffsetPoints->SetValue(t+1,finalPointscorrected[t]);
//		}
//		GeomAPI_Interpolate aNoPeriodInterpolate(finalOffsetPoints, Standard_False, Precision::Confusion());
//		aNoPeriodInterpolate.Perform();
//		Handle_Geom_BSplineCurve aCurve(aNoPeriodInterpolate.Curve());
//		 //check results
//		if (!aNoPeriodInterpolate.IsDone()) return false;
//		m_all_offset_cuts_high.push_back(aCurve);
//	}
//
//
//
//
//
//	build.saveToFile("c:/output.iv");
//	outfile.close();
//
//
//
//
//
//return true;
//
//}

//bool cutting_tools::checkPointDistance(std::vector<gp_Pnt> &finalPoints,std::vector<gp_Pnt> &output)
//{
//	std::vector<gp_Pnt>::iterator aPntIt,atempit;
//	output.clear();
//	double square_precision = Precision::Confusion()*Precision::Confusion();
//	int i;
//	for(aPntIt=finalPoints.begin();aPntIt!=finalPoints.end();++aPntIt)
//	{
//		atempit=aPntIt;
//		output.push_back(*aPntIt);
//		i=1;
//		while(((*(aPntIt+i)).SquareDistance(*aPntIt))< square_precision && (aPntIt+i)!=finalPoints.end())
//		{
//			i++;
//			atempit++;
//		}
//		aPntIt = atempit;
//		
//		
//		
//	}
//			
//	return true;
//}

//bool cutting_tools::checkPointIntersection(std::vector<projectPointContainer> &finalPoints)
//{
//	//Hier wird gecheckt ob die Punkte wirklich alle in der richtigen Reihenfolge vorliegen
//	std::vector<projectPointContainer>::iterator aPntIt;
//	double distance,distance_old;
//	projectPointContainer nearestPointStruct;
//	int k;
//	for(unsigned int j=0;j<finalPoints.size();++j)
//	{
//		distance_old = 100;
//		for(int i=1;i<30;++i)
//		{
//			//Wenn wir schon fast am Ende sind oder schon bald, dann rausspringen
//			if((j+i)>=finalPoints.size()) break;
//
//			distance = (finalPoints[j+i].point).SquareDistance(finalPoints[j].point);
//			if(distance<distance_old)
//			{
//				//Speichern wo wir den nächsten Punkt gefunden haben
//				k=i;
//				distance_old = distance;
//			}
//		}
//		//Jetzt checken ob der nächste Punkt bereits der gesuchte ist
//		if(k==1)
//		{
//			continue;
//		}
//		else
//		{
//			//Jetzt den Punkteaustausch vornehmen
//			nearestPointStruct = finalPoints[j+k];
//			finalPoints[j+k] = finalPoints[j+1];
//			finalPoints[j+1] = nearestPointStruct;
//		}
//	}
//
//
//	
//	return true;
//}
//
//

#include "edgesort.h"
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BRep_Tool.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Base/Builder3D.h>
#include "BRepAdaptor_CompCurve2.h"

bool cutting_tools::OffsetWires_Standard(float radius,float radius_slave,float sheet_thickness) //Version wo nur in X,Y-Ebene verschoben wird
{
	m_radius = radius;
	m_radius_slave = radius_slave;
	m_sheet_thickness = sheet_thickness;
	//std::ofstream inventoroutput;
	//inventoroutput.open("c:/finalTest.iv");
	//Base::InventorBuilder build(inventoroutput);;

	//std::ofstream anoutput1,anoutput2;
	//anoutput1.open("c:/master.txt");
	//anoutput2.open("c:/slave.txt");
	//bool write=true;
	for(m_ordered_cuts_it = m_ordered_cuts.begin();m_ordered_cuts_it!=m_ordered_cuts.end();++m_ordered_cuts_it)
	{
		std::vector<std::pair<gp_Pnt,double> > OffsetPointContainer;
		std::vector<gp_Pnt> SlaveOffsetPointContainer;
		OffsetPointContainer.clear();
		SlaveOffsetPointContainer.clear();
		//Order the CutShape and access the PCurve to generate the points to offset
		Edgesort aCutShapeSorter(m_ordered_cuts_it->second);
		//int k=1;
		for(aCutShapeSorter.Init();aCutShapeSorter.More();aCutShapeSorter.Next())
		{
			
			//Get the PCurve and the GeomSurface
			Handle_Geom2d_Curve aCurve;
			Handle_Geom_Surface aSurface;
			TopLoc_Location aLoc;
			double first,last;
			BRep_Tool::CurveOnSurface(aCutShapeSorter.Current(),aCurve,aSurface,aLoc,first,last);
			//Jetzt noch die resultierende Surface und die Curve sauber drehen 
			//(vielleicht wurde ja das TopoDS_Face irgendwie gedreht oder die TopoDS_Edge)
			if(aCutShapeSorter.Current().Orientation() == TopAbs_REVERSED) aCurve->Reverse();
			
			Geom2dAdaptor_Curve a2dCurveAdaptor(aCurve);
			GCPnts_QuasiUniformDeflection aPointGenerator(a2dCurveAdaptor,0.01);
			//Now get the surface normal to the generated points 
			for (int i=1;i<=aPointGenerator.NbPoints();++i)
			{
				std::pair<gp_Pnt,double> PointContactPair;
				gp_Pnt2d a2dParaPoint;
				gp_Pnt aSurfacePoint;
				TopoDS_Face aFace;
				gp_Vec Uvec,Vvec,normalVec;
				aCurve->D0(aPointGenerator.Parameter(i),a2dParaPoint);
				GeomAdaptor_Surface aGeom_Adaptor(aSurface);
				Handle_Geom_BSplineSurface aBSurface = aGeom_Adaptor.BSpline();
				//aBSurface->MovePoint();
				aSurface->D1(a2dParaPoint.X(),a2dParaPoint.Y(),aSurfacePoint,Uvec,Vvec);
				//Jetzt den Normalenvector auf die Fläche ausrechnen
				normalVec = Uvec;
				normalVec.Cross(Vvec);
				normalVec.Normalize(); 
				//Jetzt ist die Normale berechnet und auch normalisiert
				//Jetzt noch checken ob die Normale auch wirklich auf die saubere Seite zeigt
				//dazu nur checken ob der Z-Wert der Normale größer Null ist (dann im 1.und 2. Quadranten)
				if(normalVec.Z()<0) normalVec.Multiply(-1.0);
				/*if ( aSurface->.Orientation() == TopAbs_Reverse ) 
				{
					normalVec.Reverse()
				}*/
				//Mal kurz den Winkel zur Grund-Ebene ausrechnen
				gp_Vec planeVec(normalVec.X(),normalVec.Y(),0.0);
				//Den Winkel 
				PointContactPair.second = normalVec.Angle(planeVec);
				//Jetzt die Z-Komponente auf 0 setzen
				normalVec.SetZ(0.0);
				normalVec.Normalize();
				//Jetzt die Normale mit folgender Formel multiplizieren
				normalVec.Multiply(((1-sin(PointContactPair.second))*radius)/cos(PointContactPair.second));
				//Jetzt den OffsetPunkt berechnen
				PointContactPair.first.SetXYZ(aSurfacePoint.XYZ() + normalVec.XYZ());
				PointContactPair.first.SetZ(PointContactPair.first.Z() + radius);
				//Damit wir keine Punkte bekommen die zu nahe beieinander liegen
				//Den letzten hinzugefügten Punkt suchen
			
				if(OffsetPointContainer.size()>0 && (OffsetPointContainer.rbegin()->first.SquareDistance(PointContactPair.first)>(Precision::Confusion()*Precision::Confusion())))
				{
					OffsetPointContainer.push_back(PointContactPair);
					//anoutput1 << PointContactPair.first.X() <<","<< PointContactPair.first.Y() <<","<< PointContactPair.first.Z()<<std::endl;
				}
				else if(OffsetPointContainer.empty())
				{
					OffsetPointContainer.push_back(PointContactPair);
					//anoutput1 << PointContactPair.first.X() <<","<< PointContactPair.first.Y() <<","<< PointContactPair.first.Z()<<std::endl;
				}
				//Damit nur eine Runde ausgegeben wird
				/*if(write)
				{
					char output[20];
					sprintf(output,"%d",k++);
					build.addSinglePoint(PointContactPair.first.X(),PointContactPair.first.Y(),PointContactPair.first.Z());
					build.addText(PointContactPair.first.X(), PointContactPair.first.Y(),PointContactPair.first.Z(),output);
				}*/
			}
			
		}
		//write = false;

		//Jetzt wurden alle Edges offsettiert und jetzt wird das Gegenstück erzeugt werden
		//Zunächst mal die Z-Ebene fürs Gegenstück generieren
		double slave_z_level;
		double average_sheet_thickness,average_angle;
		float slave_z_level_corrected;
		TopoDS_Shape slaveCutShape;
		calculateAccurateSlaveZLevel(OffsetPointContainer,m_ordered_cuts_it->first,slave_z_level,average_sheet_thickness,average_angle);
		//check if its possible to cut. If not, take current wire as tool path
		if(slave_z_level>=(m_ordered_cuts.begin()->first)) 
		{
			//Check the smallest Wire with the Bounding Box of the Wire
			//currently we do not search for the desired z-level
			//we just take the highest level available
			TopoDS_Wire aWire = m_zl_wire_combination.rbegin()->second.begin()->second;
			BRepAdaptor_CompCurve2 wireAdaptor(aWire);
			GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.1);
			for(int i=1;i<=aProp.NbPoints();++i)
			{
				gp_Pnt SlaveOffsetPoint(aProp.Value(i).XYZ());
				SlaveOffsetPoint.SetZ(SlaveOffsetPoint.Z() - m_radius_slave - m_sheet_thickness);
				//checken ob der neue Punkt zu nahe am alten ist. Wenn ja, dann kein push_back
				if(SlaveOffsetPointContainer.size()>0 && (SlaveOffsetPointContainer.rbegin()->SquareDistance(SlaveOffsetPoint)>(Precision::Confusion()*Precision::Confusion())))
				{
					SlaveOffsetPointContainer.push_back(SlaveOffsetPoint);
					//anoutput2 << SlaveOffsetPoint.X() <<","<< SlaveOffsetPoint.Y() <<","<< SlaveOffsetPoint.Z()<<std::endl;
				}
				else if(SlaveOffsetPointContainer.empty())
				{
					SlaveOffsetPointContainer.push_back(SlaveOffsetPoint);
					//anoutput2 << SlaveOffsetPoint.X() <<","<< SlaveOffsetPoint.Y() <<","<< SlaveOffsetPoint.Z()<<std::endl;
				}
			}
		}
		else
		{
			cut(slave_z_level,m_minlevel,slaveCutShape,slave_z_level_corrected);
			for(aCutShapeSorter.ReInit(slaveCutShape);aCutShapeSorter.More();aCutShapeSorter.Next())
			{
				//Get the PCurve and the GeomSurface
				Handle_Geom2d_Curve aCurve;
				Handle_Geom_Surface aSurface;
				TopLoc_Location aLoc;
				double first,last;
				BRep_Tool::CurveOnSurface(aCutShapeSorter.Current(),aCurve,aSurface,aLoc,first,last);
				//Reverse the PCurve if the Edge-Orientation is reversed
				if(aCutShapeSorter.Current().Orientation() == TopAbs_REVERSED) aCurve->Reverse();
				Geom2dAdaptor_Curve a2dCurveAdaptor(aCurve);
				GCPnts_QuasiUniformDeflection aPointGenerator(a2dCurveAdaptor,0.01);
				//Now get the surface normal to the generated points 
				for (int i=1;i<=aPointGenerator.NbPoints();++i)
				{
					gp_Pnt2d a2dParaPoint;
					gp_Pnt aSurfacePoint;
					TopoDS_Face aFace;
					gp_Vec Uvec,Vvec,normalVec;
					aCurve->D0(aPointGenerator.Parameter(i),a2dParaPoint);
					aSurface->D1(a2dParaPoint.X(),a2dParaPoint.Y(),aSurfacePoint,Uvec,Vvec);
					//Jetzt den Normalenvector auf die Fläche ausrechnen
					normalVec = Uvec;
					normalVec.Cross(Vvec);
					normalVec.Normalize(); 
					//Jetzt ist die Normale berechnet und auch normalisiert
					//Jetzt noch checken ob die Normale auch wirklich auf die saubere Seite zeigt
					//dazu nur checken ob der Z-Wert der Normale kleiner Null ist (dann im 1.und 2. Quadranten)
					if(normalVec.Z()< 0) normalVec.Multiply(-1.0);
					/*if ( aSurface->.Orientation() == TopAbs_Reverse ) 
					{
						normalVec.Reverse()
					}*/
					//Mal kurz den Winkel zur Grund-Ebene ausrechnen
					gp_Vec planeVec(normalVec.X(),normalVec.Y(),0.0);
					double angle = normalVec.Angle(planeVec);
					//Jetzt die Z-Komponente auf 0 setzen
					normalVec.SetZ(0.0);
					//Und Normalisieren
					normalVec.Normalize();
					//Jetzt die Normale mit folgender Formel multiplizieren
					//Aktuelle Blechdicke berechnen
					double current_sheet_thickness = m_sheet_thickness * sin(PI/2-angle);
					
					normalVec.Multiply((average_sheet_thickness-current_sheet_thickness)/cos(angle));
					//Jetzt den SlaveOffsetPunkt berechnen
					gp_Pnt SlaveOffsetPoint(aSurfacePoint.XYZ() + normalVec.XYZ());
					SlaveOffsetPoint.SetZ(SlaveOffsetPoint.Z() - ((average_sheet_thickness+radius_slave)/cos(average_angle)));
					//checken ob der neue Punkt zu nahe am alten ist. Wenn ja, dann kein push_back
					if(SlaveOffsetPointContainer.size()>0 && (SlaveOffsetPointContainer.rbegin()->SquareDistance(SlaveOffsetPoint)>(Precision::Confusion()*Precision::Confusion())))
					{
						SlaveOffsetPointContainer.push_back(SlaveOffsetPoint);
						//anoutput2 << SlaveOffsetPoint.X() <<","<< SlaveOffsetPoint.Y() <<","<< SlaveOffsetPoint.Z()<<std::endl;
					}
					else if(SlaveOffsetPointContainer.empty())
					{
						SlaveOffsetPointContainer.push_back(SlaveOffsetPoint);
						//anoutput2 << SlaveOffsetPoint.X() <<","<< SlaveOffsetPoint.Y() <<","<< SlaveOffsetPoint.Z()<<std::endl;
					}
					//Debug Output
					//build.addSinglePoint(SlaveOffsetPoint.X(),SlaveOffsetPoint.Y(),SlaveOffsetPoint.Z());

				}
			}
		}

	
		//Jetzt die beiden Bahnen interpolieren und als BSpline-Curve rausschreiben
		//Zunächst die std::vector in die OCC Strukturen bringen
		Handle(TColgp_HArray1OfPnt) MasterOffsetPoints = new TColgp_HArray1OfPnt(1, OffsetPointContainer.size());
		Handle(TColgp_HArray1OfPnt) SlaveOffsetPoints = new TColgp_HArray1OfPnt(1, SlaveOffsetPointContainer.size());
		for(unsigned int t=0;t<OffsetPointContainer.size();++t)
		{
			MasterOffsetPoints->SetValue(t+1,OffsetPointContainer[t].first);
		}
		for(unsigned int t=0;t<SlaveOffsetPointContainer.size();++t)
		{
			SlaveOffsetPoints->SetValue(t+1,SlaveOffsetPointContainer[t]);
		}
		GeomAPI_Interpolate aMasterBSplineInterpolation(MasterOffsetPoints, Standard_False, Precision::Confusion());
		aMasterBSplineInterpolation.Perform();
		Handle_Geom_BSplineCurve MasterCurve(aMasterBSplineInterpolation.Curve());
		//check results
		if (!aMasterBSplineInterpolation.IsDone()) return false;
		GeomAPI_Interpolate aSlaveBSplineInterpolation(SlaveOffsetPoints, Standard_False, Precision::Confusion());
		aSlaveBSplineInterpolation.Perform();
		Handle_Geom_BSplineCurve SlaveCurve(aSlaveBSplineInterpolation.Curve());
		//check results
		if (!aSlaveBSplineInterpolation.IsDone()) return false;
		m_all_offset_cuts_high.push_back(MasterCurve);
		m_all_offset_cuts_low.push_back(SlaveCurve);
	}
	//anoutput1.close();
	//anoutput2.close();
	
		//make your wire looks like a curve to other algorithm and generate Points to offset the curve
		//BRepAdaptor_CompCurve2 wireAdaptor(m_ordered_cuts_it->second);
		/*GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.01);
		int numberofpoints = aProp.NbPoints();
		Standard_Real Umin,Vmin,lowestdistance;
		TopoDS_Face atopo_surface,atopo_surface_shortest;
		Handle_Geom_Surface geom_surface;
		std::vector<projectPointContainer> aprojectPointContainer;
		aprojectPointContainer.clear();*/

		//Now project the points to the surface and get surface normal. 
		//for (int i=1;i<=numberofpoints;++i)
	//	{
	//		lowestdistance=200;
	//		//Aktuellen Punkt holen
	//		gp_Pnt currentPoint = aProp.Value(i);
	//		projectPointContainer OffsetPointContainer;
	//		//checken auf welches Face wir projezieren könnnen
	//		for(m_face_bb_it = m_face_bboxes.begin();m_face_bb_it!=m_face_bboxes.end();++m_face_bb_it)
	//		{
	//			//Wenn der aktuelle Punkt in der BBox enthalten ist, dann machen wir mit der Projection weiter
	//			if(checkPointinFaceBB(aProp.Value(i),m_face_bb_it->second))
	//			{
	//				atopo_surface = m_face_bb_it->first;
	//				geom_surface = BRep_Tool::Surface(atopo_surface);
	//				
	//				GeomAPI_ProjectPointOnSurf aPPS(currentPoint,geom_surface,0.001);
	//				//Wenn nichts projeziert werden kann, gehts gleich weiter zum nächsten Face bzw. der nächsten BBox
	//				if (aPPS.NbPoints() == 0) continue;
	//				//Jetzt muss das aktuelle Face gespeichert werden, da es eventuell das face ist, welches am nächsten ist
	//				double length = aPPS.LowerDistance();
	//				if(lowestdistance>length)
	//				{
	//					lowestdistance=length;
	//					atopo_surface_shortest = atopo_surface;
	//					aPPS.LowerDistanceParameters (Umin,Vmin);
	//				}
	//			}
	//		}
	//		gp_Pnt projectedPoint;
	//		gp_Vec Uvec,Vvec,normalVec,projPointVec,z_normale;
	//		geom_surface = BRep_Tool::Surface(atopo_surface_shortest);
	//		//Das Face welches am nächsten ist in der temp-struct speichern
	//		OffsetPointContainer.face = atopo_surface_shortest;
	//		geom_surface->D1(Umin,Vmin,projectedPoint,Uvec,Vvec);
	//		//Jetzt den Normalenvector auf die Fläche ausrechnen
	//		normalVec = Uvec;
	//		normalVec.Cross(Vvec);
	//		normalVec.Normalize(); 
	//		//Jetzt ist die Normale berechnet und auch normalisiert
	//		//Jetzt noch checken ob die Normale auch wirklich wie alle anderen auf die gleiche Seite zeigt.
	//		//dazu nur checken ob der Z-Wert der Normale größer Null ist (dann im 1.und 2. Quadranten)
	//		if(normalVec.Z()<0) normalVec.Multiply(-1.0);
	//		//Mal kurz den Winkel zur Grund-Ebene ausrechnen
	//		gp_Vec planeVec(normalVec.X(),normalVec.Y(),0.0);
	//		OffsetPointContainer.angle = normalVec.Angle(planeVec);
	//		//Jetzt die Normale auf die Radiuslänge verlängern
	//		normalVec.Multiply(radius);
	//		//Jetzt die Z-Komponente auf 0 setzen
	//		normalVec.SetZ(0.0);
	//		if(lowestdistance>0.2)
	//		{cout<<"error"<<endl;}
	//		//Eine Korrektur vom z-Level machen weil er durch numerische Fehler nicht exact auf dem Z-Level liegt wo wir ihn haben wollen. Die Normale wird aber trotzdem vom (durch numerische Fehler) berechneten Z-Wert genommen
	//		projectedPoint.SetZ(m_ordered_cuts_it->first);
	//		projPointVec.SetXYZ(projectedPoint.XYZ());
	//		OffsetPointContainer.point.SetXYZ((projPointVec + normalVec).XYZ());
	//		OffsetPointContainer.point.SetZ(projectedPoint.Z()+radius);//Den Radius noch dazu addieren
	//		//Den OffsetPoint jetzt in einen Offset-Point Vector pushen
	//		aprojectPointContainer.push_back(OffsetPointContainer);
	//		
	//	}
	//	//cout << tempOffsetPoints.size() << std::endl;
	//	cout << m_ordered_cuts_it->first <<std::endl;
	//	checkPointIntersection(aprojectPointContainer);
	//	//Jetzt den Z-Level für die untere Bahn ausrechnen
	//	float average_delta_z;
	//	calculateAccurateSlaveZLevel(aprojectPointContainer,radius, average_delta_z);
	//	//Jetzt beim Z-Level schneiden, und dann auch gleich Punkte erzeugen
	//	
	//	//std::vector<gp_Pnt> finalPointscorrected;
	//	//finalPointscorrected.clear();
	//	//checkPointDistance(tempOffsetPoints,finalPointscorrected);
	//	Handle(TColgp_HArray1OfPnt) finalOffsetPoints = new TColgp_HArray1OfPnt(1, aprojectPointContainer.size());
	//	for(unsigned int t=0;t<tempOffsetPoints.size();++t)
	//	{
	//		finalOffsetPoints->SetValue(t+1,tempOffsetPoints[t].first);
	//	}
	//	GeomAPI_Interpolate aNoPeriodInterpolate(finalOffsetPoints, Standard_False, Precision::Confusion());
	//	aNoPeriodInterpolate.Perform();
	//	Handle_Geom_BSplineCurve aCurve(aNoPeriodInterpolate.Curve());
	//	//check results
	//	if (!aNoPeriodInterpolate.IsDone()) return false;
	//	m_all_offset_cuts_high.push_back(aCurve);
	//}
	//		//Base::Vector3f offsetPoint,projectPoint;
	//		//offsetPoint.x=OffsetPoint.X();offsetPoint.y=OffsetPoint.Y();offsetPoint.z=OffsetPoint.Z();
	//		//projectPoint.x=projectedPoint.X();projectPoint.y=projectedPoint.Y();projectPoint.z=projectedPoint.Z();
	//		//build.addSingleArrow(projectPoint,offsetPoint);
	//		//build.addSinglePoint(offsetPoint);
	//		//outfile << currentPoint.X() <<","<<currentPoint.Y()<<","<<currentPoint.Z()<<","<< projectedPoint.X() <<","<<projectedPoint.Y()<<","<<projectedPoint.Z()<<","<< OffsetPoint.X() <<","<<OffsetPoint.Y()<<","<<OffsetPoint.Z()<<","<<normalVec.X() <<","<<normalVec.Y()<<","<<normalVec.Z()<< std::endl;
	//	}
	////		build.addSinglePoint(projectedPoint.X(),projectedPoint.Y(),projectedPoint.Z());
	////		outfile << projectedPoint.X() <<","<<projectedPoint.Y()<<","<<projectedPoint.Z()<<std::endl;
	//		//Jetzt die aktuelle Kurve als BSpline interpolieren
	//	GeomAPI_Interpolate aNoPeriodInterpolate(finalOffsetPoints, Standard_False, Precision::Confusion());
	//	aNoPeriodInterpolate.Perform();
	//	Handle_Geom_BSplineCurve aCurve(aNoPeriodInterpolate.Curve());
	//	 //check results
	//	if (!aNoPeriodInterpolate.IsDone()) return false;
	//	m_all_offset_cuts_high.push_back(aCurve);
	//	
	//}

	//		IntCurvesFace_ShapeIntersector shp_int;
	//		gp_Pnt OffsetPointUP;
	//		gp_Dir pl_vec;
	//		gp_Lin line;
	//		gp_Vec Uvec,Vvec,normalVec,projPointVec;
	//		line.SetLocation(aProp.Value(i));
	//		line.SetDirection(pl_vec);
	//		shp_int.Load(m_Shape, 0.001);
	//		shp_int.PerformNearest(line, -RealLast(), +RealLast());
	//		if(shp_int.IsDone())  
	//		gp_Pnt projectedPoint;
	//		geom_surface->D1(shp_int.UParameter(1),shp_int.VParameter(1),projectedPoint,Uvec,Vvec);
	//		//Jetzt die Normale berechnen
	//		normalVec = Uvec;
	//		normalVec.Cross(Vvec);
	//		//Jetzt wird die Normale berechnet und auch normalisiert
	//		normalVec.Normalize(); 
	//		//Jetzt wird die Normale auf die Radiuslänge verlängern
	//		normalVec.Multiply(radius);
	//		//Jetzt die Z-Komponente auf 0 setzen
	//		normalVec.SetZ(0.0);
	//		//Eine Korrektur vom z-Level machen weil er durch numerische Fehler nicht exact auf dem Z-Level liegt wo wir ihn haben wollen. Die Normale wird aber trotzdem vom (durch numerische Fehler) berechneten Z-Wert genommen
	//		projectedPoint.SetZ(current_z_level);
	//		projPointVec.SetXYZ(projectedPoint.XYZ());
	//		OffsetPointUP.SetXYZ((projPointVec + normalVec).XYZ());
	//		OffsetPointUP.SetZ(projectedPoint.Z()+radius);//Den Radius noch dazu addieren
	//		finalOffsetPointsUP->SetValue(i,OffsetPointUP); //Aktuellen OffsetPoint setzen

		/*	TopExp_Explorer ExShape;*/
	/*		ExShape.Init(m_Shape,TopAbs_FACE);
			Standard_Real Umin,Vmin;
			double distance_old,distance;
			Handle_Geom_Surface nearest_surface;
			distance_old = 200.0;
			for (; ExShape.More(); ExShape.Next())
			{
				const TopoDS_Face &atopo_surface =  TopoDS::Face (ExShape.Current());
				Handle_Geom_Surface geom_surface = BRep_Tool::Surface(atopo_surface);
				gp_Pnt currentPoint = aProp.Value(i);
				GeomAPI_ProjectPointOnSurf aPPS(currentPoint,geom_surface,0.001);
				if (! aPPS.IsDone())
				{
					return false;
				}
				distance = aPPS.LowerDistance();
				if(distance<distance_old)
				{	
					distance_old = distance;
					aPPS.LowerDistanceParameters (Umin,Vmin);
					nearest_surface = geom_surface;
				}
			}*/
	//		gp_Pnt projectedPoint,OffsetPoint;
	//		gp_Vec Uvec,Vvec,normalVec,projPointVec;
	//		nearest_surface->D1(Umin,Vmin,projectedPoint,Uvec,Vvec);
	//		normalVec = Uvec;
	//		normalVec.Cross(Vvec);
	//		normalVec.Normalize(); //Jetzt ist die Normale berechnet und auch normalisiert
	//		//Jetzt die Normale auf die Radiuslänge verlängern
	//		normalVec.Multiply(12.5);
	//		//Jetzt die Z-Komponente auf 0 setzen
	//		normalVec.SetZ(0.0);
	//		//Eine Korrektur vom z-Level machen weil er durch numerische Fehler nicht exact auf dem Z-Level liegt wo wir ihn haben wollen. Die Normale wird aber trotzdem vom (durch numerische Fehler) berechneten Z-Wert genommen
	//		projectedPoint.SetZ(current_z_level);
	//		projPointVec.SetXYZ(projectedPoint.XYZ());
	//		OffsetPoint.SetXYZ((projPointVec + normalVec).XYZ());
	//		OffsetPoint.SetZ(projectedPoint.Z()+12.5);//Den Radius noch dazu addieren
	//		finalOffsetPoints->SetValue(i,OffsetPoint); //Aktuellen OffsetPoint setzen
	//		build.addSinglePoint(OffsetPoint.X(),OffsetPoint.Y(),OffsetPoint.Z(),2 ,0,0,0);
	//		build.addSinglePoint(projectedPoint.X(),projectedPoint.Y(),projectedPoint.Z());
	//		outfile << projectedPoint.X() <<","<<projectedPoint.Y()<<","<<projectedPoint.Z()<<std::endl;
	//	}
	//
	//	GeomAPI_Interpolate aNoPeriodInterpolate(finalOffsetPoints, Standard_False, Precision::Confusion());
	//	aNoPeriodInterpolate.Perform();
	//	Handle_Geom_BSplineCurve aCurve(aNoPeriodInterpolate.Curve());
	//	
	//	// check results
	//	if (!aNoPeriodInterpolate.IsDone()) return false;
	//	m_all_offset_cuts_high.push_back(aCurve);
	//	
	//}
//build.saveToFile("c:/output.iv");
//outfile.close();

//
	return true;
}

#include "BRepAdaptor_CompCurve2.h"
Base::BoundBox3f cutting_tools::getWireBBox(TopoDS_Wire aWire)
{
	//Fill Bounding Boxes with Wires
	//Therefore we have to evaluate some points on our wire and feed the BBox Algorithm
	Base::BoundBox3f currentBox;
	BRepAdaptor_CompCurve2 wireAdaptor(aWire);
	GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.1);
	Base::Vector3f aPoint;
	currentBox.Flush();
	for(int j=1;j<=aProp.NbPoints();++j)
	{
		aPoint.x = aProp.Value(j).X();
		aPoint.y = aProp.Value(j).Y();
		aPoint.z = aProp.Value(j).Z();
		currentBox.Add(aPoint);
	}	
	return currentBox;
}


bool cutting_tools::calculateAccurateSlaveZLevel(std::vector<std::pair<gp_Pnt,double> >& OffsetPoints, double current_z_level, double &slave_z_level, double &average_sheet_thickness, double &average_angle)
{
	//Mittelwert von allen Normalenwinkeln und damit dann den Mittelwert der Blechdicke bilden
	average_angle = 0.0;
	
	for(unsigned int i=0;i<OffsetPoints.size();++i) 
	{average_angle = average_angle + OffsetPoints[i].second;}
	
	average_angle = average_angle/OffsetPoints.size();
	//Average Sheet thickness based on sin-law
	average_sheet_thickness = m_sheet_thickness * sin ((PI/2)-average_angle);

	slave_z_level = current_z_level + \
					(m_radius*(1-sin(average_angle))) - \
					(sin(average_angle)*(average_sheet_thickness+m_radius_slave)) + \
					((average_sheet_thickness+m_radius_slave)/cos(average_angle));

	return  true;
}

//bool cutting_tools::OffsetWires() //Hier ist die alte Version
//{
//	for(m_it = m_all_cuts.begin();m_it<m_all_cuts.end();++m_it)
//	{
//		// make your wire looks like a curve to other algorithm and generate Points to offset the curve
//		BRepAdaptor_CompCurve2 wireAdaptor((*m_it).second);
//		GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.001);
//		int numberofpoints = aProp.NbPoints();
//		Handle(TColgp_HArray1OfPnt) finalOffsetPoints = new TColgp_HArray1OfPnt(1, numberofpoints);
//		//Now project the points to the surface. As we might have many faces just project to every face and take the min distance
//		for (int i=1;i<=numberofpoints;++i)
//		{
//			TopExp_Explorer ExShape;
//			ExShape.Init(m_Shape,TopAbs_FACE);
//			Standard_Real Umin,Vmin;
//			double distance_old,distance;
//			Handle_Geom_Surface nearest_surface;
//			distance_old = 200.0;
//			for (; ExShape.More(); ExShape.Next())
//			{
//				const TopoDS_Face &atopo_surface =  TopoDS::Face (ExShape.Current());
//				Handle_Geom_Surface geom_surface = BRep_Tool::Surface(atopo_surface);
//				gp_Pnt currentPoint = aProp.Value(i);
//				GeomAPI_ProjectPointOnSurf aPPS(currentPoint,geom_surface,0.001);
//				if (! aPPS.IsDone())
//				{
//					return false;
//				}
//				distance = aPPS.LowerDistance();
//				if(distance<distance_old)
//				{	
//					distance_old = distance;
//					aPPS.LowerDistanceParameters (Umin,Vmin);
//					nearest_surface = geom_surface;
//				}
//			}
//			gp_Pnt projectedPoint,OffsetPoint;
//			gp_Vec Uvec,Vvec,normalVec,projPointVec;
//			nearest_surface->D1(Umin,Vmin,projectedPoint,Uvec,Vvec);
//			normalVec = Uvec;
//			normalVec.Cross(Vvec);
//			normalVec.Normalize(); //Jetzt ist die Normale berechnet und auch normalisiert
//			//zeroPoint.SetCoord(0.0,0.0,0.0); //
//			projPointVec.SetXYZ(projectedPoint.XYZ());
//			OffsetPoint.SetXYZ((projPointVec + (normalVec*(-21))).XYZ());
//			finalOffsetPoints->SetValue(i,OffsetPoint); //Aktuellen OffsetPoint setzen
//		}
//		Standard_Boolean isPeriodic = Standard_False;
//		GeomAPI_Interpolate aNoPeriodInterpolate(finalOffsetPoints, isPeriodic, Precision::Confusion());
//		aNoPeriodInterpolate.Perform();
//		Handle_Geom_BSplineCurve aCurve(aNoPeriodInterpolate.Curve());
//	
//		// check results
//		if (!aNoPeriodInterpolate.IsDone()) return false;
//		m_all_offset_cuts_high.push_back(aCurve);
//	}
//}

//bool cutting_tools::featureBasedToolPath()
//{
//
//}
bool cutting_tools::cut_Mesh(float z_level, float min_level, std::list<std::vector<Base::Vector3f> >&result, float &z_level_corrected)
{
	//std::ofstream outfile;
	//outfile.open("c:/mesh_cut.out");
	Base::Vector3f z_level_plane,normal;
	z_level_plane.z=z_level;
	normal.x=0;normal.y=0;normal.z=1.0;
	bool cutok;
	//Die Richtung für die Korrektur wird hier festgelegt
	bool direction=true;
	float factor = 0.0;
	do
	{
		cutok = true;
		m_aMeshAlgo->CutWithPlane(z_level_plane,normal,*m_CAD_Mesh_Grid,result);
		//std::list<std::vector<Base::Vector3f> >::iterator it;
		//std::vector<Base::Vector3f>::iterator vector_it;
		//checken ob wirklich ein Schnitt zustande gekommen ist
		if(result.size()==0)
		{
			cutok = false;
			//Jedes Mal ein wenig mehr Abstand für die Korrektur einfügen
			factor = factor+0.05;
			if(factor>=1) factor = 0.95;
			//Wenn wir das erste Mal eine Korrektur machen müssen gehts zunächst mal mit Minus rein
			if(direction)
			{
				z_level_plane.z = (z_level-(m_pitch*factor));
				z_level_corrected = z_level_plane.z;
				direction=false;
				continue;
			}
			else
			{
				z_level_plane.z = (z_level+(m_pitch*factor));
				z_level_corrected = z_level_plane.z;
				direction=true;
				continue;
			}
		}
	}while (cutok==false);
	//for(vector_it=(*(result.begin())).begin();vector_it<(*(result.begin())).end();++vector_it)
	//outfile << (*vector_it).x <<","<<(*vector_it).y <<","<<(*vector_it).z<< std::endl;
	//outfile.close();
	return true;
}






bool cutting_tools::cut(float z_level, float min_level, TopoDS_Shape &aCutShape, float &z_level_corrected)
{
	gp_Pnt aPlanePnt(0,0,z_level);
    gp_Dir aPlaneDir(0,0,1);
	bool cutok;
	//Die Richtung für die Korrektur wird hier festgelegt
	bool direction=true;
	float factor = 0.0;
	do
	{
		cutok = true;
		Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
		BRepBuilderAPI_MakeFace Face(aPlane);
		BRepAlgo_Section mkCut(m_Shape, Face.Face(),Standard_False);
		mkCut.Approximation (Standard_True);
		mkCut.ComputePCurveOn1(Standard_True);
		mkCut.Build();
		//Den neuen Algorithmus checken
		//Edgesort aSorter(mkCut.Shape());
		//aSorter.Init();
		
		//Jetzt checken ob auch wirlich edges vorhanden sind
		TopExp_Explorer exploreShape;
		exploreShape.Init(mkCut.Shape(),TopAbs_EDGE);
		//Wenn keine Edge vorhanden ist
		if(!exploreShape.More())
		{
			cutok = false;
			//Jedes Mal ein wenig mehr Abstand für die Korrektur einfügen
			factor = factor+0.05;
			if(factor>=1) factor = 0.95;
			//Wenn wir das erste Mal eine Korrektur machen müssen gehts zunächst mal mit Minus rein
			if(direction)
			{
				aPlanePnt.SetZ(z_level-(m_pitch*factor));
				z_level_corrected = aPlanePnt.Z();
				direction=false;
				continue;
			}
			else
			{
				aPlanePnt.SetZ(z_level+(m_pitch*factor));
				z_level_corrected = aPlanePnt.Z();
				direction=true;
				continue;
			}
		}
		//Das Shape, welches per Referenz übergeben wird jetzt mit dem geordneten Schnitt füllen
		aCutShape = mkCut.Shape();

	}while (cutok==false);

	return true;
}





bool cutting_tools::classifyShape()
{
    TopExp_Explorer Explorer;
    Explorer.Init(m_Shape,TopAbs_FACE);
    if (!Explorer.More()) return false;
	//checken wieviele verschiedene Faces wir haben
	int k=0;
	for (; Explorer.More(); Explorer.Next()) 
    {
        k++;
    }
	//Wenn mehr als ein Face vorhanden, dann eine Membervariable setzen
	if (k>1) m_cad = true;
	return true;
}



/* Hier ging das alte cut los




	bool cutok=true;
    //Falls wir nur ein Face haben und keine flachen Bereiche
    if (m_all_cuts.empty())
    {
        //Schnitte über die Bounding Box bestimmen
        Bnd_Box currentBBox;
		
        Standard_Real XMin, YMin, ZMin, XMax, YMax, ZMax;
        BRepBndLib::Add(m_Shape, currentBBox );
        currentBBox.SetGap(0.0);
        currentBBox.Get(XMin, YMin, ZMin, XMax, YMax, ZMax);
        double maxlevel=Max(ZMax,ZMin);
        double minlevel=Min(ZMax,ZMin);
        int cutnumber = fabs((maxlevel-minlevel)/m_pitch);//Cast um die Nachkommastellen wegzuschneiden
        m_pitch = fabs(maxlevel-minlevel)/cutnumber;//m_pitch leicht korrigieren um wirklich auf die letzte Ebene zu kommen

		//Aktuell wird die letzte Ebene bei selbst approxmierten Flächen nicht als Bahnkurve betrachtet
		//Auch die erste Ebene fällt komplett weg, da unwichtig. Lediglich für die untere Maschine ist die Bahn sinnvoll falls überhaupt noch flache Bereiche vorhanden sind
        for (int i=0;i<cutnumber;++i) 
		{
            //Jetzt schneiden (die oberste Ebene auslassen)
            double z_level = maxlevel-(i*m_pitch);
            gp_Pnt aPlanePnt(0,0,z_level);
            gp_Dir aPlaneDir(0,0,1);
			
			do
			{
				cutok = true;
				Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
				BRepBuilderAPI_MakeFace Face(aPlane);
				BRepAlgo_Section mkCut(m_Shape, Face.Face(),Standard_False);
				mkCut.Approximation (Standard_True);
				mkCut.Build();
				//Jetzt checken ob auch wirlich edges vorhanden sind
				TopExp_Explorer exploreShape;
				exploreShape.Init(mkCut.Shape(),TopAbs_EDGE);
				//Wenn keine Edge vorhanden ist
				if(!exploreShape.More())
				{
					cutok = false;
					aPlanePnt.SetZ(z_level-(m_pitch/5));
					if(aPlanePnt.Z()<minlevel) aPlanePnt.SetZ(minlevel+m_pitch/5);
					continue;
				}
				//Weil der Punkt sich ja geändert haben könnte
				m_zl_wire_combination.first = aPlanePnt.Z();
				m_zl_wire_combination.second = ordercutShape(mkCut.Shape());
				//Geordnete Edges in den All_Cuts-Vector stecken
				m_all_cuts.push_back(m_zl_wire_combination);
			}while (cutok==false);
		}
        return true;
    }
    //Bei mehreren flachen Bereichen
    else
    {
        std::vector<float> InitialPlaneLevels;
		std::vector<std::pair<float,TopoDS_Wire> > atemp_storage;
		atemp_storage.clear();
        std::vector<float>::iterator temp_it;
        for (m_it= m_all_cuts.begin();m_it<m_all_cuts.end();++m_it)
        {
            //Die Wires filtern und z.B. bei zwei Wires auf der obersten Ebene nur die innere nehmen
			Bnd_Box currentBBox,BBox2;
			Standard_Real X1Min, Y1Min, Z1Min, X1Max, Y1Max, Z1Max,X2Min, Y2Min, Z2Min, X2Max, Y2Max, Z2Max;
			BRepBndLib::Add((*m_it).second, currentBBox );
			currentBBox.SetGap(0.0);
			currentBBox.Get(X1Min, Y1Min, Z1Min, X1Max, Y1Max, Z1Max);
			if((*(m_it+1)).first == (*m_it).first) //Wenn die beiden Wires auf der gleichen Ebene liegen.....
			{
				BRepBndLib::Add((*(m_it+1)).second,BBox2);
				BBox2.SetGap(0.0);
				BBox2.Get(X2Min,Y2Min,Z2Min,X2Max,Y2Max,Z2Max);
			}
			//Jetzt checken welche kleiner ist
			if(X1Min<X2Min && X1Max>X2Max && Y1Min < Y2Min && Y1Max>Y2Max) //1 ist größer
			{
				if(m_it == m_all_cuts.begin())//Wenn wir auf der obersten Ebene sind....
				{
					atemp_storage.push_back(*(m_it+1));
					m_it++;//Wir überspringen damit das nächste //Damit haben wir nur noch ein Problem falls wir mehr als zwei Wires auf einer ebene haben.
				}
				else
				{
					atemp_storage.push_back(*(m_it));
				}
			}
			//Mal schauen ob der Wert schon in der Liste vorhanden ist
            temp_it = std::find(InitialPlaneLevels.begin(),InitialPlaneLevels.end(),(*m_it).first);
            if (temp_it == InitialPlaneLevels.end())
            {
                InitialPlaneLevels.push_back((*m_it).first);
            }
        }
		//Jetzt die flachen Bereiche der Höhe nach sortieren
		std::sort(InitialPlaneLevels.begin(),InitialPlaneLevels.end(),FloatHuge);

		//Die Schnitte müssen jetzt zwischen die flachen Stücke einsortiert werden
		for (temp_it=InitialPlaneLevels.begin();temp_it<InitialPlaneLevels.end();++temp_it)
        {
            //Debug cout << "Bereich" <<endl;
			double maxlevel=*temp_it;//Maximaler aktueller Wert
			if(temp_it+1==InitialPlaneLevels.end()) continue;
			double minlevel=*(temp_it+1);
			int cutnumber = fabs((maxlevel-minlevel)/m_pitch);//Cast um die Nachkommastellen wegzuschneiden
			m_pitch = fabs(maxlevel-minlevel)/cutnumber;
			
			for (int i=1;i<cutnumber;++i)
			{
				//Jetzt schneiden (die oberste Ebene auslassen und die unterste, da dort wieder wires kommen)
				double z_level = maxlevel-(i*m_pitch);
				gp_Pnt aPlanePnt(0,0,z_level);
				gp_Dir aPlaneDir(0,0,1);
				cutok=true;
				do
				{
					Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
					BRepBuilderAPI_MakeFace Face(aPlane);
					BRepAlgo_Section mkCut(m_Shape, Face.Face(),Standard_False);
					try
					{
						mkCut.Approximation (Standard_True);
						mkCut.Build();
					} 
					catch(...) 
					{
						cutok = false;	
						aPlanePnt.SetZ(z_level-(m_pitch/5));
						continue;
					}

					//Weil der Punkt sich ja geändert haben könnte
					m_zl_wire_combination.first = aPlanePnt.Z();
					m_zl_wire_combination.second = ordercutShape(mkCut.Shape());
					//Geordnete Edges in den All_Cuts-Vector stecken
					m_all_cuts.push_back(m_zl_wire_combination);
				}while (cutok=false);

			}
		}
	}
}
*/

//TopoDS_Compound  cutting_tools::getCutShape()
//{
//		BRep_Builder builder;     
//		TopoDS_Compound Comp;     
//		builder.MakeCompound(Comp);     
////		for(m_it = m_ordered_wires.begin();m_it < m_ordered_wires.end();++m_it)
////		{
////			builder.Add(Comp,(*m_it).second);
////		}
//		return Comp;
//
//}




double cutting_tools::GetWireLength(TopoDS_Wire &aWire)
{
	GProp_GProps lProps;
	BRepGProp::LinearProperties(aWire,lProps);
	double length = lProps.Mass();
	return length;
}
                
