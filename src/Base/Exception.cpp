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


#include "Exception.h"
#include "Console.h"

using namespace Base;


Exception::Exception(void)
{
  _sErrMsg = "FreeCAD Exception";
}

Exception::Exception(const Exception &inst)
{
	SetMessage(inst._sErrMsg.c_str());
}


Exception::Exception(const char * sMessage)
{
  SetMessage(sMessage);
//  ReportException();
}


Exception &Exception::operator=(const Exception &inst)
{
  SetMessage(inst._sErrMsg.c_str());
  return *this;
}

const char* Exception::what(void) const throw()
{
	return _sErrMsg.c_str();
}

void Exception::ReportException (void) const
{
	Console().Error("\nException (%s): %s \n",Console().Time(),what());
}
