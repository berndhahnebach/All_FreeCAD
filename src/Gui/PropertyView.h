/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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



#ifndef __PROPTERYVIEW_H__
#define __PROPTERYVIEW_H__

#include "DockWindow.h"

#ifndef _PreComp_
# include <qptrlist.h>
#endif

class QPixmap;
class FCProperty;

namespace Gui {
namespace PropertyEditor {

class EditableListView;

} // namespace PropertyEditor
} // namespace Gui

namespace Gui {
namespace DockWnd {

/** A test class. A more elaborate class description.
 */
class PropertyView :public Gui::DockView
{
public:
  /**
   * A constructor.
   * A more elaborate description of the constructor.
   */
  PropertyView(FCGuiDocument*  pcDocument,QWidget *parent=0,const char *name=0);

  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  virtual ~PropertyView();

  bool OnMsg(const char* pMsg);

  virtual const char *GetName(void){return "PropertyView";}

  /// is called when the above function is called to handle document change stuff
  virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument);
  /// get called when the document is changed or updated
  virtual void Update(void);

  static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;

private:
  Gui::PropertyEditor::EditableListView * _pPropEditor;;
};

} // namespace DockWnd
} // namespace Gui

#endif // __PROPTERYVIEW_H__
