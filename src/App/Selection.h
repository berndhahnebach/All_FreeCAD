/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


#ifndef __Selection_h__
#define __Selection_h__

// Std. configurations

#ifndef _PreComp_
# include <string>
# include <map>
#endif

#include <Base/Observer.h>

namespace App
{


/** transport the changes of the Selection
 *  This class transport closer information what was changed in a
 *  Selection. Its a optional information and not all commands set this
 *  information. If not set all observer of the selection assume a full change
 *  and update everything (e.g 3D view). This is not a very good idea if, e.g. only
 *  a small parameter whas changed. There for one can use this class and make the
 *  update of the document much faster!
 *@see FCObserver
 */
class AppExport SelectionChanges
{
public:
};



/** The Selction class 
 */
class AppExport SelectionSingelton :public FCSubject<const SelectionChanges&>
{
public:
  /// Construction
  SelectionSingelton();
  /// Destruction
  virtual ~SelectionSingelton();



  std::string getSelectionAsString(void);


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
  /// I am method one
  virtual void one(void)=0;
  /// I am method two
  virtual void two(void)=0;
  /// I am method three
  virtual void three(void)=0;
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

  // VC6 warns a DLL interface, ignore!
  std::string something;

};

} //namespace Base

#endif // __FILETEMPLATE_H__

