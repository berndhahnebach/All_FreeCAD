#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../FCConfig.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
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
#include <time.h>

// STL 
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <sstream>
#include <queue>


#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>



#endif //_PreComp_

#endif
