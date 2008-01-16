/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *  Human Rezai <human@mytum.de>                                    *
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


#include "PreCompiled.h"
#include "best_fit.h"

#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/Builder.h>
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
#include <Mod/Mesh/App/MeshAlgos.h>

#include <Base/Builder3D.h>

#include <BRep_Tool.hxx>
#include <BRepUtils.h>
#include <BRepBuilderAPI_Sewing.hxx>

#include <BRepMeshAdapt.hxx>

#include <BRepGProp.hxx>

#include <BRepBuilderAPI_Transform.hxx>
#include <GProp_PrincipalProps.hxx>

#include <handle_poly_triangulation.hxx>
#include <Poly_Triangulation.hxx>



best_fit::best_fit(const MeshCore::MeshKernel &mesh, TopoDS_Shape &cad)
        :m_Mesh(mesh),m_Cad(cad)
{
    m_pntCurv.resize(m_Mesh.CountPoints());
}

best_fit::best_fit(const MeshCore::MeshKernel &mesh)
        :m_Mesh(mesh)
{
}

best_fit::best_fit(TopoDS_Shape &cad)
        :m_Cad(cad)
{
}

best_fit::~best_fit()
{
}

bool best_fit::Perform()
{
    cout << "trafo2origin" << endl;
    MeshFit_Coarse();
    ShapeFit_Coarse();

    cout << "tesselate shape" << endl;
    Tesselate_Shape(m_Cad, m_CadMesh, 1);

    cout << "mesh curvature" << endl;
    thinning(thin);

    cout << "compute normals" << endl;
    m_normals = Comp_Normals(m_Mesh, m_pntInd);

    cout << "correction" << endl;
    Coarse_correction();

    cout << "adjust plane" << endl;
    AdjustPlane();

    cout << "start fitting iteration:" << endl;
    Fit_iter();

    m_normals.clear();
    m_normals = Comp_Normals(m_CadMesh);

    CompTotalError();


    return true;
}

bool best_fit::RotMat(Base::Matrix4D &M, double degree, int axis)
{
    M.unity();
    degree = 2*PI*degree/360;  // trafo bogenmaß

    switch (axis)
    {
    case 1:
        M[1][1]=cos(degree);
        M[1][2]=-sin(degree);
        M[2][1]=sin(degree);
        M[2][2]=cos(degree);
        break;
    case 2:
        M[0][0]=cos(degree);
        M[0][2]=-sin(degree);
        M[2][0]=sin(degree);
        M[2][2]=cos(degree);
        break;
    case 3:
        M[0][0]=cos(degree);
        M[0][1]=-sin(degree);
        M[1][0]=sin(degree);
        M[1][1]=cos(degree);
        break;
    default:
        throw Base::Exception("second input value differs from 1,2,3 (x,y,z)");
    }

    return true;
}

bool best_fit::TransMat(Base::Matrix4D &M, double trans, int axis)
{
    M.unity();

    switch (axis)
    {
    case 1:
        M[0][3] = trans;
        break;
    case 2:
        M[1][3] = trans;
        break;
    case 3:
        M[2][3] = trans;
        break;
    default:
        throw Base::Exception("second input value differs from 1,2,3 (x,y,z)");
    }

    return true;
}

bool best_fit::ZTranslation(std::vector<Base::Vector3f> &pnts, double trans)
{
    int n = pnts.size();

    for (int i=0; i<n; ++i)
        pnts[i].MoveZ(trans);

    return true;
}

bool best_fit::MeshTransform(std::vector<Base::Vector3f> &pnts,
                             std::vector<Base::Vector3f> &normals,
                             Base::Matrix4D              &M)
{
    int m = pnts.size();
    Base::Vector3f pnt,normal;

    for (int i=0; i<m; ++i)
    {
        pnt.x  =  M[0][0]*pnts[i].x + M[0][1]*pnts[i].y + M[0][2]*pnts[i].z + M[0][3];
        pnt.y  =  M[1][0]*pnts[i].x + M[1][1]*pnts[i].y + M[1][2]*pnts[i].z + M[1][3];
        pnt.z  =  M[2][0]*pnts[i].x + M[2][1]*pnts[i].y + M[2][2]*pnts[i].z + M[2][3];

        pnts[i] = pnt;

        normal.x = M[0][0]*normals[i].x + M[0][1]*normals[i].y + M[0][2]*normals[i].z;
        normal.y = M[1][0]*normals[i].x + M[1][1]*normals[i].y + M[1][2]*normals[i].z;
        normal.z = M[2][0]*normals[i].x + M[2][1]*normals[i].y + M[2][2]*normals[i].z;

        normals[i] = normal;
    }

    return true;
}


bool best_fit::MeshTransform(std::vector<Base::Vector3f> &pnts,
                             Base::Matrix4D              &M)
{
    int m = pnts.size();
    Base::Vector3f pnt;

    for (int i=0; i<m; ++i)
    {
        pnt.x  =  M[0][0]*pnts[i].x + M[0][1]*pnts[i].y + M[0][2]*pnts[i].z + M[0][3];
        pnt.y  =  M[1][0]*pnts[i].x + M[1][1]*pnts[i].y + M[1][2]*pnts[i].z + M[1][3];
        pnt.z  =  M[2][0]*pnts[i].x + M[2][1]*pnts[i].y + M[2][2]*pnts[i].z + M[2][3];

        pnts[i] = pnt;
    }

    return true;
}

