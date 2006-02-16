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
 *  Every used library has its own section to define the configuration.
 *  This file keeps the makefiles and project files cleaner.
 */


#ifndef FC_CONFIG_H
#define FC_CONFIG_H



//**************************************************************************
// switching the operating systems


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
#elif defined(__CYGWIN__)
#	ifndef FC_OS_CYGWIN
#	define FC_OS_CYGWIN
// Avoid conflicts with Inventor
#	define HAVE_INT8_T
#	define HAVE_UINT8_T
#	define HAVE_INT16_T
#	define HAVE_UINT16_T
#	define HAVE_INT32_T
#	define HAVE_UINT32_T
#	define HAVE_INT64_T
#	define HAVE_UINT64_T
#	define HAVE_INTPTR_T
#	define HAVE_UINTPTR_T
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
#	error "FreeCAD is not ported to this OS yet. For help see free-cad.sourceforge.net"
#endif

#ifdef FC_OS_WIN32
#	define PATHSEP '\\'
#else
#	define PATHSEP '/'
#endif


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
#	define _CRT_SECURE_NO_DEPRECATE 
// Uncomment _STLP_USE_NEWALLOC to force allocator<T> to use plain "new"
// instead of STLport optimized node allocator engine.
// Note: This is needed to free memory used e.g. in std::set
# define   _STLP_USE_NEWALLOC   1
#endif //FC_OS_WIN32


//**************************************************************************
// Xerces

#ifdef _MSC_VER
#	ifdef FC_DEBUG
#		define DOMLIBNAME "xerces-c_2D.lib"
#	else
#		define DOMLIBNAME "xerces-c_2.lib"
#	endif
#endif



//FIXME: Move to modules where OCC is needed
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
// Qt

#if defined (FC_OS_WIN32) || defined(FC_OS_CYGWIN)
# ifndef QT_DLL
#  define QT_DLL
# endif
#endif

#ifndef QT_THREAD_SUPPORT
# define QT_THREAD_SUPPORT
#endif

#ifndef QT_ALTERNATE_QTSMANIP
# define QT_ALTERNATE_QTSMANIP
#endif

#ifdef _MSC_VER
#	define QTLIBNAME "qt-mtnc321.lib"
//#define QTLIBNAME "qt-mt.lib"
#endif

//**************************************************************************
// Coin3D
#if defined (FC_OS_WIN32) || defined(FC_OS_CYGWIN)
# ifndef COIN_DLL
#	 define COIN_DLL
# endif
#endif

#ifdef _MSC_VER
#	ifdef FC_DEBUG
#		define INVENTORLIBNAME "coin2.lib" // should be "coin2d.lib"
#	else
#		define INVENTORLIBNAME "coin2.lib"
#	endif
#endif

//**************************************************************************
// SoQt

#if defined (FC_OS_WIN32) || defined(FC_OS_CYGWIN)
# ifndef SOQT_DLL
#   define SOQT_DLL
# endif
#endif

#ifdef _MSC_VER
#	ifdef FC_DEBUG
#		define SOQTLIBNAME "soqt1.lib" // should be soqt1d.lib
#	else
#		define SOQTLIBNAME "soqt1.lib"
#	endif
#endif


//**************************************************************************
// Exception handling

// Don't catch C++ exceptions in DEBUG!
#ifdef FC_DEBUG
# define DONT_CATCH_CXX_EXCEPTIONS 1
#	define DBG_TRY
#	define DBG_CATCH(X)  
#else
/// used to switch a catch with the debug state
#	define DBG_TRY try	{
/// see docu DBGTRY
#	define DBG_CATCH(X) }catch(...) { X }
#endif


//**************************************************************************
// Windows import export DLL defines
#if defined (FC_OS_WIN32) || defined(FC_OS_CYGWIN)
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
// here get the warnings of too long specifiers disabled (needed for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4996 )  // supress depricated warning for e.g. open()...
#	define _PreComp_                  // use precompiled header
#endif


#endif //FC_CONFIG_H
