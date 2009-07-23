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



#ifndef Path_Simulate_h
#define Path_Simulate_h


#include <Base/Vector3D.h>
#include "cutting_tools.h"
#include <sstream>
#include <GeomAdaptor_Curve.hxx>

//#include <Base/Builder3D.h>

#define TolDist 1.0

/*! \class path_simulate
 \brief The main class for the path_simulate routine

 As it's input parameters it takes one respectivly two vectors of B-Spline-Curves (describing the Tool-Paths 
 for the IBU-simulation), two values a_max and v_max of type double, wich stands for the maximum allowable acceleration and velocity
 of the tool-movement and one value m_step of type double, specifying the step-length of the time-output-vector.
 
 As output, it gives one respectivly two output-files for the simulation-process containing
 a two-dimensional vector of time vs. velocity wich describes the tool-movement.
*/

class AppCamExport path_simulate
{
public:

	/** @brief Constructor
		@param BSplineTop vector of B-Spline-Curves describing the tool-paths of the master-tool
		@param set a struct wich also includes the parameters a_max and v_max */
   
	//path_simulate(const std::vector<Handle_Geom_BSplineCurve>& BSplineTop,
	//              struct CuttingToolsSettings& set);
	
	/** @brief Constructor
		@param BSplineTop vector of B-Spline-Curves describing the tool-paths of the master-tool
		@param BSplineBottom vector of B-Spline-Curves describing the tool-paths of the slave-tool
		@param set a struct wich also includes the parameters a_max and v_max */
    path_simulate(const std::vector<Handle_Geom_BSplineCurve>& BSplineTop,
                  const std::vector<Handle_Geom_BSplineCurve>& BSplineBottom,
                  struct CuttingToolsSettings& set);
    ~path_simulate();

	/** @brief Computes arc-length of acurve between the parameters u_0 and u_1
		@param acurve inputcurve
		@param u_0 startparameter
		@param u_1 endparameter*/
    double GetLength(GeomAdaptor_Curve& acurve, const Standard_Real u_0,const Standard_Real u_1);
    
	/** @brief Returns parametervalue at arc-length dist beginning at the specified startparameter u_0
		@param curve inputcurve
		@param dist arclength
		@param u_0 initial startparameter */
	double FindParamAt(GeomAdaptor_Curve& curve, double dist, double u_0);

	/** @brief Computes all Parameters of the velocity-function concerning the arc-length of one segmentation (master only)
		@param wirelength arc-length of actual segmentaion*/
    bool ParameterCalculation(double wirelenghth);
    
	/** @brief Computes all Parameters of the velocity-function concerning the arc-length of one segmentation (master & slave)
		@param wirelength_1 arc-length of actual segmentaion for the master
		@param wirelength_2 arc-length of actual segmentaion for the slave*/
	bool ParameterCalculation(double wirelenghth_1, double wirelenghth_2);

	/** @brief Computes necessary parameters of the velocity-function concerning the arc-length of one segmentation
		@param wirelength arc-length of actual segmentaion*/
	bool ParameterCalculationNew(double wirelenghth);

	/** @brief Main function of the output generation for the robot */
    //bool MakeRoboOutput();

	
    //std::vector<std::vector<Base::Vector3d> > GetCriticalPoints(std::vector<double> startParam);
    //std::vector<std::vector<Base::Vector3d> > Derivate(const std::vector<std::vector<Base::Vector3d> > &D);

	/** @brief Computes the curvature-radius at each discretisation-point concerning the input-vector D_1 of the first derivatives
	    @param D_1 input-vector including the first derivatives at each discretisation-point */
    std::vector<std::vector<double> > CurvCurvature(const std::vector<std::vector<Base::Vector3d> > &D_1);

	/** @brief Computes output for the connection-part in xy-direction
	    @param outputstyle false: simulation,  true: robot */
    bool ConnectPaths_xy(bool outputstyle);

	/** @brief Computes output for the connection-part in z-direction
	    @param outputstyle false: simulation,  true: robot */
    bool ConnectPaths_z(bool outputstyle);

	/** @brief Computes output for the connection-part in all directions for the feature-based-strategy
	    @param tool false: master true: slave*/
    bool ConnectPaths_Feat(bool tool, bool robo, bool connection_type);

	/** @brief Updates actual parameter sets */
    bool UpdateParam();

	/** @brief Writes output-vectors to file (only used for the robot)
	    @param  anOutputFile_1 output-file for the master
		@param  anOutputFile_1 output-file for the slave
		@param  c startindex of the curve-id
		@param  outputstyle false: simulation,  true: robot*/
    bool WriteOutput(ofstream &anOutputFile_1, ofstream &anOutputFile_2, int &c, bool outputstyle);