bool best_fit::MeshFit_Coarse()
{
    GProp_GProps prop;
    GProp_PrincipalProps pprop;
    BRepGProp SurfProp;

    Base::Vector3f pnt(0.0,0.0,0.0);
    Base::Vector3f x,y,z;
    Base::Builder3D log3d_mesh, log3d_cad;
    gp_Pnt orig;

    gp_Vec v1,v2,v3,v,vec; // Hauptachsenrichtungen
    gp_Trsf trafo;

    BRepGProp::SurfaceProperties(m_Cad, prop);
    pprop = prop.PrincipalProperties();

    v1 = pprop.FirstAxisOfInertia();
    v2 = pprop.SecondAxisOfInertia();
    v3 = pprop.ThirdAxisOfInertia();

    v1.Normalize();
    v2.Normalize();
    v3.Normalize();

    v1 *= -1;
    v2 *= -1;

    v = v1;
    v.Cross(v2);

    // right-hand-system check
    if ( v.Dot(v3) < 0.0 )
        v3 *= -1;

    orig  = prop.CentreOfMass();

    Base::Matrix4D M;

    M[0][0] =  v1.X();
    M[0][1] =  v1.Y();
    M[0][2] =  v1.Z();
    M[0][3] = 0.0f;
    M[1][0] =  v2.X();
    M[1][1] =  v2.Y();
    M[1][2] =  v2.Z();
    M[1][3] = 0.0f;
    M[2][0] =  v3.X();
    M[2][1] =  v3.Y();
    M[2][2] =  v3.Z();
    M[2][3] = 0.0f;
    M[3][0] =  0.0f;
    M[3][1] =  0.0f;
    M[3][2] =  0.0f;
    M[3][3] = 1.0f;

    M.inverse();

    MeshCore::MeshEigensystem pca(m_Mesh);
    pca.Evaluate();
    Base::Matrix4D T1 =  pca.Transform();
    m_Mesh.Transform(M*T1);

    // plot CAD -> local coordinate system

    /*x.x = 500*v1.X()+orig.X();x.y = 500*v1.Y()+orig.Y();x.z = 500*v1.Z()+orig.Z();
    y.x = 500*v2.X()+orig.X();y.y = 500*v2.Y()+orig.Y();y.z = 500*v2.Z()+orig.Z();
    z.x = 500*v3.X()+orig.X();z.y = 500*v3.Y()+orig.Y();z.z = 500*v3.Z()+orig.Z();

    pnt.x = orig.X();pnt.y = orig.Y();pnt.z = orig.Z();

    log3d_cad.addSingleLine(pnt,x,3,0,0,0);log3d_cad.addSingleLine(pnt,y,3,1,0,0);log3d_cad.addSingleLine(pnt,z,3,1,1,1);
    log3d_cad.saveToFile("c:/CAD_CoordSys.iv");*/

    // plot Mesh -> local coordinate system

    /*v1.SetX(T1[0][0]);v1.SetY(T1[0][1]);v1.SetZ(T1[0][2]);
    v2.SetX(T1[1][0]);v2.SetY(T1[1][1]);v2.SetZ(T1[1][2]);
    v3.SetX(T1[2][0]);v3.SetY(T1[2][1]);v3.SetZ(T1[2][2]);

    T1.inverse();

    orig.SetX(T1[0][3]);orig.SetY(T1[1][3]);orig.SetZ(T1[2][3]);

       x.x = 500*v1.X()+orig.X();x.y = 500*v1.Y()+orig.Y();x.z = 500*v1.Z()+orig.Z();
    y.x = 500*v2.X()+orig.X();y.y = 500*v2.Y()+orig.Y();y.z = 500*v2.Z()+orig.Z();
    z.x = 500*v3.X()+orig.X();z.y = 500*v3.Y()+orig.Y();z.z = 500*v3.Z()+orig.Z();

    pnt.x = orig.X();pnt.y = orig.Y();pnt.z = orig.Z();

    log3d_mesh.addSingleLine(pnt,x,3,0,0,0);log3d_mesh.addSingleLine(pnt,y,3,1,0,0);log3d_mesh.addSingleLine(pnt,z,3,1,1,1);
    log3d_mesh.saveToFile("c:/Mesh_CoordSys.iv");*/

    return true;
}

bool best_fit::ShapeFit_Coarse()
{
    GProp_GProps prop;
    BRepGProp    SurfProp;
    gp_Trsf      trafo;
    gp_Pnt       orig;

    SurfProp.SurfaceProperties(m_Cad, prop);
    orig  = prop.CentreOfMass();

    // CAD-Mesh -> zurück zum Ursprung
    m_cad2orig.SetX(-orig.X());
    m_cad2orig.SetY(-orig.Y());
    m_cad2orig.SetZ(-orig.Z());

    trafo.SetTranslation(m_cad2orig);
    BRepBuilderAPI_Transform trsf(trafo);

    trsf.Perform(m_Cad);
    m_Cad = trsf.Shape();

    return true;
}

bool best_fit::thinning(unsigned int numPnts)
{
    std::vector<unsigned long> Elements;
    double a, ntmp=0.0;
    int gridSize, n;

    m_pntInd.clear();
    m_pnts.clear();

    MeshCore::MeshPointGrid grid(m_Mesh);
    MeshCore::MeshGridIterator gridIt(grid);

    if (m_Mesh.CountPoints() > numPnts)
        a = numPnts / (double) m_Mesh.CountPoints();
    else
        a=1;

    for (gridIt.Init(); gridIt.More(); gridIt.Next())
    {
        gridSize = gridIt.GetCtElements();
        gridIt.GetElements(Elements);

        n = a*gridSize;

        if (n==0)
        {
            ntmp += a*gridSize;

            if ((gridSize>0) && (ntmp>=1))
            {
                n=1;
                ntmp=0;
            }
        }

        for (int i=0; i<n; ++i)
        {
            m_pntInd.push_back(Elements[i]);
            m_pnts.push_back(m_Mesh.GetPoint(Elements[i]));
        }

        Elements.clear();
    }

    cout << "number of points after thinning: " << m_pntInd.size() << endl;

    return true;
}

