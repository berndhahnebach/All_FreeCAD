/** \file PropertyView.h
 *  \brief Framework to show and work with all kind of properies
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2003                        *   
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


#ifndef __PROPTERYVIEW_H__
#define __PROPTERYVIEW_H__

#include "View.h"

class QPixmap;
class QListView;
class FCPropertyView;


/** The link between the ProperyView and the shown Property.
 */
class FCPropertyViewItem : public QListViewItem
{
public:
	/// Constructor
    FCPropertyViewItem( FCPropertyView * parent);
    FCPropertyViewItem( FCPropertyViewItem * parent);

    /// Opens the Leafs and generate them.
    void setOpen( bool );
    /// Im not realy sure whats this method do ;-).
    void setup();

    /// Delivers the pixmap that is shown.
//    const QPixmap *pixmap( int i ) const;
    /// Sets the pixmap that will be shown.
//    void setPixmap( QPixmap *p );

	/// paint mimic
    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
#if QT_VERSION <= 230
    void paintBranches( QPainter * p, const QColorGroup & cg,
			int w, int y, int h, GUIStyle s );
#else
    void paintBranches( QPainter * p, const QColorGroup & cg,	int w, int y, int h);
#endif
    void paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r );

    void updateBackColor();

    virtual void setChanged( bool b, bool updateDb = TRUE );
    bool isChanged() const;

	void Update(void);

	void BuildUp(void);

protected:
	void activate (); 
    QColor backgroundColor();
    bool open, changed;

    QColor backColor;

};




/** A test class. A more elaborate class description.
 */
class FCPropertyView :public FCDockView
{
public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	FCPropertyView(FCGuiDocument*  pcDocument,QWidget *parent=0,const char *name=0);

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~FCPropertyView();

	bool OnMsg(const char* pMsg);

	virtual const char *GetName(void){return "PropertyView";}
	
	virtual void resizeEvent ( QResizeEvent * e); 

	friend FCPropertyViewItem;

	/// is called when the above function is called to handle document change stuff
	virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument);
	/// get called when the document is changed or updated
	virtual void Update(void);

	static QColor cBackColor1;
	static QColor cBackColor2;



protected:
	QListView*		_pcListView;

	static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;
};




#endif // __PROPTERYVIEW_H__