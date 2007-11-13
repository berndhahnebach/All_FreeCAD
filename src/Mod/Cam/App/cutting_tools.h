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

#ifndef Cutting_Tools
#define Cutting_Tools


#include <Mod/Mesh/App/MeshAlgos.h>

#include "stuff.h"


class cutting_tools
{
public:
	cutting_tools(TopoDS_Shape &aShape, float pitch=1,bool mirrortobothsides=false);
	~cutting_tools();
	
	TopoDS_Wire ordercutShape(const TopoDS_Shape &aShape);
	double GetWireLength(TopoDS_Wire &aWire);
	//bool OffsetWires_Standard(float radius);
	
	/*
	Dient zum checken wieviele Faces wir haben und hier wird auch gleich ein vector gefüllt 
	wo alle flachen bereiche drin sind
	*/
	
	bool arrangecuts_ZLEVEL();
	bool checkPointIntersection(std::vector<projectPointContainer> &finalPoints);
	bool calculateAccurateSlaveZLevel(std::vector<projectPointContainer> &finalPoints, float radius, float &average_delta_z);
	//bool checkPointDistance(std::vector<gp_Pnt> &finalPoints,std::vector<gp_Pnt> &output);
	bool initializeMeshStuff();
	bool arrangecuts_SPIRAL();
	bool arrangecuts_FEATURE_BASED();
	inline std::vector<Handle_Geom_BSplineCurve>* getOutputhigh()
	{return &m_all_offset_cuts_high;}
	inline std::vector<Handle_Geom_BSplineCurve>* getOutputlow()
	{return &m_all_offset_cuts_low;}
	TopoDS_Compound getCutShape();
	
	

private:
	//typedef std::list<std::vector<Base::Vector3f> > Polylines;
	bool getShapeBB();
	//bool projectWireToSurface(const TopoDS_Wire &aWire,const TopoDS_Shape &aShape,std::vector<projectPointContainer> &aContainer);

	bool fillFaceBBoxes();
	bool checkPointinFaceBB(const gp_Pnt &aPnt,const Base::BoundBox3f &aBndBox);
	bool classifyShape();
	bool checkFlatLevel();
	bool cut(float z_level, float min_level, TopoDS_Shape &aCutShape,float &z_level_corrected);
	bool cut_Mesh(float z_level, float min_level, std::list<std::vector<Base::Vector3f> > &result,float &z_level_corrected);
	
	std::vector<std::pair<float,TopoDS_Shape> > m_ordered_cuts;
	std::vector<std::pair<TopoDS_Face,Base::BoundBox3f> > m_face_bboxes;
	std::vector<std::pair<TopoDS_Face,Base::BoundBox3f> >::iterator m_face_bb_it;

	std::vector<Handle_Geom_BSplineCurve> m_all_offset_cuts_high,m_all_offset_cuts_low;
	std::multimap<float,TopoDS_Wire> m_zl_wire_combination;
	//std::vector<std::pair<float,std::vector<TopoDS_Wire> > > m_zl_wire_combination;
	std::vector<std::pair<float,TopoDS_Shape> >::iterator m_ordered_cuts_it;
	
	//Member zum checken ob CAD oder nicht
	bool m_cad;
	TopoDS_Shape &m_Shape;
	MeshCore::MeshKernel m_CAD_Mesh;
	MeshCore::MeshAlgorithm * m_aMeshAlgo;
	MeshCore::MeshFacetGrid * m_CAD_Mesh_Grid;
	bool m_mirrortobothsides;
	//Zustellungswert
	float m_pitch;
	//Der höchste und niedrigste Z-Wert vom Shape 
	float m_minlevel,m_maxlevel;





};

#endif




