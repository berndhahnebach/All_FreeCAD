/** \file Parameter.cpp
 *  \brief The base::parameter module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here is the implementation of the parameter mimic.
 *  @see Parameter.h FCParameter FCParameterManager
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/



/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <assert.h>
#	include <xercesc/util/PlatformUtils.hpp>
#	include <xercesc/dom/DOM.hpp>
#	include <xercesc/dom/DOMImplementation.hpp>
#	include <xercesc/dom/DOMImplementationLS.hpp>
#	include <xercesc/dom/DOMWriter.hpp>
#	include <xercesc/framework/StdOutFormatTarget.hpp>
#	include <xercesc/parsers/XercesDOMParser.hpp>
#	include <xercesc/util/XMLUni.hpp>
#endif

static XercesDOMParser::ValSchemes    gValScheme       = XercesDOMParser::Val_Auto;


#include "Parameter.h"
#include "Exception.h"

//**************************************************************************
//**************************************************************************
// FCParameterManager
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction


/** Defauld construction
  * Does not much 
  */
FCParametrGrp::FCParametrGrp(DOMElement *GroupNode)	
	: FCHandled(), FCSubject(),_pGroupNode(GroupNode)
{
}



/** Copy construction
  * Makes a one to one copy of all parameters and
  * groups in the container
  */

/** Destruction
  * complete destruction of the object
  */
FCParametrGrp::~FCParametrGrp()
{
}

//**************************************************************************
// Access methodes

FCHandle<FCParametrGrp> FCParametrGrp::GetGroup(const char* Name)
{
	FCHandle<FCParametrGrp> rParamGrp;
	DOMElement *pcTemp;

	// already created?
	if( (rParamGrp=_GroupMap[Name]).IsValid() )
	{
		// just return the already existing Group handle
		return rParamGrp;
	}

	// search if Group node already there
	pcTemp = FindOrCreateElement(_pGroupNode,"FCParamGroup",Name);
/*	if(!pcTemp) // if not, create a new one
	{
		DOMDocument *pDocument = _pGroupNode->getOwnerDocument();

		// creating new group node
		pcTemp = pDocument->createElement(X("FCParamGroup"));
		pcTemp-> setAttribute(X("Name"), X(Name));
		_pGroupNode->appendChild(pcTemp);
	}*/

	// create and register handle
	rParamGrp = FCHandle<FCParametrGrp> (new FCParametrGrp(pcTemp));
	_GroupMap[Name] = rParamGrp;

	return rParamGrp;
}

bool FCParametrGrp::GetBool(const char* Name, bool bPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCBool",Name);
	// if not return preset
	if(!pcElem) return bPreset;
	// if yes check the value and return
	if(strcmp(StrX(pcElem->getAttribute(X("Value"))).c_str(),"1"))
		return false;
	else
		return true;	
}

void  FCParametrGrp::SetBool(const char* Name, bool bValue)
{
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCBool",Name);
	// and set the vaue
	pcElem->setAttribute(X("Value"), X(bValue?"1":"0"));
}

long FCParametrGrp::GetInt(const char* Name, long lPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCInt",Name);
	// if not return preset
	if(!pcElem) return lPreset;
	// if yes check the value and return
	return atol (StrX(pcElem->getAttribute(X("Value"))).c_str());
}

void  FCParametrGrp::SetInt(const char* Name, long lValue)
{
	char cBuf[256];
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCInt",Name);
	// and set the vaue
	sprintf(cBuf,"%d",lValue);
	pcElem->setAttribute(X("Value"), X(cBuf));
}

double FCParametrGrp::GetFloat(const char* Name, double dPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCFloat",Name);
	// if not return preset
	if(!pcElem) return dPreset;
	// if yes check the value and return
	return atof (StrX(pcElem->getAttribute(X("Value"))).c_str());
}

void  FCParametrGrp::SetFloat(const char* Name, double dValue)
{
	char cBuf[256];
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCFloat",Name);
	// and set the value
	sprintf(cBuf,"%f",dValue);
	pcElem->setAttribute(X("Value"), X(cBuf));
}



void  FCParametrGrp::SetBlob(const char* Name, void *pValue, long lLength)
{
	// not implemented so far
	assert(0);
}

void FCParametrGrp::GetBlob(const char* Name, void * pBuf, long lMaxLength, void* pPreset)
{
	// not implemented so far
	assert(0);
}

void  FCParametrGrp::SetASCII(const char* Name, const char *sValue)
{
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCText",Name);
	// and set the value
    DOMNode *pcElem2 = pcElem->getFirstChild();
    if (!pcElem2)
    {
		DOMDocument *pDocument = _pGroupNode->getOwnerDocument();
		DOMText *pText = pDocument->createTextNode(X(sValue));
		pcElem->appendChild(pText);
	}else{
		pcElem2->setNodeValue(X(sValue));
	}
}

