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
#	include <qaction.h>
#	include <qapplication.h>
#	include <qframe.h>
#	include <qlabel.h>
#	include <qlayout.h>
#	include <qlistbox.h>
#	include <qpainter.h>
#	include <qrect.h>
#	include <qthread.h>
#	include <qtoolbutton.h>
#	include <qtooltip.h>
#	include <qvariant.h>
#	include <qwhatsthis.h>
#endif

#include "DlgUndoRedo.h"
#include "Application.h"
#include "Document.h"

#if QT_VERSION > 230
# include <qstyle.h>
#endif

using namespace Gui::Dialog;

CUndoRedoList::CUndoRedoList( QWidget * parent, const char * name, WFlags f)
  : QListBox(parent, name, f)
{
}

void CUndoRedoList::mouseMoveEvent ( QMouseEvent * e )
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

void CUndoRedoList::mousePressEvent (QMouseEvent* e)
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

// ------------------------------------------------------------

/*
 *  Constructs a CUndoRedoDlg which is a child of 'parent', with the 
 *  name 'name'.' 
 */
CUndoRedoDlg::CUndoRedoDlg( QWidget* parent,  const char* name, TMode tMode )
    : QFrame( parent, name, WType_Popup),
      _tMode(tMode)
{
  if ( !name )
  	setName( "CUndoRedoDlg" );
  resize( 160, 140 ); 

  setFrameStyle( WinPanel | Raised );

  _pTextLabel = new QLabel( this, "TextLabel" );
  _pTextLabel->setGeometry( QRect( 5, 110, 150, 25 ) ); 
  _pTextLabel->setFrameStyle(QFrame::Sunken);
  _pTextLabel->setProperty( "focusPolicy", (int)QLabel::NoFocus );
  _pTextLabel->setProperty( "frameShape", (int)QLabel::StyledPanel );
  _pTextLabel->setProperty( "frameShadow", (int)QLabel::Sunken );


  _pListBox = new CUndoRedoList( this, "ListBox" );
  _pListBox->setGeometry( QRect( 5, 5, 150, 100 ) ); 
  _pListBox->setProperty( "frameShadow", (int)QLabel::Sunken );
  _pListBox->setSelectionMode(QListBox::Multi);

  connect(_pListBox, SIGNAL( highlighted ( QListBoxItem * )), this, SLOT(selChangeUndoRedoList()));
  connect(_pListBox, SIGNAL( returnPressed ( QListBoxItem * )), this, SLOT(close()));
  connect(_pListBox, SIGNAL( mouseButtonClicked ( int, QListBoxItem *, const QPoint & )), this, SLOT(selected()));
  fetchUndoRedoInfo();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CUndoRedoDlg::~CUndoRedoDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void CUndoRedoDlg::fetchUndoRedoInfo() 
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

void CUndoRedoDlg::selChangeUndoRedoList() 
{
  // close the listbox
//  close();
  int pos = _pListBox->currentItem() + 1;
  QString text;
  if (_tMode == Undo)
		text = tr("Undoes %1 action(s)").arg(pos);
  else
		text = tr("Redoes %1 action(s)").arg(pos);
  _pTextLabel->setText(text);
}

void CUndoRedoDlg::setMode(TMode tMode)
{
  _tMode = tMode;
}

CUndoRedoDlg::TMode CUndoRedoDlg::getMode() const
{
  return _tMode;
}

void CUndoRedoDlg::updateUndoRedoList()
{
  _pListBox->clear();
  fetchUndoRedoInfo();
}

void CUndoRedoDlg::selected()
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

namespace Gui {
namespace Dialog {

class CToolButtonDropDownPrivate
{
  public:
    CToolButtonDropDownPrivate();
    bool bEntered;
    bool bDropDown;
    bool bActButton;
};

CToolButtonDropDownPrivate::CToolButtonDropDownPrivate()
  : bEntered(false), bDropDown(false), bActButton(false)
{
}

} // namespace Dialog
} // namespace Gui

CToolButtonDropDown::CToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* pWidget, const char * name)
  : QToolButton(parent, name), _pWidget(pWidget)
{
  d = new CToolButtonDropDownPrivate;

  // set the pixmap onto the button
  setIconSet(rclPixmap);
  setAutoRaise(true);
}

