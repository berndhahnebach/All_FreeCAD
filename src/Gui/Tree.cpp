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

#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else

#endif

#include "Application.h"
#include "Document.h"
#include "../App/Document.h"
#include "Tree.h"



/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
FCTreeLabel::FCTreeLabel( FCTreeLabel * parent, FCPyHandle<FCLabel> &hcLabel )
    : QListViewItem( parent ),
	_pcDocument(parent->_pcDocument),
	_hcLabel(hcLabel),
	_bOpend(false)
{
	QString cString;

	cString.sprintf("Tag:%d",_hcLabel->GetOCCLabel().Tag());
	setText(0,cString);
		
}

#include "Icons/x.xpm"


FCTreeLabel::FCTreeLabel( FCTree * parent)
	:QListViewItem(parent->_pcListView),
	 _pcDocument(parent->_pcDocument),
	 _bOpend(false)
{
	if(_pcDocument){
		setText(0,"Main Label");
		setPixmap(0,*FCTree::pcLabelOpen);
		_hcLabel = parent->_pcDocument->GetDocument()->Main();
	}else{
		setPixmap(0,*FCTree::pcLabelClosed);
		//setPixmap(new QPixmap(px));
		setText(0,"No Active Document");
	}

	setExpandable( TRUE );
	//Update();
	
}


/** Set the actuall pixmap
 *  @param px a QT pixmap object
 */
/*
void FCTreeLabel::setPixmap( QPixmap *px )
{
    //pix = px;
	QListViewItem::setPixmap(0,*px);
    setup();
    widthChanged( 0 );
    invalidateHeight();
    repaint();
}


const QPixmap *FCTreeLabel::pixmap( int i ) const
{
    if ( i )
		return 0;
	return 0;

    //return pix;
}

  */
void FCTreeLabel::Update(void)
{
	// quieck an dirty
	if(_pcDocument && _hcLabel->GetOCCLabel().HasChild() && !_bOpend)
	{
		//for(QListViewItem* pItem = firstChild (); pItem!=0 ; pItem = pItem->nextSibling () )
		//	delete pItem;

		std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

		for(std::vector<FCPyHandle<FCLabel> >::iterator It=vpcLabels.begin();It != vpcLabels.end(); It++)
		{
			new FCTreeLabel(this,*It);
		}

		_bOpend = true;

	}	


}

void FCTreeLabel::setOpen( bool o )
{
	puts("setOpen");
	// quieck an dirty
	if(_pcDocument && _hcLabel->GetOCCLabel().HasChild() && o && !_bOpend)
	{
		//for(QListViewItem* pItem = firstChild (); pItem!=0 ; pItem = pItem->nextSibling () )
		//	delete pItem;

		std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

		for(std::vector<FCPyHandle<FCLabel> >::iterator It=vpcLabels.begin();It != vpcLabels.end(); It++)
		{
			new FCTreeLabel(this,*It);
		}

		_bOpend = true;

	}	
	QListViewItem::setOpen ( o );
	
	
	/*    if ( o )
	setPixmap( folderOpen );
    else
	setPixmap( folderClosed );

    if ( o && !childCount() ) {
	QString s( fullName() );
	QDir thisDir( s );
	if ( !thisDir.isReadable() ) {
	    readable = FALSE;
	    setExpandable( FALSE );
	    return;
	}

	listView()->setUpdatesEnabled( FALSE );
	const QFileInfoList * files = thisDir.entryInfoList();
	if ( files ) {
	    QFileInfoListIterator it( *files );
	    QFileInfo * fi;
	    while( (fi=it.current()) != 0 ) {
		++it;
		if ( fi->fileName() == "." || fi->fileName() == ".." )
		    ; // nothing
		else if ( fi->isSymLink() && !showDirsOnly ) {
		    FileItem *item = new FileItem( this, fi->fileName(),
						     "Symbolic Link" );
		    item->setPixmap( fileNormal );
		}
		else if ( fi->isDir() )
		    (void)new Directory( this, fi->fileName() );
		else if ( !showDirsOnly ) {
		    FileItem *item
			= new FileItem( this, fi->fileName(),
					     fi->isFile()?"File":"Special" );
		    item->setPixmap( fileNormal );
		}
	    }
	}
	listView()->setUpdatesEnabled( TRUE );
    }
    QListViewItem::setOpen( o );
	*/
}


void FCTreeLabel::setup()
{
    //setExpandable( TRUE );
    QListViewItem::setup();
}

void FCTreeLabel::activate ()
{
	puts("Activated");
}




QPixmap* FCTree::pcLabelOpen=0;
QPixmap* FCTree::pcLabelClosed=0;
QPixmap* FCTree::pcAttribute=0;


FCTree::FCTree(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:FCView(pcDocument,parent,name)
{

	_pcListView = new QListView(this,name);

	// set defaults and the colums
	_pcListView->setSorting(-1,false);
	_pcListView->addColumn("Labels & Attr.");
	_pcListView->setColumnWidthMode(0,QListView::Maximum);
	_pcListView->addColumn("Value");
	_pcListView->setColumnWidthMode(1,QListView::Manual );
	_pcListView->addColumn("Type");
	_pcListView->setColumnWidthMode(1,QListView::Manual );

	// retreive the Pixmaps
	pcLabelOpen   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(ApplicationWindow::Instance->GetBmpFactory().GetPixmap("RawTree_Attr"));


	// Add the first main label
	(new QListViewItem(_pcListView,"No Active Document"))->setPixmap(0,*pcLabelClosed);
	//new FCTreeLabel(this);
	//_pcListView->setRootIsDecorated(true);



	//_pcListView->setSize(200,400);
    resize( 200, 400 );

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



//Tree:: Tree(){};


#include "moc_Tree.cpp"

