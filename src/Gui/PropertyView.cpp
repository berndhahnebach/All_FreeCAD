/** \file PropertyView.cpp
 *  \brief Framework to show and work with all kind of properies
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <vector.h>
#	include <qaction.h>
#	include <qbutton.h>
#	include <qbuttongroup.h>
#	include <qcolordialog.h>
#	include <qcombobox.h>
#	include <qcursor.h>
#	include <qguardedptr.h>
#	include <qhbox.h>
#	include <qheader.h>
#	include <qinputdialog.h>
#	include <qlabel.h>
#	include <qlayout.h>
#	include <qlistbox.h>
#	include <qmessagebox.h>
#	include <qtextbrowser.h>
#	include <qobjectlist.h>
#	include <qpainter.h>
#	include <qprocess.h>
#	include <qstyle.h>
#	include <qtabbar.h>
#	include <qtabwidget.h>
#	include <qthread.h>
#	include <qurl.h>
#	include <qvalidator.h>
#	include <qwhatsthis.h>
#	include <qworkspace.h>
#	include <ctype.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Widgets.h"
#include "PropertyView.h"
#include "Tools.h"

#include "Application.h"
#include "HtmlView.h"
#include "BitmapFactory.h"
#include "../Base/Console.h"
#include "../App/Properties.h"

/* XPM */
static const char * resetproperty_xpm[] = {
"7 6 4 1",
" 	c None",
".	c #C00000",
"+	c #FF0000",
"@	c None",
".+@@@+.",
"@.+@+.@",
"@@.+.@@",
"@@+.+@@",
"@+.@.+@",
"+.@@@.+"};


/** Constructor
 */
FCPropertyViewItem::FCPropertyViewItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName)
: QListViewItem( l, after ), listview( l ), property( 0L ), propertyName( propName )
{
  setSelectable( false );
  open = false;
  setText( 0, propertyName );
  changed = false;
  setText( 1, "" );
  resetButton = 0;
  val = 0L;
}

FCPropertyViewItem::~FCPropertyViewItem()
{
  delete val;
  val = NULL;
}

void FCPropertyViewItem::setOpen( bool b )
{
  if ( b == open )
  	return;
  open = b;

  if ( !open ) 
  {
	  children.setAutoDelete( true );
	  children.clear();
	  children.setAutoDelete( false );
	  qApp->processEvents();
	  listview->updateViewSize();

    // hide the view and show it again to avoid a
    // wrong position of the view
    hideView();
    showView();
 	  return;
  }

  createChildren();
  initChildren();
  qApp->processEvents();
  listview->updateViewSize();
}

bool FCPropertyViewItem::isOpen() const
{
  return open;
}

void FCPropertyViewItem::setup()
{
  QListViewItem::setup();
  setHeight( QListViewItem::height() + 2 );
}

bool FCPropertyViewItem::isChanged() const
{
  return changed;
}

void FCPropertyViewItem::setFocus( QWidget *w )
{/*
  QWorkspace *ws = 0;
  QWidget *wid = listview->propertyView();
  while ( wid )
  {
	  if ( wid->inherits( "QWorkspace" ) )
	    break;
  	wid = wid->parentWidget();
  }

  if ( !wid )
	  return;
  ws = (QWorkspace*)wid;
  if ( ws->activeWindow() == listview->propertyView() )
  	w->setFocus();*/
}

QString FCPropertyViewItem::name() const
{
  return propertyName;
}

void FCPropertyViewItem::setPropertyParent (FCPropertyViewItem * parent)
{
  property = parent;
}

FCPropertyViewItem *FCPropertyViewItem::propertyParent() const
{
  return property;
}

void FCPropertyViewItem::notify()
{
  if ( !propertyParent() ) 
  {
	  listview->valueChanged( this );
	  setChanged( true );
	  if ( hasSubItems())
	    initChildren();
  } 
  else 
  {
	  propertyParent()->childValueChanged( this );
	  setChanged( true );
  }
}

void FCPropertyViewItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
  QColorGroup g( cg );
  g.setColor( QColorGroup::Base, backgroundColor() );
  g.setColor( QColorGroup::Foreground, Qt::black );
  g.setColor( QColorGroup::Text, Qt::black );
  int indent = 0;
  if ( column == 0 ) 
  {
	  indent = 20 + ( property ? 20 : 0 );
    int ii = height();
	  p->fillRect( 0, 0, width, height(), backgroundColor() );
	  p->save();
	  p->translate( indent, 0 );
  }

  if ( isChanged() && column == 0 ) 
  {
	  p->save();
	  QFont f = p->font();
	  f.setBold( true );
	  p->setFont( f );
  }

  if ( !hasCustomContents() || column != 1 ) 
  {
  	QListViewItem::paintCell( p, g, column, width - indent, align  );
  } 
  else 
  {
	  p->fillRect( 0, 0, width, height(), backgroundColor() );
	  drawCustomContents( p, QRect( 0, 0, width, height() ) );
  }

  if ( isChanged() && column == 0 )
		p->restore();
  if ( column == 0 )
		p->restore();

  if ( hasSubItems() && column == 0 ) 
  {
		p->save();
		p->setPen( cg.foreground() );
		p->setBrush( cg.base() );
		p->drawRect( 5, height() / 2 - 4, 9, 9 );
		p->drawLine( 7, height() / 2, 11, height() / 2 );
		if ( !isOpen() )
			p->drawLine( 9, height() / 2 - 2, 9, height() / 2 + 2 );
		p->restore();
  }

  p->save();
  p->setPen( QPen( cg.dark(), 1 ) );
  p->drawLine( 0, height() - 1, width, height() - 1 );
  p->drawLine( width - 1, 0, width - 1, height() );
  p->restore();

  if ( listview->currentItem() == this && column == 0 && !listview->hasFocus() && !listview->viewport()->hasFocus() )
  	paintFocus( p, cg, QRect( 0, 0, width, height() ) );
}

#if QT_VERSION < 300
void FCPropertyViewItem::paintBranches( QPainter * p, const QColorGroup & cg, int w, int y, int h, GUIStyle s )
{
  QColorGroup g( cg );
  g.setColor( QColorGroup::Base, backgroundColor() );
  QListViewItem::paintBranches( p, g, w, y, h, s );
}
#else
void FCPropertyViewItem::paintBranches( QPainter * p, const QColorGroup & cg, int w, int y, int h )
{
  QColorGroup g( cg );
  g.setColor( QColorGroup::Base, backgroundColor() );
  QListViewItem::paintBranches( p, g, w, y, h );
}
#endif

void FCPropertyViewItem::paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r )
{
  p->save();
#if QT_VERSION < 300
  QApplication::style().drawPanel( p, r.x(), r.y(), r.width(), r.height(), cg, true, 1 );
#else
  QApplication::style().drawPrimitive(QStyle::PE_Panel, p, r, cg, QStyle::Style_Sunken, QStyleOption(1,1) );
#endif
  p->restore();
}

void FCPropertyViewItem::updateBackColor()
{
  if ( itemAbove() && this != listview->firstChild() ) 
  {
  	if ( ( ( FCPropertyViewItem*)itemAbove() )->backColor == FCPropertyView::cBackColor1 )
	    backColor = FCPropertyView::cBackColor2;
	  else
	    backColor = FCPropertyView::cBackColor1;
  } 
  else 
  {
  	backColor = FCPropertyView::cBackColor1;
  }

  if ( listview->firstChild() == this )
  	backColor = FCPropertyView::cBackColor1;
}

void FCPropertyViewItem::setValue( FCProperty* v )
{
  updateItem ( v );
  if (val != v)
  {
    delete val;
    val = v;
  }
}

FCProperty* FCPropertyViewItem::value() const
{
  return val;
}

void FCPropertyViewItem::setChanged( bool b, bool )
{
  if ( propertyParent() )
  	return;
  if ( changed == b )
  	return;
    changed = b;
  repaint();
  updateResetButtonState();	
}

bool FCPropertyViewItem::hasCustomContents() const
{
  return false;
}

bool FCPropertyViewItem::hasSubItems() const
{
  return false;
}

void FCPropertyViewItem::drawCustomContents( QPainter *, const QRect & )
{
}

void FCPropertyViewItem::toggle()
{
}

void FCPropertyViewItem::showView()
{
  createResetButton();
  resetButton->parentWidget()->show();
}

void FCPropertyViewItem::hideView()
{
  createResetButton();
  resetButton->parentWidget()->hide();
  delete resetButton;
  resetButton = 0L;
}

void FCPropertyViewItem::placeView( QWidget *w )
{
  createResetButton();
  QRect r = listview->itemRect( this );
  if ( r == QRect( 0, 0, -1, -1 ) )
  	listview->ensureItemVisible( this );
  r = listview->itemRect( this );
  r.setX( listview->header()->sectionPos( 1 ) );
  r.setWidth( listview->header()->sectionSize( 1 ) - 1 );
  r.setWidth( r.width() - resetButton->width() - 2 );
  r = QRect( listview->viewportToContents( r.topLeft() ), r.size() );
  w->resize( r.size() );
  int posX = r.x();
  int posY = r.y();
  listview->moveChild( w, posX, posY );
  resetButton->parentWidget()->resize( resetButton->sizeHint().width() + 10, r.height() );
  listview->moveChild( resetButton->parentWidget(), r.x() + r.width() - 8, r.y() );
  resetButton->setFixedHeight( r.height() - 3 );
}

QString FCPropertyViewItem::currentItem() const
{
  return QString::null;
}

int FCPropertyViewItem::currentIntItem() const
{
  return -1;
}

void FCPropertyViewItem::setCurrentItem( const QString & )
{
}

void FCPropertyViewItem::setCurrentItem( int )
{
}

void FCPropertyViewItem::addChild( FCPropertyViewItem *i )
{
  i->setPropertyParent(this);
  children.append( i );
}

int FCPropertyViewItem::childCount() const
{
  return children.count();
}

FCPropertyViewItem *FCPropertyViewItem::child( int i ) const
{
  return ( (FCPropertyViewItem*)this )->children.at( i );
}

void FCPropertyViewItem::createChildren()
{
}

void FCPropertyViewItem::initChildren()
{
}

void FCPropertyViewItem::childValueChanged( FCPropertyViewItem * )
{
}

void FCPropertyViewItem::activate ()
{
//	puts("Activated");
}

QColor FCPropertyViewItem::backgroundColor()
{
  updateBackColor();
  return backColor;
}

void FCPropertyViewItem::createResetButton()
{
  if ( resetButton ) 
  {
	  resetButton->parentWidget()->lower();
	  return;
  }

  QHBox *hbox = new QHBox( listview->viewport() );
  hbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  hbox->setLineWidth( 1 );
  resetButton = new QPushButton( hbox );
  resetButton->setPixmap( resetproperty_xpm );
  resetButton->setFixedWidth( resetButton->sizeHint().width() );
  hbox->layout()->setAlignment( Qt::AlignRight );
  listview->addChild( hbox );
  hbox->hide();
  QObject::connect( resetButton, SIGNAL( clicked() ), listview, SLOT( resetProperty() ) );
  QToolTip::add( resetButton, FCPropertyView::tr( "Reset property to its default value" ) );
  QWhatsThis::add( resetButton, FCPropertyView::tr( "Click this button to reset the property to its default value" ) );
  FCWhatsThis::add( resetButton, FCPropertyView::tr( "Click this button to reset the property to its default value" ) );
  updateResetButtonState();
}

void FCPropertyViewItem::updateResetButtonState()
{
  if ( !resetButton )
  	return;
  if ( propertyParent() )
	  resetButton->setEnabled( false );
  else
	  resetButton->setEnabled( isChanged() );
}

//void FCPropertyViewItem::Update(void)
//{
//	puts("Update");
//}
//
//void FCPropertyViewItem::BuildUp(void)
//{
//}

FCPropertyViewItem* FCPropertyViewItem::createPropertyItem( FCProperty* v, const char* propName )
{
  if (!v) return 0L;

  FCPropertyViewItem* item=0L;
  const char* type = v->GetType();

  if (strcmp(type, "Int") == 0)
    item = new FCPropertyViewIntItem(listview, this, propName, true);
  else if (strcmp(type, "UInt") == 0)
    item = new FCPropertyViewIntItem(listview, this, propName, false);
  else if (strcmp(type, "Float") == 0)
    item = new FCPropertyViewFloatItem(listview, this, propName, true);
  else if (strcmp(type, "String") == 0)
    item = new FCPropertyViewTextItem(listview, this, propName, false, false);
  else if (strcmp(type, "StringList") == 0)
    item = new FCPropertyViewListItem(listview, this, propName, false);
  else if (strcmp(type, "Bool") == 0)
    item = new FCPropertyViewBoolItem(listview, this, propName);
  else if (strcmp(type, "Color") == 0)
    item = new FCPropertyViewColorItem(listview, this, propName, true);
//  else if (strcmp(type, "Coord") == 0)
//    item = new FCPropertyViewCoordItem(listview, this, propName);

  if (item)
    item->setValue(v);

  return item;
}

