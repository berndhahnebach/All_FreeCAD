#include "PreCompiled.h"
#include "cutting_tools.h"

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomAPI_IntSS.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCompCurve.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <Geom_OffsetSurface.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <GeomAPI_IntCS.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Approx_Curve3d.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepOffset.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepAlgo_Section.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Handle_Geom_Plane.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include "WireExplorer.h"
#include "BRepAdaptor_CompCurve2.h"
#include <BRepBuilderAPI_MakeWire.hxx>
#include "stuff.h"
#include <Base/Builder3D.h>
#include "best_fit.h"

cutting_tools::cutting_tools(TopoDS_Shape &aShape, float pitch,bool mirrortobothsides)
        : m_Shape(aShape),
        m_mirrortobothsides(mirrortobothsides),
        m_pitch(pitch),
		m_aMeshAlgo(NULL),
		m_CAD_Mesh_Grid(NULL)
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
	//Everything should be initialised now
	
}

cutting_tools::~cutting_tools()
{
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
	//Da es nicht funktioniert, wenn wir nur ein Face reinschieben. Muss ich ein paar Punkte bzw. Vertex erzeugen und diese dann in die BBox schieben
	TopoDS_Face atopo_surface;
    BRepAdaptor_Surface aAdaptor_Surface;
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
		std::pair<TopoDS_Face,Base::BoundBox3f> tempPair;
		tempPair.first = atopo_surface;
		tempPair.second = aBoundBox;
		m_face_bboxes.push_back(tempPair);




  //      aAdaptor_Surface.Initialize(atopo_surface);
  //      Standard_Real FirstUParameter, LastUParameter,FirstVParameter,LastVParameter;
  //      gp_Pnt aSurfacePoint;
		//FirstUParameter = aAdaptor_Surface.FirstUParameter();
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
	}
	return true;
}

bool cutting_tools::checkPointinFaceBB(const gp_Pnt &aPnt,const Base::BoundBox3f &aBndBox)
{
	//aBndBox.Enlarge(float(1.05));
	if((aPnt.X()>aBndBox.MinX) && (aPnt.X()<aBndBox.MaxX) && (aPnt.Y()>aBndBox.MinY) && (aPnt.Y()<aBndBox.MaxY) && (aPnt.Z()>aBndBox.MinZ) && (aPnt.Z()<aBndBox.MaxZ))
		return true;

	return false;
}
bool cutting_tools::initializeMeshStuff()
{
	
	m_CAD_Mesh_Grid = new MeshCore::MeshFacetGrid(m_CAD_Mesh);
	m_aMeshAlgo = new MeshCore::MeshAlgorithm(m_CAD_Mesh);

	//Test für die Curvature mal schnell durchführen
	best_fit abestfit(m_CAD_Mesh);
	abestfit.mesh_curvature();
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
		TopoDS_Wire aTopoWire;
		for (int i=1;i<=cutnumber;++i) 
		{
            //Jetzt schneiden (die oberste Ebene auslassen)
            z_level = m_maxlevel-(i*m_pitch);
			z_level_corrected = z_level;
			std::list<std::vector<Base::Vector3f> > result;
			result.clear();
			//cut(z_level,m_minlevel,aTopoWire,z_level_corrected);
			cut_Mesh(z_level,m_minlevel,result);

			//Jetzt die resultierenden Points in den vector schieben
			std::pair<float,std::list<std::vector<Base::Vector3f> > > tempPair;
			tempPair.first = z_level_corrected;
			tempPair.second = result;
			m_ordered_cuts.push_back(tempPair);
		}
        return true;
    }
	//Wenn wir mehrere Faces haben oder eine CAD-Geometrie vorhanden ist
	else
	{
		//Erst mal schauen ob der maximale Wert vom Bauteil m_maxlevel mit dem höchsten flachen Bereich zusammenfällt
		if(m_zl_wire_combination.empty()) return false;
		float test = m_maxlevel-m_zl_wire_combination.rbegin()->first;
		//Oberste Ebene vom Bauteil fällt mit der obersten flachen Ebene zusammen
		//-> Schnitte von der obersten flachen Ebene bis zur nächsten flachen Ebene oder bis ganz nach unten m_minlevel
		if (fabs(test) < 1)
		{
			float temp_max = m_zl_wire_combination.rbegin()->first;
			float temp_min;
			std::multimap<float,TopoDS_Wire>::iterator m_zl_wire_it;
			//Wir holen uns jetzt den nächsten Z-Level raus
			m_zl_wire_it = m_zl_wire_combination.upper_bound(temp_max-0.1);
			if(m_zl_wire_it->first == temp_max)
			{
				cout << "Tja, es gibt wohl nur eine flache Area";
				temp_min = m_minlevel;
			}
			//Wenn es mehrer flache Bereiche gibt muss ich nochmal weitermachen
			else
			{
				cout << "Mehrere Areas erkannt";
			}
			//Jetzt schnippeln von temp_max bis temp_min
			int cutnumber = (int)fabs((temp_max-temp_min)/m_pitch);
			//m_pitch leicht korrigieren um wirklich auf die letzte Ebene zu kommen
			m_pitch = fabs(temp_max-temp_min)/cutnumber;
			//Jetzt die Schnitte machen. Die höchste Ebene fällt weg, da hier noch kein Blech gedrückt wird
			float z_level,z_level_corrected;
			TopoDS_Wire aTopoWire;
			//Jetzt schneiden (die oberste Ebene auslassen)
			for (int i=1;i<=2;++i) 
			{
				z_level = temp_max-(i*m_pitch);
				z_level_corrected = z_level;
				std::list<std::vector<Base::Vector3f> > result;
				result.clear();
				//cut(z_level,m_minlevel,aTopoWire,z_level_corrected);
				cut_Mesh(z_level,m_minlevel,result);

				//Jetzt die resultierenden Points in den vector schieben
				std::pair<float,std::list<std::vector<Base::Vector3f> > > tempPair;
				tempPair.first = z_level_corrected;
				tempPair.second = result;
				m_ordered_cuts.push_back(tempPair);
				//cut(z_level,temp_min, aTopoWire,z_level_corrected);
				//Jetzt die gefüllte Wire in den vector schieben
				//std::pair<float,TopoDS_Wire> tempPair;
				//tempPair.first = z_level_corrected;
				//tempPair.second = aTopoWire;
				//m_ordered_cuts.push_back(tempPair);

			}
			return true;
		}
	}

	return false;
}

