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

#ifndef _PreComp_
#if QT_VERSION < 300
#	include <qlist.h>
#  else
#	include <qptrlist.h>
#  endif
#endif

class QPixmap;
class QListView;
class FCPropertyView;
class FCPropertyListView;
class FCProperty;

/** The link between the ProperyView and the shown Property.
 */
class FCPropertyViewItem : public QListViewItem
{
  public:
    /// Constructor
    FCPropertyViewItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName);
    virtual ~FCPropertyViewItem();

    /// Opens the Leafs and generate them.
    void setOpen( bool );
    bool isOpen() const;
    void setup();
    bool isChanged() const;
    void setFocus( QWidget *w );

    QString name() const;
    FCPropertyViewItem *propertyParent() const;
    void notify();

  	/// paint mimic
    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
#if QT_VERSION < 300
    void paintBranches( QPainter * p, const QColorGroup & cg, int w, int y, int h, GUIStyle s );
#else
    void paintBranches( QPainter * p, const QColorGroup & cg, int w, int y, int h );
#endif
    void paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r );
    void updateBackColor();

    void setValue( FCProperty* v );
    FCProperty* value() const;
    virtual void setChanged( bool b, bool updateDb = TRUE );
    virtual bool hasCustomContents() const;
    virtual bool hasSubItems() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );
    virtual void toggle();

    // show/hide
    virtual void showView();
    virtual void hideView();
    void placeView( QWidget *w );

    // current item stuff
    virtual QString currentItem() const;
    virtual int currentIntItem() const;
    virtual void setCurrentItem( const QString &s );
    virtual void setCurrentItem( int i );

    // children stuff
    void addChild( FCPropertyViewItem *i );
    int childCount() const;
    FCPropertyViewItem *child( int i ) const;
    virtual void createChildren();
    virtual void initChildren();
    virtual void childValueChanged( FCPropertyViewItem *child );

    FCPropertyViewItem* createPropertyItem( FCProperty* v, const char* propName );
    /// Delivers the pixmap that is shown.
//    const QPixmap *pixmap( int i ) const;
    /// Sets the pixmap that will be shown.
//    void setPixmap( QPixmap *p );
//	  void Update(void);
//  	void BuildUp(void);

  protected:
    virtual void updateItem ( FCProperty* v ) = 0;
	  void activate (); 
    QColor backgroundColor();
    bool open, changed;

    QColor backColor;
    FCPropertyListView* listview;

  private:
    void setPropertyParent (FCPropertyViewItem * parent);
    void createResetButton();
    void updateResetButtonState();

    FCProperty* val;
    QString propertyName;
    QPushButton *resetButton;
    FCPropertyViewItem *property;
#if QT_VERSION < 300
    QList<FCPropertyViewItem> children;
#else
    QPtrList<FCPropertyViewItem> children;
#endif
};

class FCPropertyViewIntItem : public QObject,	public FCPropertyViewItem
{
    Q_OBJECT

  public:
    FCPropertyViewIntItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, bool s);
    ~FCPropertyViewIntItem();

    virtual void showView();
    virtual void hideView();

  protected:
    virtual void updateItem ( FCProperty* v );

  private slots:
    void onSetValue();

  private:
    QSpinBox *getSpinBox();
    bool signedValue;
    QGuardedPtr<FCSpinBox> spinBox;
};

class FCPropertyViewFloatItem : public QObject,	public FCPropertyViewItem
{
    Q_OBJECT

  public:
    FCPropertyViewFloatItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, bool s);
    ~FCPropertyViewFloatItem();

    virtual void showView();
    virtual void hideView();

  protected:
    virtual void updateItem ( FCProperty* v );

  private slots:
    void onSetValue();

  private:
    FCFloatSpinBox *getSpinBox();
    bool signedValue;
    QGuardedPtr<FCFloatSpinBox> spinBox;
};

class FCPropertyViewBoolItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

  public:
    FCPropertyViewBoolItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName );
    ~FCPropertyViewBoolItem();

    virtual void toggle();
    virtual void showView();
    virtual void hideView();

  protected:
    virtual void updateItem ( FCProperty* v );

  private slots:
    void onSetValue();

  private:
    QComboBox *combo();
    QGuardedPtr<QComboBox> comb;
};

class FCPropertyViewTextItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

  public:
    FCPropertyViewTextItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, 
          bool comment, bool multiLine);
    ~FCPropertyViewTextItem();

    virtual void setChanged( bool b, bool updateDb = TRUE );
    virtual bool hasSubItems() const;
    
    virtual void showView();
    virtual void hideView();

    virtual void createChildren();
    virtual void initChildren();
    virtual void childValueChanged( FCPropertyViewItem *child );

  protected:
    virtual void updateItem ( FCProperty* v );

  private slots:
    void onSetValue();
    void getText();

  private:
    QLineEdit *lined();
    QGuardedPtr<QLineEdit> lin;
    QGuardedPtr<QHBox> box;
    QPushButton *button;
    bool withComment, hasMultiLines;
};
/*
class FCPropertyViewCoordItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

  public:
    FCPropertyViewCoordItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName );
    ~FCPropertyViewCoordItem();

    virtual void setValue( FCProperty* v );
    virtual bool hasSubItems() const;

    virtual void showView();
    virtual void hideView();

    virtual void createChildren();
    virtual void initChildren();
    virtual void childValueChanged( FCPropertyViewItem *child );

  private:
    QLineEdit *lined();
    QGuardedPtr<QLineEdit> lin;
};
*/
class FCPropertyViewListItem : public QObject, public FCPropertyViewItem
{
    Q_OBJECT

  public:
    FCPropertyViewListItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, bool editable );
    ~FCPropertyViewListItem();


    virtual void showView();
    virtual void hideView();

    QString currentItem() const;
    int currentIntItem() const;
    void setCurrentItem( const QString &s );
    void setCurrentItem( int i );

  protected:
    virtual void updateItem ( FCProperty* v );

  private slots:
    void onSetValue();

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
    FCPropertyViewColorItem( FCPropertyListView *l, FCPropertyViewItem *after, const char* propName, bool children );
    ~FCPropertyViewColorItem();

    virtual bool hasCustomContents() const;
    virtual bool hasSubItems() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

    virtual void showView();
    virtual void hideView();

    virtual void createChildren();
    virtual void initChildren();
    virtual void childValueChanged( FCPropertyViewItem *child );

  protected:
    virtual void updateItem ( FCProperty* v );

  private slots:
    void getColor();

  private:
    QHBox* boxColor();
    QGuardedPtr<QHBox> box;
    QGuardedPtr<QFrame> colorPrev;
    QGuardedPtr<QPushButton> button;
    bool withChildren;
};

class FCPropertyListView : public QListView
{
    Q_OBJECT

  public:
    FCPropertyListView( QWidget* parent );

    void setCurrentItem( QListViewItem *i );
    void valueChanged( FCPropertyViewItem *i );
    void setCurrentProperty( const QString &n );

  public slots:
    void updateViewSize();
    void resetProperty();

  private slots:
    void itemPressed( QListViewItem *i, const QPoint &p, int c );
    void toggleOpen( QListViewItem *i );
    bool eventFilter( QObject *o, QEvent *e );

  protected:
    void resizeEvent( QResizeEvent *e );
    void keyPressEvent ( QKeyEvent * e );
    void paintEmptyArea( QPainter *p, const QRect &r );
    bool addPropertyItem( FCPropertyViewItem *&item, const QCString &name, const char* type );
};

class FCPropertyEditor : public QTabWidget
{
    Q_OBJECT

	public:
    FCPropertyEditor( FCPropertyView * view );

    QObject *widget() const;

    void clear();
    void setup();

    void refetchData();

    FCPropertyListView *propertyList() const;
    QString currentProperty() const;

    void resetFocus();
    void setPropertyEditorEnabled( bool b );
    void setSignalHandlersEnabled( bool b );


    FCPropertyView *propertyView() const;


