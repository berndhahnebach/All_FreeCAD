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
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <xercesc/util/PlatformUtils.hpp>
#	include <xercesc/dom/DOM.hpp>
#	include <xercesc/dom/DOMImplementation.hpp>
#	include <xercesc/dom/DOMImplementationLS.hpp>
#	include <xercesc/dom/DOMWriter.hpp>
#	include <xercesc/framework/StdOutFormatTarget.hpp>
#	include <xercesc/framework/LocalFileFormatTarget.hpp>
#	include <xercesc/parsers/XercesDOMParser.hpp>
#	include <xercesc/util/XMLUni.hpp>
#	include <xercesc/sax/ErrorHandler.hpp>
#	include <xercesc/sax/SAXParseException.hpp>
#	include <fcntl.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	ifdef FC_OS_WIN32
#		include <io.h>
#	endif
#	include <stdio.h>
#endif


#include <fcntl.h>
#ifdef FC_OS_LINUX
#	include <unistd.h>
#endif

#include "Parameter.h"
#include "Exception.h"
#include "Console.h"


#ifdef XERCES_HAS_CPP_NAMESPACE
	using namespace xercesc;
#endif


//**************************************************************************
//**************************************************************************
// privat classes declaration:
// - DOMTreeErrorReporter
// - StrX
// - DOMPrintFilter
// - DOMPrintErrorHandler
// - XStr
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class DOMTreeErrorReporter : public ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    DOMTreeErrorReporter() :
       fSawErrors(false)
    {
    }

    ~DOMTreeErrorReporter()
    {
    }


    // -----------------------------------------------------------------------
    //  Implementation of the error handler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
};

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode);

    ~StrX();

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* c_str() const;

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

inline std::ostream& operator<<(std::ostream& target, const StrX& toDump)
{
    target << toDump.c_str();
    return target;
}


class DOMPrintFilter : public DOMWriterFilter {
public:

    /** @name Constructors */
	DOMPrintFilter(unsigned long whatToShow = DOMNodeFilter::SHOW_ALL);
    //@{

    /** @name Destructors */
	~DOMPrintFilter(){};
    //@{

	/** @ interface from DOMWriterFilter */
	virtual short acceptNode(const DOMNode*) const;
    //@{

	virtual unsigned long getWhatToShow() const {return fWhatToShow;};

	virtual void          setWhatToShow(unsigned long toShow) {fWhatToShow = toShow;};

private:
	// unimplemented copy ctor and assignement operator
	DOMPrintFilter(const DOMPrintFilter&);
	DOMPrintFilter & operator = (const DOMPrintFilter&);

	unsigned long fWhatToShow;   

};

class DOMPrintErrorHandler : public DOMErrorHandler
{
public:

    DOMPrintErrorHandler(){};
    ~DOMPrintErrorHandler(){};

    /** @name The error handler interface */
    bool handleError(const DOMError& domError);
    void resetErrors(){};

private :
    /* Unimplemented constructors and operators */ 
    DOMPrintErrorHandler(const DOMErrorHandler&);
    void operator=(const DOMErrorHandler&);
    
};



// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode);

    ~XStr();


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const;

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};


