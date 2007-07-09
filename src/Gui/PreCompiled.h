#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__
 
#include "../FCConfig.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
#pragma warning( disable : 4251 )
#pragma warning( disable : 4273 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4503 )
#pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// Python
#include <Python.h>

// Xerces
#include <xercesc/util/XercesDefs.hpp>

// standard
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
#include <typeinfo>
#include <float.h>
#include <limits.h>

#ifdef FC_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif

// streams
#include <iostream>
#include <iomanip>


// STL
#include <vector>
#include <map>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>
#include <sstream>
#include <bitset>

// Boost
#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "InventorAll.h"
#include "Qt4All.h"
#include "Qt3All.h"


#endif //_PreComp_

#endif // __PRECOMPILED_GUI__
