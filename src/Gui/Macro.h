/** \file Macro.h
 *  \brief Implements the macro recording and play back facility
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


#ifndef __MACRO_H__
#define __MACRO_H__

// Std. configurations

#include <string>


/** Macro recording and play back management
 * The purpos of this class is to handle record 
 * function calls from a FCCommand and save it in
 * a macro file (so far). 
 */
class GuiExport FCMacroManager
{

protected:
	/**
	 * A constructor.
	 */
	FCMacroManager();

	/**
	 * A destructor.
	 */
	~FCMacroManager();

public:

	/** Macro type enumeration  */
	enum MacroType { 
		  File, /**< The macro will be saved in a file */  
		  App,  /**< The macro belongs to the Application and will be saved in the UserParameter */  
		  Doc   /**< Teh macro belongs to the Document and will be saved and restored with the Document */  
		 }; 

	/** Line type enumeration  */
	enum LineType { 
		  Base,  /**< The line effects the Gui (FreeCADGui) */  
		  Gui,   /**< The line effects only the document and Application (FreeCAD) */    
		 }; 


	/** Opens a new Macro recording sassion
	 * Starts a sassion with the Type and the Name of the Macro.
	 * All user interactions will be recordet as long as the Commit() or
	 * Cancel() is called. 
	 * There is only one recording sassion possible. To open a second one
	 * cause a exception:
	 * @param eType Type of the macro
	 * @param sName Name or Path of the Macro
	 * @see Commit()
	 * @see Cancel()
	 */
	void Open(MacroType eType,const char *sName);

	/// close (and save) the recording sassion
	void Commit(void);

	/// cancels the recording sassion
	void Cancel(void);

	/// indicates if a macro recording in in progress
	bool IsOpen(void){return _bIsOpen;}

	void AddLine(LineType Type,const char* sLine);

	void Run(MacroType eType,const char *sName);

	friend class ApplicationWindow;

protected:

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#endif
	/** Container for the macro */
	std::string _sMacroInProgress;

	/// name of the macro
	std::string _sName;

	bool _bIsOpen;
 
};




#endif // __MACRO_H__