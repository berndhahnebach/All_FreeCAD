#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../Config.h"

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef FC_OS_WIN32
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
#include <qtextbrowser.h> 
#include <qurl.h>



// help system
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <python.h>

#include <windows.h>



#endif 