inline bool DOMTreeErrorReporter::getSawErrors() const
{
    return fSawErrors;
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
FCParameterGrp::FCParameterGrp(DOMElement *GroupNode,const char* sName)	
	: FCHandled(), FCSubject<const char*>(),_pGroupNode(GroupNode)
{
	if(sName) _cName=sName;
}



/** Copy construction
  * Makes a one to one copy of all parameters and
  * groups in the container
  */

/** Destruction
  * complete destruction of the object
  */
FCParameterGrp::~FCParameterGrp()
{
}

//**************************************************************************
// Access methodes

FCHandle<FCParameterGrp> FCParameterGrp::GetGroup(const char* Name)
{
	std::string cName = Name;

	std::string::size_type pos = cName.find('/');

	// is there a path seperator ?
	if(pos == std::string::npos)
	{
		return _GetGroup(Name);
	}
	else if(pos == cName.size())
	{
		// ending slash! cut it away
		cName.erase(pos);
		return _GetGroup(cName.c_str());
	} else if(pos == 0)
	{
		// a leading slash is not handled (root unknown)
		//throw FCException("FCParameterGrp::GetGroup() leading slash not allowed");
		// remove leading slash
		cName.erase(0,1);
		// subsequent call
		return GetGroup(cName.c_str());
	} else {
		// path, split the first path
		std::string cTemp;
		// geting the first part
		cTemp.assign(cName,0,pos);
		// removing the first part from the original
		cName.erase(0,pos+1);
		//sbsequent call
		return _GetGroup(cTemp.c_str())->GetGroup(cName.c_str());
	}

}

FCHandle<FCParameterGrp> FCParameterGrp::_GetGroup(const char* Name)
{
	FCHandle<FCParameterGrp> rParamGrp;
	DOMElement *pcTemp;

	// already created?
	if( (rParamGrp=_GroupMap[Name]).IsValid() )
	{
		// just return the already existing Group handle
		return rParamGrp;
	}

	// search if Group node already there
	pcTemp = FindOrCreateElement(_pGroupNode,"FCParamGroup",Name);

	// create and register handle
	rParamGrp = FCHandle<FCParameterGrp> (new FCParameterGrp(pcTemp,Name));
	_GroupMap[Name] = rParamGrp;

	return rParamGrp;

}

std::vector<FCHandle<FCParameterGrp> > FCParameterGrp::GetGroups(void)
{
	FCHandle<FCParameterGrp> rParamGrp;
	std::vector<FCHandle<FCParameterGrp> >  vrParamGrp;
	DOMElement *pcTemp; //= _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCParamGroup");

	while(pcTemp)
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// already created?
		if( ! (rParamGrp=_GroupMap[Name]).IsValid() )
		{
			rParamGrp = FCHandle<FCParameterGrp> (new FCParameterGrp(((DOMElement*)pcTemp),Name.c_str()));
			_GroupMap[Name] = rParamGrp;
		}
		vrParamGrp.push_back( rParamGrp );
		// go to next
		pcTemp = FindNextElement(pcTemp,"FCParamGroup");
	}

	return vrParamGrp;
}

/// test if this group is emty
bool FCParameterGrp::IsEmpty(void)
{
	if( _pGroupNode->getFirstChild() )
		return false;
	else
		return true;
}

/// test if a special sub group is in this group
bool FCParameterGrp::HasGroup(const char* Name)
{
	if( _GroupMap.find(Name) != _GroupMap.end() )
		return true;

	if( FindElement(_pGroupNode,"FCParamGroup",Name) != 0 )
		return true;

	return false;

}



bool FCParameterGrp::GetBool(const char* Name, bool bPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCBool",Name);
	// if not return preset
	if(!pcElem) return bPreset;
	// if yes check the value and return
	if(strcmp(StrX(pcElem->getAttribute(XStr("Value").unicodeForm())).c_str(),"1"))
		return false;
	else
		return true;	
}

void  FCParameterGrp::SetBool(const char* Name, bool bValue)
{
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCBool",Name);
	// and set the vaue
	pcElem->setAttribute(XStr("Value").unicodeForm(), XStr(bValue?"1":"0").unicodeForm());
	// trigger observer
	Notify(Name);
}

std::vector<bool> FCParameterGrp::GetBools(const char * sFilter)
{
	std::vector<bool>  vrValues;
	DOMElement *pcTemp;// = _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCBool");
	while( pcTemp)
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			if(strcmp(StrX(((DOMElement*)pcTemp)->getAttribute(XStr("Value").unicodeForm())).c_str(),"1"))
				vrValues.push_back(false);
			else
				vrValues.push_back(true);	
		}
		pcTemp = FindNextElement(pcTemp,"FCBool");
	}

	return vrValues;
}

std::map<std::string,bool> FCParameterGrp::GetBoolMap(const char * sFilter)
{
	std::map<std::string,bool>  vrValues;
	DOMElement *pcTemp;// = _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCBool");
	while( pcTemp)
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			if(strcmp(StrX(((DOMElement*)pcTemp)->getAttribute(XStr("Value").unicodeForm())).c_str(),"1"))
				vrValues[Name] = (false);
			else
				vrValues[Name] = (true);	
		}
		pcTemp = FindNextElement(pcTemp,"FCBool");
	}

	return vrValues;
}

