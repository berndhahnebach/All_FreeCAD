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

#endif

#include "Application.h"
#include "Document.h"
#include "../App/Document.h"
#include "Tree.h"



GUIDDefs AttrNames[] = {
	{0,0}
};


FCTreeLabel::FCTreeLabel( FCTree * parent)
	:QListViewItem(parent->_pcListView),
	 _pcDocument(parent->_pcDocument)
{
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

	
}

/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
FCTreeLabel::FCTreeLabel( FCTreeLabel * parent, FCPyHandle<FCLabel> &hcLabel )
    : QListViewItem( parent ),
	_pcDocument(parent->_pcDocument),
	_hcLabel(hcLabel)
{
	QString cString;

	cString.sprintf("Tag:%d",_hcLabel->GetOCCLabel().Tag());
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
		setExpandable( TRUE );
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
	_pcListView->addColumn("Labels & Attr.");
	_pcListView->setColumnWidthMode(0,QListView::Maximum);
	_pcListView->addColumn("Value");
	_pcListView->setColumnWidthMode(1,QListView::Manual );

	// retreive the Pixmaps
	pcLabelOpen   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_Attr"));


	// Add the first main label
	_pcMainItem = new FCTreeLabel(this);

	//_pcListView->setRootIsDecorated(true);



	//_pcListView->setSize(200,400);
    resize( 200, 400 );

}

void FCTree::Update(void)
{
	GetConsole().Log("Tree Updated\n");
	
	// quick and dirty so far
	delete _pcMainItem;
	_pcMainItem = new FCTreeLabel(this);

}

void FCTree::OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument)
{
//	GetConsole().Log("Tree doc activated %p\n",pcNewDocument);

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
}

bool FCTree::OnMsg(const char* pMsg)
{
	//printf("MsgTree: %s View: %p\n",pMsg,this);

	// no messages yet
	return false;
}


#include "moc_Tree.cpp"

