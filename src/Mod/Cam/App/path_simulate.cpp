#include "PreCompiled.h"
#include "path_simulate.h"
#include <GeomAdaptor_Curve.hxx>
#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <App/Document.h>


// Things from the part module
#include <Mod/Part/App/TopologyPy.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/TopoShapePy.h>

// Things from the Mesh module
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/MeshPy.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshAlgos.h>
#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/App/Core/Evaluation.h>


# include <BRepOffsetAPI_MakeOffsetShape.hxx>
# include <BRepAlgoAPI_Cut.hxx>
#include  <BRepAlgoAPI_Section.hxx>
# include <GeomAPI_IntSS.hxx>

#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepOffset.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepAlgo_Section.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Base/Builder3d.h>

/* Konstruktor mit einer Bahnfolge (master tool) als Input */
path_simulate::path_simulate(std::vector<Handle_Geom_BSplineCurve>& BSplineTop, double a_max, double v_max)
:m_BSplineTop(BSplineTop),m_amax(0.85*a_max),m_vmax(0.85*v_max),m_a1(0.85*a_max),m_v1(0.85*v_max),m_t0(0.0),m_step(1e-3),m_t(0.0),m_count(1),m_clip(90000)
{
	m_single = true;
	
	m_it1 = m_BSplineTop.begin();
    m_NumPaths = m_BSplineTop.size();

	gp_Pnt p;
	gp_Pnt q(0,0,1); /* startpunkt master tool */

	(*m_it1)->D0((*m_it1)->FirstParameter(),p); /* startpunkt auf der Kurve */

	q.SetZ(p.Z() + 2);  /* initial-z-level 2mm über dem ersten Kurvenstartpunkt */     

	m_StartPnts1.push_back(q);
	m_StartPnts1.push_back(p);

	m_StartParam.push_back((*m_it1)->FirstParameter());
	m_StartParam.push_back(0.0);

	//EstimateMaxAcceleration();
	
	/*vorläufige startparameter*/
	/*master tool*/
	m_c1.push_back(m_a1/2);
	m_c1.push_back(PI*m_a1/m_v1);

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1;
}

/* Konstruktor mit zwei Bahnfolgen (master tool & supporting die) als Input */
path_simulate::path_simulate(std::vector<Handle_Geom_BSplineCurve> &BSplineTop, std::vector<Handle_Geom_BSplineCurve> &BSplineBottom, 
							 double a, double v)
:m_BSplineTop(BSplineTop),m_BSplineBottom(BSplineBottom),m_amax(0.85*a),m_vmax(0.85*v),m_a1(0.85*a),m_a2(0.85*a),m_v1(0.85*v),m_v2(0.85*v),m_t0(0.0),m_step(1e-3),m_t(0.0),m_count(1),m_clip(90000),m_SingleTop(false),m_SingleBot(false)
{
	m_single=false;

	if(m_BSplineTop.size() == 1 && m_BSplineBottom.size() > 1)
	{
		m_SingleTop = true;

		for(m_it2 = m_BSplineBottom.begin();m_it2 < m_BSplineBottom.end()-1; ++m_it2)
			m_BSplineTop.push_back(*m_BSplineTop.begin());
	}


    if(m_BSplineBottom.size() == 1 && m_BSplineTop.size() > 1)
	{
		m_SingleBot = true;

		for(m_it1 = m_BSplineTop.begin();m_it1 < m_BSplineTop.end()-1; ++m_it1)
			m_BSplineBottom.push_back(*m_BSplineBottom.begin());

		m_it2 = m_BSplineBottom.begin();
		m_dbound = sqrt( (0.8*(*m_it2)->LastParameter()/PI) - 0.16);
	}

	m_it1 = m_BSplineTop.begin();
	m_it2 = m_BSplineBottom.begin();


	m_NumPaths = BSplineTop.size();  /* egal ob Top oder Bottom, die größen stimmen überein */

	gp_Pnt p;
	gp_Pnt q(0,0,1); /* startpunkt master tool */

	(*m_it1)->D0((*m_it1)->FirstParameter(),p); /* startpunkt auf der Kurve */

	q.SetZ(p.Z() + 2);  /* initial-z-level 2mm über dem ersten Kurvenstartpunkt */

	m_StartPnts1.push_back(q);   
    m_StartPnts1.push_back(p);   

	m_StartParam.push_back((*m_it1)->FirstParameter());
	m_StartParam.push_back(0.0);

	m_StartPnts2.push_back(Projection_pnt2curve(p,*m_it2, m_StartParam[1])); /* startpunkt auf der Kurve (supporting die)*/
    m_StartPnts2.push_back(m_StartPnts2[0]);

	m_toolrad = 10.0;
	m_stress = 1.0;

	q.SetZ(p.Z() - (m_toolrad + m_stress));
	m_StartPnts2[0] = q;
	
    //EstimateMaxAcceleration();
	
	/*vorläufige startparameter*/
	/*master tool*/
	m_c1.push_back(m_a1/2);
	m_c1.push_back(PI*m_a1/m_v1);

	m_c2 = m_c1;

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1;
}

path_simulate::~path_simulate()
{
}

gp_Pnt path_simulate::Projection_pnt2curve(gp_Pnt &pnt, Handle_Geom_BSplineCurve &curve, double &aNearestParam)
{
  // projection of a point onto a curve
  GeomAPI_ProjectPointOnCurve aPPC(pnt,curve);

  // evaluate solutions
  Standard_Integer aNumSolutions = aPPC.NbPoints();
  TColgp_Array1OfPnt aPoints(1, Max(aNumSolutions, 1));
  Standard_Real aDistance;
  gp_Pnt aNearestPoint;

  if (aNumSolutions > 0)
  {
    for(Standard_Integer i = 1; i <= aNumSolutions; i++)
      aPoints(i) = aPPC.Point(i);

    // The nearest solution
    aNearestParam = aPPC.LowerDistanceParameter();
    aNearestPoint = aPPC.NearestPoint();
    aDistance = aPPC.LowerDistance();

    return aNearestPoint;
  }
  else
	throw Base::Exception("no projection computed");

}



double path_simulate::GetLength(GeomAdaptor_Curve& curve, const Standard_Real startParameter,const Standard_Real endParameter)
{
    Standard_Real firstParameter = curve.FirstParameter();
    Standard_Real lastParameter  = curve.LastParameter();


    Standard_Real sParam = Min(startParameter,endParameter);
    Standard_Real eParam = Max(startParameter,endParameter);

    //Standard_Real length = 0.0;

    if ( eParam > lastParameter )
    {
        //get the first part of the lenght
        Standard_Real l1 = GetLength(curve,firstParameter,eParam-lastParameter);
        Standard_Real l2 = GetLength(curve,sParam,lastParameter);
        return l1 + l2;
    }
    if ( sParam < firstParameter )
    {
        Standard_Real l1 = GetLength(curve,lastParameter-fabs(sParam),lastParameter);
        Standard_Real l2 = GetLength(curve,firstParameter,eParam);
        return l1 + l2;
    }

    
    return GCPnts_AbscissaPoint::Length(curve,sParam,eParam);   /* genauigkeitssteuerung über parameter TOL nach eParam */
}


