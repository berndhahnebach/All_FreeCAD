/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_CUSTOMWIDGETS_H
#define GUI_CUSTOMWIDGETS_H

#include <QButtonGroup>
#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

namespace Gui
{

class UrlLabel : public QLabel
{
  Q_OBJECT
  Q_PROPERTY( QString  url    READ url   WRITE setUrl)

public:
  UrlLabel ( QWidget * parent = 0, Qt::WFlags f = 0 );
  virtual ~UrlLabel();

  QString url() const;

public Q_SLOTS:
  void setUrl( const QString &u );

protected:
  void enterEvent ( QEvent * );
  void leaveEvent ( QEvent * );
  void mouseReleaseEvent ( QMouseEvent * );

private:
  QString _url;
};

class FileChooser : public QWidget
{
  Q_OBJECT

  Q_ENUMS( Mode )
  Q_PROPERTY( Mode mode READ mode WRITE setMode )
  Q_PROPERTY( QString  fileName  READ fileName      WRITE setFileName      )
  Q_PROPERTY( QString  filter    READ filter        WRITE setFilter        )
  Q_PROPERTY( QString  buttonText  READ buttonText  WRITE setButtonText    )

public:
  enum Mode { File, Directory };

  FileChooser (QWidget *parent = 0);
  virtual ~FileChooser();


  QString filter() const;
  QString fileName() const;
  Mode mode() const;
  QString buttonText() const;

public Q_SLOTS:
  void setFileName( const QString &fn );
  void setMode( Mode m );
  void setFilter ( const QString & );
  void setButtonText ( const QString & );

Q_SIGNALS:
  void fileNameChanged( const QString & );
  void fileNameSelected( const QString & );

private Q_SLOTS:
  void chooseFile();

private:
  QLineEdit *lineEdit;
  QPushButton *button;
  Mode md;
  QString _filter;
};

// ------------------------------------------------------------------------------

class PrefFileChooser : public FileChooser
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefFileChooser ( QWidget * parent = 0 );
  virtual ~PrefFileChooser();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class AccelLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  AccelLineEdit ( QWidget * parent=0 );

protected:
  void keyPressEvent ( QKeyEvent * e);
};

// ------------------------------------------------------------------------------

class CommandIconView : public QListWidget
{
  Q_OBJECT

public:
  CommandIconView ( QWidget * parent = 0 );
  virtual ~CommandIconView ();

protected:
  void startDrag ( Qt::DropActions supportedActions );

protected Q_SLOTS:
  void onSelectionChanged( QListWidgetItem * item, QListWidgetItem * );

Q_SIGNALS:
  void emitSelectionChanged( const QString& );
};

// -------------------------------------------------------------

class SpinBoxPrivate;
class SpinBox : public QSpinBox
{
  Q_OBJECT

public:
  SpinBox ( QWidget* parent );
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
/*
class UIntSpinBoxPrivate;
class QT_WIDGET_PLUGIN_EXPORT UIntSpinBox : public SpinBox
{
  Q_OBJECT
  Q_OVERRIDE( uint maxValue READ maxValue WRITE setMaxValue )
  Q_OVERRIDE( uint minValue READ minValue WRITE setMinValue )
  Q_OVERRIDE( uint value READ value WRITE setValue )

public:
  UIntSpinBox ( QWidget* parent, const char* name = 0 );
  virtual ~UIntSpinBox();

  void setRange( uint minVal, uint maxVal );
  uint value() const;
  uint minValue() const;
  void setMinValue( uint value );
  uint maxValue() const;
  void setMaxValue( uint value );

  void setValidator( const QValidator * );

signals:
  void valueChanged( uint value );

public slots:
  void setValue( uint value );

protected:
  QString mapValueToText( int v );
  int mapTextToValue ( bool * ok );
  void valueChange();
  void rangeChange();

private:
  void updateValidator();
  UIntSpinBoxPrivate * d;
};
*/
// -------------------------------------------------------------

class PrefSpinBox : public SpinBox
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefSpinBox ( QWidget * parent = 0 );
  virtual ~PrefSpinBox();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class ColorButton : public QPushButton
{
  Q_OBJECT

  Q_PROPERTY( QColor color READ color WRITE setColor )

public:
  ColorButton( QWidget* parent = 0 );
  ~ColorButton();

  void setColor( const QColor& );
  QColor color() const;

public Q_SLOTS:
  virtual void onChooseColor();

Q_SIGNALS:
  void changed();

protected:
  void paintEvent ( QPaintEvent* );

private:
  QColor _col;
};

// ------------------------------------------------------------------------------

class PrefColorButton : public ColorButton
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefColorButton ( QWidget * parent = 0 );
  virtual ~PrefColorButton();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class PrefDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefDoubleSpinBox ( QWidget * parent = 0 );
  virtual ~PrefDoubleSpinBox();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class PrefLineEdit : public QLineEdit
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefLineEdit ( QWidget * parent = 0 );
  virtual ~PrefLineEdit();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class PrefComboBox : public QComboBox
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefComboBox ( QWidget * parent = 0 );
  virtual ~PrefComboBox();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------
/*
class PrefListBox : public QListBox
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefListBox ( QWidget * parent = 0 );
  virtual ~PrefListBox();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};
*/
// ------------------------------------------------------------------------------

class PrefCheckBox : public QCheckBox
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefCheckBox ( QWidget * parent = 0 );
  virtual ~PrefCheckBox();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class PrefRadioButton : public QRadioButton
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefRadioButton ( QWidget * parent = 0 );
  virtual ~PrefRadioButton();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

// ------------------------------------------------------------------------------

class PrefSlider : public QSlider
{
  Q_OBJECT

  Q_PROPERTY( QByteArray prefEntry READ entryName     WRITE setEntryName     )
  Q_PROPERTY( QByteArray prefPath  READ paramGrpPath  WRITE setParamGrpPath  )

public:
  PrefSlider ( QWidget * parent = 0 );
  virtual ~PrefSlider();

  QByteArray entryName    () const;
  QByteArray paramGrpPath () const;
  void  setEntryName     ( const QByteArray& name );
  void  setParamGrpPath  ( const QByteArray& name );

private:
  QByteArray m_sPrefName;
  QByteArray m_sPrefGrp;
};

} // namespace Gui

#endif // GUI_CUSTOMWIDGETS_H
