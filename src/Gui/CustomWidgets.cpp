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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qlayout.h>
# include <qmenubar.h>
# include <qobjectlist.h>
#endif

#include "CustomWidgets.h"
#include "Action.h"
#include "Application.h"
#include "ToolBoxBar.h"
#include "ToolBox.h"
#include "Command.h"
#include "HtmlView.h"
#include "../Base/Console.h"

using Base::Console;
using namespace Gui;
using namespace Gui::DockWnd;

/**
 * Creates a custom widget with the group \a grp and its subgroup \a name.
 */
CustomWidget::CustomWidget(const char* grp, const char * name)
    : PrefWidget(name, false), _bCanModify(true), _bCanRemovable(true)
{
  init(grp, name);
}

CustomWidget::~CustomWidget()
{
}

/**
 * Restores the settings of this widget.
 */
void CustomWidget::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  if (hPrefGrp->HasGroup("Items"))
  {
    if (!hPrefGrp->GetGroup("Items")->IsEmpty())
    {
      _clItems.clear();
      std::vector<std::string> items = hPrefGrp->GetGroup("Items")->GetASCIIs();
      for ( std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it )
        _clItems << it->c_str(); 
    }
  }
}

/**
 * Saves the settings of this widget.
 */
void CustomWidget::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  int i=0;
  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup("Items");
  hPGrp->Clear();
  for ( QStringList::Iterator it = _clItems.begin(); it != _clItems.end(); ++it, i++ )
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", entryName().latin1(), i);
    hPGrp->SetASCII(szBuf, (*it).latin1() );
  }
}

void CustomWidget::init(const char* grp, const char* name)
{
  _clWorkbench = ApplicationWindow::Instance->GetActiveWorkbench();
  setPrefName(_clWorkbench);
  hPrefGrp = getRootParamGrp()->GetGroup("Workbenches")->GetGroup(_clWorkbench.latin1());
  hPrefGrp = hPrefGrp->GetGroup(grp);
  hPrefGrp = hPrefGrp->GetGroup(name);
  hPrefGrp->Attach(this);
}

/** 
 * Returns true if items are set, otherwise returns false. 
 */
bool CustomWidget::hasCustomItems()
{
  return _clItems.size() > 0;
}

/** Returns the active workbench at creation time of this object */
QString CustomWidget::getWorkbench()
{
  return _clWorkbench;
}

/**
 * Returns a list of set custom items.
 */
const QStringList& CustomWidget::getCustomItems() const
{
  return _clItems;
}

/**
 * Sets the items \a items to current items.
 */
void CustomWidget::setCustomItems( const QStringList& items )
{
  _clItems = items;
}

/** 
 * Appends the items \a items to the parameter group \a grp..
 */
void CustomWidget::addCustomItems( FCParameterGrp::handle hGrp, const QStringList& items )
{
  if (hGrp.IsNull()) return;

  if (hPrefGrp == hGrp)
  {
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
      _clItems.push_back( *it );
  }
  else
  {
    if ( _clWbItems.find( hGrp ) == _clWbItems.end() )
    {
      hGrp->Attach(this);
      _clWbItems[ hGrp ] = items;
    }
  }
}

/** 
 * Removes the items \a items from the parameter group \a grp..
 */
void CustomWidget::removeCustomItems( FCParameterGrp::handle hGrp, const QStringList& items )
{
  if (hGrp.IsNull()) return;

  if (hPrefGrp == hGrp)
  {
    unsigned long startPos=0;
    for ( QStringList::ConstIterator it = items.begin(); it != items.end(); ++it )
    {
      if ( _clItems.size() > items.size() )
        startPos = _clItems.size() - items.size();
      QStringList::Iterator pos = _clItems.begin();
      pos += startPos;
      pos = _clItems.find( pos, *it );
      if (pos != _clItems.end())
        _clItems.erase( pos );
    }
  }
  else
  {
    WorkbenchItems::Iterator it = _clWbItems.find( hGrp );
    if ( it != _clWbItems.end() )
    {
      hGrp->Detach(this);
      it.data().clear();
      _clWbItems.erase( it );
    }
  }
}

/** 
 * Sets the 'removable' property to b
 * After switching to a new workbench the customizable
 * widgets of the old workbench (toolbars, command bars and menus 
 * (currently disabled)) normally will be deleted. To avoid this
 * you can set this property to 'false'. The default is 'true'.
 */
void CustomWidget::setRemovable(bool b)
{
  _bCanRemovable = b;
}

/** 
 * Returns whether the widget can be modified or not.
 */
