#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef WNT
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

// OpenCasCade Base
#include <Standard_Failure.hxx>

// OpenCascade View
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>

#ifndef WNT
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
#include <qcolordialog.h>
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
#include <qtoolbar.h> 
#include <qtoolbutton.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qworkspace.h>




// help system
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <python.h>

#include <windows.h>



#endif 