long FCParameterGrp::GetInt(const char* Name, long lPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCInt",Name);
	// if not return preset
	if(!pcElem) return lPreset;
	// if yes check the value and return
	return atol (StrX(pcElem->getAttribute(XStr("Value").unicodeForm())).c_str());
}

void  FCParameterGrp::SetInt(const char* Name, long lValue)
{
	char cBuf[256];
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCInt",Name);
	// and set the vaue
	sprintf(cBuf,"%i",lValue);
	pcElem->setAttribute(XStr("Value").unicodeForm(), XStr(cBuf).unicodeForm());
	// trigger observer
	Notify(Name);
}

std::vector<long> FCParameterGrp::GetInts(const char * sFilter)
{
	std::vector<long>  vrValues;
	DOMNode *pcTemp;// = _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCInt") ;
	while( pcTemp )
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			vrValues.push_back( atol (StrX(((DOMElement*)pcTemp)->getAttribute(XStr("Value").unicodeForm())).c_str()) );
		}
		pcTemp = FindNextElement(pcTemp,"FCInt") ;
	}

	return vrValues;
}

std::map<std::string,long> FCParameterGrp::GetIntMap(const char * sFilter)
{
	std::map<std::string,long>  vrValues;
	DOMNode *pcTemp;// = _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCInt") ;
	while( pcTemp )
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			vrValues[Name] = ( atol (StrX(((DOMElement*)pcTemp)->getAttribute(XStr("Value").unicodeForm())).c_str()) );
		}
		pcTemp = FindNextElement(pcTemp,"FCInt") ;
	}

	return vrValues;
}

double FCParameterGrp::GetFloat(const char* Name, double dPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCFloat",Name);
	// if not return preset
	if(!pcElem) return dPreset;
	// if yes check the value and return
	return atof (StrX(pcElem->getAttribute(XStr("Value").unicodeForm())).c_str());
}

void  FCParameterGrp::SetFloat(const char* Name, double dValue)
{
	char cBuf[256];
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCFloat",Name);
	// and set the value
	sprintf(cBuf,"%f",dValue);
	pcElem->setAttribute(XStr("Value").unicodeForm(), XStr(cBuf).unicodeForm());
	// trigger observer
	Notify(Name);
}

std::vector<double> FCParameterGrp::GetFloats(const char * sFilter)
{
	std::vector<double>  vrValues;
	DOMElement *pcTemp ;//= _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCFloat") ;
	while( pcTemp )
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			vrValues.push_back( atof (StrX(((DOMElement*)pcTemp)->getAttribute(XStr("Value").unicodeForm())).c_str()) );
		}
		pcTemp = FindNextElement(pcTemp,"FCFloat");
	}

	return vrValues;
}

std::map<std::string,double> FCParameterGrp::GetFloatMap(const char * sFilter)
{
	std::map<std::string,double>  vrValues;
	DOMElement *pcTemp ;//= _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCFloat") ;
	while( pcTemp )
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			vrValues[Name] = ( atof (StrX(((DOMElement*)pcTemp)->getAttribute(XStr("Value").unicodeForm())).c_str()) );
		}
		pcTemp = FindNextElement(pcTemp,"FCFloat");
	}

	return vrValues;
}



void  FCParameterGrp::SetBlob(const char* Name, void *pValue, long lLength)
{
	// not implemented so far
	assert(0);
}

void FCParameterGrp::GetBlob(const char* Name, void * pBuf, long lMaxLength, void* pPreset)
{
	// not implemented so far
	assert(0);
}

void  FCParameterGrp::SetASCII(const char* Name, const char *sValue)
{
	// find or create the Element
	DOMElement *pcElem = FindOrCreateElement(_pGroupNode,"FCText",Name);
	// and set the value
    DOMNode *pcElem2 = pcElem->getFirstChild();
    if (!pcElem2)
    {
		DOMDocument *pDocument = _pGroupNode->getOwnerDocument();
		DOMText *pText = pDocument->createTextNode(XStr(sValue).unicodeForm());
		pcElem->appendChild(pText);
	}else{
		pcElem2->setNodeValue(XStr(sValue).unicodeForm());
	}
	// trigger observer
	Notify(Name);

}

void FCParameterGrp::GetASCII(const char* Name, char * pBuf, long lMaxLength, const char * pPreset)
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

