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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qapplication.h>
# include <qcursor.h>
# include <qlineedit.h>
# include <qvalidator.h>
# include <cmath>
#endif

#include "SpinBox.h"

using namespace Gui;


namespace Gui {

class SpinBoxPrivate
{
public:
  SpinBoxPrivate();
  bool pressed;
  int nY, nStep;
};

SpinBoxPrivate::SpinBoxPrivate()
{
  pressed = false;
}

class FloatSpinBoxPrivate
{
public:
  FloatSpinBoxPrivate(){}
  ~FloatSpinBoxPrivate()
  {
    delete m_pValidator;
  }

  QDoubleValidator* m_pValidator;
  double            m_fDivisor;
  double            m_fEpsilon;
};

} // namespace Gui

// -----------------------------------------------------------------------------------

/**
 * Constructs a spin box with the default QRangeControl range and step values. 
 * It is called \a name and has parent \a parent.
 */
SpinBox::SpinBox ( QWidget* parent, const char* name )
  : QSpinBox (parent, name)
{
  setMouseTracking(true);
  d = new SpinBoxPrivate;
}

/**
 * Constructs a spin box that allows values from \a minValue to \a maxValue inclusive, with 
 * step amount \a step. The value is initially set to \a minValue.
 *
 * The spin box is called \a name and has parent \a parent.
 */
SpinBox::SpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name )
    : QSpinBox(minValue, maxValue, step, parent, name)
{
  setMouseTracking(true);
  d = new SpinBoxPrivate;
}

/**
 * Destroys the spin box, freeing all memory and other resources.
 */
SpinBox::~SpinBox()
{
  delete d;
  d = 0L;
}

/**
 * If this widget grabs the mouse its value is either augmented in case the mouse moves up or diminished
 * in case the mouse moves down.
 */
void SpinBox::mouseMoveEvent ( QMouseEvent* e )
{
  if (QWidget::mouseGrabber() == 0 && !rect().contains(e->pos()) && d->pressed )
    grabMouse( QCursor(IbeamCursor) );

  if (QWidget::mouseGrabber() == this)
  {
    // get "speed" of mouse move
    int mult = d->nY - e->y();

    int nValue = value() + mult * d->nStep;
    if (nValue <= maxValue())
      setValue ( nValue );
    else
      setValue ( maxValue() );

    d->nY = e->y();
  }
  else
    QSpinBox::mouseMoveEvent(e);
}

/**
 * Grabs the mouse.
 */
void SpinBox::mousePressEvent   ( QMouseEvent* e )
{
  d->pressed = true;

  int nMax = maxValue();
  int nMin = minValue();

  if (nMax == INT_MAX || nMin == -INT_MAX)
  {
    d->nStep = 100;
  }
  else
  {
    int nRange = nMax - nMin;
    int nHeight = QApplication::desktop()->height();
    if (nRange > nHeight)
      d->nStep = int(nRange / nHeight);
    else
      d->nStep = 1;
  }

  d->nY = e->y();
}

/**
 * Ungrabs the mouse.
 */
void SpinBox::mouseReleaseEvent ( QMouseEvent* e )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
  d->pressed = false;
}

/**
 * Ungrabs the mouse if this spin box loses the focus.
 */
void SpinBox::focusOutEvent ( QFocusEvent* e )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
}

void SpinBox::wheelEvent ( QWheelEvent* e )
{
  if (isEnabled())
    QSpinBox::wheelEvent(e);
}

/**
 * Some special handlings for moving and grabbing.
 */
bool SpinBox::eventFilter ( QObject* o, QEvent* e )
{
  if ( o != editor() )
    return false;

  // get the editor's mouse events
  switch (e->type())
  {
    // redirect the events to spin box (itself)
  case QEvent::MouseButtonPress:
    mousePressEvent ((QMouseEvent*)e);
    break;

  case QEvent::MouseButtonRelease:
    mouseReleaseEvent ((QMouseEvent*)e);
    break;

  case QEvent::MouseMove:
    mouseMoveEvent ((QMouseEvent*)e);
    break;

  case QEvent::FocusOut:
    focusOutEvent ((QFocusEvent*)e);
    break;

  default:
    break;
  }

  return QSpinBox::eventFilter(o, e);
}

// -------------------------------------------------------------