	signals:
    void hidden();

	protected:
    void closeEvent( QCloseEvent *e );

	private:
    void updateWindow();

	private:
    QObject *wid;
    FCPropertyListView *listview;

  private:
    FCPropertyView *propView;

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
    FCPropertyListView *propertyList() const;
    QString currentProperty() const;

	  virtual const char *GetName(void){return "PropertyView";}
	
  	virtual void resizeEvent ( QResizeEvent * e); 

  	friend class FCPropertyViewItem;

	  /// is called when the above function is called to handle document change stuff
	  virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument);
	  /// get called when the document is changed or updated
	  virtual void Update(void);

	  static QColor cBackColor1;
	  static QColor cBackColor2;



//protected:
//  	FCPropertyListView*		_pcListView;
//		class PropertyList* _pcListView;

  	static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;

  private:
    QGridLayout* _pTabLayout;
    class PropertyEditor * _pTabWnd;
    QWidget *wid;
};





#include <qfeatures.h>
#include <qvariant.h>
#include <qlistview.h>
#include <qptrlist.h>
#include <qguardedptr.h>
#include <qtabwidget.h>
#include <qmodules.h>

class PropertyList;
class PropertyEditor;
class QPainter;
class QColorGroup;
class QComboBox;
class QLineEdit;
class QPushButton;
class QHBox;
class QSpinBox;
class QLabel;
class FormWindow;
class QCloseEvent;
class QResizeEvent;
class PropertyWhatsThis;
class QDateEdit;
class QTimeEdit;
class QDateTimeEdit;

class PropertyItem : public QListViewItem
{
public:
    PropertyItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyItem();

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align );
    void paintBranches( QPainter * p, const QColorGroup & cg,
			int w, int y, int h );
    void paintFocus( QPainter *p, const QColorGroup &cg, const QRect &r );

    virtual bool hasSubItems() const;
    virtual void createChildren();
    virtual void initChildren();

    bool isOpen() const;
    void setOpen( bool b );

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    QVariant value() const;
    QString name() const;
    virtual void notifyValueChange();

    virtual void setChanged( bool b, bool updateDb = TRUE );
    bool isChanged() const;

    virtual void placeEditor( QWidget *w );

    virtual PropertyItem *propertyParent() const;
    virtual void childValueChanged( PropertyItem *child );

    void addChild( PropertyItem *i );
    int childCount() const;
    PropertyItem *child( int i ) const;

    virtual bool hasCustomContents() const;
    virtual void drawCustomContents( QPainter *p, const QRect &r );

    void updateBackColor();

    void setup() { QListViewItem::setup(); setHeight( QListViewItem::height() + 2 ); }

    virtual QString currentItem() const;
    virtual int currentIntItem() const;
    virtual void setCurrentItem( const QString &s );
    virtual void setCurrentItem( int i );
    virtual int currentIntItemFromObject() const;
    virtual QString currentItemFromObject() const;

    void setFocus( QWidget *w );

    virtual void toggle();
    void setText( int col, const QString &txt );

protected:
    PropertyList *listview;
    QVariant val;

private:
    QColor backgroundColor();
    void createResetButton();
    void updateResetButtonState();

private:
    bool open, changed;
    PropertyItem *property;
    QString propertyName;
    QPtrList<PropertyItem> children;
    QColor backColor;
    QPushButton *resetButton;

};

class PropertyBoolItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyBoolItem( PropertyList *l, PropertyItem *after, PropertyItem *prop, const QString &propName );
    ~PropertyBoolItem();

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

class PropertyIntItem : public QObject,
			public PropertyItem
{
    Q_OBJECT

public:
    PropertyIntItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		     const QString &propName, bool s );
    ~PropertyIntItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    QSpinBox *spinBox();
    QGuardedPtr<QSpinBox> spinBx;
    bool signedValue;

};

