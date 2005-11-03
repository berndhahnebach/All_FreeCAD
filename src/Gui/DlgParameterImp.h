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


#ifndef DlgParameter_H
#define DlgParameter_H

#include "DlgParameter.h"
#include "../Base/Parameter.h"

#ifndef _PreComp_
# include <qlistview.h>
# include <qmap.h>
# include <qmenudata.h>
# include <qpainter.h>
#endif 

class QPopupMenu;

namespace Gui {
namespace Dialog {

class CustomMenuItem;
class ParameterGroupItem;

/**
 * The DlgParameterImp class implements a dialog showing all parameters in a list view.
 * \author Jürgen Riegel
 */
class DlgParameterImp : public DlgParameter
{ 
    Q_OBJECT

public:
  DlgParameterImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgParameterImp();

public slots:
  virtual void onParameterSetChange(const QString& rcString);

protected slots:
  virtual void languageChange();
  virtual void onGroupSelected(QListViewItem *);

protected:
  ParameterGroupItem *_pcMainLabel;
  QListView* ParamGrp;
  QListView* ParamVal;
};

// --------------------------------------------------------------------

/**
 * The ParameterGroup class represents the tree structure of the parameters.
 * The leaves represented by ParameterValueItem are displayed in ParameterValue.
 * @author Werner Mayer
 */
class ParameterGroup : public QListView
{
  Q_OBJECT

public:
  ParameterGroup( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  virtual ~ParameterGroup();

  /** Makes sure that always exactly one item is selected. */
  void setSelected ( QListViewItem * item, bool selected );

protected:
  /** Shows the context menu. */
  void contentsContextMenuEvent ( QContextMenuEvent* event );
  /** Triggers the "Del" key. */
  void keyPressEvent (QKeyEvent* event);

protected slots:
  /** Removes the underlying parameter group and its sub-groups from the
   * parameter tree structure.
   */
  void onDeleteSelectedItem();
  /** Creates a sub-group to the current selected parameter group. */
  void onCreateSubgroup();
  /** Expands or closes the selected item. If it is open it will be closed and the
   * other way round.
   */
  void onToggleSelectedItem();
  /** Exports the current selected parameter with all sub-groups to a file. */
  void onExportToFile();
  /** Imports a file and inserts the parameter to the current selected parameter node. */
  void onImportFromFile();
  /** Changes the name of the leaf of the selected item. */
  void onRenameSelectedItem();

private:
  QPopupMenu* menuEdit;
  CustomMenuItem* custom;
  int _expandId, _importId;
};

// --------------------------------------------------------------------

/**
 * The ParameterValue class displays all leaves of a parameter group. A leaf is represented
 * by the ParameterValueItem class.
 * @author Werner Mayer
 */
class ParameterValue : public QListView
{
  Q_OBJECT

public:
  ParameterValue( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  virtual ~ParameterValue();
  
  /** Removes the underlying leaf from the parameter group and removes
   * also \i from the list view.
   */
  void takeItem ( QListViewItem * i );

  /** Sets the current parameter group that is displayed. */
  void setCurrentGroup( const FCHandle<ParameterGrp>& _hcGrp );

protected:
  /** Shows the context menu. */
  void contentsContextMenuEvent ( QContextMenuEvent* event );
  /** Invokes onDeleteSelectedItem() if the "Del" key was pressed. */
  void keyPressEvent (QKeyEvent* event);

protected slots:
  /** Changes the value of the leaf of the selected item. */
  void onChangeSelectedItem();
  /** Remove the underlying leaf from the parameter group. The selected item is also
   * removed and destroyed.
   */
  void onDeleteSelectedItem();
  /** Changes the name of the leaf of the selected item. */
  void onRenameSelectedItem();
  /** Creates and appends a new "text" leaf. */
  void onCreateTextItem();
  /** Creates and appends a new "integer" leaf. */
  void onCreateIntItem();
  /** Creates and appends a new "float" leaf. */
  void onCreateFloatItem();
  /** Creates and appends a new "boolean" leaf. */
  void onCreateBoolItem();

private:
  QPopupMenu* menuEdit;
  QPopupMenu* menuNew;
  FCHandle<ParameterGrp> _hcGrp;
};

/** The link between the Tree and the shown Label.
 * Every (shown) Label in the FCDocument class get it 
 * associated FCTreeLabel which controls the visibility 
 * and the functions of the Label.
 *
 * \author Jürgen Riegel
 */
class ParameterGroupItem : public QListViewItem
{
public:
  /// Constructor
  ParameterGroupItem( ParameterGroupItem * parent, const FCHandle<ParameterGrp> &hcGrp );
  ParameterGroupItem( QListView* parent, const FCHandle<ParameterGrp> &hcGrp);
  ~ParameterGroupItem();