////////////////////////////////////////////////////////////////////////

FCPropertyViewIntItem::FCPropertyViewIntItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, bool s)
  : FCPropertyViewItem( l, after, propName ), signedValue( s ) 
{
  spinBox = 0L;
}

FCPropertyViewIntItem::~FCPropertyViewIntItem()
{
  delete (FCSpinBox*)spinBox;
  spinBox = 0L;
}

void FCPropertyViewIntItem::updateItem( FCProperty* v )
{
  if ( ( !hasSubItems() || !isOpen() ) && value() == v )
  	return;

  FCPropertyInteger* prop = dynamic_cast<FCPropertyInteger*>(v);
  assert(prop);

  if ( spinBox ) 
  {
	  getSpinBox()->blockSignals( true );
	  if ( signedValue )
      getSpinBox()->setValue( prop->GetValue() );
	  else
      getSpinBox()->setValue( FCmax<long>(0,prop->GetValue()) );
	  getSpinBox()->blockSignals( false );
  }

  if ( signedValue )
    setText( 1, prop->GetAsString () );
  else
    setText( 1, QString::number( FCmax<long>(0,prop->GetValue()) ) );
}

void FCPropertyViewIntItem::showView()
{
  FCPropertyInteger* prop = dynamic_cast<FCPropertyInteger*>(value());
  assert(prop);

  FCPropertyViewItem::showView();
  if ( !spinBox ) 
  {
  	getSpinBox()->blockSignals( true );
	  if ( signedValue )
	    getSpinBox()->setValue( prop->GetValue() );
  	else
	    getSpinBox()->setValue( FCmax<long>(0,prop->GetValue()) );
	  getSpinBox()->blockSignals( false );
  }

  placeView( getSpinBox() );
  if ( !getSpinBox()->isVisible()  || !getSpinBox()->hasFocus()  ) 
  {
    getSpinBox()->show();
    setFocus( getSpinBox() );
  }
}

void FCPropertyViewIntItem::hideView()
{
  FCPropertyViewItem::hideView();
  getSpinBox()->hide();
  delete spinBox;
  spinBox = 0L;
}

void FCPropertyViewIntItem::onSetValue()
{
  FCPropertyInteger* prop = dynamic_cast<FCPropertyInteger*>(value());
  assert(prop);

  if ( !spinBox )
  	return;
  setText( 1, QString::number( getSpinBox()->value() ) );
  if ( signedValue )
  	prop->SetValue( getSpinBox()->value() );
  else
	  prop->SetValue ( (uint)getSpinBox()->value() );

  notify();
}

QSpinBox *FCPropertyViewIntItem::getSpinBox()
{
  if ( spinBox )
	  return spinBox;
  if ( signedValue )
	  spinBox = new FCSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
  else
	  spinBox = new FCSpinBox( 0, INT_MAX, 1, listview->viewport() );
  spinBox->hide();
  spinBox->installEventFilter( listview );
  QObjectList *ol = spinBox->queryList( "QLineEdit" );
  if ( ol && ol->first() )
  	ol->first()->installEventFilter( listview );
  delete ol;
  connect( spinBox, SIGNAL( valueChanged( int ) ), this, SLOT( onSetValue() ) );
  return spinBox;
}

////////////////////////////////////////////////////////////////////////

FCPropertyViewFloatItem::FCPropertyViewFloatItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, bool s)
  : FCPropertyViewItem( l, after, propName ), signedValue( s ) 
{
  spinBox = 0L;
}

FCPropertyViewFloatItem::~FCPropertyViewFloatItem()
{
  delete (FCFloatSpinBox*)spinBox;
  spinBox = 0L;
}

void FCPropertyViewFloatItem::updateItem( FCProperty* v )
{
  if ( ( !hasSubItems() || !isOpen() ) && value() == v )
  	return;

  FCPropertyFloat* prop = dynamic_cast<FCPropertyFloat*> (v);
  assert(prop);

  if ( spinBox ) 
  {
	  getSpinBox()->blockSignals( true );
	  if ( signedValue )
      getSpinBox()->setValueFloat( prop->GetValue() );
	  else
      getSpinBox()->setValueFloat( FCmax<double>(0.0,prop->GetValue()) );
	  getSpinBox()->blockSignals( false );
  }

  if ( signedValue )
    setText( 1, prop->GetAsString() );
  else
    setText( 1, QString::number( FCmax<double>(0.0,prop->GetValue()) ) );
}

void FCPropertyViewFloatItem::showView()
{
  FCPropertyFloat* prop = dynamic_cast<FCPropertyFloat*> (value());
  assert(prop);

  FCPropertyViewItem::showView();
  if ( !spinBox ) 
  {
  	getSpinBox()->blockSignals( true );
	  if ( signedValue )
	    getSpinBox()->setValueFloat( prop->GetValue() );
  	else
	    getSpinBox()->setValueFloat( FCmax<double>(0.0,prop->GetValue()) );
	  getSpinBox()->blockSignals( false );
  }

  placeView( getSpinBox() );
  if ( !getSpinBox()->isVisible()  || !getSpinBox()->hasFocus()  ) 
  {
    getSpinBox()->show();
    setFocus( getSpinBox() );
  }
}

void FCPropertyViewFloatItem::hideView()
{
  FCPropertyViewItem::hideView();
  getSpinBox()->hide();
  delete spinBox;
  spinBox = 0L;
}

void FCPropertyViewFloatItem::onSetValue()
{
  FCPropertyFloat* prop = dynamic_cast<FCPropertyFloat*>(value());
  assert(prop);

  if ( !spinBox )
  	return;
  setText( 1, QString::number( getSpinBox()->valueFloat() ) );
  if ( signedValue )
	  prop->SetValue( getSpinBox()->valueFloat() );
  else
	  prop->SetValue( FCmax<double>(0.0, getSpinBox()->valueFloat()) );

  notify();
}

FCFloatSpinBox *FCPropertyViewFloatItem::getSpinBox()
{
  if ( spinBox )
	  return spinBox;
  if ( signedValue )
	  spinBox = new FCFloatSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
  else
	  spinBox = new FCFloatSpinBox( 0, INT_MAX, 1, listview->viewport() );
  spinBox->setDecimals(2);
  spinBox->hide();
  spinBox->installEventFilter( listview );
  QObjectList *ol = spinBox->queryList( "QLineEdit" );
  if ( ol && ol->first() )
  	ol->first()->installEventFilter( listview );
  delete ol;
  connect( spinBox, SIGNAL( valueFloatChanged( double ) ), this, SLOT( onSetValue() ) );
  return spinBox;
}

/////////////////////////////////////////////////////////////////////////////

FCPropertyViewBoolItem::FCPropertyViewBoolItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName )
    : FCPropertyViewItem( l, after, propName )
{
  comb = 0;
}

FCPropertyViewBoolItem::~FCPropertyViewBoolItem()
{
  delete (QComboBox*)comb;
  comb = 0;
}

void FCPropertyViewBoolItem::updateItem( FCProperty* v )
{
  if ( ( !hasSubItems() || !isOpen() ) && value() == v )
  	return;

  FCPropertyBool* prop = dynamic_cast<FCPropertyBool*> (v);
  assert(prop);

  if ( comb ) 
  {
  	combo()->blockSignals( true );
	  if ( prop->GetValue() )
	    combo()->setCurrentItem( 1 );
	  else
	    combo()->setCurrentItem( 0 );
	  combo()->blockSignals( false );
  }

  setText( 1, prop->GetAsString() );
}

void FCPropertyViewBoolItem::toggle()
{
  FCPropertyBool* prop = dynamic_cast<FCPropertyBool*> (value());
  assert(prop);

  prop->SetValue(!prop->GetValue());
  onSetValue();
}

void FCPropertyViewBoolItem::showView()
{
  FCPropertyBool* prop = dynamic_cast<FCPropertyBool*> (value());
  assert(prop);

  FCPropertyViewItem::showView();
  if ( !comb ) 
  {
  	combo()->blockSignals( true );
	  if ( prop->GetValue() )
	    combo()->setCurrentItem( 1 );
	  else
	    combo()->setCurrentItem( 0 );
	  combo()->blockSignals( false );
  }

  placeView( combo() );
  if ( !combo()->isVisible()  || !combo()->hasFocus() ) 
  {
    combo()->show();
    setFocus( combo() );
  }
}

void FCPropertyViewBoolItem::hideView()
{
  FCPropertyViewItem::hideView();
  combo()->hide();
  delete comb;
  comb = 0L;
}

void FCPropertyViewBoolItem::onSetValue()
{
  FCPropertyBool* prop = dynamic_cast<FCPropertyBool*> (value());
  assert(prop);

  if ( !comb )
  	return;
  setText( 1, combo()->currentText() );
  bool b = combo()->currentItem() == 1;
  prop->SetValue(b);
  notify();
}

QComboBox *FCPropertyViewBoolItem::combo()
{
  if ( comb )
  	return comb;
  comb = new QComboBox( false, listview->viewport() );
  comb->hide();
  comb->insertItem( tr( "False" ) );
  comb->insertItem( tr( "True" ) );
  connect( comb, SIGNAL( activated( int ) ), this, SLOT( onSetValue() ) );
  comb->installEventFilter( listview );
  return comb;
}

/////////////////////////////////////////////////////////////////////////////

FCPropertyViewTextItem::FCPropertyViewTextItem( FCPropertyListView *l, FCPropertyViewItem *after,
				    const char* propName, bool comment, bool multiLine )
  : FCPropertyViewItem( l, after, propName ), withComment( comment ),
    hasMultiLines( multiLine )
{
  lin = 0;
  box = 0;
}

FCPropertyViewTextItem::~FCPropertyViewTextItem()
{
  delete (QLineEdit*)lin;
  lin = 0;
  delete (QHBox*)box;
  box = 0;
}

void FCPropertyViewTextItem::updateItem( FCProperty* v )
{
  if ( ( !hasSubItems() || !isOpen() ) && value() == v )
  	return;

  FCPropertyString* prop = dynamic_cast<FCPropertyString*>(v);
  assert(prop);

  if ( lin ) 
  {
	  lined()->blockSignals( true );
	  int oldCursorPos;
	  oldCursorPos = lin->cursorPosition();
	  lined()->setText( prop->GetString() );
	  lin->setCursorPosition( oldCursorPos );
	  lined()->blockSignals( false );
  }

  setText( 1, prop->GetString() );
}

void FCPropertyViewTextItem::setChanged( bool b, bool updateDb )
{
  FCPropertyViewItem::setChanged( b, updateDb );
  if ( withComment && childCount() > 0 )
  	( (FCPropertyViewTextItem*)FCPropertyViewItem::child( 0 ) )->lined()->setEnabled( b );
}

bool FCPropertyViewTextItem::hasSubItems() const
{
  return withComment;
}

void FCPropertyViewTextItem::showView()
{
  FCPropertyViewItem::showView();
  if ( !lin ) 
  {
    FCPropertyString* prop = dynamic_cast<FCPropertyString*>(value());
    assert(prop);

	  lined()->blockSignals( true );
	  lined()->setText( prop->GetString() );
	  lined()->blockSignals( false );
  }

  QWidget* w;
  if ( hasMultiLines )
	  w = box;
  else
	  w= lined();

  placeView( w );
  if ( !w->isVisible() || !lined()->hasFocus() ) 
  {
	  w->show();
	  setFocus( lined() );
  }
}

void FCPropertyViewTextItem::hideView()
{
  FCPropertyViewItem::hideView();
  QWidget* w;
  if ( hasMultiLines )
  	w = box;
  else
	  w = lined();

  w->hide();
  delete lin;
  lin = 0L;
}

void FCPropertyViewTextItem::createChildren()
{
  FCPropertyViewTextItem *i = new FCPropertyViewTextItem( listview, this, tr( "comment" ), false, false );
  i->lined()->setEnabled( isChanged() );
  addChild( i );
}

void FCPropertyViewTextItem::initChildren()
{
  if ( !childCount() )
  	return;
  FCPropertyViewItem *item = FCPropertyViewItem::child( 0 );
}

void FCPropertyViewTextItem::childValueChanged( FCPropertyViewItem *child )
{
}

void FCPropertyViewTextItem::onSetValue()
{
  FCPropertyString* prop = dynamic_cast<FCPropertyString*>(value());
  assert(prop);

  setText( 1, lined()->text() );
  prop->SetString(lined()->text().latin1());
  notify();
}

