/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef __TREE__
#define __TREE__

#ifndef _PreComp_
# include <qlistview.h>
# include <TDF_Label.hxx>
#endif

#include "../Base/PyExport.h"
#include "DockWindow.h"

/// Forwards
class FCLabel; 
class TreeView;
class FCGuiDocument;


struct GUIDDefs {
    char	*Guid;
    char	*Name;
};



/** The link between the Tree and the shown Label.
 *  Every (shown) Label in the FCDocument class get it 
 *  associated FCTreeLabel which controls the visibility 
 *  and the functions of the Label.
 */
class FCTreeLabel : public QListViewItem
{
public:
	/// Constructor
    FCTreeLabel( FCTreeLabel * parent, TDF_Label &hcLabel );
    FCTreeLabel( TreeView * parent);

    /// Opens the Leafs and generate them.
    void setOpen( bool );
    /// Im not realy sure whats this method do ;-).
    void setup();

    /// Delivers the pixmap that is shown.
//    const QPixmap *pixmap( int i ) const;
    /// Sets the pixmap that will be shown.
//    void setPixmap( QPixmap *p );

	void Update(void);

	void BuildUp(void);

protected:
	void activate (); 
	TDF_Label _hcTDFLabel;
	//FCPyHandle<FCLabel> _hcLabel;

    FCTreeLabel * _pcParent;
	FCGuiDocument*  _pcDocument;

//	bool _bOpend;
};




class TreeView :public Gui::DockView
{
	Q_OBJECT
public:
	TreeView(FCGuiDocument*  pcDocument,QWidget *parent=0,const char *name=0);
	// App_Tree();

	bool OnMsg(const char* pMsg);

	virtual const char *GetName(void){return "Raw Tree";}
	

	//void InitCascade(Handle(TDocStd_Document) hDoc);
	friend class FCTreeLabel;

	/// is called when the above function is called to handle document change stuff
	virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument);
	/// get called when the document is changed or updated
	virtual void Update(void);


protected:
	QListView*		_pcListView;
	QListViewItem*  _pcMainItem;

	static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;
};


#endif