bool CustomWidget::isRemovable() const
{
  return _bCanRemovable;
}

/**
 * If \a b is true this widget can be modified, otherwise it cannot be modified.
 */
void CustomWidget::setCanModify( bool b )
{
  _bCanModify = b;
}

/**
 * Returns true  if this widget can be modified, otherwise returns false. 
 */
bool CustomWidget::canModify() const
{
  return _bCanModify;
}

/** Calls @ref restorePreferences()
 * So it is possible to call the load routine from outside.
 */
void CustomWidget::loadXML()
{
  restorePreferences();
}

/** Calls @ref savePreferences()
 * So it is possible to call the save routine from outside.
 */
void CustomWidget::saveXML()
{
  savePreferences();
}

// --------------------------------------------------------------------

/**
 * Constructs an empty horizontal toolbar of the type \a type with enabled drag'n'drop.
 *
 * The toolbar is called \a name and is a child of \a parent and is managed by \a mainWindow.
 * The \a label and \a newLine parameters are passed straight to QMainWindow::addDockWindow().
 * \a name and the widget flags \a f are passed on to the QDockWindow constructor.
 *
 * Use this constructor if you want to create torn-off (undocked, floating) toolbars or toolbars 
 * in the status bar.
 */
CustomToolBar::CustomToolBar ( const QString & label, QMainWindow *mainWindow, QWidget *parent, 
                               bool newLine, const char * name, WFlags f, const char* type )
  : QToolBar(label, mainWindow, parent, newLine, name, f), CustomWidget(type, name)
{
  // allow drag and drop
  setAcceptDrops(true);
  bSaveColor = false;
}

/**
 * Constructs an empty toolbar called \a name, of the type \a type, with parent \a parent, in its 
 * parent's top dock area, without any label and without requiring a newline.
 */
CustomToolBar::CustomToolBar ( QMainWindow * parent, const char * name, const char* type )
    : QToolBar(parent, name), CustomWidget(type, name)
{
  // allow drag and drop
  setAcceptDrops(true);
  bSaveColor = false;
}

CustomToolBar::~CustomToolBar()
{
}

/**
 * If this toolbar can be modified it is rebuild using the command manager \a rclMgr, 
 * otherwise nothing happens.
 */
void CustomToolBar::update( FCCommandManager& rclMgr )
{
  if (!canModify())
    return; // no need to read again

  clearUp();
  for ( QStringList::Iterator it = _clItems.begin(); it != _clItems.end(); ++it )
  {
    if (*it == "Separator")
      addSeparator();
    else
      rclMgr.AddTo( (*it).latin1(), this );
  }

  // get also the commands from other workbenches
  QStringList items;
  for (WorkbenchItems::Iterator it2 = _clWbItems.begin(); it2 != _clWbItems.end(); ++it2)
  {
    items = it2.data();
    for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it )
    {
      if ( *it == "Separator" )
        addSeparator();
      else
        rclMgr.AddTo( (*it).latin1(), this );
    }
  }
}

void CustomToolBar::setCanModify(bool b)
{
  CustomWidget::setCanModify( b );
  setAcceptDrops(b);
}

/**
 * Returns true if the widget type \a w can be added to a toolbar, otherwise returns false.
 */
bool CustomToolBar::isAllowed( QWidget* w )
{
  if (!w)
    return false; // no valid widget

  // this is extensible
  //
  if (w->inherits("QToolButton") )
    return true;
  else if (w->inherits("QComboBox"))
    return true;
  else if (w->inherits("QSpinBox"))
    return true;
  else if (w->inherits("QLineEdit"))
    return true;
  else if (w->inherits("QToolBarSeparator"))
    return true;
  else if (w->inherits("QHBox")) // drop down buttons
    return true;

  // all other types of widget are not allowed
  return false;
}

/**
 * Clear the toolbar's content.
 */
void CustomToolBar::clearUp()
{
  clear();
}

