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
#include <Base/PyExport.h>

#error "selection hase moved to Gui! Dont use this any more"

namespace App
{

class Feature;

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



// Export an instance of the base class (to avoid warnning C4275, see also 
// C++ Language Reference/General Rules and Limitations on MSDN for more details.)
template class AppExport Base::Subject<const SelectionChanges&>;

/** The Selction class 
 */
class AppExport SelectionSingelton :public Base::Subject<const SelectionChanges&>
{
public:
  /// Construction
  SelectionSingelton();
  /// Destruction
  virtual ~SelectionSingelton();

  static SelectionSingelton& instance(void);
  static void destruct (void);


  std::string getSelectionAsString(void);

  void addFeature(Feature*);
  void removeFeature(Feature*);

  const std::set<Feature*> &Selection(void){ return _FeatureSet;}



protected:

  static SelectionSingelton* _pcSingleton;

  std::set<Feature*> _FeatureSet;

};


/// Get the global instance
inline AppExport SelectionSingelton& Selection(void)
{
  return SelectionSingelton::instance();
}




} //namespace App

#endif // __FILETEMPLATE_H__

