/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *   Human Rezai <human@mytum.de>                                          *
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


#include <Mod/Mesh/App/Core/Evaluation.h>
#include <Base/Exception.h>


#define SMALL_NUM  1e-6
#define ERR_TOL     0.5       // Abbruchkriterium für Least-Square-Matching



class AppCamExport best_fit
{
public:
	best_fit();
    ~best_fit();

	void Load(const MeshCore::MeshKernel &InputMesh,const TopoDS_Shape &CAD_Shape);
    bool MeshFit_Coarse();
    bool ShapeFit_Coarse();
    bool Perform();
    double CompError(std::vector<Base::Vector3f> &pnts, std::vector<Base::Vector3f> &normals);
    double CompError(std::vector<Base::Vector3f> &pnts, std::vector<Base::Vector3f> &normals, bool plot);

	std::vector<double> CompError_GetPnts(std::vector<Base::Vector3f> pnts, 
		                                  std::vector<Base::Vector3f> &normals);
    double CompTotalError();
    static bool Tesselate_Shape(const TopoDS_Shape &shape, MeshCore::MeshKernel &mesh, float deflection);
    static bool Tesselate_Face (const TopoDS_Face  &aface, MeshCore::MeshKernel &mesh, float deflection);
 
    static std::vector<Base::Vector3f> Comp_Normals(MeshCore::MeshKernel &M);
	bool Coarse_correction();
	double ANN();

	TopoDS_Shape m_Cad;              // CAD-Geometrie
    MeshCore::MeshKernel m_Mesh;     // das zu fittende Netz
	MeshCore::MeshKernel m_MeshWork;
    MeshCore::MeshKernel m_CadMesh;  // Netz aus CAD-Triangulierung

	std::vector<Base::Vector3f> m_pnts;
    std::vector<Base::Vector3f> m_normals;
    std::vector<double>         m_error;
	std::vector<std::vector<Base::Vector3f> > m_LSPnts;  // zu fittende Punktesätze für den Least-Square
	std::vector<double> m_weights;                       // gewichtungen für den Least-Square bzgl allen Netzpunkte
	std::vector<double> m_weights_loc;                   // gewichtungen für den Least-Square bzgl den projezierten Netzpunkten

    gp_Vec m_cad2orig;                     // Translationsvektor welche die CAD-Geometrie um den Ursprung zentriert
	std::vector<TopoDS_Face> m_LowFaces;   // Vektor der in der GUI selektierten Faces mit geringer Gewichtung

	bool Intersect(const Base::Vector3f &normal,const MeshCore::MeshKernel &mesh, Base::Vector3f &P, Base::Vector3f &I);


private:
    inline bool RotMat(Base::Matrix4D &matrix, double degree, int rotaionAxis);
    inline bool TransMat(Base::Matrix4D &matrix, double translation, int translationAxis);    
	inline bool PointNormalTransform(std::vector<Base::Vector3f> &pnts,
									 std::vector<Base::Vector3f> &normals,
									 Base::Matrix4D              &M);
	inline bool PointTransform(std::vector<Base::Vector3f> &pnts, 
		                       Base::Matrix4D &M);
	bool Comp_Weights();
	bool LSM();  // Least Square Matching
	std::vector<double>               Comp_Jacobi(const std::vector<double> &params);
    std::vector<std::vector<double> > Comp_Hess  (const std::vector<double> &params);
	int intersect_RayTriangle(const Base::Vector3f &normal,const MeshCore::MeshGeomFacet &T, Base::Vector3f &P, Base::Vector3f &I);
 

};

#endif
