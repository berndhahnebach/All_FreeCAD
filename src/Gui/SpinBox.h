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


#ifndef GUI_SPINBOX_H
#define GUI_SPINBOX_H

#include <qspinbox.h>
#include <qvalidator.h>

namespace Gui {

class GuiExport UnsignedValidator : public QValidator
{
  Q_OBJECT
  Q_PROPERTY( uint bottom READ bottom WRITE setBottom )
  Q_PROPERTY( uint top READ top WRITE setTop )

public:
  UnsignedValidator( QObject * parent, const char *name = 0 );
  UnsignedValidator( uint bottom, uint top, QObject * parent, const char *name = 0 );
  ~UnsignedValidator();

  QValidator::State validate( QString &, int & ) const;

  void setBottom( uint );
  void setTop( uint );
  virtual void setRange( uint bottom, uint top );

  uint bottom() const { return b; }
  uint top() const { return t; }

private:
  uint b, t;
};

/**
 * A special spin box: augmenting or diminishing its value by moving up or down
 * keeping the left mouse button pressed.
 * \author Werner Mayer
 */
class SpinBoxPrivate;
class GuiExport SpinBox : public QSpinBox
{
  Q_OBJECT // this is important for the inherited classes

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

class UIntSpinBoxPrivate;
class GuiExport UIntSpinBox : public SpinBox
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

/** 
 * The FloatSpinBox class provides a spin box working with floating point numbers.
 * The code is taken in part from KDoubleSpinBox implementation of the KDE library.
 * Furthermore some bugs are fixed, such as:
 * \li setting value, precision or linestep after min./max. value are set without loosing accuracy
 * \li stability factor added to make conversion between double <-> int more robust
 * \author Werner Mayer
 */
class FloatSpinBoxPrivate;
class GuiExport FloatSpinBox : public SpinBox 
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
  /** Emitted whenever @ref QSpinBox::valueChanged( int ) is emitted. */
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

} // namespace Gui

#endif // GUI_SPINBOX_H
