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
#include "PropertyView.h"

#include "Application.h"
#include "../Base/Console.h"





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
//	indent = 20 + ( property ? 20 : 0 );
	indent = 20 ;
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

/*
    if ( !hasCustomContents() || column != 1 ) {
	QListViewItem::paintCell( p, g, column, width - indent, align  );
    } else {
	p->fillRect( 0, 0, width, height(), backgroundColor() );
	drawCustomContents( p, QRect( 0, 0, width, height() ) );
    }
*/
    if ( isChanged() && column == 0 )
		p->restore();
    if ( column == 0 )
		p->restore();

/*    if ( hasSubItems() && column == 0 ) {
		p->save();
		p->setPen( cg.foreground() );
		p->setBrush( cg.base() );
		p->drawRect( 5, height() / 2 - 4, 9, 9 );
		p->drawLine( 7, height() / 2, 11, height() / 2 );
		if ( !isOpen() )
			p->drawLine( 9, height() / 2 - 2, 9, height() / 2 + 2 );
		p->restore();
    }*/
    p->save();
    p->setPen( QPen( cg.dark(), 1 ) );
    p->drawLine( 0, height() - 1, width, height() - 1 );
    p->drawLine( width - 1, 0, width - 1, height() );
    p->restore();

/*    if ( listview->currentItem() == this && column == 0 &&
	     !listview->hasFocus() && !listview->viewport()->hasFocus() )
		paintFocus( p, cg, QRect( 0, 0, width, height() ) );*/
}

void FCPropertyViewItem::paintBranches( QPainter * p, const QColorGroup & cg,
				  int w, int y, int h, GUIStyle s )
{
    QColorGroup g( cg );
    g.setColor( QColorGroup::Base, backgroundColor() );
    QListViewItem::paintBranches( p, g, w, y, h, s );
}	

void FCPropertyViewItem::paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r )
{
    p->save();
    QApplication::style().drawPanel( p, r.x(), r.y(), r.width(), r.height(), cg, TRUE, 1 );
    p->restore();
}

void FCPropertyViewItem::updateBackColor()
{
	backColor = FCPropertyView::cBackColor1;

    if ( itemAbove() /*&& this != listview->firstChild()*/ ) 
	{
		if ( ( ( FCPropertyViewItem*)itemAbove() )->backColor == FCPropertyView::cBackColor1 )
			backColor = FCPropertyView::cBackColor2;
    } 
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
	/*
    if ( propertyParent() )
	return;
    if ( changed == b )
	return;
    changed = b;
    repaint();
    if ( updateDb )
	MetaDataBase::setPropertyChanged( listview->propertyEditor()->widget(), name(), changed );
    updateResetButtonState();
	*/
}


void FCPropertyViewItem::Update(void)
{
	puts("Updtate");


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

void FCPropertyViewItem::setOpen( bool o )
{
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
}

void FCPropertyViewItem::activate ()
{
	puts("Activated");
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

	_pcListView = new QListView(this,name);

	// set defaults and the colums
	_pcListView->setSorting(-1,false);
	_pcListView->addColumn("Property");
	_pcListView->setColumnWidthMode(0,QListView::Maximum);
	_pcListView->addColumn("Value");
	_pcListView->setColumnWidthMode(1,QListView::Manual );

	// retreive the Pixmaps
	pcLabelOpen   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_Attr"));


	FCPropertyViewItem* pcItem;
	
	pcItem = new FCPropertyViewItem(this);
	pcItem->setText(1,"Test");
	pcItem->setText(2,"Test");

	pcItem = new FCPropertyViewItem(this);
	pcItem = new FCPropertyViewItem(this);
	pcItem = new FCPropertyViewItem(this);
	pcItem = new FCPropertyViewItem(this);
	pcItem = new FCPropertyViewItem(this);
	pcItem = new FCPropertyViewItem(this);

	

	// Add the first main label
//	_pcMainItem = new FCTreeLabel(this);

	//_pcListView->setRootIsDecorated(true);



	//_pcListView->setSize(200,400);
    resize( 200, 400 );

}

FCPropertyView::~FCPropertyView()
{

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






