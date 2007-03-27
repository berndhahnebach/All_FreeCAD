#ifndef __PRECOMPILED__
#define __PRECOMPILED__

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


// standard
#include <stdio.h>
#include <assert.h>
#include <time.h>

#ifdef FC_OS_WIN32
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#endif


// Streams
#include <iostream>
#include <sstream>

// STL 
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <sstream>
#include <queue>

// Boost
#include <boost/signals.hpp>
//#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/program_options.hpp>
//namespace po = boost::program_options;

// xerces
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>


#endif //_PreComp_

#endif