bool best_fit::AdjustPlane()
{
    // folgende funktionen müssen vor - AdjustPlane - ausgeführt werden:

    // - tesselate_shape
    // - compute_normals

    std::list< std::vector <unsigned long> > BoundariesIndex;
    std::list< std::vector <Base::Vector3f> > BoundariesPoints;
    MeshCore::MeshRefPointToFacets rf2pt(m_Mesh);
    MeshCore::MeshAlgorithm algo(m_Mesh);
    MeshCore::MeshGeomFacet t_face;

    algo.GetMeshBorders(BoundariesIndex);
    algo.GetMeshBorders(BoundariesPoints);

    std::list< std::vector <unsigned long> >::iterator bInd  = BoundariesIndex.begin();
    std::list< std::vector <Base::Vector3f> >::iterator bPnt = BoundariesPoints.begin();
    std::vector <unsigned long> v_ind = *bInd;
    std::vector <Base::Vector3f> v_pnts = *bPnt;

    std::vector<Base::Vector3f> b_pnts;      // boundary points
    std::vector<Base::Vector3f> b_normals;   // boundary normals

    for (unsigned int i=0; i<v_ind.size(); ++i)
    {
        b_pnts.push_back(m_Mesh.GetPoint(v_ind[i]));
    }

    b_normals = Comp_Normals(m_Mesh, v_ind);
    cout << "boundary points: " << v_ind.size() << endl;

    // erzeuge künstliches mesh um hauptachsen der randpunkte berechnen zu können
    MeshCore::MeshKernel mesh;
    MeshCore::MeshPoint mpnt;
    MeshCore::MeshPointArray mPointArray;
    MeshCore::MeshFacetArray mFacetArray;

    for (unsigned int i = 0; i < b_pnts.size(); ++i)
    {
        mpnt = b_pnts[i];
        mpnt.SetProperty(v_ind[i]);
        mPointArray.push_back(mpnt);
    }

    mesh.Assign(mPointArray, mFacetArray);

    // need at least one facet to perform the PCA
    MeshCore::MeshGeomFacet face;
    mesh.AddFacet(face);

    // bestimme hauptachsen der randpunkte
    MeshCore::MeshEigensystem pca(mesh);
    pca.Evaluate();
    Base::Matrix4D T1 = pca.Transform();
    Base::Vector3f v1, v2, v3;

    v1.x = T1[0][0];
    v1.y = T1[0][1];
    v1.z = T1[0][2];
    v2.x = T1[1][0];
    v2.y = T1[1][1];
    v2.z = T1[1][2];
    v3.x = T1[2][0];
    v3.y = T1[2][1];
    v3.z = T1[2][2];

    /*cout << "Hauptachsen: " << endl;
    cout << v1.x     << ", " << v1.y     << ", " << v1.z     << endl;
    cout << v2.x     << ", " << v2.y     << ", " << v2.z     << endl;
    cout << v3.x     << ", " << v3.y     << ", " << v3.z     << endl;
    cout << "Translation: " << endl;
    cout << T1[0][3] << ", " << T1[1][3] << ", " << T1[2][3] << endl;*/

    v1.Normalize();
    v2.Normalize();
    v3.Normalize();

    // plot main components of mesh
    /*Base::Builder3D loo;

    v1.Scale(500,500,500);
    v2.Scale(500,500,500);
    v3.Scale(500,500,500);

    loo.addSingleArrow(b_pnts[0], b_pnts[0] + v1,2,0,0,0);
    loo.addSingleArrow(b_pnts[0], b_pnts[0] + v2,2,1,0,0);
    loo.addSingleArrow(b_pnts[0], b_pnts[0] + v3,2,1,1,1);
    loo.saveToFile("c:/pca_afterCoarse.iv");*/

    if (v1.z < 0)
        v1 *= -1;

    // compute rotation matrizes for adjusting the plane to a constant z-value
    Base::Matrix4D M,M1,M2;
    double fcos1,fcos2;

    fcos1 = v1.z / sqrt( v1.y*v1.y + v1.z*v1.z );

    // rotation x-achse
    M1[0][0] = 1.0f;
    M1[0][1] = 0.0f;
    M1[0][2] = 0.0f;
    M1[0][3] = 0.0f;
    M1[1][0] = 0.0f;
    M1[1][1] = fcos1;
    M1[1][2] = -sqrt(1-fcos1*fcos1);
    M1[1][3] = 0.0f;
    M1[2][0] = 0.0f;
    M1[2][1] = sqrt(1-fcos1*fcos1);
    M1[2][2] = fcos1;
    M1[2][3] = 0.0f;
    M1[3][0] = 0.0f;
    M1[3][1] = 0.0f;
    M1[3][2] = 0.0f;
    M1[3][3] = 1.0f;

    if (v1.y < 0)
        M1.inverse();

    v1 = M1*v1;

    fcos2 = v1.z / sqrt( v1.x*v1.x + v1.z*v1.z );

    // rotation y-achse
    M2[0][0] = fcos2;
    M2[0][1] = 0.0f;
    M2[0][2] = -sqrt(1-fcos2*fcos2);
    M2[0][3] = 0.0f;
    M2[1][0] = 0.0f;
    M2[1][1] = 1.0f;
    M2[1][2] = 0.0f;
    M2[1][3] = 0.0f;
    M2[2][0] = sqrt(1-fcos2*fcos2);
    M2[2][1] = 0.0f;
    M2[2][2] = fcos2;
    M2[2][3] = 0.0f;
    M2[3][0] = 0.0f;
    M2[3][1] = 0.0f;
    M2[3][2] = 0.0f;
    M2[3][3] = 1.0f;

    if (v1.x < 0)
        M2.inverse();

    M = M1*M2;

    m_Mesh.Transform(M);
    MeshTransform(m_pnts, m_normals, M);

    // berechne z-translation
    Base::BoundBox3f bbo1 = m_CadMesh.GetBoundBox();  // !!! must perform  - best_fit::tesselate_shape -  first !!!
    T1.inverse();

    double z1 = bbo1.MaxZ;
    double z2 = T1[2][3];

    cout << "z1: " << z1 << ", z2: " << z2 << endl;

    M.unity();
    M[2][3] = z1 - z2;

    m_Mesh.Transform(M);
    MeshTransform(m_pnts, M);

    int num_tstep = 50, gridSize, n;
    double ref_trans, trans_step, a, ntmp = 0.0;
    std::vector<unsigned long> Elements, SumElem;
    std::vector<Base::Vector3f> tmp_pnts;
    Base::Matrix4D Tz, Tz_ref, M_fin;

    Base::BoundBox3f bbox = m_Mesh.GetBoundBox();

    ref_trans  = 0.1 * bbox.LengthZ();
    trans_step = 2 * ref_trans / num_tstep;

    // perform thinning of the boundary mesh
    MeshCore::MeshPointGrid grid(mesh);
    MeshCore::MeshGridIterator gridIt(grid);

    std::vector <Base::Vector3f>::iterator pnt_it;
    std::vector <unsigned long> b2Ind;

    if (mesh.CountPoints() > b_thin)
        a = b_thin / (double) mesh.CountPoints();
    else
        a=1;

    for (gridIt.Init(); gridIt.More(); gridIt.Next())
    {
        gridSize = gridIt.GetCtElements();
        gridIt.GetElements(Elements);
        n = a*gridSize;

        if (n==0)
        {
            ntmp += a*gridSize;
            if ((gridSize>0) && (ntmp>=1))
            {
                n=1;
                ntmp=0;
            }
        }

        for (int i=0; i<n; ++i)
        {
            tmp_pnts.push_back(m_Mesh.GetPoint(mesh.GetPoint(Elements[i])._ulProp));
            b2Ind.push_back(mesh.GetPoint(Elements[i])._ulProp);
        }

        Elements.clear();
    }

    cout << "number of boundary points after thinning: " << tmp_pnts.size() << endl;

    std::vector<Base::Vector3f> tmp_normals = Comp_Normals(m_Mesh, b2Ind);

    b_pnts.clear();
    b_pnts = tmp_pnts;

    b_normals.clear();
    b_normals  = tmp_normals;


    double trans_fin;
    double err = 1e+10, err_tmp, err_it = 1e+10;


    while (true)
    {

        TransMat(Tz_ref, -ref_trans, 3);
        m_Mesh.Transform(Tz_ref);
        ZTranslation(m_pnts, -ref_trans);
        ZTranslation(b_pnts, -ref_trans);
        tmp_pnts  = b_pnts;
        trans_fin = ref_trans;

        for (int n=1; n<(num_tstep+1); ++n)
        {

            ZTranslation(tmp_pnts, trans_step);
            err_tmp = ComPlaneErr(tmp_pnts, tmp_normals);

            if (err_tmp < err)
            {
                trans_fin = n*trans_step;
                err = err_tmp;

                cout << " error: " << sqrt(err_tmp)<< " at " << -ref_trans + n*trans_step << endl;
            }
        }

        TransMat(M_fin, trans_fin, 3);
        m_Mesh.Transform(M_fin);
        MeshTransform(m_pnts,  M_fin);
        MeshTransform(b_pnts,  M_fin);
        tmp_pnts = b_pnts;
        err = ComPlaneErr(b_pnts, b_normals);

        if ( (err_it - err) < TOL2 || trans_step < TOL1)
            break;

        err_it = err;
        std::cout << "average error: " << sqrt(err) << " mm" << std::endl;

        ref_trans = trans_step;
        trans_step = 2 * ref_trans / num_tstep;
    }


    err = ComPlaneErr(b_pnts, b_normals);
    std::cout << "final average error: " << sqrt(err) << " mm" << std::endl;

    CompError(b_pnts, b_normals, true);

    bpnts = b_pnts;
    bnormals = b_normals;

    return true;
}