double path_simulate::FindParamAt(GeomAdaptor_Curve& curve, double dist, double startParam)
{
	double foundParameter;

    //get the first and last parameters of the curve
    Standard_Real firstParam = curve.FirstParameter();
    Standard_Real lastParam  = curve.LastParameter();
	
    Standard_Real period = lastParam - firstParam;

    Standard_Real start = startParam;

    while ( start < firstParam )
    {
        start = period + start;
    }

    while ( start > lastParam )
    {
        start = start - period;
    }

    Standard_Real direction = 1;
    if ( dist < 0 )
        direction = -1;

    //compute the parameter of the next point
    GCPnts_AbscissaPoint absc(curve, dist, start);
    if ( absc.IsDone() )
    {
        //the parameter is computed
        double param = absc.Parameter();

        if ( param > lastParam )
        {
            //the parameter is beyond the boundary, recenter it

            //compute the distance between the startParam and the end of the curve
            //Standard_Real distStartLast = GCPnts_AbscissaPoint::Length(curve,start,lastParam);
            Standard_Real distStartLast = GetLength(curve,start,lastParam);
			

            //place the point at the remaining distance from the first parameter
            return FindParamAt(curve, direction * (fabs(dist) - fabs(distStartLast)), firstParam);
        }
        else if ( param < firstParam )
        {
            //the parameter is beyond the boundary, recenter it

            //compute the distance between the startParam and the end of the curve
            //Standard_Real distFirstStart = GCPnts_AbscissaPoint::Length(curve,firstParam,startParam);
            Standard_Real distFirstStart = GetLength(curve,firstParam,lastParam); 

            //place the point at the remaining distance from the first parameter
            return FindParamAt(curve, direction * (fabs(dist) - fabs(distFirstStart)), lastParam);
        }

        Standard_Real distance;
        if ( direction > 0 )
        {
            distance = GetLength(curve,start, param);
        }
        else
        {
            distance = direction * GetLength(curve,param, start);
        }
        if ( fabs(distance - dist) > 1e-4 )
        {
            return FindParamAt(curve, dist - distance, param);
        }
        foundParameter = param;
        return foundParameter;
    }

    return foundParameter;
}

std::vector<double> path_simulate::GetAcceleration(double t)
{
	std::vector<double> a(2); /* wird gleich mit 0 vorinitialisiert */

	if ((t>=m_t0) && (t<=m_t1))
	{
		if(t==m_t0)
		{
			a[0] = 0.0;

			if(m_single == false)
				a[1] = 0.0;
		}
		else
		{
			a[0] = m_c1[0]*(cos(m_c1[1]*(t-m_t0) - PI) + 1);
			
			if(m_single == false)		
				a[1] = m_c2[0]*(cos(m_c2[1]*(t-m_t0) - PI) + 1);
		}

		
	}
	else if ((t>m_t1)  && (t<=m_t2));  /* hier passiert nichts */
	else if ((t>m_t2) && (t<=m_T))
	{
		a[0] = -m_c1[0]*(cos(m_c1[1]*(t-m_t2) - PI) + 1);
		
		if(m_single == false)		
			a[1] = -m_c2[0]*(cos(m_c2[1]*(t-m_t2) - PI) + 1);
	}
	else
		throw Base::Exception("time input not inside [t0,T]");
	
	return a;
}

std::vector<double> path_simulate::GetVelocity(double t)
{
	std::vector<double> v(2);

	if (t>=m_t0 && t<=m_t1)
	{
		if(t==m_t0)
		{
			v[0] = 0.0;

			if(m_single == false)
				v[1] = 0.0;
		}
		else
		{
			v[0] = m_c1[0]*(sin(m_c1[1]*(t-m_t0) - PI)/m_c1[1] + (t-m_t0));
			
			if(m_single == false)		
				v[1] = m_c2[0]*(sin(m_c2[1]*(t-m_t0) - PI)/m_c2[1] + (t-m_t0));
		}

		
	}
	else if (t>m_t1  && t<=m_t2)
	{
		v[0] = m_v1;

		if(m_single == false)
				v[1] = m_v2;
	}
	else if (t>m_t2 && t<=m_T)
	{
		v[0] = m_v1 - m_c1[0]*(sin(m_c1[1]*(t-m_t2) - PI)/m_c1[1] + (t-m_t2));

		if(m_single == false)		
			v[1] = m_v2 - m_c2[0]*(sin(m_c2[1]*(t-m_t2) - PI)/m_c2[1] + (t-m_t2));
	}
	else
		throw Base::Exception("time input not inside [t0, T]");

	return v;
}

