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
# include <qapplication.h>
# include <qlabel.h>
# include <qmainwindow.h>
# include <qobjectlist.h>
# include <qtoolbutton.h>
#endif

#include "ToolBoxBar.h"

using namespace Gui;
using namespace Gui::DockWnd;

/**
 * Constructs an empty vertical toolbar.
 */
CommandBar::CommandBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: CustomToolBar(label, /*getMainWindow()*/0, w, false, name, f, "Commandbars")
{
  // remove immediately from main window again
//  getMainWindow()->removeToolBar(this);
  QWidget* widget = qApp->mainWidget();
  QMainWindow* mw = widget ? (QMainWindow*)widget->qt_cast("QMainWindow") : 0;
  if ( mw ) // avoid to show in dock menu
    mw->setAppropriate(this, false);
  setFrameStyle( QFrame::NoFrame );
  setOrientation( Qt::Vertical );
  setPalette(QPalette(Qt::white, Qt::white));
  setStretchableWidget( ( m_Dummy = new QWidget( this ) ) );
}

CommandBar::~CommandBar ()
{
  delete m_Dummy;
}

/**
 * Clears up the contents.
 */
void CommandBar::clear()
{
  CustomToolBar::clear();
  m_Dummy = NULL;
}

/**
 * Since this tool bar is in vertical mode there must be set
 * a stretchable dummy widget after the last created button.
 */
void CommandBar::setDummyToLastItem()
{
  delete m_Dummy;
  m_Dummy = new QWidget(this);
  setStretchableWidget( m_Dummy );
}

// --------------------------------------------------------------------

ToolBoxBar::ToolBoxBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: CommandBar(label, w, name, f)
{
}

ToolBoxBar::~ToolBoxBar ()
{
}

/**
 * Sets the icon of the last created tool button to left and the text \a text to the right side.
 */
void ToolBoxBar::setTextToLastItem( const QString& text )
{
  const QObjectList *l = children();
  QObjectListIt it(*l);
  QObject* o = it.toLast();
  if (o)
  {
    if (o->inherits("QToolButton"))
    {
      ((QToolButton*)o)->setTextPosition(QToolButton::BesideIcon);
      ((QToolButton*)o)->setTextLabel(text);
      ((QToolButton*)o)->setUsesTextLabel(true);
      ((QToolButton*)o)->setFixedHeight(30);
    }
  }

  setDummyToLastItem();
}

// --------------------------------------------------------------------

OutlookBar::OutlookBar ( const QString & label, QWidget *w, const char * name, WFlags f )
: CommandBar(label, w, name, f)
{
}

OutlookBar::~OutlookBar ()
{
}

/**
 * Creates a label below the last created button with the text \a text.
 */
void OutlookBar::setTextToLastItem( const QString& text )
{
  const QObjectList *l = children();
  QObjectListIt it(*l);
  QObject* o = it.toLast();
  if (o)
  {
    if (o->inherits("QToolButton"))
    {
      QLabel* label = new QLabel(this);
      label->setText(text);
      label->setAlignment(AlignHCenter);
      ((QWidget*)o)->setFixedSize(48,48);
    }
  }

  setDummyToLastItem();
}

#include "moc_ToolBoxBar.cpp"
