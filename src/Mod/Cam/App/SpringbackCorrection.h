/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *   Human Rezai <Human@web.de>                                            *
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

#ifndef _SpringbackCorrection_H_
#define _SpringbackCorrection_H_


#include <Mod/Mesh/App/Core/TopoAlgorithm.h>




#define cMin  15.0;

struct EdgeStruct
{
    TopoDS_Edge anEdge;
    std::vector<TopoDS_Face> aFace;
    gp_Vec NormStartP;
    gp_Vec NormEndP;
    std::vector<gp_Pnt> pntList;
    gp_Pnt FPnt,LPnt;
    double MaxOffset;
    double MinOffset;
    double FOff,LOff;
};

struct MeshPnt
{
    int index;
    double minCurv;
    double maxCurv;
    double error;
    Base::Vector3f pnt;
    Base::Vector3f normal;
};

struct FacePnt
{
    Base::Vector3f pnt;
    std::vector<double> distances;
    std::vector<double> MinEdgeOff;
    std::vector<double> MaxEdgeOff;
};

struct EdgeStruct_Less
{
    bool operator()(const EdgeStruct& _Left, const EdgeStruct& _Right) const
    {

        if (_Left.anEdge.IsSame(_Right.anEdge)) return false;

        return true;
    }
};

struct Edge_Less
{
    bool operator()(const TopoDS_Edge& _Left, const TopoDS_Edge& _Right) const
    {
        return(_Left.HashCode(IntegerLast())<_Right.HashCode(IntegerLast()));
    }
};




struct MeshPntLess
{
    bool operator()(const Base::Vector3f& _Left, const Base::Vector3f& _Right) const
    {
        if ( fabs(_Left.x - _Right.x) > 0.001 )
            return _Left.x < _Right.x;
        else if ( fabs(_Left.y - _Right.y) > 0.001)
            return _Left.y < _Right.y;
        else if ( fabs(_Left.z - _Right.z) > 0.001 )
            return _Left.z < _Right.z;
        return false;

    }
};

class SpringbackCorrection
{
public:
    SpringbackCorrection(const TopoDS_Shape& aShape);
    SpringbackCorrection(const TopoDS_Shape& aShape, const MeshCore::MeshKernel& aMesh);
    SpringbackCorrection(const TopoDS_Shape& aShape, const MeshCore::MeshKernel& CadMesh, const MeshCore::MeshKernel& Mesh);
    ~SpringbackCorrection();
    bool Init();
    bool Perform(int deg_Tol);
    bool CalcCurv();
    std::vector<double> MeshCurvature(const MeshCore::MeshKernel& mesh);
    bool GetCurvature(TopoDS_Face aFace);
    bool MirrorMesh(std::vector<double> error);
    double LocalCorrection(std::vector<Base::Vector3f> Neib ,std::vector<Base::Vector3f> Normal,
                           bool sign, double minScale);
    double GlobalCorrection(std::vector<Base::Vector3f> Neib ,std::vector<Base::Vector3f> Normal,
                            bool sign, int ind);
    std::vector<Base::Vector3f> FillConvex(std::vector<Base::Vector3f> Neib,std::vector<Base::Vector3f> Normals, int n);
    bool InsideCheck(Base::Vector3f pnt, Base::Vector3f normal, std::vector<Base::Vector3f> Neib);
    MeshCore::MeshKernel BuildMesh(Handle_Poly_Triangulation aTri, std::vector<Base::Vector3f> TrPoints);
    int GetBoundary(const MeshCore::MeshKernel &mesh, MeshCore::MeshPointArray &meshPnts);
    bool SmoothCurvature();
    bool SmoothMesh(MeshCore::MeshKernel &mesh, double maxTranslation);
    bool SmoothMesh(MeshCore::MeshKernel &mesh, std::vector<int> indicies, double maxTranslation);
    bool GetFaceAng(MeshCore::MeshKernel &mesh, int deg_tol);
    std::vector<int> FaceCheck(MeshCore::MeshKernel mesh, int deg_tol);
    void ReorderNeighbourList(std::set<MeshCore::MeshPointArray::_TConstIterator> &pnt,
                              std::set<MeshCore::MeshFacetArray::_TConstIterator> &face,
                              std::vector<unsigned long> &nei,
                              unsigned long CurInd);


private:
    bool TransferFaceTriangulationtoFreeCAD(const TopoDS_Face& aFace, MeshCore::MeshKernel& TFaceMesh);
    TopoDS_Shape m_Shape;
    std::vector<double> m_CurvPos;
    std::vector<double> m_CurvNeg;
    std::vector<double> m_CurvMax;
    std::vector<EdgeStruct> m_EdgeStruct;

    MeshCore::MeshKernel m_Mesh;
    MeshCore::MeshKernel MeshRef;
    MeshCore::MeshKernel m_Mesh2Fit;
    MeshCore::MeshPointArray PointArray;
    std::vector<Base::Vector3f> m_normals;
    std::vector<double> m_error;
    std::vector<double> m_FaceAng;
    std::vector<MeshPnt> m_MeshStruct;
    std::vector<double> m_Offset;
    std::map<Base::Vector3f,MeshPnt,MeshPntLess > MeshMap;
    std::map<TopoDS_Edge, std::vector<double>, Edge_Less> EdgeMap;
};

#endif