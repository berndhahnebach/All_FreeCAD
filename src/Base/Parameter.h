/** \file Parameter.h
 *  \brief The console module
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    5.2001
 *  \bug Nothing known
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/


#ifndef __PARAMETER_H__
#define __PARAMETER_H__

// Std. configurations
#include "Export.h"

#include "PyExport.h"
#define __STL_DESIGNATED_DLL 
#include <string>
#include <map>



/** The parameter container class
 *  This is the base class of all classes handle parameter.
 *  The class contains a map of key-value pairs in a pseudo 
 *  hierarchal structure, not unlike the windows registry.
 *  It allows the user to set and retrieve values of the 
 *  type float, long and string. Also it handles importing
 *  and exporting groups of parameters and enables streaming
 *  to a persistent medium.
 *  @see 
 */
class BaseExport FCParameter : public FCPythonExport 
{

public:
	FCParameter(){};
    virtual ~FCParameter(void);

    bool AddKeyFloat (const char *Key, float dWert);
    bool AddKeyString(const char *Key, const char *Wert);
    bool AddKeyLong  (const char *Key, long lWert);

    bool SetKeyFloat (const char *Key, float dWert);
    bool SetKeyString(const char *Key, const char *sWert);
    bool SetKeyLong  (const char *Key, long lWert);

    unsigned long GetGroup(const char *cKey, FCParameter &) ;
    void		  SetGroup(const char *sGrp, const FCParameter &cGrp);
    void		  DelGroup(const char *sGrp);
    
    bool DeleteKey (const char *rclKey);
    bool DeleteKeyFromString (const char *rclGroupKey, const char *rclValue);
    const char *FindKeyFromString (const char *rclGroupKey, const char *rclValue) const;

    float		GetKeyFloat (const char *Key, float fDef=0.0f);
    const char *GetKeyString(const char *Key, const char *sDef="");
    long		GetKeyLong  (const char *Key, long lDef=0l);

    unsigned long Print(std::string &cBuf);

	// scripting stuff
	virtual void PyRegister();
    virtual void PyInstance();

	PYFUNCDEF( SetKeyString );
	PYFUNCDEF( GetKeyString );


private:
#pragma warning( disable : 4251 )
	stlport::map<stlport::string,stlport::string> mData;
#pragma warning( default : 4251 )

};


#endif
