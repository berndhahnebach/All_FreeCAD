/***************************************************************************
                          DlgUndoRedo.cpp  -  description
                             -------------------
    begin                : 2002/04/23 21:02:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file DlgUndoRedo.cpp
 *  \brief The undo/redo dialog module
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qlabel.h>
#	include <qlistbox.h>
#	include <qlayout.h>
#	include <qvariant.h>
#	include <qtooltip.h>
#	include <qwhatsthis.h>
#	include <qrect.h>
#	include <qvariant.h>
#	include <qframe.h>
#       ifdef FC_OS_LINUX
#          include <qapplication.h>
#       endif
#endif

#include "DlgUndoRedo.h"
#include "Application.h"
#include "Document.h"

#if QT_VERSION > 230
# include <qstyle.h>
#endif






FCUndoRedoList::FCUndoRedoList( QWidget * parent, const char * name, WFlags f)
  : QListBox(parent, name, f)
{
}

/* 
 *  Constructs a FCUndoRedoDlg which is a child of 'parent', with the 
 *  name 'name'.' 
 */
FCUndoRedoDlg::FCUndoRedoDlg( QWidget* parent,  const char* name, TMode tMode )
    : QFrame( parent, name, WType_Popup),
      _tMode(tMode)
{
  if ( !name )
  	setName( "FCUndoRedoDlg" );
  resize( 160, 140 ); 

  setFrameStyle( WinPanel | Raised );

  _pTextLabel = new QLabel( this, "TextLabel" );
  _pTextLabel->setGeometry( QRect( 5, 110, 150, 25 ) ); 
  _pTextLabel->setFrameStyle(QFrame::Sunken);
  _pTextLabel->setProperty( "focusPolicy", (int)QLabel::NoFocus );
  _pTextLabel->setProperty( "frameShape", (int)QLabel::StyledPanel );
  _pTextLabel->setProperty( "frameShadow", (int)QLabel::Sunken );


  _pListBox = new FCUndoRedoList( this, "ListBox" );
  _pListBox->setGeometry( QRect( 5, 5, 150, 100 ) ); 
  _pListBox->setProperty( "frameShadow", (int)QLabel::Sunken );
  _pListBox->setSelectionMode(QListBox::Multi);

  connect(_pListBox, SIGNAL( highlighted ( QListBoxItem * )), this, SLOT(selChangeUndoRedoList()));
  connect(_pListBox, SIGNAL( returnPressed ( QListBoxItem * )), this, SLOT(close()));
  connect(_pListBox, SIGNAL( mouseButtonClicked ( int, QListBoxItem *, const QPoint & )), this, SLOT(selected()));
  init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCUndoRedoDlg::~FCUndoRedoDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

/** 
 *  This methode fetch the Undo / Redo infromation from the 
 *  active document and shows it in the Undo redo dialog.
 */
void FCUndoRedoDlg::init() 
{
	std::vector<std::string> vecReUndos;
	FCGuiDocument* pcDoc = ApplicationWindow::Instance->GetActiveDocument();

	if(pcDoc)
	{
		if (_tMode == Undo)	
			vecReUndos = pcDoc->GetUndoVector();
		else
			vecReUndos = pcDoc->GetRedoVector();

		for (std::vector<std::string>::iterator i=vecReUndos.begin(); i!=vecReUndos.end(); i++)
			_pListBox->insertItem((*i).c_str());
		_pTextLabel->setProperty( "text", tr( "Cancel" ) );
	}else{
		_pTextLabel->setProperty( "text", tr( "No Undo" ) );
	}

}





void FCUndoRedoList::mouseMoveEvent ( QMouseEvent * e )
{
  if (e->state() == NoButton)
    return;

  QListBox::mouseMoveEvent(e);

  QListBoxItem* item = itemAt (e->pos());
  int pos = -1;
  if (item != NULL)
    pos = index(item);
  if (pos != -1)
  {
    int i;
    for (i=pos+1; i < (int)count(); i++)
    {
      setSelected(i, false);
    }
    for (i=0; i<=pos; i++)
    {
      setSelected(i, true);
    }
  }
}

void FCUndoRedoList::mousePressEvent (QMouseEvent* e)
{
  QListBox::mousePressEvent(e);

  int pos = currentItem();
  if (pos != -1)
  {
    int i;
    for (i=pos+1; i < (int)count(); i++)
    {
      setSelected(i, false);
    }
    for (i=0; i<=pos; i++)
    {
      setSelected(i, true);
    }
  }
}




void FCUndoRedoDlg::selChangeUndoRedoList() 
{
  // close the listbox
//  close();
  int pos = _pListBox->currentItem() + 1;
  QString text;
  if (_tMode == Undo)
    text.sprintf("Undoes %d action(s)", pos);
  else
    text.sprintf("Redoes %d action(s)", pos);
  _pTextLabel->setText(text);
}

void FCUndoRedoDlg::setMode(TMode tMode)
{
  _tMode = tMode;
}

FCUndoRedoDlg::TMode FCUndoRedoDlg::getMode() const
{
  return _tMode;
}

void FCUndoRedoDlg::updateUndoRedoList()
{
  _pListBox->clear();
  init();
}

void FCUndoRedoDlg::selected()
{
  close();
  emit clickedListBox();
}


/* XPM */
static const char *pUndo[]={
"20 17 6 1",
"b c #000000",
"a c #d4d0c8",
"# c None",
". c None",
"d c None",
"c c #f5f4f2",
".##################.",
"#..................#",
"#........bbbb......#",
"#......bb....bb....#",
"#..b..b........b...#",
"#..bbb..........b..#",
"#..bbb..........b..#",
"#..bbbb.........b..#",
"#...............b..#",
"#..............b...#",
"#............bb....#",
"#..................#",
"#.bbbbbbbbb.b.b.b..#",
"#.bbbbbbbbb.b.b.b..#",
"#..................#",
"#..................#",
"cddddddddddddddddddc"};

/* XPM */
static const char *pRedo[]={
"20 17 6 1",
"b c #000000",
"a c #d4d0c8",
"# c None",
". c None",
"d c None",
"c c #f5f4f2",
".##################.",
"#..................#",
"#......bbbb........#",
"#....bb....bb......#",
"#...b........b..b..#",
"#..b..........bbb..#",
"#..b..........bbb..#",
"#..b.........bbbb..#",
"#..b...............#",
"#...b..............#",
"#....bb............#",
"#..................#",
"#..b.b.b.bbbbbbbbb.#",
"#..b.b.b.bbbbbbbbb.#",
"#..................#",
"#..................#",
"cddddddddddddddddddc"};

/* XPM */
static const char *pArrow[]={
"14 17 7 1",
"c c #000000",
"b c #d4d0c8",
"# c #e4e2dd",
"a c #eeedea",
"d c #efedea",
"e c #f5f4f2",
". c None",
".............#",
".............#",
".............#",
".............#",
".............#",
".............#",
"...ccccccc...#",
"....ccccc....#",
".....ccc.....#",
"......c......#",
".............#",
".............#",
".............#",
".............#",
".............#",
".............#",
".............#"};

FCToolButtonDropDown::FCToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* pWidget, const char * name)
  : QToolButton(parent, name), _pWidget(pWidget)
{
  // set the pixmap onto the button
  setIconSet(rclPixmap);
  // create the drop-down button
  _pDropDown = new FCToolButtonDropDown(parent, name);

  // place the two buttons in a horizontal box
  // (this is important if you drag the parent toolbar to the left/right border)
  QHBox* hb = new QHBox(parent);
  reparent(hb, QPoint(0, 0));
  _pDropDown->reparent(hb, QPoint(width(),0));

  // connect the signals of '_pDropDown' with the slots of 'this'...
  connect(_pDropDown, SIGNAL( enterEventSignal(QEvent*)), this, SLOT(enterEventSlot(QEvent*)));
  connect(_pDropDown, SIGNAL( leaveEventSignal(QEvent*)), this, SLOT(leaveEventSlot(QEvent*)));
  // ... and the other way round
  connect(this, SIGNAL( enterEventSignal(QEvent*)), _pDropDown, SLOT(enterEventSlot(QEvent*)));
  connect(this, SIGNAL( leaveEventSignal(QEvent*)), _pDropDown, SLOT(leaveEventSlot(QEvent*)));

  // connect the 'clicked()' signal
  connect(_pDropDown, SIGNAL( clicked()), this, SLOT(popupWidget()));
  bRaise = false;
}