void CustomToolBar::dropEvent ( QDropEvent * e)
{
  QPtrList<QWidget> childs;
  if ( children() )
  {
    QObjectListIt it( *children() );
    QObject * obj;
    while( (obj=it.current()) != 0 )
    {
      ++it;
      if ( obj->isWidgetType() )
      {
        // check if widget type is OK
        if (isAllowed((QWidget*)obj))
          childs.append((QWidget*)obj);
      }
    }
  }

  // check if the number of found (filtered) widgets
  // is the same as the number of items
  //
  // different sizes ->just append at the end
  if ( childs.count() != _clItems.size() )
  {
    /*    // create a new button
        //
        FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
        FCCommand* pCom = NULL;
        std::vector<QString> actions = ActionDrag::actions;
        for (std::vector<QString>::iterator it = actions.begin(); it != actions.end(); ++it)
        {
          pCom = cCmdMgr.GetCommandByName(it->latin1());
          if (pCom != NULL)
          {
            if (pCom->addTo(this))
              _clItems.push_back(it->latin1());
          }
        }
    */
    Console().Log("Cannot drop item(s). The size of found items"
                  " is different to the size of stored items\n");
    ActionDrag::actions.clear();

    return;
  }

  // assume 0: horizontal
  //        1: vertical
  Qt::Orientation orient;

#if QT_VERSION <= 230
  orient = orientation();
#else
  // Note: orientation() seems to work not properly in Qt 3.x
  QBoxLayout* layout = boxLayout ();
  if (!layout)
    return;

  if (layout->direction() == QBoxLayout::Up ||
      layout->direction() == QBoxLayout::Down)
  {
    // this is vertical
    orient = Qt::Vertical;
  }
  else
  {
    orient = Qt::Horizontal;
  }
#endif

  // find right position for the new widget(s)
  QStringList items;
  QStringList::Iterator it2 = _clItems.begin();
  QPoint pt = e->pos();
  int pos=0;
  QWidget* it;
  for ( it = childs.first(); it; it = childs.next(), ++it2)
  {
    if (orient == Qt::Horizontal)
    {
      pos += it->width();
      if (pos >= pt.x())
        break;
      else
        items.push_back(*it2);
    }
    else
    {
      pos += it->height();
      if (pos >= pt.y())
        break;
      else
        items.push_back(*it2);
    }
  }

  // append the dropped items
  QStringList actions = ActionDrag::actions;
  for (QStringList::Iterator it3 = actions.begin(); it3 != actions.end(); ++it3)
  {
    items.push_back( *it3 );
  }

  ActionDrag::actions.clear();

  // and now append the rest of the old items
  for (;it2 != _clItems.end(); ++it2)
    items.push_back(*it2);

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  _clItems = items;

  // clear all and rebuild it again
  //
  update(cCmdMgr);
}

void CustomToolBar::dragEnterEvent ( QDragEnterEvent * e)
{
  e->accept( ActionDrag::canDecode(e) );
}

void CustomToolBar::dragLeaveEvent ( QDragLeaveEvent * )
{
}

void CustomToolBar::dragMoveEvent ( QDragMoveEvent * )
{
}

void CustomToolBar::restorePreferences()
{
  CustomWidget::restorePreferences();

  if (bSaveColor)
  {
    long def = (200 << 16) | (208 << 8) | 212;
    long col = hPrefGrp->GetInt("Color", def);

    int b = col >> 16;  col -= b << 16;
    int g = col >> 8;   col -= g << 8;
    int r = col;

    QColor color(r, g, b);
    if (color.isValid())
    {
      //      setPalette(QPalette(color, color));
      //      setBackgroundMode(PaletteBackground);
    }
  }

  if (hPrefGrp->GetBool("visible", true) == false)
    hide();
}

void CustomToolBar::savePreferences()
{
  CustomWidget::savePreferences();

  if (bSaveColor)
  {
    QColor col = backgroundColor();
    long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();
    hPrefGrp->SetInt("Color", lcol);
  }

  hPrefGrp->SetBool("visible",  !isHidden());
}

// --------------------------------------------------------------------

/**
 * Constructs a popup menu called \a name with parent \a parent and the parameter group 
 * \a menu to store its content to.
 */
CustomPopupMenu::CustomPopupMenu(QWidget * parent, const char * name, const char* menu )
  : QPopupMenu(parent, name), CustomWidget("Menus", name), _bAllowDrag(false)
{
  _hCommonGrp = GetApplication().GetParameterGroupByPath("System parameter:BaseApp/WindowSettings");
  _hCommonGrp->Attach(this);
  // allow drag and drop
  //  setAcceptDrops(true);
  if (menu)
    this->_parent = menu;
}

CustomPopupMenu::~CustomPopupMenu()
{
  _hCommonGrp->Detach(this);
}

void CustomPopupMenu::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  if (strcmp(sReason, "AllowDrag") == 0)
  {
    FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
    _bAllowDrag = rclGrp.GetBool("AllowDrag", false);
  }
}

/**
 * If this menu can be modified it is rebuild using the command manager \a rclMgr, 
 * otherwise nothing happens.
 */
