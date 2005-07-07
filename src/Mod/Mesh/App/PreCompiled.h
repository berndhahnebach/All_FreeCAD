#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../FCConfig.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif


// Exporting of App classes

#ifdef FC_OS_WIN32
# define AppMeshExport __declspec(dllexport)
#else // for Linux
# define AppMeshExport
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


// OpenCASCADE
#include <BRepMesh_Discret.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <gp_Pln.hxx>
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>

// WildMagic (MagicSoftware)
#include <Wm3DistSegment3Segment3.h>
#include <Wm3DistSegment3Triangle3.h>
#include <Wm3DistVector3Segment3.h>
#include <Wm3DistVector3Triangle3.h>
#include <Wm3IntrSegment3Box3.h>
#include <Wm3IntrSegment3Plane3.h>
#include <Wm3IntrTriangle3Triangle3.h>
#include <Wm3Sphere3.h>
#include <Wm3Triangle3.h>
#include <Wm3Vector3.h>
#include <Wm3Matrix3.h>

#endif //_PreComp_

#endif