std::string FCParameterGrp::GetASCII(const char* Name, const char * pPreset)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCText",Name);
	// if not return preset
	if(!pcElem) return std::string(pPreset);
	// if yes check the value and return
    DOMNode *pcElem2 = pcElem->getFirstChild();
    if (pcElem2)
		return std::string(StrX(pcElem2->getNodeValue()).c_str());	
	else
		return std::string(pPreset);
}

std::vector<std::string> FCParameterGrp::GetASCIIs(const char * sFilter)
{
	std::vector<std::string>  vrValues;
	DOMElement *pcTemp;// = _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCText");
	while( pcTemp  )
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			// retrive the text element
			DOMNode *pcElem2 = pcTemp->getFirstChild();
			if (pcElem2)
				vrValues.push_back( std::string(StrX(pcElem2->getNodeValue()).c_str()) );
		}
		pcTemp = FindNextElement(pcTemp,"FCText");
	}

	return vrValues;
}

std::map<std::string,std::string> FCParameterGrp::GetASCIIMap(const char * sFilter)
{
	std::map<std::string,std::string>  vrValues;
	DOMElement *pcTemp;// = _pGroupNode->getFirstChild();
	std::string Name;

	pcTemp = FindElement(_pGroupNode,"FCText");
	while( pcTemp)
	{
		Name = StrX( ((DOMElement*)pcTemp)->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str();
		// check on filter condition
		if(sFilter == NULL || Name.find(sFilter)!= std::string::npos)
		{
			// retrive the text element
			DOMNode *pcElem2 = pcTemp->getFirstChild();
			if (pcElem2)
				vrValues[Name] = std::string(StrX(pcElem2->getNodeValue()).c_str()) ;
		}
		pcTemp = FindNextElement(pcTemp,"FCText");
	}

	return vrValues;
}

//**************************************************************************
// Access methodes

void FCParameterGrp::RemoveGrp(const char* Name)
{
	// remove group handle
	_GroupMap.erase(Name);

	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCGrp",Name);
	// if not return 
	if(!pcElem) 
		return; 
	else
		_pGroupNode->removeChild(pcElem); 	
	// trigger observer
	Notify(Name);
}

void FCParameterGrp::RemoveASCII(const char* Name)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCText",Name);
	// if not return 
	if(!pcElem) 
		return; 
	else
		_pGroupNode->removeChild(pcElem); 	
	// trigger observer
	Notify(Name);

}

void FCParameterGrp::RemoveBool(const char* Name)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCBool",Name);
	// if not return 
	if(!pcElem) 
		return; 
	else
		_pGroupNode->removeChild(pcElem); 	

	// trigger observer
	Notify(Name);
}

void FCParameterGrp::RemoveBlob(const char* Name)
{
	/* not implemented yet
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCGrp",Name);
	// if not return 
	if(!pcElem) 
		return; 
	else
		_pGroupNode->removeChild(pcElem); 	
*/
}

void FCParameterGrp::RemoveFloat(const char* Name)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCFloat",Name);
	// if not return 
	if(!pcElem) 
		return; 
	else
		_pGroupNode->removeChild(pcElem); 	

	// trigger observer
	Notify(Name);
}

void FCParameterGrp::RemoveInt(const char* Name)
{
	// check if Element in group
	DOMElement *pcElem = FindElement(_pGroupNode,"FCInt",Name);
	// if not return 
	if(!pcElem) 
		return; 
	else
		_pGroupNode->removeChild(pcElem); 	

	// trigger observer
	Notify(Name);
}

void FCParameterGrp::Clear(void)
{
	std::vector<DOMNode*> vecNodes;

	// remove group handles
	_GroupMap.clear();

	// searching all nodes
	for (DOMNode *clChild = _pGroupNode->getFirstChild(); clChild != 0;  clChild = clChild->getNextSibling())
	{
		vecNodes.push_back(clChild);
	}

	// deleting the nodes
	for(std::vector<DOMNode*>::iterator It=vecNodes.begin();It!=vecNodes.end();It++)
		_pGroupNode->removeChild(*It);

	// trigger observer
	Notify(0);
}




//**************************************************************************
// Access methodes


