#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../Config.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif


// Exporting of App classes

#ifdef FC_OS_WIN32
# define AppMeshExport __declspec(dllexport)
#else // for Linux
# define AppMeshExport
#endif



#ifdef FC_OS_WIN32
# include <windows.h>
#endif


// standard
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>
#include <ios>
#include <io.h>



// STL
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

//#include <gts.h>
// Xerces
#include <xercesc/util/XercesDefs.hpp>


// OpenCASCADE

#include <TDF_Label.hxx>


#endif //_PreComp_

#endif