double best_fit::ComPlaneErr(std::vector <Base::Vector3f> &pnts, std::vector <Base::Vector3f> &normals)
{
    double err_avg = 0.0;
    double err_max = 0.0;
    double sqrdis  = 0.0;

    MeshCore::MeshFacetGrid aFacetGrid(m_CadMesh);
    MeshCore::MeshAlgorithm malg(m_CadMesh);
    MeshCore::MeshAlgorithm malg2(m_CadMesh);

    Base::Vector3f projPoint, distVec;
    unsigned long facetIndex;

    std::vector<double> tmp;

    int NumOfPoints = pnts.size();
    int c = 0;

    for (int i=0; i<NumOfPoints; ++i)
    {
        if (!malg.NearestFacetOnRay(pnts[i], normals[i], aFacetGrid, projPoint, facetIndex))  // gridoptimiert
        {
            if (malg2.NearestFacetOnRay(pnts[i], normals[i], projPoint, facetIndex))
            {
                distVec  = projPoint-pnts[i];
                sqrdis   = distVec*distVec;
                err_avg += sqrdis;

                tmp.push_back(sqrdis);
            }
            else
                ++c;

        }
        else
        {
            distVec  = projPoint-pnts[i];
            sqrdis   = distVec*distVec;
            err_avg += sqrdis;

            tmp.push_back(sqrdis);
        }
    }

    std::sort(tmp.begin(), tmp.end());

    double sum = 0.0;
    int num = reject*tmp.size();

    for (int i=0; i<num; ++i)
        sum += tmp[i];

    sum /= num;

    if (c==NumOfPoints)
        return 1e+10;

    return sum;
}

bool best_fit::Tesselate_Face(const TopoDS_Face &aface, MeshCore::MeshKernel &mesh, float deflection)
{
	Base::Builder3D aBuild;
	MeshCore::MeshBuilder builder(mesh);
	builder.Initialize(1000);
	Base::Vector3f Points[3];
	if(!BRepTools::Triangulation(aface,0.1))
	{
		// removes all the triangulations of the faces ,
		// and all the polygons on the triangulations of the edges:
		BRepTools::Clean(aface);

		// adds a triangulation of the shape aShape with the deflection aDeflection:
		//BRepMesh_IncrementalMesh Mesh(pcShape->getShape(),aDeflection);
		TriangleAdapt_Parameters MeshingParams;


		BRepMeshAdapt::Mesh(aface,deflection,MeshingParams);
	}
    TopLoc_Location aLocation;
    // takes the triangulation of the face aFace:
    Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aface,aLocation);
    if (!aTr.IsNull()) // if this triangulation is not NULL
    {
        // takes the array of nodes for this triangulation:
        const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
        // takes the array of triangles for this triangulation:
        const Poly_Array1OfTriangle& triangles = aTr->Triangles();
        // create array of node points in absolute coordinate system
        TColgp_Array1OfPnt aPoints(1, aNodes.Length());
        for ( Standard_Integer i = 1; i < aNodes.Length()+1; i++)
            aPoints(i) = aNodes(i).Transformed(aLocation);
        // Takes the node points of each triangle of this triangulation.
        // takes a number of triangles:
        Standard_Integer nnn = aTr->NbTriangles();
        Standard_Integer nt,n1,n2,n3;
        for ( nt = 1 ; nt < nnn+1 ; nt++)
        {
            // takes the node indices of each triangle in n1,n2,n3:
            triangles(nt).Get(n1,n2,n3);
            // takes the node points:
            gp_Pnt aPnt1 = aPoints(n1);
            Points[0].Set(float(aPnt1.X()),float(aPnt1.Y()),float(aPnt1.Z()));
            gp_Pnt aPnt2 = aPoints(n2);
            Points[1].Set(aPnt2.X(),aPnt2.Y(),aPnt2.Z());
            gp_Pnt aPnt3 = aPoints(n3);
            Points[2].Set(aPnt3.X(),aPnt3.Y(),aPnt3.Z());
            // give the occ faces to the internal mesh structure of freecad
            MeshCore::MeshGeomFacet Face(Points[0],Points[1],Points[2]);
            builder.AddFacet(Face);
        }

    }
    // if the triangulation of only one face is not possible to get
    else
    {
        throw Base::Exception("Empty face triangulation\n");
    }

    // finish FreeCAD Mesh Builder and exit with new mesh
    builder.Finish();
    return true;
}

