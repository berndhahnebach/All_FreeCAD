#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#pragma warning( disable : 4786 )

// standard
#include <iostream>
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
#include <qapplication.h>
#include <qpopupmenu.h>
//#include <qevent.h>
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qmultilinedit.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qvbox.h>
#include <qworkspace.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qiconview.h> 
#include <qfiledialog.h>
#include <qsplitter.h> 
#include <qvbox.h>
#include <qworkspace.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qdrawutil.h>
#include <qpalette.h> 
#include <qwidget.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qrangecontrol.h>
#include <qscrollbar.h>
#include <limits.h>
#include <QTabBar.h>
#include <qrect.h>
#include <qvariant.h>
#include <qframe.h>
#include <qaction.h>
#include <qnamespace.h>





// help system
#include <qwhatsthis.h>
#include <qtooltip.h>

#include <python.h>

#include <windows.h>



#endif 