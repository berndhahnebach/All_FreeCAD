/** \file Config.h
 *  \brief Include all needed defines and macros
 *  Here all defines and macro switching is done for FreeCAD.
 *  Every used Library has its section to define the congiguration.
 *  This file keeps the makefiles and Project files cleaner.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

//**************************************************************************
// switching the Opterating systems


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	define FC_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#	define FC_OS_WIN32
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define FC_OS_LINUX
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



//**************************************************************************
// STLport

#ifdef FC_OS_WIN32
//#	define _STLP_USE_OWN_NAMESPACE
//#	define __STLP_USE_DECLSPEC
#	define _STLP_NO_NEW_IOSTREAMS
//#	define _STLP_BROKEN_USING_DIRECTIVE
#	define _STLP_NO_OWN_IOSTREAMS
#	define _STLP_USE_ABBREVS
#	ifdef FC_DEBUG
#		define __STL_DEBUG
#	endif
#endif //FC_OS_WIN32


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
#       define NO_CXX_EXCEPTION
#endif

#define CSFDB


//**************************************************************************
// QT
#define QT_VER 230

// for .NET you must use Qt version 3.1.1
#ifdef FC_OS_WIN32
#	if _MSC_VER >= 1300
#		error "For visual Studio 7.0 use QT 3.x"
#	endif
#endif

// qt lib name
#if QT_VER == 230
#	define QTLIBNAME "qt-mt230nc.lib"
#elif QT_VER == 311
#	define QTLIBNAME "qt-mteval311.lib"
#else
#	define QTLIBNAME "qt.lib"
#endif


#define QT_DLL
#define QT_THREAD_SUPPORT
#define QT_ALTERNATE_QTSMANIP
#define QT_VER 230


//**************************************************************************
// QTextMdi

#define NO_KDE2
//#define QT_NO_COMPAT
#ifdef FCGui
#	define MAKEDLL_QEXTMDI
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
#	define DocExport
#	define BaseExport 
#	define GuiExport
#endif


//**************************************************************************
// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef FC_OS_WIN32
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
// use precompiled header
#	define _PreComp_
#endif


#endif //__CONFIG_H__