bool cutting_tools::checkFlatLevel()
{
	//Falls keine CAD-Geometrie da ist, gleich wieder rausspringen
	//classifyShape();
	
	if(m_cad==false) return false;
    
	TopoDS_Face atopo_surface;
    BRepAdaptor_Surface aAdaptor_Surface;
    TopExp_Explorer Explorer;

    Explorer.Init(m_Shape,TopAbs_FACE);

    for (;Explorer.More();Explorer.Next())
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
			std::pair<float,TopoDS_Wire> aTempPair;
            //Z-Wert vom flachen Bereich in ein temporäres pair pushen
			aTempPair.first = ((first.Z()+second.Z()+third.Z())/3);
			//Jetzt durch das Face gehen und die Wires rausfiltern
			TopExp_Explorer Explore_Face;
            Explore_Face.Init(atopo_surface,TopAbs_WIRE);
			//If there is no Wire -> return
            if (!Explore_Face.More()) return false; 

			//Jetzt alle Wires in die Map schieben
            for (Explore_Face.ReInit();Explore_Face.More();Explore_Face.Next())
            {
                TopoDS_Wire wire = TopoDS::Wire(Explore_Face.Current());
				aTempPair.second = wire;
				m_zl_wire_combination.insert(aTempPair);
            }
		}
    }
    
    return true;
}

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

