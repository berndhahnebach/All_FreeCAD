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
#include "../Base/PyExport.h"

class DocTypeStdPy;

namespace App {

class Feature;

/** Base class of Document type objects

 */
class AppExport DocType
{
public:

       
	/** @name Basics */
	//@{
	/** constructor
	 * A constructor.
	 */
	DocType();

	/** destructor
	 * A destructor.
	 */
	virtual ~DocType();

	/** Return the type name as string
	 *  this methode must be overided by children of this class and return the 
	 *  name of type object.
	 */
	virtual const char *GetTypeName(void);

	/// returns the python wrapper
	virtual Base::FCPyObject *GetPyObject(void)=0;
	//@}

	
	/** sets up the document
	 *  Get called when the type object is attached to a newly
	 *  created document. This methode sets up the basic structure of the document.
	 */
	virtual void Init (FCDocument *pcDoc)=0;


};


/** Standard Document type objects
 *  If not choosed a special DocType (e.g. From a module) the 
 *  Standard DocType object get in place in the document
 */
class AppExport DocTypeStd: public DocType, public Base::PyHandler
{
public:

       
	/** @name Basics */
	//@{
	
	/// Constructor
	DocTypeStd();

	/// Destructor
	virtual ~DocTypeStd();

	///returns the type name of DocumentType object
	virtual const char *GetTypeName(void);

	/// returns the python wrapper
	virtual Base::FCPyObject *GetPyObject(void);
	//@}

	/** @name Document handling  */
	//@{
	/** sets up the document
	 *  Get called when the 
	 */
	virtual void Init (FCDocument *pcDoc);
	//@}

	/** @name Feature handling  */
	//@{
  /// Add a feature (by name) to this document and set it active
	Feature *AddFeature(const char* sName);
  /// Returns the active Feature of this document
	Feature *GetActiveFeature(void);
  /// Updates the document (recalculate)
  void UpdateDoc(void);
	//@}

	/** @name State handling  */
	//@{

	//@}

protected:
	TDF_Label _lBase;
	TDF_Label _lPos;
	TDF_Label _lFeature;
	int       _iNextFreeFeature;
	TDF_Label _lActiveFeature;

  FCDocument *_pcDoc;

	/// The one and only python object of this DocTypeStd object
	DocTypeStdPy *_pcDocTypeStdPy;

};


} //namespace App

#endif // __FCDocType_H__