void FCPropertyViewTextItem::getText()
{
  QString txt = QInputDialog::getText(tr("Text"), tr("Text input"));
  if ( !txt.isEmpty() ) 
  {
	  setText( 1, txt );
    FCPropertyString* prop = dynamic_cast<FCPropertyString*> (value());
    assert(prop);

    prop->SetString(txt.latin1());
	  notify();
	  lined()->blockSignals( true );
	  lined()->setText( txt );
	  lined()->blockSignals( false );
  }
}

QLineEdit *FCPropertyViewTextItem::lined()
{
  if ( lin )
  	return lin;
  if ( hasMultiLines ) 
  {
	  box = new QHBox( listview->viewport() );
	  box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	  box->setLineWidth( 2 );
	  box->hide();
  }

  lin = 0;
  if ( hasMultiLines )
  	lin = new QLineEdit( box );
  else
  	lin = new QLineEdit( listview->viewport() );

  if ( !hasMultiLines ) 
  {
  	lin->hide();
  } 
  else 
  {
	  button = new QPushButton( tr("..."), box );
	  button->setFixedWidth( 20 );
	  connect( button, SIGNAL( clicked() ), this, SLOT( getText() ) );
  	lin->setFrame( false );
  }

  connect( lin, SIGNAL( returnPressed() ), this, SLOT( onSetValue() ) );
  connect( lin, SIGNAL( textChanged( const QString & ) ), this, SLOT( onSetValue() ) );
  if ( FCPropertyViewItem::name() == "name" )
    lin->installEventFilter( listview );
  return lin;
}

/////////////////////////////////////////////////////////////////////////////
/*
FCPropertyViewCoordItem::FCPropertyViewCoordItem( FCPropertyListView *l, FCPropertyViewItem *after, 
				    const char* propName )
  : FCPropertyViewItem( l, after, propName )
{
  lin = 0;
}

FCPropertyViewCoordItem::~FCPropertyViewCoordItem()
{
  delete (QLineEdit*)lin;
  lin = 0;
}

void FCPropertyViewCoordItem::setValue( FCProperty* v )
{
  if ( ( !hasSubItems() || !isOpen() ) && value() == v )
  	return;

  FCPropertyViewItem *item = 0;
  QString s;

  s = "[ ";
//  for (int i=0; i < childCount(); ++i)
//  {
//	  item = FCPropertyViewItem::child( i );
//    s = s + QString::number (item->value().toDouble());
//    if (i < childCount() - 1)
//      s = s + ", ";
//  }
  QValueList<QVariant> list = v.toList();
  int i = 0;
  for (QValueList<QVariant>::ConstIterator it = list.begin(); it!= list.end(); ++it)
  {
    s = s + QString::number ((*it).toDouble());
    if (i++ < (int)list.count() - 1)
      s = s + ", ";
  }

  s = s + " ]";
  
  setText( 1, s );
  if ( lin )
	  lined()->setText( s );
  FCPropertyViewItem::setValue( v );
}

bool FCPropertyViewCoordItem::hasSubItems() const
{
  return true;
}

void FCPropertyViewCoordItem::showView()
{
  FCPropertyViewItem::showView();
  if ( !lin )
  	lined()->setText( text( 1 ) );
  placeView( lined() );
  if ( !lined()->isVisible() || !lined()->hasFocus()  ) 
  {
	  lined()->show();
	  setFocus( lined() );
  }
}

void FCPropertyViewCoordItem::hideView()
{
  FCPropertyViewItem::hideView();
  lined()->hide();
  delete lin; lin = 0L;
}

void FCPropertyViewCoordItem::createChildren()
{
  QValueList<QVariant> list = val.toList();
  FCPropertyViewItem *item = this;

  if (list.count() > 3)
  {
    for (int i = 0; i < (int)list.count(); ++i)
    {
      item = new FCPropertyViewFloatItem( listview, item, tr( "x%1" ).arg(i+1), true );
  	  addChild( item );
    }
  }
  else if (list.count() > 1)
  {
    item = new FCPropertyViewFloatItem( listview, item, tr( "x" ), true );
	  addChild( item );
	  item = new FCPropertyViewFloatItem( listview, item, tr( "y" ), true );
	  addChild( item );
    if (list.count() > 2)
    {
	    item = new FCPropertyViewFloatItem( listview, item, tr( "z" ), true );
	    addChild( item );
    }
  }
}

void FCPropertyViewCoordItem::initChildren()
{
  QValueList<QVariant> list = val.toList();
  FCPropertyViewItem *item = 0;

  QValueList<QVariant>::ConstIterator it;
  int i=0;
  for ( it=list.begin(); it != list.end(); ++it ) 
  {
    if (i >= childCount())
      break;
	  item = FCPropertyViewItem::child( i++ );
		item->setValue( *it );
  }
}

void FCPropertyViewCoordItem::childValueChanged( FCPropertyViewItem *child )
{
//  QValueList<QVariant> list;
//
//  FCPropertyViewItem *item = 0;
//  for ( int i = 0; i < childCount(); ++i ) 
//  {
//    item = FCPropertyViewItem::child( i );
//	  list.append(item->value());
//  }
//
//  setValue( list );
//   
//  notify();
}

QLineEdit *FCPropertyViewCoordItem::lined()
{
  if ( lin )
  	return lin;
  lin = new QLineEdit( listview->viewport() );
  lin->setReadOnly( true );
  lin->installEventFilter( listview );
  lin->hide();
  return lin;
}
*/
/////////////////////////////////////////////////////////////////////////////

FCPropertyViewListItem::FCPropertyViewListItem( FCPropertyListView *l, FCPropertyViewItem *after, 
				    const char* propName, bool e )
  : FCPropertyViewItem( l, after, propName ), editable( e )
{
  comb = 0;
  oldInt = -1;
}

FCPropertyViewListItem::~FCPropertyViewListItem()
{
  delete (QComboBox*)comb;
  comb = 0;
}

void FCPropertyViewListItem::updateItem( FCProperty* v )
{
  FCPropertyList* prop = dynamic_cast<FCPropertyList*> (v);
  assert(prop);

  QStringList list;
  const std::vector<std::string>& rList = prop->GetValue();
  if ( comb ) 
  {
    for (std::vector<std::string>::const_iterator it = rList.begin(); it!=rList.end(); ++it)
      list.append(it->c_str());
	  combo()->blockSignals( true );
	  combo()->clear();
	  combo()->insertStringList( list );
	  combo()->blockSignals( false );
  }

  setText( 1, rList.front().c_str() );
}

void FCPropertyViewListItem::showView()
{
  FCPropertyList* prop = dynamic_cast<FCPropertyList*> (value());
  assert(prop);

  FCPropertyViewItem::showView();
  if ( !comb ) 
  {
    QStringList list;
    const std::vector<std::string>& rList = prop->GetValue();
    for (std::vector<std::string>::const_iterator it = rList.begin(); it!=rList.end(); ++it)
      list.append(it->c_str());
	  combo()->blockSignals( true );
	  combo()->clear();
	  combo()->insertStringList( list );
	  combo()->blockSignals( false );
  }

  placeView( combo() );
  if ( !combo()->isVisible()  || !combo()->hasFocus() ) 
  {
	  combo()->show();
    combo()->setCurrentItem(prop->GetCurrentItem());
	  setFocus( combo() );
  }
}

void FCPropertyViewListItem::hideView()
{
  FCPropertyList* prop = dynamic_cast<FCPropertyList*> (value());
  assert(prop);

  FCPropertyViewItem::hideView();
  prop->SetCurrentItem(combo()->currentItem());
  combo()->hide();
  delete comb;
  comb = 0L;
}

QString FCPropertyViewListItem::currentItem() const
{
  return ( (FCPropertyViewListItem*)this )->combo()->currentText();
}

void FCPropertyViewListItem::setCurrentItem( const QString &s )
{
  if ( comb && currentItem().lower() == s.lower() )
   	return;

  FCPropertyList* prop = dynamic_cast<FCPropertyList*> (value());
  assert(prop);

  if ( !comb ) 
  {
    QStringList list;
    const std::vector<std::string>& rList = prop->GetValue();
    for (std::vector<std::string>::const_iterator it = rList.begin(); it!=rList.end(); ++it)
      list.append(it->c_str());
	  combo()->blockSignals( true );
	  combo()->clear();
	  combo()->insertStringList( list );
	  combo()->blockSignals( false );
  }

  for ( uint i = 0; i < combo()->listBox()->count(); ++i ) 
  {
  	if ( combo()->listBox()->item( i )->text().lower() == s.lower() ) 
    {
	    combo()->setCurrentItem( i );
	    setText( 1, combo()->currentText() );
	    break;
  	}
  }
  oldInt = currentIntItem();
  oldString = currentItem();
}

void FCPropertyViewListItem::setCurrentItem( int i )
{
  if ( comb && i == combo()->currentItem() )
  	return;

  FCPropertyList* prop = dynamic_cast<FCPropertyList*> (value());
  assert(prop);

  if ( !comb ) 
  {
    QStringList list;
    const std::vector<std::string>& rList = prop->GetValue();
    for (std::vector<std::string>::const_iterator it = rList.begin(); it!=rList.end(); ++it)
      list.append(it->c_str());
	  combo()->blockSignals( true );
	  combo()->clear();
	  combo()->insertStringList( list );
	  combo()->blockSignals( false );
  }

  combo()->setCurrentItem( i );
  setText( 1, combo()->currentText() );
  oldInt = currentIntItem();
  oldString = currentItem();
}

int FCPropertyViewListItem::currentIntItem() const
{
  return ( (FCPropertyViewListItem*)this )->combo()->currentItem();
}

void FCPropertyViewListItem::onSetValue()
{
  if ( !comb )
  	return;
  setText( 1, combo()->currentText() );
  std::vector<std::string> lst;
  for ( uint i = 0; i < combo()->listBox()->count(); ++i )
  	lst.push_back(combo()->listBox()->item( i )->text().latin1());
  FCPropertyList* prop = dynamic_cast<FCPropertyList*> (value());
  assert(prop);

  prop->SetValue(lst);
  notify();
  oldInt = currentIntItem();
  oldString = currentItem();
}

QComboBox *FCPropertyViewListItem::combo()
{
  if ( comb )
  	return comb;
  comb = new QComboBox( editable, listview->viewport() );
  comb->hide();
  connect( comb, SIGNAL( activated( int ) ), this, SLOT( onSetValue() ) );
  comb->installEventFilter( listview );
  if ( editable ) 
  {
	  QObjectList *ol = comb->queryList( "QLineEdit" );
	  if ( ol && ol->first() )
	    ol->first()->installEventFilter( listview );
  	delete ol;
  }

  return comb;
}

/////////////////////////////////////////////////////////////////////////////

FCPropertyViewColorItem::FCPropertyViewColorItem( FCPropertyListView *l, FCPropertyViewItem *after,
				      const char* propName, bool children )
  : FCPropertyViewItem( l, after, propName ), withChildren( children )
{
  boxColor();
}

FCPropertyViewColorItem::~FCPropertyViewColorItem()
{
  delete (QHBox*)box;
}

void FCPropertyViewColorItem::updateItem( FCProperty* v )
{
  if ( ( !hasSubItems() || !isOpen() ) && value() == v )
  	return;

  FCPropertyColor* prop = dynamic_cast<FCPropertyColor*> (v);
  assert(prop);

  QString s;
  setText( 1, QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()).name() );
  boxColor();
  colorPrev->setBackgroundColor( QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()) );
}

bool FCPropertyViewColorItem::hasCustomContents() const
{
  return true;
}

bool FCPropertyViewColorItem::hasSubItems() const
{
  return withChildren;
}

void FCPropertyViewColorItem::drawCustomContents( QPainter *p, const QRect &r )
{
  FCPropertyColor* prop = dynamic_cast<FCPropertyColor*> (value());
  assert(prop);

  p->save();
  p->setPen( QPen( black, 1 ) );
  p->setBrush( QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()) );
  p->drawRect( r.x() + 2, r.y() + 2, r.width() - 5, r.height() - 5 );
  p->restore();
}

void FCPropertyViewColorItem::showView()
{
  FCPropertyViewItem::showView();
  placeView( boxColor() );
  if ( !box->isVisible() )
  	box->show();
}

void FCPropertyViewColorItem::hideView()
{
  FCPropertyViewItem::hideView();
  box->hide();
  delete box;
  box = 0L;
}

void FCPropertyViewColorItem::createChildren()
{
  FCPropertyViewItem *i = this;
  i = new FCPropertyViewIntItem( listview, i, tr( "Red" ), true );
  addChild( i );
  i = new FCPropertyViewIntItem( listview, i, tr( "Green" ), true );
  addChild( i );
  i = new FCPropertyViewIntItem( listview, i, tr( "Blue" ), true );
  addChild( i );
}

