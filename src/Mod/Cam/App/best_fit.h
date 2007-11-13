/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *	 Human Rezai <human@mytum.de>										   * 
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

#include <Mod/Mesh/App/Core/MeshKernel.h>


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