#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../Config.h"

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// standard
#include <iostream.h>
#include <stdio.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>

// STL
#include <vector>
#include <map>
#include <string>
#include <list>
#include <set>
#include <algorithm>

// OpenCasCade Base
#include <Standard_Failure.hxx>

// OpenCascade View
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TNaming_Tool.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_SphericalSurface.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TNaming_NamedShape.hxx>
//#include <IGESControl_Controller.hxx>
//#include <IGESControl_Writer.hxx>
//#include <IGESControl_Reader.hxx>
//#include <STEPControl_Writer.hxx>
//#include <STEPControl_Reader.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>
#include <TDataStd_Real.hxx>
#include <Handle_TPrsStd_AISPresentation.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Standard.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh.hxx>
#include <gp_Pnt.hxx>
#include <BRepMesh_Discret.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <OSD_Timer.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TCollection_AsciiString.hxx>
#include <OSD_FileIterator.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Poly_Connect.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>


// extern inventor
#include <Inventor/SbColor.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h> 
#include <Inventor/actions/SoGetPrimitiveCountAction.h> 
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/fields/SoMFColor.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h> 
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h> 
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoExtSelection.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <inventor/SoInput.h>



#ifndef FC_OS_WIN32
#include <Graphic3d_GraphicDevice.hxx>
#else
#include <Graphic3d_WNTGraphicDevice.hxx>
#endif
 
// Q Toolkit
#include <QTabBar.h>
#include <limits.h>
#include <qaccel.h>
#include <qaction.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qdragobject.h>
#include <qdrawutil.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qheader.h>
#include <qiconview.h> 
#include <qimage.h>
#include <qkeycode.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qmultilinedit.h>
#include <qnamespace.h>
#include <qobjectlist.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qpalette.h> 
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qpushbutton.h>
#include <qrangecontrol.h>
#include <qrect.h>
#include <qscrollbar.h>
#include <qsplitter.h> 
#include <qstatusbar.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtoolbar.h> 
#include <qtoolbutton.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qworkspace.h>
#include <qwidgetstack.h>
#include <qtextbrowser.h> 
#include <qurl.h>
#include <qinputdialog.h>
#include <qmotifstyle.h>
#include <qcdestyle.h>
#include <qmotifplusstyle.h>
#include <qwindowsstyle.h>
#include <qplatinumstyle.h>
#include <qsgistyle.h>


// help system
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <python.h>

#include <windows.h>



#endif 