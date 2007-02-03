/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <map>
#include <string>
#include <vector>

#include "propertyeditoritem.h"

namespace App {
class Property;
}

namespace Gui {
namespace PropertyEditor {

/** Implementation of a property editor similar to this one of Qt designer.
 *
 * EditableListView contains all EditableItem objects with an editor inside.
 * The implementation is based on the sources of book "Practical Qt" in chapter 3.
 *
 * \author Werner Mayer
 */
class EditableListView :public Q3ListView
{
    Q_OBJECT
public:
  /** Constructs a list view. */
  EditableListView( QWidget* parent=0);
  /** Sets \a item to be open if \a open is true and \a item is expandable, 
   * and to be closed if \a open is false. Deletes the current editor and 
   * repaints accordingly.
   */
  void setOpen ( Q3ListViewItem * item, bool open );
  /** Stops the editor of the current active item. If no editor is open nothing
   * happens.
   */
  void stopEdit();
  /** Builds up the list view with the properties. */
  void buildUp( const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, unsigned long ct );
  /** Sets the list view to be sorted by column in ascending order if ascending is TRUE or descending 
   * order if it is FALSE. It stops also the current editor.
   */
  void setSorting ( int column, bool ascending = TRUE );

protected Q_SLOTS:
  /** This slot is connected with the clicked signal and calls itemChanged(). */
  void mouseClick(Q3ListViewItem * item );
  /** Deletes the editor of the last selected item and creates a new one
   * for the curreent item \a item.
   */
  void itemChanged(Q3ListViewItem * item );
  /** Updates the current editable item. */
  void updateItem();

protected:
  void resizeEvent( QResizeEvent* e );
//void drawContentsOffset ( QPainter * p, int ox, int oy, int cx, int cy, int cw, int ch );

private:
  EditableItem* _editingItem;
};

} //namespace PropertyEditor
} //namespace Gui

#endif // PROPERTYEDITOR_H
