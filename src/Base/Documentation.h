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

#include "PyExport.h"

#include <string>
#include <map>








/** Doc type enum 
 * specify the type of document
 */
enum TDocType { 
	  html,     /**< is a html coded document. */  
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
class BaseExport FCDocumentationManager
{
public:

	/** the constructor
	 * @param sRootPath set the root path the provider is hooked on.
	 */
	FCDocumentationManager(void){};

	/// virtual destructor
	virtual ~FCDocumentationManager(){};

	/** Retrive a given Document 
	 * @param URL give the path (without any extension).
	 * @param rtWhichType returns the type of document.
	 * @return a string with the document
	 */
	virtual std::string Retrive(const char * URL, TDocType& rtWhichType );

	/** Save a edited Document through this provider
	 * @param URL give the path (without any extension).
	 * @param rtWhichType specify the type of document.
	 */
	virtual void Save(const char * URL,const char* sDoc, TDocType tWhichType );

	void AddProvider(FCDocumentationProvider* pcProvider);

protected:
	
	std::map<const char*,FCDocumentationProvider*> mpcProviderMap;
};




#endif // __DOCUMENTATION_H__