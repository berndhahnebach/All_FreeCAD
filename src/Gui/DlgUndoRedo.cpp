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


#include "PreCompiled.h"

#ifndef _PreComp_
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
  : QToolButton(parent, name), _pWidget(pWidget), bEntered(false)
{
  // set the pixmap onto the button
  setIconSet(rclPixmap);
  setAutoRaise(true);
}

FCToolButtonDropDown::~FCToolButtonDropDown()
{
}

void FCToolButtonDropDown::popupWidget()
{
  // popup the widget
  if (_pWidget)
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

void FCToolButtonDropDown::drawButtonLabel( QPainter * p )
{
  // get draw areas for the arrow and the actual icon
  int sx = 0;
  int sy = 0;
  int x, y, w, h;
  int x2, y2, w2, h2;
  style().toolButtonRect(width()-19, 0, 19, height() ).rect( &x, &y, &w, &h );
  style().toolButtonRect(0, 0, width()-19, height() ).rect( &x2, &y2, &w2, &h2 );

  if (isDown() || isOn()) 
  {
	  style().getButtonShift(sx, sy);
	  x+=sx;
	  y+=sy;
  }

  // draw drop down arrow
  drawArrow( p,  isDown(), x+w-15, y, 15, h, colorGroup(), isEnabled() );

  if ( !text().isNull() ) 
  {
  	style().drawItem( p, x2, y2, w2, h2, AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, text() );
  } 
  else 
  {
  	QPixmap pm;
	  if ( usesBigPixmap() ) 
    {
	    if ( !isEnabled() )
		    pm = iconSet( isOn() ).pixmap( QIconSet::Large, QIconSet::Disabled );
	    else if ( uses3D() )
		    pm = iconSet( isOn() ).pixmap( QIconSet::Large, QIconSet::Active );
	    else
		    pm = iconSet( isOn() ).pixmap( QIconSet::Large, QIconSet::Normal );
	  } 
    else 
    {
	    if ( !isEnabled() )
    		pm = iconSet( isOn() ).pixmap( QIconSet::Small, QIconSet::Disabled );
	    else if ( uses3D() )
		    pm = iconSet( isOn() ).pixmap( QIconSet::Small, QIconSet::Active );
	    else
		    pm = iconSet( isOn() ).pixmap( QIconSet::Small, QIconSet::Normal );
	  }

  	if ( usesTextLabel() ) 
    {
	    int fh = fontMetrics().height();
	    style().drawItem( p, x2, y2, w2, h2 - fh, AlignCenter, colorGroup(), TRUE, &pm, QString::null );
	    p->setFont( font() );
	    style().drawItem( p, x2, h2 - fh, w2, fh, AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
 	  } 
    else 
    {
	    style().drawItem( p, x2, y2, w2, h2, AlignCenter, colorGroup(), TRUE, &pm, QString::null );
	  }
  }

  // draw vertical separator line if entered
  if (bEntered)
  {
    style().drawSeparator(p, width()-19, y2, width()-19, y2+h2,colorGroup());
  }
}

void FCToolButtonDropDown::drawArrow( QPainter *p, bool down, int x, int y, int w, int h, 
                                      const QColorGroup &g, bool enabled, const QBrush *fill )
{
  QPointArray a;
	a.setPoints( 7, -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1 );

	x++;
	y++;

  QPen savePen = p->pen();
  if (down)
  	p->setBrushOrigin(p->brushOrigin() + QPoint(1,1));
  if ( fill )
	  p->fillRect( x, y, w, h, *fill );
  if (down)
	  p->setBrushOrigin(p->brushOrigin() - QPoint(1,1));
  if ( enabled ) 
  {
	  a.translate( x+w/2, y+h/2 );
	  p->setPen( g.buttonText() );
	  p->drawLineSegments( a, 0, 3 );
	  p->drawPoint( a[6] );
  } 
  else 
  {
	  a.translate( x+w/2+1, y+h/2+1 );
	  p->setPen( g.light() );
	  p->drawLineSegments( a, 0, 3 );
	  p->drawPoint( a[6] );
	  a.translate( -1, -1 );
	  p->setPen( g.mid() );
	  p->drawLineSegments( a, 0, 3 );
	  p->drawPoint( a[6] );
  }

  p->setPen( savePen );
}

QSize FCToolButtonDropDown::sizeHint() const
{
  // take extra space for the drop down area
  QSize s = QToolButton::sizeHint();
  s.setWidth(s.width()+20);
  return s;
}

void FCToolButtonDropDown::enterEvent ( QEvent * e )
{
  bEntered = true;
  QToolButton::enterEvent(e);
}

void FCToolButtonDropDown::leaveEvent(QEvent* e)
{
  bEntered = false;
  QToolButton::leaveEvent(e);
}

void FCToolButtonDropDown::mousePressEvent( QMouseEvent *e )
{
  if ( e->button() != LeftButton )
  	return;

  // check which area is pressed
  if (QRect(width()-20, 0, 20, height()).contains(e->pos()))
  {
    popupWidget();
    return;
  }

  QToolButton::mousePressEvent(e);
}

#include "moc_DlgUndoRedo.cpp"

