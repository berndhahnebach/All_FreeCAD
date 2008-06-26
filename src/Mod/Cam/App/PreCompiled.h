#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../FCConfig.h"


// Exporting of App classes
#ifdef FC_OS_WIN32
    # define AppCamExport   __declspec(dllexport)
    # define CamExport      __declspec(dllexport)
    # define AppPartExport  __declspec(dllimport)
    # define PartExport     __declspec(dllimport)
    # define AppMeshExport  __declspec(dllimport)
    # define MeshExport     __declspec(dllimport)
#else // for Linux
    # define AppCamExport
    # define CamExport
    # define AppPartExport
    # define PartExport
    # define AppMeshExport
    # define MeshExport
#endif


#ifdef _PreComp_

    /// here get the warnings of to long specifieres disabled (needed for VC6)
    #ifdef _MSC_VER
        # pragma warning( disable : 4251 )
        # pragma warning( disable : 4275 )
        # pragma warning( disable : 4503 )
        # pragma warning( disable : 4786 )  // specifier longer then 255 chars
        # pragma warning( disable : 4101 )
    #endif

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
        #include <windows.h>
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
#include <TopoDS_Compound.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Handle_Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#endif //_PreComp_

#endif //__PRECOMPILED__


