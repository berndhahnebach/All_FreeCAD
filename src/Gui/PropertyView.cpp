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
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Widgets.h"
#include "PropertyView.h"
#include "Tools.h"

#include "Application.h"
#include "HtmlView.h"
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
{
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
  	w->setFocus();
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

FCPropertyListView::FCPropertyListView( FCPropertyView *view )
  : QListView( view ), propView( view )
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

FCPropertyView *FCPropertyListView::propertyView() const
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

QColor FCPropertyView::cBackColor1=QColor(236,236,236);
QColor FCPropertyView::cBackColor2=QColor(236,212,156);


//**************************************************************************
// Construction/Destruction

FCPropertyView::FCPropertyView(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:FCDockView(pcDocument,parent,name)
{
  setCaption( tr( "Property View" ) );
  wid = 0;

  _pcListView = new FCPropertyListView(this/*,name*/);

  pclFormLayout = new QGridLayout( this ); 
  pclFormLayout->setSpacing( 1 );
  pclFormLayout->setMargin ( 1 );
  tabs = new QTabBar(this, "jfoijwe");
  tabs->setShape(QTabBar::TriangularBelow);
  QTab* t;
  t = new QTab; t->setText("Tab1"); tabs->addTab(t);
  t = new QTab; t->setText("Tab2"); tabs->addTab(t);
  t = new QTab; t->setText("Tab3"); tabs->addTab(t);
  pclFormLayout->addWidget( _pcListView, 0, 0 );
  pclFormLayout->addWidget( tabs,   1, 0 );
//  tabs->hide();

	// set defaults and the colums
	_pcListView->setSorting(-1,false);
	_pcListView->addColumn(tr("Property"));
	_pcListView->setColumnWidthMode(0,QListView::Manual);
	_pcListView->addColumn(tr("Value"));
	_pcListView->setColumnWidthMode(1,QListView::Manual );
	_pcListView->setColumnWidth(0,100);
	_pcListView->setColumnWidth(1,100);

	// retrieve the Pixmaps
	pcLabelOpen   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_Attr"));


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
  _pcListView->setContentsPos( 0, 0 );
  _pcListView->clear();
}

void FCPropertyView::setup()
{
  _pcListView->viewport()->setUpdatesEnabled( FALSE );
  _pcListView->viewport()->setUpdatesEnabled( TRUE );
  qApp->processEvents();
  _pcListView->updateViewSize();
}

void FCPropertyView::resetFocus()
{
  if ( _pcListView->currentItem() )
  	( (FCPropertyViewItem*)_pcListView->currentItem() )->showView();
}

FCPropertyListView *FCPropertyView::propertyList() const
{
  return _pcListView;
}

QString FCPropertyView::currentProperty() const
{
  if ( !wid )
  	return QString::null;
  if ( ( (FCPropertyViewItem*)_pcListView->currentItem() )->propertyParent() )
  	return ( (FCPropertyViewItem*)_pcListView->currentItem() )->propertyParent()->name();
  return ( (FCPropertyViewItem*)_pcListView->currentItem() )->name();
}

void FCPropertyView::Update(void)
{
	GetConsole().Log("Property Updated\n");
}

void FCPropertyView::OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument)
{
}

void FCPropertyView::resizeEvent ( QResizeEvent * e) 
{
	// routing the resize event to the child
	_pcListView->resize(e->size());

	QSize vs = _pcListView->viewportSize( 0, _pcListView->contentsHeight() );

	int os = _pcListView->header()->sectionSize( 1 );
  int ns = vs.width() - _pcListView->header()->sectionSize( 0 );
  if ( ns < 16 )
  	ns = 16;
	
  _pcListView->header()->resizeSection( 1, ns );
  _pcListView->header()->repaint( _pcListView->header()->width() - _pcListView->header()->sectionSize( 1 ), 0, _pcListView->header()->sectionSize( 1 ), _pcListView->header()->height() );
}

bool FCPropertyView::OnMsg(const char* pMsg)
{
	// no messages yet
	return false;
}

//**************************************************************************
// separator for other implemetation aspects

#include "moc_PropertyView.cpp"
