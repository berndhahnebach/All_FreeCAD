/** \file Config.h
 *  \brief Include all needed defines and macros
 *  Here all defines and macro switching is done for FreeCAD.
 *  Every used Library has its section to define the congiguration.
 *  This file keeps the makefiles and Project files cleaner.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

//**************************************************************************
// STL

#define _STLP_USE_OWN_NAMESPACE
#define __STLP_USE_DECLSPEC
#define _STLP_NO_NEW_IOSTREAMS
#define _STLP_USE_ABBREVS
#ifdef _DEBUG
#	define __STL_DEBUG
#endif

#ifdef __linux
#  define FCSTD std
#else
#define FCSTD stlport
#endif

// containers
#define  FCvector               FCSTD::vector
#define  FCset                  FCSTD::set
#define  FCmap                  FCSTD::map
#define  FCmultiset             FCSTD::multiset
#define  FCmultimap             FCSTD::multimap
#define  FCpair                 FCSTD::pair
#define  FChash_set             FCSTD::hash_set
#define  FChash_map             FCSTD::hash_map
#define  FCstring               FCSTD::string
#define  FClist                 FCSTD::list
#define  FCslist                FCSTD::slist
#define  FCqueue                FCSTD::queue
#define  FCdeque                FCSTD::deque
#define  FCbitset               FCSTD::bitset
#define  FCstack                FCSTD::stack
#define  FCcout                 FCSTD::cout
#define  FCcerr                 FCSTD::cerr
#define  FCendl                 FCSTD::endl
#define  FCofstream             FCSTD::ofstream
#ifdef __linux
#  define FCstrstream           FCSTD::ostringstream
#else
#  define FCstrstream           FCSTD::strstream
#endif
#define  FCmake_pair            FCSTD::make_pair
#define  FCfind                 FCSTD::find
#define  FCmax                  FCSTD::max
#define  FCmin                  FCSTD::min

//**************************************************************************
// Open CasCade

#ifdef WNT
#	define WIN32
#	define	_WINDOWS
#endif
#define CSFDB


//**************************************************************************
// QT

#define QT_DLL
#define QT_THREAD_SUPPORT
#define QT_ALTERNATE_QTSMANIP


//**************************************************************************
// QTextMdi

#define NO_KDE2


//**************************************************************************
// windows import export DLL defines
#ifdef WNT
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
#ifdef WNT
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
// use precompiled header
#	define _PreComp_
#endif


#endif //__CONFIG_H__
