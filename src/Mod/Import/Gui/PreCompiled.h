/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include <FCConfig.h>

#ifdef _PreComp_
/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif


#ifdef FC_OS_WIN32
# define AppPartGuiExport __declspec(dllimport)
#else // for Linux
# define AppPartGuiExport
#endif



// standard
#include <list>
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
#include <set>

#include <Python.h>
#ifndef FC_OS_WIN32
#	include <windows.h>
#endif

// Xerces
#include <xercesc/util/XercesDefs.hpp>

// OpenCasCade Base
#include <Standard_Failure.hxx>

// OpenCascade View
#include <Geom_Axis2Placement.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
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
#include <TNaming_NamedShape.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <TNaming_Builder.hxx>



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
#include <qmovie.h>
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

#endif
#endif 
