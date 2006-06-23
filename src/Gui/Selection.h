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

#include <string>
#include <vector>
#include <list>
#include <map>

#include <Base/Observer.h>
#include <Base/PyExport.h>
#include <Base/Type.h>

namespace App
{
  class DocumentObject;
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
  enum MsgType {
    AddSelection,
    RmvSelection,
    ClearSelection,
    SetPreselect,
    RmvPreselect
  };

  MsgType Type;

  const char* pDocName;
  const char* pObjectName;
  const char* pSubName;
  float x;
  float y;
  float z;
};



// Export an instance of the base class (to avoid warnning C4275, see also 
// C++ Language Reference/General Rules and Limitations on MSDN for more details.)
template class GuiExport Base::Subject<const SelectionChanges&>;

/** The Selcetion singleton class
 
 */
class GuiExport SelectionSingleton :public Base::Subject<const SelectionChanges&>
{
public:

  /// Add a selection 
  bool addSelection(const char* pDocName, const char* pObjectName=0, const char* pSubName=0, float x=0, float y=0, float z=0);
  /// Remove a selection (for internal use)
  void rmvSelection(const char* pDocName, const char* pObjectName=0, const char* pSubName=0);
  /// clears the selection
  void clearSelection(void);
  /// checks if selected
  bool isSelected(const char* pDocName, const char* pObjectName=0, const char* pSubName=0) const;
  /// checks if selected
  bool isSelected(App::DocumentObject*) const;

  bool setPreselect(const char* pDocName, const char* pObjectName, const char* pSubName, float x=0, float y=0, float z=0);
  void rmvPreselect();

  /** Returns the number of selected objects with an special Object type
   * Its the convenient way to check if the right Objects are selected to 
   * perform an operation (GuiCommand). The checking also detect base type. 
   * E.g. "Part" also fits on "PartImport" or "PartTransform types.
   * If no document name is given the active document is assumed.
   */
  unsigned int countObjectsOfType(const Base::Type& typeId, const char* pDocName=0) const;

  /** Returns a vector of features of type \a TypeName selected for the given document name \a pDocName.
   * If no document name is specified the Objects from the active document are regarded.
   * If no objects of this document are selected an empty vector is returned.
   * @note The vector reflects the sequence of selection.
   */
  std::vector<App::DocumentObject*> getObjectsOfType(const Base::Type& typeId, const char* pDocName=0) const;

  struct SelObj {
    const char* DocName;
    const char* FeatName;
    const char* SubName;
    const char* TypeName;
    App::Document* pDoc;
    App::DocumentObject*  pObject;
  };

  /** returns a vector of selection objects
   * if no document name is given the objects of the active are returned.
   * If nothing for this Document is selected a empty vector is returnd.
   * The vector reflects the sequence of selection!
   */
  std::vector<SelObj> getSelection(const char* pDocName=0) const;

  /** Returns a vector of all selection objects of all documents. */
  std::vector<SelObj> getCompleteSelection() const;


  /// size of selcted enteties for all docuements
  unsigned int size(void) const {return _SelList.size();}

  static SelectionSingleton& instance(void);
  static void destruct (void);

protected:

  /// Construction
  SelectionSingleton();
  /// Destruction
  virtual ~SelectionSingleton();


  /// helper to retrive document by name
  App::Document* getDocument(const char* pDocName=0) const;


  struct _SelObj {
    std::string DocName;
    std::string FeatName;
    std::string SubName;
    std::string TypeName;
    App::Document* pDoc;
    App::DocumentObject*  pObject;
    float x,y,z;
  };
  std::list<_SelObj> _SelList;

  static SelectionSingleton* _pcSingleton;

  std::string DocName;
  std::string FeatName;
  std::string SubName;
  float hx,hy,hz;

};


/// Get the global instance
inline GuiExport SelectionSingleton& Selection(void)
{
  return SelectionSingleton::instance();
}




} //namespace Gui

#endif // __FILETEMPLATE_H__

