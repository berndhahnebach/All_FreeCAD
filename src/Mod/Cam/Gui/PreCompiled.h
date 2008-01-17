#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include <FCConfig.h>

// Importing of App classes
#ifdef FC_OS_WIN32
# define MeshExport       __declspec(dllimport)
# define AppPartExport    __declspec(dllimport)
# define AppPartGuiExport __declspec(dllimport)
# define AppCamExport     __declspec(dllimport)
# define AppCamGuiExport  __declspec(dllexport)
#else // for Linux
# define AppCamExport
# define AppCamGuiExport
#endif

#ifdef _PreComp_
// standard
#include <iostream>
//#include <stdio.h>
#include <assert.h>
//#include <io.h>
//#include <fcntl.h>
//#include <ctype.h>

// STL
#include <vector>
#include <list>
#include <map>
#include <string>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>
#include <bitset>

#include <Python.h>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// Xerces
#include <xercesc/util/XercesDefs.hpp>

// OpenCasCade Base
#include <Standard_Failure.hxx>

// OpenCascade View
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRepMesh_Discret.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLProp_SLProps.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_UniformDeflection.hxx>
#include <TopExp.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Poly_Polygon3D.hxx>
#include <BRepMesh.hxx>



// Qt Toolkit
#ifndef __Qt4All__
# include <Gui/Qt4All.h>
#endif

// Inventor
#ifndef __InventorAll__
# include <Gui/InventorAll.h>
#endif

#include <float.h>


#endif
#endif
