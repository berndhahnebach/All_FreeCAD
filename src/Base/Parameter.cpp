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



#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <assert.h>
#endif


#include "Parameter.h"
#include "Exception.h"

//**************************************************************************
// Construction/Destruction


/** Defauld construction
  * Does not much 
  */
FCParameter::FCParameter()
{

}


/** Copy construction
  * Makes a one to one copy of all parameters and
  * groups in the container
  */
FCParameter::FCParameter(const FCParameter &rcParameter)
:mData(rcParameter.mData)
{

}

/** Destruction
  * complete destruction of the object
  */
FCParameter::~FCParameter(void)
{

}

//**************************************************************************
// Preset Values 

/** Preset floating point parameter values 
  * Presets a special Grp Key value means it will be set if its not already 
  * set bevore. This is mainly done to set default values known to work well 
  * at starting time of an Application or loding preset files. Preset makes 
  * some checks so its slower then SetKey...().
  * Preset returns true if the preset was done and false if the value was 
  * already set.
  * @see SetKeyFloat
  */
bool FCParameter::PresetKeyFloat (const tKeyString &Grp,const tKeyString &Key, double dWert)
{
	// check the group
	tParamData::iterator GrpIt = mData.find(Grp);
	if(GrpIt == mData.end())
	{
		SetKeyFloat(Grp,Key,dWert);
		return true;
	}else{
		std::map<tKeyString,boost::any>::iterator KeyIt = GrpIt->second.find(Key);
		if(KeyIt == GrpIt->second.end())
		{
			SetKeyFloat(Grp,Key,dWert);
			return true;
		}else{
			return false;
		}
	}

}

/** Preset string parameter values
  * Presets a special Grp Key value means it will be set if its not already 
  * set bevore. This is mainly done to set default values known to work well 
  * at starting time of an Application or loding preset files. Preset makes 
  * some checks so its slower then SetKey...().
  * Preset returns true if the preset was done and false if the value was 
  * already set.
  * @see SetKeyString
  */
bool FCParameter::PresettKeyString(const tKeyString &Grp,const tKeyString &Key, const tValueString &rcValue)
{
	// check the group
	tParamData::iterator GrpIt = mData.find(Grp);
	if(GrpIt == mData.end())
	{   // groupe not there
		SetKeyString(Grp,Key,rcValue);
		return true;
	}else{
		// checking Key
		std::map<tKeyString,boost::any>::iterator KeyIt = GrpIt->second.find(Key);
		if(KeyIt == GrpIt->second.end())
		{   // Key not there
			SetKeyString(Grp,Key,rcValue);
			return true;
		}else{
			return false;
		}
	}
}
/** Preset integer parameter values
  * Presets a special Grp Key value means it will be set if its not already 
  * set bevore. This is mainly done to set default values known to work well 
  * at starting time of an Application or loding preset files. Preset makes 
  * some checks so its slower then SetKey...().
  * Preset returns true if the preset was done and false if the value was 
  * already set.
  * @see SetKeyString
  */
bool FCParameter::PresetKeyLong  (const tKeyString &Grp,const tKeyString &Key, long lWert)
{
	// check the group
	tParamData::iterator GrpIt = mData.find(Grp);
	if(GrpIt == mData.end())
	{   // groupe not there
		SetKeyLong(Grp,Key,lWert);
		return true;
	}else{
		// checking Key
		std::map<tKeyString,boost::any>::iterator KeyIt = GrpIt->second.find(Key);
		if(KeyIt == GrpIt->second.end())
		{   // Key not there
			SetKeyLong(Grp,Key,lWert);
			return true;
		}else{
			return false;
		}
	}
}





//**************************************************************************
// Set methodes

/** Set floating point parameter values 
  * Set the value of a special key in a special Group
  */
void FCParameter::SetKeyFloat (const tKeyString &Grp,const tKeyString &Key, double dWert)
{
	// explicite cast to the boost::any value
	boost::any to_append = dWert;
	// appending to (Grp,key) map
	mData[Grp][Key] = to_append;
}


/** Set string parameter values
  * Set the value of a special key in a special Group
  */
void FCParameter::SetKeyString(const tKeyString &Grp,const tKeyString &Key, const tValueString &rcValue)
{
	// explicite cast to the boost::any value
	boost::any to_append = rcValue;
	// appending to (Grp,key) map
	mData[Grp][Key] = to_append;

}


/** Set integer parameter values
  * Set the value of a special key in a special Group
  */
void FCParameter::SetKeyLong  (const tKeyString &Grp,const tKeyString &Key, long lWert)
{
	// explicite cast to the boost::any value
	boost::any to_append = lWert;
	// appending to (Grp,key) map
	mData[Grp][Key] = to_append;
}



//**************************************************************************
// Get methodes

/** Get floating point parameter values 
  * Returns the specified value. In debug a type checking is active which 
  * reviel wrong casts.
  */
double FCParameter::GetKeyFloat (const FCParameter::tKeyString &Grp,const FCParameter::tKeyString &Key, double dWert)
{
	// get the Value
	double* pdValue = boost::any_cast<double> (&mData[Grp][Key]);
	// check on cast
	if(!pdValue) throw FCException("Wring type conversion in _FILE_ _LINE_");
	// returns the type
	return *pdValue;
}

/** Get string parameter values
  * Returns the specified value. In debug a type checking is active which 
  * reviel wrong casts.
  */
const FCParameter::tValueString &FCParameter::GetKeyString(const tKeyString &Grp,const tKeyString &Key, const tValueString &rcValue)
{
	// get the Value
	tValueString* pcValue = boost::any_cast<tValueString> (&mData[Grp][Key]);
	// check on cast
	if(!pcValue) throw FCException("Wring type conversion in _FILE_ _LINE_");
	// returns the type
	return *pcValue;
}


/** Get integer parameter values
  * Returns the specified value. In debug a type checking is active which 
  * reviel wrong casts.
  */
long FCParameter::GetKeyLong  (const tKeyString &Grp,const tKeyString &Key, long lWert)
{
	// get the Value
	long* plValue = boost::any_cast<long> (&mData[Grp][Key]);
	// check on cast
	if(!plValue) throw FCException("Wring type conversion in _FILE_ _LINE_");
	// returns the type
	return *plValue;
}



//**************************************************************************
// Delete methodes

/** Delete a whole group
  * Delete the group with all the keys in it
  */
void FCParameter::DelGroup(const tKeyString &Grp)
{
	// delete
	mData.erase(Grp);
}

/** Delete explicitely a key
  * Delete the group with all the keys in it
  */
void FCParameter::DeleteKey (const tKeyString &Grp,const tKeyString &Key)
{
	mData[Grp].erase(Key);
}

