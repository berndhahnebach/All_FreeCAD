/** \file Parameter.h
 *  \brief The base::parameter module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  here is the basic parameter engine defined
 *  @see Parameter.cpp
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


#ifndef __PARAMETER_H__
#define __PARAMETER_H__

// Std. configurations
#include "PyExport.h"


// Include files
#ifdef FC_OS_LINUX
#  include <sstream>
#endif
#include <map>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMWriterFilter.hpp>
#include <vector>
class DOMDocument;

#include "Handle.h"
#include "Observer.h"

class DOMNode;
class DOMElement;
class FCParameterManager;

/*
class BaseExport FCParameterIterator
{
public:
	FCParameterIterator(void);
	FCParameterIterator(const FCParameterIterator &ToCopy);

	void operator=(const FCParameterIterator &ToCopy);

	virtual const char*Type(void)=0;

	operator++(void);

private:
	/// DOM Node of the Base node of this group
	DOMElement *_pIterNode;
}

class BaseExport FCParameterGroupIterator
{
public:
	FCParameterGroupIterator(void):FCParameterIterator(){};
	FCParameterGroupIterator(const FCParameterIterator &ToCopy):FCParameterIterator(ToCopy){};

	

}

*/


/** The parameter container class
 *  This is the base class of all classes handle parameter.
 *  The class contains a map of key-value pairs in a grouping 
 *  structure, not unlike the windows registry.
 *  It allows the user to set and retrieve values of the 
 *  type float, long and string. Also it handles importing
 *  and exporting groups of parameters and enables streaming
 *  to a persistent medium via XML.
 *  \par
 *  Its main task is making user parameter persitent, saving 
 *  last used values in dialog boxes, setting and retrieving all
 *  kind of preferences and so on.
 *  @see FCParameterManager
 */
class  BaseExport FCParameterGrp	: public FCHandled,public FCSubject
{


public:
	/** @name methodes for group handling */
	//@{
	/// get a handle to a sub group or creat one
	FCHandle<FCParameterGrp> GetGroup(const char* Name);
	/// get a vector of all sub groups in this group
	std::vector<FCHandle<FCParameterGrp> > GetGroups(void);
	/// test if this group is emty
	bool IsEmpty(void);
	/// test if a special sub group is in this group
	bool HasGroup(const char* Name);
	/// type of the handle
	typedef FCHandle<FCParameterGrp> handle;
	/// remove a sub group from this group
	void RemoveGrp(const char* Name);
	/// clears everithing in this group (all types)
	void Clear(void);
	//@}

	/** @name methodes for bool handling */
	//@{
	/// read bool values or give default
	bool GetBool(const char* Name, bool bPreset=false);
	/// set a bool value
	void SetBool(const char* Name, bool bValue);
	/// get a vector of all bool values in this group
	std::vector<bool> GetBools(const char * sFilter = NULL);
	/// get a map with all bool values and the keys of this group
	std::map<std::string,bool> FCParameterGrp::GetBoolMap(const char * sFilter = NULL);
	/// remove a bool value from this group
	void RemoveBool(const char* Name);
	//@}

	/** @name methodes for Int handling */
	//@{
	/// read bool values or give default
	long GetInt(const char* Name, long lPreset=0);
	/// set a int value
	void SetInt(const char* Name, long lValue);
	/// get a vector of all int values in this group
	std::vector<long> GetInts(const char * sFilter = NULL);
	/// get a map with all int values and the keys of this group
	std::map<std::string,long> GetIntMap(const char * sFilter = NULL);
	/// remove a int value from this group
	void RemoveInt(const char* Name);
	//@}


	/** @name methodes for Float handling */
	//@{
	/// set a float value
	double GetFloat(const char* Name, double dPreset=0.0);
	/// read float values or give default
	void SetFloat(const char* Name, double dValue);
	/// get a vector of all float values in this group
	std::vector<double> GetFloats(const char * sFilter = NULL);
	/// get a map with all float values and the keys of this group
	std::map<std::string,double> GetFloatMap(const char * sFilter = NULL);
	/// remove a float value from this group
	void RemoveFloat(const char* Name);
	//@}


	/** @name methodes for Blob handling (not implemented yet) */
	//@{
	/// set a blob value
	void  SetBlob(const char* Name, void *pValue, long lLength);
	/// read blob values or give default
	void GetBlob(const char* Name, void * pBuf, long lMaxLength, void* pPreset=NULL);
	/// remove a blob value from this group
	void RemoveBlob(const char* Name);
	//@}

	
	
	/** @name methodes for Blob handling (not implemented yet) */
	//@{
	/// set a string value
	void  SetASCII(const char* Name, const char *sValue);
	/// read a string values with a buffer
	void GetASCII(const char* Name, char * pBuf, long lMaxLength, const char * pPreset=NULL);
	/// read a string values 
	std::string GetASCII(const char* Name, const char * pPreset=NULL);
	/// remove a string value from this group
	void RemoveASCII(const char* Name);
	/** Return all string elements in this group as a vector of strings
	 *  Its also possible to set a filter criteria.
	 *  @param sFilter only strings which name includes sFilter are put in the vector
	 *  @return std::vector of std::strings
	 */
	std::vector<std::string> GetASCIIs(const char * sFilter = NULL);
	/// Same as GetASCIIs() but with key,value map
	std::map<std::string,std::string> FCParameterGrp::GetASCIIMap(const char * sFilter = NULL);
	//@}

