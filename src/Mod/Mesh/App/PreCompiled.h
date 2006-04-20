#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../FCConfig.h"


// Exporting of App classes
#ifdef FC_OS_WIN32
# define AppMeshExport __declspec(dllexport)
# define AppPartExport __declspec(dllimport)
#else // for Linux
# define AppMeshExport
# define AppPartExport
#endif


#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

#ifdef FC_OS_WIN32
# include <windows.h>
#endif


// standard
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>
#include <ios>
#include <io.h>


#include <gts.h>

// STL
#include <algorithm>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

//#include <gts.h>
// Xerces
#include <xercesc/util/XercesDefs.hpp>

// Python
#include <Python.h>

// OpenCASCADE
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepMesh_Discret.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

// WildMagic (MagicSoftware)
#include <Wm3ApprPlaneFit3.h>
#include <Wm3ApprQuadraticFit3.h>
#include <Wm3Delaunay3.h>
#include <Wm3DistSegment3Segment3.h>
#include <Wm3DistSegment3Triangle3.h>
#include <Wm3DistVector3Plane3.h>
#include <Wm3DistVector3Segment3.h>
#include <Wm3DistVector3Triangle3.h>
#include <Wm3Eigen.h>
#include <Wm3IntrSegment3Box3.h>
#include <Wm3IntrSegment3Plane3.h>
#include <Wm3IntrTriangle3Triangle3.h>
#include <Wm3Matrix3.h>
#include <Wm3MeshCurvature.h>
#include <Wm3QuadricSurface.h>
#include <Wm3Sphere3.h>
#include <Wm3Triangle3.h>
#include <Wm3Vector3.h>



#endif //_PreComp_

#endif

