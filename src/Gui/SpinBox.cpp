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
# include <assert.h>
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

namespace Gui {

class FloatSpinBoxPrivate {
public:
  FloatSpinBoxPrivate( int precision=1 )
    : mPrecision( precision ),
      mValue(0.0), mLineStep(0.1),
      mMinValue(0.0), mMaxValue(9.9),
      mValidator( 0 )
  {
  }

  int factor() const {
    int f = 1;
    for ( uint i = 0 ; i < mPrecision ; ++i ) f *= 10;
    return f;
  }

  double epsilon() const {
    // needed for numerical stability
    double f = 5;
    for ( uint i = 0 ; i < mPrecision+1 ; ++i ) f /= 10;
    return f;
  }

  double basicStep() const {
    return 1.0/double(factor());
  }

  int mapToInt( double value, bool * ok ) const {
    assert( ok );
    const double f = factor();
    if ( value > double(INT_MAX) / f ) {
      *ok = false;
      return INT_MAX;
    } else if ( value < double(INT_MIN) / f ) {
      *ok = false;
      return INT_MIN;
    } else {
      *ok = true;
//      return int( value * f + ( value < 0 ? -0.5 : 0.5 ) );
      return int( value * f + ( value < 0 ? -epsilon() : epsilon() ) );
    }
  }

  double mapToDouble( int value ) const {
    return double(value) * basicStep();
  }

  uint mPrecision;
  double mValue, mLineStep;
  double mMinValue, mMaxValue;
  QDoubleValidator * mValidator;
};

} // namespace Gui

/**
 * Constructs a spin box with the default QRangeControl range and step values. 
 * It is called \a name and has parent \a parent. The spin box works with floating point numbers.
 * Default precision is set to 1.
 */
FloatSpinBox::FloatSpinBox( QWidget * parent, const char * name )
  : SpinBox( parent, name )
{
  d = new FloatSpinBoxPrivate();
  updateValidator();
}

/**
 * Constructs a spin box that allows values from \a minValue to \a maxValue inclusive, with 
 * step amount \a step. The value is initially set to \a value and precision to \a precision. 
 * The spin box works with floating point numbers.
 *
 * The spin box is called \a name and has parent \a parent.
 */
FloatSpinBox::FloatSpinBox( double minValue, double maxValue, double step,
        double value, uint precision, QWidget * parent, const char * name )
  : SpinBox( parent, name )
{
  d = new FloatSpinBoxPrivate();
  setRange( minValue, maxValue, step, precision );
  setValue( value );
}

/**
 * Destroys the spin box, freeing all memory and other resources.
 */
FloatSpinBox::~FloatSpinBox() {
  delete d; d = 0;
}

/** 
 * Sets a new range for the spin box values. Note that \a lower, \a upper 
 * and \a step are rounded to \a precision decimal points first. 
 */
void FloatSpinBox::setRange( double lower, double upper, double step, uint precision )
{
  lower = lower < upper ? lower : upper;
  upper = lower < upper ? upper : lower;
  setPrecision( precision, true ); // disable bounds checking, since
  setMinValue( lower );            // it's done in set{Min,Max}Value
  setMaxValue( upper );            // anyway and we want lower, upper
  setLineStep( step );             // and step to have the right precision
}

/**
 * Returns the maximum number of digits after the decimal point.
 */
uint FloatSpinBox::precision() const 
{
  return d->mPrecision;
}

/**
 * Sets the maximum number of digits after the decimal point.
 */
/** Equivalent to @ref setPrecsion( @p precison, @p false ); Needed
    since Qt's moc doesn't ignore trailing parameters with default
    args when searching for a property setter method. */
void FloatSpinBox::setPrecision( uint precision ) 
{
  setPrecision( precision, false );
}

/** Sets the number of decimal points to use. Note that there is a
    tradeoff between the precision used and the available range of
    values. See the class docs for more.
    @param precision the new number of decimal points to use

    @param force disables checking of bound violations that can
           arise if you increase the precision so much that the
           minimum and maximum values can't be represented
           anymore. Disabling is useful if you don't want to keep
           the current min and max values anyway. This is what
           e.g. @ref setRange() does.
**/
void FloatSpinBox::setPrecision( uint precision, bool force ) 
{
  if ( precision < 1 ) return;
  if ( !force ) {
    uint maxPrec = maxPrecision();
    if ( precision > maxPrec )
      precision = maxPrec;
  }

//  updateValidator();
  // if we set precision we also must adjust min. and max. value
  d->mPrecision = precision;
  setMaxValue( d->mMaxValue );
  setMinValue( d->mMinValue );
  setLineStep( d->mLineStep );
  setValue   ( d->mValue    );
}

uint FloatSpinBox::maxPrecision() const 
{
  // INT_MAX must be > maxAbsValue * 10^precision
  // ==> 10^precision < INT_MAX / maxAbsValue
  // ==> precision < log10 ( INT_MAX / maxAbsValue )
  // ==> maxPrecision = floor( log10 ( INT_MAX / maxAbsValue ) );
  double maxAbsValue = std::max<double>( fabs(minValue()), fabs(maxValue()) );
  if ( maxAbsValue == 0 ) return 6; // return arbitrary value to avoid dbz...

  return int( floor( log10( double(INT_MAX) / maxAbsValue ) ) );
}