void FCPropertyViewColorItem::initChildren()
{
  FCPropertyViewItem *item = 0;
  FCPropertyColor* prop = dynamic_cast<FCPropertyColor*> (value());
  assert(prop);

  for ( int i = 0; i < childCount(); ++i ) 
  {
  	item = FCPropertyViewItem::child( i );
	  if ( item->name() == tr( "Red" ) )
	    item->setValue( new FCPropertyInteger(prop->GetRed()) );
  	else if ( item->name() == tr( "Green" ) )
	    item->setValue( new FCPropertyInteger(prop->GetGreen()) );
	  else if ( item->name() == tr( "Blue" ) )
	    item->setValue( new FCPropertyInteger(prop->GetBlue()) );
  }
}

void FCPropertyViewColorItem::childValueChanged( FCPropertyViewItem *child )
{
  FCPropertyColor* prop = dynamic_cast<FCPropertyColor*> (value());
  assert(prop);

  QColor c( QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()) );
  if ( child->name() == tr( "Red" ) )
	  c.setRgb( ((FCPropertyInteger*)child->value())->GetValue(), c.green(), c.blue() );
  else if ( child->name() == tr( "Green" ) )
	  c.setRgb( c.red(), ((FCPropertyInteger*)child->value())->GetValue(), c.blue() );
  else if ( child->name() == tr( "Blue" ) )
	  c.setRgb( c.red(), c.green(), ((FCPropertyInteger*)child->value())->GetValue() );

  prop->SetRed(c.red());
  prop->SetGreen(c.green());
  prop->SetBlue(c.blue());
  updateItem( prop );
  notify();
}

void FCPropertyViewColorItem::getColor()
{
  FCPropertyColor* prop = dynamic_cast<FCPropertyColor*> (value());
  assert(prop);

  QColor c = QColorDialog::getColor( QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()), listview );
  if ( c.isValid() ) 
  {
    prop->SetRed(c.red());
    prop->SetGreen(c.green());
    prop->SetBlue(c.blue());
    boxColor();
    colorPrev->setBackgroundColor( QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()) );
	  updateItem( prop );
	  notify();
  }
}

QHBox* FCPropertyViewColorItem::boxColor()
{
  if (box)
    return box;

  box = new QHBox( listview->viewport() );
  box->hide();
  colorPrev = new QFrame( box );
  button = new QPushButton( "...", box );
  button->setFixedWidth( 20 );
  box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  box->setLineWidth( 2 );
  colorPrev->setFrameStyle( QFrame::Plain | QFrame::Box );
  colorPrev->setLineWidth( 2 );
  QPalette pal = colorPrev->palette();
  QColorGroup cg = pal.active();
  cg.setColor( QColorGroup::Foreground, cg.color( QColorGroup::Base ) );
  pal.setActive( cg );
  pal.setInactive( cg );
  pal.setDisabled( cg );
  colorPrev->setPalette( pal );
  box->installEventFilter( listview );
  connect( button, SIGNAL( clicked() ), this, SLOT( getColor() ) );

  FCPropertyColor* prop = (FCPropertyColor*) value();
  if (prop)
    colorPrev->setBackgroundColor( QColor(prop->GetRed(), prop->GetGreen(), prop->GetBlue()) );

  return box;
}

/////////////////////////////////////////////////////////////////////////////

FCPropertyListView::FCPropertyListView( QWidget* parent)
  : QListView( parent )
{
  header()->setMovingEnabled( false );
  viewport()->setBackgroundMode( PaletteBackground );
  setResizePolicy( QScrollView::Manual );
  addColumn( tr( "Property" ) );
  addColumn( tr( "Value" ) );
  connect( header(), SIGNAL( sizeChange( int, int, int ) ), this, SLOT( updateViewSize() ) );
  disconnect( header(), SIGNAL( sectionClicked( int ) ),
	this, SLOT( changeSortColumn( int ) ) );
  connect( this, SIGNAL( pressed( QListViewItem *, const QPoint &, int ) ), this, SLOT( itemPressed( QListViewItem *, const QPoint &, int ) ) );
  connect( this, SIGNAL( doubleClicked( QListViewItem * ) ), this, SLOT( toggleOpen( QListViewItem * ) ) );
  setSorting( -1 );
  setHScrollBarMode( AlwaysOff );


  header()->setMovingEnabled( FALSE );
  header()->setStretchEnabled( TRUE );
  setResizePolicy( QScrollView::Manual );
//  viewport()->setAcceptDrops( TRUE );
  viewport()->installEventFilter( this );
//  addColumn( tr( "Property" ) );
//  addColumn( tr( "Value" ) );
//  setSorting( -1 );
  setColumnWidthMode( 1, QListView::Manual );
  header()->installEventFilter( this );

	FCPropertyViewItem* pcItem=0L;
	
  std::vector<std::string> list;
  list.push_back("10.89");
  list.push_back("-10.7");
  pcItem = new FCPropertyViewColorItem( this, NULL, "Color", true  );
  pcItem->setValue(new FCPropertyColor(255, 0, 0));
	pcItem = new FCPropertyViewIntItem  ( this, pcItem,   "Integer", true );
  pcItem->setValue( new FCPropertyInteger( 37 ) );
  pcItem = new FCPropertyViewFloatItem( this, pcItem, "Float", true );
  pcItem->setValue( new FCPropertyFloat( -3.14 ) );
  pcItem = new FCPropertyViewTextItem ( this, pcItem, "Text", false, false);
  pcItem->setValue( new FCPropertyString( "FreeCAD" ) );
  pcItem = new FCPropertyViewListItem ( this, pcItem, "List", false);
  pcItem->setValue( new FCPropertyList( list ) );
  pcItem = new FCPropertyViewBoolItem ( this, pcItem, "Boolean");
  pcItem->setValue( new FCPropertyBool ( false ) );
}

void FCPropertyListView::resizeEvent( QResizeEvent *e )
{
  QListView::resizeEvent( e );
  QSize vs = viewportSize( 0, contentsHeight() );

  int os = header()->sectionSize( 1 );
  int ns = vs.width() - header()->sectionSize( 0 );
  if ( ns < 16 )
  	ns = 16;
	
  header()->resizeSection( 1, ns );
  header()->repaint( header()->width() - header()->sectionSize( 1 ), 0, header()->sectionSize( 1 ), header()->height() );

  int elipsis = fontMetrics().width("...") + 10;
  viewport()->repaint( header()->sectionPos(1) + os - elipsis, 0, elipsis, viewport()->height(), false );
  if ( currentItem() )
  	( ( FCPropertyViewItem* )currentItem() )->showView();
}

void FCPropertyListView::keyPressEvent ( QKeyEvent * e )
{
  // filter these keys
  switch (e->key())
  {
    case Key_Left:
    case Key_Right:
      return;
  };

  QListView::keyPressEvent(e);
}

void FCPropertyListView::paintEmptyArea( QPainter *p, const QRect &r )
{
  p->fillRect( r, FCPropertyView::cBackColor2 );
}

void FCPropertyListView::setCurrentItem( QListViewItem *i )
{
  if ( currentItem() )
  	( (FCPropertyViewItem*)currentItem() )->hideView();
  QListView::setCurrentItem( i );
  ( (FCPropertyViewItem*)currentItem() )->showView();
}

void FCPropertyListView::updateViewSize()
{
  QSize s( header()->sectionPos(1) + header()->sectionSize(1), height() );
  QResizeEvent e( s, size() );
  resizeEvent( &e );
  viewport()->repaint( s.width(), 0, width() - s.width(), height(), false );
}

void FCPropertyListView::valueChanged( FCPropertyViewItem *i )
{
}

void FCPropertyListView::itemPressed( QListViewItem *i, const QPoint &p, int c )
{
  if ( !i )
	  return;
  FCPropertyViewItem *pi = (FCPropertyViewItem*)i;
  if ( !pi->hasSubItems() )
  	return;

  if ( c == 0 && viewport()->mapFromGlobal( p ).x() < 20 )
  	toggleOpen( i );
}

void FCPropertyListView::toggleOpen( QListViewItem *i )
{
  if ( !i )
  	return;
  FCPropertyViewItem *pi = (FCPropertyViewItem*)i;
  if ( pi->hasSubItems() ) 
  {
  	pi->setOpen( !pi->isOpen() );
  } 
  else 
  {
  	pi->toggle();
  }
}

void FCPropertyListView::resetProperty()
{
  if ( !currentItem() )
  	return;
  FCPropertyViewItem *i = (FCPropertyViewItem*)currentItem();
  if ( i->hasSubItems() )
	  i->initChildren();
}

bool FCPropertyListView::eventFilter( QObject *o, QEvent *e )
{
  if ( !o || !e )
  	return true;

  FCPropertyViewItem *i = (FCPropertyViewItem*)currentItem();
  if ( o != this &&e->type() == QEvent::KeyPress ) 
  {
  	QKeyEvent *ke = (QKeyEvent*)e;
	  if ( ( ke->key() == Key_Up || ke->key() == Key_Down ) &&
	     ( o != this || o != viewport() ) &&
	     !( ke->state() & ControlButton ) ) {
	    QApplication::sendEvent( this, (QKeyEvent*)e );
	    return true;
	  } 
    else if ( ( !o->inherits( "QLineEdit" ) || ( o->inherits( "QLineEdit" ) && ( (QLineEdit*)o )->isReadOnly() ) ) 
                && i && i->hasSubItems() ) 
    {
	    if ( !i->isOpen() && ( ke->key() == Key_Plus || ke->key() == Key_Right ))
    		i->setOpen( true );
	    else if ( i->isOpen() &&  ( ke->key() == Key_Minus || ke->key() == Key_Left ) )
    		i->setOpen( false );
	  } 
    else if ( ( ke->key() == Key_Return || ke->key() == Key_Enter ) && o->inherits( "QComboBox" ) ) 
    {
	    QKeyEvent ke2( QEvent::KeyPress, Key_Space, 0, 0 );
	    QApplication::sendEvent( o, &ke2 );
	    return true;
    }
  }

  return QListView::eventFilter( o, e );
}

// --------------------------------------------------------------

FCPropertyEditor::FCPropertyEditor( FCPropertyView * view  )
    : QTabWidget( view ), propView( view )
{
    setCaption( tr( "Property Editor" ) );
    wid = 0;
    listview = new FCPropertyListView( this );
    addTab( listview, tr( "P&roperties" ) );
//    addTab( eList, tr( "Signa&l Handlers" ) );
}

QObject *FCPropertyEditor::widget() const
{
    return wid;
}

void FCPropertyEditor::clear()
{
    listview->setContentsPos( 0, 0 );
    listview->clear();
}

void FCPropertyEditor::setup()
{
    listview->viewport()->setUpdatesEnabled( FALSE );
//    listview->setupProperties();
    listview->viewport()->setUpdatesEnabled( TRUE );
//    listview->updateEditorSize();
}

void FCPropertyEditor::refetchData()
{
//    listview->refetchData();
}

void FCPropertyEditor::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

FCPropertyListView *FCPropertyEditor::propertyList() const
{
    return listview;
}

QString FCPropertyEditor::currentProperty() const
{
    if ( !wid )
	return QString::null;
    if ( ( (PropertyItem*)listview->currentItem() )->propertyParent() )
	return ( (PropertyItem*)listview->currentItem() )->propertyParent()->name();
    return ( (PropertyItem*)listview->currentItem() )->name();
}

void FCPropertyEditor::resetFocus()
{
    if ( listview->currentItem() )
	( (PropertyItem*)listview->currentItem() )->showEditor();
}

void FCPropertyEditor::setPropertyEditorEnabled( bool b )
{
    if ( !b )
	removePage( listview );
    else
	insertTab( listview, tr( "Property Editor" ), 0 );
    updateWindow();
}

void FCPropertyEditor::setSignalHandlersEnabled( bool b )
{
}

void FCPropertyEditor::updateWindow()
{
    if ( isHidden() && count() ) {
	parentWidget()->show();
//	MainWindow::self->setAppropriate( (QDockWindow*)parentWidget(), TRUE );
    } else if ( isShown() && !count() ) {
	parentWidget()->hide();
//	MainWindow::self->setAppropriate( (QDockWindow*)parentWidget(), FALSE );
    }
}

FCPropertyView *FCPropertyEditor::propertyView() const
{
  return propView;
}

//**************************************************************************
//**************************************************************************
// FCPropertyView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QPixmap* FCPropertyView::pcLabelOpen=0;
QPixmap* FCPropertyView::pcLabelClosed=0;
QPixmap* FCPropertyView::pcAttribute=0;

QColor FCPropertyView::cBackColor1=QColor(250,248,235);
QColor FCPropertyView::cBackColor2=QColor(255,255,255);


//**************************************************************************
// Construction/Destruction