void FCParametrGrp::GetASCII(const char* Name, char * pBuf, long lMaxLength, const char * pPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCText",Name);
	// if not return preset
	if(!pcElem) strncpy(pBuf,pPreset,lMaxLength);
	// if yes check the value and return
    DOMNode *pcElem2 = pcElem->getFirstChild();
    if (pcElem2)
    {
		StrX cTemp(pcElem2->getNodeValue());
		strncpy(pBuf,cTemp.c_str(),lMaxLength);
	}else{
		strncpy(pBuf,pPreset,lMaxLength);
	}
}

stlport::string FCParametrGrp::GetASCII(const char* Name, const char * pPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCText",Name);
	// if not return preset
	if(!pcElem) return stlport::string(pPreset);
	// if yes check the value and return
    DOMNode *pcElem2 = pcElem->getFirstChild();
    if (pcElem2)
		return stlport::string(StrX(pcElem2->getNodeValue()).c_str());	
	else
		return stlport::string(pPreset);
}


//**************************************************************************
// Access methodes


DOMElement *FCParametrGrp::FindElement(DOMElement *Start, const char* Type, const char* Name)
{
	for (DOMNode *clChild = Start->getFirstChild(); clChild != 0;  clChild = clChild->getNextSibling())
	{
		if (clChild->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			// the right node Type
			if (!strcmp(Type,StrX(clChild->getNodeName()).c_str()))
			{
				if (clChild->getAttributes()->getLength() > 0)
				{
					if (!strcmp(Name,StrX(clChild->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str()))
						return (DOMElement*)clChild;
				}
			}
		}
	}
	return NULL;
}

DOMElement *FCParametrGrp::FindOrCreateElement(DOMElement *Start, const char* Type, const char* Name)
{
	// first try to find it
	DOMElement *pcElem = FindElement(Start,Type,Name);

	if(!pcElem)
	{
		DOMDocument *pDocument = _pGroupNode->getOwnerDocument();

		pcElem = pDocument->createElement(X(Type));
		pcElem-> setAttribute(X("Name"), X(Name));
		Start->appendChild(pcElem);
	}
	
	return pcElem;
}
	

//**************************************************************************
//**************************************************************************
// FCParameterManager
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//**************************************************************************
// Construction/Destruction


/** Defauld construction
  * Does not much 
  */
FCParameterManager::FCParameterManager()	: FCParametrGrp()
{
	// initialize the XML system
	Init();

// ---------------------------------------------------------------------------
//  Local data
//
//  gXmlFile
//      The path to the file to parser. Set via command line.
//
//  gDoNamespaces
//      Indicates whether namespace processing should be done.
//
//  gDoSchema
//      Indicates whether schema processing should be done.
//
//  gSchemaFullChecking
//      Indicates whether full schema constraint checking should be done.
//
//  gDoCreate
//      Indicates whether entity reference nodes needs to be created or not
//      Defaults to false
//
//  gOutputEncoding
//      The encoding we are to output in. If not set on the command line,
//      then it is defaults to the encoding of the input XML file.
//
//  gMyEOLSequence
//      The end of line sequence we are to output.
//
//  gSplitCdataSections
//      Indicates whether split-cdata-sections is to be enabled or not.
//
//  gDiscardDefaultContent
//      Indicates whether default content is discarded or not.
//
//  gUseFilter
//      Indicates if user wants to plug in the DOMPrintFilter.
//
//  gValScheme
//      Indicates what validation scheme to use. It defaults to 'auto', but
//      can be set via the -v= command.
//
// ---------------------------------------------------------------------------

	gDoNamespaces          = false;
	gDoSchema              = false;
	gSchemaFullChecking    = false;
	gDoCreate              = true;

	gOutputEncoding        = 0;
	gMyEOLSequence         = 0;

	gSplitCdataSections    = true;
	gDiscardDefaultContent = true;
	gUseFilter             = false;
	gFormatPrettyPrint     = true;
}

/** Destruction
  * complete destruction of the object
  */
FCParameterManager::~FCParameterManager()
{
}

void FCParameterManager::Init(void)
{
	static bool Init = false;
	if(!Init)
	{
		try
		{
			XMLPlatformUtils::Initialize();
		}

		catch(const XMLException& toCatch)
		{
			stlport::strstream err;
			char *pMsg = XMLString::transcode(toCatch.getMessage());
			err << "Error during Xerces-c Initialization.\n"
				 << "  Exception message:"
				 << pMsg;
			delete [] pMsg;
			throw FCException(err.str());
		}
		Init = true;
	}
}

//**************************************************************************
// Document handling

void FCParameterManager::LoadOrCreateDocument(const char* sFileName)
{
	int i=_open(sFileName,_O_RDONLY);
	if( i != -1)
	{
		_close(i);
		LoadDocument(sFileName);
	}else{
		CreateDocument();
	}
}



int  FCParameterManager::LoadDocument(const char* sFileName)
{
    const char *gXmlFile = sFileName;

    //
    //  Create our parser, then attach an error handler to the parser.
    //  The parser will call back to methods of the ErrorHandler if it
    //  discovers errors during the course of parsing the XML document.
    //
    XercesDOMParser *parser = new XercesDOMParser;
    parser->setValidationScheme(gValScheme);
    parser->setDoNamespaces(gDoNamespaces);
    parser->setDoSchema(gDoSchema);
    parser->setValidationSchemaFullChecking(gSchemaFullChecking);
    parser->setCreateEntityReferenceNodes(gDoCreate);

    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);

    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
    bool errorsOccured = false;
    try
    {
        parser->parse(gXmlFile);
    }

    catch (const XMLException& e)
    {
        cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << endl;
        errorsOccured = true;
    }

    catch (const DOMException& e)
    {
       cerr << "A DOM error occurred during parsing\n   DOMException code: "
             << e.code << endl;
        errorsOccured = true;
    }

    catch (...)
    {
        cerr << "An error occurred during parsing\n " << endl;
        errorsOccured = true;
    }

	if(errorsOccured) return 0;

	_pDocument = parser->getDocument();
	DOMElement* rootElem = _pDocument->getDocumentElement();

	_pGroupNode = FindElement(rootElem,"FCParamGroup","Root");

	if(!_pGroupNode) throw FCException("Maleforme Parameter document: Root group not found");
	
	return 1;
}

