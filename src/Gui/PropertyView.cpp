/** \file PropertyView.cpp
 *  \brief Framework to show and work with all kind of properies
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *
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

#include "Application.h"
#include "../Base/Console.h"

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



FCPropertyViewItem::FCPropertyViewItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop, const QString &propName)
: QListViewItem( l, after ), listview( l ), property( prop ), propertyName( propName )
{
    setSelectable( FALSE );
    open = FALSE;
    setText( 0, propertyName );
    changed = FALSE;
    setText( 1, "" );
    resetButton = 0;
}

FCPropertyViewItem::FCPropertyViewItem( FCPropertyView * parent)
	:QListViewItem(parent->_pcListView)
{
	/*
	if(_pcDocument){
		setText(0,"Main Label");
		setPixmap(0,*FCTree::pcLabelOpen);
		_hcLabel = parent->_pcDocument->GetDocument()->Main();
		BuildUp();
		setOpen(true);
	}else{
		setPixmap(0,*FCTree::pcLabelClosed);
		//setPixmap(new QPixmap(px));
		setText(0,"No Active Document");
	}
*/
	
}

/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
FCPropertyViewItem::FCPropertyViewItem( FCPropertyViewItem * parent )
    : QListViewItem( parent )
{
/*
	QString cString;

	cString.sprintf("Tag:%d",_hcLabel->GetOCCLabel().Tag());
	setPixmap(0,*FCTree::pcLabelClosed);
	setText(0,cString);

	BuildUp();
*/		
}


void FCPropertyViewItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    g.setColor( QColorGroup::Foreground, Qt::black );
    g.setColor( QColorGroup::Text, Qt::black );
    int indent = 0;
    if ( column == 0 ) {
	indent = 20 + ( property ? 20 : 0 );
  int ii = height();
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

#if QT_VERSION <= 230
void FCPropertyViewItem::paintBranches( QPainter * p, const QColorGroup & cg,
				  int w, int y, int h, GUIStyle s )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    QListViewItem::paintBranches( p, g, w, y, h, s );
}	
#else
void FCPropertyViewItem::paintBranches( QPainter * p, const QColorGroup & cg, int w, int y, int h)
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    QListViewItem::paintBranches( p, g, w, y, h );
}	
#endif

void FCPropertyViewItem::paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r )
{
    p->save();
#if QT_VERSION <= 230
    QApplication::style().drawPanel( p, r.x(), r.y(), r.width(), r.height(), cg, TRUE, 1 );
#else
    QApplication::style().drawPrimitive(QStyle::PE_Panel, p, QRect(r.x(), r.y(), r.width(), r.height()), cg);
#endif
    p->restore();
}

void FCPropertyViewItem::updateBackColor()
{
//	backColor = FCPropertyView::cBackColor1;
//
//    if ( itemAbove() /*&& this != listview->firstChild()*/ )
//	{
//		if ( ( ( FCPropertyViewItem*)itemAbove() )->backColor == FCPropertyView::cBackColor1 )
//			backColor = FCPropertyView::cBackColor2;
//    }

    if ( itemAbove() && this != listview->firstChild() ) {
	if ( ( ( FCPropertyViewItem*)itemAbove() )->backColor == FCPropertyView::cBackColor1 )
	    backColor = FCPropertyView::cBackColor2;
	else
	    backColor = FCPropertyView::cBackColor1;
    } else {
	backColor = FCPropertyView::cBackColor1;
    }
    if ( listview->firstChild() == this )
	backColor = FCPropertyView::cBackColor1;
}

QColor FCPropertyViewItem::backgroundColor()
{
    updateBackColor();
/*    if ( (QListViewItem*)this == listview->currentItem() )
		return selectedBack;*/
    return backColor;
}

bool FCPropertyViewItem::isChanged() const
{
    return changed;
}

void FCPropertyViewItem::setChanged( bool b, bool updateDb )
{
	
    if ( propertyParent() )
	return;
    if ( changed == b )
	return;
    changed = b;
    repaint();
//    if ( updateDb )
//	MetaDataBase::setPropertyChanged( listview->propertyEditor()->widget(), name(), changed );
    updateResetButtonState();
	
}