FCPropertyView::FCPropertyView(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:FCDockView(pcDocument,parent,name)
{
  setCaption( tr( "Property View" ) );
  wid = 0;

  _pTabLayout = new QGridLayout( this ); 
  _pTabLayout->setSpacing( 0 );
  _pTabLayout->setMargin ( 0 );

	_pTabWnd = new PropertyEditor( this );
	_pTabWnd->setTabPosition(QTabWidget::Bottom);
	_pTabWnd->setTabShape(QTabWidget::Triangular);
  _pTabLayout->addWidget( _pTabWnd, 0, 0 );

	// set defaults and the colums
//  _pcListView = new FCPropertyListView(_pTabWnd, this/*,name*/);
//  _pcListView = new PropertyList(_pTabWnd/*,name*/);
//	_pcListView->setSorting(-1);
//	_pcListView->addColumn(tr("Property"));
//	_pcListView->setColumnWidthMode(0,QListView::Manual);
//	_pcListView->addColumn(tr("Value"));
//	_pcListView->setColumnWidthMode(1,QListView::Manual );
//	_pcListView->setColumnWidth(0,100);
//	_pcListView->setColumnWidth(1,100);
/*
  _pcListView->header()->setMovingEnabled( FALSE );
  _pcListView->header()->setStretchEnabled( TRUE );
  _pcListView->setResizePolicy( QScrollView::Manual );
  _pcListView->viewport()->setAcceptDrops( TRUE );
  _pcListView->viewport()->installEventFilter( this );
  _pcListView->addColumn( tr( "Property" ) );
  _pcListView->addColumn( tr( "Value" ) );
  _pcListView->setSorting( -1 );
  _pcListView->setColumnWidthMode( 1, QListView::Manual );
  _pcListView->header()->installEventFilter( this );
*/
//	_pTabWnd->insertTab(_pcListView, "Test");

	// retrieve the Pixmaps
	pcLabelOpen   = new QPixmap(GetBitmapFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(GetBitmapFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(GetBitmapFactory().GetPixmap("RawTree_Attr"));

/*
  // some examples how to use the ProprtyViewItem-Framework
  //
  //
	FCPropertyViewItem* pcItem=0L;
	
  std::vector<std::string> list;
  list.push_back("10.89");
  list.push_back("-10.7");
  pcItem = new FCPropertyViewColorItem( _pcListView, NULL, "Color", true  );
  pcItem->setValue(new FCPropertyColor(255, 0, 0));
	pcItem = new FCPropertyViewIntItem  ( _pcListView, pcItem,   "Integer", true );
  pcItem->setValue( new FCPropertyInteger( 37 ) );
  pcItem = new FCPropertyViewFloatItem( _pcListView, pcItem, "Float", true );
  pcItem->setValue( new FCPropertyFloat( -3.14 ) );
  pcItem = new FCPropertyViewTextItem ( _pcListView, pcItem, "Text", false, false);
  pcItem->setValue( new FCPropertyString( "FreeCAD" ) );
  pcItem = new FCPropertyViewListItem ( _pcListView, pcItem, "List", false);
  pcItem->setValue( new FCPropertyList( list ) );
  pcItem = new FCPropertyViewBoolItem ( _pcListView, pcItem, "Boolean");
  pcItem->setValue( new FCPropertyBool ( false ) );
//  pcItem = new FCPropertyViewCoordItem( _pcListView, pcItem, "Coord2D" );
//  pcItem->setValue(list);
//  pcItem = new FCPropertyViewCoordItem( _pcListView, pcItem, "Coord3D" );
//  list.append(2.6);
//  pcItem->setValue(list);
//  pcItem->createPropertyItem(list, "Automatic");
//  list.prepend("Text");
//  pcItem->createPropertyItem(list, "Misc");

  // Add the first main label
//	_pcMainItem = new FCTreeLabel(this);
*/
	//_pcListView->setRootIsDecorated(true);



	//_pcListView->setSize(200,400);
    resize( 200, 400 );

}

FCPropertyView::~FCPropertyView()
{
}

QWidget *FCPropertyView::widget() const
{
  return wid;
}

void FCPropertyView::clear()
{
//  _pcListView->setContentsPos( 0, 0 );
//  _pcListView->clear();
}

void FCPropertyView::setup()
{
//  _pcListView->viewport()->setUpdatesEnabled( FALSE );
//  _pcListView->viewport()->setUpdatesEnabled( TRUE );
//  qApp->processEvents();
//  _pcListView->updateViewSize();
}

void FCPropertyView::resetFocus()
{
//  if ( _pcListView->currentItem() )
//  	( (FCPropertyViewItem*)_pcListView->currentItem() )->showView();
}

FCPropertyListView *FCPropertyView::propertyList() const
{
  return 0;//_pcListView;
}

QString FCPropertyView::currentProperty() const
{
/*  if ( !wid )
  	return QString::null;
  if ( ( (FCPropertyViewItem*)_pcListView->currentItem() )->propertyParent() )
  	return ( (FCPropertyViewItem*)_pcListView->currentItem() )->propertyParent()->name();
  return ( (FCPropertyViewItem*)_pcListView->currentItem() )->name();*/
	return "";
}

void FCPropertyView::Update(void)
{
	GetConsole().Log("Property Updated\n");
}

void FCPropertyView::OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument)
{
}

void FCPropertyView::resizeEvent ( QResizeEvent * e) 
{/*
	// routing the resize event to the child
	_pcListView->resize(e->size());

	QSize vs = _pcListView->viewportSize( 0, _pcListView->contentsHeight() );

	int os = _pcListView->header()->sectionSize( 1 );
  int ns = vs.width() - _pcListView->header()->sectionSize( 0 );
  if ( ns < 16 )
  	ns = 16;
	
  _pcListView->header()->resizeSection( 1, ns );
  _pcListView->header()->repaint( _pcListView->header()->width() - _pcListView->header()->sectionSize( 1 ), 0, _pcListView->header()->sectionSize( 1 ), _pcListView->header()->height() );*/
}

bool FCPropertyView::OnMsg(const char* pMsg)
{
	// no messages yet
	return false;
}

//**************************************************************************
// separator for other implemetation aspects

#include <qvariant.h> // HP-UX compiler needs this here


#include <qpainter.h>
#include <qpalette.h>
#include <qapplication.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qstrlist.h>
#include <qmetaobject.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qfontdialog.h>
#include <qspinbox.h>
#include <qevent.h>
#include <qobjectlist.h>
#include <qlistbox.h>
#include <qfontdatabase.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qsizepolicy.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaccel.h>
#include <qworkspace.h>
#include <qtimer.h>
#include <qdragobject.h>
#include <qdom.h>
#include <qprocess.h>
#include <qstyle.h>
#include <qdatetimeedit.h>
#include <qassistantclient.h>
#include <qptrdict.h>
#include <qpopupmenu.h>

#include <limits.h>
static QColor *backColor1 = 0;
static QColor *backColor2 = 0;
static QColor *selectedBack = 0;

static void init_colors()
{
    if ( backColor1 )
	return;

#if 0 // a calculated alternative for backColor1
    QColorGroup myCg = qApp->palette().active();
    int h1, s1, v1;
    int h2, s2, v2;
    myCg.color( QColorGroup::Base ).hsv( &h1, &s1, &v1 );
    myCg.color( QColorGroup::Background ).hsv( &h2, &s2, &v2 );
    QColor c( h1, s1, ( v1 + v2 ) / 2, QColor::Hsv );
#endif

    backColor1 = new QColor(  250, 248, 235 );
    backColor2 = new QColor( 255, 255, 255 );
    selectedBack = new QColor( 230, 230, 230 );
}

#define setupStyle(x)

PropertyItem::PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : QListViewItem( l, after ), listview( l ), property( prop ), propertyName( propName )
{
    setSelectable( FALSE );
    open = FALSE;
    setText( 0, propertyName );
    changed = FALSE;
    setText( 1, "" );
    resetButton = 0;
}

PropertyItem::~PropertyItem()
{
    if ( resetButton )
	delete resetButton->parentWidget();
    resetButton = 0;
}

void PropertyItem::toggle()
{
}

void PropertyItem::updateBackColor()
{
    if ( itemAbove() && this != listview->firstChild() ) {
	if ( ( ( PropertyItem*)itemAbove() )->backColor == *backColor1 )
	    backColor = *backColor2;
	else
	    backColor = *backColor1;
    } else {
	backColor = *backColor1;
    }
    if ( listview->firstChild() == this )
	backColor = *backColor1;
}

QColor PropertyItem::backgroundColor()
{
    updateBackColor();
    if ( (QListViewItem*)this == listview->currentItem() )
	return *selectedBack;
    return backColor;
}

void PropertyItem::createChildren()
{
}

void PropertyItem::initChildren()
{
}

void PropertyItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    g.setColor( QColorGroup::Foreground, Qt::black );
    g.setColor( QColorGroup::Text, Qt::black );
    int indent = 0;
    if ( column == 0 ) {
	indent = 20 + ( property ? 20 : 0 );
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	p->save();
	p->translate( indent, 0 );
    }

    if ( isChanged() && column == 0 ) {
	p->save();
	QFont f = p->font();
	f.setBold( TRUE );
	p->setFont( f );
    }

    if ( !hasCustomContents() || column != 1 ) {
	QListViewItem::paintCell( p, g, column, width - indent, align  );
    } else {
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	drawCustomContents( p, QRect( 0, 0, width, height() ) );
    }

    if ( isChanged() && column == 0 )
	p->restore();
    if ( column == 0 )
	p->restore();
    if ( hasSubItems() && column == 0 ) {
	p->save();
	p->setPen( cg.foreground() );
	p->setBrush( cg.base() );
	p->drawRect( 5, height() / 2 - 4, 9, 9 );
	p->drawLine( 7, height() / 2, 11, height() / 2 );
	if ( !isOpen() )
	    p->drawLine( 9, height() / 2 - 2, 9, height() / 2 + 2 );
	p->restore();
    }
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();

    if ( listview->currentItem() == this && column == 0 &&
	 !listview->hasFocus() && !listview->viewport()->hasFocus() )
	paintFocus( p, cg, QRect( 0, 0, width, height() ) );
}

void PropertyItem::paintBranches( QPainter * p, const QColorGroup & cg,
				  int w, int y, int h )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    QListViewItem::paintBranches( p, g, w, y, h );
}

void PropertyItem::paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r )
{
    p->save();
    QApplication::style().drawPrimitive(QStyle::PE_Panel, p, r, cg,
					QStyle::Style_Sunken, QStyleOption(1,1) );
    p->restore();
}

/*!  Subclasses which are expandable items have to return TRUE
  here. Default is FALSE.
*/

bool PropertyItem::hasSubItems() const
{
    return FALSE;
}

/*!  Returns the parent property item here if this is a child or 0
 otherwise.
 */

PropertyItem *PropertyItem::propertyParent() const
{
    return property;
}

bool PropertyItem::isOpen() const
{
    return open;
}

void PropertyItem::setOpen( bool b )
{
    if ( b == open )
	return;
    open = b;

    if ( !open ) {
	children.setAutoDelete( TRUE );
	children.clear();
	children.setAutoDelete( FALSE );
	qApp->processEvents();
	listview->updateEditorSize();
	return;
    }

    createChildren();
    initChildren();
    qApp->processEvents();
    listview->updateEditorSize();
}

/*!  Subclasses have to show the editor of the item here
*/

void PropertyItem::showEditor()
{
    createResetButton();
    resetButton->parentWidget()->show();
}

/*!  Subclasses have to hide the editor of the item here
*/

void PropertyItem::hideEditor()
{
    createResetButton();
    resetButton->parentWidget()->hide();
}

/*!  This is called to init the value of the item. Reimplement in
  subclasses to init the editor
*/

void PropertyItem::setValue( const QVariant &v )
{
    val = v;
}

QVariant PropertyItem::value() const
{
    return val;
}

bool PropertyItem::isChanged() const
{
    return changed;
}

void PropertyItem::setChanged( bool b, bool updateDb )
{
    if ( propertyParent() )
	return;
    if ( changed == b )
	return;
    changed = b;
    repaint();
    if ( updateDb ) {
    }
    updateResetButtonState();
}

QString PropertyItem::name() const
{
    return propertyName;
}

void PropertyItem::createResetButton()
{
    if ( resetButton ) {
	resetButton->parentWidget()->lower();
	return;
    }
    QHBox *hbox = new QHBox( listview->viewport() );
    hbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    hbox->setLineWidth( 1 );
    resetButton = new QPushButton( hbox );
    setupStyle( resetButton );
	  resetButton->setPixmap( resetproperty_xpm );
    resetButton->setFixedWidth( resetButton->sizeHint().width() );
    hbox->layout()->setAlignment( Qt::AlignRight );
    listview->addChild( hbox );
    hbox->hide();
    QObject::connect( resetButton, SIGNAL( clicked() ),
		      listview, SLOT( resetProperty() ) );
    QToolTip::add( resetButton, PropertyEditor::tr( "Reset the property to its default value" ) );
    QWhatsThis::add( resetButton, PropertyEditor::tr( "Click this button to reset the property to its default value" ) );
    updateResetButtonState();
}