std::vector<double> path_simulate::GetDistance(double t)
{
	std::vector<double> d(2);

	if (t>=m_t0 && t<=m_t1)
	{
		if(t==m_t0)
		{
			d[0] = 0.0;

			if(m_single == false)
				d[1] = 0.0;
		}
		else
		{
			d[0] = -(m_c1[0]/pow(m_c1[1],2.0))*cos(m_c1[1]*(t-m_t0) - PI) + m_c1[0]*pow((t-m_t0),2.0)/2 - m_c1[0]/(m_c1[1]*m_c1[1]);

			if(m_single == false)
				d[1] = -(m_c2[0]/pow(m_c2[1],2.0))*cos(m_c2[1]*(t-m_t0) - PI) + m_c2[0]*pow((t-m_t0),2.0)/2 - m_c2[0]/(m_c2[1]*m_c2[1]);
		}

	    
	}
	else if (t>m_t1  && t<=m_t2)
	{
		d[0] = m_c1[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(t-m_t1);

		if(m_single == false)		
			d[1] = m_c2[0]*pow((m_t1-m_t0),2.0)/2 + m_v2*(t-m_t1);
	}
	else if (t>m_t2 && t<=m_T)
	{
		d[0] = m_c1[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(m_t2-m_t1) + (m_c1[0]/pow(m_c1[1],2.0))*cos(m_c1[1]*(t-m_t2) - PI) - m_c1[0]*pow((t-m_t2),2.0)/2 + (m_c1[0]/pow(m_c1[1],2.0)) + m_v1*(t-m_t2);

		if(m_single == false)		
			d[1] = m_c2[0]*pow((m_t1-m_t0),2.0)/2 + m_v2*(m_t2-m_t1) + (m_c2[0]/pow(m_c2[1],2.0))*cos(m_c2[1]*(t-m_t2) - PI) - m_c2[0]*pow((t-m_t2),2.0)/2 + m_c2[0]/pow(m_c2[1],2.0) + m_v2*(t-m_t2);		
	}
	else
	{
		cout << m_t0 << "," << t << "," << m_T << endl;
		throw Base::Exception("time input not inside [t0,T]");
	}
	
	return d;
}

/*
double path_simulate::GetWireLength(TopoDS_Wire &aWire)
{
	GProp_GProps lProps;
	BRepGProp::LinearProperties(aWire,lProps);
	double length = lProps.Mass();
	return length;
}
*/


/*Parameterberechnung für die Geschwindigkeitsfunktion für 2 Inputgrößen*/
bool path_simulate::ParameterCalculation(double S1, double S2)
{	
	double S,v,a;
	bool b = false;
	std::vector<double> m_c;

	if(S2 > S1)
	{
		b= true;
		S = S1;
		S1 = S2;
		S2 = S;
	}


	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1 - m_t0;

	if((GetDistance(m_t1))[0] <= S1/2)
	{/*Fall 1:  distance(m_t1) =< S/2*/
		m_t2 = m_t1 + (S1 - 2*(GetDistance(m_t1))[0])/m_v1;
		m_v2    = (S2 - S2*2*(GetDistance(m_t1))[0]/S1)/(m_t2-m_t1);
		m_a2 = (2*m_v2)/(m_t1-m_t0);
	}
	else								                            /*Fall 2*/				
	{
        m_v1    = sqrt(m_a1*S1/2);                                  /*m_v1 muss neu berechnet werden*/
		m_c1[1] = PI*m_a1/m_v1;
        m_t1    = m_t0 + 2*m_v1/m_a1;
		m_t2    = m_t1;

		m_a2 = 2*S2/pow(m_t1-m_t0,2);
		m_v2 = (m_t1-m_t0)*m_a2/2;
	}
	
	m_T = m_t1 + m_t2 - m_t0;

	/*increm. die*/
	m_c2[0] = m_a2/2;
	
	if(m_v2!=0)
		m_c2[1] = PI*m_a2/m_v2;
	else
		m_c2[1] = 0.0;

	if(b==true)
	{
		m_c = m_c1;
		m_c1 = m_c2;
		m_c2 = m_c;

		v = m_v1;
		m_v1 = m_v2;
		m_v2 = v;

		a = m_a1;
		m_a1 = m_a2;
		m_a2 = a;
	}


	return true;
}

/*Parameterberechnung für die Geschwindigkeitsfunktion für einen Input*/
bool path_simulate::ParameterCalculation(double S1)
{

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1 - m_t0;
    
    std::vector<double> tmp = GetDistance(m_t1);

	if(tmp[0] <= (S1/2))    
	{                          

		m_t2 = m_t1 + (S1 - 2*tmp[0])/m_v1;

	}
	else									                         /*Fall 2*/				
	{
        m_v1    = sqrt(m_a1*S1/2);                                   /*m_v1 muss neu berechnet werden*/
		m_c1[1] = PI*m_a1/m_v1;
        m_t1    = m_t0 + 2*m_v1/m_a1;
		m_t2    = m_t1;
	}
	
	m_T = m_t1 + m_t2 - m_t0;

	return true;
}
std::vector<std::vector<Base::Vector3d>> path_simulate::PointEvaluation(double T, 
																		unsigned int N, 
																		std::vector<double> startParam, 
																		std::vector<std::vector<Base::Vector3d>> &D1)
{
    double t = m_t0;
	double foundParameter;
	double firstParam,lastParam,period;
	std::vector<double> d;
	gp_Pnt tmp, p1,p2;
	gp_Vec dtmp1,v1,v2;
	Base::Vector3d tmp2,tmp4;

	GeomAdaptor_Curve curve1(*m_it1);
	
	firstParam = curve1.FirstParameter();
    lastParam  = curve1.LastParameter();
	period     = lastParam - firstParam;

	m_del_t =  (m_T- m_t0)/N;
	
	std::vector<std::vector<Base::Vector3d>> D0;
	std::vector<Base::Vector3d> tmp3,tmp5;

	for(unsigned int i=0; i<N; ++i)
	{
		d = GetDistance(t);

		//foundParameter = FindParamAt(curve1, d[0], startParam[0]);     /* d[0] - master tool, d[1] - supporting die */
		
		if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
			d[0] = -d[0];     /* führe Richtungsänderung durch */


		if( startParam[0] + d[0] > lastParam )
        {
     		curve1.D1(startParam[0] + d[0] - period, tmp, dtmp1);
		}
        else if( startParam[0] + d[0] < firstParam )
		{
			curve1.D1(startParam[0] + d[0] + period, tmp, dtmp1);
		}
		else
        {
			curve1.D1(startParam[0] + d[0], tmp, dtmp1);
		}
		
		dtmp1.Normalize();

		/* erste ableitung */
		tmp4.x = dtmp1.X();
		tmp4.y = dtmp1.Y();
		tmp4.z = dtmp1.Z();

		tmp5.push_back(tmp4);

		if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
			dtmp1 = -dtmp1;              /* dreht den bei richtungsänderung tangentenvektor um  */

		/* nullte Ableitung, entspricht den punkten */
		tmp2.x = tmp.X();
		tmp2.y = tmp.Y();
		tmp2.z = tmp.Z();

		tmp3.push_back(tmp2);		

		//if(m_single == false)
		//{
		//	GeomAdaptor_Curve curve2(*m_it2);

		//	foundParameter = FindParamAt(curve2, d[1], startParam[1]);   /*increm. die*/
		//	curve2.D1(foundParameter,tmp,dtmp1);

		//	dtmp1.Normalize();

	 //       tmp4.x = dtmp1.X();
		//    tmp4.y = dtmp1.Y();
		//    tmp4.z = dtmp1.Z();

		//    tmp5.push_back(tmp4);

		//	tmp2.x = tmp.X();
		//    tmp2.y = tmp.Y();
		//    tmp2.z = tmp.Z();

		//    tmp3.push_back(tmp2); 
		//}

		D0.push_back(tmp3);
		D1.push_back(tmp5);

		tmp3.clear();
		tmp5.clear();

		/*
		dtmp1 = (dtmp2*dtmp1.SquareMagnitude() - dtmp1*(dtmp1.Dot(dtmp2)))/pow(dtmp1.Magnitude(),3);

		tmp2.x = dtmp1.X();
	    tmp2.y = dtmp1.Y();
		tmp2.z = dtmp1.Z();

		tmp3.push_back(tmp2);
		D2.push_back(tmp3);
        tmp3.clear();
		*/
		
		t += m_del_t;
	}

	return D0;
}

std::vector<std::vector<Base::Vector3d>> path_simulate::PointEvaluation(double T, 
																		unsigned int N, 
																		std::vector<double> startParam, 
																		std::vector<std::vector<Base::Vector3d>> &D1,
																		std::vector<std::vector<Base::Vector3d>> &D2)
{
    double t = m_t0;
	double foundParameter;
	double firstParam,lastParam,period;
	std::vector<double> d;
	gp_Pnt tmp, p1,p2;
	gp_Vec dtmp1,dtmp2,v1,v2;
	Base::Vector3d tmp2,tmp4;

	GeomAdaptor_Curve curve1(*m_it1);
	
	firstParam = curve1.FirstParameter();
    lastParam  = curve1.LastParameter();
	period     = lastParam - firstParam;

	m_del_t =  (m_T- m_t0)/N;
	
	std::vector<std::vector<Base::Vector3d>> D0;
	std::vector<Base::Vector3d> tmp3,tmp5;

	for(unsigned int i=0; i<N; ++i)
	{
		d = GetDistance(t);

		//foundParameter = FindParamAt(curve1, d[0], startParam[0]);     /* d[0] - master tool, d[1] - supporting die */
		
		if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
			d[0] = -d[0];     /* führe Richtungsänderung durch */


		if( startParam[0] + d[0] > lastParam )
        {
     		curve1.D2(startParam[0] + d[0] - period, tmp, dtmp1, dtmp2);
		}
        else if( startParam[0] + d[0] < firstParam )
		{
			curve1.D2(startParam[0] + d[0] + period, tmp, dtmp1, dtmp2);
		}
		else
        {
			curve1.D2(startParam[0] + d[0], tmp, dtmp1, dtmp2);
		}
		
		dtmp1.Normalize();

		/* erste ableitung */
		tmp4.x = dtmp1.X();
		tmp4.y = dtmp1.Y();
		tmp4.z = dtmp1.Z();

		tmp5.push_back(tmp4);

		if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
			dtmp1 = -dtmp1;              /* dreht den bei richtungsänderung tangentenvektor um  */

		/* nullte Ableitung, entspricht den punkten */
		tmp2.x = tmp.X();
		tmp2.y = tmp.Y();
		tmp2.z = tmp.Z();

		tmp3.push_back(tmp2);		

		if(m_single == false)
		{
			GeomAdaptor_Curve curve2(*m_it2);

			foundParameter = FindParamAt(curve2, d[1], startParam[1]);   /*increm. die*/
			curve2.D1(foundParameter,tmp,dtmp1);

			dtmp1.Normalize();

			tmp2.x = tmp.X();
		    tmp2.y = tmp.Y();
		    tmp2.z = tmp.Z();

		    tmp3.push_back(tmp2); 
		}

		D0.push_back(tmp3);
		D1.push_back(tmp5);

		tmp3.clear();
		tmp5.clear();

		
		dtmp1 = (dtmp2/dtmp1.Magnitude() - dtmp1*(dtmp1.Dot(dtmp2)))/pow(dtmp1.Magnitude(),3);

		tmp2.x = dtmp1.X();
	    tmp2.y = dtmp1.Y();
		tmp2.z = dtmp1.Z();

		tmp3.push_back(tmp2);
		D2.push_back(tmp3);
        tmp3.clear();
		
		
		t += m_del_t;
	}

	return D0;
}

/* berechnet die diskrete Ableitung einer Punktefolge über symmetrische Differenzen */
std::vector<std::vector<Base::Vector3d>> path_simulate::Derivate(const std::vector<std::vector<Base::Vector3d>> &D0)
{
	std::vector<std::vector<Base::Vector3d>> D1 = D0;
	int N = D0.size();
	
	D1[0][0] = (D0[1][0]-D0[N-1][0])/(2*m_del_t);
    
	for (int i=1; i<N-1; ++i)
	{
        D1[i][0] = (D0[i+1][0]-D0[i-1][0])/(2*m_del_t);
	}
    
	D1[N-1][0] = (D0[0][0]-D0[N-2][0])/(2*m_del_t);
	
	/*if(m_single == false)
	{
		D1[0][1] = (D0[1][1]-D0[N-1][1])/(2*m_del_t);
        
		for (int i=1; i<N-1; ++i)
		{
			D1[1][i] = (D0[i+1][1]-D0[i-1][1])/(2*m_del_t);
		}
		
		D1[N-1][1] = (D0[0][1]-D0[N-2][1])/(2*m_del_t);
	}*/

	return D1;
}

bool path_simulate::OutputPath(std::vector<std::vector<Base::Vector3d>> &D1, std::vector<std::vector<Base::Vector3d>> &D2)
{
	std::vector<Base::Vector3d> tmp2;
	Base::Vector3d tmp;
	double t = m_t0;
     
	for (unsigned int i=0; i<D1.size(); ++i)
	{
		m_Output_time.push_back(t);

		/* master tool path */
        tmp.x = D2[i][0].x * (GetVelocity(t))[0] + D1[i][0].x * (GetAcceleration(t))[0];
        tmp.y = D2[i][0].y * (GetVelocity(t))[0] + D1[i][0].y * (GetAcceleration(t))[0];
        tmp.z = D2[i][0].z * (GetVelocity(t))[0] + D1[i][0].z * (GetAcceleration(t))[0];

		tmp2.push_back(tmp);

		if(m_single == false)
		{
			/* supporting die */
			tmp.x = D2[i][1].x * (GetVelocity(t))[1] + D1[i][1].x * (GetAcceleration(t))[1];
			tmp.y = D2[i][1].y * (GetVelocity(t))[1] + D1[i][1].y * (GetAcceleration(t))[1];
			tmp.z = D2[i][1].z * (GetVelocity(t))[1] + D1[i][1].z * (GetAcceleration(t))[1];

			tmp2.push_back(tmp);
		}

		m_Output.push_back(tmp2);
		tmp2.clear();
		
		t += m_del_t;
	}

	tmp.x = 0.0;
	tmp.y = 0.0;
	tmp.z = 0.0;
	
	tmp2.push_back(tmp);
	m_Output.push_back(tmp2);
	m_Output_time.push_back(m_T);
	
	return true;          
}


bool path_simulate::UpdateParam()
{
	m_v1 = m_vmax;
	m_a1 = m_amax;

	m_t0 = m_T; /* endzeit des letzten durchlaufs wird zur neuen startzeit */

	if(m_c1.empty() == true)
		m_c1.resize(2);
	  
	m_c1[0] = m_a1/2;
	m_c1[1] = PI*m_a1/m_v1;

	return true;
}


/*computes the startpoints lying on the curves*/
/*
bool path_simulate::CalculateConnection()
{
	gp_Vec vec1;
	gp_Vec vec2;

	(*m_it1)->D0((*m_it1)->FirstParameter(), p);
	
	m_StartPnts1.push_back(p);
	m_StartPnts2.push_back(Projection_pnt2curve(p,*m_it2));
	
	m_it2 = m_BSplineBottom.begin()+1;
	
	for(m_it1 = m_BSplineTop.begin()+1; m_it1<m_BSplineTop.end(); ++m_it1)
	{
		p = Projection_pnt2curve(p,*m_it1);
		m_StartPnts1.push_back(p);
     	m_StartPnts2.push_back(Projection_pnt2curve(p,*m_it2));

		++m_it2;
	}

	for(int i=0; i<m_StartPnts1.size(); ++i)
	{
		gp_Vec vec1(m_StartPnts1[i], m_StartPnts1[i+1]);
		m_vecs1.push_back(vec1);
		delete vec1;

		gp_Vec vec2(m_StartPnts2[i], m_StartPnts2[i+1]);
		m_vecs2.push_back(vec2);
		delete vec2;
	}

}
*/

bool path_simulate::ConnectPaths_xy(ofstream &anOutputFile, int &c, bool brob)
{
	int N;
    double t = m_t0;
	std::vector<double> d;
	
	Base::Vector3d tmp;
	std::vector<Base::Vector3d> tmp2;

	if(m_it1 != m_BSplineTop.begin() || m_it2 != m_BSplineBottom.begin())
	{
		m_StartPnts1[0] = m_StartPnts1[1];

		if(m_SingleTop == false && m_SingleBot == false)
			m_StartPnts1[1] = Projection_pnt2curve(m_StartPnts1[0],*m_it1, m_StartParam[0]);
		else if(m_SingleBot == true)
			m_StartPnts1[1] = Projection_pnt2curve(m_StartPnts2[1],*m_it1, m_StartParam[0]);

		if(m_single == false)
		{
			m_StartPnts2[0] = m_StartPnts2[1];

			if(m_SingleBot == false && m_SingleTop == false)
				m_StartPnts2[1] = Projection_pnt2curve(m_StartPnts1[1],*m_it2, m_StartParam[1]);
			else if(m_SingleTop == true)
				m_StartPnts2[1] = Projection_pnt2curve(m_StartPnts1[1],*m_it2, m_StartParam[1]);
		}
	}

	if(m_single == false)
	{
		gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);
		gp_Vec vec_2(m_StartPnts2[0], m_StartPnts2[1]);

		gp_Vec2d vec_11, vec_21;

		vec_11.SetX(vec_1.X());
		vec_11.SetY(vec_1.Y());

		vec_21.SetX(vec_2.X());
		vec_21.SetY(vec_2.Y());

		if(brob == false)
		{

			ParameterCalculation(vec_11.Magnitude(), vec_21.Magnitude());

			if(vec_11.Magnitude() != 0)
				vec_11.Normalize();
			
			if(vec_21.Magnitude() != 0)
				vec_21.Normalize();

			N = (m_T - m_t0)/m_step;  /* anzahl der zu erzeugenden Outputwerte */

			m_del_t = (m_T - m_t0)/N;

			Base::Vector3f pit1,pit2;

			for (int i=0; i<N; ++i)
			{
				m_Output_time.push_back(t);

				d = GetDistance(t);

				tmp.x = vec_11.X()*(GetVelocity(t))[0];
				tmp.y = vec_11.Y()*(GetVelocity(t))[0];
				tmp.z = 0.0;

				tmp2.push_back(tmp);

				pit1.x = m_StartPnts1[0].X() + d[0]*vec_11.X();
				pit1.y = m_StartPnts1[0].Y() + d[0]*vec_11.Y();
				pit1.z = m_StartPnts1[0].Z();

				tmp.x = vec_21.X()*(GetVelocity(t))[1];
				tmp.y = vec_21.Y()*(GetVelocity(t))[1];     // optimierungsbedarf, velocity func wird hier 2 mal aufgerufen
				tmp.z = 0.0;

				tmp2.push_back(tmp);

				pit2.x = m_StartPnts1[1].X() + d[1]*vec_21.X();
				pit2.y = m_StartPnts1[1].Y() + d[1]*vec_21.Y();
				pit2.z = m_StartPnts1[1].Z();

				//m_log3d.addSingleArrow(pit1,pit2);

				m_Output.push_back(tmp2);
				tmp2.clear();

				t += m_del_t;
			}

			m_Output_time.push_back(m_T);
			
			tmp.x = 0.0;
			tmp.y = 0.0;
			tmp.z = 0.0;

			tmp2.push_back(tmp);
			tmp2.push_back(tmp);

			m_Output.push_back(tmp2);	
		}
		else
		{

			N = vec_11.Magnitude();  /* anzahl der zu erzeugenden Outputwerte */

			for (int i=0; i<N; ++i)
			{				
				tmp.x = m_StartPnts1[0].X() + (i*vec_11.X())/N;
				tmp.y = m_StartPnts1[0].Y() + (i*vec_11.Y())/N;
				tmp.z = m_StartPnts1[0].Z();

				m_Output_robo1.push_back(tmp);
			}

			for (int i=0; i<N; ++i)
			{				
				tmp.x = m_StartPnts2[0].X() + (i*vec_21.X())/N;
				tmp.y = m_StartPnts2[0].Y() + (i*vec_21.Y())/N;
				tmp.z = m_StartPnts2[0].Z();

				m_Output_robo2.push_back(tmp);
			}
		}				
	}
	else
	{
		gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);

		gp_Vec2d vec_11,vec_12;

		vec_11.SetX(vec_1.X());
		vec_11.SetY(vec_1.Y());

		if(brob == false)
		{

			ParameterCalculation(vec_11.Magnitude());

			vec_11.Normalize();

			N = (m_T - m_t0)/m_step;        
			m_del_t = (m_T - m_t0)/N;

			for(int i=0; i<N; ++i)
			{
				m_Output_time.push_back(t);
				d = GetDistance(t);

				tmp.x = vec_11.X()*(GetVelocity(t))[0];
				tmp.y = vec_11.Y()*(GetVelocity(t))[0];
				tmp.z = 0.0;

				tmp2.push_back(tmp);

				m_Output.push_back(tmp2);
				tmp2.clear();

				t += m_del_t;
			}
			
			m_Output_time.push_back(m_T);

			tmp.x = 0.0;
			tmp.y = 0.0;
			tmp.z = 0.0;
			
			tmp2.push_back(tmp);
			m_Output.push_back(tmp2);
			tmp2.clear();
		}
		else
		{
			N = vec_11.Magnitude();  /* anzahl der zu erzeugenden Outputwerte */
			
			for (int i=0; i<N; ++i)
			{				
				tmp.x = m_StartPnts1[0].X() + (i*vec_11.X())/N;
				tmp.y = m_StartPnts1[0].Y() + (i*vec_11.Y())/N;
				tmp.z = m_StartPnts1[0].Z();

				m_Output_robo1.push_back(tmp);
			}		
		}
	}
	return true;
}