#include <BRepMeshAdapt_IncrementalMesh.hxx>
#include <BRepMesh.hxx>
bool best_fit::Tesselate_Shape(const TopoDS_Shape &shape, MeshCore::MeshKernel &mesh, float deflection)
{
    Base::Builder3D aBuild;

	MeshCore::MeshDefinitions::_fMinPointDistanceD1 = 0.001;
    MeshCore::MeshBuilder builder(mesh);
    builder.Initialize(1000);
    Base::Vector3f Points[3];

    //bool check = BRepUtils::CheckTopologie(shape);
    //if (!check)
    //{
    //    cout <<"an error"<< endl;
    //}

    //BRepBuilderAPI_Sewing aSewer;
    //aSewer.Load(shape);
    //aSewer.Perform();
    //aSewer.IsModified(shape);
    //const TopoDS_Shape& asewedShape = aSewer.SewedShape();
    //if (asewedShape.IsNull())
    //{
    //    cout << "Nothing Sewed" << endl;
    //}
    //int test = aSewer.NbFreeEdges();
    //int test1 = aSewer.NbMultipleEdges();
    //int test2 = aSewer.NbDegeneratedShapes();

    

    // adds a triangulation of the shape aShape with the deflection deflection:

    TriangleAdapt_Parameters MeshParams;
    MeshParams._minAngle = 30.0;
    MeshParams._minNbPntsPerEdgeLine = 10;
    MeshParams._minNbPntsPerEdgeOther = 10;
    MeshParams._minEdgeSplit = 5;
	BRepMesh::Mesh(shape,deflection);

    TopExp_Explorer aExpFace;

    for (aExpFace.Init(shape,TopAbs_FACE);aExpFace.More();aExpFace.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
        TopLoc_Location aLocation;

        // takes the triangulation of the face aFace:
        Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace,aLocation);
        if (!aTr.IsNull()) // if this triangulation is not NULL
        {
            // takes the array of nodes for this triangulation:
            const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
            // takes the array of triangles for this triangulation:
            const Poly_Array1OfTriangle& triangles = aTr->Triangles();
            // create array of node points in absolute coordinate system
            TColgp_Array1OfPnt aPoints(1, aNodes.Length());
            for ( Standard_Integer i = 1; i <= aNodes.Length(); i++)
                aPoints(i) = aNodes(i).Transformed(aLocation);
            // Takes the node points of each triangle of this triangulation.
            // takes a number of triangles:
            Standard_Integer nnn = aTr->NbTriangles();
            Standard_Integer nt,n1,n2,n3;
            for ( nt = 1 ; nt < nnn+1 ; nt++)
            {
                // takes the node indices of each triangle in n1,n2,n3:
                triangles(nt).Get(n1,n2,n3);
                // takes the node points:
                gp_Pnt aPnt1 = aPoints(n1);
                Points[0].Set(float(aPnt1.X()),float(aPnt1.Y()),float(aPnt1.Z()));
                gp_Pnt aPnt2 = aPoints(n2);
                Points[1].Set(aPnt2.X(),aPnt2.Y(),aPnt2.Z());
                gp_Pnt aPnt3 = aPoints(n3);
                Points[2].Set(aPnt3.X(),aPnt3.Y(),aPnt3.Z());
                // give the occ faces to the internal mesh structure of freecad
                MeshCore::MeshGeomFacet Face(Points[0],Points[1],Points[2]);
                builder.AddFacet(Face);
            }
        }
        // if the triangulation of only one face is not possible to get
        else
        {
            throw Base::Exception("Empty face triangulation\n");
        }
    }
    // finish FreeCAD Mesh Builder and exit with new mesh
    builder.Finish();

    /*MeshCore::MeshAlgorithm algo(mesh);
    std::list< std::vector <unsigned long> > BoundariesIndex;
    algo.GetMeshBorders(BoundariesIndex);*/

    return true;
}

std::vector<Base::Vector3f> best_fit::Comp_Normals(MeshCore::MeshKernel &M, std::vector<unsigned long> &Ind)
{
    //Base::Builder3D log3d;
    Base::Vector3f normal,local_normal;
    MeshCore::MeshRefPointToFacets rf2pt(M);
    MeshCore::MeshGeomFacet        t_face;
    std::vector<Base::Vector3f>    normals, origPoint;

    int NumOfPoints = Ind.size();

    for (int i=0; i<NumOfPoints; ++i)
    {
        // Satz von Dreiecken zu jedem Punkt
        const std::set<MeshCore::MeshFacetArray::_TConstIterator>& faceSet = rf2pt[Ind[i]];
        float fArea = 0.0;
        normal.Set(0.0,0.0,0.0);

        // Iteriere über die Dreiecke zu jedem Punkt
        for (std::set<MeshCore::MeshFacetArray::_TConstIterator>::const_iterator it = faceSet.begin(); it != faceSet.end(); ++it)
        {
            // Zweimal derefernzieren, um an das MeshFacet zu kommen und dem Kernel uebergeben, dass er ein MeshGeomFacet liefert
            t_face = M.GetFacet(**it);
            // Flaecheninhalt aufsummieren
            float local_Area = t_face.Area();
            local_normal = t_face.GetNormal();
            if (local_normal.z < 0)
            {
                local_normal = local_normal * (-1);
            }

            fArea  = fArea  + local_Area;
            normal = normal + local_normal;

        }

        normals.push_back(normal);

        //log3d.addSingleArrow(origPoint,origPoint+normal,1,0,0,0);
    }

    //log3d.saveToFile("c:/normals.iv");

    return normals;
}

std::vector<Base::Vector3f> best_fit::Comp_Normals(MeshCore::MeshKernel &M)
{
    Base::Builder3D log3d;
    Base::Vector3f normal,local_normal,origPoint;
    MeshCore::MeshRefPointToFacets rf2pt(M);
    MeshCore::MeshGeomFacet        t_face;
    MeshCore::MeshPoint mPnt;
    std::vector<Base::Vector3f>    normals;

    int NumOfPoints = M.CountPoints();
    float local_Area;
    float fArea;

    for (int i=0; i<NumOfPoints; ++i)
    {
        // Satz von Dreiecken zu jedem Punkt
        mPnt = M.GetPoint(i);
        origPoint.x = mPnt.x;
        origPoint.y = mPnt.y;
        origPoint.z = mPnt.z;

        const std::set<MeshCore::MeshFacetArray::_TConstIterator>& faceSet = rf2pt[i];
        fArea = 0.0;
        normal.Set(0.0,0.0,0.0);

        // Iteriere über die Dreiecke zu jedem Punkt
        for (std::set<MeshCore::MeshFacetArray::_TConstIterator>::const_iterator it = faceSet.begin(); it != faceSet.end(); ++it)
        {
            // Zweimal derefernzieren, um an das MeshFacet zu kommen und dem Kernel uebergeben, dass er ein MeshGeomFacet liefert
            t_face = M.GetFacet(**it);
            // Flaecheninhalt aufsummieren
            local_Area = t_face.Area();
            local_normal = t_face.GetNormal();
            if (local_normal.z < 0)
            {
                local_normal = local_normal * (-1);
            }

            fArea  = fArea  + local_Area;
            normal = normal + local_Area*local_normal;

        }

        normal.Normalize();
        normals.push_back(normal);

        log3d.addSingleArrow(origPoint,origPoint+normal,1,0,0,0);
    }

    log3d.saveToFile("c:/normals.iv");

    return normals;
}