void PropertyItem::updateResetButtonState()
{
    if ( !resetButton )
	return;
    if ( propertyParent() )
	resetButton->setEnabled( FALSE );
    else
	resetButton->setEnabled( isChanged() );
}

/*!  Call this to place/resize the item editor correctly (normally
  call it from showEditor())
*/

void PropertyItem::placeEditor( QWidget *w )
{
    createResetButton();
    QRect r = listview->itemRect( this );
    if ( !r.size().isValid() ) {
	listview->ensureItemVisible( this );
#if defined(Q_WS_WIN)
	listview->repaintContents( FALSE );
#endif
	r = listview->itemRect( this );
    }
    r.setX( listview->header()->sectionPos( 1 ) );
    r.setWidth( listview->header()->sectionSize( 1 ) - 1 );
    r.setWidth( r.width() - resetButton->width() - 2 );
    r = QRect( listview->viewportToContents( r.topLeft() ), r.size() );
    w->resize( r.size() );
    listview->moveChild( w, r.x(), r.y() );
    resetButton->parentWidget()->resize( resetButton->sizeHint().width() + 10, r.height() );
    listview->moveChild( resetButton->parentWidget(), r.x() + r.width() - 8, r.y() );
    resetButton->setFixedHeight( QMAX( 0, r.height() - 3 ) );
}

/*!  This should be called by subclasses if the use changed the value
  of the property and this value should be applied to the widget property
*/

void PropertyItem::notifyValueChange()
{
    if ( !propertyParent() ) {
	listview->valueChanged( this );
	setChanged( TRUE );
	if ( hasSubItems() )
	    initChildren();
    } else {
	propertyParent()->childValueChanged( this );
	setChanged( TRUE );
    }
}

/*!  If a subclass is a expandable item reimplement this as this is
  always called if a child item changed its value. So update the
  display of the item here then.
*/

void PropertyItem::childValueChanged( PropertyItem * )
{
}

/*!  When adding a child item, call this (normally from addChildren()
*/

void PropertyItem::addChild( PropertyItem *i )
{
    children.append( i );
}

int PropertyItem::childCount() const
{
    return children.count();
}

PropertyItem *PropertyItem::child( int i ) const
{
    // ARRRRRRRRG
    return ( (PropertyItem*)this )->children.at( i );
}

/*!  If the contents of the item is not displayable with a text, but
  you want to draw it yourself (using drawCustomContents()), return
  TRUE here.
*/

bool PropertyItem::hasCustomContents() const
{
    return FALSE;
}

/*!
  \sa hasCustomContents()
*/

void PropertyItem::drawCustomContents( QPainter *, const QRect & )
{
}

QString PropertyItem::currentItem() const
{
    return QString::null;
}

int PropertyItem::currentIntItem() const
{
    return -1;
}

void PropertyItem::setCurrentItem( const QString & )
{
}

void PropertyItem::setCurrentItem( int )
{
}

int PropertyItem::currentIntItemFromObject() const
{
    return -1;
}

QString PropertyItem::currentItemFromObject() const
{
    return QString::null;
}

void PropertyItem::setFocus( QWidget *w )
{
    if ( !qApp->focusWidget() ||
	 ( !qApp->focusWidget()->inherits( "Editor" ) ) )
	w->setFocus();
}

void PropertyItem::setText( int col, const QString &t )
{
    QString txt( t );
    if ( col == 1 )
	txt = txt.replace( "\n", " " );
    QListViewItem::setText( col, txt );
}

// --------------------------------------------------------------

PropertyDoubleItem::PropertyDoubleItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    lin = 0;
}

QLineEdit *PropertyDoubleItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->setValidator( new QDoubleValidator( lin, "double_validator" ) );

    connect( lin, SIGNAL( returnPressed() ),
	     this, SLOT( setValue() ) );
    connect( lin, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( setValue() ) );
    lin->installEventFilter( listview );
    return lin;
}

PropertyDoubleItem::~PropertyDoubleItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
}

void PropertyDoubleItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( TRUE );
	lined()->setText( QString::number( value().toDouble() ) );
	lined()->blockSignals( FALSE );
    }
    QWidget* w = lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}


void PropertyDoubleItem::hideEditor()
{
    PropertyItem::hideEditor();
    QWidget* w = lined();
    w->hide();
}

void PropertyDoubleItem::setValue( const QVariant &v )
{
    if ( value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( TRUE );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( QString::number( v.toDouble() ) );
	if ( oldCursorPos < (int)lin->text().length() )
	    lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( FALSE );
    }
    setText( 1, QString::number( v.toDouble() ) );
    PropertyItem::setValue( v );
}

void PropertyDoubleItem::setValue()
{
    setText( 1, lined()->text() );
    QVariant v = lined()->text().toDouble();
    PropertyItem::setValue( v );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyBoolItem::PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName )
    : PropertyItem( l, after, prop, propName )
{
    comb = 0;
}

QComboBox *PropertyBoolItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( FALSE, listview->viewport() );
    comb->hide();
    comb->insertItem( tr( "False" ) );
    comb->insertItem( tr( "True" ) );
    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    return comb;
}

PropertyBoolItem::~PropertyBoolItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void PropertyBoolItem::toggle()
{
    bool b = value().toBool();
    setValue( QVariant( !b, 0 ) );
    setValue();
}

void PropertyBoolItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( TRUE );
	if ( value().toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( FALSE );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyBoolItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyBoolItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( comb ) {
	combo()->blockSignals( TRUE );
	if ( v.toBool() )
	    combo()->setCurrentItem( 1 );
	else
	    combo()->setCurrentItem( 0 );
	combo()->blockSignals( FALSE );
    }
    QString tmp = tr( "True" );
    if ( !v.toBool() )
	tmp = tr( "False" );
    setText( 1, tmp );
    PropertyItem::setValue( v );
}

void PropertyBoolItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    bool b = combo()->currentItem() == 0 ? (bool)FALSE : (bool)TRUE;
    PropertyItem::setValue( QVariant( b, 0 ) );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyIntItem::PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				  const QString &propName, bool s )
    : PropertyItem( l, after, prop, propName ), signedValue( s )
{
    spinBx = 0;
}

QSpinBox *PropertyIntItem::spinBox()
{
    if ( spinBx )
	return spinBx;
    if ( signedValue )
	spinBx = new QSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
    else
	spinBx = new QSpinBox( 0, INT_MAX, 1, listview->viewport() );
    spinBx->hide();
    spinBx->installEventFilter( listview );
    QObjectList *ol = spinBx->queryList( "QLineEdit" );
    if ( ol && ol->first() )
	ol->first()->installEventFilter( listview );
    delete ol;
    connect( spinBx, SIGNAL( valueChanged( int ) ),
	     this, SLOT( setValue() ) );
    return spinBx;
}

PropertyIntItem::~PropertyIntItem()
{
    delete (QSpinBox*)spinBx;
    spinBx = 0;
}

void PropertyIntItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !spinBx ) {
	spinBox()->blockSignals( TRUE );
	if ( signedValue )
	    spinBox()->setValue( value().toInt() );
	else
	    spinBox()->setValue( value().toUInt() );
	spinBox()->blockSignals( FALSE );
    }
    placeEditor( spinBox() );
    if ( !spinBox()->isVisible()  || !spinBox()->hasFocus()  ) {
	spinBox()->show();
	setFocus( spinBox() );
    }
}

void PropertyIntItem::hideEditor()
{
    PropertyItem::hideEditor();
    spinBox()->hide();
}

void PropertyIntItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    if ( spinBx ) {
	spinBox()->blockSignals( TRUE );
	if ( signedValue )
	    spinBox()->setValue( v.toInt() );
	else
	    spinBox()->setValue( v.toUInt() );
	spinBox()->blockSignals( FALSE );
    }

    if ( signedValue )
	    setText( 1, QString::number( v.toInt() ) );
    else
	    setText( 1, QString::number( v.toUInt() ) );
    PropertyItem::setValue( v );
}

void PropertyIntItem::setValue()
{
    if ( !spinBx )
	return;
    setText( 1, QString::number( spinBox()->value() ) );
    if ( signedValue )
	PropertyItem::setValue( spinBox()->value() );
    else
	PropertyItem::setValue( (uint)spinBox()->value() );
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyListItem::PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, bool e )
    : PropertyItem( l, after, prop, propName ), editable( e )
{
    comb = 0;
    oldInt = -1;
}

QComboBox *PropertyListItem::combo()
{
    if ( comb )
	return comb;
    comb = new QComboBox( editable, listview->viewport() );
    comb->hide();
    connect( comb, SIGNAL( activated( int ) ),
	     this, SLOT( setValue() ) );
    comb->installEventFilter( listview );
    if ( editable ) {
	QObjectList *ol = comb->queryList( "QLineEdit" );
	if ( ol && ol->first() )
	    ol->first()->installEventFilter( listview );
	delete ol;
    }
    return comb;
}

PropertyListItem::~PropertyListItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void PropertyListItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( FALSE );
    }
    placeEditor( combo() );
    if ( !combo()->isVisible()  || !combo()->hasFocus() ) {
	combo()->show();
	setFocus( combo() );
    }
}

void PropertyListItem::hideEditor()
{
    PropertyItem::hideEditor();
    combo()->hide();
}

void PropertyListItem::setValue( const QVariant &v )
{
    if ( comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( v.toStringList() );
	combo()->blockSignals( FALSE );
    }
    setText( 1, v.toStringList().first() );
    PropertyItem::setValue( v );
}

void PropertyListItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    QStringList lst;
    for ( uint i = 0; i < combo()->listBox()->count(); ++i )
	lst << combo()->listBox()->item( i )->text();
    PropertyItem::setValue( lst );
    notifyValueChange();
    oldInt = currentIntItem();
    oldString = currentItem();
}

QString PropertyListItem::currentItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentText();
}

void PropertyListItem::setCurrentItem( const QString &s )
{
    if ( comb && currentItem().lower() == s.lower() )
	return;

    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( FALSE );
    }
    for ( uint i = 0; i < combo()->listBox()->count(); ++i ) {
	if ( combo()->listBox()->item( i )->text().lower() == s.lower() ) {
	    combo()->setCurrentItem( i );
	    setText( 1, combo()->currentText() );
	    break;
	}
    }
    oldInt = currentIntItem();
    oldString = currentItem();
}

void PropertyListItem::addItem( const QString &s )
{
    combo()->insertItem( s );
}

void PropertyListItem::setCurrentItem( int i )
{
    if ( comb && i == combo()->currentItem() )
	return;

    if ( !comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( value().toStringList() );
	combo()->blockSignals( FALSE );
    }
    combo()->setCurrentItem( i );
    setText( 1, combo()->currentText() );
    oldInt = currentIntItem();
    oldString = currentItem();
}

int PropertyListItem::currentIntItem() const
{
    return ( (PropertyListItem*)this )->combo()->currentItem();
}

int PropertyListItem::currentIntItemFromObject() const
{
    return oldInt;
}

QString PropertyListItem::currentItemFromObject() const
{
    return oldString;
}

// --------------------------------------------------------------

PropertyCoordItem::PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				    const QString &propName, Type t )
    : PropertyItem( l, after, prop, propName ), typ( t )
{
    lin = 0;

}

QLineEdit *PropertyCoordItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->setReadOnly( TRUE );
    lin->installEventFilter( listview );
    lin->hide();
    return lin;
}

void PropertyCoordItem::createChildren()
{
    PropertyItem *i = this;
    if ( typ == Rect || typ == Point ) {
	i = new PropertyIntItem( listview, i, this, tr( "x" ), TRUE );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, tr( "y" ), TRUE );
	addChild( i );
    }
    if ( typ == Rect || typ == Size ) {
	i = new PropertyIntItem( listview, i, this, tr( "width" ), TRUE );
	addChild( i );
	i = new PropertyIntItem( listview, i, this, tr( "height" ), TRUE );
	addChild( i );
    }
}

void PropertyCoordItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "x" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().x() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().x() );
	} else if ( item->name() == tr( "y" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().y() );
	    else if ( typ == Point )
		item->setValue( val.toPoint().y() );
	} else if ( item->name() == tr( "width" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().width() );
	    else if ( typ == Size )
		item->setValue( val.toSize().width() );
	} else if ( item->name() == tr( "height" ) ) {
	    if ( typ == Rect )
		item->setValue( val.toRect().height() );
	    else if ( typ == Size )
		item->setValue( val.toSize().height() );
	}
    }
}

