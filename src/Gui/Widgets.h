/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef ___WIDGETS_H__
#define ___WIDGETS_H__

#ifndef _PreComp_
# include <qbutton.h>
# include <qprogressbar.h>
# include <qlabel.h>
# include <qiconview.h>
//#include <qdragobject.h>
//#include <qstatusbar.h>
//#include <qtoolbar.h>
# include <qfiledialog.h>
# include <qmessagebox.h>
//#include <qvariant.h>
# include <qdialog.h>
# include <qspinbox.h>
# include <qlistview.h>
# include <qlineedit.h>
# include <string>
# include <vector>
#endif

#include "../Base/Sequencer.h"

//namespace Gui {

class QHBoxLayout;
class QTime;
class QGridLayout;

/**
 * Using the Qt's open/save dialogs with own adjustments
 */
class GuiExport FileDialog : public QFileDialog
{
  Q_OBJECT

public:
  static QString getOpenFileName( const QString &initially = QString::null, const QString &filter = QString::null, QWidget *parent = 0, const char* name = 0 );
  static QString getOpenFileName( const QString &initially, const QString &filter, QWidget *parent, const char* name, const QString& caption );
  static QString getSaveFileName ( const QString & initially = QString::null, const QString & filter = QString::null, QWidget * parent = 0, const char * name = 0 );
  static QString getSaveFileName ( const QString & initially, const QString & filter, QWidget * parent, const char * name, const QString & caption );
  static QString getSaveFileName ( const QString & initially, const QString & filter, QWidget * parent, const QString & caption );

public:
  FileDialog (Mode mode, QWidget* parent = 0, const char* name = 0, bool modal = false);
  FileDialog (Mode mode, const QString& dirName, const QString& filter = QString::null, QWidget* parent = 0, const char* name = 0, bool modal = false);
  virtual ~FileDialog();

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

namespace Gui {

struct ProgressBarPrivate;

/**
 * FreeCAD's progressbar for long operations
 * If you call @ref Start() several times without calling
 * @ref Stop() before the number of new steps will be added
 * to the number of current steps, i.e. nevertheless the
 * progress bar will run only once.
 */
class ProgressBar : public QProgressBar, public Base::SequencerBase
{
public:
  /**
  * Returns the sequencer object
  */
  static ProgressBar* Instance();
  /** Starts the progress bar */
  bool start(const char* pszStr, unsigned long steps);
  /** Does the next step */
  bool next();

  /** Handles all incoming events while the
   * progress bar is running. All key and mouse
   * events are ignored to block user input.
   */
  bool eventFilter(QObject* o, QEvent* e);

private:
  /** construction */
  ProgressBar ( QWidget * parent=0, const char * name=0, WFlags f=0 );
  /** Destruction */
  ~ProgressBar ();
  /** @name for internal use only */
  //@{
  /** Puts text to the status bar */
  void setText (const char* pszTxt);
  /** Get the events under control */
  void enterControlEvents();
  /** Lose the control over incoming events*/
  void leaveControlEvents();
  /** Throws an exception to stop the pending operation. */
  void abort();
  /** Resets the sequencer */
  void resetData();
  /** Draws the content of the progress bar */
  //    void drawContents( QPainter *p );
  /** Reimplemented */
  bool setIndicator ( QString & indicator, int progress, int totalSteps );
  //@}
  ProgressBarPrivate* d;
  static ProgressBar* _pclSingleton;
};

} // namespace Gui

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
class QAction;

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
  void emitSelectionChanged(const QString&);
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
  FCAccelLineEdit ( QWidget * parent=0, const char * name=0 );

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
  void setItems(const std::vector<std::pair<std::string, bool> >& items);
  /** get all checked items */
  std::vector<std::string> getCheckedItems() const;
  void show ();
  void hide ();

protected:
  std::vector<std::pair<std::string, bool> > items;
  std::vector<std::string> checked;

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
  /** opens the QColorDialog dialog to select a new color */
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

  Q_PROPERTY( uint   decimals        READ decimals      WRITE setDecimals      )
  Q_PROPERTY( double valueFloat      READ valueFloat    WRITE setValueFloat    )
  Q_PROPERTY( double maxValueFloat   READ maxValueFloat WRITE setMaxValueFloat )
  Q_PROPERTY( double minValueFloat   READ minValueFloat WRITE setMinValueFloat )

public:
  FCFloatSpinBox ( QWidget * parent = 0, const char * name = 0 );
  FCFloatSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name = 0 );
  virtual ~FCFloatSpinBox();

  uint   decimals () const;
  void   setDecimals ( uint );
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

//} // namespace Gui

#endif // __FC_WIDGETS_H__