void CustomPopupMenu::update( FCCommandManager& rclMgr )
{
  if (!canModify())
    return; // no need to read again

  clear();
  for ( QStringList::Iterator it = _clItems.begin(); it != _clItems.end(); ++it )
  {
    if ( *it == "Separator" )
      insertSeparator();
    else
      rclMgr.AddTo( (*it).latin1(), this );
  }

  // get also the commands from other workbenches
  QStringList items;
  for ( WorkbenchItems::Iterator it2 = _clWbItems.begin(); it2 != _clWbItems.end(); ++it2 )
  {
    items = it2.data();
    for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it )
    {
      if ( *it == "Separator" )
        insertSeparator();
      else
        rclMgr.AddTo( (*it).latin1(), this );
    }
  }
}

void CustomPopupMenu::dropEvent ( QDropEvent * e)
{
  // create a new button
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCom = NULL;

  QStringList actions = ActionDrag::actions;
  for ( QStringList::Iterator it = actions.begin(); it != actions.end(); ++it )
  {
    pCom = cCmdMgr.GetCommandByName( (*it).latin1() );
    if (pCom != NULL)
    {
      if (pCom->addTo(this))
        _clItems.push_back( *it );
    }
  }

  ActionDrag::actions.clear();
}

void CustomPopupMenu::dragEnterEvent ( QDragEnterEvent * e)
{
  e->accept(/*ActionDrag::canDecode(e)*/false);
}

void CustomPopupMenu::dragLeaveEvent ( QDragLeaveEvent * )
{
}

void CustomPopupMenu::dragMoveEvent ( QDragMoveEvent * )
{
}

