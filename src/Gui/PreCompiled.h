#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../FCConfig.h"
#ifdef _DEBUG
# define NEW_WB_FRAMEWORK
#endif
#ifdef _PreComp_

#ifdef FC_OS_WIN32
# include <windows.h>
#endif
/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// standard
#include <iostream>
#include <stdio.h>
#include <assert.h>
#ifdef FC_OS_WIN32
# include <io.h>
#endif
#include <fcntl.h>
#include <ctype.h>
#include <typeinfo>

// STL
#include <vector>
#include <map>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>


// Xerces
#include <xercesc/util/XercesDefs.hpp>

// OpenCasCade Base
#include <Standard_Failure.hxx>

// OpenCascade View
#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepMesh.hxx>
#include <BRepMesh_Discret.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <Handle_TPrsStd_AISPresentation.hxx>
//#include <IGESControl_Controller.hxx>
//#include <IGESControl_Reader.hxx>
//#include <IGESControl_Writer.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_Timer.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard.hxx>
//#include <STEPControl_Reader.hxx>
//#include <STEPControl_Writer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <V3d_OrthographicView.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <OSD_Process.hxx>

#include <Quantity_Date.hxx>

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
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture3.h>
#include <Inventor/nodes/SoTextureCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/events/SoMotion3Event.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/projectors/SbSphereSheetProjector.h>
#include <Inventor/projectors/SbSpherePlaneProjector.h>
#include <Inventor/actions/SoRayPickAction.h> 
#include <Inventor/SoPickedPoint.h> 

#include <float.h>



#ifndef FC_OS_WIN32
# include <Xw_Window.hxx>
# include <Graphic3d_GraphicDevice.hxx>
#else
# include <WNT_Window.hxx>
# include <Graphic3d_WNTGraphicDevice.hxx>
#endif

#include <Python.h>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif
#include <limits.h>


// Qt Toolkit
#include <qaccel.h>
#include <qaction.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qdragobject.h>
#include <qdrawutil.h>
#include <qevent.h>
#include <qeventloop.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qfontdatabase.h>
#include <qfontdialog.h>
#include <qftp.h>
#include <qgl.h>
#include <qhbox.h>
#include <qheader.h>
#include <qhttp.h>
#include <qiconview.h>
#include <qimage.h>
#include <qinputdialog.h>
#include <qkeycode.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qmetaobject.h>
#include <qmime.h>
#include <qmovie.h>
#include <qmultilineedit.h>
#include <qnamespace.h>
#include <qobjectlist.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qprocess.h>
#include <qprogressbar.h>
#include <qprogressdialog.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qrangecontrol.h>
#include <qrect.h>
#include <qregexp.h>
#include <qscrollbar.h>
#include <qsimplerichtext.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qsplashscreen.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qstylesheet.h>
#include <qsyntaxhighlighter.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qtextcodec.h>
#include <qtextedit.h>
#include <qtextbrowser.h>
#include <qtextstream.h>
#include <qthread.h>
#include <qtimer.h>
#include <qtoolbar.h>
#include <qtoolbox.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qtranslator.h>
#include <qurl.h>
#include <qvalidator.h>
#include <qvaluestack.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qwidget.h>
#include <qwidgetfactory.h>
#include <qwidgetstack.h>
#include <qworkspace.h>
#include <private/qrichtext_p.h>


#endif //_PreComp_


#endif // __PRECOMPILED_GUI__
