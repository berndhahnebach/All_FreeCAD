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


#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qiconview.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qwidgetplugin.h>

namespace Gui
{

class QT_WIDGET_PLUGIN_EXPORT FileChooser : public QWidget
{
  Q_OBJECT

  Q_ENUMS( Mode )
  Q_PROPERTY( Mode mode READ mode WRITE setMode )
  Q_PROPERTY( QString  fileName  READ fileName      WRITE setFileName      )
  Q_PROPERTY( QString  filter    READ filter        WRITE setFilter        )
  Q_PROPERTY( QString  buttonText  READ buttonText  WRITE setButtonText    )

public:
  enum Mode { File, Directory };

  FileChooser ( QWidget * parent = 0, const char * name = 0 );
  virtual ~FileChooser();


  QString filter() const;
  QString fileName() const;
  Mode mode() const;
  QString buttonText() const;

public slots:
  void setFileName( const QString &fn );
  void setMode( Mode m );
  void setFilter ( const QString & );
  void setButtonText ( const QString & );

signals:
  void fileNameChanged( const QString & );
  void fileNameSelected( const QString & );

private slots:
  void chooseFile();

private:
  QLineEdit *lineEdit;
  QPushButton *button;
  Mode md;
  QString _filter;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefFileChooser : public FileChooser
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefFileChooser ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefFileChooser();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT AccelLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  AccelLineEdit ( QWidget * parent=0, const char * name=0 );

protected:
  void keyPressEvent ( QKeyEvent * e);
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT CommandIconView : public QIconView
{
  Q_OBJECT

public:
  CommandIconView ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  virtual ~CommandIconView ();

protected slots:
  void onSelectionChanged( QIconViewItem * item );

signals:
  void emitSelectionChanged( const QString& );
};

// ------------------------------------------------------------------------------

class SpinBoxPrivate;
class QT_WIDGET_PLUGIN_EXPORT SpinBox : public QSpinBox
{
  Q_OBJECT

public:
  SpinBox ( QWidget* parent, const char* name = 0 );
  SpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name = 0 );
  virtual ~SpinBox();

protected:
  void mouseMoveEvent    ( QMouseEvent* e );
  void mousePressEvent   ( QMouseEvent* e );
  void mouseReleaseEvent ( QMouseEvent* e );
  void wheelEvent        ( QWheelEvent* e );
  void focusOutEvent     ( QFocusEvent* e );
  bool eventFilter       ( QObject* o, QEvent* e );

private:
  SpinBoxPrivate* d;
};

// -------------------------------------------------------------

class FloatSpinBoxPrivate;
class QT_WIDGET_PLUGIN_EXPORT FloatSpinBox : public SpinBox 
{
  Q_OBJECT
  Q_OVERRIDE( double maxValue READ maxValue WRITE setMaxValue )
  Q_OVERRIDE( double minValue READ minValue WRITE setMinValue )
  Q_OVERRIDE( double lineStep READ lineStep WRITE setLineStep )
  Q_OVERRIDE( double value READ value WRITE setValue )
  Q_PROPERTY( uint precision READ precision WRITE setPrecision )

public:
  FloatSpinBox( QWidget * parent=0, const char * name=0 );
  FloatSpinBox( double minValue, double maxValue, double step, double value,
                uint precision=2, QWidget * parent=0, const char * name=0 );
  virtual ~FloatSpinBox();

  void setRange( double lower, double upper, double step=0.01, uint precision=2 );

  uint precision() const;
  void setPrecision( uint precision );
  virtual void setPrecision( uint precision, bool force );

  double value() const;
  double minValue() const;
  void setMinValue( double value );
  double maxValue() const;
  void setMaxValue( double value );

  double lineStep() const;
  void setLineStep( double step );

  void setValidator( const QValidator * );

signals:
  void valueChanged( double value );

public slots:
  virtual void setValue( double value );

protected:
  virtual void valueChange();
  virtual void stepChange();
  virtual QString mapValueToText(int);
  virtual int mapTextToValue(bool*);

private:
  void updateValidator();
  uint maxPrecision() const;

  FloatSpinBoxPrivate * d;
};

// -------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefSpinBox : public SpinBox
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefSpinBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefSpinBox();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefFloatSpinBox : public FloatSpinBox
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefFloatSpinBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefFloatSpinBox();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefLineEdit : public QLineEdit
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefLineEdit ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefLineEdit();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefComboBox : public QComboBox
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefComboBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefComboBox();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefListBox : public QListBox
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefListBox ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
  virtual ~PrefListBox();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefCheckBox : public QCheckBox
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefCheckBox ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefCheckBox();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefRadioButton : public QRadioButton
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefRadioButton ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefRadioButton();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefButtonGroup : public QButtonGroup
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefButtonGroup ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefButtonGroup();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefSlider : public QSlider
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefSlider ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefSlider();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT ColorButton : public QPushButton
{
  Q_OBJECT

  Q_PROPERTY( QColor color READ color WRITE setColor )

public:
  ColorButton( QWidget* parent = 0, const char* name = 0 );
  ~ColorButton();

  void setColor( const QColor& );
  QColor color() const;

public slots:
  virtual void onChooseColor();

signals:
  void changed();

protected:
  void drawButtonLabel( QPainter* );

private:
  QColor _col;
};

// ------------------------------------------------------------------------------

class QT_WIDGET_PLUGIN_EXPORT PrefColorButton : public ColorButton
{
  Q_OBJECT

  Q_PROPERTY( QCString prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QCString prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefColorButton ( QWidget * parent = 0, const char * name = 0 );
  virtual ~PrefColorButton();

  QCString entryName    () const;
  QCString paramGrpPath () const;
  void  setEntryName     ( const QCString& name );
  void  setParamGrpPath  ( const QCString& name );

private:
  QCString m_sPrefName;
  QCString m_sPrefGrp;
};

} // namespace Gui

#endif
