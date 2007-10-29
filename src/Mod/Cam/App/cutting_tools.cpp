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
#include <Bnd_Box.hxx>
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
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include "WireExplorer.h"
#include "BRepAdaptor_CompCurve2.h"
#include <BRepBuilderAPI_MakeWire.hxx>
#include "stuff.h"
#include <Base/Builder3d.h>

cutting_tools::cutting_tools(TopoDS_Shape &aShape, float pitch,bool mirrortobothsides)
        : m_Shape(aShape),
        m_mirrortobothsides(mirrortobothsides),
        m_pitch(pitch)
{
    m_all_cuts.clear();
	m_all_offset_cuts_high.clear();
	m_all_offset_cuts_low.clear();
}

cutting_tools::~cutting_tools()
{
}


bool cutting_tools::getFlatLevels()
{
    TopoDS_Face atopo_surface;
    BRepAdaptor_Surface aAdaptor_Surface;
    TopExp_Explorer Explorer;

    Explorer.Init(m_Shape,TopAbs_FACE);
    if (!Explorer.More()) return false;

    for (;Explorer.More();Explorer.Next())
    {
        atopo_surface = TopoDS::Face (Explorer.Current());
        aAdaptor_Surface.Initialize(atopo_surface);
        Standard_Real FirstUParameter, LastUParameter,FirstVParameter,LastVParameter;
        gp_Pnt first,second,third;
        gp_Vec first_u,first_v,second_u,second_v,third_u,third_v, Norm_first,Norm_second,Norm_third,Norm_average;
        double u_middle,v_middle;
        //Generate three random point on the surface to get the surface normal and decide wether its a
        //planar face or not
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
            //Z-Wert vom flachen Bereich in einen Vektor pushen
            m_zl_wire_combination.first = ((first.Z()+second.Z()+third.Z())/3);
            
			TopExp_Explorer Explore_Face;
            Explore_Face.Init(atopo_surface,TopAbs_WIRE);
            if (!Explore_Face.More()) return false; //If there is no Wire return

            BRep_Builder builder;

            std::vector<TopoDS_Wire> aTopoWire_Container;
            aTopoWire_Container.clear();
            int k=0;
            for (Explore_Face.ReInit();Explore_Face.More();Explore_Face.Next())
            {
                TopoDS_Wire wire = TopoDS::Wire(Explore_Face.Current());
                aTopoWire_Container.push_back(wire);
                k++;

            }
            for (int j=0;j<k;++j)
            {
                m_zl_wire_combination.second = aTopoWire_Container[j]; //Z-Level bleibt ja gleich fürs Pair
                m_all_cuts.push_back(m_zl_wire_combination);
            }
        }
    }
    //Nach dem Z-Level sortieren (Von groß zu klein)
    std::sort(m_all_cuts.begin(),m_all_cuts.end(),FloatWirePairHuge);
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

