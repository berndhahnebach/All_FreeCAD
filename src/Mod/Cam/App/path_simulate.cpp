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

path_simulate::path_simulate(std::vector<Handle_Geom_BSplineCurve>& BSplineTop, double a, double v)
:m_BSplineTop(BSplineTop),
m_amax(a),
m_vmax(v),
m_a1(a),
m_a2(a),
m_v1(v),
m_v2(v),
m_t0(0)
{
	m_it1 = BSplineTop.begin();

	m_NumPaths = BSplineTop.size();

	gp_Pnt p(0,0,2);             /* startpunkt master tool */
	
	m_StartPnts1.push_back(p);
   (*m_it1)->D0((*m_it1)->FirstParameter(),p);
	m_StartPnts1.push_back(p);

	m_single = true;
	//m_NumPaths = m_BSplineTop.size();
	//EstimateMaxAcceleration();
	
	/*vorläufige startparameter*/
	/*master tool*/
	m_c1.push_back(m_a1/2);
	m_c1.push_back(PI*m_a1/m_v1);

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T = 2*m_t1;
}

path_simulate::path_simulate(std::vector<Handle_Geom_BSplineCurve> &BSplineTop, std::vector<Handle_Geom_BSplineCurve> &BSplineBottom, 
							  double a, double v)
:m_BSplineTop(BSplineTop),
m_BSplineBottom(BSplineBottom),
m_amax(a),
m_vmax(v),
m_a1(a),
m_a2(a),
m_v1(v),
m_v2(v),
m_t0(0)
{

	m_it1 = m_BSplineTop.begin();
	m_it2 = m_BSplineBottom.begin();

	gp_Pnt p(0,0,2);             /* startpunkt master tool */
	gp_Pnt q(0,0,-12);           /* startpunkt supporting die */
	
	m_StartPnts1.push_back(p);
    m_StartPnts2.push_back(q);

    m_StartParam1 = (*m_it1)->FirstParameter();
	
	(*m_it1)->D0(m_StartParam1,p);
	m_StartPnts1.push_back(p);
	m_StartPnts2.push_back(Projection_pnt2curve(p,*m_it2, m_StartParam2));

	m_single=false;
	//EstimateMaxAcceleration();
	
	/*master tool*/
	m_c1[0] = m_a1/2;
	m_c1[1] = PI*m_a1/m_v1;

	m_t1 = (2*m_v1/m_a1) + m_t0;
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
  gp_Pnt aNearestPoint;

  if (aNumSolutions > 0)
  {
    aNearestPoint = aPPC.NearestPoint();
    return aNearestPoint;
  }
  else
  {
	throw Base::Exception("no projection computed");
  }
}



double path_simulate::GetLength(GeomAdaptor_Curve& curve, const Standard_Real startParameter,const Standard_Real endParameter)
{
    Standard_Real firstParameter = curve.FirstParameter();
    Standard_Real lastParameter = curve.LastParameter();


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

    
    return GCPnts_AbscissaPoint::Length(curve,sParam,eParam);
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
	std::vector<double> a(2);
	a.clear();

	a[0] = 0.0; //a.push_back(0.0); Damit es konsistent zum anderen Zeugs bei dir ist
	a[1] = -1.0; //a.push_back(-1.0); 

	if ((t>m_t0) && (t<=m_t1))
	{
		a[0] = m_c1[0]*(cos(m_c1[1]*(t-m_t0) - PI) + 1);
		
		if(m_single == false)		
			a[1] = m_c2[0]*(cos(m_c2[1]*(t-m_t0) - PI) + 1);

		return a;
	}
	else if ((t>m_t1)  && (t<=m_t2))
		return a;
	else if ((t>m_t2) && (t<=m_T))
	{
		a[0] = -m_c1[0]*(cos(m_c1[1]*(t-m_t2) - PI) + 1);
		
		if(m_single == false)		
			a[1] = -m_c2[0]*(cos(m_c2[1]*(t-m_t2) - PI) + 1);

		return a;
	}
	else
	{
		throw Base::Exception("time input not inside [t0,T]");
	}
}

