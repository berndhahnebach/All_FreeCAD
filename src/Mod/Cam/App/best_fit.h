/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *	 Human Rezai <human.rezai@eads.net>                                    * 
 *   Mohamad Najib Muhammad Noor <najib_bean@yahoo.co.uk>                  *
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

#ifndef BEST_FIT_H
#define BEST_FIT_H


#define WM4_FOUNDATION_DLL_IMPORT

#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Builder3D.h>
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
#include <Mod/Mesh/App/Core/Grid.h>
//#include <Mod/Mesh/App/FeatureMeshCurvature.h>
//#include <Mod/Mesh/App/MeshFeature.h>

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomAPI_IntSS.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepAdaptor_CompCurve.hxx>

#include <Geom_BSplineSurface.hxx>
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
//#include <Handle_Adaptor3d_GenHCurve.hxx>
//#include <Handle_Adaptor3d_GenHSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Handle_Geom_Plane.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include "Approx.h"
#include "ConvertDyna.h"
#include "WireExplorer.h"
#include "cutting_tools.h"
#include "path_simulate.h"
#include "BRepAdaptor_CompCurve2.h"
#include <BRepClass3d_SolidClassifier.hxx>
#include <IntCurvesFace_ShapeIntersector.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepGProp.hxx>
#include <GProp_PrincipalProps.hxx>
#include <gp_Mat.hxx>
#include <BRepMesh.hxx>
#include <MeshShape.hxx>
#include <MeshAlgo.hxx>
#include <handle_poly_triangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Mod/Mesh/App/WildMagic4/Wm4Vector3.h>
#include <Mod/Mesh/App/WildMagic4/Wm4MeshCurvature.h>




class best_fit
{
public:
	best_fit(const MeshCore::MeshKernel &InputMesh, TopoDS_Shape &CAD_Shape);
	best_fit(const MeshCore::MeshKernel &InputMesh);
	best_fit(TopoDS_Shape &CAD_Shape);
	~best_fit();

	bool MeshFit_Coarse();
	bool ShapeFit_Coarse();
	bool mesh_curvature();
	bool thinning();
	bool Comp_Normals();
	bool rotation_fit();
	bool translation_fit();
	bool projection();
	bool Coarse_correction();
	bool Fit_iter();
	bool test();
	double Comp_Error(MeshCore::MeshKernel &mesh);
	static bool Tesselate_Shape(TopoDS_Shape &shape, MeshCore::MeshKernel &mesh, float deflection);
	static bool Tesselate_Face(TopoDS_Face &aface, MeshCore::MeshKernel &mesh, float deflection);


	MeshCore::MeshKernel m_Mesh;
	MeshCore::MeshKernel m_CadMesh;
	std::vector<Base::Vector3f> m_normals;
	std::list< std::vector <unsigned long> >  m_boundInd;
	TopoDS_Shape m_Cad;
	std::vector<int> m_pntInd;

protected:
	inline bool RotMat(Base::Matrix4D &matrix, double degree, int rotaionAxis);
	inline bool TransMat(Base::Matrix4D &matrix, double translation, int translationAxis);

};

#endif