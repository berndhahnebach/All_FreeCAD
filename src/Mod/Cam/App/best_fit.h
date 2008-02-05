/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *   Human Rezai <Human@mytum.de                                           *
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


#include <Mod/Mesh/App/Core/Evaluation.h>
#include <Base/Exception.h>

#define TOL1       0.0001  // step-break
#define TOL2       0.0001  // error-break
#define ntstep     4       // num of transition steps in one direction
#define nrstep     4       // num of rotation steps ...
#define aref_rot   10      // rotation area in one direction
#define tpart      1/7     // transition area in proportion to boundingbox-length
#define rstep_corr 10.0    // z-rotation step for - coarse correction - function
#define reject     0.5     // part of the boundary points to get rejected in error computation ( - ComPlaneErr - )
// starting with maximum error points
#define thin       200
#define b_thin     100


class best_fit
{
public:
    best_fit(const MeshCore::MeshKernel &InputMesh, TopoDS_Shape &CAD_Shape);
    best_fit(const MeshCore::MeshKernel &InputMesh);
    best_fit(TopoDS_Shape &CAD_Shape);
    ~best_fit();

    bool MeshFit_Coarse();
    bool ShapeFit_Coarse();
    bool thinning(unsigned int numPnts);
    bool Perform();
    bool Coarse_correction();
    bool Fit_iter();
    bool AdjustPlane();
    double CompError(std::vector<Base::Vector3f> &pnts, std::vector<Base::Vector3f> &normals);
    double CompError(std::vector<Base::Vector3f> &pnts, std::vector<Base::Vector3f> &normals, bool plot);
    double CompError(std::vector<Base::Vector3f> &pnts,  std::vector<Base::Vector3f> &normals,
                     std::vector<Base::Vector3f> &bpnts, std::vector<Base::Vector3f> &bnormals);
    double CompTotalError();
    static bool Tesselate_Shape(const TopoDS_Shape &shape, MeshCore::MeshKernel &mesh, float deflection);
    static bool Tesselate_Face (const TopoDS_Face  &aface, MeshCore::MeshKernel &mesh, float deflection);
    //static int intersect_RayTriangle( Ray R, Triangle T, Point* I );
    std::vector<Base::Vector3f> Comp_Normals(MeshCore::MeshKernel &M, std::vector<unsigned long> &Ind);
    static std::vector<Base::Vector3f> Comp_Normals(MeshCore::MeshKernel &M);

    MeshCore::MeshKernel m_Mesh;     // das zu fittende Netz
    MeshCore::MeshKernel m_CadMesh;  // Netz aus CAD-Triangulierung
    std::vector<Base::Vector3f> m_normals;
    std::vector<double> m_error;
    std::vector<int> m_FailProj;
    std::list< std::vector <unsigned long> >  m_boundInd;
    std::vector<double> m_pntCurv;
    TopoDS_Shape m_Cad;
    std::vector<unsigned long> m_pntInd;
    std::vector<Base::Vector3f> m_pnts;
    Base::Matrix4D m_Mat;
    gp_Vec m_cad2orig;
    std::vector<Base::Vector3f> bpnts;
    std::vector<Base::Vector3f> bnormals;



public:
    inline bool RotMat(Base::Matrix4D &matrix, double degree, int rotaionAxis);
    inline bool TransMat(Base::Matrix4D &matrix, double translation, int translationAxis);
    inline double ComPlaneErr(std::vector <Base::Vector3f> &pnts, std::vector <Base::Vector3f> &normals);
    inline bool MeshTransform(std::vector<Base::Vector3f> &pnts, Base::Matrix4D &M);
    inline bool MeshTransform(std::vector<Base::Vector3f> &pnts,
                              std::vector<Base::Vector3f> &normals,
                              Base::Matrix4D              &M);
    inline bool ZTranslation(std::vector<Base::Vector3f> &pnts, double trans);



};

#endif
