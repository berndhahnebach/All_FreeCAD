/** \file FileTemplate.h
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


#ifndef __FILETEMPLATE_H__
#define __FILETEMPLATE_H__

// Std. configurations

//#include "PyExport.h"

//#include <string>
//#include <map>


/** A test class. A more elaborate class description.
 * Detaild description with some formating:
 *  \par
 *  bla blablablablablablabl:
 *  \code
 *  #include <Base/Console.h>
 *  GetConsole().Log("Stage: %d",i);
 *  \endcode
 *  \par
 *  nother blablablablablablablablablabl:
 * Text before the list
 * - list item 1
 *   - sub item 1
 *     - sub sub item 1
 *     - sub sub item 2
 *     . 
 *     The dot above ends the sub sub item list.
 *     More text for the first sub item
 *   .
 *   The dot above ends the first sub item.
 *   More text for the first list item
 *   - sub item 2
 *   - sub item 3
 * - list item 2
 * .
 * More text in the same paragraph.
 *
 * More text in a new paragraph.
 * Also with HTML tags:
 *  <ul>
 *  <li> mouse events
 *     <ol>
 *     <li>mouse move event
 *     <li>mouse click event\n
 *         More info about the click event.
 *     <li>mouse double click event
 *     </ol>
 *  <li> keyboard events
 *     <ol>     
 *     <li>key down event
 *     <li>key up event
 *     </ol>
 *  </ul>
 *  More text here.
 */
class BaseExport ClassTemplate
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	ClassTemplate();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~ClassTemplate();

	/**
	 * a normal member taking two arguments and returning an integer value.
	 * \par
	 * You can use a printf like interface like:
	 * \code
	 * GetConsole().Warning("Some defects in %s, loading anyway\n",str);
	 * \endcode
	 * @param a an integer argument.
	 * @param s a constant character pointer.
	 * @see ClassTemplate()
	 * @see ~ClassTemplate()
	 * @see testMeToo()
	 * @see publicVar()
	 * @return The test results
	 */
	int testMe(int a,const char *s);

	/** 
	 * An enum.
	 * More detailed enum description.
	 */

	enum TEnum { 
		  TVal1, /**< enum value TVal1. */  
		  TVal2, /**< enum value TVal2. */  
		  TVal3  /**< enum value TVal3. */  
		 } 
	*enumPtr, /**< enum pointer. Details. */
	enumVar;  /**< enum variable. Details. */

	/**
	 * A pure virtual member.
	 * @see testMe()
	 * @param c1 the first argument.
	 * @param c2 the second argument.
	 */
	virtual void testMeToo(char c1,char c2) = 0;

	/** @name a group of methodes */
	//@{
	/// I am mthode one
	virtual void One(void)=0;
	/// I am mthode Two
	virtual void Two(void)=0;
	/// I am mthode Three
	virtual void Three(void)=0;
	//@}


	/** 
	 * a public variable.
	 * Details.
	 */
	int publicVar;
 
	/**
	 * a function variable.
	 * Details.
	 */
	int (*handler)(int a,int b);

	// VC6 warns a DLL interface, ignor!
	std::string something;

};




#endif // __FILETEMPLATE_H__

