#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include <FCConfig.h>

// Importing of App classes
#ifdef FC_OS_WIN32
# define ImageAppExport __declspec(dllimport)
# define ImageGuiExport __declspec(dllexport)
#else // for Linux
# define ImageAppExport
# define ImageGuiExport
#endif


/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// Python
#include <Python.h>

// standard
#include <iostream>
#include <assert.h>
#include <math.h>

// STL
#include <vector>
#include <map>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>
#include <bitset>

#ifdef FC_OS_WIN32
# include <windows.h>
#endif

// Qt Toolkit
#ifndef __Qt4All__
# include <Gui/Qt4All.h>
#endif

#include <xercesc/util/XercesDefs.hpp>


#endif // __PRECOMPILED_GUI__
