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
# include <vector>
# include <list>
# include <map>
#endif

#include <Base/Observer.h>
#include <Base/PyExport.h>

namespace App
{
  class Feature;
  class Document;
}

namespace Gui
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
class GuiExport SelectionChanges
{
public:
};



// Export an instance of the base class (to avoid warnning C4275, see also 
// C++ Language Reference/General Rules and Limitations on MSDN for more details.)
template class GuiExport Base::Subject<const SelectionChanges&>;

/** The Selction class 
 */
class GuiExport SelectionSingelton :public Base::Subject<const SelectionChanges&>
{
public:
  /// Construction
  SelectionSingelton();
  /// Destruction
  virtual ~SelectionSingelton();

  static SelectionSingelton& instance(void);
  static void destruct (void);


 // std::string getSelectionAsString(void);

//  void addFeature(App::Feature*);
//  void removeFeature(App::Feature*);

  /// Add a selection (for internal use)
  void addSelection(const char* pDocName, const char* pFeatName, const char* pSubName);
  /// Remove a selection (for internal use)
  void rmvSelection(const char* pDocName, const char* pFeatName, const char* pSubName);

  /** returns the number of selected objects with an special feature type
   *  Its the conviniant way to check if the right features selected to do 
   *  an operation (GuiCommand). The checking also detect base type. E.g. 
   *  "Part" also fits on "PartImport" or "PartTransform types.
   *  If no document name is given the active document is asumed.
   */
  unsigned int getNbrOfType(const char *TypeName, const char* pDocName=0);

  /** returns a vector of Features selected for the given Document name
   * if no document name is given the features from the active document are returnd.
   * If nothing for this Document is selected a empty vector is returnd.
   * The vector reflects the sequence of selection!
   */
  std::vector<App::Feature*> getSelectedFeatures(const char* pDocName=0) const;

  struct SelObj {
    const char* DocName;
    const char* FeatName;
    const char* SubName;
    const char* TypeName;
    App::Document* pDoc;
    App::Feature*  pFeat;
  };

  /** returns a vector of selection objectse
   * if no document name is given the objects of the active are returned.
   * If nothing for this Document is selected a empty vector is returnd.
   * The vector reflects the sequence of selection!
   */
  std::vector<SelObj> getSelection(const char* pDocName=0);

  /// clears the selection
  void clearSelection(void);

  /// size of selcted enteties for all docuements
  unsigned int size(void){return _SelList.size();}

protected:

  /// helper to retrive document by name
  App::Document* getDocument(const char* pDocName=0);


  struct _SelObj {
    std::string DocName;
    std::string FeatName;
    std::string SubName;
    std::string TypeName;
    App::Document* pDoc;
    App::Feature*  pFeat;
  };
  std::list<_SelObj> _SelList;

  static SelectionSingelton* _pcSingleton;

};


/// Get the global instance
inline GuiExport SelectionSingelton& Selection(void)
{
  return SelectionSingelton::instance();
}




} //namespace Gui

#endif // __FILETEMPLATE_H__

