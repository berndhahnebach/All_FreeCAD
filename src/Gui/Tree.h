/** \file Tree.h
 *  \brief The document Tree view module
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


#ifndef __TREE__
#define __TREE__

#include <qlistview.h>
#include "../Base/PyExport.h"


/// Forwards
class FCLabel; 


/** A container for FCTreeLabelType objects
 
class FCTreeLabelTypeContainer : public FCPyObject
{
public:
	/// Constructor
    FCTreeLabelTypeContainer( void );

protected:

};




/** Defines the behacvior of the Tree node of special signature of a Label
 
class FCTreeLabelType : public FCPyObject
{
public:
	/// Constructor
    FCTreeLabelType( void );

protected:

};
*/






/** The link between the Tree and the shown Label.
 *  Every (shown) Label in the FCDocument class get it 
 *  associated FCTreeLabel which controls the visibility 
 *  and the functions of the Label.
 */
class FCTreeLabel : public QListViewItem
{
public:
	/// Constructor
    FCTreeLabel( FCTreeLabel * parent, const FCLabel& rcLabel );

    /// Opens the Leafs and generate them.
    void setOpen( bool );
    /// Im not realy sure whats this method do ;-).
    void setup();

    /// Delivers the pixmap that is shown.
    const QPixmap *pixmap( int i ) const;
    /// Sets the pixmap that will be shown.
    void setPixmap( QPixmap *p );

protected:
	const FCLabel& _rcLabel;

    FCTreeLabel * _pcParent;

};



class FCGuiDocument;

class FCTree :public QListView
{
	Q_OBJECT
public:
	FCTree(FCGuiDocument*  pcDocument,QWidget *parent=0,const char *name=0);
	// App_Tree();
	
	//void InitCascade(Handle(TDocStd_Document) hDoc);

private:
	FCGuiDocument*  _pcDocument;
};


#endif

