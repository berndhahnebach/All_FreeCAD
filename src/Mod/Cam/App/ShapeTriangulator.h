#ifndef _SHAPETRIANGULATOR_H_
#define _SHAPETRIANGULATOR_H_

#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

//Mesh Stuff
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
//#include <Mod/Mesh/App/Core/Iterator.h>
//#include <Mod/Mesh/App/MeshAlgos.h>
//#include <Mod/Mesh/App/Core/Elements.h>
//#include <Mod/Mesh/App/Core/Grid.h>
//#include <Mod/Mesh/App/Core/Evaluation.h>
//#include <Mod/Mesh/App/Core/Builder.h>



struct EdgeStruct
{
	TopoDS_Edge anEdge;
	std::vector<TopoDS_Face> aFace;
	gp_Vec NormStartP;
	gp_Vec NormEndP;
	std::vector<gp_Pnt> pntList;
};


struct EdgeStruct_Less
{
       bool operator()(const EdgeStruct& _Left, const EdgeStruct& _Right) const
       {

		   if(_Left.anEdge.IsSame(_Right.anEdge)) return false;

		   return true;
       }
};

struct ShapeLess
	{
		//Standard_Integer maxShapes;
		bool operator()(const TopoDS_Shape& _Left, const TopoDS_Shape& _Right) const
		{
			return (_Left.HashCode(IntegerLast())<_Right.HashCode(IntegerLast()));
		}
	};

struct gp_PntLess
{
       bool operator()(const gp_Pnt& _Left, const gp_Pnt& _Right) const
       {
               Standard_Real x1,y1,z1,x2,y2,z2;
               _Left.Coord(x1,y1,z1);
               _Right.Coord(x2,y2,z2);
               if ( x1 != x2 )
                       return x1 < x2;
               else if ( y1 != y2 )
                       return y1 < y2;
               return z1 < z2;
       }
};


class ShapeTriangulator
{
public:
	ShapeTriangulator(const TopoDS_Shape& aShape);
	bool Init();
	~ShapeTriangulator();

private:
	//Functions
	
	bool FillUpFacePointsMap();
	bool FillUpFaceMeshIndexMap();
	bool GenerateFreeCADMesh();
	bool ConnectOCCtoFreeCAD();
	TopoDS_Shape m_Shape;
	std::vector<EdgeStruct> m_EdgeStruct;
	MeshCore::MeshKernel m_Mesh;
	//A Map to connect Faces and the Vertices in 3D and 2D Parameters
	std::map<TopoDS_Face,map<gp_Pnt,gp_Pnt2d,gp_PntLess>,ShapeLess > m_FacePointsMap;
};

#endif