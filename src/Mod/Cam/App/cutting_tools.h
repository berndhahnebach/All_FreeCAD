#ifndef Cutting_Tools
#define Cutting_Tools

#include "PreCompiled.h"
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Bnd_Box.hxx>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Builder.h>
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/MeshPy.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshAlgos.h>


class cutting_tools
{
public:
	cutting_tools(TopoDS_Shape &aShape, float pitch=1,bool mirrortobothsides=false);
	~cutting_tools();
	
	TopoDS_Wire ordercutShape(const TopoDS_Shape &aShape);
	double GetWireLength(TopoDS_Wire &aWire);
	bool OffsetWires_Standard(float radius);
	
	/*
	Dient zum checken wieviele Faces wir haben und hier wird auch gleich ein vector gefüllt 
	wo alle flachen bereiche drin sind
	*/
	
	bool arrangecuts_ZLEVEL();
	bool arrangecuts_SPIRAL();
	bool arrangecuts_FEATURE_BASED();
	inline std::vector<Handle_Geom_BSplineCurve>* getOutputhigh()
	{return &m_all_offset_cuts_high;}
	inline std::vector<Handle_Geom_BSplineCurve>* getOutputlow()
	{return &m_all_offset_cuts_low;}
	TopoDS_Compound getCutShape();
	
	

private:
	bool getShapeBB();
	bool fillFaceBBoxes();
	bool checkPointinFaceBB(const gp_Pnt &aPnt,const Base::BoundBox3f &aBndBox);
	bool classifyShape();
	bool checkFlatLevel();
	bool cut(float z_level, float min_level, TopoDS_Wire &aWire,float &z_level_corrected);
	bool cut_Mesh(float z_level, float min_level, std::list<std::vector<Base::Vector3f> > &result);
	
	std::vector<std::pair<float,std::list<std::vector<Base::Vector3f> > > > m_ordered_cuts;
	std::vector<std::pair<TopoDS_Face,Base::BoundBox3f> > m_face_bboxes;
	std::vector<std::pair<TopoDS_Face,Base::BoundBox3f> >::iterator m_face_bb_it;

	std::vector<Handle_Geom_BSplineCurve> m_all_offset_cuts_high,m_all_offset_cuts_low;
	std::multimap<float,TopoDS_Wire> m_zl_wire_combination;
	//std::vector<std::pair<float,std::vector<TopoDS_Wire> > > m_zl_wire_combination;
	std::vector<std::pair<float,TopoDS_Wire> >::iterator m_it;
	
	//Member zum checken ob CAD oder nicht
	bool m_cad;
	TopoDS_Shape &m_Shape;
	MeshCore::MeshKernel m_CAD_Mesh;
	MeshCore::MeshAlgorithm m_aMeshAlgo;
	MeshCore::MeshFacetGrid m_CAD_Mesh_Grid;
	bool m_mirrortobothsides;
	//Zustellungswert
	float m_pitch;
	//Der höchste und niedrigste Z-Wert vom Shape 
	float m_minlevel,m_maxlevel;





};

#endif




