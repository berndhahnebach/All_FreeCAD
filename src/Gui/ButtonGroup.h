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
#include <qlist.h>
#include <qevent.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qscrollbar.h>
#include <qscrollview.h>

#include "CommandLine.h"
#include "Window.h"
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
    FCmap<int, QPixmap> m_Pixmaps;
};

class FCToolboxButton;
class GuiExport FCToolboxGroup : public QVButtonGroup, public FCWidgetPrefs
{
  Q_OBJECT

  public:
    FCToolboxGroup ( QWidget * parent=0, const char * name=0 );
    FCToolboxGroup ( const QString & title, QWidget * parent=0, const char * name=0 );
    ~FCToolboxGroup ();
    QScrollView *pScrollWidget;
    QGridLayout* ButtonGroupLayout;

    /// add a new widget
    bool addWidget(QWidget* w, int i);
    /// add a new toolbox button
    bool addToolboxButton(FCToolboxButton* b, int i);

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
    FCvector<FCstring> alDroppedActions;
    QColor       m_Color;
    QPopupMenu*  m_Popup;

  protected slots:
    void popupMenuAboutToShow();
    void setNewBackgroundColor();
    void resetBackgroundColor();
    void slotRedrawScrollBar(int);

  signals:
    void signalMaximumWidth(int);
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

class QStackBarBtn
{
protected:
	QColorGroup *_color, *_selColor, *_hiColor;
	QFont _font, _selFont, _hiFont;
	QColor _fcolor, _fselcolor, _fhicolor;
	QString _label;
	QWidget *pWidget;
	QPixmap *pPixmap;

public:
	QStackBarBtn( QString, QWidget * );
	QStackBarBtn( QString, QWidget *, QColor );

	virtual ~QStackBarBtn();

	QFont *selFont() { return &_selFont; }
	QFont *hiFont() { return &_hiFont; }
	QFont *font() { return &_font; }

	QPixmap *pixmap() { return pPixmap; }

	QColorGroup *selColor() { return _selColor; }
	QColorGroup *hiColor() { return _hiColor; }
	QColorGroup *color() { return _color; }

	void setSelColor( QColor  );
	void setHiColor( QColor  );
	void setColor( QColor  );

	void setFontColor( QColor c ) { _fcolor=c; }
	void setFontSelColor( QColor c ) { _fselcolor=c; }
	void setFontHiColor( QColor c ) { _fhicolor=c; }

	void setPixmap( QPixmap * );

	QColor fontColor() { return _fcolor; }
	QColor fontSelColor() { return _fselcolor; }
	QColor fontHiColor() { return _fhicolor; }

	QString label() { return _label; }

	void setLabel( QString s ) { _label=s; }

	QWidget *widget() { return pWidget; }
	void setWidget( QWidget * );
};



class FCCmdBar : public FCDockWindow
{

Q_OBJECT;

public:
	FCCmdBar( QWidget *parent=0, const char *name=0 );
	virtual ~FCCmdBar();

  // toolbox handling
  bool HasView(const char *sName);
  FCToolboxGroup* GetView(const char *sName);
  FCToolboxGroup* CreateView(const char *sName);
  void DeleteView(const char *sName);
	
  void addPage( QStackBarBtn * );
  void remPage( QStackBarBtn * );
	void setCurPage( int );

	void setButtonHeight(int);

	/*!
	 * \brief Gets the button heights
	 *
	 * All buttons must have the same height, here is where you set it.
	 */
	int buttonHeight() { return _stackHeight; }
	/// add buttons just for test purpose
	void AddTestButtons(void);

protected:
	QList <QStackBarBtn> *pButtons;
  FCmap <QWidget*, FCToolboxGroup*> alViews;
	int curPage;

	int curHighlight;
	int _stackHeight;

protected:
	void resizeEvent( QResizeEvent * );
	void paintEvent( QPaintEvent * );
	void mouseMoveEvent( QMouseEvent * );
	void mousePressEvent( QMouseEvent * );

	int whichButton(int, int);
	void rearrangeButtons(int,int);

signals:
	void pageSelected( int );
	void pageHighlighted(int);
	
};

#endif // __BUTTON_GROUP_H__