bool path_simulate::ConnectPaths_z(ofstream &anOutputFile, int &c, bool brob)
{
	int N;
    double t = m_t0;
	
	Base::Vector3d tmp;
	std::vector<double> d;
	std::vector<Base::Vector3d> tmp2;
	Base::Vector3f pit1,pit2;

	if(m_single == false)
	{
		gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);
		gp_Vec vec_2(m_StartPnts2[0], m_StartPnts2[1]);

		gp_Vec2d vec_11,vec_12;

		vec_11.SetX(0.0);
		vec_11.SetY(vec_1.Z());

		vec_12.SetX(0.0);
		vec_12.SetY(vec_2.Z());

		if(brob == false)
		{
			ParameterCalculation(vec_11.Magnitude(), vec_12.Magnitude());

			N = (m_T - m_t0)/m_step;
			m_del_t = (m_T - m_t0)/N;

			//vec_11.Normalize();
			//vec_12.Normalize();

			m_del_t = (m_T - m_t0)/N;

			for (int i=0; i<N; ++i)
			{
				m_Output_time.push_back(t);
				
				d = GetDistance(t);

				tmp.x = 0.0;
				tmp.y = 0.0;
				tmp.z = -(GetVelocity(t))[0];

				tmp2.push_back(tmp);

				/*pit1.x = m_StartPnts1[1].X();
				pit1.y = m_StartPnts1[1].Y();
				pit1.z = m_StartPnts1[0].Z() + d[0]*vec_11.Y();*/

				tmp.x = 0.0;
				tmp.y = 0.0;
				
				if(vec_12.Y() > 0)
    				tmp.z = (GetVelocity(t))[1];
				else if(vec_12.Y() < 0)
					tmp.z = -(GetVelocity(t))[1];
				else
					tmp.z = 0.0;

				tmp2.push_back(tmp);

				/*pit2.x = m_StartPnts2[1].X();
				pit2.y = m_StartPnts2[1].Y();
				pit2.z = m_StartPnts2[0].Z() + d[0]*vec_12.Y();*/

				//m_log3d.addSingleArrow(pit1,pit2);

				m_Output.push_back(tmp2);
				tmp2.clear();

				t += m_del_t;
			}
			
			m_Output_time.push_back(m_T);
			
			tmp.x = 0.0;
			tmp.y = 0.0;
			tmp.z = 0.0;

			tmp2.push_back(tmp);
			tmp2.push_back(tmp);

			m_Output.push_back(tmp2);
		}
		else
		{
			N = vec_11.Magnitude();  /* anzahl der zu erzeugenden Outputwerte */

			for (int i=0; i<N; ++i)
			{				
				tmp.x = m_StartPnts1[1].X();
				tmp.y = m_StartPnts1[1].Y();
				tmp.z = m_StartPnts1[0].Z() + (i*vec_11.Y())/N;

				m_Output_robo1.push_back(tmp);
			}

			for (int i=0; i<N; ++i)
			{				
				tmp.x = m_StartPnts2[1].X();
				tmp.y = m_StartPnts2[1].Y();
				tmp.z = m_StartPnts2[0].Z() + (i*vec_12.Y())/N;

				m_Output_robo2.push_back(tmp);
			}
		}
	}
	else
	{
		gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);

		gp_Vec2d vec_12;

		vec_12.SetX(0.0);
		vec_12.SetY(vec_1.Z());

		if(brob == false)
		{
			ParameterCalculation(vec_12.Magnitude());

			N = (m_T - m_t0)/m_step;
			m_del_t = (m_T - m_t0)/N;

			for(int i=0; i<N; ++i)
			{
				m_Output_time.push_back(t);

				tmp.x = 0.0;
				tmp.y = 0.0;
				tmp.z = -(GetVelocity(t))[0];

				tmp2.push_back(tmp);

				m_Output.push_back(tmp2);
				tmp2.clear();

				t += m_del_t;
			}

			t = m_T;

			m_Output_time.push_back(t);

			tmp.x = 0.0;
			tmp.y = 0.0;
			tmp.z = 0.0;
			
			tmp2.push_back(tmp);
			m_Output.push_back(tmp2);
			tmp2.clear();
		}
		else
		{
			N = vec_12.Magnitude();  /* anzahl der zu erzeugenden Outputwerte */
			
			for (int i=0; i<N; ++i)
			{				
				tmp.x = m_StartPnts1[0].X() + (i*vec_12.X())/N;
				tmp.y = m_StartPnts1[0].Y() + (i*vec_12.Y())/N;
				tmp.z = m_StartPnts1[0].Z();

				m_Output_robo1.push_back(tmp);
			}		
		}
	}
	return true;
}