void FCPropertyViewItem::Update(void)
{
	puts("Update");


	/*
	// quieck an dirty
	if(_pcDocument && _hcLabel->GetOCCLabel().HasChild() && !isOpen())
	{
		//for(QListViewItem* pItem = firstChild (); pItem!=0 ; pItem = pItem->nextSibling () )
		//	delete pItem;

		std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

		for(std::vector<FCPyHandle<FCLabel> >::iterator It=vpcLabels.begin();It != vpcLabels.end(); It++)
		{
			new FCTreeLabel(this,*It);
		}

		setPixmap(0,*FCTree::pcLabelOpen);

	}	
*/

}

void FCPropertyViewItem::BuildUp(void)
{
/*
	std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

	for(std::vector<FCPyHandle<FCLabel> >::reverse_iterator It=vpcLabels.rbegin();It != vpcLabels.rend(); It++)
	{
		new FCTreeLabel(this,*It);
	}

	TDF_Label l = _hcLabel->GetOCCLabel();
	
	if( l.IsAttribute(TNaming_NamedShape::GetID()) )
		(new QListViewItem(this,"Shape","1"))->setPixmap(0,*FCTree::pcAttribute);
	if( l.IsAttribute(TDataStd_Integer::GetID()) )
		(new QListViewItem(this,"Int","1"))->setPixmap(0,*FCTree::pcAttribute);
	if( l.IsAttribute(TDataStd_Name::GetID()) )
		(new QListViewItem(this,"String","1"))->setPixmap(0,*FCTree::pcAttribute);


	if(childCount()>0)
		setExpandable( TRUE );*/
}

void FCPropertyViewItem::setOpen( bool b )
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
	puts("setOpen");

/*	if(o)
	{
		setPixmap(0,*FCTree::pcLabelOpen);

		Update();
	
	}else{
		setPixmap(0,*FCTree::pcLabelClosed);
	}

	QListViewItem::setOpen ( o );
*/	
}


void FCPropertyViewItem::setup()
{
    //setExpandable( TRUE );
    QListViewItem::setup();
    setHeight( QListViewItem::height() + 2 );
}

void FCPropertyViewItem::activate ()
{
	puts("Activated");
}


void FCPropertyViewItem::setValue( const QVariant &v )
{
    val = v;
}

QVariant FCPropertyViewItem::value() const
{
    return val;
}

QString FCPropertyViewItem::name() const
{
    return propertyName;
}

void FCPropertyViewItem::notifyValueChange()
{
    if ( !propertyParent() ) {
	listview->valueChanged( this );
	setChanged( TRUE );
	if ( hasSubItems())
	    initChildren();
    } else {
	propertyParent()->childValueChanged( this );
	setChanged( TRUE );
    }
}

FCPropertyViewItem *FCPropertyViewItem::propertyParent() const
{
    return property;
}

void FCPropertyViewItem::childValueChanged( FCPropertyViewItem * )
{
}

bool FCPropertyViewItem::hasCustomContents() const
{
    return FALSE;
}

void FCPropertyViewItem::drawCustomContents( QPainter *, const QRect & )
{
}

bool FCPropertyViewItem::hasSubItems() const
{
    return FALSE;
}


void FCPropertyViewItem::showEditor()
{
    createResetButton();
    resetButton->parentWidget()->show();
}

void FCPropertyViewItem::hideEditor()
{
    createResetButton();
    resetButton->parentWidget()->hide();
}

