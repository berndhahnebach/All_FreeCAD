#ifndef __PRECOMPILED__
#define __PRECOMPILED__


// Std. configurations
#include "../Config.h"

#ifdef _PreComp_


//**************************************************************************
// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef FC_OS_WIN32
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#endif

// standard
#include <iostream>
#include <stdio.h>
#include <FCNTL.H>
#include <assert.h>
#include <time.h>
#include "fcntl.h"
#ifdef FC_OS_WIN32
#	include <windows.h>
#endif

// STL 
#include <string>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <strstream>

#include <iostream>

// Python
#include <Python.h>


// Xerces
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXParseException.hpp>


// OpenCasCade
//#include <Standard_Failure.hxx>


#endif //_PreComp_

#endif