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
#include "Export.h"

#include "PyExport.h"

#include <string>
#include <map>
#include <boost/any.hpp>



/** The parameter container class
 *  This is the base class of all classes handle parameter.
 *  The class contains a map of key-value pairs in a grouping 
 *  structure, not unlike the windows registry.
 *  It allows the user to set and retrieve values of the 
 *  type float, long and string. Also it handles importing
 *  and exporting groups of parameters and enables streaming
 *  to a persistent medium via XML.
 *  \par
 *  @see FCParameterSet 
 */
class BaseExport FCParameter 
{

public:
	// Construction destruction *******************************************
	/// Defauld construction
	FCParameter();
	/// Copy construction
	FCParameter(const FCParameter &rcParameter);
	/// Destruction
    ~FCParameter(void);


	// Typedefs ***********************************************************
	/// typedef of the Key string type
	typedef std::string	                                          tKeyString;
	/// typedef of the Value string type (maybe unicode)
	typedef std::string                                           tValueString;
	/// typedef of the container
	typedef std::map<tKeyString,std::map<tKeyString,boost::any> > tParamData;

	// Preset Values *********************************************************
	/// Preset floating point parameter values 
    bool PresetKeyFloat (const tKeyString &Grp,const tKeyString &Key, double dWert);
	/// Preset string parameter values
    bool PresettKeyString(const tKeyString &Grp,const tKeyString &Key, const tValueString &rcValue);
	/// Preset integer parameter values
    bool PresetKeyLong  (const tKeyString &Grp,const tKeyString &Key, long lWert);

	// Set Values *********************************************************
	/// Set floating point parameter values 
    void SetKeyFloat (const tKeyString &Grp,const tKeyString &Key, double dWert);
	/// Set string parameter values
    void SetKeyString(const tKeyString &Grp,const tKeyString &Key, const tValueString &rcValue);
	/// Set integer parameter values
    void SetKeyLong  (const tKeyString &Grp,const tKeyString &Key, long lWert);

	// Get Values *********************************************************
	/// Get floating point parameter values 
    double GetKeyFloat (const tKeyString &Grp,const tKeyString &Key, double dWert);
	/// Get string parameter values
    const tValueString &GetKeyString(const tKeyString &Grp,const tKeyString &Key, const tValueString &rcValue);
	/// Get integer parameter values
    long GetKeyLong  (const tKeyString &Grp,const tKeyString &Key, long lWert);

	// deletion ***********************************************************
	/// Delete a whole group
    void DelGroup(const tKeyString &Grp);    
	/// Delete explicitely a key
    void DeleteKey (const tKeyString &Grp,const tKeyString &Key);

    
private:
	/// Container for the 2 level key value structure
	tParamData mData;

};


#endif
