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


#ifndef ___SPINBOX_H__
#define ___SPINBOX_H__

#ifndef _PreComp_
# include <qspinbox.h>
#endif

namespace Gui {

/**
 * A special spin box: augmenting or diminishing its value by moving up or down
 * keeping the left mouse button pressed.
 * \author Werner Mayer
 */
class SpinBoxPrivate;
class SpinBox : public QSpinBox
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

/** 
 * The FloatSpinBox class provides a spin box working with floating point numbers.
 * \author Werner Mayer
 */
class FloatSpinBoxPrivate;
class FloatSpinBox : public SpinBox
{
  Q_OBJECT

  Q_PROPERTY( uint   decimals        READ decimals      WRITE setDecimals      )
  Q_PROPERTY( double valueFloat      READ valueFloat    WRITE setValueFloat    )
  Q_PROPERTY( double maxValueFloat   READ maxValueFloat WRITE setMaxValueFloat )
  Q_PROPERTY( double minValueFloat   READ minValueFloat WRITE setMinValueFloat )

public:
  FloatSpinBox ( QWidget * parent = 0, const char * name = 0 );
  FloatSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name = 0 );
  virtual ~FloatSpinBox();

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
  FloatSpinBoxPrivate* d;
};

} // namespace Gui

#endif // ___SPINBOX_H__
