/** \file Tree.cpp
 *  \brief The console module
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    5.2001
 *  \bug Nothing known
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
#	include <TDataStd_Integer.hxx>
#	include <TDataStd_Name.hxx>
#	include <TNaming_NamedShape.hxx>
#	include <qheader.h>
#endif

#include "Tree.h"
#include "Document.h"
#include "BitmapFactory.h"

#include "../App/Document.h"
#include "../App/Label.h"

#include "../Base/Console.h"


GUIDDefs AttrNames[] = {
	{0,0}
};


FCTreeLabel::FCTreeLabel( FCTree * parent)
	:QListViewItem(parent->_pcListView),
	 _pcDocument(parent->_pcDocument)
{
	if(_pcDocument){
		setText(0,QObject::tr("Main Label"));
		setPixmap(0,*FCTree::pcLabelOpen);
//		_hcLabel = parent->_pcDocument->GetDocument()->Main();
		_hcTDFLabel = parent->_pcDocument->GetDocument()->Main();
		BuildUp();
		setOpen(true);
	}else{
		setPixmap(0,*FCTree::pcLabelClosed);
		//setPixmap(new QPixmap(px));
		setText(0,QObject::tr("No Active Document"));
	}


}

/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
FCTreeLabel::FCTreeLabel( FCTreeLabel * parent, TDF_Label &hcLabel )
    : QListViewItem( parent ),
	_hcTDFLabel(hcLabel),
	_pcDocument(parent->_pcDocument)
{
	QString cString;

	cString.sprintf("Tag:%d",_hcTDFLabel.Tag());
	setPixmap(0,*FCTree::pcLabelClosed);
	setText(0,cString);

	BuildUp();

}

void FCTreeLabel::Update(void)
{
	//puts("Updtate");


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

void FCTreeLabel::BuildUp(void)
{

/*	std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

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
		setExpandable( TRUE );
*/
  }

void FCTreeLabel::setOpen( bool o )
{
	//puts("setOpen");

	if(o)
	{
		setPixmap(0,*FCTree::pcLabelOpen);

		Update();

	}else{
		setPixmap(0,*FCTree::pcLabelClosed);
	}

	QListViewItem::setOpen ( o );

}


void FCTreeLabel::setup()
{
    //setExpandable( TRUE );
    QListViewItem::setup();
}

void FCTreeLabel::activate ()
{
	//puts("Activated");
}




QPixmap* FCTree::pcLabelOpen=0;
QPixmap* FCTree::pcLabelClosed=0;
QPixmap* FCTree::pcAttribute=0;


FCTree::FCTree(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:FCDockView(pcDocument,parent,name)
{

	_pcListView = new QListView(this,name);

	// set defaults and the colums
	_pcListView->setSorting(-1,false);
	_pcListView->addColumn(tr("Labels & Attr."));
	_pcListView->setColumnWidthMode(0,QListView::Maximum);
	_pcListView->addColumn(tr("Value"));
	_pcListView->setColumnWidthMode(1,QListView::Manual );

	// retreive the Pixmaps
	pcLabelOpen   = new QPixmap(Gui::BitmapFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(Gui::BitmapFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(Gui::BitmapFactory().GetPixmap("RawTree_Attr"));


	// Add the first main label
	_pcMainItem = new FCTreeLabel(this);

	//_pcListView->setRootIsDecorated(true);



	//_pcListView->setSize(200,400);
    resize( 200, 400 );

}

void FCTree::Update(void)
{
	Base::Console().Log("Tree Updated\n");
	
	// quick and dirty so far
	delete _pcMainItem;
	_pcMainItem = new FCTreeLabel(this);

}

void FCTree::OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument)
{
//	Console().Log("Tree doc activated %p\n",pcNewDocument);

	if(pcOldDocument != pcNewDocument)
	{
		delete _pcMainItem;
		_pcMainItem = new FCTreeLabel(this);
	}

}


void FCTree::resizeEvent ( QResizeEvent * e) 
{
	// routing the resize event to the child

 	_pcListView->resize(e->size());

    QSize vs = _pcListView->viewportSize( 0, _pcListView->contentsHeight() );

	_pcListView->header()->sectionSize( 1 );
    int ns = vs.width() - _pcListView->header()->sectionSize( 0 );
    if ( ns < 16 )
	ns = 16;
	
    _pcListView->header()->resizeSection( 1, ns );
    _pcListView->header()->repaint( _pcListView->header()->width() - _pcListView->header()->sectionSize( 1 ), 0, _pcListView->header()->sectionSize( 1 ), _pcListView->header()->height() );


}

bool FCTree::OnMsg(const char* pMsg)
{
	//printf("MsgTree: %s View: %p\n",pMsg,this);

	// no messages yet
	return false;
}


#include "moc_Tree.cpp"