bool path_simulate::EstimateMaxAcceleration()
{
	GeomAdaptor_Curve anAdaptorCurve1;

	anAdaptorCurve1.Load(*m_it1);
    double w1 = GetLength(anAdaptorCurve1, anAdaptorCurve1.FirstParameter(), anAdaptorCurve1.LastParameter());


	std::vector< std::vector<Base::Vector3d> > D1;//(2,N);   /* 1.abl */
	std::vector< std::vector<Base::Vector3d> > D2;//(2,N);   /* 2.abl */

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1 - m_t0;
	
	if((GetDistance(m_t1))[0] <= w1/2)                               /*Fall 1:  distance(m_t1) =< S/2*/
		m_t2 = m_t1 + (w1 - 2*(GetDistance(m_t1))[0])/m_v1;
	else									                         /*Fall 2*/				
	{
        m_v1    = sqrt(m_a1*w1/2);                                   /*m_v1 muss neu berechnet werden*/
		m_c1[1] = PI*m_a1/m_v1;
        m_t1    = m_t0 + 2*m_v1/m_a1;
		m_t2    = m_t1;
	}
	
	m_T = m_t1 + m_t2 - m_t0;

	int N = (m_T - m_t0)/1e-3;

	if(N>10000)
		N = 10000;	

	PointEvaluation(m_T, N, m_StartParam, D1);
	m_del_t = (m_T - m_t0)/N;
	D2 = Derivate(D1);

	/* berechnung des maximums der 2.ableitung nach der zeit */
	double M  = pow(D2[0][0].x, 2) + pow(D2[0][0].y, 2) + pow(D2[0][0].z, 2);
	double m1 = (D2[0][0].x)   + (D2[0][0].y)   + (D2[0][0].z);
	double m2 = M;

	double c1 = M;
	double c2 = m1;
	double c3 = m2;

	for(int i=1; i<N; ++i)
	{
		if(M < (pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2)))
			M = pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2);

		c1 += pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2);

		if(m1 > ((D2[i][0].x) + (D2[i][0].y) + (D2[i][0].z)))
			m1 = (D2[i][0].x) + (D2[i][0].y) + (D2[i][0].z);

		c2 += (D2[i][0].x) + (D2[i][0].y) + (D2[i][0].z);

		if(m2 > (pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2)))
			m2 = pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2);

		c3 += pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2);
	}

	c1 /= N;
	c2 /= N;
	c3 /= N;

	if(pow(m_vmax,2)* c1 > pow(m_amax,2))
		m_v1 = m_amax/sqrt(c1);
	else
		m_v1 = m_vmax;

	m1 = ((D2[N/2][0].x) + (D2[N/2][0].y) + (D2[N/2][0].z));
	m2 = pow(D2[N/2][0].x, 2) + pow(D2[N/2][0].y, 2) + pow(D2[N/2][0].z, 2);

	m_a1 = (-m_v1*c2 + sqrt(pow(m_v1*c2,2) - 3*c3*pow(m_v1,2) + 12*pow(m_amax,2)))/6;

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1;

	m_a1 = (-m_v1*c2 + sqrt(pow(m_v1*c2,2) - 3*c3*pow(m_v1,2) + 12*pow(m_amax,2)))/6;
	if((GetDistance(m_t1))[0] <= w1/2)                               /*Fall 1:  distance(m_t1) =< S/2*/
		m_t2 = m_t1 + (w1 - 2*(GetDistance(m_t1))[0])/m_v1;
	else									                         /*Fall 2*/				
	{
        m_v1    = sqrt(m_a1*w1/2);                                   /*m_v1 muss neu berechnet werden*/
		m_c1[1] = PI*m_a1/m_v1;
        m_t1    = m_t0 + 2*m_v1/m_a1;
		m_t2    = m_t1;
	}
	
	m_T = m_t1 + m_t2 - m_t0;

	m_del_t = (m_T - m_t0)/N;
	D2 = Derivate(D1);

	 /* berechnung des maximums der 2.ableitung nach der zeit */
	 M  = pow(D2[0][0].x, 2) + pow(D2[0][0].y, 2) + pow(D2[0][0].z, 2);
	 m1 = (D2[0][0].x)   + (D2[0][0].y)   + (D2[0][0].z);
	 m2 = M;

	for(int i=1; i<N; ++i)
	{
		if(M < (pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2)))
			M = pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2);

		if(m1 > ((D2[i][0].x) + (D2[i][0].y) + (D2[i][0].z)))
			m1 = (D2[i][0].x) + (D2[i][0].y) + (D2[i][0].z);

		if(m2 > (pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2)))
			m2 = pow(D2[i][0].x, 2) + pow(D2[i][0].y, 2) + pow(D2[i][0].z, 2);
	}

	if(pow(m_vmax,2)* M > pow(m_amax,2))
		m_v1 = m_amax/sqrt(M);
	else
		m_v1 = m_vmax;

	m1 = ((D2[N/2][0].x) + (D2[N/2][0].y) + (D2[N/2][0].z));
	m2 = pow(D2[N/2][0].x, 2) + pow(D2[N/2][0].y, 2) + pow(D2[N/2][0].z, 2);

	m_a1 = (-m_v1*m1 + sqrt(pow(m_v1*m1,2) - 3*m2*pow(m_v1,2) + 12*pow(m_amax,2)))/6;

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1;

	m_c1[0] = (m_a1/2);
	m_c1[1] = (PI*m_a1/m_v1);
	
	return true;
}

