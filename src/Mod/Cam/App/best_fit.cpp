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


#include "PreCompiled.h"
#include "best_fit.h"
#include "routine.h"
#include <strstream>

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

#include <ANN/ANN.h> // ANN declarations


best_fit::best_fit()
{
    m_LSPnts.resize(2);
}

best_fit::~best_fit()
{
}

void best_fit::Load(const MeshCore::MeshKernel &mesh,const TopoDS_Shape &cad)
{
    m_Mesh = mesh;
    m_Cad  = cad;

    m_MeshWork = m_Mesh;

}


double best_fit::ANN()
{
    ANNpointArray   dataPts;                // data points
    ANNpoint        queryPt;                // query point
    ANNidxArray     nnIdx;                    // near neighbor indices
    ANNdistArray    dists;                    // near neighbor distances
    ANNkd_tree*     kdTree;                    // search structure

    MeshCore::MeshPointArray meshPnts = m_MeshWork.GetPoints();
    Base::Vector3f projPoint;

    double error = 0.0;

    int a_dim = 3;
    int a_nbPnts =(int) meshPnts.size(); //Size vom eingescanntem Netz
    int a_nbNear = 1;               //anzahl der r�ckgabewerte
    queryPt = annAllocPt(a_dim);                    // allocate query point storage
    dataPts = annAllocPts(a_nbPnts, a_dim);         // allocate data points storage
    nnIdx = new ANNidx[a_nbNear];                   // allocate near neigh indices
    dists = new ANNdist[a_nbNear];                  // allocatenear neighbor dists

    m_LSPnts[0].clear();
    m_LSPnts[1].clear();

    for (int i=0; i<a_nbPnts; ++i)
    {
        dataPts[i][0] = meshPnts[i].x;
        dataPts[i][1] = meshPnts[i].y;
        dataPts[i][2] = meshPnts[i].z;
    }

    kdTree = new ANNkd_tree(        // build search structure
        dataPts,                    // the data points
        a_nbPnts,            // number of points
        a_dim);                     // dimension of space


    for (unsigned int i = 0 ; i < m_pnts.size() ; i++ )
    {
        queryPt[0] = m_pnts[i].x;
        queryPt[1] = m_pnts[i].y;
        queryPt[2] = m_pnts[i].z;

        kdTree->annkSearch(                        // search
            queryPt,                        // query point
            a_nbNear,                        // number of near neighbors
            nnIdx,                            // nearest neighbors (returned)
            dists                           // distance (returned)
        );                            // error bound

        m_LSPnts[1].push_back(m_pnts[i]);
        m_LSPnts[0].push_back(meshPnts[nnIdx[0]]);

        error += dists[0];

    }

    error /= double(m_pnts.size());
    m_weights_loc = m_weights;


    delete [] nnIdx; // clean things up
    delete [] dists;
    delete kdTree;
    annClose(); // done with ANN

    return error;
}

bool best_fit::Perform()
{
    Base::Matrix4D M;

    cout << "tesselate shape" << endl;
    Tesselate_Shape(m_Cad, m_CadMesh, 1);
    Comp_Weights();

    cout << "trafo2origin" << endl;
    MeshFit_Coarse();
    ShapeFit_Coarse();


    M.unity();
    M[0][3] = m_cad2orig.X();
    M[1][3] = m_cad2orig.Y();
    M[2][3] = m_cad2orig.Z();

    m_CadMesh.Transform(M);
    PointTransform(m_pnts,M);

    cout << "error: " << ANN() << endl;;

    Coarse_correction();

    time_t seconds1, seconds2;
    seconds1 = time(NULL);

    cout << "Least-Square-Matching" << endl;
    LSM();

    seconds2 = time(NULL);
    cout << "laufzeit: " << seconds2-seconds1 << " sec" << endl;

    Base::Matrix4D T;
    T.unity();
    T[0][3] = -m_cad2orig.X();
    T[1][3] = -m_cad2orig.Y();
    T[2][3] = -m_cad2orig.Z();
    m_MeshWork.Transform(T);
    m_CadMesh.Transform(T);

    return true;


}