double best_fit::CompError(std::vector<Base::Vector3f> &pnts,  std::vector<Base::Vector3f> &normals,
                           std::vector<Base::Vector3f> &bpnts, std::vector<Base::Vector3f> &bnormals)
{
    double err_avg = 0.0;
    double err_max = 0.0;
    double sqrdis  = 0.0;
    double weight  = 3*thin/b_thin;

    MeshCore::MeshFacetGrid aFacetGrid(m_CadMesh);
    MeshCore::MeshAlgorithm malg(m_CadMesh);
    MeshCore::MeshAlgorithm malg2(m_CadMesh);

    Base::Vector3f origPoint, projPoint, distVec;
    unsigned long  facetIndex;

    int NumOfPoints1 = pnts.size();
    int c = 0;

    for (int i=0; i<NumOfPoints1; ++i)
    {
        if (!malg.NearestFacetOnRay(pnts[i], normals[i], aFacetGrid, projPoint, facetIndex))  // gridoptimiert
        {
            if (malg2.NearestFacetOnRay(pnts[i], normals[i], projPoint, facetIndex))
            {
                distVec  = projPoint-pnts[i];
                sqrdis   = distVec*distVec;
                err_avg += sqrdis;
            }
            else
                ++c;
        }
        else
        {
            distVec  = projPoint-pnts[i];
            sqrdis   = distVec*distVec;
            err_avg += sqrdis;
        }
    }

    int NumOfPoints2 = bpnts.size();

    for (int i=0; i<NumOfPoints2; ++i)
    {
        if (!malg.NearestFacetOnRay(bpnts[i], bnormals[i], aFacetGrid, projPoint, facetIndex))  // gridoptimiert
        {
            if (malg2.NearestFacetOnRay(bpnts[i], bnormals[i], projPoint, facetIndex))
            {
                distVec  = projPoint-bpnts[i];
                sqrdis   = distVec*distVec;
                err_avg += weight*sqrdis;
            }
            else
                c += weight;
        }
        else
        {
            distVec  = projPoint-bpnts[i];
            sqrdis   = distVec*distVec;
            err_avg += weight*sqrdis;
        }
    }

    int numAll = NumOfPoints1 + weight*NumOfPoints2;

    if (c>(2*numAll/3))
        return 1e+10;

    return err_avg/(numAll-c);
}


double best_fit::CompError(std::vector<Base::Vector3f> &pnts, std::vector<Base::Vector3f> &normals)
{
    double err_avg = 0.0;
    double err_max = 0.0;
    double sqrdis = 0.0;

    MeshCore::MeshFacetGrid aFacetGrid(m_CadMesh);
    MeshCore::MeshAlgorithm malg(m_CadMesh);
    MeshCore::MeshAlgorithm malg2(m_CadMesh);

    Base::Vector3f origPoint, projPoint, distVec;
    unsigned long  facetIndex;

    int NumOfPoints = pnts.size();
    int c = 0;

    for (int i=0; i<NumOfPoints; ++i)
    {
        if (!malg.NearestFacetOnRay(pnts[i], normals[i], aFacetGrid, projPoint, facetIndex))  // gridoptimiert
        {
            if (malg2.NearestFacetOnRay(pnts[i], normals[i], projPoint, facetIndex))
            {
                distVec  = projPoint-pnts[i];
                sqrdis   = distVec*distVec;
                err_avg += sqrdis;
            }
            else
                ++c;
        }
        else
        {
            distVec  = projPoint-pnts[i];
            sqrdis   = distVec*distVec;
            err_avg += sqrdis;
        }
    }

    if (c==NumOfPoints)
        return 1e+10;

    return err_avg/(NumOfPoints-c);
}

double best_fit::CompError(std::vector<Base::Vector3f> &pnts, std::vector<Base::Vector3f> &normals, bool plot)
{
    if (plot==false)
        return CompError(pnts, normals);
    else
    {
        Base::Builder3D log3d;
        double err_avg = 0.0;
        double err_max = 0.0;
        double sqrdis  = 0.0;


        MeshCore::MeshFacetGrid aFacetGrid(m_CadMesh);
        MeshCore::MeshAlgorithm malg(m_CadMesh);
        MeshCore::MeshAlgorithm malg2(m_CadMesh);

        Base::Vector3f projPoint, distVec;
        unsigned long  facetIndex;

        int NumOfPoints = pnts.size();
        int c=0;

        for (int i=0; i<NumOfPoints; ++i)
        {
            if (!malg.NearestFacetOnRay(pnts[i], normals[i], aFacetGrid, projPoint, facetIndex))   // gridoptimiert
            {
                if (malg2.NearestFacetOnRay(pnts[i], normals[i], projPoint, facetIndex))
                {

                    log3d.addSingleArrow(pnts[i],projPoint, 3, 0,0,0);
                    distVec  = projPoint-pnts[i];
                    sqrdis   = distVec*distVec;
                    err_avg += sqrdis;
                }
                else
                    c++;

            }
            else
            {
                log3d.addSingleArrow(pnts[i],projPoint, 3, 0,0,0);
                distVec  = projPoint-pnts[i];
                sqrdis   = distVec*distVec;
                err_avg += sqrdis;
            }
        }

        log3d.saveToFile("c:/projection.iv");

        if (c>(NumOfPoints/2))
            return 1e+10;

        return err_avg/(NumOfPoints-c);
    }
}