void  FCParameterManager::SaveDocument(const char* sFileName)
{
	DOMPrintFilter   *myFilter = 0;


    try
    {
		// get a serializer, an instance of DOMWriter
		XMLCh tempStr[100];
		XMLString::transcode("LS", tempStr, 99);
		DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMWriter         *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

		// set user specified end of line sequence and output encoding
		theSerializer->setNewLine(gMyEOLSequence);
		theSerializer->setEncoding(gOutputEncoding);

		// plug in user's own filter
		if (gUseFilter)
		{
			// even we say to show attribute, but the DOMWriter
			// will not show attribute nodes to the filter as
			// the specs explicitly says that DOMWriter shall
			// NOT show attributes to DOMWriterFilter.
			//
			// so DOMNodeFilter::SHOW_ATTRIBUTE has no effect.
			// same DOMNodeFilter::SHOW_DOCUMENT_TYPE, no effect.
			//
			myFilter = new DOMPrintFilter(DOMNodeFilter::SHOW_ELEMENT   |
                                          DOMNodeFilter::SHOW_ATTRIBUTE |
                                          DOMNodeFilter::SHOW_DOCUMENT_TYPE
										  );
			theSerializer->setFilter(myFilter);
		}

		// plug in user's own error handler
		DOMErrorHandler *myErrorHandler = new DOMPrintErrorHandler();
		theSerializer->setErrorHandler(myErrorHandler);

		// set feature if the serializer supports the feature/mode
		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections))
			theSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections);
		
		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent))
			theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent);

		if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint))
			theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint);

        //
		// Plug in a format target to receive the resultant
		// XML stream from the serializer.
        //
		// LocalFileFormatTarget prints the resultant XML stream
		// to a file once it receives any thing from the serializer.
		//
		XMLFormatTarget *myFormTarget = new LocalFileFormatTarget (sFileName);

		//
		// do the serialization through DOMWriter::writeNode();
		//
		theSerializer->writeNode(myFormTarget, *_pDocument);

		delete theSerializer;

        //
		// Filter, formatTarget and error handler
		// are NOT owned by the serializer.
		//
		delete myFormTarget;
		delete myErrorHandler;

		if (gUseFilter)
			delete myFilter;      	

    }
    catch (XMLException& e)
    {
        cerr << "An error occurred during creation of output transcoder. Msg is:"
            << endl
            << StrX(e.getMessage()) << endl;
     }

}

void  FCParameterManager::CreateDocument(void)
{
	// creating a document from screatch
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
	_pDocument = impl->createDocument(
                    0,                    // root element namespace URI.
                    X("FCParameters"),         // root element name
                    0);                   // document type object (DTD).

	// creating the node for the root group
	DOMElement* rootElem = _pDocument->getDocumentElement();
	_pGroupNode = _pDocument->createElement(X("FCParamGroup"));
	((DOMElement*)_pGroupNode)->setAttribute(X("Name"), X("Root"));
	rootElem->appendChild(_pGroupNode);

	
}


void  FCParameterManager::CheckDocument()
{
	
}