PropertyCoordItem::~PropertyCoordItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
}

void PropertyCoordItem::showEditor()
{
    PropertyItem::showEditor();
    if ( !lin )
	lined()->setText( text( 1 ) );
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus()  ) {
	lined()->show();
	setFocus( lined() );
    }
}

void PropertyCoordItem::hideEditor()
{
    PropertyItem::hideEditor();
    lined()->hide();
}

void PropertyCoordItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    if ( typ == Rect )
	s = "[ " + QString::number( v.toRect().x() ) + ", " + QString::number( v.toRect().y() ) + ", " +
	    QString::number( v.toRect().width() ) + ", " + QString::number( v.toRect().height() ) + " ]";
    else if ( typ == Point )
	s = "[ " + QString::number( v.toPoint().x() ) + ", " +
	    QString::number( v.toPoint().y() ) + " ]";
    else if ( typ == Size )
	s = "[ " + QString::number( v.toSize().width() ) + ", " +
	    QString::number( v.toSize().height() ) + " ]";
    setText( 1, s );
    if ( lin )
	lined()->setText( s );
    PropertyItem::setValue( v );
}

bool PropertyCoordItem::hasSubItems() const
{
    return TRUE;
}

void PropertyCoordItem::childValueChanged( PropertyItem *child )
{
    if ( typ == Rect ) {
	QRect r = value().toRect();
	if ( child->name() == tr( "x" ) )
	    r.moveBy( -r.x() + child->value().toInt(), 0 );
	else if ( child->name() == tr( "y" ) )
	    r.moveBy( 0, -r.y() + child->value().toInt() );
	else if ( child->name() == tr( "width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == tr( "height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    } else if ( typ == Point ) {
	QPoint r = value().toPoint();
	if ( child->name() == tr( "x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == tr( "y" ) )
	    r.setY( child->value().toInt() );
	setValue( r );
    } else if ( typ == Size ) {
	QSize r = value().toSize();
	if ( child->name() == tr( "width" ) )
	    r.setWidth( child->value().toInt() );
	else if ( child->name() == tr( "height" ) )
	    r.setHeight( child->value().toInt() );
	setValue( r );
    }
    notifyValueChange();
}

// --------------------------------------------------------------

PropertyColorItem::PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
				      const QString &propName, bool children )
    : PropertyItem( l, after, prop, propName ), withChildren( children )
{
    box = new QHBox( listview->viewport() );
    box->hide();
    colorPrev = new QFrame( box );
    button = new QPushButton( "...", box );
    setupStyle( button );
    button->setFixedWidth( 20 );
    box->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    box->setLineWidth( 2 );
    colorPrev->setFrameStyle( QFrame::Plain | QFrame::Box );
    colorPrev->setLineWidth( 2 );
    QPalette pal = colorPrev->palette();
    QColorGroup cg = pal.active();
    cg.setColor( QColorGroup::Foreground, cg.color( QColorGroup::Base ) );
    pal.setActive( cg );
    pal.setInactive( cg );
    pal.setDisabled( cg );
    colorPrev->setPalette( pal );
    box->installEventFilter( listview );
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getColor() ) );
}

void PropertyColorItem::createChildren()
{
    PropertyItem *i = this;
    i = new PropertyIntItem( listview, i, this, tr( "Red" ), TRUE );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Green" ), TRUE );
    addChild( i );
    i = new PropertyIntItem( listview, i, this, tr( "Blue" ), TRUE );
    addChild( i );
}

void PropertyColorItem::initChildren()
{
    PropertyItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = PropertyItem::child( i );
	if ( item->name() == tr( "Red" ) )
	    item->setValue( val.toColor().red() );
	else if ( item->name() == tr( "Green" ) )
	    item->setValue( val.toColor().green() );
	else if ( item->name() == tr( "Blue" ) )
	    item->setValue( val.toColor().blue() );
    }
}

PropertyColorItem::~PropertyColorItem()
{
    delete (QHBox*)box;
}

void PropertyColorItem::showEditor()
{
    PropertyItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() ) {
	box->show();
	listView()->viewport()->setFocus();
    }
}

void PropertyColorItem::hideEditor()
{
    PropertyItem::hideEditor();
    box->hide();
}

void PropertyColorItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    setText( 1, v.toColor().name() );
    colorPrev->setBackgroundColor( v.toColor() );
    PropertyItem::setValue( v );
}

bool PropertyColorItem::hasSubItems() const
{
    return withChildren;
}

void PropertyColorItem::childValueChanged( PropertyItem *child )
{
    QColor c( val.toColor() );
    if ( child->name() == tr( "Red" ) )
	c.setRgb( child->value().toInt(), c.green(), c.blue() );
    else if ( child->name() == tr( "Green" ) )
	c.setRgb( c.red(), child->value().toInt(), c.blue() );
    else if ( child->name() == tr( "Blue" ) )
	c.setRgb( c.red(), c.green(), child->value().toInt() );
    setValue( c );
    notifyValueChange();
}

void PropertyColorItem::getColor()
{
    QColor c = QColorDialog::getColor( val.asColor(), listview );
    if ( c.isValid() ) {
	setValue( c );
	notifyValueChange();
    }
}

bool PropertyColorItem::hasCustomContents() const
{
    return TRUE;
}

void PropertyColorItem::drawCustomContents( QPainter *p, const QRect &r )
{
    p->save();
    p->setPen( QPen( black, 1 ) );
    p->setBrush( val.toColor() );
    p->drawRect( r.x() + 2, r.y() + 2, r.width() - 5, r.height() - 5 );
    p->restore();
}

// --------------------------------------------------------------

/*!
  \class PropertyList propertyeditor.h
  \brief PropertyList is a QListView derived class which is used for editing widget properties

  This class is used for widget properties. It has to be child of a
  PropertyEditor.

  To initialize it for editing a widget call setupProperties() which
  iterates through the properties of the current widget (see
  PropertyEditor::widget()) and builds the list.

  To update the item values, refetchData() can be called.

  If the value of an item has been changed by the user, and this
  change should be applied to the widget's property, valueChanged()
  has to be called.

  To set the value of an item, setPropertyValue() has to be called.
*/

PropertyList::PropertyList( PropertyEditor *e )
    : QListView( e ), editor( e )
{
    init_colors();

    showSorted = FALSE;
    header()->setMovingEnabled( FALSE );
    header()->setStretchEnabled( TRUE );
    setResizePolicy( QScrollView::Manual );
    viewport()->setAcceptDrops( TRUE );
    viewport()->installEventFilter( this );
    addColumn( tr( "Property" ) );
    addColumn( tr( "Value" ) );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( updateEditorSize() ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );
//    connect( header(), SIGNAL( sectionClicked( int ) ),
//	     this, SLOT( toggleSort() ) );
    connect( this, SIGNAL( pressed( QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( itemPressed( QListViewItem *, const QPoint &, int ) ) );
    connect( this, SIGNAL( doubleClicked( QListViewItem * ) ),
	     this, SLOT( toggleOpen( QListViewItem * ) ) );
    setSorting( -1 );
    setHScrollBarMode( AlwaysOff );
//    setVScrollBarMode( AlwaysOn );
    setColumnWidthMode( 1, Manual );
    mousePressed = FALSE;
    pressItem = 0;
    header()->installEventFilter( this );

		addPropertyItem();
}

void PropertyList::toggleSort()
{
    showSorted = !showSorted;
    editor->clear();
    editor->setup();
}

void PropertyList::resizeEvent( QResizeEvent *e )
{
    QListView::resizeEvent( e );
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

static QVariant::Type type_to_variant( const QString &s )
{
    if ( s == "Invalid " )
	return QVariant::Invalid;
    if ( s == "Map" )
	return QVariant::Map;
    if ( s == "List" )
	return QVariant::List;
    if ( s == "String" )
	return QVariant::String;
    if ( s == "StringList" )
	return QVariant::StringList;
    if ( s == "Font" )
	return QVariant::Font;
    if ( s == "Pixmap" )
	return QVariant::Pixmap;
    if ( s == "Brush" )
	return QVariant::Brush;
    if ( s == "Rect" )
	return QVariant::Rect;
    if ( s == "Size" )
	return QVariant::Size;
    if ( s == "Color" )
	return QVariant::Color;
    if ( s == "Palette" )
	return QVariant::Palette;
    if ( s == "ColorGroup" )
	return QVariant::ColorGroup;
    if ( s == "IconSet" )
	return QVariant::IconSet;
    if ( s == "Point" )
	return QVariant::Point;
    if ( s == "Image" )
	return QVariant::Image;
    if ( s == "Int" )
	return QVariant::Int;
    if ( s == "UInt" )
	return QVariant::UInt;
    if ( s == "Bool" )
	return QVariant::Bool;
    if ( s == "Double" )
	return QVariant::Double;
    if ( s == "CString" )
	return QVariant::CString;
    if ( s == "PointArray" )
	return QVariant::PointArray;
    if ( s == "Region" )
	return QVariant::Region;
    if ( s == "Bitmap" )
	return QVariant::Bitmap;
    if ( s == "Cursor" )
	return QVariant::Cursor;
    if ( s == "SizePolicy" )
	return QVariant::SizePolicy;
    if ( s == "Date" )
	return QVariant::Date;
    if ( s == "Time" )
	return QVariant::Time;
    if ( s == "DateTime" )
	return QVariant::DateTime;
    return QVariant::Invalid;
}

#ifndef QT_NO_SQL
static bool parent_is_data_aware( QObject *o )
{
    if ( !o->inherits( "QWidget" ) )
	return FALSE;
    QWidget *w = (QWidget*)o;
    QWidget *p = w->parentWidget();
    while ( p && !p->isTopLevel() ) {
	if ( p->inherits( "QDesignerDataBrowser" ) || p->inherits( "QDesignerDataView" ) )
	    return TRUE;
	p = p->parentWidget();
    }
    return FALSE;
}
#endif

void PropertyList::setupProperties()
{
	addPropertyItem();
}

bool PropertyList::addPropertyItem( /*PropertyItem *&item, const QCString &name, QVariant::Type t*/ )
{
	PropertyItem * item = 0;
	QCString name = "Test";
	item = new PropertyColorItem( this, item, 0, name, TRUE );
	item = new PropertyBoolItem( this, item, 0, name );
  item = new PropertyIntItem( this, item, 0, name, TRUE );
	item = new PropertyListItem( this, item, 0, name, TRUE );
	item = new PropertyColorItem( this, item, 0, name, TRUE );

/*    switch ( t ) {
    case QVariant::Bool:
	item = new PropertyBoolItem( this, item, 0, name );
	break;
    case QVariant::Int:
	    item = new PropertyIntItem( this, item, 0, name, TRUE );
	break;
    case QVariant::UInt:
	item = new PropertyIntItem( this, item, 0, name, FALSE );
	break;
    case QVariant::StringList:
	item = new PropertyListItem( this, item, 0, name, TRUE );
	break;
    case QVariant::Color:
	item = new PropertyColorItem( this, item, 0, name, TRUE );
	break;
    default:
	return FALSE;
    }*/
    return TRUE;
}

void PropertyList::paintEmptyArea( QPainter *p, const QRect &r )
{
    p->fillRect( r, *backColor2 );
}

void PropertyList::setCurrentItem( QListViewItem *i )
{
    if ( !i )
	return;

    if ( currentItem() )
	( (PropertyItem*)currentItem() )->hideEditor();
    QListView::setCurrentItem( i );
    ( (PropertyItem*)currentItem() )->showEditor();
}

void PropertyList::updateEditorSize()
{
    if ( currentItem() )
	( ( PropertyItem* )currentItem() )->showEditor();
}

/*!  This has to be called if the value if \a i should be set as
  property to the currently edited widget.
*/

void PropertyList::valueChanged( PropertyItem *i )
{
}

void PropertyList::layoutInitValue( PropertyItem *i, bool changed )
{
}

void PropertyList::itemPressed( QListViewItem *i, const QPoint &p, int c )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( !pi->hasSubItems() )
	return;

    if ( c == 0 && viewport()->mapFromGlobal( p ).x() < 20 )
	toggleOpen( i );
}

void PropertyList::toggleOpen( QListViewItem *i )
{
    if ( !i )
	return;
    PropertyItem *pi = (PropertyItem*)i;
    if ( pi->hasSubItems() ) {
	pi->setOpen( !pi->isOpen() );
    } else {
	pi->toggle();
    }
}

bool PropertyList::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return TRUE;

    PropertyItem *i = (PropertyItem*)currentItem();
    if ( o != this &&e->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent*)e;
	if ( ( ke->key() == Key_Up || ke->key() == Key_Down ) &&
	     ( o != this || o != viewport() ) &&
	     !( ke->state() & ControlButton ) ) {
	    QApplication::sendEvent( this, (QKeyEvent*)e );
	    return TRUE;
	} else if ( ( !o->inherits( "QLineEdit" ) ||
		      ( o->inherits( "QLineEdit" ) && ( (QLineEdit*)o )->isReadOnly() ) ) &&
		    i && i->hasSubItems() ) {
	    if ( !i->isOpen() &&
		 ( ke->key() == Key_Plus ||
		   ke->key() == Key_Right ))
		i->setOpen( TRUE );
	    else if ( i->isOpen() &&
		      ( ke->key() == Key_Minus ||
			ke->key() == Key_Left ) )
		i->setOpen( FALSE );
	} else if ( ( ke->key() == Key_Return || ke->key() == Key_Enter ) && o->inherits( "QComboBox" ) ) {
	    QKeyEvent ke2( QEvent::KeyPress, Key_Space, 0, 0 );
	    QApplication::sendEvent( o, &ke2 );
	    return TRUE;
	}
    } 
	else if ( o == viewport() ) {
	QMouseEvent *me;
	PropertyListItem* i;
	switch ( e->type() ) {
	case QEvent::MouseButtonPress:
	    me = (QMouseEvent*)e;
	    i = (PropertyListItem*) itemAt( me->pos() );
	    if( i  && ( i->inherits("PropertyColorItem") || i->inherits("PropertyPixmapItem") ) ) {
		pressItem = i;
		pressPos = me->pos();
		mousePressed = TRUE;
	    }
	    break;
	case QEvent::MouseMove:
	    me = (QMouseEvent*)e;
	    if ( me && me->state() & LeftButton && mousePressed) {

		i = (PropertyListItem*) itemAt( me->pos() );
		if( i  && i == pressItem ) {

		    if(( pressPos - me->pos() ).manhattanLength() > QApplication::startDragDistance() ){
			if ( i->inherits("PropertyColorItem") ) {
			    QColor col = i->value().asColor();
			    QColorDrag *drg = new QColorDrag( col, this );
			    QPixmap pix( 25, 25 );
			    pix.fill( col );
			    QPainter p( &pix );
			    p.drawRect( 0, 0, pix.width(), pix.height() );
			    p.end();
			    drg->setPixmap( pix );
			    mousePressed = FALSE;
			    drg->dragCopy();
			}
			else if ( i->inherits("PropertyPixmapItem") ) {
			    QPixmap pix = i->value().asPixmap();
			    if( !pix.isNull() ) {
				QImage img = pix.convertToImage();
				QImageDrag *drg = new QImageDrag( img, this );
				drg->setPixmap( pix );
				mousePressed = FALSE;
				drg->dragCopy();
			    }
			}
		    }
		}
	    }
	    break;
	default:
	    break;
	}
    } else if ( o == header() ) {
	if ( e->type() == QEvent::ContextMenu ) {
	    ((QContextMenuEvent *)e)->accept();
	    QPopupMenu menu( 0 );
	    menu.setCheckable( TRUE );
	    const int cat_id = 1;
	    const int alpha_id = 2;
	    menu.insertItem( tr( "Sort &Categorized" ), cat_id );
	    int alpha = menu.insertItem( tr( "Sort &Alphabetically" ), alpha_id );
	    if ( showSorted )
		menu.setItemChecked( alpha_id, TRUE );
	    else
		menu.setItemChecked( cat_id, TRUE );
	    int res = menu.exec( ( (QContextMenuEvent*)e )->globalPos() );
	    if ( res != -1 ) {
		bool newShowSorted = ( res == alpha );
		if ( showSorted != newShowSorted ) {
		    showSorted = newShowSorted;
		    editor->clear();
		    editor->setup();
		}
	    }
	    return TRUE;
	}
    }

    return QListView::eventFilter( o, e );
}

