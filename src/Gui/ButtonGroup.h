/***************************************************************************
                          ButtonGroup.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ButtonGroup.h
 *  \brief The button group module
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#ifndef __BUTTON_GROUP_H__
#define __BUTTON_GROUP_H__

#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qtoolbutton.h>
#include <qdragobject.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qevent.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qscrollbar.h>
#include <qscrollview.h>
#include <qwidget.h>
#include <qlayout.h>

#include "CommandLine.h"
#include "Window.h"
#include "Widgets.h"
#include "PrefWidgets.h"

// forward declaration
class QAction;

/** The button group class
 */
class GuiExport FCButtonGroup : public QButtonGroup
{
  Q_OBJECT
  public:
	  FCButtonGroup(QWidget * parent=0, const char * name=0);
    FCButtonGroup(const QString & title, QWidget * parent=0, const char * name=0);
    FCButtonGroup(int columns, Orientation o, QWidget * parent=0, const char * name=0);
    FCButtonGroup(int columns, Orientation o, const QString & title, QWidget * parent=0, const char * name=0);
	  ~FCButtonGroup(void);

    // overwrite method from base class
    void resizeEvent (QResizeEvent * e);
    QScrollView *pScrollWidget;

  protected:
    /// initialize the button group
    void initialize(void);
    /// rearrange the buttons
    void resizeColumns(void);

    void mousePressEvent( QMouseEvent * );

  protected slots:
    /// show popup menu
    void popupMenuAboutToShow();
    void setNewBackgroundColor();
    void resetBackgroundColor();
    void showIcons();
    void showText();

  protected:
    int m_iChildWidth;
    int m_iChildHeight;
    int m_iWidth, m_iHeight;
    QColor       m_Color;
    QPopupMenu*  m_Popup;
#	pragma warning( disable : 4251 )
    std::map<int, QPixmap> m_Pixmaps;
};

class GuiExport FCToolboxBar : public FCToolBar
{
  Q_OBJECT

  public:
    FCToolboxBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
    virtual ~FCToolboxBar ();

    virtual void clearAll();
    /// set dummy widget to the end
    void addedButton(QString);

  protected:
    void mousePressEvent( QMouseEvent * );
    QColor       m_Color;
    QPopupMenu*  m_Popup;
    QWidget*     m_Dummy;

  protected slots:
    void cleanupEventFilter();
    void popupMenuAboutToShow();
    void setNewBackgroundColor();
    void resetBackgroundColor();
};

class GuiExport FCOutlookBar : public FCToolBar
{
  Q_OBJECT

  public:
    FCOutlookBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
    virtual ~FCOutlookBar ();

    virtual void clearAll();
    /// set dummy widget to the end
    void addedButton(QString);

  protected:
    void mousePressEvent( QMouseEvent * );
    QColor       m_Color;
    QPopupMenu*  m_Popup;
    QWidget*     m_Dummy;

  protected slots:
    void cleanupEventFilter();
    void popupMenuAboutToShow();
    void setNewBackgroundColor();
    void resetBackgroundColor();
};

class QStackBarBtn : public QToolButton
{
  public:
    QStackBarBtn( QWidget *parent, const char *name );
    QStackBarBtn( QWidget *object, QWidget *parent, const char *name );
    ~QStackBarBtn();

    void setSelected( bool b );
    bool isSelected ();
    void setWidget(QWidget* widget);
    QWidget* widget();

  protected:
    void drawButton( QPainter * );

  private:
    bool bIsSelected;
    QWidget* w;
};

class FCCmdBar : public FCWindow, public FCObserver
{
  Q_OBJECT;

  public:
    FCCmdBar( QWidget *parent=0, const char *name=0 );
	  virtual ~FCCmdBar();
    
    // observers method
    void OnChange(FCSubject &rCaller);

    // toolbox handling
    bool addView(QWidget* w, const QString &name);
    bool hasView(QWidget* w);
    bool remView(QWidget* w);
  	bool showView(QWidget* w);
    QWidget* showedView();

  private slots:
    void buttonClicked();

  private:
    void updatePages();
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
    QStackBarBtn                 * m_pLastBtn;
    std::map <QStackBarBtn*, 
              QScrollView*>        m_mButtonView;
};

#endif // __BUTTON_GROUP_H__
