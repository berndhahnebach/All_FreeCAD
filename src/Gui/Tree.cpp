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
#include "Tree.h"



/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
FCTreeLabel::FCTreeLabel( FCTreeLabel * parent, const FCLabel& rcLabel )
    : QListViewItem( parent ),
	_rcLabel(rcLabel)
{
/*
    if ( !readable )
		setPixmap( folderLocked );
    else
		setPixmap( folderClosed );
		*/
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

void FCTreeLabel::setOpen( bool o )
{
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
    setExpandable( TRUE );
    QListViewItem::setup();
}



FCTree::FCTree(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:QListView(parent,name),
	 _pcDocument(pcDocument)
{
	//_hDoc = hDoc;
	//setRootIsDecorated(true);
	setSorting(-1,false);

	addColumn("Labels");
	setColumnWidthMode(0,Maximum);
	addColumn("Value");
	setColumnWidthMode(1,Maximum);
	//addColumn("Attributes.");

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

	//setSize(200,400);

}

//Tree:: Tree(){};


#include "Tree_moc.cpp"

