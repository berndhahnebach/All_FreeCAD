#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../FCConfig.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// Exporting of App classes
#ifdef FC_OS_WIN32
# define _TEMPLATE_AppExport __declspec(dllexport)
#else // for Linux
# define _TEMPLATE_AppExport
#endif

// standard
#include <stdio.h>
#include <assert.h>

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

// Xerces
#include <xercesc/util/XercesDefs.hpp>

#endif //_PreComp_

#endif

