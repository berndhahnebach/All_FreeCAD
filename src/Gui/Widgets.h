/***************************************************************************
                          Widgets.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Widgets.h
 *  \brief A set of FreeCAD widgets
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/
 

#ifndef __FC_WIDGETS_H__
#define __FC_WIDGETS_H__
//#include <qprogressbar.h>
//#include <qlabel.h>
//#include <qiconview.h>
//#include <qdragobject.h>
//#include <qstatusbar.h>
//#include <qtoolbar.h>
//#include <qfiledialog.h>
//#include <qvariant.h>
//#include <qdialog.h>
//#include <qspinbox.h>
//#if QT_VERSION > 230
//# include <qlistview.h>
//# include <qlineedit.h>
//#endif

class QHBoxLayout; 
class QTime;

/**
 * Using the Qt's open/save dialogs with own adjustments
 * In this implementation a waitcursor will additionally 
 * be set.
 * @see FCAutoWaitCursor
 */
class GuiExport FCFileDialog : public QFileDialog
{
  Q_OBJECT

  public:
    static QString getOpenFileName( const QString &initially = QString::null,
				                            const QString &filter = QString::null,
				                            QWidget *parent = 0, const char* name = 0 );
    static QString getOpenFileName( const QString &initially, const QString &filter,
				                            QWidget *parent, const char* name, const QString& caption );
    static QString getSaveFileName ( const QString & initially = QString::null, 
                                     const QString & filter = QString::null, QWidget * parent = 0, 
                                     const char * name = 0 );
    static QString getSaveFileName ( const QString & initially, const QString & filter, 
                                     QWidget * parent, const char * name, const QString & caption );
    static QString getSaveFileName ( const QString & initially, const QString & filter, 
                                     QWidget * parent, const QString & caption ); 

  public:
    FCFileDialog (Mode mode, QWidget* parent = 0, const char* name = 0, bool modal = false);
    FCFileDialog (Mode mode, const QString& dirName, const QString& filter = QString::null, 
                  QWidget* parent = 0, const char* name = 0, bool modal = false);
    virtual ~FCFileDialog();

    QString selectedFileName();

  protected slots:
    virtual void accept();
};

class GuiExport FCMessageBox : public QMessageBox
{
	public:
		FCMessageBox(QWidget * parent=0, const char * name=0);
		~FCMessageBox();

		static int information(QWidget * parent, const QString & caption, const QString & text, 
			int button0, int button1=0, int button2=0);
		static int information(QWidget * parent, const QString & caption, const QString & text, 
			const QString & button0Text = QString::null, const QString & button1Text = QString::null, 
			const QString & button2Text = QString::null, int defaultButtonNumber = 0, int escapeButtonNumber = -1);
		static int warning ( QWidget * parent, const QString & caption, const QString & text, 
			int button0, int button1, int button2=0 ); 
		static int warning ( QWidget * parent, const QString & caption, const QString & text, 
			const QString & button0Text = QString::null, const QString & button1Text = QString::null, 
			const QString & button2Text = QString::null, int defaultButtonNumber = 0, int escapeButtonNumber = -1 );
		static int critical ( QWidget * parent, const QString & caption, const QString & text, 
			int button0, int button1, int button2=0 );
		static int critical ( QWidget * parent, const QString & caption, const QString & text, 
			const QString & button0Text = QString::null, const QString & button1Text = QString::null, 
			const QString & button2Text = QString::null, int defaultButtonNumber = 0, int escapeButtonNumber = -1 );

	protected:

		// protected constructor
		FCMessageBox(const QString & caption, const QString & text, Icon icon, int button0, int button1, 
			int button2, QWidget * parent=0, const char * name=0, bool modal=TRUE, WFlags f=WStyle_DialogBorder);

		class FCCheckBox* checkBox;
		QGridLayout* layout;
};

/**  
 * FreeCAD's progressbar for long operations
 * If you call @ref Start() several times without calling
 * @ref Stop() before the number of new steps will be added 
 * to the number of current steps, i.e. nevertheless the 
 * progress bar will run only once.
 */
class FCProgressBarPrivate;

class FCProgressBar : public QProgressBar
{
  public:
    /// construction
    FCProgressBar ( QWidget * parent=0, const char * name=0, WFlags f=0 );
    virtual ~FCProgressBar ();
    /** Starts the progress bar */
    void Start(QString txt, int steps/*, bool& flag*/);
    /** Does the next step */
    void Next();
    /** Stops the sequencer */
    void Stop ();
    /** Resets the sequencer */
    void Reset();

  private:
	  /** @name for internal use only */
    //@{
    /** Checks if the ESC button was pressed 
     *  If ESC @ref interrupt() is called.
     */
    bool isInterrupted();
    /**  
     * Throws an exception to stop the pending operation.
     */
    void interrupt();
    /** Draws the content of the progress bar */
    virtual void drawContents( QPainter *p );
    /** Reimplemented */
    bool setIndicator ( QString & indicator, int progress, int totalSteps );
    //@}
    FCProgressBarPrivate* d;
};

/**
 * List view class
 * This class also has a method to get the last
 * element of the tree. @see lastItem()
 */
class FCListView : public QListView
{
  Q_OBJECT

  public:
    FCListView ( QWidget * parent, const char * name, WFlags f );
    FCListView ( QWidget * parent = 0, const char * name = 0 );
    virtual ~FCListView (); 

    QListViewItem * lastItem () const;
    static QListViewItem * lastItem (QListView*);
};

/**
 *  Icon items used by the 'FCCmdView' and 'FCDlgCustomize' classes
 */
class FCCmdViewItemPrivate;

class FCCmdViewItem : public QIconViewItem
{
  public:
    FCCmdViewItem ( QIconView * parent, QString Action, QAction* pAct );
    virtual ~FCCmdViewItem ();