/**
 * Constructs a spin box with the default QRangeControl range and step values. 
 * It is called \a name and has parent \a parent. The spin box works with floating point numbers.
 */
FloatSpinBox::FloatSpinBox ( QWidget * parent, const char * name )
  : SpinBox(parent, name)
{
  d = new FloatSpinBoxPrivate;

  d->m_pValidator = new QDoubleValidator((double)minValue(), (double)maxValue(), 2, this, name );
  setValidator(d->m_pValidator);

  setDecimals(0);
}

/**
 * Constructs a spin box that allows values from \a minValue to \a maxValue inclusive, with 
 * step amount \a step. The value is initially set to \a minValue. The spin box works
 * with floating point numbers.
 *
 * The spin box is called \a name and has parent \a parent.
 */
FloatSpinBox::FloatSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name )
    : SpinBox(minValue, maxValue, step, parent, name)
{
  d = new FloatSpinBoxPrivate;

  d->m_pValidator = new QDoubleValidator((double)minValue, (double)maxValue, 2, this, name );
  setValidator(d->m_pValidator);

  setDecimals(0);
}

/**
 * Destroys the spin box, freeing all memory and other resources.
 */
FloatSpinBox::~FloatSpinBox()
{
  delete d;
}

/**
 * Returns the maximum number of digits after the decimal point.
 */
uint FloatSpinBox::decimals() const
{
  return d->m_pValidator->decimals();
}

/**
 * Sets the maximum number of digits after the decimal point.
 */
void FloatSpinBox::setDecimals(uint i)
{
  d->m_pValidator->setDecimals(i);
  d->m_fDivisor = pow(10.0, double(i));
  d->m_fEpsilon = 1.0 / pow(10.0, double(i+1));
}

/**
 * Sets the minimum floating point value of the spin box.
 */
void FloatSpinBox::setMinValue(double value)
{
  double fMax = d->m_fDivisor * value;
  fMax = std::max<double>(fMax, (double)-INT_MAX);
  SpinBox::setMinValue(int(fMax));
}

/**
 * Sets the maximum floating point value of the spin box.
 */
void FloatSpinBox::setMaxValue(double value)
{
  double fMin = d->m_fDivisor * value;
  fMin = std::min<double>(fMin, (double)INT_MAX);
  SpinBox::setMaxValue(int(fMin));
}

/**
 * Returns the minimum floating point value of the spin box.
 */
double FloatSpinBox::minValue () const
{
  return SpinBox::minValue() / double(d->m_fDivisor);
}

/**
 * Returns the maximum floating point value of the spin box.
 */
double FloatSpinBox::maxValue () const
{
  return SpinBox::maxValue() / double(d->m_fDivisor);
}

/**
 * This virtual function is used by the spin box whenever it needs to display value \a value.
 */
QString FloatSpinBox::mapValueToText(int value)
{
  return QString::number(double(value) / d->m_fDivisor, 'f', d->m_pValidator->decimals());
}

/**
 * This virtual function is used by the spin box whenever it needs to interpret text entered by 
 * the user as a value. The text is available as text() and as cleanText(), and this function 
 * must parse it if possible.
 */ 
int FloatSpinBox::mapTextToValue( bool* ok )
{
  double fEps = value() > 0.0 ? d->m_fEpsilon : - d->m_fEpsilon;
  return int(text().toDouble() * d->m_fDivisor + fEps);
}

/**
 * This virtual function is called by QRangeControl whenever the value has changed.
 */
void FloatSpinBox::valueChange()
{
  SpinBox::valueChange();
  emit valueFloatChanged( value() );
}

/**
 * Sets the value of the spin box to \a value.
 */
void FloatSpinBox::setValue(double value)
{
  double fEps = value > 0.0 ? d->m_fEpsilon : - d->m_fEpsilon;
  double fValue = d->m_fDivisor * value + fEps;
  fValue = std::min<double>(fValue, (double) INT_MAX);
  fValue = std::max<double>(fValue, (double)-INT_MAX);
  SpinBox::setValue(int(fValue));
}

/**
 * Returns the value of the spin box.
 */
double FloatSpinBox::value() const
{
  return SpinBox::value() / double(d->m_fDivisor);
}

/**
 * This function is called whenever the range control's line or page step settings change.
 */
void FloatSpinBox::stepChange ()
{
  SpinBox::stepChange();
}

#include "moc_SpinBox.cpp"
