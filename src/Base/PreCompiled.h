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


// Xwerces
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXParseException.hpp>


// OpenCasCade
//#include <Standard_Failure.hxx>




#endif