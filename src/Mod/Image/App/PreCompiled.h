#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../../../FCConfig.h"

// Exporting of App classes
#ifdef FC_OS_WIN32
# define ImageAppExport __declspec(dllexport)
#else // for Linux
# define ImageAppExport
#endif

#ifdef _PreComp_
/// here get the warnings of to long specifieres disabled (needed for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// standard
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <map>
#include <vector>
#include <set>

#include <Python.h>
#include <xercesc/util/XercesDefs.hpp>

#endif
#endif // _PreComp_

