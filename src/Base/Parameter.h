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
#include <map>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMWriterFilter.hpp>
class DOMDocument;

#include "Handle.h"
#include "Observer.h"

class DOMNode;
class DOMElement;
class FCParameterManager;

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
 *  last used values in dialog boxes, seting and retriving all
 *  kind of preferences and so on.
 *  @see FCParameterManager
 */
class  BaseExport FCParametrGrp	: public FCHandled,public FCSubject
{

protected:
	FCParametrGrp(DOMElement *GroupNode=0L);
	~FCParametrGrp();

public:
	FCHandle<FCParametrGrp> GetGroup(const char* Name);
	typedef FCHandle<FCParametrGrp> handle;

	bool GetBool(const char* Name, bool bPreset=false);

	void  SetBool(const char* Name, bool bValue);

	long GetInt(const char* Name, long lPreset=0);

	void  SetInt(const char* Name, long lValue);

	double GetFloat(const char* Name, double dPreset=0.0);

	void  SetFloat(const char* Name, double dValue);


	void  SetBlob(const char* Name, void *pValue, long lLength);

	void GetBlob(const char* Name, void * pBuf, long lMaxLength, void* pPreset=NULL);

	void  SetASCII(const char* Name, const char *sValue);

	void GetASCII(const char* Name, char * pBuf, long lMaxLength, const char * pPreset=NULL);

	FCstring GetASCII(const char* Name, const char * pPreset=NULL);


	static void Init(void);

	friend FCParameterManager;

	/** Find a Element specified by Type and Name
	 *  Search in the parent element Start for the first occourenc of a 
	 *  element of Type and with the atribute Name=Name. On succes it returns
	 *  the pointer to that element, otherwise NULL
	 */
	DOMElement *FindElement(DOMElement *Start, const char* Type, const char* Name);

	/** Find a Element specified by Type and Name or create it if not found
	 *  Search in the parent element Start for the first occourenc of a 
	 *  element of Type and with the atribute Name=Name. On succes it returns
	 *  the pointer to that element, otherwise it create the element and return the pointer.
	 */
	DOMElement *FCParametrGrp::FindOrCreateElement(DOMElement *Start, const char* Type, const char* Name);


protected:

	/// DOM Node of the Base node of this group
	DOMElement *_pGroupNode;
	/// map of already exported groups
#	pragma warning( disable : 4251 )
	FCmap <FCstring ,FCHandle<FCParametrGrp> > _GroupMap;
#	pragma warning( default : 4251 )

};



/** The parameter manager class
 *  This class manage a parameter XML document. 
 *  Does loding, saving and handling the DOM document.
 *  @see FCParameterGrp
 */
class BaseExport FCParameterManager	: public FCParametrGrp
{
public:
	FCParameterManager();
	~FCParameterManager();
	static void Init(void);

	int   LoadDocument(const char* sFileName);

	void  LoadOrCreateDocument(const char* sFileName);

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
 *  This class wrapps the functionality of the TDFSdt_Label of OCC. 
 *  Its used for building up hirachy in a OCC document by representing
 *  Nodes and Leavs
 *  @see FCDocument
 */
class BaseExport FCPyParametrGrp :public FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

public:


	//---------------------------------------------------------------------
	// construction / destruction +++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	/// Constructer 
	FCPyParametrGrp(const FCHandle<FCParametrGrp> &rcParamGrp, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCPyParametrGrp();

	//---------------------------------------------------------------------
	// python exports  ++++++++++++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PYFUNCDEF_D (FCPyParametrGrp,PyGetGrp,sPyGetGrp);
//	PyObject *PyGetGrp(PyObject *args);	// Python wrapper
//	static PyObject *sPyGetGrp(PyObject *self, PyObject *args, PyObject *kwd){return ((FCPyParametrGrp*)self)->PyGetGrp(args);};
	PyObject *PySetBool(PyObject *args);	// Python wrapper
	static PyObject *sPySetBool(PyObject *self, PyObject *args, PyObject *kwd){return ((FCPyParametrGrp*)self)->PySetBool(args);};
	PyObject *PyGetBool(PyObject *args);	// Python wrapper
	static PyObject *sPyGetBool(PyObject *self, PyObject *args, PyObject *kwd){return ((FCPyParametrGrp*)self)->PyGetBool(args);};

protected:

	/// Pointer to the FCDocument where the label comes from 
#	pragma warning( disable : 4251 )
	FCHandle<FCParametrGrp> _cParamGrp;
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

inline ostream& operator<<(ostream& target, const StrX& toDump)
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

#define X(str) XStr(str).unicodeForm()


#endif