void CustomPopupMenu::mouseMoveEvent ( QMouseEvent * e)
{
  if ( e->state() == LeftButton && _bAllowDrag)
  {
    // try to find out the correct menu item
    // in most cases this should work correctly
    //
    //
    int id = idAt(e->pos());
    if (id == -1)
      return; // out of range

    ActionDrag::actions.clear();

    // get pixmap and text of this item
    QPixmap* pix = pixmap(id);
    QString txt = text(id);

    // find the corresponding command to this item
    const std::map<std::string, FCCommand*>& rclCmds = ApplicationWindow::Instance->GetCommandManager().GetCommands();

    // search item with same text first
    for (std::map<std::string, FCCommand*>::const_iterator it = rclCmds.begin(); it != rclCmds.end(); ++it)
    {
      QAction* a = it->second->GetAction();
      if (a != NULL)
      {
        if ( a->menuText() == txt )
        {
          ActionDrag *ad = new ActionDrag( it->second->GetName(), this );

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // if nothing found search item with similar text
#ifdef FC_OS_LINUX
    for (std::map<std::string, FCCommand*>::const_iterator it = rclCmds.begin(); it != rclCmds.end(); ++it)
#else
    for (it = rclCmds.begin(); it != rclCmds.end(); ++it)
#endif
    {
      QAction* a = it->second->GetAction();
      if (a != NULL)
      {
        // check if menu item starts with txt
        // both strings need not to be equal (because of accelarators)
        if ( txt.startsWith(a->menuText()) )
        {
          ActionDrag *ad = new ActionDrag( it->second->GetName(), this );

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // error
    Console().Warning("No corresponding Action object found\n");
  }
  else
    QPopupMenu::mouseMoveEvent(e);
}

void CustomPopupMenu::mouseReleaseEvent( QMouseEvent * e )
{
  if (FCWhatsThis::inWhatsThisMode())
  {
    int id = idAt(mapFromGlobal(QCursor::pos()));
    close();
    FCWhatsThis::leaveWhatsThisMode(whatsThis(id));
  }
  else
    QPopupMenu::mouseReleaseEvent(e);
}

void CustomPopupMenu::restorePreferences()
{
  CustomWidget::restorePreferences();
  _bAllowDrag = _hCommonGrp->GetBool("AllowDrag", false);
}

void CustomPopupMenu::savePreferences()
{
  CustomWidget::savePreferences();
  if (! _parent.isEmpty())
    hPrefGrp->SetASCII("Parent Menu", _parent.latin1());
  hPrefGrp->SetBool("AllowDrag", _bAllowDrag);
}

// --------------------------------------------------------------------

namespace Gui {
struct CustomWidgetManagerP
{
  CustomWidgetManagerP(FCCommandManager& rclMgr, ToolBox* pStackBar)
      : _clCmdMgr(rclMgr), _pclStackBar(pStackBar)
  {
  }

  int                                  _iSeparator;
  std::map <QString,CustomPopupMenu*>  _clPopupMenus;
  std::map <CustomPopupMenu*,int>          _clPopupID;
  std::map <QString,CustomToolBar*>    _clToolbars;
  std::map <QString,CustomToolBar*>    _clCmdbars;
  FCCommandManager&                    _clCmdMgr;
  ToolBox*                           _pclStackBar;
};
} // namespace Gui

CustomWidgetManager::CustomWidgetManager(FCCommandManager& rclMgr, ToolBox* pStackBar)
{
  d = new CustomWidgetManagerP(rclMgr, pStackBar);
}

CustomWidgetManager::~CustomWidgetManager()
{
  for (std::map <QString,CustomPopupMenu*>::iterator it1 = d->_clPopupMenus.begin(); it1 != d->_clPopupMenus.end(); ++it1)
    it1->second->saveXML();
  for (std::map <QString,CustomToolBar*>::iterator it2 = d->_clToolbars.begin(); it2 != d->_clToolbars.end(); ++it2)
    it2->second->saveXML();
  for (std::map <QString,CustomToolBar*>::iterator it3 = d->_clCmdbars.begin(); it3 != d->_clCmdbars.end(); ++it3)
    it3->second->saveXML();

  delete d;d=NULL;
}

/** 
 * Loads the custom widgets depending on the given
 * workbench from the preferences. 
 */
bool CustomWidgetManager::loadCustomWidegts( const QString& workbench )
{
  QStringList dummy;
  bool bFound = false;
  FCParameterGrp::handle hGrp = PrefWidget::getRootParamGrp();
  hGrp = hGrp->GetGroup("Workbenches")->GetGroup( workbench.latin1() );

  std::vector<FCParameterGrp::handle> hSubGrps;
  std::vector<FCParameterGrp::handle>::iterator it;
  FCParameterGrp::handle hCmdGrp  = hGrp->GetGroup("Cmdbar");
  FCParameterGrp::handle hToolGrp = hGrp->GetGroup("Toolbars");
  FCParameterGrp::handle hMenuGrp = hGrp->GetGroup("Menus");

  hSubGrps = hCmdGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    addCommandBar( (*it)->GetGroupName(), dummy );
  }
  if (d->_clCmdbars.size() > 0)
  {
    if (d->_pclStackBar->currentItem() == NULL)
      d->_pclStackBar->setCurrentItem(d->_clCmdbars.begin()->second);
  }

  hSubGrps = hToolGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    addToolBar( (*it)->GetGroupName(), dummy );
  }

  hSubGrps = hMenuGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    if ((*it)->GetASCII("Parent Menu", "nix") != "nix")
      addPopupMenu( (*it)->GetGroupName(), dummy, (*it)->GetASCII("Parent Menu").c_str() );
    else
      addPopupMenu( (*it)->GetGroupName(), dummy );
  }

  return bFound;
}

/** 
 * Updates the custom widgets depending on the given workbench.
 */
bool CustomWidgetManager::update( const QString& workbench )
{
  QString wb = ApplicationWindow::Instance->GetActiveWorkbench();
  std::map <QString,CustomPopupMenu*> clPopupMenus = d->_clPopupMenus;
  for (std::map <QString,CustomPopupMenu*>::iterator it1 = clPopupMenus.begin(); it1 != clPopupMenus.end(); ++it1)
  {
    if (wb != it1->second->getWorkbench())
      removePopupMenu( it1->first );
  }
  std::map <QString,CustomToolBar*> clToolbars = d->_clToolbars;
  for (std::map <QString,CustomToolBar*>::iterator it2 = clToolbars.begin(); it2 != clToolbars.end(); ++it2)
  {
    if (wb != it2->second->getWorkbench())
      removeToolBar( it2->first );
  }
  std::map <QString,CustomToolBar*> clCmdbars = d->_clCmdbars;
  for (std::map <QString,CustomToolBar*>::iterator it3 = clCmdbars.begin(); it3 != clCmdbars.end(); ++it3)
  {
    if (wb != it3->second->getWorkbench())
      removeCommandBar( it3->first );
  }

  return loadCustomWidegts( workbench );
}

/** 
 * Clears all custom widgets and reload it from preferences.
 */
bool CustomWidgetManager::update()
{
  // clear the content of all custom widgets and read it from preferences again
  //
  std::map <QString,CustomPopupMenu*> clPopupMenus = d->_clPopupMenus;
  for (std::map <QString,CustomPopupMenu*>::iterator it1 = clPopupMenus.begin(); it1 != clPopupMenus.end(); ++it1)
  {
    it1->second->loadXML();
    it1->second->update(d->_clCmdMgr);
  }
  std::map <QString,CustomToolBar*> clToolbars = d->_clToolbars;
  for (std::map <QString,CustomToolBar*>::iterator it2 = clToolbars.begin(); it2 != clToolbars.end(); ++it2)
  {
    it2->second->loadXML();
    it2->second->update(d->_clCmdMgr);
  }
  std::map <QString,CustomToolBar*> clCmdbars = d->_clCmdbars;
  for (std::map <QString,CustomToolBar*>::iterator it3 = clCmdbars.begin(); it3 != clCmdbars.end(); ++it3)
  {
    it3->second->loadXML();
    it3->second->update(d->_clCmdMgr);
  }

  return true;
}

/** 
 * Adds new menu with its items and its parent.
 */
void CustomWidgetManager::addPopupMenu(const QString& type, const QStringList& defIt, const char* parent)
{
  CustomPopupMenu* popup = getPopupMenu( type, parent);

  // append if these items are from another workbench
  QString oldWb = popup->getWorkbench();
  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (oldWb != newWb)
  {
    FCParameterGrp::handle hPrefGrp;
    hPrefGrp = PrefWidget::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
    hPrefGrp = hPrefGrp->GetGroup("Menus");
    hPrefGrp = hPrefGrp->GetGroup(type.latin1());

    popup->addCustomItems(hPrefGrp, defIt);
    popup->update(d->_clCmdMgr);
    return;
  }

  popup->loadXML();
  if (!popup->hasCustomItems())
  {
    popup->setCustomItems(defIt);
  }

  popup->update(d->_clCmdMgr);
}

/** 
 * Returns the menu bar by name.
 * If it does not exist it will be created. If parent is not NULL
 * the menu with the name 'parent' is the father. Otherwise
 * the menu will be put in the application's menu bar.
 */
CustomPopupMenu* CustomWidgetManager::getPopupMenu( const QString& name, const char* parent )
{
  std::map <QString,CustomPopupMenu*>::iterator It = d->_clPopupMenus.find( name );
  if ( It!=d->_clPopupMenus.end() )
  {
    return It->second;
  }
  else if (parent == 0 || strcmp(parent,"") == 0)
  {
    CustomPopupMenu *pcPopup = new CustomPopupMenu( ApplicationWindow::Instance, name );
    d->_clPopupMenus[ name ] = pcPopup;

    // if the help menu should be inserted
    if (strcmp(name, "Help") == 0)
    {
      d->_iSeparator = ApplicationWindow::Instance->menuBar()->insertSeparator();
    }

    int id = ApplicationWindow::Instance->menuBar()->insertItem( QObject::tr(name), pcPopup );
    d->_clPopupID[pcPopup] = id;

    // search for the "Help" menu if inside
    if (strcmp(name, "Help") != 0)
    {
      It = d->_clPopupMenus.find("Help");
      // not found
      if ( It!=d->_clPopupMenus.end())
      {
        // get old id, remove it from the menubar, insert it immediately and
        // set the new id
        ApplicationWindow::Instance->menuBar()->removeItem(d->_iSeparator);
        d->_iSeparator = ApplicationWindow::Instance->menuBar()->insertSeparator();
        id = d->_clPopupID[It->second];
        ApplicationWindow::Instance->menuBar()->removeItem(id);
        id = ApplicationWindow::Instance->menuBar()->insertItem( QObject::tr("Help"), It->second );
        d->_clPopupID[It->second] = id;
      }
    }

    return pcPopup;
  }
  else
  {
    CustomPopupMenu *pParent = getPopupMenu(parent);
    CustomPopupMenu *pcPopup = new CustomPopupMenu( pParent, name, parent );
    d->_clPopupMenus[name] = pcPopup;
    int id = pParent->insertItem(QObject::tr(name), pcPopup);
    d->_clPopupID[pcPopup] = id;
    return pcPopup;
  }
}

/** 
 * Returns a vector of all menus.
 */
QPtrList<CustomPopupMenu> CustomWidgetManager::getPopupMenus()
{
  QPtrList<CustomPopupMenu> aclMenus;
  for (std::map <QString,CustomPopupMenu*>::iterator It = d->_clPopupMenus.begin(); It != d->_clPopupMenus.end(); ++It)
  {
    aclMenus.append( It->second );
  }

  return aclMenus;
}

/** 
 * Deletes the specified menu if it exists.
 */
void CustomWidgetManager::removePopupMenu( const QString& name )
{
  std::map <QString,CustomPopupMenu*>::iterator It = d->_clPopupMenus.find( name );
  if( It!=d->_clPopupMenus.end() )
  {
    if (!It->second->isRemovable())
      return; // cannot be removed

    It->second->saveXML();
    if (strcmp(It->second->parentWidget()->className(), ApplicationWindow::Instance->className()) == 0)
    {
      // remove properly from menubar
      int id = d->_clPopupID[It->second];
      ApplicationWindow::Instance->menuBar()->removeItem(id);

#ifdef FC_DEBUG
      printf( "%s removed from menubar\n", name.latin1() );
#endif
    }
    else if (strcmp(It->second->parentWidget()->className(), It->second->className()) == 0)
    {
      // remove properly from parent menu
      CustomPopupMenu* pParent = (CustomPopupMenu*)It->second->parentWidget();
      int id = d->_clPopupID[It->second];
      pParent->removeItem(id);

#ifdef FC_DEBUG
      printf( "%s removed from parent menu\n", name.latin1() );
#endif
    }

    delete It->second;
    d->_clPopupMenus.erase(It);
  }
}

/** 
 * Removes menu items.
 */
void CustomWidgetManager::removeMenuItems(const QString& type, const QStringList& items)
{
  CustomPopupMenu* popup = getPopupMenu( type );

  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  FCParameterGrp::handle hPrefGrp;
  hPrefGrp = PrefWidget::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
  hPrefGrp = hPrefGrp->GetGroup("Menus");
  hPrefGrp = hPrefGrp->GetGroup( type.latin1() );

  popup->CustomWidget::removeCustomItems( hPrefGrp, items );
  popup->update(d->_clCmdMgr);
}

/**
 * Get the number of menus.
 */
int CustomWidgetManager::countPopupMenus()
{
  return int(d->_clPopupMenus.size());
}

/** 
 * Adds new toolbar with its items
 * After adding the new toolbar it searches for its items in 
 * the preferences. If it has found any items it uses these instead of 
 * the given in 'defIt'. If force is set to true the toolbar takes the
 * given items anyway.
 */
void CustomWidgetManager::addToolBar( const QString& type, const QStringList& defIt )
{
  CustomToolBar* toolbar = getToolBar( type );

  // append if these items are from another workbench
  QString oldWb = toolbar->getWorkbench();
  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (oldWb != newWb)
  {
    FCParameterGrp::handle hPrefGrp;
    hPrefGrp = PrefWidget::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
    hPrefGrp = hPrefGrp->GetGroup("Toolbars");
    hPrefGrp = hPrefGrp->GetGroup( type.latin1() );

    toolbar->addCustomItems(hPrefGrp, defIt);
    toolbar->update(d->_clCmdMgr);
    return;
  }

  toolbar->loadXML();
  if (!toolbar->hasCustomItems())
  {
    toolbar->setCustomItems(defIt);
  }

  toolbar->update(d->_clCmdMgr);
}

/** 
 * Returns the toolbar by name.
 * If it does not exist it will be created.
 */
CustomToolBar* CustomWidgetManager::getToolBar( const QString& name )
{
  std::map <QString,CustomToolBar*>::iterator It = d->_clToolbars.find( name );
  if( It!=d->_clToolbars.end() )
    return It->second;
  else
  {
//    CustomToolBar *pcToolBar = new CustomToolBar( ApplicationWindow::Instance, name );
    CustomToolBar *pcToolBar = new CustomToolBar( name, ApplicationWindow::Instance, 
                                          ApplicationWindow::Instance, false, name.latin1() );
    ApplicationWindow::Instance->addToolBar(pcToolBar);
    d->_clToolbars[ name ] = pcToolBar;
    pcToolBar->show();
    return pcToolBar;
  }
}

/** 
 * Returns a vector of all toolbars.
 */
QPtrList<CustomToolBar> CustomWidgetManager::getToolBars()
{
  QPtrList<CustomToolBar> aclToolbars;
  for (std::map <QString,CustomToolBar*>::iterator It = d->_clToolbars.begin(); It != d->_clToolbars.end(); ++It)
  {
    aclToolbars.append(It->second);
  }

  return aclToolbars;
}

/** 
 * Deletes the specified toolbar if it exists.
 */
void CustomWidgetManager::removeToolBar( const QString& name )
{
  std::map <QString,CustomToolBar*>::iterator It = d->_clToolbars.find( name );
  if( It!=d->_clToolbars.end() )
  {
    if (!It->second->isRemovable())
      return; // cannot be removed
    It->second->saveXML();
    ApplicationWindow::Instance->removeToolBar(It->second);
    delete It->second;
    d->_clToolbars.erase(It);
  }
}

/** 
 * Removes toolbar items.
 */
void CustomWidgetManager::removeToolBarItems( const QString& type, const QStringList& items )
{
  CustomToolBar* tb = getToolBar( type );

  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  FCParameterGrp::handle hPrefGrp;
  hPrefGrp = PrefWidget::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
  hPrefGrp = hPrefGrp->GetGroup("Toolbars");
  hPrefGrp = hPrefGrp->GetGroup( type.latin1() );

  tb->CustomWidget::removeCustomItems( hPrefGrp, items );
  tb->update(d->_clCmdMgr);
}

/** 
 * Get the number of toolbars.
 */
int CustomWidgetManager::countToolBars()
{
  return int(d->_clToolbars.size());
}

/** 
 * Adds new command bar with its items.
 * After adding the new command bar it searches for its items in 
 * the preferences. If it has found any items it uses these instead of 
 * the given in 'defIt'. If force is set to true the toolbar takes the
 * given items anyway.
 */
void CustomWidgetManager::addCommandBar(const QString& type, const QStringList& defIt)
{
  CustomToolBar* toolbar = getCommandBar( type );

  // append if these items are from another workbench
  QString oldWb = toolbar->getWorkbench();
  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (oldWb != newWb)
  {
    FCParameterGrp::handle hPrefGrp;
    hPrefGrp = PrefWidget::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
    hPrefGrp = hPrefGrp->GetGroup("Cmdbar");
    hPrefGrp = hPrefGrp->GetGroup(type.latin1());

    toolbar->addCustomItems(hPrefGrp, defIt);
    toolbar->update(d->_clCmdMgr);
    return;
  }

  toolbar->loadXML();
  if (!toolbar->hasCustomItems())
  {
    toolbar->setCustomItems(defIt);
  }

  toolbar->update(d->_clCmdMgr);
}

/** 
 * Returns the command bar by name.
 * If it does not exist it will be created.
 */
CustomToolBar* CustomWidgetManager::getCommandBar( const QString& name )
{
  std::map <QString,CustomToolBar*>::iterator It = d->_clCmdbars.find( name );
  if( It!=d->_clCmdbars.end() )
    return It->second;
  else
  {
    CustomToolBar *pcToolBar = new ToolBoxBar( name, d->_pclStackBar, name );
    d->_clCmdbars[ name ] = pcToolBar;
    d->_pclStackBar->addItem(pcToolBar, name);
    return pcToolBar;
  }
}

/** 
 * Returns a vector of all command bars.
 */
QPtrList<CustomToolBar> CustomWidgetManager::getCommdandBars()
{
  QPtrList<CustomToolBar> aclCmdbars;
  for (std::map <QString,CustomToolBar*>::iterator It = d->_clCmdbars.begin(); It != d->_clCmdbars.end(); ++It)
  {
    aclCmdbars.append(It->second);
  }

  return aclCmdbars;
}

/** 
 * Deletes the specified command bar if it exists.
 */
void CustomWidgetManager::removeCommandBar( const QString& name )
{
  std::map <QString,CustomToolBar*>::iterator It = d->_clCmdbars.find( name );
  if( It!=d->_clCmdbars.end() )
  {
    if (!It->second->isRemovable())
      return; // cannot be removed
    It->second->saveXML();
    d->_pclStackBar->removeItem(It->second);
    d->_clCmdbars.erase(It);
  }
}

/** 
 * Removes command bar items.
 */
void CustomWidgetManager::removeCommandBarItems(const QString& type, const QStringList& items)
{
  CustomToolBar* tb = getCommandBar( type );

  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  FCParameterGrp::handle hPrefGrp;
  hPrefGrp = PrefWidget::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
  hPrefGrp = hPrefGrp->GetGroup("Cmdbar");
  hPrefGrp = hPrefGrp->GetGroup(type.latin1());

  tb->CustomWidget::removeCustomItems(hPrefGrp,items);
  tb->update(d->_clCmdMgr);
}

/** 
 * Get the number of command bars.
 */
int CustomWidgetManager::countCommandBars()
{
  return int(d->_clCmdbars.size());
}

void CustomWidgetManager::show()
{
  d->_pclStackBar->show();
}

void CustomWidgetManager::hide()
{
  d->_pclStackBar->hide();
}

#include "moc_CustomWidgets.cpp"
