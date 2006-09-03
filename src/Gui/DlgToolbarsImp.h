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


#ifndef DLGTOOLBARS_IMP_H
#define DLGTOOLBARS_IMP_H

#include "DlgToolbars.h"

namespace Gui {
class Command;
class CustomToolBar;
class ToolBarItem;

namespace Dialog {

/** This class implements the creation of user defined toolbars.
 * In the left panel are shown all command groups with their command objects.
 * If any changeable toolbar was created in the left panel are shown all commands 
 * of the currently edited toolbar, otherwise it is emtpy.
 * All changes to a toolbar is done immediately.
 * 
 * \author Werner Mayer
 */
class DlgCustomToolbars : public DlgCustomToolbarsBase
{ 
  Q_OBJECT

protected:
  DlgCustomToolbars( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  virtual ~DlgCustomToolbars();
  void reparent ( QWidget * parent, WFlags f, const QPoint & p, bool showIt = FALSE );

protected:
  virtual void refreshActionList();
  virtual void cancel();
  virtual void refreshToolBarList();

protected slots:
  void onItemActivated(const QString &);
  void onDoubleClickedAction(QListViewItem*);
  void onAddAction();
  void onRemoveAction();
  void onMoveUpAction();
  void onMoveDownAction();
  void onNewActionChanged( QListViewItem *i );
  void onAllActionsChanged( QListViewItem *i );
  void onCreateToolbar();
  void onDeleteToolbar();
  void onAddMacroAction(const QString&);
  void onRemoveMacroAction(const QString&);

private:
  void refreshFullActionList();

protected:
  /** List of all toolbars */
  ToolBarItem* _toolBars;
};

/** This class implements the creation of user defined toolbars.
 * @see DlgCustomToolbars
 * @see DlgCustomCmdbarsImp
 * \author Werner Mayer
 */
class DlgCustomToolbarsImp : public DlgCustomToolbars
{ 
  Q_OBJECT

public:
  DlgCustomToolbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgCustomToolbarsImp();

protected:
  void cancel();
};

} // namespace Dialog
} // namespace Gui

#endif
