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
# include <qpainter.h>
# include <qstyle.h>
# include <qvariant.h>
#endif

#include "DlgUndoRedo.h"
#include "Application.h"
#include "Document.h"

using namespace Gui::Dialog;

/**
 *  Constructs a UndoRedoList which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
UndoRedoList::UndoRedoList( QWidget * parent, const char * name, WFlags f)
  : QListBox(parent, name, f)
{
}

/** Destroys the object and frees any allocated resources */
UndoRedoList::~UndoRedoList()
{
}

/** 
 * If the left mouse button is pressed and depending on the moving direction
 * the items are selected or deselected.
 */
void UndoRedoList::mouseMoveEvent ( QMouseEvent * e )
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

/** Select all items above the selected one */
void UndoRedoList::mousePressEvent (QMouseEvent* e)
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

/**
 *  Constructs a UndoRedoDialog which is a child of 'parent', with the 
 *  name 'name'.' 
 */
UndoRedoDialog::UndoRedoDialog( QWidget* parent,  const char* name, TMode mode )
    : QFrame( parent, name, WType_Popup), tMode(mode)
{
  if ( !name )
    setName( "UndoRedoDlg" );
  resize( 160, 140 ); 

  setFrameStyle( WinPanel | Raised );

  pTextLabel = new QLabel( this, "TextLabel" );
  pTextLabel->setGeometry( QRect( 5, 110, 150, 25 ) ); 
  pTextLabel->setFrameStyle(QFrame::Sunken);
  pTextLabel->setProperty( "focusPolicy", (int)QLabel::NoFocus );
  pTextLabel->setProperty( "frameShape", (int)QLabel::StyledPanel );
  pTextLabel->setProperty( "frameShadow", (int)QLabel::Sunken );


  pListBox = new UndoRedoList( this, "ListBox" );
  pListBox->setGeometry( QRect( 5, 5, 150, 100 ) ); 
  pListBox->setProperty( "frameShadow", (int)QLabel::Sunken );
  pListBox->setSelectionMode(QListBox::Multi);

  connect(pListBox, SIGNAL( highlighted ( QListBoxItem * )), this, SLOT(onSelChangeUndoRedoList()));
  connect(pListBox, SIGNAL( returnPressed ( QListBoxItem * )), this, SLOT(close()));
  connect(pListBox, SIGNAL( mouseButtonClicked ( int, QListBoxItem *, const QPoint & )), this, SLOT(onSelected()));
  fetchUndoRedoInfo();
}

/** 
 *  Destroys the object and frees any allocated resources.
 */
UndoRedoDialog::~UndoRedoDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

/** 
 *  This method fetches the undo / redo information from the 
 *  active document and shows it in the undo / redo dialog.
 */
void UndoRedoDialog::fetchUndoRedoInfo() 
{
  std::vector<std::string> vecReUndos;
  FCGuiDocument* pcDoc = ApplicationWindow::Instance->GetActiveDocument();

  if( pcDoc )
  {
    if ( tMode == Undo ) 
      vecReUndos = pcDoc->GetUndoVector();
    else
      vecReUndos = pcDoc->GetRedoVector();

    for (std::vector<std::string>::iterator i=vecReUndos.begin(); i!=vecReUndos.end(); i++)
      pListBox->insertItem((*i).c_str());
    pTextLabel->setProperty( "text", tr( "Cancel" ) );
  }
  else
  {
    pTextLabel->setProperty( "text", tr( "No Undo" ) );
  }
}

/** Sets the number of actons to undo/redo. */
void UndoRedoDialog::onSelChangeUndoRedoList() 
{
  // close the listbox
//  close();
  int pos = pListBox->currentItem() + 1;
  QString text;
  if ( tMode == Undo )
    text = tr("Undoes %1 action(s)").arg(pos);
  else
    text = tr("Redoes %1 action(s)").arg(pos);
  pTextLabel->setText(text);
}

/** Switch to the mode \a tMode (either Undo or Redo). */
void UndoRedoDialog::setMode(TMode mode)
{
  tMode = mode;
}

/** Returns the current mode. */
UndoRedoDialog::TMode UndoRedoDialog::getMode() const
{
  return tMode;
}

/** Updates the undo/redo list. */
void UndoRedoDialog::updateUndoRedoList()
{
  pListBox->clear();
  fetchUndoRedoInfo();
}

/** Closes the dialog and emits the @ref clickedListBox() signal. */
void UndoRedoDialog::onSelected()
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

class ToolButtonDropDownPrivate
{
  public:
    ToolButtonDropDownPrivate();
    bool bEntered;
    bool bDropDown;
    bool bActButton;
};

ToolButtonDropDownPrivate::ToolButtonDropDownPrivate()
  : bEntered(false), bDropDown(false), bActButton(false)
{
}

} // namespace Dialog
} // namespace Gui

/** Construction */
ToolButtonDropDown::ToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* widget, const char * name)
  : QToolButton(parent, name), pWidget(widget)
{
  d = new ToolButtonDropDownPrivate;

  // set the pixmap onto the button
  setIconSet(rclPixmap);
  setAutoRaise(true);
}

