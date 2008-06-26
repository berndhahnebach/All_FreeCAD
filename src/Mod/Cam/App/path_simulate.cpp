/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                  *
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

#include "path_simulate.h"

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Base/Exception.h>
#include <Base/Builder3D.h>
#include <GCPnts_AbscissaPoint.hxx>

#define curvTOL  30.0
#define boundTOL 20.0

// Konstruktor mit einer Bahnfolge (master tool) als Input

/*
path_simulate::path_simulate(const std::vector<Handle_Geom_BSplineCurve>& BSplineTop, double a_max, double v_max)
        :m_BSplineTop(BSplineTop),m_amax(0.85*a_max),m_vmax(0.85*v_max),m_a1(0.85*a_max),m_v1(0.85*v_max),
        m_t0(0.0),m_step(1e-3),m_t(0.0),m_count(1),m_clip(90000)
{
    m_single = true;

    m_it1 = m_BSplineTop.begin(); 
    m_NumPaths = m_BSplineTop.size();

    gp_Pnt p;
    gp_Pnt q(0,0,1); // startpunkt master tool 

    (*m_it1)->D0((*m_it1)->FirstParameter(),p); // startpunkt auf der Kurve

    q.SetZ(p.Z() + 2);  // initial-z-level 2mm über dem ersten Kurvenstartpunkt

    m_StartPnts1.push_back(q);
    m_StartPnts1.push_back(p);

    m_StartParam.push_back((*m_it1)->FirstParameter());
    m_StartParam.push_back(0.0);

    //EstimateMaxAcceleration();

    //vorläufige startparameter
    //master tool
    m_c1.push_back(m_a1/2);
    m_c1.push_back(PI*m_a1/m_v1);

    m_t1 = 2*m_v1/m_a1 + m_t0;
    m_t2 = m_t1;
    m_T = 2*m_t1;
}
*/

/* Konstruktor mit zwei Bahnfolgen (master tool & supporting die) als Input */
path_simulate::path_simulate(const std::vector<Handle_Geom_BSplineCurve> &BSplineTop,
                             const std::vector<Handle_Geom_BSplineCurve> &BSplineBottom,
                             struct CuttingToolsSettings& set)
        :m_BSplineTop(BSplineTop),m_BSplineBottom(BSplineBottom),m_set(set),
        m_t0(0.0),m_step(1e-3),m_clip(90000)
{
    m_single=false;

    m_pretension = m_set.spring_pretension;
    m_blech = (float) m_set.sheet_thickness;

	if(m_pretension > 0) beam = true;
	else           		 beam = false;

    m_amax  = 0.85*m_set.max_Acc;
    m_vmax  = 0.85*m_set.max_Vel;
    m_a1    = m_amax;
    m_a2    = m_amax;
    m_v1    = m_vmax;
    m_v2    = m_vmax;

    //Initialize the Iterators
    m_it1 = m_BSplineTop.begin();
    m_it2 = m_BSplineBottom.begin();

    /* The size of Master and Slave paths are equal */
    m_NumPaths = BSplineTop.size();

    //First we clear the vectors
    m_StartPnts1.clear();
    m_StartPnts2.clear();

    //Initalise some vars
    gp_Pnt p(0,0,0);
    gp_Pnt q(0,0,0);

    /*------------------------------Generate the first movement of the Master------------*/

    /* Fill p with the starting point of the first Master curve*/
    (*m_it1)->D0((*m_it1)->FirstParameter(),p);

    /* Set q to the initial-Z-level in the Simulation: 2mm + Master-Radius over the sheet which is located at Z=0 */
    q.SetZ(2.0 + set.master_radius);

    //Now we insert the start points for the Master movement
    m_StartPnts1.push_back(q);
    m_StartPnts1.push_back(p);

    /*Master finished here*/

    /*-----------------------------Generate the first movement for the Slave--------------*/

    /* Fill p with the starting point of the first Slave curve*/
    (*m_it2)->D0((*m_it2)->FirstParameter(),p);

    /* Set q to the initial-Z-level in the Simulation: -5mm - Slave-Radius-Spring-Pretensionbelow the sheet upper level which is located at Z=0 */
    q.SetZ(-5.0 - set.slave_radius - m_pretension);

    /*Now we insert the start points for the Master movement*/
    m_StartPnts2.push_back(q);
    m_StartPnts2.push_back(p);

    /*Slave finished here*/


    //Other stuff.....!!!!Human bitte was dazu schreiben
    m_StartParam.push_back((*m_it1)->FirstParameter());
    m_StartParam.push_back((*m_it2)->FirstParameter());
    m_c1.push_back(m_a1/2);
    m_c1.push_back(PI*m_a1/m_v1);
    m_c2 = m_c1;
}

path_simulate::~path_simulate()
{
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


    return GCPnts_AbscissaPoint::Length(curve,sParam,eParam);   // genauigkeitssteuerung über parameter TOL nach eParam 
}


double path_simulate::FindParamAt(GeomAdaptor_Curve& curve, double dist, double startParam)
{
    double foundParameter = 0.0;

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
        if (t==m_t0)
        {
            a[0] = 0.0;

            if (m_single == false)
                a[1] = 0.0;
        }
        else
        {
            a[0] = m_c1[0]*(cos(m_c1[1]*(t-m_t0) - PI) + 1);

            if (m_single == false)
                a[1] = m_c2[0]*(cos(m_c2[1]*(t-m_t0) - PI) + 1);
        }
    }
    else if ((t>m_t1)  && (t<=m_t2));  /* hier passiert nichts */
    else if ((t>m_t2) && (t<=m_T))
    {
        a[0] = -m_c1[0]*(cos(m_c1[1]*(t-m_t2) - PI) + 1);

        if (m_single == false)
            a[1] = -m_c2[0]*(cos(m_c2[1]*(t-m_t2) - PI) + 1);
    }
    else
        throw Base::Exception("time input not inside [t0,T]");

    return a;
}

double path_simulate::GetVelocity(double t)
{
    double vel;
    double c[2];

    if ( (m_v[1] - m_v[0]) == 0.0) throw Base::Exception("division by NULL");

    c[0] = m_a/2.0;

    if (t>=m_t0 && t<=m_t1)
    {
        c[1] = PI*m_a / (m_v[1] - m_v[0]);
        if (t==m_t0)
        {
            vel = m_v[0];
        }
        else
        {
            vel = c[0]*(sin(c[1]*(t-m_t0) - PI)/c[1] + (t-m_t0)) + m_v[0];
        }
    }
    else if (t>m_t1  && t<=m_t2)
    {
        vel = m_v[1];
    }
    else if (t>m_t2 && t<=m_T)
    {
        c[1] = PI*m_a / (m_v[1] - m_v[2]);

        if (t==m_T)
        {
            vel = m_v[1] - c[0]*(t-m_t2);
        }
        else
        {
            vel = m_v[1] - c[0]*(sin(c[1]*(t-m_t2) - PI)/c[1] + (t-m_t2));
        }
    }
    else
        throw Base::Exception("time input not inside [t0, T]");

    return vel;
}