	/** @brief Writes output-vectors to file for the feature-based-strategy
	    @param  anOutputFile_1 output-file for the master
		@param  anOutputFile_1 output-file for the slave
		@param  c startindex of the curve-id
		@param  outputstyle false: simulation,  true: robot */
    bool WriteOutput_Feat(ofstream &anOutputFile_1, ofstream &anOutputFile_2, int &c, bool outputstyle);

	/** @brief Write start- and endtime for one tool-path-run (master & slave) */
    bool WriteTimes();

	/** @brief Writes output-vectors to file
	    @param  anOutputFile output-file
		@param  outputstyle false: simulation,  true: robot 
		@param  tool false: master, true: slave
		@param beamfl specifies an additional outputvalue wich determines the waiting-status of the tool-movement*/
    bool WriteOutputSingle(ofstream &m_anOutputFile, int &c, bool outputstyle, bool tool, bool beamfl);

	/** @brief Writes output-vectors to file
	    @param  anOutputFile_1 output-file for the master
		@param  nOutputFile_2 output-file for the slave
		@param  c1 startindex of the curve-id for the master
		@param  c2 startindex of the curve-id for the slave
		@param  outputstyle false: simulation,  true: robot 
		@param beamfl specifies an additional outputvalue using spring-pretension*/
    bool WriteOutputDouble(ofstream &anOutputFile_1,ofstream &nOutputFile_2, int &c1, int &c2, bool outputstyle,bool beamfl);
    
	
    
    bool MakeSinglePathNew(bool outputstyle, double length, bool part, bool curveType);

	/** @brief Main function of the output-generation for the simulation using the standard-strategy*/
    bool MakePathSimulate();

	/** @brief Main function of the output-generation for the simulation using the feature-based-strategy
	    @param flatAreas index-vector specifying the flat areas */
    bool MakePathSimulate_Feat(std::vector<float> &flatAreas);

	/** @brief Main function of the output-generation for the simulation using the spiral-based-strategy
	    @param flatAreas index-vector specifying the flat areas */
    bool MakePathSimulate_Spiral(std::vector<float> &flatAreas);

	/** @brief Computes and write output of the actual tool-path for the simulation-process
	    @param anOutputFile output-file
		@param c1 startindex of the curve-id
		@param  outputstyle false: simulation,  true: robot */
	bool MakeSinglePath(ofstream &anOutputFile, int &c, bool outputstyle);

	/** @brief Main function of the output-generation for the robot using the standard-strategy*/
    bool MakePathRobot();
	/** @brief Main function of the output-generation for the robot using the feature-based-strategy*/
    bool MakePathRobot_Feat(std::vector<float> &flatAreas);
	/** @brief Computes output-vector of the actual tool-path for the robot
	    @param tool false: master, true: slave */
	bool MakeSingleRoboPath(bool tool);
	/** @brief computes numerical-interation concerning the actual discretisation 
	    @param b false: master, true: slave*/	
    bool Integrate(bool b);
	/** @brief computes path-correction after a tool-path-run 
	    @param b false: master, true: slave*/
    bool Correction(bool b);
	/** @brief Adds additional time-values to the output-vector of the waiting tool for synchronisation */
    bool TimeCorrection();

	/** @brief Returns the next curve-index for the feature-based-strategy */ 
    int  Detect_FeatCurve(bool tool);

private:
    //bool SortNew(std::vector<gp_Pnt> &CornerPoints, std::vector<Handle_Geom_BSplineCurve> &PlaneCurvesTop,
    //             std::vector<Handle_Geom_BSplineCurve> &PlaneCurvesBot);

    /** @brief  curve-vector for the master-tool*/
    std::vector<Handle_Geom_BSplineCurve> m_BSplineTop;
	 /** @brief curve-vector for the slave-tool*/
    std::vector<Handle_Geom_BSplineCurve> m_BSplineBottom;
	/** @brief actual start-point for the connection-part*/
    std::vector<gp_Pnt> m_StartPnts1;
	/** @brief actual end-point for the connection-part*/
    std::vector<gp_Pnt> m_StartPnts2;
	/** @brief */
    std::vector<gp_Vec> m_vecs1;
    /** @brief */
	std::vector<gp_Vec> m_vecs2;
    /** @brief output-vector for the master including velocity-values for the simulation-process*/
	std::vector< std::vector<Base::Vector3d> > m_Output;
    /** @brief output-vector for the slave including velocity-values for the simulation-process*/
	std::vector< std::vector<Base::Vector3d> > m_Output2;
    /** @brief output-vector for the master including a point-set for the robot*/
	std::vector<Base::Vector3d> m_Output_robo1;
    /** @brief output-vector for the slave including a point-set for the robot*/
	std::vector<Base::Vector3d> m_Output_robo2;
    /** @brief additional output-vector of the master-tool for the robot-output including values 0 and 1 */
	std::vector<int> RoboFlag_Master;
    /** @brief additional output-vector of the slave-tool for the robot-output including values 0 and 1 */
	std::vector<int> RoboFlag_Slave;
    /** @brief output-vector for the master including a time-set conform to m_Output respectively m_Output_robo1*/
	std::vector<double> m_Output_time;
    /** @brief output-vector for the slave including a time-set conform to m_Output2 respectively m_Output_robo2*/
	std::vector<double> m_Output_time2;
     /** @brief timestep for the simulation-output*/
	double m_step;
     /** @brief vector specifying the path-Correction*/
	double m_Sum[3];
    /** @brief external setting-parameters*/
    CuttingToolsSettings m_set;

