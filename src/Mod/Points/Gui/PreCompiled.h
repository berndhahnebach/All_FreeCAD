#ifndef __PRECOMPILED_GUI__
#define __PRECOMPILED_GUI__

#include <FCConfig.h>

// Importing of App classes
#ifdef FC_OS_WIN32
# define PointsAppExport __declspec(dllimport)
# define PointsGuiExport __declspec(dllexport)
#else // for Linux
# define PointsAppExport
# define PointsGuiExport
#endif

#ifdef _PreComp_

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
#include <bitset>

// Xerces
#include <xercesc/util/XercesDefs.hpp>

#include <Python.h>
#ifdef FC_OS_WIN32
# include <windows.h>
#endif


// Qt Toolkit
#ifndef __Qt4All__
# include <Gui/Qt4All.h>
#endif

// Inventor
#ifndef __InventorAll__
# include <Gui/InventorAll.h>
#endif


#endif  //_PreComp_

#endif // __PRECOMPILED_GUI__ 
