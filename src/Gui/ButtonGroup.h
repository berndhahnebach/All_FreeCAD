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
/*
class FCToolboxButton;
class GuiExport FCToolboxGroup : public QVButtonGroup, public FCWidgetPrefs
{
  Q_OBJECT

  public:
    FCToolboxGroup ( QWidget * parent=0, const char * name=0 );
    FCToolboxGroup ( const QString & title, QWidget * parent=0, const char * name=0 );
    ~FCToolboxGroup ();
    QGridLayout* ButtonGroupLayout;

    /// add a new widget
    bool addWidget(QWidget* w, int i);
    /// add a new toolbox button
    bool addToolboxButton(FCToolboxButton* b, int i);
    void loadUserDefButtons();

  protected:
    /// restore the preferences
    virtual void restorePreferences();
    /// save the preferences
    virtual void savePreferences();
    void initialize(QWidget* parent);
    void paintEvent (QPaintEvent * e);
    void mousePressEvent( QMouseEvent * );
    /// allow drop buttons
    void dropEvent ( QDropEvent * );
    void dragEnterEvent ( QDragEnterEvent * );
    void dragLeaveEvent ( QDragLeaveEvent * );
    void dragMoveEvent ( QDragMoveEvent * );
    std::vector<std::string> alDroppedActions;
    QColor       m_Color;
    QPopupMenu*  m_Popup;

  protected slots:
    void popupMenuAboutToShow();
    void setNewBackgroundColor();
    void resetBackgroundColor();

  signals:
    void signalMaximumWidth(int);
};
*/

class FCToolBar;
class FCToolboxButton;
class GuiExport FCToolboxGroup : public FCToolBar
{
  Q_OBJECT

  public:
    FCToolboxGroup ( const QString & label, QWidget *, const char * name = 0, WFlags f = 0 );
    virtual ~FCToolboxGroup ();

  protected:
    /// restore the preferences
    virtual void restorePreferences();
    /// save the preferences
    virtual void savePreferences();
    void mousePressEvent( QMouseEvent * );
    QColor       m_Color;
    QPopupMenu*  m_Popup;

  protected slots:
    void cleanupEventFilter();
    void popupMenuAboutToShow();
    void setNewBackgroundColor();
    void resetBackgroundColor();
  
  signals:
    void destroyed();
};

class GuiExport FCToolboxButton : public QToolButton
{
  Q_OBJECT

  public:
    FCToolboxButton ( QWidget * parent=0, const char * name=0 );
    FCToolboxButton ( const QString & title, const QString &tooltip = 0, QWidget *parent = 0, const char *name = 0 );
    FCToolboxButton ( const QString & title, const QPixmap &pix, const QString &tooltip = 0, 
                      QWidget *parent = 0, const char *name = 0 );
    FCToolboxButton ( const QString & title, const QPixmap &pix, const QString &tooltip = 0,
                      QObject *receiver = 0, const char *member = 0, QWidget *parent = 0, const char *name = 0 );

    virtual ~FCToolboxButton();
    
    /// set text and pixmap
    void setTextAndPixmap( const QString &text, const QPixmap &pix);
    /// set text only
    void setText(const char *text);
    /// set pixmap only
    void setPixmap( const QPixmap& pixmap );
    /// set tooltip
    void setTooltip( const QString& tooltip );
    /// get text
    const char *text() const; 
    QSize sizeHint() const;
    /// enable the button if enable is true
    void enable(bool enable);
    void showText(bool enable);
    void makeDisabledPixmap();
    void on(bool flag);
    void toggle();
    void setAction(QAction* action) { pLastAction = action; }

  public slots:
    void slotResizeButton(int);
  
  protected:
    // mouse events
    void leaveEvent( QEvent *_ev );
    void enterEvent( QEvent *_ev );
    // drag'n'drop
    void dropEvent ( QDropEvent * );
    void dragEnterEvent ( QDragEnterEvent * );
    void dragLeaveEvent ( QDragLeaveEvent * );
    void dragMoveEvent ( QDragMoveEvent * );

    void drawButton( QPainter *_painter );
    void drawButtonLabel( QPainter *_painter );
    void paint( QPainter *_painter );
    void paletteChange(const QPalette &);

  private:
    bool tbShowText;
    bool raised;    
    QString textLabel;
    QPixmap enabledPixmap;
    QPixmap disabledPixmap;
    QAction* pLastAction;
};

class QStackBarBtn : public QToolButton
{
  public:
    QStackBarBtn( QWidget *parent, const char *name );
    QStackBarBtn( QWidget *object, QWidget *parent, const char *name );
    ~QStackBarBtn();

    void setSelected( bool b );
    void setWidget(QWidget* widget);
    QWidget* widget();

  protected:
    void drawButton( QPainter * );

  private:
    bool bIsSelected;
    QWidget* w;
};

class FCCmdBar : public FCDockWindow
{
  Q_OBJECT;

  public:
	  FCCmdBar( QWidget *parent=0, const char *name=0 );
	  virtual ~FCCmdBar();

    void addPage( const QString &name, QWidget *page );
    void remPage( QStackBarBtn * );
  	void setCurPage( int );

    // toolbox handling
    bool HasView(const char *sName);
    FCToolboxGroup* GetView(const char *sName);
    FCToolboxGroup* CreateView(const char *sName);
    void DeleteView(const char *sName);

  private slots:
    void buttonClicked();

  private:
    void updatePages();
    void timerEvent ( QTimerEvent * e);
    void animatePageScroll(QWidget* pCurPage, QWidget* pNewPage);

  private:
    long                           m_lAnimCount;
    QVBoxLayout                  * m_pLayout;
    QWidget                      * m_pCurPage;
    QStackBarBtn                 * m_pLastPage;
    std::list<QStackBarBtn*>       m_lButtons;
    std::map <QWidget*, QWidget*>  m_mViews;
};

#endif // __BUTTON_GROUP_H__