/*!  This method re-initializes each item of the property list.
*/

void PropertyList::refetchData()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	PropertyItem *i = (PropertyItem*)it.current();
	if ( !i->propertyParent() )
	    setPropertyValue( i );
	if ( i->hasSubItems() )
	    i->initChildren();
	bool changed = true;
	if ( ( i->name() == "layoutSpacing" || i->name() == "layoutMargin" ) ) {
	    if ( i->value().toString() != "-1" )
		i->setChanged( TRUE, FALSE );
	    else
		i->setChanged( FALSE, FALSE );
	}
	else if ( changed != i->isChanged() )
	    i->setChanged( changed, FALSE );
    }
    updateEditorSize();
}

static void clearAlignList( QStrList &l )
{
    if ( l.count() == 1 )
	return;
    if ( l.find( "AlignAuto" ) != -1 )
	l.remove( "AlignAuto" );
    if ( l.find( "WordBreak" ) != -1 )
	l.remove( "WordBreak" );
}

/*!  This method initializes the value of the item \a i to the value
  of the corresponding property.
*/

void PropertyList::setPropertyValue( PropertyItem *i )
{
    const QMetaProperty *p =
	editor->widget()->metaObject()->
	property( editor->widget()->metaObject()->findProperty( i->name(), TRUE), TRUE );
    if ( !p ) {
	if ( i->name() == "hAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->
		property( editor->widget()->metaObject()->findProperty( "alignment", TRUE ), TRUE );
	    align &= ~AlignVertical_Mask;
	    QStrList l = p->valueToKeys( align );
	    clearAlignList( l );
	    ( (PropertyListItem*)i )->setCurrentItem( l.last() );
	} else if ( i->name() == "vAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->
		property( editor->widget()->metaObject()->findProperty( "alignment", TRUE ), TRUE );
	    align &= ~AlignHorizontal_Mask;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).last() );
	} else if ( i->name() == "wordwrap" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		i->setValue( QVariant( TRUE, 0 ) );
	    else
		i->setValue( QVariant( FALSE, 0 ) );
	} else if ( i->name() == "layoutSpacing" ) {
	} else if ( i->name() == "layoutMargin" ) {
	} else if ( i->name() == "toolTip" || i->name() == "whatsThis" || i->name() == "database" || i->name() == "frameworkCode" ) {
	} else if ( editor->widget()->inherits( "CustomWidget" ) ) {
	}
	return;
    }
    if ( p->isSetType() )
	;
    else if ( p->isEnumType() )
	( (PropertyListItem*)i )->setCurrentItem( p->valueToKey( editor->widget()->property( i->name() ).toInt() ) );
    else if ( qstrcmp( p->name(), "buddy" ) == 0 )
	( (PropertyListItem*)i )->setCurrentItem( editor->widget()->property( i->name() ).toString() );
    else
	i->setValue( editor->widget()->property( i->name() ) );
}

void PropertyList::setCurrentProperty( const QString &n )
{
    if ( currentItem() && currentItem()->text( 0 ) == n ||
	 currentItem() && ( (PropertyItem*)currentItem() )->propertyParent() &&
	 ( (PropertyItem*)currentItem() )->propertyParent()->text( 0 ) == n )
	return;

    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	if ( it.current()->text( 0 ) == n ) {
	    setCurrentItem( it.current() );
	    break;
	}
    }
}

PropertyEditor *PropertyList::propertyEditor() const
{
    return editor;
}

void PropertyList::resetProperty()
{
    if ( !currentItem() )
	return;
    PropertyItem *i = (PropertyItem*)currentItem();
    if ( i->hasSubItems() )
	i->initChildren();
}

void PropertyList::viewportDragEnterEvent( QDragEnterEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( i->inherits("PropertyColorItem") && QColorDrag::canDecode( e ) )
	e->accept();
    else if ( i->inherits("PropertyPixmapItem") && QImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void PropertyList::viewportDragMoveEvent ( QDragMoveEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( i->inherits("PropertyColorItem") && QColorDrag::canDecode( e ) )
	e->accept();
    else if ( i->inherits("PropertyPixmapItem") && QImageDrag::canDecode( e ) )
	e->accept();
    else
	e->ignore();
}

void PropertyList::viewportDropEvent ( QDropEvent *e )
{
    PropertyListItem *i = (PropertyListItem*) itemAt( e->pos() );
    if( !i ) {
	e->ignore();
	return;
    }

    if ( i->inherits("PropertyColorItem") && QColorDrag::canDecode( e ) ) {
	QColor color;
	QColorDrag::decode( e, color );
	i->setValue( QVariant( color ) );
	valueChanged( i );
	e->accept();
    }
    else if ( i->inherits("PropertyPixmapItem")  && QImageDrag::canDecode( e ) ) {
	QImage img;
	QImageDrag::decode( e, img );
	QPixmap pm;
	pm.convertFromImage( img );
	i->setValue( QVariant( pm ) );
	valueChanged( i );
	e->accept();
    }
    else
	e->ignore();
}

QString PropertyList::whatsThisAt( const QPoint &p )
{
    return whatsThisText( itemAt( p ) );
}

void PropertyList::showCurrentWhatsThis()
{
    if ( !currentItem() )
	return;
    QPoint p( 0, currentItem()->itemPos() );
    p = viewport()->mapToGlobal( contentsToViewport( p ) );
    QWhatsThis::display( whatsThisText( currentItem() ), p, viewport() );
}

QString PropertyList::whatsThisText( QListViewItem *i )
{
    if ( !i || !editor->widget() )
	return QString::null;
    readPropertyDocs();
    if ( ( (PropertyItem*)i )->propertyParent() )
	i = ( (PropertyItem*)i )->propertyParent();

    const QMetaObject *mo = editor->widget()->metaObject();
    QString prop = ( (PropertyItem*)i )->name();
    while ( mo ) {
	QString s;
	s = QString( mo->className() ) + "::" + prop;
	QMap<QString, QString>::Iterator it;
	if ( ( it = propertyDocs.find( s ) ) != propertyDocs.end() ) {
	    return *it;
	}
	mo = mo->superClass();
    }

    return tr( "<p><b>QWidget::%1</b></p><p>There is no documentation available for this property.</p>" ).arg( prop );
}

void PropertyList::readPropertyDocs()
{
}

extern QListViewItem *newItem;

// --------------------------------------------------------------

PropertyEditor::PropertyEditor( QWidget *parent )
    : QTabWidget( parent )
{
    setCaption( tr( "Property Editor" ) );
    wid = 0;
    listview = new PropertyList( this );
    addTab( listview, tr( "P&roperties" ) );
//    addTab( eList, tr( "Signa&l Handlers" ) );
}

QObject *PropertyEditor::widget() const
{
    return wid;
}

void PropertyEditor::clear()
{
    listview->setContentsPos( 0, 0 );
    listview->clear();
}

void PropertyEditor::setup()
{
    listview->viewport()->setUpdatesEnabled( FALSE );
    listview->setupProperties();
    listview->viewport()->setUpdatesEnabled( TRUE );
    listview->updateEditorSize();
}

void PropertyEditor::refetchData()
{
    listview->refetchData();
}

void PropertyEditor::closeEvent( QCloseEvent *e )
{
    emit hidden();
    e->accept();
}

PropertyList *PropertyEditor::propertyList() const
{
    return listview;
}

QString PropertyEditor::currentProperty() const
{
    if ( !wid )
	return QString::null;
    if ( ( (PropertyItem*)listview->currentItem() )->propertyParent() )
	return ( (PropertyItem*)listview->currentItem() )->propertyParent()->name();
    return ( (PropertyItem*)listview->currentItem() )->name();
}

QString PropertyEditor::classOfCurrentProperty() const
{
    if ( !wid )
	return QString::null;
    QObject *o = wid;
    QString curr = currentProperty();
    QMetaObject *mo = o->metaObject();
    while ( mo ) {
	QStrList props = mo->propertyNames( FALSE );
	if ( props.find( curr.latin1() ) != -1 )
	    return mo->className();
	mo = mo->superClass();
    }
    return QString::null;
}

void PropertyEditor::resetFocus()
{
    if ( listview->currentItem() )
	( (PropertyItem*)listview->currentItem() )->showEditor();
}

void PropertyEditor::setPropertyEditorEnabled( bool b )
{
    if ( !b )
	removePage( listview );
    else
	insertTab( listview, tr( "Property Editor" ), 0 );
    updateWindow();
}

void PropertyEditor::setSignalHandlersEnabled( bool b )
{
}

void PropertyEditor::updateWindow()
{
    if ( isHidden() && count() ) {
	parentWidget()->show();
//	MainWindow::self->setAppropriate( (QDockWindow*)parentWidget(), TRUE );
    } else if ( isShown() && !count() ) {
	parentWidget()->hide();
//	MainWindow::self->setAppropriate( (QDockWindow*)parentWidget(), FALSE );
    }
}

#include "moc_PropertyView.cpp"