	/** @brief iterator for the master-curves*/
    std::vector<Handle_Geom_BSplineCurve>::iterator m_it1;   /* iterator über inner-paths */
    /** @brief iterator for the slave-curves*/
	std::vector<Handle_Geom_BSplineCurve>::iterator m_it2;   /* iterator über outer-paths */
    /** @brief sheet-thickness */
	double m_blech;
     /** @brief spring-pretension*/
	double m_pretension;
     /** @brief maximum number of output-values per file*/
	int  m_clip;
     /** @brief number of tool-paths*/
	int  m_NumPaths;
	 /** @brief flag specifies if spring-pretension is used or not*/
	bool beam;
     /** @brief flag specifying the used forming-strategy*/
	bool m_single;
     /** @brief flag specifying moving-direction (clockwise vs. anticlockwise)*/
	bool m_dir;
    /** @brief maximum allowable resulting velocity of the tool*/
	double m_vmax;
	/** @brief maximum allowable resulting acceleration of the tool*/
	double m_amax;
     /** @brief three-dimensional vector containing start-, maximum- and end-velocity of the velocity-function*/
	double m_v[3];
     /** @brief parameter of the velocity-function specifying the maximum acceleration of the tool-movement*/
	double m_a;
     /** @brief parameter of the velocity-function */
	double m_v1;
	 /** @brief parameter of the velocity-function */
	double m_v2;
	 /** @brief parameter of the velocity-function */
	double m_a1;
	 /** @brief parameter of the velocity-function */
	double m_a2;
     /** @brief parameters of the velocity-function*/
	std::vector<double> m_c1, m_c2;  /* parameter for the velocity function for the master toolpath ( c_1 )
                                        and the incrementlal die ( c_2 ) */
     /** @brief initial start-parameter for the current master- and slave-curves*/
	std::vector<double> m_StartParam;
     /** @brief timeparameter of the velocity-function*/
	double m_t0;
	/** @brief timeparameter of the velocity-function*/
	double m_t1;
	/** @brief timeparameter of the velocity-function*/
	double m_t2;
	/** @brief timeparameter of the velocity-function*/
	double m_T;
	/** @brief timeparameter of the velocity-function*/
	double m_del_t;  /* t_0 - starttime, T - endtime, del_t - timestep */
    /** @brief */
    //bool getFlatLevels();
     /** @brief estimates parameter-values of the velocity-function for current segmentation*/
	bool EstimateMaxAcceleration();
     /** @brief */
	//gp_Pnt Projection_pnt2curve(gp_Pnt &pnt, Handle_Geom_BSplineCurve &curve, double &parametervalue);
    
	/** @brief returns absolute acceleration at the time-value t */
    std::vector<double> GetAcceleration(double t);
     /** @brief returns absolute velocity at the time-value t */
	double GetVelocity(double time);
     /** @brief returns absolute velocity at the time-value t (connection-parts only)*/
	std::vector<double> GetVelocityOld(double t);
     /** @brief returns arc-length of the current tool-path at the time-value t*/
	std::vector<double> GetDistance(double t);
     /** @brief returns arc-length of the current tool-path at the time-value t*/
	double GetDistanceNew(double t);
     /** @brief determines connection-strategy for both tools*/
	bool CheckConnect();
     /** @brief determines connection-strategy for the specified tool
	     @param tool false: master, true: slave*/
	bool CheckConnect(bool tool);
     /** @brief determines critical bounds of the current curve for the specified tool
	     @param tool false: master, true: slave*/
	std::vector<std::vector<double> > CompBounds(bool tool);
     /** @brief Generates output for the current tool-path*/
	bool CompPath(bool tool);
     /** @brief determines wich tool should wait (feature-based-stategy only)*/
	bool StartingTool();
    /** @brief vector containing start- and end-times for the master-curves*/
    std::vector<std::pair<float,float> > m_PathTimes_Master;
    /** @brief vector containing start- and end-times for the slave-curves*/
	std::vector<std::pair<float,float> > m_PathTimes_Slave;
    /** @brief help-vector*/
	std::vector<double> times_tmp;
	 /** @brief */
	std::vector<double> velo_tmp;
     /** @brief flag specifying current connection-type*/
	bool m_conn;
     /** @brief flag specifying if a feature-based-strategy is used*/
	bool m_Feat;
};


#endif //Path_Simulate_h




