/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>          					   *
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



#ifndef Path_Simulate_h
#define Path_Simulate_h


#include <Base/Vector3D.h>
//#include <Base/Builder3D.h>

#define TolDist 2.0


class AppCamExport path_simulate
{
public:
    path_simulate(const std::vector<Handle_Geom_BSplineCurve>& BSplineTop,double a_max=15000.0,double v_max=1000.0);                        /*eine kurve  input*/
    path_simulate(const std::vector<Handle_Geom_BSplineCurve>& BSplineTop,
                  const std::vector<Handle_Geom_BSplineCurve>& BSplineBottom,
                  double a_max=20000.0, double v_max=600.0);             /*zwei kurven input*/
    ~path_simulate();

    double GetLength(GeomAdaptor_Curve& acurve, const Standard_Real startParameter,const Standard_Real endParameter);
    double FindParamAt(GeomAdaptor_Curve& curve, double dist, double startParam);

    bool ParameterCalculation(double wirelenghth);
    bool ParameterCalculationNew(double wirelenghth_1);
    bool ParameterCalculation(double wirelenghth_1, double wirelenghth_2);

	bool MakeRoboOutput();

	std::vector<std::vector<Base::Vector3d> > GetCriticalPoints(std::vector<double> startParam);

    std::vector<std::vector<Base::Vector3d> > PointEvaluation(double TotaltimeforonePath,
            unsigned int NumberOfEvalPoints,
            std::vector<double> startParam,
            std::vector<std::vector<Base::Vector3d>> &D1);

    std::vector<std::vector<Base::Vector3d> > PointEvaluation(double T,
            unsigned int N,
            std::vector<double> startParam,
            std::vector<std::vector<Base::Vector3d> > &D1,
            std::vector<std::vector<Base::Vector3d> > &D2);

    std::vector<std::vector<Base::Vector3d> > Derivate(const std::vector<std::vector<Base::Vector3d> > &D);
    std::vector<std::vector<double> > CurvCurvature(const std::vector<std::vector<Base::Vector3d> > &D);
    bool ConnectPaths_xy(ofstream &anOutputFile, int &c, bool outputstyle);
    bool ConnectPaths_z(ofstream &anOutputFile, int &c, bool outputstyle);
    bool OutputPath(std::vector<std::vector<Base::Vector3d> > &D1, std::vector<std::vector<Base::Vector3d> > &D2);
    bool UpdateParam();
    bool WriteOutput(ofstream &m_anOutputFile, ofstream &anOutputFile, int &c, bool outputstyle);
	bool WriteOutputSingle(ofstream &m_anOutputFile, int &c, bool outputstyle);
	bool WriteOutputDouble(ofstream &m_anOutputFile_Master,ofstream &m_anOutputFile_Slave, int &c, bool outputstyle);
    bool MakeSinglePath(ofstream &anOutputFile, int &c, bool outputstyle);
	bool MakeSinglePathNew(bool outputstyle, double length, bool part, bool curveType);
    bool MakePathSimulate();
	bool MakePathSimulate_Feat(std::vector<float> flatAreas);
    bool MakePathRobot();
	bool Integrate(bool b);
	bool Correction(bool b);
	bool TimeCorrection();
	inline const std::vector<std::pair<float,float> >* getPathTimes() { return &m_PathTimes; }

private:
	bool SortNew(std::vector<gp_Pnt> &CornerPoints, std::vector<Handle_Geom_BSplineCurve> &PlaneCurvesTop, 
													std::vector<Handle_Geom_BSplineCurve> &PlaneCurvesBot);

    std::vector<Handle_Geom_BSplineCurve> m_BSplineTop;
    std::vector<Handle_Geom_BSplineCurve> m_BSplineBottom;
    std::vector<gp_Pnt> m_StartPnts1;
    std::vector<gp_Pnt> m_StartPnts2;
    std::vector<gp_Vec> m_vecs1;
    std::vector<gp_Vec> m_vecs2;
    std::vector< std::vector<Base::Vector3d> > m_Output;
	std::vector< std::vector<Base::Vector3d> > m_Output2;
    std::vector<Base::Vector3d> m_Output_robo1;
    std::vector<Base::Vector3d> m_Output_robo2;
	std::vector<int> RoboFlag;
    std::vector<double> m_Output_time;
	std::vector<double> m_Output_time2;
	std::vector< std::vector<double> > CriBound;
	std::vector< std::vector<double> > CriBound_Master;
	std::vector< std::vector<double> > CriBound_Slave;
    int m_count;
    double m_step;
    double m_dbound;
    double m_t;
	double m_Sum[3];

	bool index;

	int runInd;
    //Base::Builder3D m_log3d;

    std::vector<Handle_Geom_BSplineCurve>::iterator m_it1;   /* iterator über inner-paths */
    std::vector<Handle_Geom_BSplineCurve>::iterator m_it2;   /* iterator über outer-paths */
    ofstream m_anOutputFile;
    double m_toolrad;
    double m_stress;
	float m_blech;
    int  m_clip;
    int  m_NumPaths;
    bool m_single;
    bool m_SingleTop, m_SingleBot;
    bool m_dir;
	bool m_checka;
    int  m_ind;        /* index describing current curve */
	int  m_num;
	double ratio;
    double m_vmax, m_amax;
	double m_v[3];
	double m_vS[3];
	double m_a;
	double m_aS;
    double m_v1, m_v2, m_a1, m_a2;
    std::vector<double> m_c1, m_c2;         /* parameter for the velocity function for the master toolpath ( c_1 )
                          and the incrementlal die ( c_2 ) */
    std::vector<double> m_StartParam;
    double m_t0, m_t1, m_t2, m_T, m_del_t;  /* t_0 - starttime, T - endtime, del_t - timestep */

    bool getFlatLevels();
    bool EstimateMaxAcceleration();
    gp_Pnt Projection_pnt2curve(gp_Pnt &pnt, Handle_Geom_BSplineCurve &curve, double &parametervalue);

    std::vector<double> GetAcceleration(double time);
    double GetVelocity(double time);
	std::vector<double> GetVelocityOld(double time);
    std::vector<double> GetDistance(double time);
	double GetDistanceNew(double time);
	bool CheckConnect();
	bool FillPathTimes();
	std::vector<std::vector<double> > CompBounds(bool tool);
	bool CompPath(bool tool);

	std::vector<std::pair<float,float> > m_PathTimes;
};


#endif //Path_Simulate_h




