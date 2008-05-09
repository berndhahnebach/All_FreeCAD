#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include <FCConfig.h>


// Exporting of App classes
#ifdef FC_OS_WIN32
#   define    MeshExport __declspec(dllexport)
#   define AppPartExport __declspec(dllimport)
#else // for Linux
#   define    MeshExport
#   define AppPartExport
#endif


#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
#   pragma warning( disable : 4251 )
#   pragma warning( disable : 4503 )
#   pragma warning( disable : 4275 )
#   pragma warning( disable : 4786 )  // specifier longer then 255 chars
#   pragma warning( disable : 4661 )  // no suitable definition provided for explicit
#endif                                // template instantiation request

#ifdef FC_OS_WIN32
#   include <windows.h>
#endif


// standard
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>
#include <ios>
#include <io.h>

#ifdef FC_USE_GTS
#  include <gts.h>
#endif
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


#endif //_PreComp_

#endif