void FCPropertyViewItem::createResetButton()
{
    if ( resetButton ) {
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
    QObject::connect( resetButton, SIGNAL( clicked() ),
		      listview, SLOT( resetProperty() ) );
    QToolTip::add( resetButton, FCPropertyView::tr( "Reset property to its default value" ) );
    QWhatsThis::add( resetButton, FCPropertyView::tr( "Click this button to reset the property to its default value" ) );
    updateResetButtonState();
}

void FCPropertyViewItem::updateResetButtonState()
{
    if ( !resetButton )
	return;
//    if ( propertyParent() || !WidgetFactory::canResetProperty( listview->propertyEditor()->widget(), name() ) )
//	resetButton->setEnabled( FALSE );
//    else
	resetButton->setEnabled( isChanged() );
}

void FCPropertyViewItem::setFocus( QWidget *w )
{
    QWorkspace *ws = 0;
    QWidget *wid = listview->propertyView();
    while ( wid ) {
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

void FCPropertyViewItem::toggle()
{
}

void FCPropertyViewItem::placeEditor( QWidget *w )
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
    listview->moveChild( w, r.x(), r.y() );
    resetButton->parentWidget()->resize( resetButton->sizeHint().width() + 10, r.height() );
    listview->moveChild( resetButton->parentWidget(), r.x() + r.width() - 8, r.y() );
    resetButton->setFixedHeight( r.height() - 3 );
}

bool FCPropertyViewItem::isOpen() const
{
    return open;
}

QString FCPropertyViewItem::currentItemFromObject() const
{
    return QString::null;
}

int FCPropertyViewItem::currentIntItemFromObject() const
{
    return -1;
}

void FCPropertyViewItem::setCurrentItem( const QString & )
{
}

void FCPropertyViewItem::setCurrentItem( int )
{
}

int FCPropertyViewItem::currentIntItem() const
{
    return -1;
}

QString FCPropertyViewItem::currentItem() const
{
    return QString::null;
}

void FCPropertyViewItem::initChildren()
{
}

void FCPropertyViewItem::createChildren()
{
}

FCPropertyViewItem *FCPropertyViewItem::child( int i ) const
{
    return ( (FCPropertyViewItem*)this )->children.at( i );
}

void FCPropertyViewItem::addChild( FCPropertyViewItem *i )
{
    children.append( i );
}

int FCPropertyViewItem::childCount() const
{
    return children.count();
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
    setCaption( tr( "Property Editor" ) );
//    setIcon( PixmapChooser::loadPixmap( "logo" ) );
    wid = 0;
//    formwindow = 0;

	_pcListView = new FCPropertyListView(this/*,name*/);

	// set defaults and the colums
	_pcListView->setSorting(-1,false);
	_pcListView->addColumn("Property");
	_pcListView->setColumnWidthMode(0,QListView::Manual);
	_pcListView->addColumn("Value");
	_pcListView->setColumnWidthMode(1,QListView::Manual );
	_pcListView->setColumnWidth(0,100);
	_pcListView->setColumnWidth(1,100);

	// retreive the Pixmaps
	pcLabelOpen   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_Attr"));


	FCPropertyViewItem* pcItem;
	
  QString s;
//	pcItem = new FCPropertyViewItem(_pcListView, NULL, NULL, s);
//	pcItem->setText(0,"Property");
//	pcItem->setText(1,"Value");

  s = "Integer";
	pcItem = new FCPropertyViewIntItem ( _pcListView, NULL,   NULL, s, true);
	pcItem = new FCPropertyViewIntItem ( _pcListView, pcItem, NULL, s, false);
  pcItem = new FCPropertyViewBoolItem( _pcListView, pcItem, NULL, s  );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, false, false, false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, false, false, true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, false, true , false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, false, true , true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, true , false, false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, true , false, true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, true , true , false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, false, true , true , true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , false, false, false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , false, false, true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , false, true , false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , false, true , true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , true , false, false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , true , false, true );
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , true , true , false);
  pcItem = new FCPropertyViewTextItem( _pcListView, pcItem, NULL, s, true , true , true , true );
  pcItem = new FCPropertyViewCoordItem( _pcListView, pcItem, NULL, s, FCPropertyViewCoordItem::Rect );
  pcItem = new FCPropertyViewCoordItem( _pcListView, pcItem, NULL, s, FCPropertyViewCoordItem::Size );
  pcItem = new FCPropertyViewCoordItem( _pcListView, pcItem, NULL, s, FCPropertyViewCoordItem::Point );
  pcItem = new FCPropertyViewColorItem     ( _pcListView, pcItem, NULL, s, false );
  pcItem = new FCPropertyViewColorItem     ( _pcListView, pcItem, NULL, s, true  );
  pcItem = new FCPropertyViewListItem      ( _pcListView, pcItem, NULL, s, true  );
  pcItem = new FCPropertyViewListItem      ( _pcListView, pcItem, NULL, s, false );
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
//    _pcListView->setupProperties();
    _pcListView->viewport()->setUpdatesEnabled( TRUE );
    qApp->processEvents();
    _pcListView->updateEditorSize();
}

void FCPropertyView::resetFocus()
{
    if ( _pcListView->currentItem() )
	( (FCPropertyViewItem*)_pcListView->currentItem() )->showEditor();
}

