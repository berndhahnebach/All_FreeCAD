#ifndef __PRECOMPILED__
#define __PRECOMPILED__


/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef WNT
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif


// Std. configurations
//#include "Config.h"

// standard
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <time.h>


// STL 
#include <string>
#include <map>
#include <vector>
#include <set>
#include <stack>

// Python
#include <Python.h>

// OpenCasCade
//#include <Standard_Failure.hxx>




#endif