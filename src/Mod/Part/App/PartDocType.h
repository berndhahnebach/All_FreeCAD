/** \file PartDocType.h
 *  \brief FileTemplate example header
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.cpp
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


#ifndef __PartDocType_H__
#define __PartDocType_H__

// Std. configurations

//#include <App/DocType.h>

using App::DocTypeStd;


namespace Part
{


/** The Part document type class
  */
class PartDocType: public DocTypeStd
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PartDocType();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PartDocType();

	/**
	 * sets up the document
	 */
  virtual void Init (App::Document *pcDoc);

	/**
	 * return the type name 
	 */
	virtual const char *PartDocType::GetTypeName(void);

	virtual Base::FCPyObject *GetPyObject(void);

};


} // namespace Part

#endif // __PartDocType_H__