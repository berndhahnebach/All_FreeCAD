#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../../../FCConfig.h"

// Importing of App classes
#ifdef FC_OS_WIN32
# define AppPartExport    __declspec(dllimport)
# define AppPartGuiExport __declspec(dllexport)
#else // for Linux
# define AppPartExport
# define AppPartGuiExport
#endif

#ifdef _PreComp_
/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif




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

#include <Python.h>

#ifdef FC_OS_WIN32
#	include <windows.h>
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

#ifndef __Qt3All__
# include <Gui/Qt3All.h>
#endif



// Open Inventor
#include <GL/gl.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/SbClip.h>
#include <Inventor/SbColor.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/actions/SoLineHighlightRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/fields/SoMFColor.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/manips/SoTransformerManip.h> 
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoImage.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture3.h>
#include <Inventor/nodes/SoTextureCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include <Inventor/manips/SoTransformerManip.h> 
#include <Inventor/nodes/SoTranslation.h>

#include <float.h>


#endif
#endif 
