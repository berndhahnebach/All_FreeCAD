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

#ifndef __GUICONSOLE_H__
#define __GUICONSOLE_H__


#include "../Base/Console.h"

/** The console window class
 *  This class opens a console window when instaciated 
 *  and redirect the stdio streams to it as long it exists. 
 *  This is for Windows only!
 *  After instanciation it automaticly register itself at
 *  the FCConsole class and gets all the FCConsoleObserver
 *  massages. The class must not used directly! Only the 
 *  over the FCConsole class is allowec!
 *  @see FCConsole
 */
class GuiExport FCGUIConsole :public Base::ConsoleObserver
{
  public:
    /// Constructor
    FCGUIConsole(void);
    /// Destructor
    virtual ~FCGUIConsole(void);
	//@{
    /** Observer implementation */
	virtual void Warning(const char *sWarn);
	virtual void Message(const char *sMsg);
	virtual void Error  (const char *sErr);
	virtual void Log    (const char *sErr);
  const char* Name(void){return "GUIConsole";}

	//@}


  protected:
    static const unsigned int s_nMaxLines;
    static unsigned int       s_nRefCount;
};

#endif