void FCPropertyView::refetchData()
{
    _pcListView->refetchData();
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

QString FCPropertyView::classOfCurrentProperty() const
{
//    if ( !wid )
//	return QString::null;
//    QObject *o = wid;
//    QString curr = currentProperty();
//    QMetaObject *mo = o->metaObject();
//    while ( mo ) {
//	QStrList props = mo->propertyNames( FALSE );
//	if ( props.find( curr.latin1() ) != -1 )
//	    return mo->className();
//	mo = mo->superClass();
//    }
    return QString::null;
}

void FCPropertyView::Update(void)
{
	GetConsole().Log("Property Updated\n");
	
	// quick and dirty so far
//	delete _pcMainItem;
//	_pcMainItem = new FCTreeLabel(this);

}

void FCPropertyView::OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument)
{
//	GetConsole().Log("Tree doc activated %p\n",pcNewDocument);

	if(pcOldDocument != pcNewDocument)
	{
//		delete _pcMainItem;
//		_pcMainItem = new FCTreeLabel(this);
	}

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
	//printf("MsgTree: %s View: %p\n",pMsg,this);

	// no messages yet
	return false;
}



//**************************************************************************
// separator for other implemetation aspects







// --------------------------------------------------------------
FCPropertyViewIntItem::FCPropertyViewIntItem(FCPropertyView * parent)
: FCPropertyViewItem(parent)
{
  listview = parent->_pcListView;
  spinBx = 0;
}

FCPropertyViewIntItem::FCPropertyViewIntItem(FCPropertyViewItem * parent)
: FCPropertyViewItem(parent)
{
  spinBx = 0;
}


FCPropertyViewIntItem::FCPropertyViewIntItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop, const QString &propName, bool s)
  : FCPropertyViewItem( l, after, prop, propName ), signedValue( s )
{
    spinBx = 0;
}

QSpinBox *FCPropertyViewIntItem::spinBox()
{
    if ( spinBx )
	return spinBx;
    if ( signedValue )
	spinBx = new FCSpinBox( -INT_MAX, INT_MAX, 1, listview->viewport() );
    else
	spinBx = new FCSpinBox( 0, INT_MAX, 1, listview->viewport() );
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

FCPropertyViewIntItem::~FCPropertyViewIntItem()
{
    delete (FCSpinBox*)spinBx;
    spinBx = 0;
}

void FCPropertyViewIntItem::showEditor()
{
    FCPropertyViewItem::showEditor();
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

void FCPropertyViewIntItem::hideEditor()
{
    FCPropertyViewItem::hideEditor();
    spinBox()->hide();
}

void FCPropertyViewIntItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() ) && value() == v )
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
    FCPropertyViewItem::setValue( v );
}

void FCPropertyViewIntItem::setValue()
{
    if ( !spinBx )
	return;
    setText( 1, QString::number( spinBox()->value() ) );
    if ( signedValue )
	FCPropertyViewItem::setValue( spinBox()->value() );
    else
	FCPropertyViewItem::setValue( (uint)spinBox()->value() );
    notifyValueChange();
}

/////////////////////////////////////////////////////////////////////////////