//**************************************************************************
//**************************************************************************
// DOMTreeErrorReporter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DOMTreeErrorReporter::warning(const SAXParseException&)
{
    //
    // Ignore all warnings.
    //
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;
    cerr << "Error at file \"" << StrX(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << StrX(toCatch.getMessage()) << endl;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;
    cerr << "Fatal Error at file \"" << StrX(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << StrX(toCatch.getMessage()) << endl;
}

void DOMTreeErrorReporter::resetErrors()
{
    // No-op in this case
}




//**************************************************************************
//**************************************************************************
// DOMPrintFilter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



DOMPrintFilter::DOMPrintFilter(unsigned long whatToShow)
:fWhatToShow(whatToShow)
{

}

short DOMPrintFilter::acceptNode(const DOMNode* node) const
{
	//
	// The DOMWriter shall call getWhatToShow() before calling 
	// acceptNode(), to show nodes which are supposed to be 
	// shown to this filter.
	// 
	// REVISIT: In case the DOMWriter does not follow the protocol, 
	//          Shall the filter honour, or NOT, what it claimes
	//         (when it is constructed/setWhatToShow()) 
	//          it is interested in ?
	// 
	// The DOMLS specs does not specify that acceptNode() shall do
	// this way, or not, so it is up the implementation,
	// to skip the code below for the sake of performance ...
	//
	if ((getWhatToShow() & (1 << (node->getNodeType() - 1))) == 0)
		return DOMNodeFilter::FILTER_ACCEPT;

	switch (node->getNodeType())
	{
	case DOMNode::ELEMENT_NODE:
		{
			// for element whose name is "person", skip it
			//if (XMLString::compareString(node->getNodeName(), element_person)==0)
			//	return DOMNodeFilter::FILTER_SKIP;
			// for element whose name is "line", reject it
			//if (XMLString::compareString(node->getNodeName(), element_link)==0)
			//	return DOMNodeFilter::FILTER_REJECT;
			// for rest, accept it
			return DOMNodeFilter::FILTER_ACCEPT;

			break;
		}
	case DOMNode::COMMENT_NODE:
		{
			// the WhatToShow will make this no effect
			//return DOMNodeFilter::FILTER_REJECT;
			return DOMNodeFilter::FILTER_ACCEPT;
			break;
		}
	case DOMNode::TEXT_NODE:
		{
			// the WhatToShow will make this no effect
			//return DOMNodeFilter::FILTER_REJECT;
			return DOMNodeFilter::FILTER_ACCEPT;
			break;
		}
	case DOMNode::DOCUMENT_TYPE_NODE:
		{
			// even we say we are going to process document type,
			// we are not able be to see this node since
			// DOMWriterImpl (a XercesC's default implementation
			// of DOMWriter) will not pass DocumentType node to
			// this filter.
			//
			return DOMNodeFilter::FILTER_REJECT;  // no effect
			break;
		}
	case DOMNode::DOCUMENT_NODE:
		{
			// same as DOCUMENT_NODE
			return DOMNodeFilter::FILTER_REJECT;  // no effect
			break;
		}
	default :
		{
			return DOMNodeFilter::FILTER_ACCEPT;
			break;
		}
	}

	return DOMNodeFilter::FILTER_ACCEPT;
}


//**************************************************************************
//**************************************************************************
// DOMPrintErrorHandler
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




bool DOMPrintErrorHandler::handleError(const DOMError &domError)
{
    // Display whatever error message passed from the serializer
    char *msg = XMLString::transcode(domError.getMessage());
    cout<<msg<<endl;
    delete[] msg;

    // Instructs the serializer to continue serialization if possible.
    return true;
}



//**************************************************************************
//**************************************************************************
// StrX
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



StrX::StrX(const XMLCh* const toTranscode)
{
    // Call the private transcoding method
    fLocalForm = XMLString::transcode(toTranscode);
}

StrX::~StrX()
{
    delete [] fLocalForm;
}


// -----------------------------------------------------------------------
//  Getter methods
// -----------------------------------------------------------------------
const char* StrX::c_str() const
{
    return fLocalForm;
}


//**************************************************************************
//**************************************************************************
// XStr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


XStr::XStr(const char* const toTranscode)
{
    // Call the private transcoding method
    fUnicodeForm = XMLString::transcode(toTranscode);
}

XStr::~XStr()
{
    delete [] fUnicodeForm;
}


// -----------------------------------------------------------------------
//  Getter methods
// -----------------------------------------------------------------------
const XMLCh* XStr::unicodeForm() const
{
    return fUnicodeForm;
}


//**************************************************************************
//**************************************************************************
// XMLCh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


static const XMLCh  element_person[]=
{
	chLatin_p, chLatin_e, chLatin_r, chLatin_s, chLatin_o, chLatin_n, chNull 
};

static const XMLCh  element_link[]=
{
	chLatin_l, chLatin_i, chLatin_n, chLatin_k, chNull 
};