FCToolButtonDropDown::FCToolButtonDropDown( QWidget * parent, const char * name)
  : QToolButton(parent, name), _pDropDown(NULL), _pWidget(NULL)
{
  // resize the drop-down button
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
  setFixedWidth(15);
  // set the pixmap onto the drop-down button
  setIconSet(QPixmap(pArrow));
  bRaise = false;
}

FCToolButtonDropDown::~FCToolButtonDropDown()
{
  if (_pDropDown != NULL)
    delete _pDropDown;
  _pDropDown = NULL;
}

void FCToolButtonDropDown::enterEvent ( QEvent * e )
{
  bRaise = true;
  // do the things of the base class
  QToolButton::enterEvent(e);
  // emit a signal with this event to the other button
  emit enterEventSignal(e);
}

void FCToolButtonDropDown::leaveEvent ( QEvent * e ) 
{
  bRaise = false;
  // do the things of the base class
  QToolButton::leaveEvent(e);
  // emit a signal with this event to the other button
  emit leaveEventSignal(e);
  // when leaving the button has to be redrawn
  update();
}

void FCToolButtonDropDown::enterEventSlot(QEvent* e)
{
  bRaise = true;
  // process the event
  QToolButton::enterEvent(e);
}

void FCToolButtonDropDown::leaveEventSlot(QEvent* e)
{
  bRaise = false;
  // process the event
  QToolButton::leaveEvent(e);
}

