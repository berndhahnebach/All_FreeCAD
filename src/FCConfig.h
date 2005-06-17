/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
/** \file FCConfig.h
 *  \brief Include all needed defines and macros
 *  Here all defines and macro switching is done for FreeCAD.
 *  Every used Library has its section to define the congiguration.
 *  This file keeps the makefiles and Project files cleaner.
 */


#ifndef __FCCONFIG_H__
#define __FCCONFIG_H__



//**************************************************************************
// switching the Opterating systems


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	ifndef FC_OS_WIN32
#	define FC_OS_WIN32
#	endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#	ifndef FC_OS_WIN32
#	define FC_OS_WIN32
#	endif
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	ifndef FC_OS_LINUX
#	define FC_OS_LINUX
#	endif
#elif defined(CYGWIN)
#	ifndef FC_OS_CYGWIN
#	define FC_OS_CYGWIN
#	endif
//#elif defined(sun) || defined(__sun) || defined(__sun__)
//#	if defined(__SVR4)
//#		define _FC_OS_SOLARIS
//#	else
//#		define _FC_OS_SUN_
//#	endif
//#elif defined(hpux) || defined(__hpux) || defined(__hpux__)
//#	define FC_OS_HPUX_
//#elif defined(__FreeBSD__)
//#	define FC_OS_FREEBSD
//#elif defined(__NetBSD__)
//#	define FC_OS_NETBSD
//#elif defined(__OpenBSD__)
//#	define FC_OS_OPENBSD
//#elif defined(sgi) || defined(__sgi)
//#	define FC_OS_IRIX
//#elif defined(_AIX)
//#	define FC_OS_AIX
//#elif defined(__GNU__)
//#	define FC_OS_GNU
#else
#	error "FreeCAD was not ported to this os yet. For help see free-cad.sourceforge.net"
#endif

#	ifdef FC_OS_WIN32
#		define PATHSEP '\\'
#	else
#		define PATHSEP '/'
#	endif


//**************************************************************************
// STLport

#ifdef FC_OS_WIN32
//#	define _STLP_USE_OWN_NAMESPACE
//#	define __STLP_USE_DECLSPEC
//#	define _STLP_NO_NEW_IOSTREAMS 1
//#	define _STLP_BROKEN_USING_DIRECTIVE
#	define _STLP_NO_OWN_IOSTREAMS 1
#	define _STLP_USE_ABBREVS
// switch on StlPort debuging, need the libs build!
//#	define __STL_DEBUG

#endif //FC_OS_WIN32


//**************************************************************************
// Xerces

#ifdef FC_DEBUG
#	define DOMLIBNAME "xerces-c_2D.lib"
#else
#	define DOMLIBNAME "xerces-c_2.lib"
#endif




//**************************************************************************
// Open CasCade

#ifdef FC_OS_WIN32
#	define WNT
#	define WIN32
#	define _WINDOWS
#endif

#ifdef FC_OS_LINUX
#	define LIN
#       define LININTEL
//#       define NO_CXX_EXCEPTION
#endif

#define CSFDB

/// enables the use of the OCC DocumentBrowser
#ifndef FC_OS_LINUX
#	define FC_USE_OCAFBROWSER
#endif


#ifdef FC_OCC_DEBUG
#	ifdef FC_DEBUG
#		define DEBUG 1
#	else
#		undef  DEBUG
#	ifndef NDEBUG
#		define NDEBUG
#	endif
#	endif
#endif


//**************************************************************************
// QT
#define QTLIBNAME "qt-mtnc321.lib"
//#define QTLIBNAME "qt-mt.lib"


#ifndef QT_DLL
# define QT_DLL
#endif

#ifndef QT_THREAD_SUPPORT
# define QT_THREAD_SUPPORT
#endif

#ifndef QT_ALTERNATE_QTSMANIP
# define QT_ALTERNATE_QTSMANIP
#endif

//**************************************************************************
// Coin3D

#ifndef COIN_DLL
#	define COIN_DLL
#endif
#define INVENTORLIBNAME "coin2.lib"

//**************************************************************************
// SoQt

#define SOQTLIBNAME     "soqt1.lib"

#ifdef FC_OS_WIN32

#ifndef SOQT_DLL
# define SOQT_DLL
#endif

	/* Define to use GetEnvironmentVariable() instead of getenv() */
//#	define HAVE_GETENVIRONMENTVARIABLE 1
	/* define if the GL header should be included as GL/gl.h */
//#	define HAVE_GL_GL_H 1
	/* Define this to 1 if operator==(QGLFormat&, QGLFormat&) is available */
//#	define HAVE_QGLFORMAT_EQ_OP 1
	/* Define this to 1 if QGLFormat::setOverlay() is available */
//#	define HAVE_QGLFORMAT_SETOVERLAY 1
	/* Define this to 1 if QGLWidget::setAutoBufferSwap() is available */
//#	define HAVE_QGLWIDGET_SETAUTOBUFFERSWAP 1
	/* Define this if Qt::Keypad is available */
//#	define HAVE_QT_KEYPAD_DEFINE 1
	/* Define this if QWidget::showFullScreen() is available */
//#	define HAVE_QWIDGET_SHOWFULLSCREEN 1
	/* Define to enable use of Inventor feature */
//#	define HAVE_SOCAMERA_SETSTEREOMODE 1
	/* Define to enable use of Inventor feature */
//#	define HAVE_SOKEYBOARDEVENT_DELETE 1
	/* Define to enable use of Inventor feature */
//#	define HAVE_SOMOUSEBUTTONEVENT_BUTTON5 1
	/* Define to enable use of the Open Inventor SoPolygonOffset node */
//#	define HAVE_SOPOLYGONOFFSET 1
	/* Define to 1 if you have the <sys/types.h> header file. */
//#	define HAVE_SYS_TYPES_H 1
	/* Define to 1 if you have the <windows.h> header file. */
//#	define HAVE_WINDOWS_H 1

#endif


//**************************************************************************
// Exception handling

// dont catch a c++ exception in DEBUG!
#ifdef FC_DEBUG
#  define DONT_CATCH_CXX_EXCEPTIONS 1
#	 define DBG_TRY
#	 define DBG_CATCH(X)  
#else
/// used to switch a catch with the debug state
#	 define DBG_TRY try	{
/// see docu DBGTRY
#	 define DBG_CATCH(X) }catch(...) { X }
#endif


//**************************************************************************
// windows import export DLL defines
#ifdef FC_OS_WIN32
#	ifdef FCApp
#		define AppExport  __declspec(dllexport)
#	else
#		define AppExport  __declspec(dllimport)
#	endif
#	ifdef FCBase
#		define BaseExport  __declspec(dllexport)
#	else
#		define BaseExport  __declspec(dllimport)
#	endif
#	ifdef FCGui
#		define GuiExport  __declspec(dllexport)
#	else
#		define GuiExport  __declspec(dllimport)
#	endif
#else
#	ifndef BaseExport
#		define BaseExport
#	endif
#	ifndef GuiExport
#		define GuiExport
#	endif
#	ifndef AppExport
#		define AppExport
#	endif
#endif


//**************************************************************************
// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
// use precompiled header
#	define _PreComp_
#endif


#endif //__CONFIG_H__