/**
 * Returns the value of the spin box.
 */
double FloatSpinBox::value() const 
{
  return d->mapToDouble( SpinBox::value() );
}

/**
 * Sets the value of the spin box to \a value.
 */
  /** Sets the current value to @p value, cubject to the constraints
      that @p value is frist rounded to the current precision and then
      clipped to the interval [@p minvalue(),@p maxValue()]. */
void FloatSpinBox::setValue( double value ) 
{
  if ( value == this->value() ) return;
  if ( value < minValue() )
    SpinBox::setValue( SpinBox::minValue() );
  else if ( value > maxValue() )
    SpinBox::setValue( SpinBox::maxValue() );
  else 
  {
    bool ok = false;
    SpinBox::setValue( d->mapToInt( value, &ok ) );
    d->mValue = value;
    assert( ok );
  }
}

/**
 * Returns the minimum floating point value of the spin box.
 */
double FloatSpinBox::minValue() const 
{
  return d->mapToDouble( SpinBox::minValue() );
}

/**
 * Sets the minimum floating point value of the spin box.
 */
  /** Sets the lower bound of the range to @p value, subject to the
      contraints that @p value is first rounded to the current
      precision and then clipped to the maximum representable
      interval.
      @see maxValue, minValue, setMaxValue, setRange
  */
void FloatSpinBox::setMinValue( double value ) 
{
  bool ok = false;
  int min = d->mapToInt( value, &ok );
  if ( !ok ) return;
  SpinBox::setMinValue( min );
  updateValidator();
  d->mMinValue = value;
}

/**
 * Returns the maximum floating point value of the spin box.
 */
double FloatSpinBox::maxValue() const 
{
  return d->mapToDouble( SpinBox::maxValue() );
}

/**
 * Sets the maximum floating point value of the spin box.
 */
  /** Sets the upper bound of the range to @p value, subject to the
      contraints that @p value is first rounded to the current
      precision and then clipped to the maximum representable
      interval.
      @see minValue, maxValue, setMinValue, setRange
  */
void FloatSpinBox::setMaxValue( double value ) 
{
  bool ok = false;
  int max = d->mapToInt( value, &ok );
  if ( !ok ) return;
  SpinBox::setMaxValue( max );
  updateValidator();
  d->mMaxValue = value;
}

/** @return the current step size */
double FloatSpinBox::lineStep() const 
{
  return d->mapToDouble( SpinBox::lineStep() );
}

/** Sets the step size for clicking the up/down buttons to @p step,
    subject to the constraints that @p step is first rounded to the
    current precision and then clipped to the meaningful interval
    [1, @p maxValue - @p minValue]. */
void FloatSpinBox::setLineStep( double step ) 
{
  bool ok = false;
  if ( step > maxValue() - minValue() )
    SpinBox::setLineStep( 1 );
  else
  {
    SpinBox::setLineStep( std::max<int>( d->mapToInt( step, &ok ), 1 ) );
    d->mLineStep = step;
  }
}

/**
 * This virtual function is used by the spin box whenever it needs to display value \a value.
 */
QString FloatSpinBox::mapValueToText( int value ) 
{
  return QString().setNum( d->mapToDouble( value ), 'f', d->mPrecision );
}

/**
 * This virtual function is used by the spin box whenever it needs to interpret text entered by 
 * the user as a value. The text is available as text() and as cleanText(), and this function 
 * must parse it if possible.
 */ 
int FloatSpinBox::mapTextToValue( bool * ok ) 
{
  double value = cleanText().toDouble( ok );
  if ( !*ok ) return 0;
  if ( value > maxValue() )
    value = maxValue();
  else if ( value < minValue() )
    value = minValue();
  return d->mapToInt( value, ok );
}

/**
 * This virtual function is called by QRangeControl whenever the value has changed.
 */
void FloatSpinBox::valueChange()
{
  SpinBox::valueChange();
  emit valueChanged( d->mapToDouble( SpinBox::value() ) );
}

/**
 * This function is called whenever the range control's line or page step settings change.
 */
void FloatSpinBox::stepChange ()
{
  SpinBox::stepChange();
}

/** Overridden to ignore any setValidator() calls. */
void FloatSpinBox::setValidator( const QValidator * ) 
{
  // silently discard the new validator. We don't want another one ;-)
}

void FloatSpinBox::updateValidator() 
{
  if ( !d->mValidator ) 
  {
    d->mValidator =  new QDoubleValidator( minValue(), maxValue(), precision(),
             this, "d->mValidator" );
    SpinBox::setValidator( d->mValidator );
  } 
  else
    d->mValidator->setRange( minValue(), maxValue(), precision() );
}

#include "moc_SpinBox.cpp"