FCPropertyListView::FCPropertyListView( FCPropertyView *view )
    : QListView( view ), propView( view )
{
    header()->setMovingEnabled( FALSE );
    viewport()->setBackgroundMode( PaletteBackground );
    setResizePolicy( QScrollView::Manual );
    addColumn( tr( "Property" ) );
    addColumn( tr( "Value" ) );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( updateEditorSize() ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );
    connect( this, SIGNAL( pressed( QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( itemPressed( QListViewItem *, const QPoint &, int ) ) );
    connect( this, SIGNAL( doubleClicked( QListViewItem * ) ),
	     this, SLOT( toggleOpen( QListViewItem * ) ) );
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
    viewport()->repaint( header()->sectionPos(1) + os - elipsis, 0, elipsis, viewport()->height(), FALSE );
    if ( currentItem() )
	( ( FCPropertyViewItem* )currentItem() )->showEditor();
}

void FCPropertyListView::paintEmptyArea( QPainter *p, const QRect &r )
{
  p->fillRect( r, FCPropertyView::cBackColor2 );
}

void FCPropertyListView::setCurrentItem( QListViewItem *i )
{
    if ( currentItem() )
	( (FCPropertyViewItem*)currentItem() )->hideEditor();
    QListView::setCurrentItem( i );
    ( (FCPropertyViewItem*)currentItem() )->showEditor();
}

void FCPropertyListView::updateEditorSize()
{
    QSize s( header()->sectionPos(1) + header()->sectionSize(1), height() );
    QResizeEvent e( s, size() );
    resizeEvent( &e );
    viewport()->repaint( s.width(), 0, width() - s.width(), height(), FALSE );
}

void FCPropertyListView::valueChanged( FCPropertyViewItem *i )
{
//    if ( !editor->widget() )
//	return;
//    QString pn( tr( "Set '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
//    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
//						      editor->widget(), editor,
//						      i->name(), WidgetFactory::property( editor->widget(), i->name() ),
//						      i->value(), i->currentItem(), i->currentItemFromObject() );
//    cmd->execute();
//    editor->formWindow()->commandHistory()->addCommand( cmd, TRUE );
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
    if ( pi->hasSubItems() ) {
	pi->setOpen( !pi->isOpen() );
    } else {
	pi->toggle();
    }
}

void FCPropertyListView::resetProperty()
{
    if ( !currentItem() )
	return;
    FCPropertyViewItem *i = (FCPropertyViewItem*)currentItem();
//    if ( !MetaDataBase::isPropertyChanged( editor->widget(), i->PropertyItem::name() ) )
//	return;
//    QString pn( tr( "Reset '%1' of '%2'" ).arg( i->name() ).arg( editor->widget()->name() ) );
//    SetPropertyCommand *cmd = new SetPropertyCommand( pn, editor->formWindow(),
//						      editor->widget(), editor,
//						      i->name(), i->value(),
//						      WidgetFactory::defaultValue( editor->widget(), i->name() ),
//						      WidgetFactory::defaultCurrentItem( editor->widget(), i->name() ),
//						      i->currentItem(), TRUE );
//    cmd->execute();
//    editor->formWindow()->commandHistory()->addCommand( cmd, FALSE );
    if ( i->hasSubItems() )
	i->initChildren();
}

bool FCPropertyListView::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
	return TRUE;

    FCPropertyViewItem *i = (FCPropertyViewItem*)currentItem();
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
    } else if ( e->type() == QEvent::FocusOut && o->inherits( "QLineEdit" ) ) {
//  	QTimer::singleShot( 100, editor->formWindow()->commandHistory(), SLOT( checkCompressedCommand() ) );
    }

    return QListView::eventFilter( o, e );
}

FCPropertyView *FCPropertyListView::propertyView() const
{
    return propView;
}

void FCPropertyListView::refetchData()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
	FCPropertyViewItem *i = (FCPropertyViewItem*)it.current();
	if ( !i->propertyParent() )
	    setPropertyValue( i );
//	if ( i->hasSubItems() )
//	    i->initChildren();
//	bool changed = MetaDataBase::isPropertyChanged( editor->widget(), i->name() );
//	if ( changed != i->isChanged() )
//	    i->setChanged( changed, FALSE );
    }
    updateEditorSize();
}

void FCPropertyListView::setPropertyValue( FCPropertyViewItem *i )
{/*
    const QMetaProperty *p = editor->widget()->metaObject()->property( i->name(), TRUE );
    if ( !p ) {
	if ( i->name() == "hAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->property( "alignment", TRUE );
	    align &= AlignLeft | AlignHCenter | AlignRight;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).first() );
	} else if ( i->name() == "vAlign" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    p = editor->widget()->metaObject()->property( "alignment", TRUE );
	    align &= AlignTop | AlignVCenter | AlignBottom;
	    ( (PropertyListItem*)i )->setCurrentItem( p->valueToKeys( align ).first() );
	} else if ( i->name() == "wordwrap" ) {
	    int align = editor->widget()->property( "alignment" ).toInt();
	    if ( align & WordBreak )
		i->setValue( QVariant( TRUE, 0 ) );
	    else
		i->setValue( QVariant( FALSE, 0 ) );
	} else if ( i->name() == "layoutSpacing" ) {
	    ( (PropertyIntItem*)i )->setValue( MetaDataBase::spacing( WidgetFactory::containerOfWidget( editor->widget() ) ) );
	} else if ( i->name() == "layoutMargin" ) {
	    ( (PropertyIntItem*)i )->setValue( MetaDataBase::margin( WidgetFactory::containerOfWidget( editor->widget() ) ) );
	} else if ( i->name() == "toolTip" || i->name() == "whatsThis" ) {
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	} else if ( editor->widget()->inherits( "CustomWidget" ) ) {
	    MetaDataBase::CustomWidget *cw = ( (CustomWidget*)editor->widget() )->customWidget();
	    if ( !cw )
		return;
	    i->setValue( MetaDataBase::fakeProperty( editor->widget(), i->name() ) );
	}
	return;
    }
    if ( p->isSetType() )
	;
    else if ( p->isEnumType() )
	( (PropertyListItem*)i )->setCurrentItem( p->valueToKey( editor->widget()->property( i->name() ).toInt() ) );
    else
	i->setValue( editor->widget()->property( i->name() ) );*/
}

