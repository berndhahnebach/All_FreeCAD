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

#include "Window.h"
#include "PrefWidgets.h"

// forward declaration
class QAction;

namespace Gui {

// forward declaration
namespace DockWnd {
class StackBar;
} // namespace DockWnd

/** The button group class
 */
class GuiExport ButtonGroup : public QButtonGroup
{
  Q_OBJECT
  public:
    /** @name Construction/destruction */
    //@{
	  ButtonGroup(QWidget * parent=0, const char * name=0);
    ButtonGroup(const QString & title, QWidget * parent=0, const char * name=0);
    ButtonGroup(int columns, Orientation o, QWidget * parent=0, const char * name=0);
    ButtonGroup(int columns, Orientation o, const QString & title, QWidget * parent=0, const char * name=0);
	  ~ButtonGroup(void);
    //@}

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
    std::map<int, QPixmap> m_Pixmaps;
};

/** The command bar base class
 */
class GuiExport CommandBar : public FCToolBar
{
  Q_OBJECT

  public:
    CommandBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
    virtual ~CommandBar ();

    virtual void clearAll();
    /// set dummy widget to the end
    virtual void addedButton(QString);

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

/** A special implementation of command bar
 */
class GuiExport ToolboxBar : public CommandBar
{
  Q_OBJECT

  public:
    ToolboxBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
    virtual ~ToolboxBar ();
    /// set dummy widget to the end
    virtual void addedButton(QString);
};

/** Another special implementation of command bar
 * The bar seems like the Outlook bar.
 */
class GuiExport OutlookBar : public CommandBar
{
  Q_OBJECT

  public:
    OutlookBar ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
    virtual ~OutlookBar ();
    /// set dummy widget to the end
    virtual void addedButton(QString);
};

/** QStackBarBtn class.
 * Each QStackBarBtn object corresponds to a @ref StackBar's widget.
 */
class QStackBarBtn : public QToolButton
{
  public:
    QStackBarBtn( DockWnd::StackBar *parent, const char *name );
    QStackBarBtn( QWidget *object, DockWnd::StackBar *parent, const char *name );
    ~QStackBarBtn();

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
		DockWnd::StackBar* pStackBar;
};

namespace DockWnd {

/** StackBar class
 * To the stack bar you can add any type of QWidget objects.
 */
class StackBar : public FCDockWindow, public FCParameterGrp::ObserverType
{
  Q_OBJECT;

  public:
    StackBar( QWidget *parent=0, const char *name=0 );
	  virtual ~StackBar();
    
    // observers method
    void OnChange(FCSubject<const char*> &rCaller,const char* sReason);

    /** @name toolbox handling */
    //@{
    bool addView(QWidget* w, const QString &name);
    bool hasView(QWidget* w);
    bool remView(QWidget* w);
  	bool showView(QWidget* w);
    void showPage(QWidget* w);
    void hidePage(QWidget* w);
    bool isPageVisible(QWidget* w);
    QWidget* showedView();
    //@}

  private slots:
    void buttonClicked();

  private:
    void updatePages();
    QStackBarBtn* firstPageVisible();
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

} // namespace DockWnd
} // namespace Gui

#endif // __BUTTON_GROUP_H__
