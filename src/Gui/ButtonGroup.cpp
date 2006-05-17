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
# include <qbutton.h>
# include <qcolordialog.h>
# include <qcursor.h>
# include <qdragobject.h>
# include <qlayout.h>
# include <qobjectlist.h>
# include <qpopupmenu.h>
# include <qscrollview.h>
#endif

#include "ButtonGroup.h"

using namespace Gui::DockWnd;

/* TRANSLATOR Gui::DockWnd::ButtonGroup */

/** 
 * Constructs a button group with no title. 
 */
ButtonGroup::ButtonGroup(QWidget * parent, const char * name)
: QButtonGroup(parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

/**
 * Constructs a button group with the title \a title.
 */
ButtonGroup::ButtonGroup(const QString & title, QWidget * parent, const char * name)
: QButtonGroup(title, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

/**
 * Constructs a button group with no title. Child widgets will be arranged in \a strips rows 
 * or columns (depending on \a orientation).
 */
ButtonGroup::ButtonGroup(int strips, Orientation o, QWidget * parent, const char * name)
: QButtonGroup(strips, o, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

/**
 * Constructs a button group with title \a title. Child widgets will be arranged in \a strips rows 
 * or columns (depending on \a orientation).
 */
ButtonGroup::ButtonGroup(int strips, Orientation o, const QString & title, QWidget * parent, const char * name)
: QButtonGroup(strips, o, title, parent, name), m_iChildWidth(32), m_iChildHeight(32)
{
  pScrollWidget = new QScrollView(parent, "Scroll", QWidget::WPaintClever);
  setFrameStyle(QFrame::NoFrame);
  pScrollWidget->setFrameStyle(QFrame::NoFrame);
  pScrollWidget->addChild(this);
  pScrollWidget->setResizePolicy(QScrollView::AutoOneFit);
  initialize();
}

ButtonGroup::~ButtonGroup(void)
{
}

void ButtonGroup::initialize()
{
  m_Color = backgroundColor();

  resizeColumns();
  setFocusPolicy (ClickFocus);

  m_Popup = new QPopupMenu(0L);
  m_Popup->setCheckable(true);
  connect(m_Popup, SIGNAL(aboutToShow()), this, SLOT(onPopupMenuAboutToShow()));
}

/**
 * Reimplememted from QButtonGroup.
 */
void ButtonGroup::resizeEvent (QResizeEvent * e)
{
  QButtonGroup::resizeEvent(e);

  QSize size = e->size();

  if (m_iWidth != size.width())
  {
    resizeColumns();
  }

  if (m_iHeight != size.height())
  {
    // reduce the minimum width
    setMinimumWidth(layout()->margin() + m_iChildWidth);
    resizeColumns();
  }
}

/**
 * Resizes the columns and rearranges the buttons inside.
 */
void ButtonGroup::resizeColumns(void)
{
  int cols, rows;
  int h = height();
  int w = width ();
  int cnt = count();

  int margin = layout()->margin();

  cols = w / (margin + m_iChildWidth);
  rows = h / (margin + m_iChildHeight);

  if (rows > 0 && cols > 0)
  {
    if (cnt / rows + 1 > cols)
    {
      cols = cnt / rows;
      if (cnt % rows > 0)
        cols++;
    }
  }

  setColumns(cols);

  if (cols * (margin + m_iChildWidth) > w)
  {
    // increase the minimum width to show all buttons
    setMinimumWidth(cols * (margin + m_iChildWidth));
  }

  m_iWidth  = width();
  m_iHeight = height();
}

/**
 * Reimplememted from QButtonGroup.
 */
void ButtonGroup::mousePressEvent( QMouseEvent * e )
{
  if (e->button() == LeftButton)
  {
    if (acceptDrops() == true)
    {
      QDragObject *drobj;
      drobj = new QTextDrag( title(), this );
      drobj->dragCopy();
    }
  }
  else if (e->button() == RightButton)
  {
    m_Popup->exec(QCursor::pos());
  }
  else if (e->button() == MidButton)
  {
  }
}

/**
 * Fill up context menu when it is about to appear.
 */
void ButtonGroup::onPopupMenuAboutToShow()
{
  m_Popup->clear();

  if (count() > 0)
  {
//    m_Popup->insertItem("Side by side", this, SLOT(onSetNewBackgroundColor()));
//    m_Popup->insertItem("Untereinander", this, SLOT(onSetNewBackgroundColor()));
//    m_Popup->insertSeparator();

    if (m_Pixmaps.size() > 0)
      m_Popup->insertItem(tr("Show buttons as icon"), this, SLOT(onShowIcons()));
    else
      m_Popup->insertItem(tr("Show buttons as text"), this, SLOT(onShowText()));
    m_Popup->insertSeparator();
  }

  m_Popup->insertItem(tr("Background color..."), this, SLOT(onSetNewBackgroundColor()));
  m_Popup->insertItem(tr("Reset background color"), this, SLOT(onResetBackgroundColor()));
}

/**
 * Opens a QColorDialog to change background color.
 */
void ButtonGroup::onSetNewBackgroundColor()
{
  QColor color = QColorDialog::getColor ( backgroundColor(), this);
  if (color.isValid())
  {
    setPalette(QPalette(color, color));
  }
}

/**
 * Resets the default background color. 
 */
void ButtonGroup::onResetBackgroundColor()
{
  if (m_Color.isValid())
    setPalette(QPalette(m_Color, m_Color));
}

/**
 * Show the buttons inside as icons.
 */
void ButtonGroup::onShowIcons()
{
  int cnt = count();

  for (std::map<int, QPixmap>::const_iterator it = m_Pixmaps.begin(); it != m_Pixmaps.end(); it++)
  {
    if (it->first < cnt)
    {
      QButton* button = find(it->first);
      if (button != NULL)
        button->setPixmap(it->second);
    }
  }

  m_Pixmaps.clear();
}

/**
 * Shows the buttons inside as text.
 */
void ButtonGroup::onShowText()
{
  m_Pixmaps.clear();
  int cnt = count();
  for (int i=0; i<cnt; i++)
  {
    QButton* button = find(i);
    if (button != NULL)
    {
      const QPixmap* pic = button->pixmap();
      if (pic != NULL)
      {
        if (!pic->isNull())
         m_Pixmaps[i] = *button->pixmap();
      }

      button->setText(button->name(""));
    }
  }
}

#include "moc_ButtonGroup.cpp"