DOMElement *FCParameterGrp::FindElement(DOMElement *Start, const char* Type, const char* Name)
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
					if(Name)
					{
						if (!strcmp(Name,StrX(clChild->getAttributes()->getNamedItem(XStr("Name").unicodeForm())->getNodeValue()).c_str()))
							return (DOMElement*)clChild;
					}
					else
						return (DOMElement*)clChild;

				}
			}
		}
	}
	return NULL;
}

DOMElement *FCParameterGrp::FindNextElement(DOMNode *Prev, const char* Type)
{
	DOMNode *clChild = Prev;
	if(!clChild) return 0l;

	while ((clChild = clChild->getNextSibling())!=0)
	{
		if (clChild->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			// the right node Type
			if (!strcmp(Type,StrX(clChild->getNodeName()).c_str()))
			{
				return (DOMElement*)clChild;
			}
		}
	}
	return NULL;
}

DOMElement *FCParameterGrp::FindOrCreateElement(DOMElement *Start, const char* Type, const char* Name)
{
	// first try to find it
	DOMElement *pcElem = FindElement(Start,Type,Name);

	if(!pcElem)
	{
		DOMDocument *pDocument = _pGroupNode->getOwnerDocument();

		pcElem = pDocument->createElement(XStr(Type).unicodeForm());
		pcElem-> setAttribute(XStr("Name").unicodeForm(), XStr(Name).unicodeForm());
		Start->appendChild(pcElem);
	}
	
	return pcElem;
}
	

//**************************************************************************
//**************************************************************************
// FCParameterManager
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static XercesDOMParser::ValSchemes    gValScheme       = XercesDOMParser::Val_Auto;

//**************************************************************************
// Construction/Destruction


/** Defauld construction
  * Does not much 
  */
FCParameterManager::FCParameterManager()	: FCParameterGrp()
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
#if defined(FC_OS_LINUX) || defined(FC_OS_CYGWIN)
			std::ostringstream err;
#else
			std::strstream err;
#endif
			char *pMsg = XMLString::transcode(toCatch.getMessage());
			err << "Error during Xerces-c Initialization.\n"
				 << "  Exception message:"
				 << pMsg;
			delete [] pMsg;
#ifdef FC_OS_LINUX
			throw FCException(err.str().c_str());
#else
			throw FCException(err.str());
#endif
		}
		Init = true;
	}
}

//**************************************************************************
// Document handling

bool FCParameterManager::LoadOrCreateDocument(const char* sFileName)
{
	int i=open(sFileName,O_RDONLY);
	if( i != -1)
	{
		close(i);
		LoadDocument(sFileName);
		return false;
	}else{
		CreateDocument();
		return true;
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
		std::cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << std::endl;
        errorsOccured = true;
    }

    catch (const DOMException& e)
    {
       std::cerr << "A DOM error occurred during parsing\n   DOMException code: "
             << e.code << std::endl;
        errorsOccured = true;
    }

    catch (...)
    {
        std::cerr << "An error occurred during parsing\n " << std::endl;
        errorsOccured = true;
    }

	if(errorsOccured) return 0;

	_pDocument = parser->getDocument();
	DOMElement* rootElem = _pDocument->getDocumentElement();

	_pGroupNode = FindElement(rootElem,"FCParamGroup","Root");

	if(!_pGroupNode) throw FCException("Malformed Parameter document: Root group not found");
	
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
        std::cerr << "An error occurred during creation of output transcoder. Msg is:"
            << std::endl
            << StrX(e.getMessage()) << std::endl;
     }

}

void  FCParameterManager::CreateDocument(void)
{
	// creating a document from screatch
	DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(XStr("Core").unicodeForm());
	_pDocument = impl->createDocument(
                    0,                                          // root element namespace URI.
                    XStr("FCParameters").unicodeForm(),         // root element name
                    0);                                         // document type object (DTD).

	// creating the node for the root group
	DOMElement* rootElem = _pDocument->getDocumentElement();
	_pGroupNode = _pDocument->createElement(XStr("FCParamGroup").unicodeForm());
	((DOMElement*)_pGroupNode)->setAttribute(XStr("Name").unicodeForm(), XStr("Root").unicodeForm());
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
    std::cerr << "Error at file \"" << StrX(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << StrX(toCatch.getMessage()) << std::endl;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;
    std::cerr << "Fatal Error at file \"" << StrX(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << StrX(toCatch.getMessage()) << std::endl;
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
    std::cout<<msg<<std::endl;
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
