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
#include "WhatsThis.h"
#include "../Base/Console.h"

using Base::Console;
using namespace Gui;
using namespace Gui::DockWnd;

/**
 * Creates a custom widget that by default can be modified.
 */
CustomWidget::CustomWidget() : _bCanModify(true)
{
}

/**
 * Destroys the custom widget and detaches from preferences.
 */
CustomWidget::~CustomWidget()
{
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
  : QToolBar(label, mainWindow, parent, newLine, name, f)
{
  // allow drag and drop
//  setAcceptDrops(true);
}

/**
 * Constructs an empty toolbar called \a name, of the type \a type, with parent \a parent, in its
 * parent's top dock area, without any label and without requiring a newline.
 */
CustomToolBar::CustomToolBar ( QMainWindow * parent, const char * name, const char* type )
    : QToolBar(parent, name)
{
  // allow drag and drop
//  setAcceptDrops(true);
}

CustomToolBar::~CustomToolBar()
{
}

/**
 * If to a new language is switched this method gets called.
 */
void CustomToolBar::languageChange()
{
  setLabel( tr( name() ) );
}

/**
 * If \a b is \a true this toolbar can be modified by the user..
 */
void CustomToolBar::setCanModify(bool b)
{
  CustomWidget::setCanModify( b );
//  setAcceptDrops(b);
}

void CustomToolBar::dropEvent ( QDropEvent * e)
{
  // append the dropped items
  CommandManager& rMgr = Application::Instance->commandManager();
  if (!canModify())
    return; // no need to read again
  QStringList actions = ActionDrag::actions;
  for (QStringList::Iterator it = actions.begin(); it != actions.end(); ++it)
  {
    rMgr.addTo( (*it).latin1(), this );
  }

  ActionDrag::actions.clear();
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

// --------------------------------------------------------------------

/**
 * Constructs a popup menu called \a name with parent \a parent and the parameter group 
 * \a menu to store its content to.
 */
CustomPopupMenu::CustomPopupMenu(QWidget * parent, const char * name, const char* menu )
  : QPopupMenu(parent, name), _bAllowDrag(false)
{
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("General");
  hGrp->Attach(this);
  _bAllowDrag = hGrp->GetBool("AllowDrag", _bAllowDrag);
}

CustomPopupMenu::~CustomPopupMenu()
{
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("General");
  hGrp->Detach(this);
}

void CustomPopupMenu::OnChange(Base::Subject<const char*> &rCaller, const char * sReason)
{
  if (strcmp(sReason, "AllowDrag") == 0)
  {
    ParameterGrp& rclGrp = ((ParameterGrp&)rCaller);
    _bAllowDrag = rclGrp.GetBool("AllowDrag", false);
  }
}

void CustomPopupMenu::dropEvent ( QDropEvent * e)
{
  // create a new button
  CommandManager & cCmdMgr = Application::Instance->commandManager();
  Command* pCom = NULL;

  QStringList actions = ActionDrag::actions;
  for ( QStringList::Iterator it = actions.begin(); it != actions.end(); ++it )
  {
    pCom = cCmdMgr.getCommandByName( (*it).latin1() );
    if (pCom != NULL)
    {
      pCom->addTo(this);
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
    const std::map<std::string, Command*>& rclCmds = Application::Instance->commandManager().getCommands();

    // search item with same text first
    for (std::map<std::string, Command*>::const_iterator it = rclCmds.begin(); it != rclCmds.end(); ++it)
    {
      QAction* a = it->second->getAction();
      if (a != 0)
      {
        if ( a->menuText() == txt )
        {
          ActionDrag *ad = new ActionDrag( it->second->getName(), this );

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // if nothing found search item with similar text
#ifdef FC_OS_LINUX
    for (std::map<std::string, Command*>::const_iterator it = rclCmds.begin(); it != rclCmds.end(); ++it)
#else
    for (it = rclCmds.begin(); it != rclCmds.end(); ++it)
#endif
    {
      QAction* a = it->second->getAction();
      if (a != 0)
      {
        // check if menu item starts with txt
        // both strings need not to be equal (because of accelarators)
        if ( txt.startsWith(a->menuText()) )
        {
          ActionDrag *ad = new ActionDrag( it->second->getName(), this );

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // error
    Console().Log("No corresponding Action object found\n");
  }
  else
    QPopupMenu::mouseMoveEvent(e);
}

#include "moc_CustomWidgets.cpp"
