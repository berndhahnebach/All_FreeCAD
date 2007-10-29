#ifndef Cutting_Tools
#define Cutting_Tools

#include "PreCompiled.h"
#include <TopoDS_Wire.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_Interpolate.hxx>


class cutting_tools
{
public:
	cutting_tools(TopoDS_Shape &aShape, float pitch=1,bool mirrortobothsides=false);
	~cutting_tools();
	bool cutShape();
	TopoDS_Wire ordercutShape(const TopoDS_Shape &aShape);
	double GetWireLength(TopoDS_Wire &aWire);
	std::vector<double> GetFirstDerivates(const std::vector<gp_Pnt> &aPointVector);
	std::vector<double> GetSecondDerivates(const std::vector<gp_Pnt> &aPointVector);
	bool OffsetWires();
	bool getFlatLevels();
	inline std::vector<Handle_Geom_BSplineCurve>* getOutputhigh()
	{return &m_all_offset_cuts_high;}
	inline std::vector<Handle_Geom_BSplineCurve>* getOutputlow()
	{return &m_all_offset_cuts_low;}
	

private:
	TopoDS_Shape &m_Shape;


	std::vector<std::pair<float,TopoDS_Wire> > m_all_cuts;
	std::vector<Handle_Geom_BSplineCurve> m_all_offset_cuts_high,m_all_offset_cuts_low;
	std::pair<float,TopoDS_Wire> m_zl_wire_combination;
	std::vector<std::pair<float,TopoDS_Wire> >::iterator m_it;

	bool m_approx;//Haben wir nur ein Face, dann kommt das wohl aus der Approximierung

	bool m_mirrortobothsides;

	float m_pitch;





};

#endif



		