class PropertyListItem : public QObject,
			 public PropertyItem
{
    Q_OBJECT

public:
    PropertyListItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		      const QString &propName, bool editable );
    ~PropertyListItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

    QString currentItem() const;
    int currentIntItem() const;
    void setCurrentItem( const QString &s );
    void setCurrentItem( int i );
    int currentIntItemFromObject() const;
    QString currentItemFromObject() const;
    void addItem( const QString &s );

private slots:
    void setValue();

private:
    QComboBox *combo();
    QGuardedPtr<QComboBox> comb;
    int oldInt;
    bool editable;
    QString oldString;

};

class PropertyCoordItem : public QObject,
			  public PropertyItem
{
    Q_OBJECT

public:
    enum Type { Rect, Size, Point };

    PropertyCoordItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		       const QString &propName, Type t );
    ~PropertyCoordItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

private:
    QLineEdit *lined();
    QGuardedPtr<QLineEdit> lin;
    Type typ;

};

class PropertyColorItem : public QObject,
			  public PropertyItem
{
    Q_OBJECT

public:
    PropertyColorItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		       const QString &propName, bool children );
    ~PropertyColorItem();

    virtual void createChildren();
    virtual void initChildren();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );
    virtual bool hasSubItems() const;
    virtual void childValueChanged( PropertyItem *child );

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

class PropertyList : public QListView
{
    Q_OBJECT

public:
    PropertyList( PropertyEditor *e );

    virtual void setupProperties();

    virtual void setCurrentItem( QListViewItem *i );
    virtual void valueChanged( PropertyItem *i );
    virtual void refetchData();
    virtual void setPropertyValue( PropertyItem *i );
    virtual void setCurrentProperty( const QString &n );

    void layoutInitValue( PropertyItem *i, bool changed = FALSE );
    PropertyEditor *propertyEditor() const;
    QString whatsThisAt( const QPoint &p );
    void showCurrentWhatsThis();

public slots:
    void updateEditorSize();
    void resetProperty();
    void toggleSort();

private slots:
    void itemPressed( QListViewItem *i, const QPoint &p, int c );
    void toggleOpen( QListViewItem *i );
    bool eventFilter( QObject *o, QEvent *e );

protected:
    void resizeEvent( QResizeEvent *e );
    void paintEmptyArea( QPainter *p, const QRect &r );
    bool addPropertyItem( /*PropertyItem *&item, const QCString &name, QVariant::Type t */);

    void viewportDragEnterEvent( QDragEnterEvent * );
    void viewportDragMoveEvent ( QDragMoveEvent * );
    void viewportDropEvent ( QDropEvent * );

protected:
    PropertyEditor *editor;

private:
    void readPropertyDocs();
    QString whatsThisText( QListViewItem *i );

private:
    PropertyListItem* pressItem;
    QPoint pressPos;
    bool mousePressed;
    bool showSorted;
    QMap<QString, QString> propertyDocs;
    PropertyWhatsThis *whatsThis;

};

class PropertyDoubleItem : public QObject,
			public PropertyItem
{
    Q_OBJECT

public:
    PropertyDoubleItem( PropertyList *l, PropertyItem *after, PropertyItem *prop,
		     const QString &propName );
    ~PropertyDoubleItem();

    virtual void showEditor();
    virtual void hideEditor();

    virtual void setValue( const QVariant &v );

private slots:
    void setValue();

private:
    QLineEdit *lined();
    QGuardedPtr<QLineEdit> lin;
};


class PropertyEditor : public QTabWidget
{
    Q_OBJECT

public:
    PropertyEditor( QWidget *parent );

    QObject *widget() const;

    void clear();
    void setup();

    void refetchData();

    void closed( FormWindow *w );

    PropertyList *propertyList() const;
    QString currentProperty() const;
    QString classOfCurrentProperty() const;

    void resetFocus();
    void setPropertyEditorEnabled( bool b );
    void setSignalHandlersEnabled( bool b );



signals:
    void hidden();

protected:
    void closeEvent( QCloseEvent *e );


private:
    void updateWindow();

private:
    QObject *wid;
    PropertyList *listview;

};

#endif // __PROPTERYVIEW_H__
