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


#ifndef __STACKBAR_H__
#define __STACKBAR_H__

#include "DockWindow.h"

#ifndef _PreComp_
# include <qtoolbutton.h>
#endif

class QScrollView;
class QVBoxLayout;

namespace Gui {
namespace DockWnd {

class StackBar;

/**
 * Each StackBarButton object corresponds to a @ref StackBar's widget.
 * \author Werner Mayer
 */
class StackBarButton : public QToolButton
{
public:
  StackBarButton( QWidget *object, StackBar *parent, const char *name );
  StackBarButton( StackBar *parent, const char *name );
  ~StackBarButton();

  void setSelected( bool b );
  bool isSelected ();
  void setWidget(QWidget* widget);
  QWidget* widget();

protected:
  void drawButton( QPainter * );
  void mousePressEvent ( QMouseEvent * e );

private:
  bool bIsSelected;
  QWidget* w;
  StackBar* pStackBar;
};

/**
 * Implements a button-bar similar to that of MS Outlook
 *
 * Use this widget to implement multiple pages of widgets in a single vertical
 * column.  
 * 
 * The following code demonstrates how to create a stackbar, and add a few pages
 * to it with some widgets.
 * 
 * For more examples, see the tests in the test directory of the distribution.
 * 
 * \code
 *
 * StackBar *sb;
 * QListBox *l1;
 * 
 * sb = new StackBar(pParent, 0);
 * l1 = new QListBox *l1 = new QListBox( sb );
 * 
 * l1->insertItem("Item 1");
 * l1->insertItem("Item 2");
 * l1->insertItem("Item 3");
 * l1->insertItem("Item 4");
 * 
 * sb.addItem( l1, "List Box 1", l1 );
 * sb.addItem( new QPushButton("Hello"), "A Button" );
 * sb.addItem( new QPushButton("Yep"), "Another Button" );
 *
 * \endcode
 * \author Werner Mayer
 *
 * This code was originally taken from the QStackBarBtn and QStackBar implementation
 * written by Dave Marotti (http://phlan.net/~lndshark/qstackbar).
 */
class StackBar : public Gui::DockWindow, public ParameterGrp::ObserverType
{
  Q_OBJECT;

public:
  StackBar( QWidget *parent=0, const char *name=0 );
  virtual ~StackBar();
  
  // observers method
  void OnChange(Base::Subject<const char*> &rCaller,const char* sReason);

  /** @name toolbox handling */
  //@{
  bool addItem( QWidget* w, const QString &name );
  bool removeItem( QWidget* w );

  int  indexOf( QWidget* w ) const;
  void setCurrentItem( QWidget* w );
  QWidget* currentItem() const;

  void setItemVisible( QWidget* w, bool b );
  bool isItemVisible(QWidget* w) const;
  //@}

private slots:
  void buttonClicked();

private:
  void updatePages();
  StackBarButton* firstPageVisible();
  void timerEvent ( QTimerEvent * e);
  void animatePageScroll(QScrollView* pCurPage, QScrollView* pNewPage);

private:
  bool                           m_bAnimated;
  long                           m_lAnimCount;
  int                            m_iCurHeight;
  int                            m_iNewHeight;
  QVBoxLayout                  * m_pLayout;
  QScrollView                  * m_pCurPage;
  QScrollView                  * m_pAnimNewPage;
  QScrollView                  * m_pAnimCurPage;
  StackBarButton               * m_pLastBtn;
  std::map <StackBarButton*, 
            QScrollView*>        m_mButtonView;
};

} // namespace DockWnd
} // namespace Gui

#endif // __STACKBAR_H__

