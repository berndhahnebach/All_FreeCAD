/** \file Documentation.h
 *  \brief This module handels the retraivel and editing of build in documentation 
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
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


#ifndef __DOCUMENTATION_H__
#define __DOCUMENTATION_H__

// Std. configurations

//#include "PyExport.h"
typedef struct _object PyObject;

#include <string>
#include <list>








/** Doc type enum 
 * specify the type of document
 */
enum TDocType { 
	  TextPlain,/**< is a plain text coded document. */  
	  TextAnsi, /**< is a text coded document with ANSI higlighting. */  
	  Html,     /**< is a html coded document. */  
	  Tech,     /**< is a LaTex coded document. */ 
	  Doc,      /**< is a FreeCAD XML coded document. */  
	  Script,   /**< is a python script which produce a html coded document. */  
	  DocScript /**< is a python script which produce a Doc coded document. */  
	 };


/** Abstract base class of all DocumentationProvider
 */
class BaseExport FCDocumentationProvider
{
public:

	/** the constructor
	 * @param sRootPath set the root path the provider is hooked on.
	 */
	FCDocumentationProvider(const char* sRootPath);

	/// virtual destructor
	virtual ~FCDocumentationProvider();

	/** Retrive a given Document from this provider
	 * @param PathExtension give the path beond the root path of the provider (without any extension).
	 * @param rtWhichType returns the type of document.
	 * @return a string with the document
	 */
	virtual std::string Retrive(const char * PathExtension, TDocType& rtWhichType )=0;

	/** Save a edited Document through this provider
	 * @param PathExtension give the path beond the root path of the provider (without any extension).
	 * @param rtWhichType specify the type of document.
	 */
	virtual void Save(const char * PathExtension,const char* sDoc , TDocType tWhichType )=0;


	const char * GetRootPath(void){return _cRootPath.c_str();}


protected:
	
	std::string _cRootPath;

};



/** Abstract base class of all DocumentationProvider
 */
class BaseExport FCDocProviderDirectory : public FCDocumentationProvider
{
public:

	/** the constructor
	 * @param sRootPath set the root path the provider is hooked on.
	 */
	FCDocProviderDirectory(const char* sRootPath,const char* sDirPath );

	/// virtual destructor
	virtual ~FCDocProviderDirectory();

	/** Retrive a given Document from this provider
	 * @param PathExtension give the path beond the root path of the provider (without any extension).
	 * @param rtWhichType returns the type of document.
	 * @return a string with the document
	 */
	virtual std::string Retrive(const char * PathExtension, TDocType& rtWhichType );

	/** Save a edited Document through this provider
	 * @param PathExtension give the path beond the root path of the provider (without any extension).
	 * @param rtWhichType specify the type of document.
	 */
	virtual void Save(const char * PathExtension,const char* sDoc , TDocType tWhichType );




protected:
	
	std::string _cDirPath;

};



/** Abstract base class of all DocumentationProvider
 */
class BaseExport FCDocumentationManager
{
public:


	/** Retrive a given Document 
	 * @param URL give the path (without any extension).
	 * @param rtWhichType returns the type of document.
	 * @return a string with the document
	 */
	virtual std::string Retrive(const char * URL, TDocType tTypeRequest );

	/** Save a edited Document through this provider
	 * @param URL give the path (without any extension).
	 * @param rtWhichType specify the type of document.
	 */
	virtual void Save(const char * URL,const char* sDoc, TDocType tWhichType );

	void AddProvider(FCDocumentationProvider* pcProvider);

private:
	
	/// translate the document by a given XSL
	void _Translate(std::string &rcDoc, const char* sTransXSL);
	/// call the python script and delivers the output as a document
	void _CallScript(std::string &rcScript);
	/// list of registered Document Provider
	std::list<FCDocumentationProvider*> mpcProviderList;


	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	// static python wrapper of the exported functions
	static PyObject *sPyAddPath  (PyObject *self,PyObject *args,PyObject *kwd);
	static PyObject *sPyGet      (PyObject *self,PyObject *args,PyObject *kwd);
	static PyMethodDef    Methods[]; 

	// Singelton!
	/** the constructor */
	FCDocumentationManager(void);
	/// virtual destructor
	virtual ~FCDocumentationManager();
	// singelton 
	static void Destruct(void);
	static FCDocumentationManager &Instance(void);
	static FCDocumentationManager *_pcSingelton;
	friend FCDocumentationManager &GetDocumentationManager(void); 
	
};

inline FCDocumentationManager &GetDocumentationManager(void){
	return FCDocumentationManager::Instance();
}




#endif // __DOCUMENTATION_H__
