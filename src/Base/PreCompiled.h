#ifndef __PRECOMPILED__
#define __PRECOMPILED__


// Std. configurations
#include "../FCConfig.h"

#ifdef _PreComp_


//**************************************************************************
// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
//#	pragma warning( disable : 4251 )
//#	pragma warning( disable : 4503 )
//#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
//#	pragma warning( disable : 4290 )  // not implemented throw specification
#endif

// standard
#include <stdio.h>
#include <assert.h>
#include <time.h>
//#include <sys/timeb.h>
#include <direct.h>
#include <float.h>
#include <fcntl.h>

#ifdef FC_OS_WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

// STL 
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <queue>

//streams
#include <iostream>
#include <fstream>
#include <sstream>

// Python
#include <Python.h>


// Xerces
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>




#endif //_PreComp_

#endif

