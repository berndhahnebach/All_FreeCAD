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
#include "PropertyPage.h"

#ifndef _PreComp_
# include <map>
# include <vector>
#endif

class FCCommand;

namespace Gui {
class Gui::CustomToolBar;

namespace Dialog {

/** This class implements the creation of user defined toolbars.
 * In the left panel are shown all command groups with their command objects.
 * If any changeable toolbar was created in the left panel are shown all commands 
 * of the currently edited toolbar, otherwise it is emtpy.
 * All changes to a toolbar is done immediately.
 * 
 * \author Werner Mayer
 */
class DlgCustomToolbars : public DlgCustomToolbarsBase, public Gui::Dialog::PropertyPage
{ 
  Q_OBJECT

protected:
  DlgCustomToolbars( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  virtual ~DlgCustomToolbars();

protected:
  virtual void apply();
  virtual void cancel();
  virtual void updateData();

protected slots:
  void onItemActivated(const QString &);
  void onDoubleClickedAction(QListViewItem*);
  void onAddAction();
  void onRemoveAction();
  void onMoveUpAction();
  void onMoveDownAction();
  void onNewActionChanged( QListViewItem *i );
  void onAllActionsChanged( QListViewItem *i );

protected:
  /** Groups of commands */
  std::map<std::string, std::vector<FCCommand*> > _alCmdGroups;
  /** List of all toolbars */
  QPtrList<Gui::CustomToolBar>                _aclToolbars;
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
  void apply();
  void cancel();
  void updateData();

protected slots:
  void onCreateToolbar();
  void onDeleteToolbar();
};

} // namespace Dialog
} // namespace Gui

#endif