  void setSelected ( bool o );
  int rtti () const { return 2000; }
  void takeItem ( QListViewItem * item );
  void startRename ( int col );

  void fillUp(void);
  FCHandle<ParameterGrp> _hcGrp;

protected:
  /** If the name of the item has changed replace() is invoked. */
  virtual void okRename ( int col );
};

// --------------------------------------------------------------------

/**
 * The ParameterValueItem class represents items that are added to the ParameterValue
 * listview. Each item represents a leaf in a parameter group and allows interaction
 * with this leaf, such as modifying its name, its value or even remove it from the 
 * parameter group.
 * @author Werner Mayer
 */
class ParameterValueItem : public QListViewItem
{
public:
  /// Constructor
  ParameterValueItem ( QListView * parent, QString label1, const FCHandle<ParameterGrp> &hcGrp);
  virtual ~ParameterValueItem();

  /** This is the class Id to distinguish from QListViewItem itself or 
   * from other QListViewItem-subclasses. 
   */
  int rtti () const { return 3000; }
  /** Opens an input dialog to change the value. */
  virtual void changeValue() = 0;
  /** Append this item as leaf to the parameter group. */
  virtual void appendToGroup() = 0;
  /** Remove the leaf from the parameter group. */
  virtual void removeFromGroup() = 0;
  void startRename ( int col );

protected:
  /** Replaces the name of the leaf from \a oldName to \a newName. */
  virtual void replace( const QString& oldName, const QString& newName ) = 0;
  /** If the name of the item has changed replace() is invoked. */
  virtual void okRename ( int col );

protected:
  FCHandle<ParameterGrp> _hcGrp;
};

/**
 * The ParameterText class allows interaction with "text" parameter leaves.
 * @author Werner Mayer
 */
class ParameterText : public ParameterValueItem
{
public:
  /// Constructor
  ParameterText ( QListView * parent, QString label1, QString value, const FCHandle<ParameterGrp> &hcGrp);
  ~ParameterText();

  void changeValue();
  void appendToGroup();
  void removeFromGroup();

protected:
  void replace( const QString& oldName, const QString& newName );
};

/**
 * The ParameterInt class allows interaction with "integer" parameter leaves.
 * @author Werner Mayer
 */
class ParameterInt : public ParameterValueItem
{
public:
  /// Constructor
  ParameterInt ( QListView * parent, QString label1, long value, const FCHandle<ParameterGrp> &hcGrp);
  ~ParameterInt();

  void changeValue();
  void appendToGroup();
  void removeFromGroup();

protected:
  void replace( const QString& oldName, const QString& newName );
};

/**
 * The ParameterFloat class allows interaction with "float" parameter leaves.
 * @author Werner Mayer
 */
class ParameterFloat : public ParameterValueItem
{
public:
  /// Constructor
  ParameterFloat ( QListView * parent, QString label1, double value, const FCHandle<ParameterGrp> &hcGrp);
  ~ParameterFloat();

  void changeValue();
  void appendToGroup();
  void removeFromGroup();

protected:
  void replace( const QString& oldName, const QString& newName );
};

/**
 * The ParameterBool class allows interaction with "boolean" parameter leaves.
 * @author Werner Mayer
 */
class ParameterBool : public ParameterValueItem
{
public:
  /// Constructor
  ParameterBool ( QListView * parent, QString label1, bool value, const FCHandle<ParameterGrp> &hcGrp);
  ~ParameterBool();

  void changeValue();
  void appendToGroup();
  void removeFromGroup();

protected:
  void replace( const QString& oldName, const QString& newName );
};

// --------------------------------------------------------------------

/**
 * Custom menu item to display bold items in a popup menu.
 * @author Werner Mayer
 */
class CustomMenuItem : public QCustomMenuItem
{
public:
  CustomMenuItem( const QString& s, const QFont& f );
  ~CustomMenuItem();

  void paint( QPainter* p, const QColorGroup&, bool, bool, int x, int y, int w, int h );
  QSize sizeHint();
  void setText( const QString& text );

private:
  QString string;
  QFont font;
};

} // namespace Dialog
} // namespace Gui

#endif // DlgParameter_H