CToolButtonDropDown::~CToolButtonDropDown()
{
  delete d;
}

void CToolButtonDropDown::popupWidget()
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

void CToolButtonDropDown::setWidget(QWidget* pWidget)
{
  _pWidget = pWidget;
}

QWidget* CToolButtonDropDown::getWidget()
{
  return _pWidget;
}

void CToolButtonDropDown::drawButton( QPainter * p )
{
  QToolButton::drawButton(p);

  if (isDown())
  {
    // drop down area
    if (d->bDropDown && !d->bActButton)
    {
	    p->setPen( white );
	    p->drawLine( 0, 0, 0, height() );
	    p->drawLine( 0, 0, width()-20-1, 0 );
	    p->setPen( darkGray );
	    p->drawLine( 0, height()-1, width()-20-1, height()-1);
      p->drawLine( width()-20+1, 0, width()-20+1, height());
    }
    // the actual button area
    else
    {
	    p->setPen( white );
	    p->drawLine( width()-20+1, 0, width(), 0 );
      p->drawLine( width()-20, 0, width()-20, height());
	    p->setPen( darkGray );
	    p->drawLine( width()-1, 0, width()-1, height()-1 );
	    p->drawLine( width()-20+1, height()-1, width(), height()-1 );
    }
  }
}

void CToolButtonDropDown::drawButtonLabel( QPainter * p )
{
  // get draw areas for the arrow and the actual icon
  int sx = 0;
  int sy = 0;
  int x, y, w, h;
  int x2, y2, w2, h2;
#if QT_VERSION <= 230
  style().toolButtonRect(width()-19, 0, 19, height() ).rect( &x, &y, &w, &h );
  style().toolButtonRect(0, 0, width()-19, height() ).rect( &x2, &y2, &w2, &h2 );
#else
  style().visualRect(QRect(width()-19, 0, 19, height()), this).rect( &x, &y, &w, &h );
  style().visualRect(QRect(0, 0, width()-19, height()), this ).rect( &x2, &y2, &w2, &h2 );
#endif

  if (isDown() || isOn()) 
  {
#if QT_VERSION <= 230
	  style().getButtonShift(sx, sy);
#else
    //TODO
#endif
	  x+=sx;
	  y+=sy;
  }

  // draw drop down arrow
  if (isDown()&&d->bActButton&&!d->bDropDown)
    drawArrow( p,  isDown(), x+w-15-1, y-1, 15, h, colorGroup(), isEnabled() );
  else
    drawArrow( p,  isDown(), x+w-15, y, 15, h, colorGroup(), isEnabled() );

  if ( !text().isNull() ) 
  {
#if QT_VERSION <= 230
  	style().drawItem( p, x2, y2, w2, h2, AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, text() );
#else
  	style().drawItem( p, QRect(x2, y2, w2, h2), AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, text() );
#endif
  } 
  else 
  {
  	QPixmap pm;
	  if ( usesBigPixmap() ) 
    {
#if QT_VERSION < 300
	    if ( !isEnabled() )
		    pm = iconSet( isOn() ).pixmap( QIconSet::Large, QIconSet::Disabled );
	    else if ( uses3D() )
		    pm = iconSet( isOn() ).pixmap( QIconSet::Large, QIconSet::Active );
	    else
		    pm = iconSet( isOn() ).pixmap( QIconSet::Large, QIconSet::Normal );
#else
	    if ( !isEnabled() )
		    pm = iconSet().pixmap( QIconSet::Large, QIconSet::Disabled );
	    else if ( uses3D() )
		    pm = iconSet().pixmap( QIconSet::Large, QIconSet::Active );
	    else
		    pm = iconSet().pixmap( QIconSet::Large, QIconSet::Normal );
#endif
	  }
    else
    {
#if QT_VERSION < 300
	    if ( !isEnabled() )
    		pm = iconSet( isOn() ).pixmap( QIconSet::Small, QIconSet::Disabled );
	    else if ( uses3D() )
		    pm = iconSet( isOn() ).pixmap( QIconSet::Small, QIconSet::Active );
	    else
		    pm = iconSet( isOn() ).pixmap( QIconSet::Small, QIconSet::Normal );
#else
	    if ( !isEnabled() )
    		    pm = iconSet().pixmap( QIconSet::Small, QIconSet::Disabled );
	    else if ( uses3D() )
		    pm = iconSet().pixmap( QIconSet::Small, QIconSet::Active );
	    else
		    pm = iconSet().pixmap( QIconSet::Small, QIconSet::Normal );
#endif
	  }

  	if ( usesTextLabel() )
    {
	    int fh = fontMetrics().height();
      if (isDown()&&d->bActButton&&!d->bDropDown)
      {
#if QT_VERSION <= 230
	      style().drawItem( p, x2+1, y2+1, w2, h2 - fh, AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#else
	      style().drawItem( p, QRect(x2+1, y2+1, w2, h2 - fh), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#endif
	      p->setFont( font() );
#if QT_VERSION <= 230
	      style().drawItem( p, x2+1, h2+1 - fh, w2, fh, AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
#else
	      style().drawItem( p, QRect(x2+1, h2+1 - fh, w2, fh), AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
#endif
      }
      else
      {
#if QT_VERSION <= 230
	      style().drawItem( p, x2, y2, w2, h2 - fh, AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#else
	      style().drawItem( p, QRect(x2, y2, w2, h2 - fh), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#endif
	      p->setFont( font() );
#if QT_VERSION <= 230
	      style().drawItem( p, x2, h2 - fh, w2, fh, AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
#else
	      style().drawItem( p, QRect(x2, h2 - fh, w2, fh), AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
#endif
      }
 	  } 
    else 
    {
      if (isDown()&&d->bActButton&&!d->bDropDown)
#if QT_VERSION <= 230
  	    style().drawItem( p, x2+1, y2+1, w2, h2, AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#else
  	    style().drawItem( p, QRect(x2+1, y2+1, w2, h2), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#endif
      else
#if QT_VERSION <= 230
  	    style().drawItem( p, x2, y2, w2, h2, AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#else
  	    style().drawItem( p, QRect(x2, y2, w2, h2), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
#endif
	  }
  }

  // draw vertical separator line if entered
  if (d->bEntered)
  {
#if QT_VERSION <= 230
    style().drawSeparator(p, width()-19, y2-1, width()-19, y2+h2,colorGroup());
#else
    style().drawPrimitive( QStyle::PE_Separator, p, QRect( width()-19, y2-1, width()-19, y2+h2 ),	colorGroup() );
#endif
  }
}

void CToolButtonDropDown::drawArrow( QPainter *p, bool down, int x, int y, int w, int h, 
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

QSize CToolButtonDropDown::sizeHint() const
{
  // take extra space for the drop down area
  QSize s = QToolButton::sizeHint();
  s.setWidth(s.width()+20);
  return s;
}

void CToolButtonDropDown::enterEvent ( QEvent * e )
{
  d->bEntered = true;
  QToolButton::enterEvent(e);
}

void CToolButtonDropDown::leaveEvent(QEvent* e)
{
  d->bEntered = false;
  QToolButton::leaveEvent(e);
}

void CToolButtonDropDown::paintEvent( QPaintEvent *e )
{
  if (d->bEntered)
    d->bEntered = isEnabled();
  QToolButton::paintEvent(e);
}

void CToolButtonDropDown::mousePressEvent( QMouseEvent *e )
{
  if ( e->button() != LeftButton )
  	return;

  // check which area is pressed
  if (QRect(width()-20, 0, 20, height()).contains(e->pos()))
  {
    d->bDropDown = true;
    d->bActButton = false;
  }
  else
  {
    d->bDropDown = false;
    d->bActButton = true;
  }

  QToolButton::mousePressEvent(e);
}

void CToolButtonDropDown::mouseReleaseEvent( QMouseEvent *e )
{
  if ( e->button() != LeftButton )
  	return;

  // check which area is pressed
  if (d->bDropDown && !d->bActButton)
  {
    if (hitButton(e->pos()))
      popupWidget();
    setDown(false);
    return;
  }

  QToolButton::mouseReleaseEvent(e);
}

#include "moc_DlgUndoRedo.cpp"

