/** \file GUIConsole.h
 *  \brief The console helper class (Windows only)
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    5.2001
 *  \bug Nothing known
 *  \warning Uses windows.h !!!
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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
class GuiExport FCGUIConsole :public FCConsoleObserver
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
	//@}


  protected:
    static const unsigned int s_nMaxLines;
    static unsigned int       s_nRefCount;
};

#endif
