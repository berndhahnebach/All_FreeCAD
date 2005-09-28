/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <assert.h>
# include <string>
# include <stdio.h>
# include <qglobal.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Macro.h"
#include "../Base/Interpreter.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"


using namespace Gui;

//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
MacroManager::MacroManager()
:_bIsOpen(false), _bRecordGui(true), _bGuiAsComment(true)
{
}

MacroManager::~MacroManager()
{
}


//**************************************************************************
// separator for other implemetation aspects

void MacroManager::open(MacroType eType,const char *sName)
{
  // check 
  assert(!_bIsOpen);
  assert(eType == File);

  _sModuleSet.clear();
  _sName = sName;

  if(_sName.find(".FCMacro") == std::string::npos)
    _sName += ".FCMacro";

  _sMacroInProgress = "# Macro Begin: ";
  _sMacroInProgress += _sName;
  _sMacroInProgress += " +++++++++++++++++++++++++++++++++++++++++++++++++\n";
  _sMacroInProgress += "import FreeCAD\n";

  _bIsOpen = true;

  Base::Console().Log("CmdM: Open macro: %s\n",_sName.c_str());

}

void MacroManager::setRecordGuiCommands(bool bRecord, bool bAsComment)
{
  _bRecordGui = bRecord;
  _bGuiAsComment = bAsComment;
}

/// close (and save) the recording sassion
void MacroManager::commit(void)
{
  _sMacroInProgress += "# Macro End: ";
  _sMacroInProgress += _sName;
  _sMacroInProgress += " +++++++++++++++++++++++++++++++++++++++++++++++++\n";

  std::ofstream file(_sName.c_str());

  file << 	_sMacroInProgress.c_str();

  Base::Console().Log("CmdM: Commit macro: %s\n",_sName.c_str());

  _sMacroInProgress = "";
  _sName = "";
  _sModuleSet.clear();
  _bIsOpen = false;
}


/// cancels the recording sassion
void MacroManager::cancel(void)
{
  _sMacroInProgress = "";
  _sName = "";
  _sModuleSet.clear();

  Base::Console().Log("CmdM: Candel macro: %s\n",_sName.c_str());

  _bIsOpen = false;
}

void MacroManager::addLine(LineType Type,const char* sLine)
{
  if(_bIsOpen)
  {
    //sMacroInProgress += "\t" + sLine + "\n";
    if(Type == Gui)
    {
      if (_bRecordGui&&_bGuiAsComment)
        _sMacroInProgress += "#";
      else if (!_bRecordGui)
        return; // ignore Gui commands
    }

    _sMacroInProgress += sLine;
    _sMacroInProgress += "\n";
  }
}

void MacroManager::setModule(const char* sModule)
{
  if(_bIsOpen && sModule && *sModule != '\0' && _sModuleSet.find(sModule) == _sModuleSet.end())
  {
    _sMacroInProgress += "import ";
    _sMacroInProgress += sModule;
    _sMacroInProgress += "\n";
    _sModuleSet.insert(sModule);
  }
}


void MacroManager::run(MacroType eType,const char *sName)
{
  try
  {
    Base::Interpreter().runFile( sName );
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