bool cutting_tools::OffsetWires_Standard(float radius) //Version wo nur in X,Y-Ebene verschoben wird
{

	//Base::Builder3D build;
	//std::ofstream outfile;
	//outfile.open("c:/atest.out");

//	for(m_it = m_ordered_cuts.begin();m_it<m_ordered_cuts.end();++m_it)
	//{
		// make your wire looks like a curve to other algorithm and generate Points to offset the curve
		/*BRepAdaptor_CompCurve2 wireAdaptor((*m_it).second);
		GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.001);
		int numberofpoints = aProp.NbPoints();
		Handle(TColgp_HArray1OfPnt) finalOffsetPoints = new TColgp_HArray1OfPnt(1, numberofpoints);
		float current_z_level = (*m_it).first;
		Standard_Real Umin,Vmin,lowestdistance;
		TopoDS_Face atopo_surface,atopo_surface_shortest;
		Handle_Geom_Surface geom_surface;*/

	//	//Now project the points to the surface and get surface normal. 
	//	for (int i=1;i<=numberofpoints;++i)
	//	{
	//		lowestdistance=200;
	//		//Aktuellen Punkt holen
	//		gp_Pnt currentPoint = aProp.Value(i);
	//		//checken auf welches Face wir projezieren könnnen
	//		for(m_face_bb_it = m_face_bboxes.begin();m_face_bb_it<m_face_bboxes.end();++m_face_bb_it)
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
	//		gp_Pnt projectedPoint,OffsetPoint;
	//		gp_Vec Uvec,Vvec,normalVec,projPointVec,z_normale;
	//		geom_surface = BRep_Tool::Surface(atopo_surface_shortest);
	//		geom_surface->D1(Umin,Vmin,projectedPoint,Uvec,Vvec);
	//		normalVec = Uvec;
	//		normalVec.Cross(Vvec);
	//		normalVec.Normalize(); //Jetzt ist die Normale berechnet und auch normalisiert
	//		//Jetzt noch checken ob die Normale auch wirklich wie alle anderen auf die gleiche Seite zeigt.
	//		//dazu nur checken ob der Z-Wert der Normale größer Null ist (dann im 1.und 2. Quadranten)
	//		if(normalVec.Z()<0) normalVec.Multiply(-1.0);
	//		//Jetzt die Normale auf die Radiuslänge verlängern
	//		normalVec.Multiply(radius);
	//		//Jetzt die Z-Komponente auf 0 setzen
	//		normalVec.SetZ(0.0);
	//		float abstand = currentPoint.Distance(projectedPoint);
	//		if(abstand>0.2)
	//		{cout<<"error"<<endl;}
	//		//Eine Korrektur vom z-Level machen weil er durch numerische Fehler nicht exact auf dem Z-Level liegt wo wir ihn haben wollen. Die Normale wird aber trotzdem vom (durch numerische Fehler) berechneten Z-Wert genommen
	//		projectedPoint.SetZ(current_z_level);
	//		projPointVec.SetXYZ(projectedPoint.XYZ());
	//		OffsetPoint.SetXYZ((projPointVec + normalVec).XYZ());
	//		OffsetPoint.SetZ(projectedPoint.Z()+radius);//Den Radius noch dazu addieren
	//		finalOffsetPoints->SetValue(i,OffsetPoint); //Aktuellen OffsetPoint setzen
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


	return true;
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
bool cutting_tools::cut_Mesh(float z_level, float min_level, std::list<std::vector<Base::Vector3f> >&result)
{
	std::ofstream outfile;
	outfile.open("c:/mesh_cut.out");
	Base::Vector3f z_level_plane,normal;
	z_level_plane.z=z_level;
	normal.x=0;normal.y=0;normal.z=1.0;
	m_aMeshAlgo->CutWithPlane(z_level_plane,normal,*m_CAD_Mesh_Grid,result);
	std::list<std::vector<Base::Vector3f> >::iterator it;
	std::vector<Base::Vector3f>::iterator vector_it;
	//Zum testen mal schnell den Output überprüfen

		for(vector_it=(*(result.begin())).begin();vector_it<(*(result.begin())).end();++vector_it)
		outfile << (*vector_it).x <<","<<(*vector_it).y <<","<<(*vector_it).z<< std::endl;

	outfile.close();
	return true;


}






bool cutting_tools::cut(float z_level, float min_level, TopoDS_Wire &aWire, float &z_level_corrected)
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
		mkCut.Build();
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
		//Die Wire, welche per Referenz übergeben wird jetzt mit dem geordneten Schnitt füllen
		aWire = ordercutShape(mkCut.Shape());

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

TopoDS_Compound  cutting_tools::getCutShape()
{
		BRep_Builder builder;     
		TopoDS_Compound Comp;     
		builder.MakeCompound(Comp);     
//		for(m_it = m_ordered_wires.begin();m_it < m_ordered_wires.end();++m_it)
//		{
//			builder.Add(Comp,(*m_it).second);
//		}
		return Comp;

}




double cutting_tools::GetWireLength(TopoDS_Wire &aWire)
{
	GProp_GProps lProps;
	BRepGProp::LinearProperties(aWire,lProps);
	double length = lProps.Mass();
	return length;
}
                