bool path_simulate::MakeSinglePath(ofstream &anOutputFile, int &c, bool brob)
{
	GeomAdaptor_Curve anAdaptorCurve;
	GeomAdaptor_Curve anAdaptorCurve2;
	double firstParam1,lastParam1,period1,firstParam2,lastParam2,period2;
	std::vector<double> d;
	gp_Pnt tmp;
	gp_Vec dtmp1;
	Base::Vector3d tmp2;
	std::vector<Base::Vector3d> tmp3;
	bool b = false;
	Base::Vector3f pit1,pit2;

	anAdaptorCurve.Load(*m_it1);
	double w1 = anAdaptorCurve.LastParameter();//GetLength(anAdaptorCurve, anAdaptorCurve.FirstParameter(), anAdaptorCurve.LastParameter());
    double w2;

	firstParam1 = anAdaptorCurve.FirstParameter();
    lastParam1  = anAdaptorCurve.LastParameter();
	period1     = lastParam1 - firstParam1;

	if(m_single==false)
    {
		b = true;
	    anAdaptorCurve2.Load(*m_it2);
		w2 = GetLength(anAdaptorCurve2, anAdaptorCurve2.FirstParameter(), anAdaptorCurve2.LastParameter());

		ParameterCalculation(w1,w2);

		firstParam2 = anAdaptorCurve2.FirstParameter();
        lastParam2  = anAdaptorCurve2.LastParameter();
	    period2     = lastParam2 - firstParam2;
	}
	else
	{
		ParameterCalculation(w1);
	}
	
	int N = (m_T-m_t0)/m_step;
	m_del_t = (m_T-m_t0)/N;

	cout << "NumOfPoints: " << N << endl;

	if(N>=100000)
		N = 99999;

    std::vector< std::vector<Base::Vector3d> > D0;//(2,N);     /* 0.abl entspricht punkte auf kurve */
	std::vector< std::vector<Base::Vector3d> > D1;//(2,N);     /* 1.abl */
	std::vector< std::vector<Base::Vector3d> > D2;//(2,N);     /* 2.abl */

 

	//D0 = PointEvaluation(m_T, N, m_StartParam, D1);

	//std::vector<Base::Vector3d> tmp2;
	//Base::Vector3d tmp;
	
	double t = m_t0;
	
	if(m_single == true)
    {
		if(brob == false)
		{
			for(int i=0; i<N; ++i)
			{
				m_Output_time.push_back(t);
				d = GetDistance(t);
			  /*d = FindParamAt(anAdaptorCurve, d, m_StartParam[0]);*/
			
				if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
					d[0] = -d[0];   /* führe Richtungsänderung durch */
		      
			
				if( m_StartParam[0] + d[0] > lastParam1 )
				{
     				anAdaptorCurve.D1(m_StartParam[0] + d[0] - period1, tmp, dtmp1);
				}
				else if( m_StartParam[0] + d[0] < firstParam1 )
				{
					anAdaptorCurve.D1(m_StartParam[0] + d[0] + period1, tmp, dtmp1);
				}
				else
				{
					anAdaptorCurve.D1(m_StartParam[0] + d[0], tmp, dtmp1);
				}



				if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
					dtmp1 = -dtmp1;

				dtmp1.Normalize();
						
				tmp2.x = dtmp1.X()*(GetVelocity(t))[0];
				tmp2.y = dtmp1.Y()*(GetVelocity(t))[0];
				tmp2.z = dtmp1.Z()*(GetVelocity(t))[0];

				tmp3.push_back(tmp2);

				m_Output.push_back(tmp3);
				tmp3.clear();
				
				t += m_del_t;
			}

			t = m_T;

			m_Output_time.push_back(t);

			tmp2.x = 0.0;
			tmp2.y = 0.0;
			tmp2.z = 0.0;
			
			tmp3.push_back(tmp2);
			m_Output.push_back(tmp3);
			tmp3.clear();
		}
		else
		{

			N = w1;

			for(int i=0; i<N; ++i)
			{
				anAdaptorCurve.D0(firstParam1 + i*w1/N, tmp);

				tmp2.x = tmp.X();
				tmp2.y = tmp.Y();
				tmp2.z = tmp.Z();

				m_Output_robo1.push_back(tmp2);
			}
		}
	}
	else
	{
		if(brob == false)
		{
			for(int i=0; i<N; ++i)
			{
				m_Output_time.push_back(t);
				d = GetDistance(t);
			  
				/*d = FindParamAt(anAdaptorCurve, d, m_StartParam[0]);*/
			
				if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
				{
					d[0] = -d[0];   /* führe Richtungsänderung durch */
					d[1] = -d[1];
				}
			
				if( m_StartParam[0] + d[0] > lastParam1 )
				{
     				anAdaptorCurve.D1(m_StartParam[0] + d[0] - period1, tmp, dtmp1);
				}
				else if( m_StartParam[0] + d[0] < firstParam1 )
				{
					anAdaptorCurve.D1(m_StartParam[0] + d[0] + period1, tmp, dtmp1);
				}
				else
				{
					anAdaptorCurve.D1(m_StartParam[0] + d[0], tmp, dtmp1);
				}



				if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
					dtmp1 = -dtmp1;

				dtmp1.Normalize();

				tmp2.x = dtmp1.X()*(GetVelocity(t))[0];
				tmp2.y = dtmp1.Y()*(GetVelocity(t))[0];
				tmp2.z = dtmp1.Z()*(GetVelocity(t))[0];

				tmp3.push_back(tmp2);

				
				pit1.x = tmp.X();
				pit1.y = tmp.Y();
				pit1.z = tmp.Z();


				if( m_StartParam[1] + d[1] > lastParam2 )
				{
     				anAdaptorCurve2.D1(m_StartParam[1] + d[1] - period2, tmp, dtmp1);
				}
				else if( m_StartParam[1] + d[1]< firstParam2)
				{
					anAdaptorCurve2.D1(m_StartParam[1] + d[1] + period2, tmp, dtmp1);
				}
				else
				{
					anAdaptorCurve2.D1(m_StartParam[1] + d[1], tmp, dtmp1);
				}

				if(m_it1 != m_BSplineTop.begin() && m_dir == 1)
					dtmp1 = -dtmp1;

				dtmp1.Normalize();

				tmp2.x = dtmp1.X()*(GetVelocity(t))[1];
				tmp2.y = dtmp1.Y()*(GetVelocity(t))[1];
				tmp2.z = dtmp1.Z()*(GetVelocity(t))[1];

				tmp3.push_back(tmp2);	

				
				pit2.x = tmp.X();
				pit2.y = tmp.Y();
				pit2.z = tmp.Z();

				
				if(m_it1==m_BSplineTop.begin()+30)
				{
					m_log3d.addSinglePoint(pit1.x,pit1.y,pit1.z);
					m_log3d.addSinglePoint(pit2.x,pit2.y,pit2.z);
					m_log3d.addSingleArrow(pit1,pit2,1);
				}

				m_Output.push_back(tmp3);
				tmp3.clear();
				
				t += m_del_t;
			}

			t = m_T;

			m_Output_time.push_back(t);

			tmp2.x = 0.0;
			tmp2.y = 0.0;
			tmp2.z = 0.0;
			
			tmp3.push_back(tmp2);
			tmp3.push_back(tmp2);

			m_Output.push_back(tmp3);
			tmp3.clear();
		}
		else
		{

			
			N = w1;
			double he = 0.9;

			while(w2/N > m_dbound)
			{
				N = m_dbound/w2;


				if(w1/N < he)
				{
					N = w1/he;
					he = he - 0.1;
				}

				if(he<0.5)
				{
					N = w1/0.5;
					break;
				}
			}


			for(int i=0; i<N; ++i)
			{
				anAdaptorCurve.D0(firstParam1 + i*w1/N, tmp);

				tmp2.x = tmp.X();
				tmp2.y = tmp.Y();
				tmp2.z = tmp.Z();

				m_Output_robo1.push_back(tmp2);

				anAdaptorCurve2.D0(firstParam2 + i*w2/N, tmp);

				tmp2.x = tmp.X();
				tmp2.y = tmp.Y();
				tmp2.z = tmp.Z();

				m_Output_robo2.push_back(tmp2);
			}
		}
	}

	GeomAdaptor_Curve curve1(*m_it1);	
	gp_Pnt p1,p2;
		
	if(m_BSplineTop.size()>1 && m_it1 != (m_BSplineTop.end()-1))
	{
		++m_it1;
	    GeomAdaptor_Curve curve2(*m_it1);

		std::vector<double> d;
		d.push_back( (curve2.LastParameter()-curve2.FirstParameter())/100 );
		
        curve1.D0(m_StartParam[0],        p1);

		Projection_pnt2curve(p1,*m_it1, m_StartParam[0]);
		--m_it1;
		
		if(m_it1 == m_BSplineTop.begin())
		{			
			curve1.D0(m_StartParam[0] + d[0], p2);
			
			gp_Vec vec1(p1,p2);

			curve2.D0(m_StartParam[1],        p1);
        	curve2.D0(m_StartParam[1] + d[0], p2);

			gp_Vec vec2(p1,p2);

			if((acos(vec1.Dot(vec2)/(vec1.Magnitude()*vec2.Magnitude())))*(180/PI) > 90)
				m_dir = 0;                               /* keine manuelle richtungsänderung notwendig */
			else
				m_dir = 1;
		}
		else
		{
			
			if(m_dir == 0)				
    			curve1.D0(m_StartParam[0] + d[0], p2);
			else
				curve1.D0(m_StartParam[0] - d[0], p2);

			gp_Vec vec1(p1,p2);

   			curve2.D0(m_StartParam[1],        p1);
           	curve2.D0(m_StartParam[1] + d[0], p2);

	    	gp_Vec vec2(p1,p2);

			if((acos(vec1.Dot(vec2)/(vec1.Magnitude()*vec2.Magnitude())))*(180/PI) > 90)
				m_dir = 0;                          /* keine manuelle richtungsänderung notwendig */
			else
				m_dir = 1;
		}
	}


	return true;
}

