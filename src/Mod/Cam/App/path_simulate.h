#ifndef Path_Simulate
#define Path_Simulate

#include "PreCompiled.h"
#include "Base/Vector3D.h"
#include <TopoDS_Wire.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI.hxx> 
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Base/Exception.h>
#include <Base/Builder3D.h>
#include <GCPnts_AbscissaPoint.hxx>
#include <math.h>

class path_simulate
{
public:
	path_simulate(std::vector<Handle_Geom_BSplineCurve>& BSplineTop,double a_max=10000.0,double v_max=600.0);                        /*eine kurve  input*/
	path_simulate(std::vector<Handle_Geom_BSplineCurve>& BSplineTop, 
		          std::vector<Handle_Geom_BSplineCurve>& BSplineBottom,
				  double a_max=10000.0, double v_max=600.0);             /*zwei kurven input*/
	~path_simulate();

	double GetLength(GeomAdaptor_Curve& acurve, const Standard_Real startParameter,const Standard_Real endParameter);
	double FindParamAt(GeomAdaptor_Curve& curve, double dist, double startParam);

	bool ParameterCalculation(double wirelenghth);
	bool ParameterCalculation(double wirelenghth_1, double wirelenghth_2);   
	
	std::vector<std::vector<Base::Vector3d> > PointEvaluation(double TotaltimeforonePath, 
		                                                     unsigned int NumberOfEvalPoints, 
															 std::vector<double> startParam, 
															 std::vector<std::vector<Base::Vector3d>> &D1);

	std::vector<std::vector<Base::Vector3d> > PointEvaluation(double T, 
															 unsigned int N, 
															 std::vector<double> startParam, 
															 std::vector<std::vector<Base::Vector3d>> &D1,
															 std::vector<std::vector<Base::Vector3d>> &D2);

	std::vector<std::vector<Base::Vector3d> > Derivate(const std::vector<std::vector<Base::Vector3d>> &D);
	bool ConnectPaths_xy(ofstream &anOutputFile, int &c, bool outputstyle);
	bool ConnectPaths_z(ofstream &anOutputFile, int &c, bool outputstyle);
	bool OutputPath(std::vector<std::vector<Base::Vector3d> > &D1, std::vector<std::vector<Base::Vector3d>> &D2);
	bool UpdateParam();
	bool WriteOutput(ofstream &m_anOutputFile, int &c, bool outputstyle);
	bool MakeSinglePath(ofstream &anOutputFile, int &c, bool outputstyle);
	bool MakePathSimulate();
	bool MakePathRobot();

private:
	std::vector<Handle_Geom_BSplineCurve> m_BSplineTop;
	std::vector<Handle_Geom_BSplineCurve> m_BSplineBottom;
	std::vector<gp_Pnt> m_StartPnts1;
	std::vector<gp_Pnt> m_StartPnts2;	
	std::vector<gp_Vec> m_vecs1;
	std::vector<gp_Vec> m_vecs2;
	std::vector< std::vector<Base::Vector3d> > m_Output;
	std::vector<Base::Vector3d> m_Output_robo1;
	std::vector<Base::Vector3d> m_Output_robo2;
	std::vector<double> m_Output_time;
	int m_count;
	double m_step;
	double m_dbound;
	double m_t;
	Base::Builder3D m_log3d;
	
	std::vector<Handle_Geom_BSplineCurve>::iterator m_it1;   /* iterator über inner-paths */
	std::vector<Handle_Geom_BSplineCurve>::iterator m_it2;   /* iterator über outer-paths */
	ofstream m_anOutputFile;
	double m_toolrad;
	double m_stress;
	int  m_clip;
	int  m_NumPaths;
	bool m_single;
	bool m_SingleTop, m_SingleBot;
	bool m_dir;
	int  m_ind;                              /* index describing current curve */
	double m_vmax, m_amax;
	double m_v1, m_v2, m_a1, m_a2;
	std::vector<double> m_c1, m_c2;         /* parameter for the velocity function for the master toolpath ( c_1 )
							                   and the incrementlal die ( c_2 ) */
    std::vector<double> m_StartParam;
	double m_t0, m_t1, m_t2, m_T, m_del_t;  /* t_0 - starttime, T - endtime, del_t - timestep */
	
	bool getFlatLevels();
	bool EstimateMaxAcceleration();
	gp_Pnt Projection_pnt2curve(gp_Pnt &pnt, Handle_Geom_BSplineCurve &curve, double &parametervalue);
	
	std::vector<double> GetAcceleration(double time);
	std::vector<double> GetVelocity(double time);
	std::vector<double> GetDistance(double time);
};

#endif




		