bool cutting_tools::OffsetWires() //Version wo nur in X,Y-Ebene verschoben wird
{
	Base::Builder3D build;
	std::ofstream outfile;
	outfile.open("c:/atest.out");
	for(m_it = m_all_cuts.begin();m_it<m_all_cuts.end();++m_it)
	{
		// make your wire looks like a curve to other algorithm and generate Points to offset the curve
		BRepAdaptor_CompCurve2 wireAdaptor((*m_it).second);
		GCPnts_QuasiUniformDeflection aProp(wireAdaptor,0.001);
		int numberofpoints = aProp.NbPoints();
		Handle(TColgp_HArray1OfPnt) finalOffsetPoints = new TColgp_HArray1OfPnt(1, numberofpoints);
		double current_z_level = (aProp.Value(1)).Z();
		//Now project the points to the surface. As we might have many faces just project to every face and take the min distance
		for (int i=1;i<=numberofpoints;++i)
		{
			TopExp_Explorer ExShape;
			ExShape.Init(m_Shape,TopAbs_FACE);
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
			}
			gp_Pnt projectedPoint,OffsetPoint;
			gp_Vec Uvec,Vvec,normalVec,projPointVec;
			nearest_surface->D1(Umin,Vmin,projectedPoint,Uvec,Vvec);
			normalVec = Uvec;
			normalVec.Cross(Vvec);
			normalVec.Normalize(); //Jetzt ist die Normale berechnet und auch normalisiert
			//Jetzt die Normale auf die Radiuslänge verlängern
			normalVec.Multiply(10.0);
			//Jetzt die Z-Komponente auf 0 setzen
			normalVec.SetZ(0.0);
			//Eine Korrektur vom z-Level machen weil er durch numerische Fehler nicht exact auf dem Z-Level liegt wo wir ihn haben wollen. Die Normale wird aber trotzdem vom (durch numerische Fehler) berechneten Z-Wert genommen
			projectedPoint.SetZ(current_z_level);
			projPointVec.SetXYZ(projectedPoint.XYZ());
			OffsetPoint.SetXYZ((projPointVec + normalVec).XYZ());
			OffsetPoint.SetZ(projectedPoint.Z()+10);//Den Radius noch dazu addieren
			finalOffsetPoints->SetValue(i,OffsetPoint); //Aktuellen OffsetPoint setzen
			build.addSinglePoint(OffsetPoint.X(),OffsetPoint.Y(),OffsetPoint.Z(),2 ,0,0,0);
			build.addSinglePoint(projectedPoint.X(),projectedPoint.Y(),projectedPoint.Z());
			outfile << projectedPoint.X() <<","<<projectedPoint.Y()<<","<<projectedPoint.Z()<<std::endl;
		}
	
		GeomAPI_Interpolate aNoPeriodInterpolate(finalOffsetPoints, Standard_False, Precision::Confusion());
		aNoPeriodInterpolate.Perform();
		
		Handle_Geom_BSplineCurve aCurve(aNoPeriodInterpolate.Curve());
		// check results
		if (!aNoPeriodInterpolate.IsDone()) return false;
		m_all_offset_cuts_high.push_back(aCurve);
		
	}
	build.saveToFile("c:/test.iv");
	outfile.close();
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
bool cutting_tools::cutShape()
{

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
		minlevel=minlevel+0.1;//kleine Korrektur weil die Bounding Box nicht stimmt
        int cutnumber = fabs((maxlevel-minlevel)/m_pitch);//Cast um die Nachkommastellen wegzuschneiden
        m_pitch = fabs(maxlevel-minlevel)/cutnumber;//m_pitch leicht korrigieren um wirklich auf die letzte Ebene zu kommen

		//Aktuell wird die letzte Ebene bei selbst approxmierten Flächen nicht als Bahnkurve betrachtet
		//Auch die erste Ebene fällt komplett weg, da unwichtig. Lediglich für die untere Maschine ist die Bahn sinnvoll falls überhaupt noch flache Bereiche vorhanden sind
        for (int i=1;i<cutnumber;++i) 
        {
            //Jetzt schneiden (die oberste Ebene auslassen)
            double z_level = maxlevel-(i*m_pitch);
            gp_Pnt aPlanePnt(0,0,z_level);
            gp_Dir aPlaneDir(0,0,1);
            Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
            BRepBuilderAPI_MakeFace Face(aPlane);
            BRepAlgo_Section mkCut(m_Shape, Face.Face(),Standard_False);
            mkCut.Approximation (Standard_True);
            mkCut.Build();
			if(mkCut.IsDone())
			{
				m_zl_wire_combination.first = z_level;
				m_zl_wire_combination.second = ordercutShape(mkCut.Shape());
				m_all_cuts.push_back(m_zl_wire_combination);//Geordnete Edges in den All_Cuts-Vector stecken
			}
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
				Handle_Geom_Plane aPlane = new Geom_Plane(aPlanePnt, aPlaneDir);
				BRepBuilderAPI_MakeFace Face(aPlane);
				BRepAlgo_Section mkCut(m_Shape, Face.Face(),Standard_False);
				mkCut.Approximation (Standard_True);
				mkCut.Build();
				m_zl_wire_combination.first = z_level;
				m_zl_wire_combination.second = ordercutShape(mkCut.Shape());
				
				m_all_cuts.push_back(m_zl_wire_combination);//Geordnete Edges in den All_Cuts-Vector stecken
			}
		}
	}
}


double cutting_tools::GetWireLength(TopoDS_Wire &aWire)
{
	GProp_GProps lProps;
	BRepGProp::LinearProperties(aWire,lProps);
	double length = lProps.Mass();
	return length;
}
                