// --------------------------------------------------------------

FCPropertyViewBoolItem::FCPropertyViewBoolItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop, const QString &propName )
    : FCPropertyViewItem( l, after, prop, propName )
{
    comb = 0;
}

QComboBox *FCPropertyViewBoolItem::combo()
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

FCPropertyViewBoolItem::~FCPropertyViewBoolItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void FCPropertyViewBoolItem::toggle()
{
    bool b = value().toBool();
    setValue( QVariant( !b, 0 ) );
    setValue();
}

void FCPropertyViewBoolItem::showEditor()
{
    FCPropertyViewItem::showEditor();
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

void FCPropertyViewBoolItem::hideEditor()
{
    FCPropertyViewItem::hideEditor();
    combo()->hide();
}

void FCPropertyViewBoolItem::setValue( const QVariant &v )
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
    FCPropertyViewItem::setValue( v );
}

void FCPropertyViewBoolItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    bool b = combo()->currentItem() == 0 ? (bool)FALSE : (bool)TRUE;
    FCPropertyViewItem::setValue( QVariant( b, 0 ) );
    notifyValueChange();
}

// --------------------------------------------------------------

FCPropertyViewTextItem::FCPropertyViewTextItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
				    const QString &propName, bool comment, bool multiLine, bool ascii, bool a )
    : FCPropertyViewItem( l, after, prop, propName ), withComment( comment ),
      hasMultiLines( multiLine ), asciiOnly( ascii ), accel( a )
{
    lin = 0;
    box = 0;
}

QLineEdit *FCPropertyViewTextItem::lined()
{
    if ( lin )
	return lin;
    if ( hasMultiLines ) {
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

//    if ( asciiOnly )
//	lin->setValidator( new AsciiValidator( lin, "ascii_validator" ) );
    if ( !hasMultiLines ) {
	lin->hide();
    } else {
	button = new QPushButton( tr("..."), box );
	button->setFixedWidth( 20 );
	connect( button, SIGNAL( clicked() ),
		 this, SLOT( getText() ) );
	lin->setFrame( FALSE );
    }
    connect( lin, SIGNAL( returnPressed() ),
	     this, SLOT( setValue() ) );
    connect( lin, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( setValue() ) );
    if ( FCPropertyViewItem::name() == "name" )
//	connect( lin, SIGNAL( returnPressed() ),
//		 listview->propertyEditor()->formWindow()->commandHistory(),
//		 SLOT( checkCompressedCommand() ) );
    lin->installEventFilter( listview );
    return lin;
}

FCPropertyViewTextItem::~FCPropertyViewTextItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
    delete (QHBox*)box;
    box = 0;
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

void FCPropertyViewTextItem::childValueChanged( FCPropertyViewItem *child )
{
//    MetaDataBase::setPropertyComment( listview->propertyEditor()->widget(), PropertyItem::name(), child->value().toString() );
}

static QString to_string( const QVariant &v, bool accel )
{
    if ( !accel )
	return v.toString();
    return QAccel::keyToString( v.toInt() );
}

void FCPropertyViewTextItem::showEditor()
{
    FCPropertyViewItem::showEditor();
    if ( !lin ) {
	lined()->blockSignals( TRUE );
	lined()->setText( to_string( value(), accel ) );
	lined()->blockSignals( FALSE );
    }
    QWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w= lined();

    placeEditor( w );
    if ( !w->isVisible() || !lined()->hasFocus() ) {
	w->show();
	setFocus( lined() );
    }
}

void FCPropertyViewTextItem::createChildren()
{
    FCPropertyViewTextItem *i = new FCPropertyViewTextItem( listview, this, this, tr( "comment" ), FALSE, FALSE );
    i->lined()->setEnabled( isChanged() );
    addChild( i );
}

void FCPropertyViewTextItem::initChildren()
{
    if ( !childCount() )
	return;
    FCPropertyViewItem *item = FCPropertyViewItem::child( 0 );
//    if ( item )
//	item->setValue( MetaDataBase::propertyComment( listview->propertyEditor()->widget(), PropertyItem::name() ) );
}

void FCPropertyViewTextItem::hideEditor()
{
    FCPropertyViewItem::hideEditor();
    QWidget* w;
    if ( hasMultiLines )
	w = box;
    else
	w = lined();

    w->hide();
}

void FCPropertyViewTextItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;
    if ( lin ) {
	lined()->blockSignals( TRUE );
	int oldCursorPos;
	oldCursorPos = lin->cursorPosition();
	lined()->setText( to_string( v, accel ) );
	lin->setCursorPosition( oldCursorPos );
	lined()->blockSignals( FALSE );
    }
    setText( 1, to_string( v, accel ) );
    FCPropertyViewItem::setValue( v );
}