std::vector<double> path_simulate::GetVelocityOld(double t)
{
    std::vector<double> v(2);

    if (t>=m_t0 && t<=m_t1)
    {
        if (t==m_t0)
        {
            v[0] = 0.0;

            if (m_single == false)
                v[1] = 0.0;
        }
        else
        {
            if (m_c1[1] == 0.0)
                v[0] = 0.0;
            else
                v[0] = m_c1[0]*(sin(m_c1[1]*(t-m_t0) - PI)/m_c1[1] + (t-m_t0));

            if (m_single == false)
            {
                if (m_c2[1] == 0.0)
                    v[1] = 0.0;
                else
                    v[1] = m_c2[0]*(sin(m_c2[1]*(t-m_t0) - PI)/m_c2[1] + (t-m_t0));
            }
        }


    }
    else if (t>m_t1  && t<=m_t2)
    {
        v[0] = m_v1;

        if (m_single == false)
            v[1] = m_v2;
    }
    else if (t>m_t2 && t<=m_T)
    {
        if (m_c1[1] == 0.0)
            v[0] = 0.0;
        else
            v[0] = m_v1 - m_c1[0]*(sin(m_c1[1]*(t-m_t2) - PI)/m_c1[1] + (t-m_t2));

        if (m_single == false)
        {
            if (m_c2[1] == 0.0)
                v[1] = 0.0;
            else
                v[1] = m_v2 - m_c2[0]*(sin(m_c2[1]*(t-m_t2) - PI)/m_c2[1] + (t-m_t2));
        }
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
        if (t==m_t0)
        {
            d[0] = 0.0;

            if (m_single == false)
                d[1] = 0.0;
        }
        else
        {
            if (m_c1[1] == 0.0)
                d[0] = 0.0;
            else
                d[0] = -(m_c1[0]/pow(m_c1[1],2.0))*cos(m_c1[1]*(t-m_t0) - PI) + m_c1[0]*pow((t-m_t0),2.0)/2 - m_c1[0]/(m_c1[1]*m_c1[1]);

            if (m_single == false)
            {
                if (m_c2[1] == 0.0)
                    d[1] = 0.0;
                else
                    d[1] = -(m_c2[0]/pow(m_c2[1],2.0))*cos(m_c2[1]*(t-m_t0) - PI) + m_c2[0]*pow((t-m_t0),2.0)/2 - m_c2[0]/(m_c2[1]*m_c2[1]);
            }
        }
    }
    else if (t>m_t1  && t<=m_t2)
    {
        d[0] = m_c1[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(t-m_t1);

        if (m_single == false)
            d[1] = m_c2[0]*pow((m_t1-m_t0),2.0)/2 + m_v2*(t-m_t1);
    }
    else if (t>m_t2 && t<=m_T)
    {
        if (m_c1[1] == 0.0)
            d[0] = 0.0;
        else
            d[0] = m_c1[0]*pow((m_t1-m_t0),2.0)/2 + m_v1*(m_t2-m_t1) + (m_c1[0]/pow(m_c1[1],2.0))*cos(m_c1[1]*(t-m_t2) - PI) - m_c1[0]*pow((t-m_t2),2.0)/2 + (m_c1[0]/pow(m_c1[1],2.0)) + m_v1*(t-m_t2);

        if (m_single == false)
        {
            if (m_c2[1] == 0.0)
                d[1] = 0.0;
            else
                d[1] = m_c2[0]*pow((m_t1-m_t0),2.0)/2 + m_v2*(m_t2-m_t1) + (m_c2[0]/pow(m_c2[1],2.0))*cos(m_c2[1]*(t-m_t2) - PI) - m_c2[0]*pow((t-m_t2),2.0)/2 + m_c2[0]/pow(m_c2[1],2.0) + m_v2*(t-m_t2);
        }
    }
    else
    {
        cout << m_t0 << "," << t << "," << m_T << endl;
        throw Base::Exception("time input not inside [t0,T]");
    }

    return d;
}

double path_simulate::GetDistanceNew(double t)
{
    double d;
    double c[2];

    c[0] = m_a/2.0;

    if (t>=m_t0 && t<m_t1)
    {
        c[1] = PI*m_a / (m_v[1] - m_v[0]);

        if (t==m_t0) d = 0.0;
        else         d = -(c[0]/pow(c[1],2.0))*cos(c[1]*(t-m_t0) - PI) + c[0]*pow((t-m_t0),2.0)/2 - c[0]/(c[1]*c[1]) + m_v[0]*(t-m_t0);
    }
    else if (t>=m_t1  && t<=m_t2)
    {
        d = c[0]*pow((m_t1-m_t0),2.0)/2 + m_v[0]*(m_t1-m_t0) + m_v[1]*(t-m_t1);
    }
    else if (t>m_t2 && t<=m_T)
    {
        c[1] = PI*m_a / (m_v[1] - m_v[2]);

        if (t==m_T)
        {
            d = c[0]*pow((m_t1-m_t0),2.0)/2 + m_v[0]*(m_t1-m_t0) + m_v[1]*(m_t2-m_t1) - c[0]*pow((t-m_t2),2.0)/2 + m_v[1]*(t-m_t2);
        }
        else d = c[0]*pow((m_t1-m_t0),2.0)/2 + m_v[0]*(m_t1-m_t0) + m_v[1]*(m_t2-m_t1) + (c[0]/pow(c[1],2.0))*cos(c[1]*(t-m_t2) - PI) - c[0]*pow((t-m_t2),2.0)/2 + (c[0]/pow(c[1],2.0)) + m_v[1]*(t-m_t2);
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

    if (S2 > S1)
    {
        b= true;
        S = S1;
        S1 = S2;
        S2 = S;
    }

    m_t1 = 2*m_v1/m_a1 + m_t0;
    m_t2 = m_t1;
    m_T = 2*m_t1 - m_t0;

    if ((GetDistance(m_t1))[0] <= S1/2)
    {
        /*Fall 1:  distance(m_t1) =< S/2*/
        m_t2 = m_t1 + (S1 - 2*(GetDistance(m_t1))[0])/m_v1;
        m_v2 = (S2 - S2*2*(GetDistance(m_t1))[0]/S1)/(m_t2-m_t1);
        m_a2 = (2*m_v2)/(m_t1-m_t0);


    }
    else                                    /*Fall 2*/
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

    if (m_v2!=0)
        m_c2[1] = PI*m_a2/m_v2;
    else
        m_c2[1] = 0.0;

    std::vector<double> m_c;

    if (b==true)
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

    if (S1 == 0.0)
    {
        m_T = m_t0;
        return true;
    }

    m_t1 = 2*m_v1/m_a1 + m_t0;
    m_t2 = m_t1;
    m_T = 2*m_t1 - m_t0;

    std::vector<double> tmp = GetDistance(m_t1);

    if (tmp[0] <= (S1/2))
    {

        m_t2 = m_t1 + (S1 - 2*tmp[0])/m_v1;

    }
    else                                  /*Fall 2*/
    {
        m_v1    = sqrt(m_a1*S1/2);                                   /*m_v1 muss neu berechnet werden*/
        m_c1[1] = PI*m_a1/m_v1;
        m_t1    = m_t0 + 2*m_v1/m_a1;
        m_t2    = m_t1;
    }

    m_T = m_t1 + m_t2 - m_t0;

    return true;
}

bool path_simulate::ParameterCalculationNew(double S1)
{
    m_t1 = m_t0 + 2*(abs(m_v[1]-m_v[0]))/m_a;
    m_t2 = m_t1;
    m_T  = m_t1 + 2*(abs(m_v[1]-m_v[2]))/m_a;

    double tmp;
    tmp = GetDistanceNew(m_T);

    if (tmp <= S1)
    {
        m_t2 = m_t1 + (S1 - tmp)/m_v[1];
    }
    else                                  /*Fall 2*/
    {
        m_v[1] = sqrt((m_a*S1 + m_v[0]*m_v[0] + m_v[2]*m_v[2])/2.0);
        m_t1 = m_t0 + 2*(abs(m_v[1]-m_v[0]))/m_a;
        m_t2 = m_t1;
    }

    m_T = m_t2 + 2*(abs(m_v[1]-m_v[2]))/m_a;

    return true;
}

/*
// berechnet die diskrete Ableitung einer Punktefolge über symmetrische Differenzen 
std::vector<std::vector<Base::Vector3d> > path_simulate::Derivate(const std::vector<std::vector<Base::Vector3d> > &D0)
{
    std::vector<std::vector<Base::Vector3d> > D1 = D0;
    unsigned int N = D0[0].size();

    D1[0][0] = (D0[0][1]-D0[0][N-1])/(2*m_del_t);

    for (unsigned int i=1; i<N-1; ++i)
    {
        D1[0][i] = (D0[0][i+1]-D0[0][i-1])/(2*m_del_t);
    }

    D1[0][N-1] = (D0[0][0]-D0[0][N-2])/(2*m_del_t);

    //if(m_single == false)
      {
         D1[0][1] = (D0[1][1]-D0[N-1][1])/(2*m_del_t);
    
	    //for (int i=1; i<N-1; ++i)
        //{
	       //D1[1][i] = (D0[i+1][1]-D0[i-1][1])/(2*m_del_t);
        //}
     
      //D1[N-1][1] = (D0[0][1]-D0[N-2][1])/(2*m_del_t);
    //}

    return D1;
}
*/
std::vector<std::vector<double> > path_simulate::CurvCurvature(const std::vector<std::vector<Base::Vector3d> > &D1)
{
    std::vector<std::vector<Base::Vector3d> > D2  = D1;
    std::vector<std::vector<double> > Cr(1);
    unsigned int N = D1[0].size();
    Cr[0].resize(N);

    D2[0][0] = (D1[0][1]-D1[0][N-1])/2.0;

    for (unsigned int i=1; i<N-1; ++i)
    {
        D2[0][i] = (D1[0][i+1]-D1[0][i-1])/2.0;
    }

    D2[0][N-1] = (D1[0][0]-D1[0][N-2])/2.0;


    D2[0][0]  = D1[0][0] % D2[0][0];
    Cr[0][0]  = D2[0][0].Length();
    Cr[0][0] /= pow(D1[0][0].Length(), 3);

    for (unsigned int i=1; i<N-1; ++i)
    {
        D2[0][i]  = D1[0][i] % D2[0][i];
        Cr[0][i]  = D2[0][i].Length();
        Cr[0][i] /= pow(D1[0][i].Length(), 3);
    }

    D2[0][N-1]  = D1[0][N-1] % D2[0][N-1];
    Cr[0][N-1]  = D2[0][N-1].Length();
    Cr[0][N-1] /= pow(D1[0][N-1].Length(), 3);

    // bilde kehrwert -> krümmungsradien
    for (unsigned int i=0; i<N; ++i)
    {
        if (Cr[0][i] == 0.0) Cr[0][i] = 1e+3;
        else             Cr[0][i] = 1.0 / Cr[0][i];
    }

    return Cr;
}

bool path_simulate::UpdateParam()
{
    m_Output.clear();
    m_Output_time.clear();
    m_Output2.clear();
    m_Output_time2.clear();

    m_v1 = m_vmax;
    m_a1 = m_amax;

    m_t0 = m_T; /* endzeit des letzten durchlaufs wird zur neuen startzeit */

    if (m_c1.empty() == true)
        m_c1.resize(2);

    m_c1[0] = m_a1/2;
    m_c1[1] = PI*m_a1/m_v1;

    return true;
}

bool path_simulate::CheckConnect()
{
    // Rückgabewert legt fest ob zuerst in z- oder in xy-Richtung zugestellt wird
    gp_Pnt tmp;

    // ab dem 2. lauf
    if (m_it1 != m_BSplineTop.begin() || m_it2 != m_BSplineBottom.begin())
    {

        m_StartPnts1.clear();
        m_StartPnts2.clear();

        // berechne neue Verbindungspunkte für die Zustellung - MASTER
        m_it1--;
        (*m_it1)->D0((*m_it1)->LastParameter(),tmp);
        m_it1++;
        m_StartPnts1.push_back(tmp);  // Startpunkt

        (*m_it1)->D0((*m_it1)->FirstParameter(),tmp);
        m_StartPnts1.push_back(tmp);  // Zielpunkt

        if (m_single == false)
        {
            // berechne neue Verbindungspunkte für die Zustellung - SLAVE
            m_it2--;
            (*m_it2)->D0((*m_it2)->LastParameter(),tmp);
            m_it2++;
            m_StartPnts2.push_back(tmp);  // Startpunkt

            (*m_it2)->D0((*m_it2)->FirstParameter(),tmp);
            m_StartPnts2.push_back(tmp);  // Zielpunkt
        }
    }
    else
    {
        return true;  // erste zustellung immer gleich
    }

    if (m_StartPnts1[0].Z() - m_StartPnts1[1].Z() >= 0.0) return true;
    else                                                 return false;
}

bool path_simulate::CheckConnect(bool tool)
{
    // Rückgabewert legt fest ob zuerst in z- oder in xy-Richtung zugestellt wird
    gp_Pnt tmp;

    // ab dem 2. lauf
    if (m_it1 != m_BSplineTop.begin() || m_it2 != m_BSplineBottom.begin())
    {

        if (m_Feat == true) goto FEAT;

        m_StartPnts1.clear();
        m_StartPnts2.clear();

        // berechne neue Verbindungspunkte für die Zustellung - MASTER
        m_it1--;
        (*m_it1)->D0((*m_it1)->LastParameter(),tmp);
        m_it1++;
        m_StartPnts1.push_back(tmp);  // Startpunkt

        (*m_it1)->D0((*m_it1)->FirstParameter(),tmp);
        m_StartPnts1.push_back(tmp);  // Zielpunkt

        if (m_single == false)
        {
            // berechne neue Verbindungspunkte für die Zustellung - SLAVE
            m_it2--;
            (*m_it2)->D0((*m_it2)->LastParameter(),tmp);
            m_it2++;
            m_StartPnts2.push_back(tmp);  // Startpunkt

            (*m_it2)->D0((*m_it2)->FirstParameter(),tmp);
            m_StartPnts2.push_back(tmp);  // Zielpunkt
        }
    }
    else
    {
        return true;  // erste zustellung immer gleich
    }

    if (m_StartPnts1[0].Z() - m_StartPnts1[1].Z() >= 0.0) return true;
    else                                                 return false;

FEAT:
    if (!tool)
    {
        m_StartPnts1.clear();
        m_it1--;
        (*m_it1)->D0((*m_it1)->LastParameter(),tmp);
        m_it1++;
        m_StartPnts1.push_back(tmp);  // Startpunkt

        (*m_it1)->D0((*m_it1)->FirstParameter(),tmp);
        m_StartPnts1.push_back(tmp);  // Zielpunkt

        if (m_StartPnts1[0].Z() - m_StartPnts1[1].Z() >= 0.0) return true;
        else                                                 return false;
    }
    else
    {
        m_StartPnts2.clear();
        m_it2--;
        (*m_it2)->D0((*m_it2)->LastParameter(),tmp);
        m_it2++;
        m_StartPnts2.push_back(tmp);  // Startpunkt

        (*m_it2)->D0((*m_it2)->FirstParameter(),tmp);
        m_StartPnts2.push_back(tmp);  // Zielpunkt

        if (m_StartPnts2[0].Z() - m_StartPnts2[1].Z() >= 0.0) return true;
        else                                                 return false;
    }
}

bool path_simulate::ConnectPaths_xy(bool brob)
{
    int N;
    double t = m_t0;
    gp_Pnt tmpPnt, pnt1, pnt2, p;
    std::vector<double> d;

    Base::Vector3d tmp;
    std::vector<Base::Vector3d> tmp2;


    if (m_single == false)
    {
        gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);
        gp_Vec vec_2(m_StartPnts2[0], m_StartPnts2[1]);

        gp_Vec2d vec_11, vec_21;

        vec_11.SetX(vec_1.X());  // master zustellung in XY-Richtung
        vec_11.SetY(vec_1.Y());

        if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())  // erster lauf -> xy zustellung (Slave)
        {
            vec_21.SetX(vec_2.X());
            vec_21.SetY(vec_2.Y());
        }
        else
        {
            vec_21.SetX(0.0);
            vec_21.SetY(vec_2.Z());  // slave zustellung in z-Richtung (ab 2.Lauf)
        }

        // Simulationsoutput
        if (brob == false)
        {
            ParameterCalculation(vec_11.Magnitude(), vec_21.Magnitude());

            if (vec_11.Magnitude() != 0)
                vec_11.Normalize();

            if (vec_21.Magnitude() != 0)
                vec_21.Normalize();

            N = int(ceil((m_T - m_t0)/m_step));  /* anzahl der zu erzeugenden Outputwerte */

            m_del_t = (m_T - m_t0)/N;

            Base::Vector3d pit1,pit2;

            for (int i=0; i<N; ++i)
            {
                m_Output_time.push_back(t);
                m_Output_time2.push_back(t);

                d = GetDistance(t);

                // MASTER
                tmp.x = vec_11.X()*(GetVelocityOld(t))[0];
                tmp.y = vec_11.Y()*(GetVelocityOld(t))[0];
                tmp.z = 0.0;

                tmp2.push_back(tmp);
                m_Output.push_back(tmp2);
                tmp2.clear();

                // SLAVE
                if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())
                {
                    tmp.x = vec_21.X()*(GetVelocityOld(t))[1];
                    tmp.y = vec_21.Y()*(GetVelocityOld(t))[1];
                    tmp.z = 0.0;
                }
                else
                {
                    tmp.x = 0.0;
                    tmp.y = 0.0;
                    tmp.z = vec_21.Y()*(GetVelocityOld(t))[1];
                }

                tmp2.push_back(tmp);
                m_Output2.push_back(tmp2);
                tmp2.clear();

                t += m_del_t;
            }

            m_Output_time.push_back(m_T);
            m_Output_time2.push_back(m_T);

            tmp.x = 0.0;
            tmp.y = 0.0;
            tmp.z = 0.0;

            tmp2.push_back(tmp);

            m_Output.push_back(tmp2);
            m_Output2.push_back(tmp2);
        }
        else  // Roboter Output
        {
            bool con = false;

            // hier wird die Anzahl der Punkte für die Diskretisierung der Zustellungslinie berechnet
            if (vec_11.Magnitude() > vec_21.Magnitude())
            {
                if (vec_21.Magnitude() < TolDist + 1.0) N=2;
                else                                   N = int(ceil(vec_21.Magnitude()/TolDist));  /* anzahl der zu erzeugenden Outputwerte */
            }
            else
            {
                if (vec_11.Magnitude() < TolDist + 1.0) N=2;
                else                                   N = int(ceil(vec_11.Magnitude()/TolDist));  /* anzahl der zu erzeugenden Outputwerte */
            }

            if (N<2) throw Base::Exception("attention - division by zero ... ");

            if (vec_11.Magnitude() == 0.0 && vec_21.Magnitude() == 0.0) N=0;
            if (!m_conn)
            {
                con = true;
            }

            // erster Punkt wird stets weggelassen ...
            if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())
            {
                tmp.x = m_StartPnts1[0].X();
                tmp.y = m_StartPnts1[0].Y();
                tmp.z = m_StartPnts1[0].Z();
                m_Output_robo1.push_back(tmp);
                RoboFlag_Master.push_back(0);

                tmp.x = m_StartPnts2[0].X();
                tmp.y = m_StartPnts2[0].Y();
                tmp.z = m_StartPnts2[0].Z();
                m_Output_robo2.push_back(tmp);
                RoboFlag_Slave.push_back(0);
            }

            // MASTER
            for (int i=1; i<N; ++i)
            {
                tmp.x = m_StartPnts1[0].X() + (double(i)*vec_11.X())/(double(N)-1.0);
                tmp.y = m_StartPnts1[0].Y() + (double(i)*vec_11.Y())/(double(N)-1.0);
                tmp.z = m_StartPnts1[con].Z();

                m_Output_robo1.push_back(tmp);
                RoboFlag_Master.push_back(0);
            }

            // SLAVE
            for (int i=1; i<N; ++i)
            {
                if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())
                {
                    tmp.x = m_StartPnts2[0].X() + (double(i)*vec_21.X())/(double(N)-1.0);
                    tmp.y = m_StartPnts2[0].Y() + (double(i)*vec_21.Y())/(double(N)-1.0);
                    tmp.z = m_StartPnts2[0].Z();
                }
                else
                {
                    tmp.x = m_StartPnts2[con].X();
                    tmp.y = m_StartPnts2[con].Y();
                    tmp.z = m_StartPnts2[0].Z() + (double(i)*vec_21.Y())/(double(N)-1.0);
                }
                m_Output_robo2.push_back(tmp);
                RoboFlag_Slave.push_back(0);
            }
        }
    }
    else  // still to do ....
    {
        gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);

        gp_Vec2d vec_11,vec_12;

        vec_11.SetX(vec_1.X());
        vec_11.SetY(vec_1.Y());

        if (brob == false)
        {
            ParameterCalculation(vec_11.Magnitude());

            if (vec_11.Magnitude() != 0.0) vec_11.Normalize();

            N = (int)((m_T - m_t0)/m_step);

            if (N != 0)
            {
                m_del_t = (m_T - m_t0)/N;

                for (int i=0; i<N; ++i)
                {
                    m_Output_time.push_back(t);
                    d = GetDistance(t);

                    tmp.x = vec_11.X()*(GetVelocityOld(t))[0];
                    tmp.y = vec_11.Y()*(GetVelocityOld(t))[0];
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
        }
        else
        {
            N = (int) vec_11.Magnitude();  /* anzahl der zu erzeugenden Outputwerte */

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

bool path_simulate::ConnectPaths_z(bool brob)
{
    int N;
    double t = m_t0;

    Base::Vector3d tmp;
    std::vector<double> d;
    std::vector<Base::Vector3d> tmp2;

    if (m_single == false)
    {
        gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);
        gp_Vec vec_2(m_StartPnts2[0], m_StartPnts2[1]);

        gp_Vec2d vec_11,vec_12;

        vec_11.SetX(0.0);
        vec_11.SetY(vec_1.Z());

        if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())
        {
            vec_12.SetX(0.0);
            vec_12.SetY(vec_2.Z());
        }
        else
        {
            vec_12.SetX(vec_2.X());
            vec_12.SetY(vec_2.Y());
        }

        if (brob == false)
        {

            ParameterCalculation(vec_11.Magnitude(), vec_12.Magnitude());
            if (vec_11.Magnitude() != 0.0)
                vec_11.Normalize();
            if (vec_12.Magnitude() != 0.0)
                vec_12.Normalize();

            N = (int) ceil((m_T - m_t0)/m_step);

            if ( N != 0)
            {

                m_del_t = (m_T - m_t0)/double(N);

                for (int i=0; i<N; ++i)
                {
                    m_Output_time.push_back(t);
                    m_Output_time2.push_back(t);


                    d = GetDistance(t);

                    // MASTER
                    tmp.x = 0.0;
                    tmp.y = 0.0;
                    tmp.z = vec_11.Y()*(GetVelocityOld(t))[0];    // master zustellung in Z-Richtung

                    tmp2.push_back(tmp);
                    m_Output.push_back(tmp2);
                    tmp2.clear();

                    // SLAVE
                    if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())
                    {
                        tmp.x = 0.0;
                        tmp.y = 0.0;
                        tmp.z = vec_12.Y()*(GetVelocityOld(t))[1];
                    }
                    else
                    {
                        tmp.x = vec_12.X()*(GetVelocityOld(t))[1];   // slave zustellung in XY-Richtung
                        tmp.y = vec_12.Y()*(GetVelocityOld(t))[1];
                        tmp.z = 0.0;
                    }

                    tmp2.push_back(tmp);
                    m_Output2.push_back(tmp2);
                    tmp2.clear();

                    t += m_del_t;
                }

                m_Output_time.push_back(m_T);
                m_Output_time2.push_back(m_T);

                tmp.x = 0.0;
                tmp.y = 0.0;
                tmp.z = 0.0;

                tmp2.push_back(tmp);
                m_Output.push_back(tmp2);
                m_Output2.push_back(tmp2);
            }
        }
        else
        {
            bool con = false;

            if (vec_11.Magnitude() > vec_12.Magnitude())
            {
                if (vec_12.Magnitude() < TolDist + 1.0) N=2;
                else                                   N = int(ceil(vec_12.Magnitude()/TolDist));  /* anzahl der zu erzeugenden Outputwerte */
            }
            else
            {
                if (vec_11.Magnitude() < TolDist + 1.0) N=2;
                else                                   N = int(ceil(vec_11.Magnitude()/TolDist));  /* anzahl der zu erzeugenden Outputwerte */
            }

            if (N<2)
                throw Base::Exception("attention - division by zero...");

            if (vec_11.Magnitude() == 0.0 && vec_12.Magnitude() == 0.0) N=0;

            if (m_conn)
            {
                con = true;
            }

            for (int i=1; i<N; ++i)
            {
                tmp.x = m_StartPnts1[con].X();
                tmp.y = m_StartPnts1[con].Y();
                tmp.z = m_StartPnts1[0].Z() + (double(i)*vec_11.Y())/double(N-1);

                m_Output_robo1.push_back(tmp);
                RoboFlag_Master.push_back(0);

                if (m_it1 == m_BSplineTop.begin() && m_it2 == m_BSplineBottom.begin())
                {
                    tmp.x = m_StartPnts2[1].X();
                    tmp.y = m_StartPnts2[1].Y();
                    tmp.z = m_StartPnts2[0].Z() + (double(i)*vec_12.Y())/double(N-1);
                }
                else
                {
                    tmp.x = m_StartPnts2[0].X() + (double(i)*vec_12.X())/double(N-1);
                    tmp.y = m_StartPnts2[0].Y() + (double(i)*vec_12.Y())/double(N-1);
                    tmp.z = m_StartPnts2[con].Z();
                }

                m_Output_robo2.push_back(tmp);
                RoboFlag_Slave.push_back(0);
            }
        }
    }
    else // still to do ...
    {
        gp_Vec vec_1(m_StartPnts1[0], m_StartPnts1[1]);

        gp_Vec2d vec_12;

        vec_12.SetX(0.0);
        vec_12.SetY(vec_1.Z());

        if (brob == false)
        {
            ParameterCalculation(vec_12.Magnitude());

            N = (int) ceil((m_T - m_t0)/m_step);
            m_del_t = (m_T - m_t0)/N;

            for (int i=0; i<N; ++i)
            {
                m_Output_time.push_back(t);

                tmp.x = 0.0;
                tmp.y = 0.0;
                tmp.z = -(GetVelocityOld(t))[0];

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
            N =(int) vec_12.Magnitude();  /* anzahl der zu erzeugenden Outputwerte */

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

bool path_simulate::ConnectPaths_Feat(bool tool,
                                      bool rob,
                                      bool c_typ)
{
    int N, ind;
    double rad, t;
    bool dir;

    std::vector<double> Times;
    std::vector<gp_Pnt> ConnPnts;
    std::vector< std::vector<Base::Vector3d> > Out;
    gp_Vec vec[3], vec_tmp[3];

    Base::Vector3d tmp;
    std::vector<double> d;
    std::vector<Base::Vector3d> tmp2;
    double vel;

    dir = CheckConnect(tool);  // setze starpunkte neu

    if (!tool)
    {
        ConnPnts = m_StartPnts1;
        rad = m_set.master_radius;
    }
    else
    {
        ConnPnts = m_StartPnts2;
        rad = m_set.slave_radius;
    }

    if (c_typ)
    {
        ind = 2;
        vec_tmp[0].SetCoord(0.0, 0.0, abs(ConnPnts[1].Z()-ConnPnts[0].Z()));
        vec_tmp[1].SetCoord(ConnPnts[1].X()-ConnPnts[0].X(), ConnPnts[1].Y()-ConnPnts[0].Y(), 0.0);

        if (dir) // tool  (Master/Slave) muss runter fahren
        {
            if (!tool)
            {
                vec[0] =  vec_tmp[1];    // tool = Master
                vec[1] = -vec_tmp[0];
            }
            else
            {
                vec[0] = -vec_tmp[0];    // tool = Slave
                vec[1] =  vec_tmp[1];
            }
        }
        else   // tool  (Master/Slave) muss hoch fahren
        {
            if (!tool)
            {
                vec[0] = vec_tmp[0];    // tool = Master
                vec[1] = vec_tmp[1];
            }
            else
            {
                vec[0] = vec_tmp[1];    // tool = Slave
                vec[1] = vec_tmp[0];
            }
        }

    }
    else
    {
        ind = 3;

        vec_tmp[0].SetCoord(0.0, 0.0, rad);
        vec_tmp[1].SetCoord(ConnPnts[1].X()-ConnPnts[0].X(), ConnPnts[1].Y()-ConnPnts[0].Y(), 0.0);
        vec_tmp[2].SetCoord(0.0, 0.0, abs(ConnPnts[1].Z()-ConnPnts[0].Z()) + rad);

        if (dir) // tool  (Master/Slave) muss runter fahren
        {
            if (!tool)
            {
                vec[0] =  vec_tmp[0];    // tool = Master
                vec[1] =  vec_tmp[1];
                vec[2] = -vec_tmp[2];
            }
            else
            {
                vec[0] = -vec_tmp[2];    // tool = Slave
                vec[1] =  vec_tmp[1];
                vec[2] =  vec_tmp[0];
            }
        }
        else   // tool  (Master/Slave) muss hoch fahren
        {
            if (!tool)
            {
                vec[0] =  vec_tmp[2];    // tool = Master
                vec[1] =  vec_tmp[1];
                vec[2] = -vec_tmp[0];
            }
            else
            {
                vec[0] = -vec_tmp[0];    // tool = Slave
                vec[1] =  vec_tmp[1];
                vec[2] =  vec_tmp[2];
            }
        }
    }

    if (rob)   goto robo;

    for (int i=0; i<ind; ++i)
    {
        t = m_t0;
        ParameterCalculation(vec[i].Magnitude());
        if (vec[i].Magnitude() != 0.0) vec[i].Normalize();
        else continue;

        N =(int) ceil((m_T - m_t0)/m_step);

        if ( N != 0)
        {
            m_del_t = (m_T - m_t0)/double(N);

            for (int j=0; j<N; ++j)
            {
                tmp2.clear();

                Times.push_back(t);
                d = GetDistance(t);
                vel = (GetVelocityOld(t))[0];

                tmp.x = vec[i].X()*vel;
                tmp.y = vec[i].Y()*vel;
                tmp.z = vec[i].Z()*vel;

                tmp2.push_back(tmp);
                Out.push_back(tmp2);  // Output

                t += m_del_t;
            }
        }
        else throw Base::Exception("Endzeit entspricht wohl der Startzeit ...");

        m_t0 = m_T; /* endzeit des letzten durchlaufs wird zur neuen startzeit */
        m_v1 = m_vmax;
        m_a1 = m_amax;
        m_c1[0] = m_a1/2;
        m_c1[1] = PI*m_a1/m_v1;
    }

    Times.push_back(m_T);

    tmp.x = 0.0;
    tmp.y = 0.0;
    tmp.z = 0.0;

    tmp2.clear();
    tmp2.push_back(tmp);
    Out.push_back(tmp2);

    if (!tool)
    {
        m_Output      = Out;
        m_Output_time = Times;
    }
    else
    {
        m_Output2      = Out;
        m_Output_time2 = Times;
    }

    return true;

robo:
    for (int i=0; i<ind; ++i)
    {
        N = int( ceil(vec[i].Magnitude()/TolDist) );  /* anzahl der zu erzeugenden Outputwerte */

        if (N==1)
            N=2;

        for (int j=1; j<N; ++j)
        {
            tmp.x = ConnPnts[0].X() + (double(j)*vec[i].X())/(double(N)-1.0);
            tmp.y = ConnPnts[0].Y() + (double(j)*vec[i].Y())/(double(N)-1.0);
            tmp.z = ConnPnts[0].Z() + (double(j)*vec[i].Z())/(double(N)-1.0);;

            if (!tool) m_Output_robo1.push_back(tmp); // master
            else      m_Output_robo2.push_back(tmp);  // slave
        }

        if (N!=0) ConnPnts[0].SetCoord(tmp.x , tmp.y, tmp.z);
    }

    return true;
}

std::vector<std::vector<double> > path_simulate::CompBounds(bool tool) //true = Slave, false = Master
{
    int step = (int) ceil(m_vmax*8e-3);  // schrittweite welche bei maximaler geschwindigkeit m_vmax in bogenmass vorkommen kann (*8)
    GeomAdaptor_Curve curve;
    std::vector<double> hPnts;
    std::vector<double> bounds;
    std::vector<std::vector<double> > hPntsInt;
    std::vector<std::vector<double> > CriticalBounds;

    std::vector<std::vector<Base::Vector3d> > D2;
    std::vector<std::vector<double> > Cr;

    // lade aktuelle kurve
    if (!tool)  curve.Load(*m_it1);
    else        curve.Load(*m_it2);

    double fParam = curve.FirstParameter(),
                    lParam = curve.LastParameter();

    double period = lParam - fParam;

    std::vector<std::vector<Base::Vector3d> > D1;
    std::vector<Base::Vector3d> tmp3;
    Base::Vector3d tmp2;
    gp_Pnt tmp;
    gp_Vec dtmp1;

    int N = (int) ceil(period);

    // Compute 2.Deriavative
    for (int i=0; i<N; ++i)
    {
        //foundParameter = FindParamAt(curve1, d[0], startParam[0]);     /* d[0] - master tool, d[1] - supporting die */

        if      ( m_StartParam[tool] + i > lParam  )curve.D1(m_StartParam[tool] + i - period, tmp, dtmp1);
        else if ( m_StartParam[tool] + i < fParam ) curve.D1(m_StartParam[tool] + i + period, tmp, dtmp1);
        else                                        curve.D1(m_StartParam[tool] + i         , tmp, dtmp1);


        /* erste ableitung */
        tmp2.x = dtmp1.X();
        tmp2.y = dtmp1.Y();
        tmp2.z = dtmp1.Z();

        tmp3.push_back(tmp2);
    }

    D1.push_back(tmp3);
    Cr = CurvCurvature(D1);
    tmp3.clear();

    // Get Point-Parameters with high Curvature
    for (int i=0; i<N; ++i)
    {
        if (Cr[0][i] < curvTOL)
        {
            hPnts.push_back(i);
        }
    }

    // füge zusammenhängende Punkte zu Bereichen zusammen
    std::vector<double> tmpo;
    std::vector<double> tmpo2Push;

    Base::Builder3D log;

    if (hPnts.size() > 2)
    {
        tmpo.push_back(hPnts[0]);
        tmpo2Push.push_back(hPnts[0]);

        for (unsigned int i=0; i<hPnts.size()-1; ++i)
        {
            if ( (hPnts[i+1] - tmpo2Push[i]) < (period/10.0))
            {
                tmpo.push_back(hPnts[i+1]);
                tmpo2Push.push_back(hPnts[i+1]);
            }
            else
            {
                hPntsInt.push_back(tmpo);
                tmpo.clear();
                tmpo.push_back(hPnts[i+1]);
                tmpo2Push.push_back(hPnts[i+1]);
            }
        }

        tmpo2Push.clear();

        if (tmpo.size() > 0)
        {
            hPntsInt.push_back(tmpo);
        }
        else
            throw Base::Exception("ooops");

        // suche die Bereichsgrenzen
        for (unsigned int i=0; i<hPntsInt.size(); ++i)
        {
            bounds.push_back(hPntsInt[i][0]);
            bounds.push_back(hPntsInt[i][hPntsInt[i].size()-1]);
            CriticalBounds.push_back(bounds);
            bounds.clear();
        }
    }

	if( CriticalBounds.size() > 0)
	{
		for(int i=0; i<CriticalBounds.size(); ++i)
		{
		if(CriticalBounds[i][0] < (boundTOL + step + 1.0))
			CriticalBounds[i][0] = boundTOL + step + 1.0;
		
		if(CriticalBounds[i][1] < (boundTOL + step + 1.0))
			CriticalBounds[i][1] = boundTOL + step + 1.0;
		
		if(CriticalBounds[i][1] > (lParam - boundTOL - step - 1.0))
			CriticalBounds[i][1] = (lParam - boundTOL - step - 1.0);

		if(CriticalBounds[i][0] > (lParam - boundTOL - step - 1.0))
			CriticalBounds[i][0] = (lParam - boundTOL - step - 1.0);
		}
	}

	std::vector<double> tmpos1;
	std::vector<std::vector<double> > tmppos;
	std::vector<std::vector<double> > bndEnd;

	int ind = 0;
	int j = 0;
	while((ind+1)<CriticalBounds.size())
	{
		j = ind;
		while(j<(CriticalBounds.size()-1))
		{
			if((CriticalBounds[j+1][0] - CriticalBounds[j][1])< (2*boundTOL + step + 1.0))
			{
				j++;
			}
			else
				break;
		}

		tmpos1.push_back(CriticalBounds[ind][0]);
	    tmpos1.push_back(CriticalBounds[j][1]);
		tmppos.push_back(tmpos1);
		tmpos1.clear();

		ind = j+1;

	}

	if(j==CriticalBounds.size()-2 && ind > 0)
	{
		tmpos1.push_back(CriticalBounds[j+1][0]);
	    tmpos1.push_back(CriticalBounds[j+1][1]);
		tmppos.push_back(tmpos1);
		tmpos1.clear();
	}

	if(tmppos.size() != 0)
		CriticalBounds = tmppos;




    if (CriticalBounds.size() == 1)
    {
        double len = 0.0;
        len += CriticalBounds[0][0] + m_StartParam[tool];

        if (CriticalBounds[0][0] < boundTOL + step + 1.0)
		{
			CriticalBounds[0][0] = boundTOL + step + 1.0;
		}

		if(CriticalBounds[0][1] < boundTOL + step + 1.0)
		{
			CriticalBounds[0][1] = boundTOL + step + 1.0;
		}


        if (lParam < (m_StartParam[0] + CriticalBounds[0][1]))
            len = CriticalBounds[0][1] + m_StartParam[0] - lParam;
        else
            len = lParam - CriticalBounds[0][1];

        if (len < boundTOL)
            CriticalBounds[0][1] = m_StartParam[0];

        if (CriticalBounds[0][0] == CriticalBounds[0][1])
            CriticalBounds.clear();
    }

    return CriticalBounds;
}

bool path_simulate::CompPath(bool tool) // tool = 0  -> Master
// tool = 1  -> Slave
{
	int step = (int) ceil(m_vmax*8e-3);  // schrittweite für die Abschätzung der max_velocity
   
	// berechnet auf Basis der aktuellen Kurve die kritischen Bereiche im Parameterraum
    std::vector<std::vector<double> > CriticalBounds = CompBounds(tool);

	double cur = 0.1;
	m_amax = 2000;
   
    GeomAdaptor_Curve curve;

    gp_Pnt pnt0;
    gp_Vec pnt1;
	gp_Vec pnt2;
	gp_Vec pnt;
    Base::Vector3d Pnt1;
    std::vector<Base::Vector3d> Pnt1Vec;

    // lade aktuelle kurve
    if (!tool)  curve.Load(*m_it1);
    else       curve.Load(*m_it2);

    double fParam = curve.FirstParameter(),
                    lParam = curve.LastParameter();

    double period = lParam - fParam;
    double D1_Max[3], D1_Min[3], d2, length, velo, tetha, S;

    // Iniitialisierung für die Nachkorrektur
    m_Sum[0] = 0.0;
    m_Sum[1] = 0.0;
    m_Sum[2] = 0.0;

    double t0 = m_t0;
    int start = (int) m_StartParam[tool];

    m_v[0] = 0.0;  // starte immer mit v = 0
	d2 = 0.0;
    for (unsigned int i=0; i<CriticalBounds.size(); ++i)
    {

		S = CriticalBounds[i][0] - boundTOL + start - m_StartParam[tool];
		length = CriticalBounds[i][0] - boundTOL - m_StartParam[tool] + start;
        // gerader Bereich
        for (int j=(int) m_StartParam[tool]; j<((int) ceil(CriticalBounds[i][0] - boundTOL) + start); ++j)
        {

            if      ( j > lParam )  curve.D2(j - period, pnt0, pnt1, pnt2);
            else if ( j < fParam )  curve.D2(j + period, pnt0, pnt1, pnt2);
            else                    curve.D2(j         , pnt0, pnt1, pnt2);

            Pnt1.x = pnt2.X();
            Pnt1.y = pnt2.Y();
            Pnt1.z = pnt2.Z();

			if(d2 < Pnt1.Length()) d2 = Pnt1.Length();
        }



		tetha  = min(1,((m_vmax)*(sqrt(cur)/sqrt(m_amax))));
		tetha  = tetha*3/4;
		velo   = tetha*(sqrt(m_amax/cur));
		m_a    = m_amax*(1 - tetha*tetha);
		m_v[1] = velo;

		d2 = 0;
        
		double st = start + CriticalBounds[i][0] - boundTOL;
		double delta = st/1000;
		// kritischer Abschnitt
        while(st <  start + CriticalBounds[i][1] + boundTOL)
        {

            if      ( st > lParam )  curve.D2(st - period, pnt0, pnt1, pnt2);
            else if ( st < fParam )  curve.D2(st + period, pnt0, pnt1, pnt2);
            else                     curve.D2(st         , pnt0, pnt1, pnt2);

            Pnt1.x = pnt2.X();
            Pnt1.y = pnt2.Y();
            Pnt1.z = pnt2.Z();

			pnt = pnt1;
			pnt1.Cross(pnt2);
			double tmp = (pnt1.Magnitude()/(pow(pnt.Magnitude(),3)));
 
			if(d2 < Pnt1.Length())
			{
				velo = tmp;
				d2 = Pnt1.Length();
			}
			st += delta;
        }

		velo   = tetha*(sqrt(m_amax/d2));
		d2 = 0;

		if(velo < m_v[0] && m_v[0] != 0) velo = m_v[0];

		if(S < (4*PI*m_v[1] - 2*PI*(m_v[0] + velo))/m_a)
		{
			m_v[1] = (S*m_a/(4*PI)) + ((m_v[0] + velo)/(2));

			while(m_v[1] < velo) 
			{
				velo /= 2;
				m_v[1] = (S*m_a/(4*PI)) + ((m_v[0] + velo)/(2));

				if(velo < m_v[0])
				{
					velo = m_v[0];
					m_v[1] = (S*m_a/(4*PI)) + ((m_v[0] + velo)/(2));
					break;
				}
			}
		}

		velo = 60;
        m_v[2] = velo;

        // gerader Bereich
        length = CriticalBounds[i][0] - boundTOL - m_StartParam[tool] + start;
        MakeSinglePathNew(1, length, 0,tool); // erzeuge Output
        m_t0 = m_T;
        m_StartParam[tool] += length;

        // gekrümmter Bereich
        length = CriticalBounds[i][1] - CriticalBounds[i][0] + 2*boundTOL;
        MakeSinglePathNew(1, length, 1,tool); // erzeuge Output
        m_t0 = m_T;
        m_StartParam[tool] += length;

        m_v[0] = m_v[2];  // Endgeschwindigkeit wird zur Startgeschwindigkeit
    }

    // letzter nicht-kritischer Abschnitt
	if(m_StartParam[tool] < lParam-1e-4 || m_StartParam[tool] > lParam+1e-4)
	{
		S = lParam + start - m_StartParam[tool];
		d2 = 0;
		for (int j = (int) m_StartParam[tool]; j<int(lParam + start); ++j)
		{

            if      ( j > lParam )  curve.D2(j - period, pnt0, pnt1, pnt2);
            else if ( j < fParam )  curve.D2(j + period, pnt0, pnt1, pnt2);
            else                    curve.D2(j         , pnt0, pnt1, pnt2);

            Pnt1.x = pnt2.X();
            Pnt1.y = pnt2.Y();
            Pnt1.z = pnt2.Z();
 
			if(d2 < Pnt1.Length()) d2 = Pnt1.Length();
        
		}

    

		tetha  = min(1,((m_vmax)*(sqrt(cur)/sqrt(m_amax))));
		tetha  = tetha*3/4;
		velo   = tetha*(sqrt(m_amax/cur));
		m_a    = m_amax*(1 - tetha*tetha);
		m_v[1] = velo;
		m_v[2] = 0.0;
   
		d2 = 0;
		length = lParam - m_StartParam[tool] + start;
		MakeSinglePathNew(1, length, 0,tool); // erzeuge Output für den letzen Abschnitt
		m_t0 = m_T;
	}

    // Korrektur des Weges
    Integrate(tool);
    Correction(tool);
    Integrate(tool);

    m_t0 = t0;

    m_StartParam[tool] = start;
    CriticalBounds.clear();

    return true;
}


bool path_simulate::Integrate(bool b)
{
    m_Sum[0] = 0.0;
    m_Sum[1] = 0.0;
    m_Sum[2] = 0.0;

    if (b==false)
    {

        for (unsigned int i=1; i<m_Output.size(); ++i)
        {
            m_Sum[0] += (m_Output[i][0].x + m_Output[i-1][0].x)*(m_Output_time[i] - m_Output_time[i-1]) / 2.0;
            m_Sum[1] += (m_Output[i][0].y + m_Output[i-1][0].y)*(m_Output_time[i] - m_Output_time[i-1]) / 2.0;
            m_Sum[2] += (m_Output[i][0].z + m_Output[i-1][0].z)*(m_Output_time[i] - m_Output_time[i-1]) / 2.0;
        }
    }
    else
    {

        for (unsigned int i=1; i<m_Output2.size(); ++i)
        {
            m_Sum[0] += (m_Output2[i][0].x + m_Output2[i-1][0].x)*(m_Output_time2[i] - m_Output_time2[i-1]) / 2.0;
            m_Sum[1] += (m_Output2[i][0].y + m_Output2[i-1][0].y)*(m_Output_time2[i] - m_Output_time2[i-1]) / 2.0;
            m_Sum[2] += (m_Output2[i][0].z + m_Output2[i-1][0].z)*(m_Output_time2[i] - m_Output_time2[i-1]) / 2.0;
        }
    }

    return true;
}

bool path_simulate::Correction(bool b)
{
    int N;
    gp_Vec vec;
    Base::Vector3d tmp;
    std::vector<Base::Vector3d> tmp2;
    vec.SetCoord(-m_Sum[0],-m_Sum[1],-m_Sum[2]);

    ParameterCalculation(vec.Magnitude());

    N = (int) ceil((m_T - m_t0)/m_step);  /* anzahl der zu erzeugenden Outputwerte */
    m_del_t = (m_T - m_t0)/N;

    if (N==1)
    {
        m_T = m_t0 + 2e-3;

        if (b==false)
        {
            m_Output_time.push_back(m_t0);
            m_Output_time.push_back(m_t0 + 1e-3);
        }
        else
        {
            m_Output_time2.push_back(m_t0);
            m_Output_time2.push_back(m_t0 + 1e-3);
        }

        tmp.x = 0.0;
        tmp.y = 0.0;
        tmp.z = 0.0;

        tmp2.push_back(tmp);

        if (b==false)
            m_Output.push_back(tmp2);
        else
            m_Output2.push_back(tmp2);

        tmp2.clear();


        tmp.x = (vec.X()/vec.Magnitude())*(vec.X()/1e-3);
        tmp.y = (vec.Y()/vec.Magnitude())*(vec.Y()/1e-3);
        tmp.z = (vec.Z()/vec.Magnitude())*(vec.Z()/1e-3);

        tmp2.push_back(tmp);

        if (b==false)
            m_Output.push_back(tmp2);
        else
            m_Output2.push_back(tmp2);

        tmp2.clear();
    }
    else
    {

        double t = m_t0;
        if (vec.Magnitude() != 0) vec.Normalize();

        for (int i=0; i<N; ++i)
        {
            if (b==false)
                m_Output_time.push_back(t);
            else
                m_Output_time2.push_back(t);


            // MASTER
            tmp.x = vec.X()*(GetVelocityOld(t))[0];
            tmp.y = vec.Y()*(GetVelocityOld(t))[0];
            tmp.z = vec.Z()*(GetVelocityOld(t))[0];

            tmp2.push_back(tmp);
            if (b==false)
                m_Output.push_back(tmp2);
            else
                m_Output2.push_back(tmp2);


            t += m_del_t;
            tmp2.clear();
        }
    }

    if (b==false)
        m_Output_time.push_back(m_T);
    else
        m_Output_time2.push_back(m_T);


    tmp.x = 0.0;
    tmp.y = 0.0;
    tmp.z = 0.0;

    tmp2.push_back(tmp);

    if (b==false)
        m_Output.push_back(tmp2);
    else
        m_Output2.push_back(tmp2);


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

    anAdaptorCurve.Load(*m_it1);
    double w1; // = GetLength(anAdaptorCurve, anAdaptorCurve.FirstParameter(), anAdaptorCurve.LastParameter());
    double w2;

    firstParam1 = anAdaptorCurve.FirstParameter();
    lastParam1  = anAdaptorCurve.LastParameter();
    period1     = lastParam1 - firstParam1;

    w1 = period1;

    int N;

    if (brob == true) goto robo;

    /* ---------- BERECHNUNG DER PARAMETER ---------------------------------------- */
    if (m_single==false)
    {
        b = true;
        anAdaptorCurve2.Load(*m_it2);
        firstParam2 = anAdaptorCurve2.FirstParameter();
        lastParam2  = anAdaptorCurve2.LastParameter();
        period2     = lastParam2 - firstParam2;

        w2 = period2; //GetLength(anAdaptorCurve2, firstParam2, lastParam2);
        ParameterCalculation(w1,w2);
    }
    else
    {
        ParameterCalculation(w1);
    }

    N = (int)((m_T-m_t0)/m_step);
    m_del_t = (m_T-m_t0)/N;

    if (N>=100000)
        N = 99999;

    double t = m_t0;
    /* ------------------------------------------------------------- */

    if (m_single == true)
    {
        if (brob == false)
        {
            for (int i=0; i<N; ++i)
            {
                m_Output_time.push_back(t);
                d = GetDistance(t);
                d[0] = FindParamAt(anAdaptorCurve, d[0], m_StartParam[0]);

                if (m_it1 != m_BSplineTop.begin() && m_dir == 1)
                    d[0] = -d[0];   /* führe Richtungsänderung durch */

                if ( m_StartParam[0] + d[0] > lastParam1 )
                {
                    anAdaptorCurve.D1(m_StartParam[0] + d[0] - period1, tmp, dtmp1);
                }
                else if ( m_StartParam[0] + d[0] < firstParam1 )
                {
                    anAdaptorCurve.D1(m_StartParam[0] + d[0] + period1, tmp, dtmp1);
                }
                else
                {
                    anAdaptorCurve.D1(m_StartParam[0] + d[0], tmp, dtmp1);
                }

                if (m_it1 != m_BSplineTop.begin() && m_dir == 1)
                    dtmp1 = -dtmp1;

                dtmp1.Normalize();

                tmp2.x = dtmp1.X()*(GetVelocityOld(t)[0]);
                tmp2.y = dtmp1.Y()*(GetVelocityOld(t)[0]);
                tmp2.z = dtmp1.Z()*(GetVelocityOld(t)[0]);

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

            N = (int)w1;

            for (int i=0; i<N; ++i)
            {
                anAdaptorCurve.D0(firstParam1 + double(i)*w1/(double(N)-1.0), tmp);

                tmp2.x = tmp.X();
                tmp2.y = tmp.Y();
                tmp2.z = tmp.Z();

                m_Output_robo1.push_back(tmp2);
            }
        }
    }
    else
    {
        if (brob == false)
        {
            for (int i=0; i<N; ++i)
            {
                m_Output_time.push_back(t);
                d = GetDistance(t);

                /*d = FindParamAt(anAdaptorCurve, d, m_StartParam[0]);*/

                if (m_it1 != m_BSplineTop.begin() && m_dir == 1)
                {
                    d[0] = -d[0];   /* führe Richtungsänderung durch */
                    d[1] = -d[1];
                }

                if ( m_StartParam[0] + d[0] > lastParam1 )
                {
                    anAdaptorCurve.D1(m_StartParam[0] + d[0] - period1, tmp, dtmp1);
                }
                else if ( m_StartParam[0] + d[0] < firstParam1 )
                {
                    anAdaptorCurve.D1(m_StartParam[0] + d[0] + period1, tmp, dtmp1);
                }
                else
                {
                    anAdaptorCurve.D1(m_StartParam[0] + d[0], tmp, dtmp1);
                }



                if (m_it1 != m_BSplineTop.begin() && m_dir == 1)
                    dtmp1 = -dtmp1;

                dtmp1.Normalize();

                tmp2.x = dtmp1.X()*(GetVelocity(t));
                tmp2.y = dtmp1.Y()*(GetVelocity(t));
                tmp2.z = dtmp1.Z()*(GetVelocity(t));

                tmp3.push_back(tmp2);


                if ( m_StartParam[1] + d[1] > lastParam2 )
                {
                    anAdaptorCurve2.D1(m_StartParam[1] + d[1] - period2, tmp, dtmp1);
                }
                else if ( m_StartParam[1] + d[1]< firstParam2)
                {
                    anAdaptorCurve2.D1(m_StartParam[1] + d[1] + period2, tmp, dtmp1);
                }
                else
                {
                    anAdaptorCurve2.D1(m_StartParam[1] + d[1], tmp, dtmp1);
                }

                if (m_it1 != m_BSplineTop.begin() && m_dir == 1)
                    dtmp1 = -dtmp1;

                dtmp1.Normalize();

                tmp2.x = dtmp1.X()*(GetVelocity(t));
                tmp2.y = dtmp1.Y()*(GetVelocity(t));
                tmp2.z = dtmp1.Z()*(GetVelocity(t));

                tmp3.push_back(tmp2);



                if (m_it1==m_BSplineTop.begin()+30)
                {
                    //               m_log3d.addSinglePoint(pit1.x,pit1.y,pit1.z);
                    //               m_log3d.addSinglePoint(pit2.x,pit2.y,pit2.z);
                    //m_log3d.addSingleArrow(Base::Vector3f(pit1.x,pit1.y,pit1.z),Base::Vector3f(pit2.x,pit2.y,pit2.z),1);
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
robo:
            anAdaptorCurve2.Load(*m_it2);
            firstParam2 = anAdaptorCurve2.FirstParameter();
            lastParam2  = anAdaptorCurve2.LastParameter();
            period2     = lastParam2 - firstParam2;

            w2 = period2; //GetLength(anAdaptorCurve2, firstParam2, lastParam2);

            if (w1<w2) N = (int) ceil(w1 / double(TolDist));
            else      N = (int) ceil(w2 / double(TolDist));

            if (N<2)
                throw Base::Exception("thats new ...");

            for (int i=1; i<N; ++i)  // niemals den ersten punkt mitnehmen
            {
                if (m_StartParam[0] + double(i)*w1/(double(N)-1.0) > lastParam1)
                {
                    anAdaptorCurve.D0(m_StartParam[0] + double(i)*w1/(double(N)-1.0) - period1, tmp);
                }
                else if (m_StartParam[0] + double(i)*w1/(double(N)-1) < firstParam1 )
                {
                    anAdaptorCurve.D0(m_StartParam[0] + double(i)*w1/(double(N)-1.0) + period1, tmp);
                }
                else
                {
                    anAdaptorCurve.D0(m_StartParam[0] + double(i)*w1/(double(N)-1.0), tmp);
                }

                tmp2.x = tmp.X();
                tmp2.y = tmp.Y();
                tmp2.z = tmp.Z();

                m_Output_robo1.push_back(tmp2);

                if (i==1)
                {
                    RoboFlag_Master.pop_back();
                    RoboFlag_Master.push_back(1);
                }

                RoboFlag_Master.push_back(0);

                if ( m_StartParam[1] + double(i)*w2/(double(N)-1.0) > lastParam2 )
                    anAdaptorCurve2.D0(m_StartParam[1] + double(i)*w2/(double(N)-1.0) - period2, tmp);
                else if ( m_StartParam[1] + double(i)*w2/(double(N)-1.0) < firstParam2 )
                    anAdaptorCurve2.D0(m_StartParam[1] + double(i)*w2/(double(N)-1.0) + period2, tmp);
                else
                    anAdaptorCurve2.D0(m_StartParam[1] + double(i)*w2/(double(N)-1.0), tmp);

                tmp2.x = tmp.X();
                tmp2.y = tmp.Y();
                tmp2.z = tmp.Z();

                m_Output_robo2.push_back(tmp2);

                if (i==1)
                {
                    RoboFlag_Slave.pop_back();
                    RoboFlag_Slave.push_back(1);
                }

                RoboFlag_Slave.push_back(0);
            }
        }
    }

    return true;
}

bool path_simulate::MakeSingleRoboPath(bool tool)
{
    GeomAdaptor_Curve anAdaptorCurve;
    double firstParam,lastParam,period;
    std::vector<double> d;
    gp_Pnt tmp;
    gp_Vec dtmp1;
    Base::Vector3d tmp2;
    std::vector<Base::Vector3d> tmp3;
    bool b = false;

    switch (tool)
    {
    case 0:  // master
        anAdaptorCurve.Load(*m_it1);
        break;
    case 1:  // slave
        anAdaptorCurve.Load(*m_it2);
        break;
    }

    firstParam = anAdaptorCurve.FirstParameter();
    lastParam  = anAdaptorCurve.LastParameter();
    period     = lastParam - firstParam;

    double w = period;   //GetLength(anAdaptorCurve, firstParam, lastParam);
    int N = (int) ceil(w / double(TolDist));

    if (N<2)
        throw Base::Exception("thats new ...");

    for (int i=1; i<N; ++i)  // niemals den ersten punkt mitnehmen
    {
        if (m_StartParam[tool] + double(i)*w/(double(N)-1.0) > lastParam)
        {
            anAdaptorCurve.D0(m_StartParam[tool] + double(i)*w/(double(N)-1.0) - period, tmp);
        }
        else if (m_StartParam[tool] + double(i)*w/(double(N)-1) < firstParam )
        {
            anAdaptorCurve.D0(m_StartParam[tool] + double(i)*w/(double(N)-1.0) + period, tmp);
        }
        else
        {
            anAdaptorCurve.D0(m_StartParam[tool] + double(i)*w/(double(N)-1.0), tmp);
        }

        tmp2.x = tmp.X();
        tmp2.y = tmp.Y();
        tmp2.z = tmp.Z();

        if (!tool)
        {
            m_Output_robo1.push_back(tmp2);

            if (i==1)
            {
                RoboFlag_Master.pop_back();
                RoboFlag_Master.push_back(1);
            }

            RoboFlag_Master.push_back(0);
        }
        else
        {
            m_Output_robo2.push_back(tmp2);

            if (i==1)
            {
                RoboFlag_Slave.pop_back();
                RoboFlag_Slave.push_back(1);
            }

            RoboFlag_Slave.push_back(0);
        }
    }

    return true;
}

bool path_simulate::MakeSinglePathNew(bool brob, double length, bool part, bool type)
{
    GeomAdaptor_Curve anAdaptorCurve;
    double firstParam,lastParam,period,d;
    int N;

    if (type == false) anAdaptorCurve.Load(*m_it1); // Master
    else              anAdaptorCurve.Load(*m_it2); // Slave

    firstParam = anAdaptorCurve.FirstParameter();
    lastParam  = anAdaptorCurve.LastParameter();
    period     = lastParam - firstParam;

    gp_Vec                      dtmp1;
    gp_Pnt                      tmp;
    Base::Vector3d              tmp2;
    std::vector<Base::Vector3d> tmp3;

    if (part ==true) m_T = m_t0 + length/m_v[2]; // kritische Bereiche werden mit konstanter Geschwindigkeit durchlaufen
    else            ParameterCalculationNew(length);

    N = (int)((m_T-m_t0)/m_step);
    m_del_t = (m_T-m_t0)/double(N);

    if (N==0)
        throw Base::Exception("stimmt was nicht...");

    if (N>=100000)
        N = 99999;

    std::vector< std::vector<Base::Vector3d> > D0;
    std::vector< std::vector<Base::Vector3d> > D1;

    double t = m_t0;

    // für den kritischen bereich
    if (part == true)
    {

        for (int i=0; i<N; ++i)
        {

            if (!type) m_Output_time.push_back(t);
            else      m_Output_time2.push_back(t);

            d = m_v[2]*(t-m_t0);

            if ( m_StartParam[type] + d > lastParam )
            {
                anAdaptorCurve.D1(m_StartParam[type] + d - period, tmp, dtmp1);
            }
            else if ( m_StartParam[type] + d < firstParam )
            {
                anAdaptorCurve.D1(m_StartParam[type] + d + period, tmp, dtmp1);
            }
            else
            {
                anAdaptorCurve.D1(m_StartParam[type] + d, tmp, dtmp1);
            }

            dtmp1.Normalize();

            if (dtmp1.X() == 1.0)
            {
                dtmp1.SetY(0.0);
                dtmp1.SetZ(0.0);
            }
            else if (dtmp1.Y() == 1.0)
            {
                dtmp1.SetX(0.0);
                dtmp1.SetZ(0.0);
            }
            else if (dtmp1.Z() == 1.0)
            {
                dtmp1.SetX(0.0);
                dtmp1.SetY(0.0);
            }

			times_tmp.push_back(t);
		    velo_tmp.push_back(m_v[2]);

            tmp2.x = dtmp1.X()*m_v[2];
            tmp2.y = dtmp1.Y()*m_v[2];
            tmp2.z = dtmp1.Z()*m_v[2];

            tmp3.push_back(tmp2);
            if (!type) m_Output.push_back(tmp3);
            else      m_Output2.push_back(tmp3);
            tmp3.clear();

            t += m_del_t;
        }
        return true;
    }

    // unkritischer bereich
    for (int i=0; i<N; ++i)
    {
        if (!type) m_Output_time.push_back(t);
        else      m_Output_time2.push_back(t);
        d = GetDistanceNew(t);

        if ( m_StartParam[type] + d > lastParam )
        {
            anAdaptorCurve.D1(m_StartParam[type] + d - period, tmp, dtmp1);
        }
        else if ( m_StartParam[type] + d < firstParam )
        {
            anAdaptorCurve.D1(m_StartParam[type] + d + period, tmp, dtmp1);
        }
        else
        {
            anAdaptorCurve.D1(m_StartParam[type] + d, tmp, dtmp1);
        }

        dtmp1.Normalize();

        // fange ungenauigkeiten ab ...
        if (dtmp1.X() == 1.0)
        {
            dtmp1.SetY(0.0);
            dtmp1.SetZ(0.0);
        }
        else if (dtmp1.Y() == 1.0)
        {
            dtmp1.SetX(0.0);
            dtmp1.SetZ(0.0);
        }
        else if (dtmp1.Z() == 1.0)
        {
            dtmp1.SetX(0.0);
            dtmp1.SetY(0.0);
        }

		times_tmp.push_back(t);
		velo_tmp.push_back((GetVelocity(t)));

        tmp2.x = dtmp1.X()*(GetVelocity(t));
        tmp2.y = dtmp1.Y()*(GetVelocity(t));
        tmp2.z = dtmp1.Z()*(GetVelocity(t));

        tmp3.push_back(tmp2);
        if (!type) m_Output.push_back(tmp3);
        else      m_Output2.push_back(tmp3);
        tmp3.clear();

        t += m_del_t;
    }
    return true;
}


bool path_simulate::MakePathRobot()
{
    int c=1;

    ofstream anOutputFile;
    ofstream anOutputFile2;

    anOutputFile.open("output_master.k");
    anOutputFile.precision(7);

    if (m_single == false)
    {
        anOutputFile2.open("output_slave.k");
        anOutputFile2.precision(7);
    }

    anOutputFile  << "none" << std::endl;
    anOutputFile2 << "none" << std::endl;

    for (m_it1 = m_BSplineTop.begin(); m_it1 != m_BSplineTop.end(); ++m_it1)
    {
        m_StartParam[0] = ((*m_it1)->FirstParameter());
        if (m_single == false) m_StartParam[1] = ((*m_it2)->FirstParameter());

        /**************************************** ZUSTELLUNG 1 **********************************************/
        m_conn = CheckConnect();

        if (m_conn)   ConnectPaths_xy(1);
        else    ConnectPaths_z(1);

        UpdateParam();
        /****************************************/

        /**************************************** ZUSTELLUNG 2 **********************************************/
        if (m_conn)   ConnectPaths_z(1);
        else    ConnectPaths_xy(1);

        UpdateParam();
        /****************************************/

        /****************************************** KURVE ***************************************************/
        MakeSingleRoboPath(0);
        MakeSingleRoboPath(1);
        UpdateParam();
        /****************************************/

        if (m_single==false && (m_it1 != (m_BSplineTop.end()-1)))
            ++m_it2;

    }

    WriteOutput(anOutputFile,anOutputFile2,c,1);
    anOutputFile  << "*END" << endl;
    anOutputFile.close();
    anOutputFile2 << "*END" << endl;
    anOutputFile2.close();

    return true;
}

bool path_simulate::TimeCorrection()
{
    if (m_single == false)
    {

        if (m_Output_time.size() == 0 || m_Output_time2.size() == 0)
            throw Base::Exception("output-time-vector is NULL");

        if (m_Output_time[m_Output_time.size()-1] < m_Output_time2[m_Output_time2.size()-1])
        {
            m_T = m_Output_time2[m_Output_time2.size()-1];

            int N = int(ceil((m_Output_time2[m_Output_time2.size()-1] - m_Output_time[m_Output_time.size()-1])/ m_step));
            m_del_t = (m_Output_time2[m_Output_time2.size()-1] - m_Output_time[m_Output_time.size()-1])/double(N);

            int ind = m_Output_time.size()-1;
            double time = 0.0;
            Base::Vector3d vec(0.0,0.0,0.0);
            std::vector<Base::Vector3d> vecc;
            vecc.push_back(vec);

            for (int i=1; i<N; ++i)
            {
                time += m_del_t;
                m_Output_time.push_back(m_Output_time[ind] + time);
                m_Output.push_back(vecc);
            }

            m_Output_time.push_back(m_Output_time2[m_Output_time2.size()-1]);
            m_Output.push_back(vecc);
        }
        else if (m_Output_time[m_Output_time.size()-1] > m_Output_time2[m_Output_time2.size()-1])
        {
            m_T = m_Output_time[m_Output_time.size()-1];

            int N = int(ceil((m_Output_time[m_Output_time.size()-1] - m_Output_time2[m_Output_time2.size()-1])/ m_step));
            m_del_t = (m_Output_time[m_Output_time.size()-1] - m_Output_time2[m_Output_time2.size()-1])/double(N);

            int ind = m_Output_time2.size()-1;
            double time = 0.0;

            Base::Vector3d vec(0.0,0.0,0.0);
            std::vector<Base::Vector3d> vecc;
            vecc.push_back(vec);


            for (int i=1; i<N; ++i)
            {
                time += m_del_t;
                m_Output_time2.push_back(m_Output_time2[ind] + time);
                m_Output2.push_back(vecc);
            }

            m_Output_time2.push_back(m_Output_time[m_Output_time.size()-1]);
            m_Output2.push_back(vecc);
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool path_simulate::MakePathSimulate_Feat(std::vector<float> &flatAreas)
{
    m_Feat = true;
    bool   tool;
    int    c[2];
    int run;
    double eps=0.2,t0,T,T_ges,t_ges;
    double rad[2];
    gp_Pnt pnt0, pnt1;

    std::vector<Handle_Geom_BSplineCurve>::iterator *it_1,*it_2;
    std::vector<Handle_Geom_BSplineCurve> *curves_1, *curves_2;

    ofstream anOutputFile[2];

    anOutputFile[0].open("output_master.k");
    anOutputFile[0].precision(7);

    if (m_single == false)
    {
        anOutputFile[1].open("output_slave.k");
        anOutputFile[1].precision(7);
    }

    m_it1 = m_BSplineTop.begin();
    m_it2 = m_BSplineBottom.begin();

    rad[0] =  m_set.master_radius;
    rad[1] = - m_set.slave_radius - m_set.sheet_thickness;

    c[0] = 1;    // Start Index der Master Kurven
    c[1] = 2001; // Start Index der Slave  Kurven

    int i = 0;

    while (m_it1 != m_BSplineTop.end() && m_it2 != m_BSplineBottom.end())
    {

        tool = StartingTool();  // bestimmt welches tool warten muss ...

        // tool == true  : Roboter = Slave  & NC = Master
        // tool == fasle : Roboter = Master & NC = Slave

        // setze neue Startparameter (in unserem Fall immer 0)
        m_StartParam[0] = ((*m_it1)->FirstParameter());
        if (m_single == false) m_StartParam[1] = ((*m_it2)->FirstParameter());

        // die erste Zustellung vor dem Kontakt mit dem Blech wird hier seperat gehandelt
        if (i==0)
        {

            /**************************************** ZUSTELLUNG 1 **********************************************/
            ConnectPaths_xy(0);
            WriteOutputDouble(anOutputFile[0],anOutputFile[1],c[0],c[1],0,beam);
            UpdateParam();


            /**************************************** ZUSTELLUNG 2 **********************************************/
            ConnectPaths_z(0);
            WriteOutputDouble(anOutputFile[0],anOutputFile[1],c[0],c[1],0,beam);
            UpdateParam();
            /****************************************/
        }

        if (!tool)
        {
            it_1 = &m_it1;
            it_2 = &m_it2;
            curves_1 = &m_BSplineTop;
            curves_2 = &m_BSplineBottom;
        }
        else
        {
            it_1 = &m_it2;
            it_2 = &m_it1;
            curves_1 = &m_BSplineBottom;
            curves_2 = &m_BSplineTop;
        }

        t0 = m_t0;  // speicherung der startzeit (wird später benötigt)

        while (true)
        {

            if (*it_1 != (*curves_1).end()-1)
            {

                // berechne output für die aktuelle bahn für das mastertools
                CompPath(tool);
                WriteOutputSingle(anOutputFile[tool],c[tool],0,tool,beam);
                UpdateParam();

                (*it_1)++;  // gehe zur nächsten kurve
                ConnectPaths_Feat(tool, 0, 1);

                if (*it_1 == (*curves_1).end()-1 && *it_2 == (*curves_2).end()-1)
                {
                    WriteOutputSingle(anOutputFile[tool],c[tool],0, tool,beam);
                    UpdateParam();
                    CompPath(tool);
                    WriteOutputSingle(anOutputFile[tool],c[tool],0, tool,beam);
                    break;
                }

                (**it_1)->D0((**it_1)->FirstParameter(),pnt0);

                // Abbruchkriterium:
                // erreicht der MASTER den nächsten ebenen Bereich, muss auf den SLAVE gewartet werden
                if ((pnt0.Z() > (flatAreas[i+1] + rad[tool] - 0.1)) &&
                        (pnt0.Z() < (flatAreas[i+1] + rad[tool] + 0.1)))
                {

                    run = Detect_FeatCurve(tool);  // bestimmt die nächste SLAVE-Kurve

                    // fahre alle kurven bis zur nächste SLAVE-Kurve ab
                    for (int i=0; i<run; ++i)
                    {
                        WriteOutputSingle(anOutputFile[tool],c[tool],0, tool,beam);
                        UpdateParam();
                        CompPath(tool);
                        WriteOutputSingle(anOutputFile[tool],c[tool],0, tool,beam);
                        UpdateParam();

                        (*it_1)++;
                        ConnectPaths_Feat(tool, 0, 1);  // Output wird hier nicht gleich ausgeschrieben,
                        // da die Synchronisierung mit dem Slave noch fehlt
                    }

                    break;
                }
            }
            else
            {
                if (*it_2 == (*curves_2).end()-1)
                {
                    CompPath(tool);
                    WriteOutputSingle(anOutputFile[tool],c[tool],0, tool,beam);
                    break;
                }
                else break;
            }

            WriteOutputSingle(anOutputFile[tool],c[tool],0,tool,beam);
            UpdateParam();
        }

        T = m_T;
        T_ges = T-t0;
        m_t0 = t0;

        CompPath(!tool);

        t_ges = m_T - m_t0;
        int n = (int) ceil(T_ges/t_ges);

        for (int j=0; j<n; ++j)
        {
            WriteOutputSingle(anOutputFile[!tool],c[!tool],0, !tool,beam);

            if (!tool)
            {
                //  Output_time aktualisieren
                for (unsigned int i=0; i<m_Output_time2.size(); ++i)
                {
                    m_Output_time2[i] += t_ges;
                }
            }
            else
            {
                //  Output_time aktualisieren
                for (unsigned int i=0; i<m_Output_time.size(); ++i)
                {
                    m_Output_time[i] += t_ges;
                }
            }
        }

        if (m_it1 == m_BSplineTop.end()-1 && m_it2 == m_BSplineBottom.end()-1)
            break;

        if (!tool)
        {
            m_Output_time2.clear();
            m_Output2.clear();
        }
        else
        {
            m_Output_time.clear();
            m_Output.clear();
        }

        m_T = m_t0 + n*t_ges;
        m_t0 = m_T;
        m_v1 = m_vmax;
        m_a1 = m_amax;
        m_c1[0] = m_a1/2;
        m_c1[1] = PI*m_a1/m_v1;

        (*it_2)++;

        ConnectPaths_Feat(!tool, 0, 0);
        TimeCorrection();
        WriteOutputDouble(anOutputFile[0],anOutputFile[1],c[0],c[1],0,beam);

        UpdateParam();
        ++i;
    }

    anOutputFile[0] << "*END" << endl;
    anOutputFile[0].close();
    anOutputFile[1] << "*END" << endl;
    anOutputFile[1].close();
    WriteTimes();

    return true;
}

bool path_simulate::MakePathRobot_Feat(std::vector<float> &flatAreas)
{
    m_Feat = true;
    int    f = 0;
    int run;
    bool   tool;
    double rad[2];
    gp_Pnt pnt0, pnt1;

    std::vector<Handle_Geom_BSplineCurve>::iterator *it_1,*it_2;
    std::vector<Handle_Geom_BSplineCurve> *curves_1, *curves_2;

    ofstream anOutputFile[2];

    stringstream master_file, slave_file;

    m_it1 = m_BSplineTop.begin();
    m_it2 = m_BSplineBottom.begin();

    rad[0] =  m_set.master_radius;
    rad[1] = - m_set.slave_radius - m_set.sheet_thickness;

    int i = 0;

    while (m_it1 != m_BSplineTop.end() && m_it2 != m_BSplineBottom.end())
    {

        tool = StartingTool();  // bestimmt welches tool warten muss ...

        // setze startparameter ( in unserem fall unnötig da immer 0 )
        m_StartParam[0] = ((*m_it1)->FirstParameter());
        if (m_single == false) m_StartParam[1] = ((*m_it2)->FirstParameter());

        // erste Zustellung wird noch seperat gehandelt
        if (i==0)
        {
            /**************************************** ZUSTELLUNG 1 **********************************************/
            ConnectPaths_xy(1);
            /**************************************** ZUSTELLUNG 2 **********************************************/
            ConnectPaths_z(1);
        }

        if (!tool)
        {
            it_1 = &m_it1;
            it_2 = &m_it2;
            curves_1 = &m_BSplineTop;
            curves_2 = &m_BSplineBottom;
        }
        else
        {
            it_1 = &m_it2;
            it_2 = &m_it1;
            curves_1 = &m_BSplineBottom;
            curves_2 = &m_BSplineTop;
        }

        // Hauptschleife
        while (true)
        {

            MakeSingleRoboPath(tool);  // Bahngenerierung des kontinuierlich fahrenden tools für aktuelle kurve

            // abbruchkriterium bei Bahnwechsel des wartenden tools

            // MASTER
            if (*it_1 != (*curves_1).end()-1)
            {
                (*it_1)++;
                (**it_1)->D0((**it_1)->FirstParameter(),pnt0);

                if (*it_1 == (*curves_1).end()-1 && *it_2 == (*curves_2).end()-1)
                {
                    ConnectPaths_Feat(tool, 1, 1);
                    MakeSingleRoboPath(tool);
                    break;
                }

                if ((pnt0.Z() > (flatAreas[i+1] + rad[tool] - 0.1)) &&
                        (pnt0.Z() < (flatAreas[i+1] + rad[tool] + 0.1)))
                {

                    run = Detect_FeatCurve(tool);

                    for (int i=0; i<run; ++i)
                    {
                        ConnectPaths_Feat(tool, 1, 1);
                        MakeSingleRoboPath(tool);
                        ++(*it_1);
                    }

                    break;
                }
            }
            else
            {
                if (*it_2 == (*curves_2).end()-1)
                {
                    ConnectPaths_Feat(tool, 1, 1);
                    MakeSingleRoboPath(tool);
                    break;
                }
                else break;
            }

            ConnectPaths_Feat(tool, 1, 1);
        }

        (**it_2)->D0((**it_2)->FirstParameter(),pnt0);

        while ((pnt0.Z() > (flatAreas[i] + rad[!tool] - 0.1)) && (pnt0.Z() < (flatAreas[i] + rad[!tool] + 0.1)))
        {
            MakeSingleRoboPath(!tool);

            if (*it_2 != (*curves_2).end()-1)
                (*it_2)++;
            else
                break;

            (**it_2)->D0((**it_2)->FirstParameter(),pnt0);
        }

        master_file << "output_master" << f << ".k";
        slave_file  << "output_slave"  << f << ".k";
        ++f;

        std::cout << master_file.str() << "  ,  " << slave_file << std::endl;

        anOutputFile[0].open((master_file.str()).c_str());
        anOutputFile[1].open((slave_file.str()).c_str());

        if (!tool)
        {
            anOutputFile[0] << "none" << std::endl;
            anOutputFile[1] << "continuous" << std::endl;
        }
        else
        {
            anOutputFile[0] << "continuous" << std::endl;
            anOutputFile[1] << "none" << std::endl;
        }

        WriteOutput_Feat(anOutputFile[0], anOutputFile[1],f,1);

        master_file.str("");
        master_file.clear();
        slave_file.str("");
        slave_file.clear();

        m_Output_robo1.clear();
        m_Output_robo2.clear();

        if (*it_1 == (*curves_1).end()-1 && *it_2 == (*curves_2).end()-1)
            break;

        ConnectPaths_Feat(0, 1, 1);
        ConnectPaths_Feat(1, 1, 0);
        ++i;
    }

    anOutputFile[0] << "*END" << endl;
    anOutputFile[0].close();
    anOutputFile[1] << "*END" << endl;
    anOutputFile[1].close();

    return true;
}

// Hilfsfunktion für den Featurebasierten Teil:
// Bestimmt welches Tool wartet während das andere läuft
bool path_simulate::StartingTool()
{
    double z0,z1;
    gp_Pnt pnt0,pnt1;

    if (m_it1 != m_BSplineTop.end()-1)
    {

        // z-Abstand zur nächsten Bahn - MASTER
        (*m_it1)->D0((*m_it1)->FirstParameter(),pnt0);
        m_it1++;
        (*m_it1)->D0((*m_it1)->FirstParameter(),pnt1);
        m_it1--;

        z0 = abs(pnt0.Z() - pnt1.Z());
    }
    else
        z0 = 1e+3;

    if (m_it2 != m_BSplineBottom.end()-1)
    {
        // z-Abstand zur nächsten Bahn - SLAVE
        (*m_it2)->D0((*m_it2)->FirstParameter(),pnt0);
        m_it2++;
        (*m_it2)->D0((*m_it2)->FirstParameter(),pnt1);
        m_it2--;

        z1 = abs(pnt0.Z() - pnt1.Z());
    }
    else z1 = 1e+3;

    if (z0<z1) return false;  // Slave muss warten
    else      return true;    // Master muss warten
}

int path_simulate::Detect_FeatCurve(bool tool)
{
    std::vector<Handle_Geom_BSplineCurve>::iterator it;
    std::vector<Handle_Geom_BSplineCurve> *curves;
    gp_Pnt p0,p1,p2;
    int t=0, ref = 0;

    if (!tool)
    {
        it = m_it1;
        curves = &m_BSplineTop;
    }
    else
    {
        it = m_it2;
        curves = &m_BSplineBottom;
    }

    while (true)
    {
        if (it == (*curves).end()-1) // fertig falls letzte kurve im vektor
        {
            for (int i=0; i<t; ++i)
                it--;

            return t;
        }

        // check ob geschlossene kurve
        (*it)->D0((*it)->FirstParameter(),p0);
        (*it)->D0((*it)->LastParameter(),p1);

        if (3.0 > p0.Distance(p1))        //p0.IsEqual(p1, 2))
        {
            ++t;
            ++ref;
            ++it;
        }
        else
        {
            (*it)->D0((*it)->FirstParameter(),p0);
            ref = t;

            while (true)
            {
                ++t;
                ++it;
                (*it)->D0((*it)->LastParameter(),p1);

                if (3.0 > p0.Distance(p1))
                {
                    ++t;
                    ++it;
                    break;
                }
            }
        }

        (*it)->D0((*it)->FirstParameter(),p2);

        if (p2.Z() != p0.Z())
        {
            for (int i=0; i<t; ++i)
                it--;

            t = ref;

            return t;
        }
        else
            ref = t;
    }
}

bool path_simulate::MakePathSimulate()
{
    int c1 = 1, c2 = 2001;

    ofstream anOutputFile;
    ofstream anOutputFile2;

    anOutputFile.open("output_master.k");
    anOutputFile.precision(7);

    if (m_single == false)
    {
        anOutputFile2.open("output_slave.k");
        anOutputFile2.precision(7);
    }

    for (m_it1 = m_BSplineTop.begin(); m_it1 < m_BSplineTop.end(); ++m_it1)
    {
        m_StartParam[0] = ((*m_it1)->FirstParameter());
        if (m_single == false) m_StartParam[1] = ((*m_it2)->FirstParameter());


        /**************************************** ZUSTELLUNG 1 **********************************************/
        m_conn = CheckConnect();
        if (m_conn)   ConnectPaths_xy(0);
        else    ConnectPaths_z(0);

        if (m_single == false) WriteOutputDouble(anOutputFile,anOutputFile2,c1,c2,0,beam);
        else                  WriteOutputSingle(anOutputFile,c1,0,0,beam);

        UpdateParam();
        /****************************************/

        /**************************************** ZUSTELLUNG 2 **********************************************/
        if (m_conn)   ConnectPaths_z(0);
        else    ConnectPaths_xy(0);

        if (m_single == false) WriteOutputDouble(anOutputFile,anOutputFile2,c1,c2,0,beam);
        else                  WriteOutputSingle(anOutputFile,c1,0,0,beam);

        UpdateParam();
        /****************************************/

        /****************************************** KURVE ***************************************************/
        CompPath(0); // Master
        CompPath(1); // Slave
        TimeCorrection();

        if (m_single == false) WriteOutputDouble(anOutputFile,anOutputFile2,c1,c2,0,beam);
        else                  WriteOutputSingle(anOutputFile,c1,0,0,beam);

        UpdateParam();
        /****************************************/

        if (m_single==false && (m_it1 != (m_BSplineTop.end()-1)))
            ++m_it2;
    }

	ofstream anOutputvelocity;

    anOutputvelocity.open("output_velocity.k");
    anOutputvelocity.precision(7);

	for(int i=0; i<times_tmp.size(); ++i)
	{
		anOutputvelocity << times_tmp[i] << ", " << velo_tmp[i] << endl;
	}


    anOutputFile  << "*END" << endl;
    anOutputFile.close();
    anOutputFile2 << "*END" << endl;
    anOutputFile2.close();
    WriteTimes();

    return true;
}

bool path_simulate::WriteOutput(ofstream &anOutputFile, ofstream &anOutputFile2, int &c, bool brob)
{
    if (m_single == false)
    {
        if (brob == false)
        {
            int n = m_Output.size();

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "3,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][1].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].x<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+2 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y<< std::endl;


            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "3,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+3 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][1].y<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].y<< std::endl;


            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+4 <<  std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;


            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "3,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+5 <<  std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][1].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].z<< std::endl;

            c = c+6;
        }
        else
        {
            int n1 = m_Output_robo1.size();
            for (int i=0; i<n1; ++i)
                anOutputFile  << m_Output_robo1[i].x + 290.0 << "," <<  m_Output_robo1[i].y + 145.0 << "," << m_Output_robo1[i].z << "," << RoboFlag_Master[i] << endl;

            anOutputFile.close();

            int n2 = m_Output_robo2.size();
            for (int i=0; i<n2; ++i)
                anOutputFile2 << m_Output_robo2[i].x + 290.0 << "," <<  m_Output_robo2[i].y + 145.0 << "," << m_Output_robo2[i].z << "," << RoboFlag_Slave[i] << endl;

            anOutputFile2.close();
        }
    }
    else
    {
        if (brob == false)
        {

            int n = m_Output.size();

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+4 <<  std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;

            c = c+3;
        }
        else
        {
            int n = m_Output_robo1.size();

            for (int i=0; i<n; ++i)
                anOutputFile << m_Output_robo1[i].x << "," <<  m_Output_robo1[i].y << "," << m_Output_robo1[i].z << ",";

            anOutputFile << std::endl;
        }
    }

    return true;
}

bool path_simulate::WriteOutput_Feat(ofstream &anOutputFile, ofstream &anOutputFile2, int &c, bool brob)
{
    if (m_single == false)
    {
        if (brob == false)
        {
            int n = m_Output.size();

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "3,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][1].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].x<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+2 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y<< std::endl;


            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "3,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+3 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][1].y<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].y<< std::endl;


            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+4 <<  std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;


            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "3,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+5 <<  std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][1].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][1].z<< std::endl;

            c = c+6;
        }
        else
        {
            int n1 = m_Output_robo1.size();
            for (int i=0; i<n1; ++i)
                anOutputFile  << m_Output_robo1[i].x + 290.0 << "," <<  m_Output_robo1[i].y + 145.0 << "," << m_Output_robo1[i].z  << endl;

            anOutputFile.close();

            int n2 = m_Output_robo2.size();
            for (int i=0; i<n2; ++i)
                anOutputFile2 << m_Output_robo2[i].x + 290.0 << "," <<  m_Output_robo2[i].y + 145.0 << "," << m_Output_robo2[i].z  << endl;

            anOutputFile2.close();
        }
    }
    else
    {
        if (brob == false)
        {

            int n = m_Output.size();

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,1,0," << c <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,2,0," << c+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y<< std::endl;

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << "2,3,0," << c+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c+4 <<  std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;

            c = c+3;
        }
        else
        {
            int n = m_Output_robo1.size();

            for (int i=0; i<n; ++i)
                anOutputFile << m_Output_robo1[i].x << "," <<  m_Output_robo1[i].y << "," << m_Output_robo1[i].z << ",";

            anOutputFile << std::endl;
        }
    }

    return true;
}


bool path_simulate::WriteTimes()
{
    ofstream anOutputFile;

    anOutputFile.open("CurveTimes.k");
    anOutputFile.precision(7);

    for (unsigned int i=0; i< m_PathTimes_Master.size(); ++i)
    {
        anOutputFile << m_PathTimes_Master[i].first << " " << m_PathTimes_Master[i].second << std::endl;
    }

    for (unsigned int i=0; i< m_PathTimes_Slave.size(); ++i)
    {
        anOutputFile << m_PathTimes_Slave[i].first << " " << m_PathTimes_Slave[i].second << std::endl;
    }

    anOutputFile.close();

    return true;
}

bool path_simulate::WriteOutputSingle(ofstream &anOutputFile, int &c, bool brob, bool tool, bool beamfl)
{
    std::vector< std::vector<Base::Vector3d> > Out_val;
    std::vector<double> Out_time;
    std::pair<float,float> times;
    int n;
    int ind;

    int pid;

    if (brob == true) goto robo;

    if (!tool)
    {
        Out_val  = m_Output;
        Out_time = m_Output_time;
        ind = 2;
    }
    else
    {
        Out_val  = m_Output2;
        Out_time = m_Output_time2;
        ind = 3;
    }

    if (beamfl) pid = ind+2;
    else    pid = ind;


    n = Out_val.size();

    if (n != Out_time.size())
        throw Base::Exception("Outputlängen passen nicht zusammen");

    if (n>1)
    {
        anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
        anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
        anOutputFile << pid << ",1,0," << c <<  ",1.000000, ," << Out_time[n-1] << ","  << Out_time[0] << std::endl;
        anOutputFile << "*DEFINE_CURVE" << std::endl << c << std::endl;

        for (int i=0; i<n; ++i)
        {
            anOutputFile << Out_time[i] - Out_time[0]<<"," << Out_val[i][0].x << std::endl;
        }

        anOutputFile << Out_time[n-1] - Out_time[0] + 0.1 << "," << Out_val[n-1][0].x << std::endl;

        times.first  = (float) Out_time[0];
        times.second = (float) Out_time[n-1];

        if (!tool) m_PathTimes_Master.push_back(times);
        else      m_PathTimes_Slave.push_back(times);

        anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
        anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
        anOutputFile << pid << ",2,0," << c+1 <<  ",1.000000, ," << Out_time[n-1] << "," << Out_time[0] << std::endl;
        anOutputFile << "*DEFINE_CURVE" << std::endl << c+1 << std::endl;

        for (int i=0; i<n; ++i)
        {
            anOutputFile <<  Out_time[i] - Out_time[0]<< "," << Out_val[i][0].y << std::endl;
        }

        anOutputFile << Out_time[n-1] - Out_time[0] + 0.1 << "," << Out_val[n-1][0].y << std::endl;

        times.first  = (float) Out_time[0];
        times.second = (float) Out_time[n-1];

        if (!tool) m_PathTimes_Master.push_back(times);
        else      m_PathTimes_Slave.push_back(times);

        anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
        anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
        anOutputFile << ind << ",3,0," << c+2 <<  ",1.000000, ," << Out_time[n-1] << "," << Out_time[0] << std::endl;

        if (beamfl)
        {
            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << pid << ",3,0," << c+2 <<  ",1.000000, ," << Out_time[n-1] << "," << Out_time[0] << std::endl;
        }

        anOutputFile << "*DEFINE_CURVE" << std::endl << c+2 <<  std::endl;


        for (int i=0; i<n; ++i)
        {
            anOutputFile <<  Out_time[i] - Out_time[0]<<"," << Out_val[i][0].z<< std::endl;
        }

        anOutputFile << Out_time[n-1] - Out_time[0] + 0.1 << "," << Out_val[n-1][0].z<< std::endl;

        times.first  = (float) Out_time[0];
        times.second = (float) Out_time[n-1];

        if (!tool) m_PathTimes_Master.push_back(times);
        else      m_PathTimes_Slave.push_back(times);

        c = c+3;
    }

    return true;

robo:  //--------- ROBOTER OUTPUT --------------

    std::vector<Base::Vector3d> Out_rob;

    if (!tool) Out_rob  = m_Output_robo1;
    else      Out_rob  = m_Output_robo2;

    n = Out_rob.size();

    for (int i=0; i<n; ++i)
        anOutputFile << Out_rob[i].x << "," <<  Out_rob[i].y << "," << Out_rob[i].z << "," << std::endl;

    anOutputFile << std::endl;


    return true;
}


bool path_simulate::WriteOutputDouble(ofstream &anOutputFile, ofstream &anOutputFile2, int &c1, int &c2, bool brob, bool beamfl)
{
    std::pair<float,float> times;
    int pid1 = 2; // Master
	int pid2 = 3; // Slave
	int pid3 = 4; // Platte


    if (brob == false) // simulation-output (brob == true -> roboter-ouput)
    {
        int n = m_Output.size();
        int n2 = m_Output2.size();

        if (n>1)
        {

            // MASTER-X

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << pid1 << ",1,0," << c1 <<  ",1.000000, ," << m_Output_time[n-1] << ","  << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c1 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile << m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].x << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].x << std::endl;

            times.first  = (float) m_Output_time[0];
            times.second = (float) m_Output_time[n-1];
            m_PathTimes_Master.push_back(times);       // fülle vektor für curve-times

            
			
			
			// SLAVE-X

            anOutputFile2 << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile2 << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile2 << pid2 << ",1,0," << c2 <<  ",1.000000, ," << m_Output_time2[n2-1] << ","  << m_Output_time2[0] << std::endl;
			
			if (beamfl) // wenn auf true, dann füge neuen part mit ein
            {
                anOutputFile2 << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
                anOutputFile2 << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
                anOutputFile2 << pid3 << ",1,0," << c2<<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            }

			anOutputFile2 << "*DEFINE_CURVE" << std::endl << c2 << std::endl;

            for (int i=0; i<n2; ++i)
            {
                anOutputFile2 << m_Output_time2[i] - m_Output_time2[0]<<"," << m_Output2[i][0].x << std::endl;
            }

            anOutputFile2 << m_Output_time2[n2-1] - m_Output_time2[0] + 0.1 << "," << m_Output2[n2-1][0].x << std::endl;

            times.first  = (float) m_Output_time2[0];
            times.second = (float) m_Output_time2[n2-1];
            m_PathTimes_Slave.push_back(times);

            
			
			
			
			// MASTER-Y

            anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile << pid1 << ",2,0," << c1+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c1+1 << std::endl;

            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<< "," << m_Output[i][0].y << std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].y << std::endl;

            times.first  = (float) m_Output_time[0];
            times.second = (float) m_Output_time[n-1];
            m_PathTimes_Master.push_back(times);

            
			
			
			// SLAVE-Y

            anOutputFile2 << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
            anOutputFile2 << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
            anOutputFile2 << pid2 << ",2,0," << c2+1 <<  ",1.000000, ," << m_Output_time2[n2-1] << ","  << m_Output_time2[0] << std::endl;
            
			if (beamfl) // wenn auf true, dann füge neuen part mit ein
            {
                anOutputFile2 << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
                anOutputFile2 << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
                anOutputFile2 << pid3 << ",2,0," << c2+1 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            }
			
			anOutputFile2 << "*DEFINE_CURVE" << std::endl << c2+1 << std::endl;

            for (int i=0; i<n2; ++i)
            {
                anOutputFile2 << m_Output_time2[i] - m_Output_time2[0]<<"," << m_Output2[i][0].y << std::endl;
            }

            anOutputFile2 << m_Output_time2[n2-1] - m_Output_time2[0] + 0.1 << "," << m_Output2[n2-1][0].y << std::endl;

            times.first  = (float) m_Output_time2[0];
            times.second = (float) m_Output_time2[n2-1];
            m_PathTimes_Slave.push_back(times);

            
			
			
			
			// MASTER-Z
			anOutputFile << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
			anOutputFile << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
			anOutputFile << pid1 << ",3,0," << c1+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;
            anOutputFile << "*DEFINE_CURVE" << std::endl << c1+2 <<  std::endl;


            for (int i=0; i<n; ++i)
            {
                anOutputFile <<  m_Output_time[i] - m_Output_time[0]<<"," << m_Output[i][0].z<< std::endl;
            }

            anOutputFile << m_Output_time[n-1] - m_Output_time[0] + 0.1 << "," << m_Output[n-1][0].z<< std::endl;

            times.first  = (float) m_Output_time[0];
            times.second = (float) m_Output_time[n-1];
            m_PathTimes_Master.push_back(times);

            
			
			
			
			// SLAVE-Z

			if (beamfl)
            {
                anOutputFile2 << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
                anOutputFile2 << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
                anOutputFile2 << pid3 << ",3,0," << c2+2 <<  ",1.000000, ," << m_Output_time[n-1] << "," << m_Output_time[0] << std::endl;

            }
			else
			{
				anOutputFile2 << "*BOUNDARY_PRESCRIBED_MOTION_RIGID" << std::endl;
				anOutputFile2 << "$#     pid       dof       vad      lcid        sf       vid     death     birth" << std::endl;
				anOutputFile2 << pid2 << ",3,0," << c2+2 <<  ",1.000000, ," << m_Output_time2[n2-1] << ","  << m_Output_time2[0] << std::endl;
			}

            anOutputFile2 << "*DEFINE_CURVE" << std::endl << c2+2 <<  std::endl;

            for (int i=0; i<n2; ++i)
            {
                anOutputFile2 << m_Output_time2[i] - m_Output_time2[0]<<"," << m_Output2[i][0].z << std::endl;
            }

            anOutputFile2 << m_Output_time2[n2-1] - m_Output_time2[0] + 0.1 << "," << m_Output2[n2-1][0].z << std::endl;

            times.first  = (float) m_Output_time2[0];
            times.second = (float) m_Output_time2[n2-1];
            m_PathTimes_Slave.push_back(times);

            c1 += 3;
            c2 += 3;
        }
    }
    else  // to do ..
    {
        int n = m_Output_robo1.size();

        for (int i=0; i<n; ++i)
            anOutputFile << m_Output_robo1[i].x << "," <<  m_Output_robo1[i].y << "," << m_Output_robo1[i].z << "," << std::endl;

        anOutputFile << std::endl;
    }


    return true;
}