bool path_simulate::MakePathRobot()
{
	int c=1;
		
	ofstream anOutputFile;
	anOutputFile.open("M:/Documents/output.k");  
	anOutputFile.precision(2);
	anOutputFile.setf(ios::fixed,ios::floatfield);

	for(m_it1 = m_BSplineTop.begin(); m_it1 < m_BSplineTop.end(); ++m_it1)
	{
		ConnectPaths_xy(anOutputFile,c,1);
		ConnectPaths_z(anOutputFile,c,1);
		MakeSinglePath(anOutputFile,c,1);	

		if(m_single==false && (m_it1 != (m_BSplineTop.end()-1)))
			++m_it2;
	}

	cout << "writing" << endl;
	WriteOutput(anOutputFile,c,1);
	m_Output_robo1.clear();
	m_Output_robo2.clear();
	
	return true;

}

bool path_simulate::MakePathSimulate()
{
	int c=1;
		
	ofstream anOutputFile;
	anOutputFile.open("M:/Documents/output.k");  
	anOutputFile.precision(7);
	
	for(m_it1 = m_BSplineTop.begin(); m_it1 < m_BSplineTop.begin()+9; ++m_it1)
	{
		cout << "connecting" << endl;      
	
		ConnectPaths_xy(anOutputFile,c,0);
		//WriteOutput(anOutputFile,c,0);
		m_Output.clear();
		m_Output_time.clear();
		
		UpdateParam();

	    ConnectPaths_z(anOutputFile,c,0);
		//WriteOutput(anOutputFile,c,0);
        m_Output.clear();
		m_Output_time.clear();
		
		UpdateParam();
		EstimateMaxAcceleration();


		cout << "max velocity: "     << m_v1 << endl;
		cout << "max acceleration: " << m_a1 << endl;
		cout << "makesinglepath" << endl;

		MakeSinglePath(anOutputFile,c,0);
		

		if(m_single==false && (m_it1 != (m_BSplineTop.end()-1)))
			++m_it2;

        cout << "writing" << endl;

        //WriteOutput(anOutputFile,c,0);
        m_Output.clear();
		m_Output_time.clear();
        
		UpdateParam();
	}

	//WriteOutput(anOutputFile, c, 0);
    m_Output.clear();
    m_Output_time.clear();

	//m_log3d.saveToFile("c:/test.iv");

	
	anOutputFile << "*END" << endl;
	anOutputFile.close();

	return true;
}

