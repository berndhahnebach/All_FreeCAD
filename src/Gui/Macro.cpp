/** \file Macro.cpp
 *  \brief implementation of the macro manager
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <string>
#	include <stdio.h>
# include <qglobal.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Macro.h"
#include "../Base/Interpreter.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"





//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
FCMacroManager::FCMacroManager()
:_bIsOpen(false)
{}

FCMacroManager::~FCMacroManager(){}


//**************************************************************************
// separator for other implemetation aspects

void FCMacroManager::Open(MacroType eType,const char *sName)
{
	// check 
	assert(!_bIsOpen);
	assert(eType == File);

	_sName = sName;

	if(_sName.find(".FCMacro") == std::string::npos)
		_sName += ".FCMacro";


	_sMacroInProgress += "# Macro Begin: ";
	_sMacroInProgress += _sName;
	_sMacroInProgress += " +++++++++++++++++++++++++++++++++++++++++++++++++\n";
	//sMacroInProgress += "def Macro:\n\n";

	_bIsOpen = true;
}

/// close (and save) the recording sassion
void FCMacroManager::Commit(void)
{
	_sMacroInProgress += "# Macro End: ";
	_sMacroInProgress += _sName;
	_sMacroInProgress += " +++++++++++++++++++++++++++++++++++++++++++++++++\n";

	std::ofstream file(_sName.c_str());

	file << 	_sMacroInProgress.c_str();

	Base::Console().Log("FCMacroManager::Commit()\n%s\n",_sMacroInProgress.c_str());

	_sMacroInProgress = "";
	_sName = "";

	_bIsOpen = false;

}


/// cancels the recording sassion
void FCMacroManager::Cancel(void)
{
	_sMacroInProgress = "";
	_sName = "";

	_bIsOpen = false;
	
	
}

void FCMacroManager::AddLine(LineType Type,const char* sLine)
{
	//sMacroInProgress += "\t" + sLine + "\n";
	if(Type == Gui) 
			_sMacroInProgress += "#";	
	_sMacroInProgress += sLine;		
	_sMacroInProgress += "\n";		
}


void FCMacroManager::Run(MacroType eType,const char *sName)
{
  try
  {
  	Base::Interpreter().LaunchFile( sName );
  }
  catch ( const Base::Exception& e )
  {
    qWarning( e.what() );
  }
}




//**************************************************************************
//**************************************************************************
// Seperator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



