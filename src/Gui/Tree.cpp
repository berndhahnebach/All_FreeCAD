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

FCTreeLabel::FCTreeLabel( FCTree * parent)
	:QListViewItem(parent->_pcListView),
	 _pcDocument(parent->_pcDocument),
	 _bOpend(false)
{
	if(_pcDocument){
		setText(0,"Main Label");
		_hcLabel = parent->_pcDocument->GetDocument()->Main();
	}else{
		setText(0,"No Active Document");
	}

	Update();
	
}


/** Set the actuall pixmap
 *  @param px a QT pixmap object
 */
void FCTreeLabel::setPixmap( QPixmap *px )
{
    //pix = px;
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



FCTree::FCTree(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:FCView(pcDocument,parent,name)
{
	_pcVBoxLayout = new QVBox(this);

	_pcListView = new QListView(_pcVBoxLayout,name);
	//_hDoc = hDoc;
	//setRootIsDecorated(true);
	_pcListView->setSorting(-1,false);

	_pcListView->addColumn("Labels");
	_pcListView->setColumnWidthMode(0,QListView::Maximum);
	_pcListView->addColumn("Value");
	_pcListView->setColumnWidthMode(1,QListView::Maximum);
	//addColumn("Attributes.");

	// Add the first main label
	new FCTreeLabel(this);


	/*
	QListViewItem *tmp = new QListViewItem(this,"Part","no");
	tmp->setOpen ( true );
	new QListViewItem(tmp,"xy_Plane","no");
	new QListViewItem(tmp,"yz_Plane","no");
	new QListViewItem(tmp,"zx_Plane","no");
	tmp = new QListViewItem(tmp,"PartBody","no");
	new QListViewItem(tmp,"Thickness","no");
	new QListViewItem(tmp,"Thickness.2","no");
	new QListViewItem(tmp,"Split","no");
	new QListViewItem(tmp,"usw","no");
	tmp = new QListViewItem(tmp,"Empty","no");
	tmp = new QListViewItem(tmp,"Empty","no");
*/
	//setSize(200,400);

}

void FCTree::resizeEvent ( QResizeEvent * e) 
{
  _pcListView->resize(e->size());
}

bool FCTree::OnMsg(const char* pMsg)
{
	//printf("MsgTree: %s View: %p\n",pMsg,this);

	return false;
}



//Tree:: Tree(){};


#include "moc_Tree.cpp"