bool path_simulate::WriteOutput(ofstream &anOutputFile, int &c, bool brob)
{

	if(m_single == false)
	{
		if(brob == false)
		{
			int n = m_Output.size();

			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;	

			for(int i=0; i<n; ++i)
			{
				anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x<< std::endl;


			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;	

			for(int i=0; i<n; ++i)
			{
				anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][1].x << std::endl;
			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].x<< std::endl;
			
			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+2 << std::endl;
		    
			for(int i=0; i<n; ++i)
			{
				anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y<< std::endl;
 			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y<< std::endl;


			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+3 << std::endl;
		    
			for(int i=0; i<n; ++i)
			{
				anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][1].y<< std::endl;
 			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].y<< std::endl;
			
			
			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+4 <<  std::endl;
		    
			for(int i=0; i<n; ++i)
			{
				anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;


			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+5 <<  std::endl;
		    
			for(int i=0; i<n; ++i)
			{
				anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][1].z<< std::endl;
			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].z<< std::endl;

			c = c+6;
		}
		else
		{
			int n1 = m_Output_robo1.size();
			int n2 = m_Output_robo2.size();

			for(int i=0; i<n1; ++i)
				anOutputFile << m_Output_robo1[i].x << "," <<  m_Output_robo1[i].y << "," << m_Output_robo1[i].z << "," << m_Output_robo2[i].x << "," <<  m_Output_robo2[i].y << "," << m_Output_robo2[i].z << endl;

			anOutputFile << std::endl;
		}

	}
	else
	{
		if(brob == false)
		{

			int n = m_Output.size();

			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;	

			for(int i=0; i<n; ++i)
			{
				anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x<< std::endl;

			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;
		    
			for(int i=0; i<n; ++i)
			{
				anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y<< std::endl;
 			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y<< std::endl;

			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
			anOutputFile << "*DEFINE_CURVE" << std::endl << c+4 <<  std::endl;
		    
			for(int i=0; i<n; ++i)
			{
				anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
			}

			anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;

			c = c+3;
		}
		else
		{
			int n = m_Output_robo1.size();
	
			for(int i=0; i<n; ++i)
				anOutputFile << m_Output_robo1[i].x << "," <<  m_Output_robo1[i].y << "," << m_Output_robo1[i].z << ",";

			anOutputFile << std::endl;
		}
    }

	return true;
}