void FCPropertyViewTextItem::setValue()
{
    setText( 1, lined()->text() );
    QVariant v;
    if ( accel )
	v = QAccel::stringToKey( lined()->text() );
    else
	v = lined()->text();
    FCPropertyViewItem::setValue( v );
    notifyValueChange();
}

void FCPropertyViewTextItem::getText()
{
    QString txt = "kpkfpw";//TextEditor::getText( listview, value().toString() );
    if ( !txt.isEmpty() ) {
	setText( 1, txt );
	FCPropertyViewItem::setValue( txt );
	notifyValueChange();
	lined()->blockSignals( TRUE );
	lined()->setText( txt );
	lined()->blockSignals( FALSE );
    }
}

// --------------------------------------------------------------

FCPropertyViewCoordItem::FCPropertyViewCoordItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
				    const QString &propName, Type t )
    : FCPropertyViewItem( l, after, prop, propName ), typ( t )
{
    lin = 0;

}

QLineEdit *FCPropertyViewCoordItem::lined()
{
    if ( lin )
	return lin;
    lin = new QLineEdit( listview->viewport() );
    lin->setReadOnly( TRUE );
    lin->installEventFilter( listview );
    lin->hide();
    return lin;
}

void FCPropertyViewCoordItem::createChildren()
{
    FCPropertyViewItem *i = this;
    if ( typ == Rect || typ == Point ) {
	i = new FCPropertyViewIntItem( listview, i, this, tr( "x" ), TRUE );
	addChild( i );
	i = new FCPropertyViewIntItem( listview, i, this, tr( "y" ), TRUE );
	addChild( i );
    }
    if ( typ == Rect || typ == Size ) {
	i = new FCPropertyViewIntItem( listview, i, this, tr( "width" ), TRUE );
	addChild( i );
	i = new FCPropertyViewIntItem( listview, i, this, tr( "height" ), TRUE );
	addChild( i );
    }
}

void FCPropertyViewCoordItem::initChildren()
{
    FCPropertyViewItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = FCPropertyViewItem::child( i );
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

FCPropertyViewCoordItem::~FCPropertyViewCoordItem()
{
    delete (QLineEdit*)lin;
    lin = 0;
}

void FCPropertyViewCoordItem::showEditor()
{
    FCPropertyViewItem::showEditor();
    if ( !lin )
	lined()->setText( text( 1 ) );
    placeEditor( lined() );
    if ( !lined()->isVisible() || !lined()->hasFocus()  ) {
	lined()->show();
	setFocus( lined() );
    }
}

void FCPropertyViewCoordItem::hideEditor()
{
    FCPropertyViewItem::hideEditor();
    lined()->hide();
}

void FCPropertyViewCoordItem::setValue( const QVariant &v )
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
    FCPropertyViewItem::setValue( v );
}

bool FCPropertyViewCoordItem::hasSubItems() const
{
    return TRUE;
}