bool best_fit::Intersect(const Base::Vector3f &normal,const MeshCore::MeshKernel &mesh, Base::Vector3f &P, Base::Vector3f &I)
{
    MeshCore::MeshFacetIterator f_it(mesh);

    for (f_it.Begin(); f_it.More(); f_it.Next())
    {
        if (intersect_RayTriangle(normal, *f_it, P, I) == 1)
            return true;
    }

    return false;
}

int best_fit::intersect_RayTriangle(const Base::Vector3f &normal,const MeshCore::MeshGeomFacet &T, Base::Vector3f &P, Base::Vector3f &I)
{
    Base::Vector3f    u, v, n, null(0.0,0.0,0.0), J;          // triangle vectors
    Base::Vector3f    dir, w0, w;                             // ray vectors
    float             r, a, b;                                // params to calc ray-plane intersect

    J = P+normal;

    // get triangle edge vectors and plane normal
    u = T._aclPoints[1] - T._aclPoints[0];
    v = T._aclPoints[2] - T._aclPoints[0];
    n = u % v;                // cross product
    if (n == null)            // triangle is degenerate
        return -1;            // do not deal with this case

    dir = normal;    // ray direction vector
    w0  = P - T._aclPoints[0];
    a   = n * w0;
    b   = n * dir;
    if (fabs(b) < SMALL_NUM)      // ray is parallel to triangle plane
    {
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = -a / b;

    //if (r < 0.0)                   // ray goes away from triangle
    //    return 0;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect

    dir.Scale(r,r,r);
    I = P + dir;              // intersect point of ray and plane

    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = u*u;
    uv = u*v;
    vv = v*v;
    w = I - T._aclPoints[0];
    wu = w*u;
    wv = w*v;
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                      // I is in T
}

bool best_fit::Coarse_correction()
{
    double error, error_tmp, rot = 0.0;
    Base::Matrix4D M,T;

    MeshCore::MeshKernel MeshCopy = m_MeshWork;

    T.unity();

    //error = CompError_GetPnts(m_pnts, m_normals)[0];  // startfehler    int n=360/rstep_corr;
    error = ANN();

    for (int i=1; i<4; ++i)
    {

        RotMat(M, 180, i);
        m_MeshWork.Transform(M);

        //error_tmp = CompError_GetPnts(m_pnts, m_normals)[0];
        error_tmp = ANN();

        if (error_tmp < error)
        {
            T = M;
            error = error_tmp;
        }

        m_MeshWork = MeshCopy;
    }

    m_MeshWork.Transform(T);

    return true;
}

// Least Square Matching bzgl. Rotation um z-Achse und Translation in (x,y)-Richtung
bool best_fit::LSM()
{
    double TOL  = 0.01;          // Abbruchkriterium des Newton-Verfahren
    int maxIter = 200;           // maximale Anzahl von Iterationen f�r den Fall,
    // dass das Abbruchkriterium nicht erf�llt wird

    double val, tmp;
    Base::Matrix4D Tx,Ty,Tz,Rx,Ry,Rz,M;   // Transformaitonsmatrizen

    int c=0;
    int mult;

    std::vector<double> errVec(2,ERR_TOL+1);
    std::vector<double> del_x(3,0.0);
    std::vector<double>     x(3,0.0); // Startparameter entspricht Nullvektor

    Base::Vector3f centr_l,centr_r;  // Schwerpunkte der Punktes�tze

    // Newton Verfahren: 1. L�se  H*del_x = -J
    //                   2. Setze x = x + del_x

    std::vector<double> Jac(3);           // 1.Ableitung der Fehlerfunktion (Jacobi-Matrix)
    std::vector< std::vector<double> > H; // 2.Ableitung der Fehlerfunktion (Hesse-Matrix)

    time_t seconds1, seconds2;
    seconds1 = time(NULL);

    while (true)
    {

        seconds1 = time(NULL);

        if (c==maxIter || errVec[0] < ERR_TOL) break; // Abbruchkriterium

        /*
        time_t seconds1, seconds2;
           seconds1 = time(NULL);
        */

        // Fehlerberechnung vom CAD -> Mesh
        //errVec = CompError_GetPnts(m_pnts, m_normals);   // hier: - Berechnung der LS-Punktes�tze
        //       - Berechnung der zugeh�rigen Gewichtungen

        errVec[0] = ANN();
        /*
        seconds2 = time(NULL);
              cout << "laufzeit projection: " << seconds2-seconds1 << " sec" << endl;
        */

        seconds2 = time(NULL);
        std::cout << "Iter.: " << c << " AVG. : " << errVec[0] << "   " << "MAX. : " << errVec[1] <<  " Time: " <<  seconds2-seconds1 << " sec" << endl;
        seconds1 = time(NULL);

        for (unsigned int i=0; i<x.size(); ++i) x[i] = 0.0; // setze startwerte auf null

        // Berechne gewichtete Schwerpunkte und verschiebe die Punktes�tze entsprechend:
        centr_l.Scale(0.0,0.0,0.0);
        centr_r.Scale(0.0,0.0,0.0);

        Base::Vector3f p,q;

        for (unsigned int i=0; i<m_LSPnts[0].size(); ++i)
        {
            p = m_LSPnts[0][i];
            q = m_LSPnts[1][i];

            p.Scale((float) m_weights_loc[i],(float) m_weights_loc[i],(float) m_weights_loc[i]);
            q.Scale((float) m_weights_loc[i],(float) m_weights_loc[i],(float) m_weights_loc[i]);

            centr_l += p;
            centr_r += q;
        }

        float s = (float) m_LSPnts[0].size();
        s = float(-1.0/s);

        centr_l.Scale(s,s,s);
        centr_r.Scale(s,s,s);

        // Verschiebung der Schwerpunkte zum Ursprung
        TransMat(Tx,centr_l.x,1);
        TransMat(Ty,centr_l.y,2);
        TransMat(Tz,centr_l.z,3);

        M = Tx*Ty*Tz;
        PointTransform(m_LSPnts[0],M);
        m_MeshWork.Transform(M);

        TransMat(Tx,centr_r.x,1);
        TransMat(Ty,centr_r.y,2);
        TransMat(Tz,centr_r.z,3);

        M = Tx*Ty*Tz;
        PointTransform(m_LSPnts[1],M);
        PointNormalTransform(m_pnts, m_normals, M);
        //m_CadMesh.Transform(M);

        tmp  = 1e+10;
        mult = 2;

        // Newton-Verfahren zur Berechnung der Rotationsmatrix:
        while (true)
        {

            Jac = Comp_Jacobi(x);  // berechne 1.Ableitung
            H   = Comp_Hess(x);    // berechne 2.Ableitung

            val = 0.0;
            for (unsigned int i=0; i<Jac.size(); ++i)
            {
                val += Jac[i]*Jac[i];
            }
            val = sqrt(val);

            if (val < TOL) break; // Abbruchkriterium des Newton-Verfahren

            if (val>tmp && mult < 1e+4)
            {
                for (unsigned int i=0; i<del_x.size(); ++i)
                {
                    x[i] -= del_x[i]/double(mult);  // Halbiere Schrittweite falls keine Verbesserung
                }
                mult *= 2;
                continue;
            }
            else
            {
                mult = 2;
            }

            tmp = val;

            del_x = Routines::NewtonStep(Jac,H);      // l�st Gl.system:          H*del_x = -J
            for (unsigned int i=0; i<x.size(); ++i)   // n�chster Iterationswert: x = x + del_x
            {
                x[i] += del_x[i];
            }
        }

        // Rotiere und verschiebe zur�ck zum Ursprung der !!! CAD-Geometrie !!!
        RotMat  (Rx,(x[0]*180.0/PI),1);
        RotMat  (Ry,(x[1]*180.0/PI),2);
        RotMat  (Rz,(x[2]*180.0/PI),3);
        TransMat(Tx, -centr_r.x, 1);
        TransMat(Ty, -centr_r.y, 2);
        TransMat(Tz, -centr_r.z, 3);

        M = Tx*Ty*Tz*Rx*Ry*Rz; // Rotiere zuerst !!! (Rotationen stets um den Nullpunkt...)
        m_MeshWork.Transform(M);

        M = Tx*Ty*Tz;
        //m_CadMesh.Transform(M);
        PointNormalTransform(m_pnts, m_normals, M);

        ++c;
    }

    /*TransMat(Tx,-centr_l.x,1);
       TransMat(Ty,-centr_l.y,2);
    TransMat(Tz,-centr_l.z,3);

    M = Tx*Ty*Tz;
    PointTransform(m_LSPnts[0],M);

    TransMat(Tx,-centr_r.x,1);
       TransMat(Ty,-centr_r.y,2);
    TransMat(Tz,-centr_r.z,3);

    M = Tx*Ty*Tz;
    PointTransform(m_LSPnts[1],M);

    Base::Builder3D log;

    for(unsigned int i=0; i<m_LSPnts[0].size(); ++i)
     log.addSingleArrow(m_LSPnts[0][i],m_LSPnts[1][i]);

    log.saveToFile("c:/newton_pnts.iv");*/

    return true;
}

std::vector<double> best_fit::Comp_Jacobi(const std::vector<double> &x)
{
    std::vector<double> F(3,0.0);
    double s1 = sin(x[0]), c1 = cos(x[0]);
    double s2 = sin(x[1]), c2 = cos(x[1]);
    double s3 = sin(x[2]), c3 = cos(x[2]);

    Base::Vector3f p,q;

    for (unsigned int i=0; i<m_LSPnts[0].size(); ++i)
    {
        p = m_LSPnts[0][i];
        q = m_LSPnts[1][i];

        F[0] += ( q.y * ( (-c1*s2*c3-s1*s3) * p.x +  (c1*s2*s3-s1*c3) * p.y + (-c1*c2) * p.z ) +
                  q.z * ( (-s1*s2*c3+c1*s3) * p.x +  (s1*s2*s3+c1*c3) * p.y + (-s1*c2) * p.z ) )*m_weights_loc[i];

        F[1] += ( q.x * (          (-s2*c3) * p.x +           (s2*s3) * p.y +    (-c2) * p.z ) +
                  q.y * (       (-s1*c2*c3) * p.x +        (s1*c2*s3) * p.y +  (s1*s2) * p.z ) +
                  q.z * (        (c1*c2*c3) * p.x +       (-c1*c2*s3) * p.y + (-c1*s2) * p.z ) )*m_weights_loc[i];

        F[2] += ( q.x * (          (-c2*s3) * p.x +          (-c2*c3) * p.y) +
                  q.y * (  (s1*s2*s3+c1*c3) * p.x +  (s1*s2*c3-c1*s3) * p.y) +
                  q.z * ( (-c1*s2*s3+s1*c3) * p.x + (-c1*s2*c3-s1*s3) * p.y) )*m_weights_loc[i];
    }

    return F;
}

std::vector<std::vector<double> > best_fit::Comp_Hess(const std::vector<double> &x)
{
    std::vector<std::vector<double> > DF(3);
    for (unsigned int i=0; i<DF.size(); ++i)
    {
        DF[i].resize(3,0.0);
    }

    double s1 = sin(x[0]), c1 = cos(x[0]);
    double s2 = sin(x[1]), c2 = cos(x[1]);
    double s3 = sin(x[2]), c3 = cos(x[2]);

    double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0, sum5 = 0.0, sum6 = 0.0;

    Base::Vector3f p,q;

    for (unsigned int i=0; i<m_LSPnts[0].size(); ++i)
    {
        p = m_LSPnts[0][i];
        q = m_LSPnts[1][i];

        sum1      = q.y * (  (s1*s2*c3-c1*s3) * p.x + (-s1*s2*s3-c1*c3) * p.y +  (s1*c2) * p.z ) +
                    q.z * ( (-c1*s2*c3-s1*s3) * p.x +  (c1*s2*s3-s1*c3) * p.y + (-c1*c2) * p.z );

        sum2      = q.x * (          (-c2*c3) * p.x +           (c2*s3) * p.y +     (s2) * p.z ) +
                    q.y * (        (s1*s2*c3) * p.x +       (-s1*s2*s3) * p.y +  (s1*c2) * p.z ) +
                    q.z * (       (-c1*s2*c3) * p.x +        (c1*s2*s3) * p.y + (-c1*c2) * p.z );

        sum3      = q.x * (          (-c2*c3) * p.x +           (c2*s3) * p.y) +
                    q.y * (  (s1*s2*c3-c1*s3) * p.x + (-s1*s2*s3-c1*c3) * p.y) +
                    q.z * ( (-c1*s2*c3-s1*s3) * p.x +  (c1*s2*s3-s1*c3) * p.y);

        sum4      = q.y * (       (-c1*c2*c3) * p.x +        (c1*c2*s3) * p.y +  (c1*s2) * p.z ) +
                    q.z * (       (-s1*c2*c3) * p.x +        (s1*c2*s3) * p.y +  (s1*s2) * p.z );

        sum5      = q.y * (  (c1*s2*s3-s1*c3) * p.x +  (c1*s2*c3+s1*s3) * p.y) +
                    q.z * (  (s1*s2*s3+c1*c3) * p.x +  (s1*s2*c3-c1*s3) * p.y);

        sum6      = q.x * (           (s2*s3) * p.x +           (s2*c3) * p.y) +
                    q.y * (        (s1*c2*s3) * p.x +        (s1*c2*c3) * p.y) +
                    q.z * (       (-c1*c2*s3) * p.x +       (-c1*c2*c3) * p.y);

        DF[0][0] += sum1*m_weights_loc[i];
        DF[1][1] += sum2*m_weights_loc[i];
        DF[2][2] += sum3*m_weights_loc[i];
        DF[0][1] += sum4*m_weights_loc[i];
        DF[1][0] += sum4*m_weights_loc[i];
        DF[0][2] += sum5*m_weights_loc[i];
        DF[2][0] += sum5*m_weights_loc[i];
        DF[1][2] += sum6*m_weights_loc[i];
        DF[2][1] += sum6*m_weights_loc[i];
    }

    return DF;
}

bool best_fit::Comp_Weights()
{
    TopExp_Explorer aExpFace;
    MeshCore::MeshKernel FaceMesh;
    MeshCore::MeshFacetArray facetArr;

    MeshCore::MeshKernel mesh1,mesh2;
    MeshCore::MeshBuilder builder1(mesh1);
    MeshCore::MeshBuilder builder2(mesh2);
    builder1.Initialize(1000);
    builder2.Initialize(1000);

    Base::Vector3f Points[3];
    TopLoc_Location aLocation;

    bool bf;

    // explores all faces  ------------  Hauptschleife
    for (aExpFace.Init(m_Cad,TopAbs_FACE);aExpFace.More();aExpFace.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());

        bf = false;
        for (unsigned int i=0; i<m_LowFaces.size(); ++i)
        {
            if (m_LowFaces[i].HashCode(IntegerLast()) == aFace.HashCode(IntegerLast())) bf = true;
        }

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
                Points[1].Set((float) aPnt2.X(),(float) aPnt2.Y(),(float) aPnt2.Z());
                gp_Pnt aPnt3 = aPoints(n3);
                Points[2].Set((float) aPnt3.X(),(float) aPnt3.Y(),(float) aPnt3.Z());

                // give the occ faces to the internal mesh structure of freecad
                MeshCore::MeshGeomFacet Face(Points[0],Points[1],Points[2]);

                if (bf == false) builder1.AddFacet(Face);
                else   builder2.AddFacet(Face);

            }
        }
    }

    builder1.Finish();
    builder2.Finish();

    m_pnts.clear();
    m_weights.clear();

    MeshCore::MeshPointArray pnts = mesh1.GetPoints();

    for (unsigned int i=0; i<pnts.size(); ++i)
    {
        m_pnts.push_back(pnts[i]);
        m_weights.push_back(1);
    }

    pnts = mesh2.GetPoints();

    for (unsigned int i=0; i<pnts.size(); ++i)
    {
        m_pnts.push_back(pnts[i]);
        m_weights.push_back(0.15);
    }

    m_normals = Comp_Normals(mesh1);
    std::vector<Base::Vector3f> tmp = Comp_Normals(mesh2);

    for (unsigned int i=0; i<tmp.size(); ++i)
    {
        m_normals.push_back(tmp[i]);
    }

    return true;
}