void FCToolButtonDropDown::popupWidget()
{
  // popup the widget
  if (_pWidget && _pDropDown)
  {
    // update the content of the widget
    emit updateWidgetSignal();

    // compute the position moving to for the widget
    QPoint point  = mapToGlobal(pos()) - pos();
    QPoint height = QPoint(0, this->height());
    QPoint pos    = point+height;

    QSize  size   = _pWidget->size ();
    int w = size.width();
    int h = size.height();

    QWidget* desk = QApplication::desktop();
    int screen_w  = desk->width();
    int screen_h  = desk->height();

    if (pos.x() < 0)
      pos.setX(0);
    if (pos.x() + w > screen_w)
      pos.setX(screen_w - w);
    if (pos.y() < 0)
      pos.setY(0);
    if (pos.y() + h > screen_h)
      pos.setY(pos.y() - height.y() - h);

    _pWidget->move(pos);
    _pWidget->show();
  }
}

void FCToolButtonDropDown::setWidget(QWidget* pWidget)
{
  _pWidget = pWidget;
}

QWidget* FCToolButtonDropDown::getWidget()
{
  return _pWidget;
}

void FCToolButtonDropDown::setButtonEnabled(bool bEnable)
{
  setEnabled(bEnable);
  if (_pDropDown)
    _pDropDown->setEnabled(bEnable);
}

bool FCToolButtonDropDown::isButtonEnabled()
{
  bool bEnabled = true;
  bEnabled &= isEnabled();
  if (_pDropDown)
    bEnabled &= _pDropDown->isEnabled();

  return bEnabled;
}

void FCToolButtonDropDown::setAutoRaiseEx (bool bEnable)
{
  setAutoRaise(bEnable);
  if (_pDropDown != NULL)
    _pDropDown->setAutoRaise(bEnable);
}

bool FCToolButtonDropDown::autoRaiseEx () const
{
  if (_pDropDown != NULL)
    return _pDropDown->autoRaise() && autoRaise();
  return autoRaise();
}

void FCToolButtonDropDown::drawButton( QPainter * p )
{

#if QT_VERSION >= 300

  QStyle::SCFlags controls = QStyle::SC_ToolButton;
  QStyle::SCFlags active = QStyle::SC_None;

  if (isDown())
	  active |= QStyle::SC_ToolButton;

  QStyle::SFlags flags = QStyle::Style_Default;
  if (isEnabled())
	  flags |= QStyle::Style_Enabled;
  if (hasFocus())
	  flags |= QStyle::Style_HasFocus;
  if (isDown())
	  flags |= QStyle::Style_Down;
  if (isOn())
	  flags |= QStyle::Style_On;
  if (autoRaise()) 
  {
	  flags |= QStyle::Style_AutoRaise;
  	if (bRaise) 
    {
      flags |= QStyle::Style_MouseOver;
	    if (! isOn() && ! isDown())
		    flags |= QStyle::Style_Raised;
  	}
  } 
  else if (! isOn() && ! isDown())
  	flags |= QStyle::Style_Raised;

  style().drawComplexControl(QStyle::CC_ToolButton, p, this, rect(), colorGroup(),
			       flags, controls, active, QStyleOption());

  drawButtonLabel(p);

#else // < QT 3.x
    QToolButton::drawButton(p);
#endif

}

#include "moc_DlgUndoRedo.cpp"

