#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include "../../../Config.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
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

#include <Python.h>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// Qt Toolkit
#include <qaction.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qworkspace.h>

#endif  //_PreComp_

#endif // __PRECOMPILED_GUI__ 