/** Destruction */
ToolButtonDropDown::~ToolButtonDropDown()
{
  delete d;
}

/** Pops up the window */
void ToolButtonDropDown::onPopupWidget()
{
  // popup the widget
  if ( pWidget )
  {
    // update the content of the widget
    emit updateWidgetSignal();

    // compute the position moving to for the widget
    QPoint point  = mapToGlobal(pos()) - pos();
    QPoint height = QPoint(0, this->height());
    QPoint pos    = point+height;

    QSize  size   = pWidget->size ();
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

    pWidget->move(pos);
    pWidget->show();
  }
}

/** Sets a widget, this widget will be shown if click this button */
void ToolButtonDropDown::setWidget(QWidget* widget)
{
  pWidget = widget;
}

/** Returns the current widget */
QWidget* ToolButtonDropDown::getWidget()
{
  return pWidget;
}

/** Draws the button */
void ToolButtonDropDown::drawButton( QPainter * p )
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

/** Draws the button label */
void ToolButtonDropDown::drawButtonLabel( QPainter * p )
{
  // get draw areas for the arrow and the actual icon
  int sx = 0;
  int sy = 0;
  int x, y, w, h;
  int x2, y2, w2, h2;
  style().visualRect(QRect(width()-19, 0, 19, height()), this).rect( &x, &y, &w, &h );
  style().visualRect(QRect(0, 0, width()-19, height()), this ).rect( &x2, &y2, &w2, &h2 );

  if (isDown() || isOn()) 
  {
    //TODO
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
    style().drawItem( p, QRect(x2, y2, w2, h2), AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, text() );
  } 
  else 
  {
    QPixmap pm;
    if ( usesBigPixmap() ) 
    {
      if ( !isEnabled() )
        pm = iconSet().pixmap( QIconSet::Large, QIconSet::Disabled );
      else if ( uses3D() )
        pm = iconSet().pixmap( QIconSet::Large, QIconSet::Active );
      else
        pm = iconSet().pixmap( QIconSet::Large, QIconSet::Normal );
    }
    else
    {
      if ( !isEnabled() )
        pm = iconSet().pixmap( QIconSet::Small, QIconSet::Disabled );
      else if ( uses3D() )
        pm = iconSet().pixmap( QIconSet::Small, QIconSet::Active );
      else
        pm = iconSet().pixmap( QIconSet::Small, QIconSet::Normal );
    }

    if ( usesTextLabel() )
    {
      int fh = fontMetrics().height();
      if (isDown()&&d->bActButton&&!d->bDropDown)
      {
        style().drawItem( p, QRect(x2+1, y2+1, w2, h2 - fh), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
        p->setFont( font() );
        style().drawItem( p, QRect(x2+1, h2+1 - fh, w2, fh), AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
      }
      else
      {
        style().drawItem( p, QRect(x2, y2, w2, h2 - fh), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
        p->setFont( font() );
        style().drawItem( p, QRect(x2, h2 - fh, w2, fh), AlignCenter + ShowPrefix, colorGroup(), isEnabled(), 0, textLabel() );
      }
    } 
    else 
    {
      if (isDown()&&d->bActButton&&!d->bDropDown)
        style().drawItem( p, QRect(x2+1, y2+1, w2, h2), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
      else
        style().drawItem( p, QRect(x2, y2, w2, h2), AlignCenter, colorGroup(), TRUE, &pm, QString::null );
    }
  }

  // draw vertical separator line if entered
  if (d->bEntered)
  {
    style().drawPrimitive( QStyle::PE_Separator, p, QRect( width()-19, y2-1, width()-19, y2+h2 ),  colorGroup() );
  }
}

void ToolButtonDropDown::drawArrow( QPainter *p, bool down, int x, int y, int w, int h, 
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

/** Returns the size hint of a normal button plus extra space for the drop down button */
QSize ToolButtonDropDown::sizeHint() const
{
  // take extra space for the drop down area
  QSize s = QToolButton::sizeHint();
  s.setWidth(s.width()+20);
  return s;
}

void ToolButtonDropDown::enterEvent ( QEvent * e )
{
  d->bEntered = true;
  QToolButton::enterEvent(e);
}

void ToolButtonDropDown::leaveEvent(QEvent* e)
{
  d->bEntered = false;
  QToolButton::leaveEvent(e);
}

void ToolButtonDropDown::paintEvent( QPaintEvent *e )
{
  if (d->bEntered)
    d->bEntered = isEnabled();
  QToolButton::paintEvent(e);
}

void ToolButtonDropDown::mousePressEvent( QMouseEvent *e )
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

void ToolButtonDropDown::mouseReleaseEvent( QMouseEvent *e )
{
  if ( e->button() != LeftButton )
    return;

  // check which area is pressed
  if (d->bDropDown && !d->bActButton)
  {
    if (hitButton(e->pos()))
      onPopupWidget();
    setDown(false);
    return;
  }

  QToolButton::mouseReleaseEvent(e);
}

#include "moc_DlgUndoRedo.cpp"