double best_fit::CompTotalError()
{
    Base::Builder3D log3d;
    double err_avg = 0.0;
    double err_max = 0.0;
    double sqrdis  = 0.0;

    MeshCore::MeshFacetGrid aFacetGrid(m_Mesh);
    MeshCore::MeshAlgorithm malg(m_Mesh);
    MeshCore::MeshAlgorithm malg2(m_Mesh);
    MeshCore::MeshPointIterator p_it(m_CadMesh);

    Base::Vector3f projPoint, distVec, projPoint2;
    unsigned long  facetIndex;

    m_error.resize(m_CadMesh.CountPoints());

    int c=0;

    for (p_it.Begin(); p_it.More(); p_it.Next())
    {
        if (!malg.NearestFacetOnRay(*p_it, m_normals[p_it.Position()], aFacetGrid, projPoint, facetIndex))   // gridoptimiert
        {
            if (malg2.NearestFacetOnRay(*p_it, m_normals[p_it.Position()], projPoint, facetIndex))
            {

                log3d.addSingleArrow(*p_it, projPoint, 3, 0,0,0);
                distVec  = projPoint - *p_it;
                sqrdis   = distVec*distVec;

                if (((projPoint.z - p_it->z) / m_normals[p_it.Position()].z ) > 0)
                    m_error[p_it.Position()] = sqrt(sqrdis);
                else
                    m_error[p_it.Position()] = -sqrt(sqrdis);

                err_avg += sqrdis;
            }
            else
            {
                c++;
                m_FailProj.push_back(p_it.Position());
            }
        }
        else
        {
            distVec  = projPoint-*p_it;
            sqrdis   = distVec*distVec;

            m_normals[p_it.Position()].Scale(-1,-1,-1);

            if (malg.NearestFacetOnRay(*p_it, m_normals[p_it.Position()], aFacetGrid, projPoint2, facetIndex))
            {
                distVec  = projPoint2-*p_it;
                if (sqrdis > distVec*distVec)
                {
                    sqrdis   = distVec*distVec;
                    log3d.addSingleArrow(*p_it, projPoint2, 3, 0,0,0);
                }
                else
                {
                    log3d.addSingleArrow(*p_it, projPoint, 3, 0,0,0);
                }

            }
            m_normals[p_it.Position()].Scale(-1,-1,-1);

            if (((projPoint.z - p_it->z) / m_normals[p_it.Position()].z ) > 0)
                m_error[p_it.Position()] = sqrt(sqrdis);
            else
                m_error[p_it.Position()] = -sqrt(sqrdis);

            err_avg += sqrdis;
        }
    }


    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator v_it;
    MeshCore::MeshRefPointToPoints vv_it(m_CadMesh);

    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;

    for (unsigned int i=0; i<m_FailProj.size(); ++i)
    {
        PntNei = vv_it[m_FailProj[i]];
        m_error[m_FailProj[i]] = 0.0;

        for (v_it = PntNei.begin(); v_it !=PntNei.end(); ++v_it)
        {
            m_error[m_FailProj[i]] += m_error[(*v_it)[0]._ulProp];
        }

        m_error[m_FailProj[i]] /= double(PntNei.size());
        PntNei.clear();
    }


    log3d.saveToFile("c:/projection.iv");

    if (c>(m_CadMesh.CountPoints()/2))
        return 1e+10;

    return err_avg/(m_CadMesh.CountPoints()-c);
}

bool best_fit::Coarse_correction()
{
    std::vector<Base::Vector3f> normals = m_normals;
    std::vector<Base::Vector3f> pnts    = m_pnts;

    double error, error_tmp, rot = 0.0;
    Base::Matrix4D M_z;

    // 180°- rotation um z-Achse
    RotMat(M_z, rstep_corr, 3);

    error = CompError(pnts, normals);  // startfehler

    int n=360/rstep_corr;

    for (int i=1; i<n; ++i)
    {
        MeshTransform(pnts, normals, M_z);
        error_tmp = CompError(pnts, normals);

        if (error_tmp < error)
        {
            error = error_tmp;
            rot = i*rstep_corr;
        }
    }

    if (rot != 0.0)
    {
        RotMat(M_z, rot, 3);

        MeshTransform(m_pnts, m_normals, M_z);
        m_Mesh.Transform(M_z);
    }

    CompError(m_pnts, m_normals, true);

    normals.clear();
    pnts.clear();

    return true;
}

