#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../../../Config.h"

/// here get the warnings of to long specifieres disabled (needet for VC6)
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
#include <map>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>

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
#include <BRepTools.hxx>



#ifndef FC_OS_WIN32
#include <Graphic3d_GraphicDevice.hxx>
#else
#include <Graphic3d_WNTGraphicDevice.hxx>
#endif
 
// Q Toolkit
#include <qaccel.h>
#include <qaction.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcdestyle.h>
#include <qclipboard.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qdragobject.h>
#include <qdrawutil.h>
#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qhbox.h>
#include <qheader.h>
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
#include <qmotifplusstyle.h>
#include <qmotifstyle.h>
#include <qmultilinedit.h>
#include <qnamespace.h>
#include <qobjectlist.h>
#include <qpaintdevicemetrics.h>
#include <qpainter.h>
#include <qpalette.h> 
#include <qpixmap.h>
#include <qplatinumstyle.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qrangecontrol.h>
#include <qrect.h>
#include <qscrollbar.h>
#include <qspinbox.h>
#include <qsplitter.h> 
#include <qsgistyle.h>
#include <qstatusbar.h>
#include <qtabbar.h>
#include <qtextbrowser.h> 
#include <qtextstream.h>
#include <qthread.h>
#include <qtimer.h>
#include <qtoolbar.h> 
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qurl.h>
#include <qvalidator.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qwindowsstyle.h>
#include <qworkspace.h>


#include <python.h>
#include <windows.h>



#endif 