bool best_fit::RotMat(Base::Matrix4D &M, double degree, int axis)
{
    M.unity();
    degree = 2*PI*degree/360;  // trafo bogenma�

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

bool best_fit::PointNormalTransform(std::vector<Base::Vector3f> &pnts,
                                    std::vector<Base::Vector3f> &normals,
                                    Base::Matrix4D              &M)
{
    int m = pnts.size();
    Base::Vector3f pnt,normal;

    for (int i=0; i<m; ++i)
    {
        pnt.x  = float(M[0][0]*pnts[i].x + M[0][1]*pnts[i].y + M[0][2]*pnts[i].z + M[0][3]);
        pnt.y  = float(M[1][0]*pnts[i].x + M[1][1]*pnts[i].y + M[1][2]*pnts[i].z + M[1][3]);
        pnt.z  = float(M[2][0]*pnts[i].x + M[2][1]*pnts[i].y + M[2][2]*pnts[i].z + M[2][3]);

        pnts[i] = pnt;

        normal.x = float(M[0][0]*normals[i].x + M[0][1]*normals[i].y + M[0][2]*normals[i].z);
        normal.y = float(M[1][0]*normals[i].x + M[1][1]*normals[i].y + M[1][2]*normals[i].z);
        normal.z = float(M[2][0]*normals[i].x + M[2][1]*normals[i].y + M[2][2]*normals[i].z);

        normals[i] = normal;
    }

    return true;
}

bool best_fit::PointTransform(std::vector<Base::Vector3f> &pnts, Base::Matrix4D &M)
{
    int m = pnts.size();
    Base::Vector3f pnt;

    for (int i=0; i<m; ++i)
    {
        pnt.x  =  float(M[0][0]*pnts[i].x + M[0][1]*pnts[i].y + M[0][2]*pnts[i].z + M[0][3]);
        pnt.y  =  float(M[1][0]*pnts[i].x + M[1][1]*pnts[i].y + M[1][2]*pnts[i].z + M[1][3]);
        pnt.z  =  float(M[2][0]*pnts[i].x + M[2][1]*pnts[i].y + M[2][2]*pnts[i].z + M[2][3]);

        pnts[i] = pnt;
    }

    return true;
}

bool best_fit::MeshFit_Coarse()
{

    GProp_GProps prop;
    GProp_PrincipalProps pprop;

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

    MeshCore::MeshEigensystem pca(m_MeshWork);
    pca.Evaluate();
    Base::Matrix4D T1 =  pca.Transform();
    m_MeshWork.Transform(M*T1);

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

    // CAD-Mesh -> zur�ck zum Ursprung
    m_cad2orig.SetX(-orig.X());
    m_cad2orig.SetY(-orig.Y());
    m_cad2orig.SetZ(-orig.Z());

    trafo.SetTranslation(m_cad2orig);
    BRepBuilderAPI_Transform trsf(trafo);

    trsf.Perform(m_Cad);
    m_Cad = trsf.Shape();

    return true;
}

bool best_fit::Tesselate_Face(const TopoDS_Face &aface, MeshCore::MeshKernel &mesh, float deflection)
{
    Base::Builder3D aBuild;
    MeshCore::MeshBuilder builder(mesh);
    builder.Initialize(1000);
    Base::Vector3f Points[3];
    if (!BRepTools::Triangulation(aface,0.1))
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
            Points[1].Set((float) aPnt2.X(),(float) aPnt2.Y(),(float) aPnt2.Z());
            gp_Pnt aPnt3 = aPoints(n3);
            Points[2].Set((float) aPnt3.X(),(float) aPnt3.Y(),(float) aPnt3.Z());
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

    MeshCore::MeshDefinitions::_fMinPointDistanceD1 = (float) 0.0001;
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
    MeshParams._minEdgeSplit = 10;
    BRepMeshAdapt::Mesh(shape,deflection,MeshParams);
    //BRepMesh::Mesh(shape,deflection);
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
                Points[1].Set((float) aPnt2.X(),(float) aPnt2.Y(),(float) aPnt2.Z());
                gp_Pnt aPnt3 = aPoints(n3);
                Points[2].Set((float) aPnt3.X(),(float) aPnt3.Y(),(float) aPnt3.Z());
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

        // Iteriere �ber die Dreiecke zu jedem Punkt
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

std::vector<double> best_fit::CompError_GetPnts(std::vector<Base::Vector3f> pnts,
        std::vector<Base::Vector3f> &normals)

{
    double err_avg = 0.0;
    double dist;

    std::vector<double> errVec(2);    // errVec[0]: avg. error, errVec[1]: max. error

    MeshCore::MeshFacetGrid aFacetGrid(m_Mesh);
    MeshCore::MeshAlgorithm malg(m_Mesh);
    unsigned long  facetIndex;

    Base::Vector3f origPoint, projPoint, distVec;
    //Base::Builder3D log;

    unsigned int NumOfPoints = pnts.size();
    int c = 0;

    m_LSPnts[0].clear();
    m_LSPnts[1].clear();

    m_weights_loc.clear();

    for (unsigned int i=0; i<NumOfPoints; ++i)
    {
        if (!malg.NearestFacetOnRay(pnts[i], normals[i], aFacetGrid, projPoint, facetIndex)
                /*!Intersect(normals[i], *m_Mesh, pnts[i], projPoint)*/)  // gridoptimiert
        {
            ++c;
        }
        else
        {
            m_LSPnts[0].push_back(projPoint);
            m_LSPnts[1].push_back(pnts[i]);
            m_weights_loc.push_back(m_weights[i]);

            //log.addSingleArrow(pnts[i], projPoint);
        }
    }

    double max_err = 0.0;

    for (unsigned int i=0; i<NumOfPoints-c; ++i)
    {
        distVec  = m_LSPnts[0][i]-m_LSPnts[1][i];
        dist     = distVec*distVec;
        err_avg += dist;

        if (dist > max_err)
            max_err = dist;
    }

    //log.saveToFile("c:/intersection.iv");

    errVec[0] = err_avg /= (NumOfPoints-c);  // durchschnittlicher Fehlerquadrat
    errVec[1] = max_err;         // maximaler Fehlerquadrat

    //cout << c << " projections failed" << endl;

    return errVec;
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

    std::vector<int> FailProj;

    MeshCore::MeshFacetGrid aFacetGrid(m_Mesh,10);
    MeshCore::MeshAlgorithm malg(m_Mesh);
    MeshCore::MeshAlgorithm malg2(m_Mesh);
    MeshCore::MeshPointIterator p_it(m_CadMesh);

    Base::Vector3f projPoint, distVec, projPoint2;
    unsigned long  facetIndex;
    stringstream text;
    m_error.resize(m_CadMesh.CountPoints());

    unsigned int c=0;
    int i=0;

    for (p_it.Begin(); p_it.More(); p_it.Next())
    {
        if (malg.NearestFacetOnRay(*p_it, m_normals[i], aFacetGrid, projPoint, facetIndex))   // gridoptimiert
        {
            //log3d.addSingleArrow(*p_it, projPoint, 3, 0,0,0);
            distVec  = projPoint - *p_it;
            sqrdis   = distVec*distVec;

            if (((projPoint.z - p_it->z) / m_normals[i].z ) > 0)
                m_error[i] = sqrt(sqrdis);
            else
                m_error[i] = -sqrt(sqrdis);

            err_avg += sqrdis;
        }
        else
        {
            m_normals[i].Scale(-1,-1,-1);

            if (!malg.NearestFacetOnRay(*p_it, m_normals[i], aFacetGrid, projPoint, facetIndex))   // gridoptimiert
            {
                c++;
                m_normals[i].Scale(-10,-10,-10);
                text << p_it->x << ", " << p_it->y << ", " << p_it->z << "; " << m_normals[i].x << ", " << m_normals[i].y << ", " << m_normals[i].z;
                log3d.addSingleArrow(*p_it, *p_it + m_normals[i], 1, 1,0,0);
                log3d.addText(*p_it,(text.str()).c_str());
            }
            /*else
             log3d.addSingleArrow(*p_it, projPoint, 3, 0,0,0);*/

        }

        ++i;
    }

    //for (p_it.Begin(); p_it.More(); p_it.Next())
//   {
    //    if (!malg.NearestFacetOnRay(*p_it, m_normals[i], aFacetGrid, projPoint, facetIndex))   // gridoptimiert
//       {
//           if (malg2.NearestFacetOnRay(*p_it, m_normals[i], projPoint, facetIndex))
//           {

//               log3d.addSingleArrow(*p_it, projPoint, 3, 0,0,0);
//               distVec  = projPoint - *p_it;
//               sqrdis   = distVec*distVec;

//               if (((projPoint.z - p_it->z) / m_normals[i].z ) > 0)
//                   m_error[i] = sqrt(sqrdis);
//               else
//                   m_error[i] = -sqrt(sqrdis);

//               err_avg += sqrdis;
//           }
//           else
//           {
//               c++;
//               FailProj.push_back(i);
//           }
//       }
//       else
//       {
//           distVec  = projPoint-*p_it;
//           sqrdis   = distVec*distVec;

//           m_normals[i].Scale(-1,-1,-1);

//           if (malg.NearestFacetOnRay(*p_it, m_normals[i], aFacetGrid, projPoint2, facetIndex))
//           {
//               distVec  = projPoint2-*p_it;
//               if (sqrdis > distVec*distVec)
//               {
//                   sqrdis   = distVec*distVec;
//                   log3d.addSingleArrow(*p_it, projPoint2, 3, 0,0,0);
//               }
//               else
//               {
//                   log3d.addSingleArrow(*p_it, projPoint, 3, 0,0,0);
//               }

//           }
//           m_normals[p_it.Position()].Scale(-1,-1,-1);

//           if (((projPoint.z - p_it->z) / m_normals[i].z ) > 0)
//               m_error[i] = sqrt(sqrdis);
//           else
//               m_error[i] = -sqrt(sqrdis);

//           err_avg += sqrdis;
//       }
//       ++i;
    //}


    std::set<MeshCore::MeshPointArray::_TConstIterator>::iterator v_it;
    MeshCore::MeshRefPointToPoints vv_it(m_CadMesh);

    std::set<MeshCore::MeshPointArray::_TConstIterator> PntNei;

    for (unsigned int i=0; i<FailProj.size(); ++i)
    {
        PntNei = vv_it[FailProj[i]];
        m_error[FailProj[i]] = 0.0;

        for (v_it = PntNei.begin(); v_it !=PntNei.end(); ++v_it)
        {
            m_error[FailProj[i]] += m_error[(*v_it)[0]._ulProp];
        }

        m_error[FailProj[i]] /= double(PntNei.size());
        PntNei.clear();
    }


    log3d.saveToFile("c:/projection.iv");

    if (c>(m_CadMesh.CountPoints()/2))
        return 1e+10;

    return err_avg/(m_CadMesh.CountPoints()-c);

}