/** \file DocType.h
 *  \brief The document type system
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2000 - 2003                 *   
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
 *   Juergen Riegel 2003                                                   *
 ***************************************************************************/


#ifndef __FCDocType_H__
#define __FCDocType_H__

class FCDocument;
class FCPyObject;

namespace App {

/** Base class of Document type objects

 */
class AppExport DocType
{
public:

       
	/**
	 * A constructor.
	 */
	DocType();

	/**
	 * A destructor.
	 */
	virtual ~DocType();

	/**
	 * sets up the document
	 */
	virtual void Init (FCDocument *pcDoc)=0;

	/**
	 * sets up the document
	 */
	virtual const char *GetTypeName(void);

	virtual FCPyObject *GetPyObject(void)=0;

};


/** Standard Document type objects
 *  If not choosed a special DocType (e.g. From a module) the 
 *  Standard DocType object get in place in the document
 */
class AppExport DocTypeStd: public DocType
{
public:

       
	/**
	 * A constructor.
	 */
	DocTypeStd();

	/**
	 * A destructor.
	 */
	virtual ~DocTypeStd();

	/**
	 * sets up the document
	 */
	virtual void Init (FCDocument *pcDoc);

	/**
	 * sets up the document
	 */
	virtual const char *GetTypeName(void);


	virtual FCPyObject *GetPyObject(void);



};


} //namespace App

#endif // __FCDocType_H__
