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
class FCPropertyListView;

/** The link between the ProperyView and the shown Property.
 */
class FCPropertyViewItem : public QListViewItem
{
public:
	/// Constructor
    FCPropertyViewItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop, const QString &propName);
    FCPropertyViewItem( FCPropertyView * parent);
    FCPropertyViewItem( FCPropertyViewItem * parent);

    /// Opens the Leafs and generate them.
    void setOpen( bool );
    bool isOpen() const;
    /// Im not realy sure whats this method do ;-).
    void setup();

    virtual void setValue( const QVariant &v );
    QVariant value() const;
    QString name() const;
    virtual void notifyValueChange();
    virtual FCPropertyViewItem *propertyParent() const;
    virtual void childValueChanged( FCPropertyViewItem *child );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );
    virtual bool hasSubItems() const;

    virtual void showEditor();
    virtual void hideEditor();
    void setFocus( QWidget *w );
    virtual void toggle();
    virtual void placeEditor( QWidget *w );

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
    virtual void createChildren();
    virtual void initChildren();

    void addChild( FCPropertyViewItem *i );
    int childCount() const;
    FCPropertyViewItem *child( int i ) const;



    virtual QString currentItem() const;
    virtual int currentIntItem() const;
    virtual void setCurrentItem( const QString &s );
    virtual void setCurrentItem( int i );
    virtual int currentIntItemFromObject() const;
    virtual QString currentItemFromObject() const;

protected:
	void activate ();
    QColor backgroundColor();
    bool open, changed;
    QVariant val;

    QColor backColor;
    FCPropertyListView* listview;
private:
    void createResetButton();
    void updateResetButtonState();

    QString propertyName;
    QPushButton *resetButton;
    FCPropertyViewItem *property;
    QList<FCPropertyViewItem> children;
};


class FCPropertyViewIntItem : public QObject,	public FCPropertyViewItem
{
    Q_OBJECT

public:
    FCPropertyViewIntItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop, const QString &propName, bool s);
    FCPropertyViewIntItem( FCPropertyView * parent);
    FCPropertyViewIntItem( FCPropertyViewItem * parent);
    ~FCPropertyViewIntItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    QSpinBox *spinBox();
    QGuardedPtr<FCSpinBox> spinBx;
    bool signedValue;

};


class FCPropertyViewBoolItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

public:
    FCPropertyViewBoolItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop, const QString &propName );
    ~FCPropertyViewBoolItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual void toggle();

private slots:
    void setValue();

private:
    QComboBox *combo();
    QGuardedPtr<QComboBox> comb;

};

class FCPropertyViewTextItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

public:
    FCPropertyViewTextItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
		      const QString &propName, bool comment, bool multiLine, bool ascii = FALSE, bool a = FALSE );
    ~FCPropertyViewTextItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( FCPropertyViewItem *child );

    virtual void setChanged( bool b, bool updateDb = TRUE );

private slots:
    void setValue();
    void getText();

private:
    QLineEdit *lined();
    QGuardedPtr<QLineEdit> lin;
    QGuardedPtr<QHBox> box;
    QPushButton *button;
    bool withComment, hasMultiLines, asciiOnly, accel;

};


class FCPropertyViewCoordItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

public:
    enum Type { Rect, Size, Point };

    FCPropertyViewCoordItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
		       const QString &propName, Type t );
    ~FCPropertyViewCoordItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( FCPropertyViewItem *child );

private:
    QLineEdit *lined();
    QGuardedPtr<QLineEdit> lin;
    Type typ;

};

class FCPropertyViewListItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

public:
    FCPropertyViewListItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
		      const QString &propName, bool editable );
    ~FCPropertyViewListItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

    QString currentItem() const;
    int currentIntItem() const;
    void setCurrentItem( const QString &s );
    void setCurrentItem( int i );
    int currentIntItemFromObject() const;
    QString currentItemFromObject() const;

private slots:
    void setValue();

private:
    QComboBox *combo();
    QGuardedPtr<QComboBox> comb;
    int oldInt;
    bool editable;
    QString oldString;

};

class FCPropertyViewColorItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

public:
    FCPropertyViewColorItem( FCPropertyListView *l, FCPropertyViewItem *after, FCPropertyViewItem *prop,
		       const QString &propName, bool children );
    ~FCPropertyViewColorItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( FCPropertyViewItem *child );

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

private slots:
    void getColor();

private:
    QGuardedPtr<QHBox> box;
    QGuardedPtr<QFrame> colorPrev;
    QGuardedPtr<QPushButton> button;
    bool withChildren;

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
    QWidget *widget() const;

	bool OnMsg(const char* pMsg);
    void clear();
    void setup();
    void resetFocus();
    void refetchData();
    FCPropertyListView *propertyList() const;
    QString currentProperty() const;
    QString classOfCurrentProperty() const;

	virtual const char *GetName(void){return "PropertyView";}
	
	virtual void resizeEvent ( QResizeEvent * e);

	friend FCPropertyViewItem;

	/// is called when the above function is called to handle document change stuff
	virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument);
	/// get called when the document is changed or updated
	virtual void Update(void);

	static QColor cBackColor1;
	static QColor cBackColor2;



//protected:
	FCPropertyListView*		_pcListView;

	static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;

private:
    QWidget *wid;
};



class FCPropertyListView : public QListView
{
    Q_OBJECT

public:
    FCPropertyListView( FCPropertyView *e );

//    void setupProperties();

    void setCurrentItem( QListViewItem *i );
    void valueChanged( FCPropertyViewItem *i );
    void refetchData();
    void setPropertyValue( FCPropertyViewItem *i );
    void setCurrentProperty( const QString &n );

    FCPropertyView *propertyView() const;

public slots:
    void updateEditorSize();
    void resetProperty();

private slots:
    void itemPressed( QListViewItem *i, const QPoint &p, int c );
    void toggleOpen( QListViewItem *i );
    bool eventFilter( QObject *o, QEvent *e );

protected:
    void resizeEvent( QResizeEvent *e );
    void paintEmptyArea( QPainter *p, const QRect &r );
    bool addPropertyItem( FCPropertyViewItem *&item, const QCString &name, QVariant::Type t );

private:
    FCPropertyView *propView;

};

#endif // __PROPTERYVIEW_H__