std::vector<double> path_simulate::GetVelocity(double t)
{
	std::vector<double> v(2);
	v.clear();

	v[0] = m_v1;
	v[1] = -1.0; 

	if (t>m_t0 && t<=m_t1)
	{
		v[0] = m_c1[0]*(sin(m_c1[1]*(t-m_t0) - PI)/m_c1[1] + (t-m_t0));

		if(m_single == false)		
			v[1] = m_c2[0]*(sin(m_c2[1]*(t-m_t0) - PI)/m_c2[1] + (t-m_t0));

		return v;
	}
	else if (t>m_t1  && t<=m_t2)
		return v;
	else if (t>m_t2 && t<=m_T)
	{
		v[0] = m_v1 - m_c1[0]*(sin(m_c1[1]*(t-m_t2) - PI)/m_c1[1] + (t-m_t2));

		if(m_single == false)		
			v[1] = m_v1 - m_c2[0]*(sin(m_c2[1]*(t-m_t2) - PI)/m_c2[1] + (t-m_t2));

		return v;
	}
	else
	{
		throw Base::Exception("time input not inside [t0, T]");
	}
}

std::vector<double> path_simulate::GetDistance(double t)
{
	std::vector<double> d(2);
	d.clear();

	d[0] = 0.0; 
	d[1] = -1.0;

	if (t>m_t0 && t<=m_t1)
	{
		d[0] = -(m_c1[0]/pow(m_c1[1],2.0))*cos(m_c1[1]*(t-m_t0) - PI) + m_c1[0]*pow((t-m_t0),2.0)/2 - m_c1[0]/(m_c1[1]*m_c1[1]);

	    if(m_single == false)		
			d[1] = -(m_c2[0]/pow(m_c2[1],2.0))*cos(m_c2[1]*(t-m_t0) - PI) + m_c2[0]*pow((t-m_t0),2.0)/2 - m_c2[0]/(m_c2[1]*m_c2[1]);

		return d;
	}
	else if (t>m_t1  && t<=m_t2)
	{
		d[0] = m_c1[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(t-m_t1);

		if(m_single == false)		
			d[1] = m_c2[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(t-m_t1);

		return d;
	}
	else if (t>m_t2 && t<=m_T)
	{
		d[0] = m_c1[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(m_t2-m_t1) + (m_c1[0]/pow(m_c1[1],2.0))*cos(m_c1[1]*(t-m_t2) - PI) - m_c1[0]*pow((t-m_t2),2.0)/2 + (m_c1[0]/pow(m_c1[1],2.0)) + m_v1*(t-m_t2);

		if(m_single == false)		
			d[1] = m_c2[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(m_t2-m_t1) + (m_c2[0]/pow(m_c2[1],2.0))*cos(m_c2[1]*(t-m_t2) - PI) - m_c2[0]*pow((t-m_t2),2.0)/2 + m_c2[0]/pow(m_c2[1],2.0) + m_v1*(t-m_t2);

		return d;
	}
	else
		throw Base::Exception("time input not inside [t0,T]");
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



/*Parameterberechnung für die Geschwindigkeitsfunktion*/


bool path_simulate::ParameterCalculation(double S1, double S2)
{	
	if((GetDistance(m_t1))[0] <= S1/2)                     /*Fall 1:  distance(m_t1) =< S/2*/
		m_t2 = m_t1 + (S1 - 2*(GetDistance(m_t1))[0])/m_v1;
	else												  /*Fall 2*/				
	{
        m_v1 = sqrt(m_a1*S1/2);            /*m_v1 muss neu berechnet werden*/
		m_c1[1] = PI*m_a1/m_v1;
        m_t1   = m_t0 + 2*m_v1/m_a1;
		m_t2  = m_t1;
	}
	
	m_T= m_t1 + m_t2 - m_t0;

	/*increm. die*/
	m_v2 = S2/(m_T-m_t0);
	m_a2 = m_a1*m_v2/m_v1;
	m_c2[0] = m_a2/2;
	m_c2[1] = PI*m_a2/m_v2;


	return true;
}

bool path_simulate::ParameterCalculation(double S1)
{
	if((GetDistance(m_t1))[0] <= S1/2)                      /*Fall 1:  distance(m_t1) =< S/2*/
		m_t2 = m_t1 + (S1 - 2*(GetDistance(m_t1))[0])/m_v1;
	else												/*Fall 2*/				
	{
        m_v1 = sqrt(m_a1*S1/2);            /*m_v1 muss neu berechnet werden*/
		m_c1[1] = PI*m_a1/m_v1;
        m_t1   = m_t0 + 2*m_v1/m_a1;
		m_t2  = m_t1;
	}
	
	m_T= m_t1 + m_t2 - m_t0;

	return true;
}

std::vector<std::vector<Base::Vector3d>> path_simulate::PointEvaluation(double T, unsigned int N, double startParam)
{
    double t = m_t0;
	double foundParameter;
	std::vector<double> d;
	gp_Pnt tmp;
	gp_Vec dtmp;
	Base::Vector3d tmp2;

	GeomAdaptor_Curve curve1(*m_it1);

	m_del_t = ( (m_T- m_t0)/N);
	
	std::vector<std::vector<Base::Vector3d>> pnts;
	std::vector<Base::Vector3d> tmp3;

	for(unsigned int i=0; i<N; ++i)
	{
		t += m_del_t;

		if(i==(N-1))
			t = m_T;

		d = GetDistance(t);

		//foundParameter = FindParamAt(curve1, d[0], startParam);       /*master tool*/
		curve1.D1(d[0],tmp,dtmp);
		
		dtmp.Normalize();

		tmp2.x = dtmp.X();
		tmp2.y = dtmp.Y();
		tmp2.z = dtmp.Z();

		tmp3.push_back(tmp2); 

		if(m_single == false)
		{
			GeomAdaptor_Curve curve2(*m_it2);

			foundParameter = FindParamAt(curve2, d[1], startParam);   /*increm. die*/
			curve2.D1(foundParameter,tmp,dtmp);

			tmp2.x = dtmp.X();
		    tmp2.y = dtmp.Y();
		    tmp2.z = dtmp.Z();

		    tmp3.push_back(tmp2); 
		}

		pnts.push_back(tmp3);
		tmp3.clear();
	}

	return pnts;
}

std::vector<std::vector<Base::Vector3d>> path_simulate::Derivate(const std::vector<std::vector<Base::Vector3d>> &D0)
{
	std::vector<std::vector<Base::Vector3d>> D1 = D0;
	int N = D0.size();
	
	D1[0][0] = (D0[1][0]-D0[N-1][0])/(2*m_del_t);
    //D1[0][0].Normalize();

    for (int i=1; i<N-1; ++i)
	{
        D1[i][0] = (D0[i+1][0]-D0[i-1][0])/(2*m_del_t);
        //D1[i][0].Normalize();
	}

    D1[N-1][0] = (D0[0][0]-D0[N-2][0])/(2*m_del_t);
    //D1[N-1][0].Normalize();

	if(m_single == false)
	{
		D1[0][1] = (D0[1][1]-D0[N-1][1])/(2*m_del_t);
		//D1[0][1].Normalize();

		for (int i=1; i<N-1; ++i)
		{
			D1[1][i] = (D0[i+1][1]-D0[i-1][1])/(2*m_del_t);
			//D1[i][1].Normalize();
		}

		D1[N-1][1] = (D0[0][1]-D0[N-2][1])/(2*m_del_t);
		//D1[N-1][1].Normalize();
	}

	return D1;
}

bool path_simulate::OutputPath(std::vector<std::vector<Base::Vector3d>> &D1, std::vector<std::vector<Base::Vector3d>> &D2)
{
	std::vector<Base::Vector3d> tmp2;
	Base::Vector3d tmp;
	double t = m_t0;
     
	for (unsigned int i=0; i<D1.size(); ++i)
	{
        t += m_del_t;

		if(i==(D1.size()-1))
			t = m_T;

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
		else
		{
			tmp.x = 0;
			tmp.y = 0;
			tmp.z = 0;

			tmp2.push_back(tmp);
		}

		m_Output.push_back(tmp2);
		tmp2.clear();
	}

	return true;
            
}

bool path_simulate::UpdateParam()
{
	//EstimateMaxAcceleration();
	m_t0 = m_T;   /* endzeit des letzten durchlaufs wird zur neuen startzeit */

	m_c1[0] = m_a1/2;
	m_c1[1] = PI*m_a1/m_v1;

	m_t1 = 2*m_v1/m_a1 + m_t0;
	m_t2 = m_t1;
	m_T  = 2*m_t1;

	m_StartPnts1[0] = m_StartPnts1[1];
	m_StartPnts1[1] = Projection_pnt2curve(m_StartPnts1[0],*m_it1, m_StartParam1);

	if(m_single == false)
	{
		m_StartPnts2[0] = m_StartPnts2[1];
		m_StartPnts2[1] = Projection_pnt2curve(m_StartPnts1[1],*m_it2, m_StartParam2);
	}

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

bool path_simulate::ConnectPaths()
{
	int N = 100;
    double t = m_t0;
	
	Base::Vector3d tmp;
	std::vector<Base::Vector3d> tmp2;
	
	if(m_single == false)
	{
		gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);
		gp_Vec vec_2(m_StartPnts2[0], m_StartPnts2[1]);

		ParameterCalculation(vec_1.Magnitude(), vec_2.Magnitude());

		vec_1.Normalize();
		vec_2.Normalize();

		m_del_t = m_T/N;

		for (int i=0; i<N; ++i)
		{
			t += m_del_t;

			tmp.x = vec_1.X() * (GetAcceleration(t))[0];
			tmp.y = vec_1.Y() * (GetAcceleration(t))[0];
			tmp.z = vec_1.Z() * (GetAcceleration(t))[0];

			tmp2.push_back(tmp);

			tmp.x = vec_2.X() * (GetAcceleration(t))[1];
			tmp.y = vec_2.Y() * (GetAcceleration(t))[1];
			tmp.z = vec_2.Z() * (GetAcceleration(t))[1];

			tmp2.push_back(tmp);

			m_Output.push_back(tmp2);
			tmp2.clear();

		}
	}
	else
	{
		gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);

		ParameterCalculation(vec_1.Magnitude());

		vec_1.Normalize();

		m_del_t = m_T/N;

		for (int i=0; i<N; ++i)
		{
			t += m_del_t;

			if(i==N-1)
				t = m_T;

			tmp.x = vec_1.X() * (GetAcceleration(t))[0];
			tmp.y = vec_1.Y() * (GetAcceleration(t))[0];
			tmp.z = vec_1.Z() * (GetAcceleration(t))[0];

			tmp2.push_back(tmp);

			m_Output.push_back(tmp2);
			tmp2.clear();
		}
	}
	return true;
}


bool path_simulate::EstimateMaxAcceleration()
{
	GeomAdaptor_Curve anAdaptorCurve1;

	anAdaptorCurve1.Load(*m_it1);
    double w1 = GetLength(anAdaptorCurve1, anAdaptorCurve1.FirstParameter(), anAdaptorCurve1.LastParameter());

	int N = w1;

	std::vector< std::vector<Base::Vector3d> > D0(2,N);   /* 0.abl entspricht punkte auf kurve */
	std::vector< std::vector<Base::Vector3d> > D1(2,N);   /* 1.abl */
	std::vector< std::vector<Base::Vector3d> > D2(2,N);   /* 2.abl */

	m_c1.push_back(m_amax/2);
	m_c2.push_back(m_amax/2);

	m_c1.push_back(PI*m_amax/m_vmax);
	m_c2.push_back(PI*m_amax/m_vmax);

	m_t1 = 2*m_vmax/m_amax + m_t0;
	m_t2 = m_t1;
	m_T= 2*m_t1;

    ParameterCalculation(w1);
	
	D0 = PointEvaluation(m_T, N, 0);
	D1 = Derivate(D0);
	//D2 = Derivate(D1);
    D2 = D1;
	D1 = D0;
	
	/* berechnung des maximums der 2.ableitung nach der zeit */
	double M  = pow(D2[0][0].x, 2) + pow(D2[0][0].y, 2) + pow(D2[0][0].z, 2);
	double m1 = (D2[0][0].x)   + (D2[0][0].y)   + (D2[0][0].z);
	double m2 = M;

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
		m_v1 = sqrt(pow(m_amax,2)/M);
	else
		m_v1 = m_vmax;

	m_a1 = (-m_v1*m1 + sqrt(pow(m_v1*m1,2) - 3*m2*pow(m_v1,2) - 12*pow(m_amax,2)))/6;

	if(m_single == false)
	{
		for(int i=1; i<N; ++i)
		{
			if(M < (pow(D2[i][1].x, 2) + pow(D2[i][1].y, 2) + pow(D2[i][1].z, 2)))
				M = pow(D2[i][1].x, 2) + pow(D2[i][1].y, 2) + pow(D2[i][1].z, 2);

			if(m1 > ((D2[i][1].x) + (D2[i][1].y) + (D2[i][1].z)))
				m1 = (D2[i][1].x) + (D2[i][1].y) + (D2[i][1].z);

			if(m2 >  (pow(D2[i][1].x, 2) + pow(D2[i][1].y, 2) + pow(D2[i][1].z, 2)))
				m2 = (pow(D2[i][1].x, 2) + pow(D2[i][1].y, 2) + pow(D2[i][1].z, 2));
		}

		if(pow(m_vmax,2)* M > pow(m_amax,2))
			m_v2 = sqrt(pow(m_amax,2)/M);
		else
			m_v2 = m_vmax;

		m_a2 = (-m_v1*m1 + sqrt(pow(m_v1*m1,2) - 3*m2*pow(m_v1,2) - 12*pow(m_amax,2)))/6;
	}

	return true;
}

bool path_simulate::MakeSinglePath()
{

	
	GeomAdaptor_Curve anAdaptorCurve;

	anAdaptorCurve.Load(*m_it1);
	double w1 = GetLength(anAdaptorCurve, anAdaptorCurve.FirstParameter(), anAdaptorCurve.LastParameter());

	if(m_single==false)
    {
		anAdaptorCurve.Load(*m_it2);
		double w2 = GetLength(anAdaptorCurve, anAdaptorCurve.FirstParameter(), anAdaptorCurve.LastParameter());
		ParameterCalculation(w1,w2);
	}
	else
		ParameterCalculation(w1);

	int N = w1;
	
	std::vector< std::vector<Base::Vector3d> > D0(2,N);   /* 0.abl entspricht punkte auf kurve */
	std::vector< std::vector<Base::Vector3d> > D1(2,N);   /* 1.abl */
	std::vector< std::vector<Base::Vector3d> > D2(2,N);   /* 2.abl */
    
	
	D0 = PointEvaluation(m_T, N, 0);
	D1 = Derivate(D0);
	//D2 = Derivate(D1);

	D2 = D1;
	D1= D0;

	OutputPath(D1,D2);
	
	return true;
}

bool path_simulate::MakePath()
{
	ofstream anOutputFile;
	anOutputFile.open("c:/output.txt");

	for(int i=0; i<m_NumPaths; ++i)
	{
		ConnectPaths();
		UpdateParam();

		MakeSinglePath();
		UpdateParam();
	}


	for(int i=0; i<m_Output:(length(x))
	{
		anOutputFile << t(i) <<"," << x(i) << std::endl;
	}
	anOutputFile << "*DEFINE_CURVE" << std::endl << "2, 0,0.000,0.000,0.000,0" << std::endl;

	
	for i=1:(length(x))
	{
		anOutputFile << t(i) <<"," << x(i) << std::endl;
	}
	anOutputFile << "*DEFINE_CURVE" << std::endl << "2, 0,0.000,0.000,0.000,0" << std::endl;

	for i=1:(length(x))
	{
		anOutputFile << t(i) <<"," << x(i) << std::endl;
	}
	anOutputFile << "*DEFINE_CURVE" << std::endl << "2, 0,0.000,0.000,0.000,0" << std::endl;
	for i=1:(length(y));
    fprintf(fid,'%6.10f,%6.3f\n',t(i),y(i));
end
fprintf(fid,'*DEFINE_CURVE\n$     lcid      sidr      scla      sclo      offa      offo\n3, 0,0.000,0.000,0.000,%6.3f\n$\n',0);
for i=1:(length(x));
    fprintf(fid,'%6.10f,%6.3f\n',t(i),z(i));

	anOutputFile.close();
	return true;
}
