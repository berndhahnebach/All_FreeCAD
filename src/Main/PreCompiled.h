#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../Config.h"

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// Q Toolkit
#ifdef  _FC_GUI_ENABLED_
# include <qaccel.h>
# include <qaction.h>
# include <qapplication.h>
# include <qbuttongroup.h>
# include <qcdestyle.h>
# include <qclipboard.h>
# include <qcolordialog.h>
# include <qcombobox.h>
# include <qdragobject.h>
# include <qdrawutil.h>
# include <qevent.h>
# include <qfile.h>
# include <qfiledialog.h>
# include <qframe.h>
# include <qhbox.h>
# include <qheader.h>
# include <qiconview.h> 
# include <qimage.h>
# include <qinputdialog.h>
# include <qkeycode.h>
# include <qlabel.h>
# include <qlayout.h>
# include <qlineedit.h>
# include <qlistbox.h>
# include <qlistview.h>
# include <qmainwindow.h>
# include <qmenubar.h>
# include <qmessagebox.h>
# include <qmotifplusstyle.h>
# include <qmotifstyle.h>
# include <qmovie.h>
# include <qmultilinedit.h>
# include <qnamespace.h>
# include <qobjectlist.h>
# include <qpaintdevicemetrics.h>
# include <qpainter.h>
# include <qpalette.h> 
# include <qpixmap.h>
# include <qplatinumstyle.h>
# include <qpopupmenu.h>
# include <qprinter.h>
# include <qprogressbar.h>
# include <qpushbutton.h>
# include <qrangecontrol.h>
# include <qrect.h>
# include <qscrollbar.h>
# include <qspinbox.h>
# include <qsplitter.h> 
# include <qsgistyle.h>
# include <qstatusbar.h>
# include <qtabbar.h>
# include <qtextbrowser.h> 
# include <qtextstream.h>
# include <qthread.h>
# include <qtimer.h>
# include <qtoolbar.h> 
# include <qtoolbutton.h>
# include <qtooltip.h>
# include <qurl.h>
# include <qvalidator.h>
# include <qvariant.h>
# include <qvbox.h>
# include <qwhatsthis.h>
# include <qwidget.h>
# include <qwidgetstack.h>
# include <qwindowsstyle.h>
# include <qworkspace.h>
#endif

#endif 