	static void Init(void);

	friend FCParameterManager;

	/// returns the name
	const char* GetGroupName(void) {return _cName.c_str();}

protected:
	/// constructor is protected (handle concept)
	FCParameterGrp(DOMElement *GroupNode=0L,const char* sName=0L);
	/// destructor is protected (handle concept)
	~FCParameterGrp();
	/// helper function for GetGroup
	FCHandle<FCParameterGrp> _GetGroup(const char* Name);

	DOMElement *FindNextElement(DOMNode *Prev, const char* Type);

	/** Find an element specified by Type and Name
	 *  Search in the parent element Start for the first occourrence of an 
	 *  element of Type and with the attribute Name=Name. On success it returns
	 *  the pointer to that element, otherwise NULL
	 *  If the names not given he returns the first occourence fo Type.
	 */
	DOMElement *FindElement(DOMElement *Start, const char* Type, const char* Name=0L);

	/** Find an element specified by Type and Name or create it if not found
	 *  Search in the parent element Start for the first occourrence of an 
	 *  element of Type and with the attribute Name=Name. On success it returns
	 *  the pointer to that element, otherwise it creates the element and returns the pointer.
	 */
	DOMElement *FCParameterGrp::FindOrCreateElement(DOMElement *Start, const char* Type, const char* Name);


	/// DOM Node of the Base node of this group
	DOMElement *_pGroupNode;
#	pragma warning( disable : 4251 )
	/// the own name
	std::string _cName;
	/// map of already exported groups
	std::map <std::string ,FCHandle<FCParameterGrp> > _GroupMap;
#	pragma warning( default : 4251 )

};



/** The parameter manager class
 *  This class manages a parameter XML document. 
 *  Does loding, saving and handling the DOM document.
 *  @see FCParameterGrp
 */
class BaseExport FCParameterManager	: public FCParameterGrp
{
public:
	FCParameterManager();
	~FCParameterManager();
	static void Init(void);

	int   LoadDocument(const char* sFileName);

	bool  LoadOrCreateDocument(const char* sFileName);

	void  SaveDocument(const char* sFileName);

	void  CreateDocument(void);

	void  CheckDocument();
protected:
private:

	DOMDocument   *_pDocument;

	bool          gDoNamespaces         ;
	bool          gDoSchema             ;
	bool          gSchemaFullChecking   ;
	bool          gDoCreate             ;


	const XMLCh*  gOutputEncoding       ;
	const XMLCh*  gMyEOLSequence        ;

	bool          gSplitCdataSections   ;
	bool          gDiscardDefaultContent;
	bool          gUseFilter            ;
	bool          gFormatPrettyPrint    ;

};



/** The OCC Label wrapper class
 *  This class wraps the functionality of the TDFSdt_Label of OCC. 
 *  It's used for building up hierarchy in a OCC document by representing
 *  nodes and leaves
 *  @see FCDocument
 */
class BaseExport FCPyParameterGrp :public FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

public:


	//---------------------------------------------------------------------
	// construction / destruction +++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	/// Constructor 
	FCPyParameterGrp(const FCHandle<FCParameterGrp> &rcParamGrp, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCPyParameterGrp();

	//---------------------------------------------------------------------
	// python exports  ++++++++++++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PYFUNCDEF_D (FCPyParameterGrp,PyGetGrp);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemGrp);
	PYFUNCDEF_D (FCPyParameterGrp,PyClear);
	PYFUNCDEF_D (FCPyParameterGrp,PyHasGroup);
	PYFUNCDEF_D (FCPyParameterGrp,PyIsEmpty);

	PYFUNCDEF_D (FCPyParameterGrp,PySetBool);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetBool);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemBool);

	PYFUNCDEF_D (FCPyParameterGrp,PySetInt);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetInt);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemInt);

	PYFUNCDEF_D (FCPyParameterGrp,PySetFloat);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetFloat);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemFloat);

	PYFUNCDEF_D (FCPyParameterGrp,PySetString);
	PYFUNCDEF_D (FCPyParameterGrp,PyGetString);
	PYFUNCDEF_D (FCPyParameterGrp,PyRemString);


protected:

	/// Pointer to the FCDocument where the label comes from 
#	pragma warning( disable : 4251 )
	FCHandle<FCParameterGrp> _cParamGrp;
#	pragma warning( default : 4251 )
};










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

inline bool DOMTreeErrorReporter::getSawErrors() const
{
    return fSawErrors;
}



// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class BaseExport StrX
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

//#define (str) XStr(str).unicodeForm()


#endif