    QString text() const;
    QString GetAction();

  protected:
    FCCmdViewItemPrivate* d;
};

/**
 * Icon view class
 * This class allows to drag one or more items which
 * correspond to a FreeCAD command. The dragged items you
 * can drop onto a @ref FCToolBar object
 * @see FCCmdViewItem, FCCommand
 */
class FCCmdView : public QIconView
{
  Q_OBJECT

  public:
    FCCmdView ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    virtual ~FCCmdView ();

  protected:
    /** Allows to drag an item */
    QDragObject * dragObject ();

  protected slots:
    /// for internal use only
    void slotSelectionChanged(QIconViewItem * item);

  signals:
    void emitSelectionChanged(QString);
};

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

/**
 *  Toolbar/command bar creation class
 */
class FCDlgCreateToolOrCmdBar : public QDialog
{ 
    Q_OBJECT

  public:
    FCDlgCreateToolOrCmdBar( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FCDlgCreateToolOrCmdBar();

    QGroupBox* GroupBox1;
    QLabel* TextLabel;
    QLineEdit* LineEditName;
    QCheckBox* CheckCreateCmdBar;
    QCheckBox* CheckCreateToolBar;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

  protected slots:
    void accept ();

  protected:
    QGridLayout* FCDlgCreateToolOrCmdBarLayout;
    QGridLayout* GroupBox1Layout;
    QHBoxLayout* Layout2;
};

/**
 * Line accelerator
 * A special implementation of QLineEdit useful
 * for pressing shortcuts.
 */
class FCAccelLineEdit : public QLineEdit
{
  public:
    FCAccelLineEdit ( QWidget * parent, const char * name=0 );

  protected:
     void keyPressEvent ( QKeyEvent * e);
};

/**
 *  A dialog having a checklist inside.
 */
class FCCheckListDlg : public QDialog
{ 
    Q_OBJECT

  public:
    FCCheckListDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FCCheckListDlg();

    /** set all items to be shown in the list */
    void setItems(const std::vector<std::string>& items);
    /** get all checked items */
    std::vector<int> getCheckedItems(); 
    void show ();
    void hide ();

  protected:
    std::vector<std::string> items;
    std::vector<int> checked;

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* GroupBox1;
    QListView* ListView;
    QGridLayout* FCCheckListDlgLayout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupBox1Layout;
};

/**
 *  Implementation of a color button
 */
class FCColorButton : public QButton
{
    Q_OBJECT

    Q_PROPERTY( QColor color READ color WRITE setColor )
 
 public:
    /// construction
    FCColorButton( QWidget* parent = 0, const char* name = 0 );
    FCColorButton( const QBrush& b, QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    /// destruction
    ~FCColorButton();

    /** set the color */
    void setColor( const QColor& );
    /** get the current color */
    QColor color() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    
  public slots:
    /** opens the @ref QColorDialog dialog to select a new color */
    virtual void onChooseColor();

  signals:
    /** emits this signal when color is changed */
    void changed();

  protected:
    /// for internal use only
    void drawButton( QPainter* );
    /// for internal use only
    void drawButtonLabel( QPainter* );

  private:
    QColor col;
};

/**
 * A special spinbox: augment or diminish the value by moving up or down 
 * keeping the left mouse button pressed
 */
class FCSpinBoxPrivate;
class FCSpinBox : public QSpinBox
{
  Q_OBJECT // this is important for the inherited classes

  public:
    FCSpinBox ( QWidget* parent, const char* name = 0 );
    FCSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name = 0 );
    virtual ~FCSpinBox();

  protected:
    void mouseMoveEvent    ( QMouseEvent* e );
    void mousePressEvent   ( QMouseEvent* e );
    void mouseReleaseEvent ( QMouseEvent* e );
    void wheelEvent        ( QWheelEvent* e );
    void focusOutEvent     ( QFocusEvent* e );
    bool eventFilter       ( QObject* o, QEvent* e );

  private:
    FCSpinBoxPrivate* d;
};

/** The FCFloatSpinBox class
 */
class FCFloatSpinBoxPrivate;
class FCFloatSpinBox : public FCSpinBox
{
    Q_OBJECT

    Q_PROPERTY( int    decimals   READ decimals      WRITE setDecimals      )
    Q_PROPERTY( double valueFloat READ valueFloat    WRITE setValueFloat    )
    Q_PROPERTY( double valueMax   READ maxValueFloat WRITE setMaxValueFloat )
    Q_PROPERTY( double valueMin   READ minValueFloat WRITE setMinValueFloat )

  public:
    FCFloatSpinBox ( QWidget * parent = 0, const char * name = 0 );
    FCFloatSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name = 0 );
    virtual ~FCFloatSpinBox();

    int    decimals () const;
    void   setDecimals ( int );
    double minValueFloat () const;
    void   setMinValueFloat ( double );
    double maxValueFloat () const;
    void   setMaxValueFloat ( double );
    double valueFloat () const;

  public slots:
    void  setValueFloat ( double );

  protected:
    virtual QString mapValueToText(int value);
    virtual int mapTextToValue(bool* ok);
    virtual void valueChange();
    void stepChange();

  signals:
    void valueFloatChanged(double value);

  private:
    FCFloatSpinBoxPrivate* d;
};

class FCAnimation : public QLabel
{
  public:
    static FCAnimation* Instance();
    static void Destruct();

    void startAnimation();
    void stopAnimation();

  private:
    FCAnimation(QWidget * parent, const char * name = 0, WFlags f = 0);
    virtual ~FCAnimation();

    static FCAnimation* _pcSingleton;
};

#endif // __FC_WIDGETS_H__