bool best_fit::Fit_iter()
{
    double ref_trans, ref_rot = aref_rot;
    double trans_step, rot_step;

    int m = m_pntInd.size();
    int n = m_Mesh.CountPoints();

    double err, err_tmp, err_it = 1e+10;

    std::vector<Base::Vector3f> normals_tmp;
    std::vector<Base::Vector3f> pnts_tmp;

    Base::BoundBox3f bbox = m_Mesh.GetBoundBox();

    double xlen = bbox.LengthX();
    double ylen = bbox.LengthY();

    if (xlen<=ylen)
        ref_trans = ylen*tpart;
    else
        ref_trans = xlen*tpart;


    cout << "search area: translation: " << -ref_trans << " to " << ref_trans << endl;
    cout << "             rotation   : " << -ref_rot   << " to " << ref_rot << endl;


    Base::Matrix4D Mx,My,Mz,Mx_ref,My_ref,Mz_ref,   // Rotationsmatrizen
    Tx,Ty,Tz,Tx_ref,Ty_ref,Tz_ref,   // Translationsmatrizen
    M_fin,M;


    std::cout << "start fitting in 3 dimensions ....." << std::endl;

    trans_step = 2 * ref_trans / ntstep;
    rot_step   = 2 * ref_rot   / nrstep;

    while (true)
    {

        err = CompError(m_pnts, m_normals);  // startfehler
        cout << "startfehler: " << sqrt(err) << endl;

        // übergebe referenz
        pnts_tmp    = m_pnts;
        normals_tmp = m_normals;

        M_fin.unity();

        for (int n3=1; n3<(nrstep+1); ++n3)
        {
            for (int n4=1; n4<(ntstep+1); ++n4)
            {
                for (int n5=1; n5<(ntstep+1); ++n5)
                {
                    std::cout << "\r" << ceil((100.0*( (double) ((n3-1)*ntstep*ntstep + (n4-1)*ntstep + (n5-1))/((double) (nrstep*ntstep*ntstep))))) << "%" << " ";

                    TransMat(Tx, n4*trans_step - ref_trans,1);
                    TransMat(Ty, n5*trans_step - ref_trans,2);
                    RotMat  (Mz, n3*rot_step   - ref_rot,  3);

                    M = Tx*Ty*Mz;  // rotiere zuerst !!!

                    if (n4==1 && n5==1)
                        MeshTransform(pnts_tmp, normals_tmp, M);
                    else
                        MeshTransform(pnts_tmp, M);

                    err_tmp = CompError(pnts_tmp, normals_tmp);

                    if (err_tmp < err)
                    {
                        M_fin = M;
                        err = err_tmp;

                        cout << "error: " << sqrt(err_tmp);
                        cout << " at "    << -ref_rot + n3*rot_step << ", " <<  -ref_trans + n4*trans_step << ", " << -ref_trans + n5*trans_step << endl;
                    }

                    pnts_tmp    = m_pnts;
                    normals_tmp = m_normals;
                }
            }
        }

        MeshTransform(m_pnts, m_normals, M_fin);
        MeshTransform(bpnts,  bnormals,  M_fin);
        m_Mesh.Transform(M_fin);
        err = CompError(m_pnts, m_normals, true);

        if ( (err_it - err) < TOL2 || trans_step < TOL1)
            break;

        err_it = err;
        std::cout << "average error: " <<  sqrt(err) << " mm" << std::endl;

        ref_trans  = trans_step;
        ref_rot    = rot_step;
        trans_step = 2 * ref_trans / ntstep;
        rot_step   = 2 * ref_rot   / nrstep;
    }

    err = CompError(m_pnts, m_normals, true);
    std::cout << "final average error: " << sqrt(err) << " mm" << std::endl;


    // correction of z-value

    std::vector<Base::Vector3f> tmp_bpnts;
    std::vector<Base::Vector3f> tmp_bnormals;

    double berr_1, berr_2, err_1;

    cout << "compute new thinning..." << endl;
    thinning(4*thin);
    m_normals.clear();
    m_normals = Comp_Normals(m_Mesh, m_pntInd);


    ref_trans = 2.5;    // do correction from [-5/2, +5/2] with 1/10 mm steps
    trans_step = 0.1;
    err = 1e+10;
    pnts_tmp.clear();

    berr_1 = CompError(bpnts,bnormals);
    err_1  = CompError(m_pnts, m_normals);

    cout << "startfehler - z-correction" << sqrt(CompError(m_pnts, m_normals)) << endl;
    cout << "startfehler - z-correction bound" << sqrt(CompError(bpnts,bnormals)) << endl;
    cout << "start z-trans correction... " << endl;

    while (true)
    {

        TransMat(Tz, -ref_trans, 3);
        m_Mesh.Transform(Tz);
        ZTranslation(m_pnts, -ref_trans);
        ZTranslation(bpnts,  -ref_trans);
        double trans_fin = ref_trans;

        pnts_tmp  = m_pnts;
        tmp_bpnts = bpnts;

        for (int n=1; n<51; ++n)
        {

            ZTranslation(m_pnts, n*trans_step);
            ZTranslation(bpnts,  n*trans_step);
            err_tmp = CompError(m_pnts, m_normals, bpnts, bnormals);

            /*
            err_tmp = CompError(bpnts, bnormals);
            cout << " error bound: " << sqrt(err_tmp)<< endl;

            err_tmp = CompError(m_pnts, m_normals);*/

            cout << " error: " << sqrt(err_tmp)<< endl;

            if (err_tmp < err)
            {
                trans_fin = n*trans_step;
                err = err_tmp;

                //cout << " error: " << sqrt(err_tmp)<< " at " << -ref_trans + n*trans_step << endl;
            }

            m_pnts = pnts_tmp;
            bpnts  = tmp_bpnts;
        }

        ZTranslation(m_pnts, trans_fin);
        ZTranslation(bpnts, trans_fin);
        berr_2 = CompError(bpnts, bnormals);
        //err = CompError(m_pnts, m_normals, bpnts, bnormals);
        err_tmp = CompError(m_pnts, m_normals);

        cout << "quotient: " << (berr_1-berr_2)/(err_1 - err_tmp) << endl;
        if ((berr_1-berr_2)/(err_1 - err_tmp) > -2)
        {
            TransMat(M_fin, trans_fin, 3);
            m_Mesh.Transform(M_fin);
            cout << "do correction" << endl;
        }
        else
        {
            ZTranslation(m_pnts, -trans_fin);
            ZTranslation(bpnts,  -trans_fin);
        }

        std::cout << "average error: " << sqrt(err) << " mm" << std::endl;
        cout << "endfehler - z-correction" << sqrt(CompError(m_pnts, m_normals)) << endl;
        cout << "endfehler - z-correction bound" << sqrt(CompError(bpnts, bnormals)) << endl;
        break;
    }



    ref_trans = 5;
    ref_rot   = 5;

    double ntstep1 = 4;
    double nrstep1 = 4;

    trans_step = 2 * ref_trans / ntstep1;
    rot_step   = 2 * ref_rot   / nrstep1;

    err_it = 1e+10;

    while (true)
    {

        err = CompError(m_pnts, m_normals);  // startfehler
        cout << "startfehler: " << sqrt(err) << endl;

        // übergebe referenz
        pnts_tmp    = m_pnts;
        normals_tmp = m_normals;

        tmp_bpnts    = bpnts;
        tmp_bnormals = bnormals;

        M_fin.unity();

        for (int n3=1; n3<(nrstep1+1); ++n3)
        {
            for (int n4=1; n4<(ntstep1+1); ++n4)
            {
                for (int n5=1; n5<(ntstep1+1); ++n5)
                {
                    std::cout << "\r" << ceil((100.0*( (double) ((n3-1)*ntstep1*ntstep1 + (n4-1)*ntstep1 + (n5-1))/((double) (nrstep1*ntstep1*ntstep1))))) << "%" << " ";

                    TransMat(Tx, n4*trans_step - ref_trans,1);
                    TransMat(Ty, n5*trans_step - ref_trans,2);
                    RotMat  (Mz, n3*rot_step   - ref_rot,  3);

                    M = Tx*Ty*Mz;  // rotiere zuerst !!!

                    if (n4==1 && n5==1)
                    {
                        MeshTransform(pnts_tmp, normals_tmp, M);
                        MeshTransform(tmp_bpnts, tmp_bnormals, M);
                    }
                    else
                    {
                        MeshTransform(pnts_tmp, M);
                        MeshTransform(tmp_bpnts, M);
                    }

                    err_tmp = CompError(pnts_tmp, normals_tmp);

                    if (err_tmp < err)
                    {
                        M_fin = M;
                        err = err_tmp;

                        cout << "error: " << sqrt(err_tmp);
                        cout << " at "    << -ref_rot + n3*rot_step << ", " <<  -ref_trans + n4*trans_step << ", " << -ref_trans + n5*trans_step << endl;
                    }

                    pnts_tmp    = m_pnts;
                    normals_tmp = m_normals;

                    tmp_bpnts    = bpnts;
                    tmp_bnormals = bnormals;
                }
            }
        }

        MeshTransform(m_pnts, m_normals, M_fin);
        MeshTransform(bpnts, bnormals, M_fin);
        m_Mesh.Transform(M_fin);
        err = CompError(m_pnts, m_normals);

        if ( (err_it - err) < TOL2 || trans_step < TOL1)
            break;

        err_it = err;
        std::cout << "average error: " <<  sqrt(err) << " mm" << std::endl;

        ref_trans  = trans_step;
        ref_rot    = rot_step;
        trans_step = 2 * ref_trans / ntstep;
        rot_step   = 2 * ref_rot   / nrstep;
    }

    err = CompError(pnts_tmp, normals_tmp);
    std::cout << "final average error: " << sqrt(err) << " mm" << std::endl;

    // shape zurück zur ausgangsposition
    gp_Trsf trafo;

    trafo.SetTranslation(-m_cad2orig);
    BRepBuilderAPI_Transform trsf(trafo);

    trsf.Perform(m_Cad);
    m_Cad = trsf.Shape();

    // Mesh ebenfalls rückverschieben
    M_fin.unity();

    M_fin[0][3] = -m_cad2orig.X();
    M_fin[1][3] = -m_cad2orig.Y();
    M_fin[2][3] = -m_cad2orig.Z();

    m_Mesh.Transform(M_fin);

    return true;
}