void FCPropertyViewCoordItem::childValueChanged( FCPropertyViewItem *child )
{
    if ( typ == Rect ) {
	QRect r = value().toRect();
	if ( child->name() == tr( "x" ) )
	    r.setX( child->value().toInt() );
	else if ( child->name() == tr( "y" ) )
	    r.setY( child->value().toInt() );
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

FCPropertyViewListItem::FCPropertyViewListItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
				    const QString &propName, bool e )
    : FCPropertyViewItem( l, after, prop, propName ), editable( e )
{
    comb = 0;
    oldInt = -1;
}

QComboBox *FCPropertyViewListItem::combo()
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

FCPropertyViewListItem::~FCPropertyViewListItem()
{
    delete (QComboBox*)comb;
    comb = 0;
}

void FCPropertyViewListItem::showEditor()
{
    FCPropertyViewItem::showEditor();
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

void FCPropertyViewListItem::hideEditor()
{
    FCPropertyViewItem::hideEditor();
    combo()->hide();
}

void FCPropertyViewListItem::setValue( const QVariant &v )
{
    if ( comb ) {
	combo()->blockSignals( TRUE );
	combo()->clear();
	combo()->insertStringList( v.toStringList() );
	combo()->blockSignals( FALSE );
    }
    setText( 1, v.toStringList().first() );
    FCPropertyViewItem::setValue( v );
}

void FCPropertyViewListItem::setValue()
{
    if ( !comb )
	return;
    setText( 1, combo()->currentText() );
    QStringList lst;
    for ( uint i = 0; i < combo()->listBox()->count(); ++i )
	lst << combo()->listBox()->item( i )->text();
    FCPropertyViewItem::setValue( lst );
    notifyValueChange();
    oldInt = currentIntItem();
    oldString = currentItem();
}

QString FCPropertyViewListItem::currentItem() const
{
    return ( (FCPropertyViewListItem*)this )->combo()->currentText();
}

void FCPropertyViewListItem::setCurrentItem( const QString &s )
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

void FCPropertyViewListItem::setCurrentItem( int i )
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

int FCPropertyViewListItem::currentIntItem() const
{
    return ( (FCPropertyViewListItem*)this )->combo()->currentItem();
}

int FCPropertyViewListItem::currentIntItemFromObject() const
{
    return oldInt;
}

QString FCPropertyViewListItem::currentItemFromObject() const
{
    return oldString;
}

// --------------------------------------------------------------

FCPropertyViewColorItem::FCPropertyViewColorItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
				      const QString &propName, bool children )
    : FCPropertyViewItem( l, after, prop, propName ), withChildren( children )
{
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
    connect( button, SIGNAL( clicked() ),
	     this, SLOT( getColor() ) );
}

void FCPropertyViewColorItem::createChildren()
{
    FCPropertyViewItem *i = this;
    i = new FCPropertyViewIntItem( listview, i, this, tr( "Red" ), TRUE );
    addChild( i );
    i = new FCPropertyViewIntItem( listview, i, this, tr( "Green" ), TRUE );
    addChild( i );
    i = new FCPropertyViewIntItem( listview, i, this, tr( "Blue" ), TRUE );
    addChild( i );
}

void FCPropertyViewColorItem::initChildren()
{
    FCPropertyViewItem *item = 0;
    for ( int i = 0; i < childCount(); ++i ) {
	item = FCPropertyViewItem::child( i );
	if ( item->name() == tr( "Red" ) )
	    item->setValue( val.toColor().red() );
	else if ( item->name() == tr( "Green" ) )
	    item->setValue( val.toColor().green() );
	else if ( item->name() == tr( "Blue" ) )
	    item->setValue( val.toColor().blue() );
    }
}

FCPropertyViewColorItem::~FCPropertyViewColorItem()
{
    delete (QHBox*)box;
}

void FCPropertyViewColorItem::showEditor()
{
    FCPropertyViewItem::showEditor();
    placeEditor( box );
    if ( !box->isVisible() )
	box->show();
}

void FCPropertyViewColorItem::hideEditor()
{
    FCPropertyViewItem::hideEditor();
    box->hide();
}

void FCPropertyViewColorItem::setValue( const QVariant &v )
{
    if ( ( !hasSubItems() || !isOpen() )
	 && value() == v )
	return;

    QString s;
    setText( 1, v.toColor().name() );
    colorPrev->setBackgroundColor( v.toColor() );
    FCPropertyViewItem::setValue( v );
}

bool FCPropertyViewColorItem::hasSubItems() const
{
    return withChildren;
}

void FCPropertyViewColorItem::childValueChanged( FCPropertyViewItem *child )
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

void FCPropertyViewColorItem::getColor()
{
    QColor c = QColorDialog::getColor( val.asColor(), listview );
    if ( c.isValid() ) {
	setValue( c );
	notifyValueChange();
    }
}

bool FCPropertyViewColorItem::hasCustomContents() const
{
    return TRUE;
}

void FCPropertyViewColorItem::drawCustomContents( QPainter *p, const QRect &r )
{
    p->save();
    p->setPen( QPen( black, 1 ) );
    p->setBrush( val.toColor() );
    p->drawRect( r.x() + 2, r.y() + 2, r.width() - 5, r.height() - 5 );
    p->restore();
}

#include "moc_PropertyView.cpp"


