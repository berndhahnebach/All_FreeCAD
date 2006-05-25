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


#include "customwidgets.h"

#include <qaccel.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qvalidator.h>
#include <qcursor.h>
#include <qapplication.h>
#include <math.h>
#include <assert.h>

// to have std::max defined
#include <vector>

using namespace Gui;

FileChooser::FileChooser( QWidget *parent, const char *name )
  : QWidget( parent, name ), md( File ), _filter( QString::null )
{
  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setMargin( 0 );
  layout->setSpacing( 6 );

  lineEdit = new QLineEdit( this, "filechooser_lineedit" );
  layout->addWidget( lineEdit );

  connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SIGNAL( fileNameChanged( const QString & ) ) );

  button = new QPushButton( "...", this, "filechooser_button" );
  button->setFixedWidth( 2*button->fontMetrics().width( " ... " ) );
  layout->addWidget( button );

  connect( button, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );

  setFocusProxy( lineEdit );
}

FileChooser::~FileChooser()
{
}

QString FileChooser::fileName() const
{
  return lineEdit->text();
}

void FileChooser::setFileName( const QString &fn )
{
  lineEdit->setText( fn );
}

void FileChooser::chooseFile()
{
  QString fn;
  if ( mode() == File )
  	fn = QFileDialog::getOpenFileName( lineEdit->text(), QString::null, this );
  else
  	fn = QFileDialog::getExistingDirectory( lineEdit->text(),this );

  if ( !fn.isEmpty() ) 
  {
  	lineEdit->setText( fn );
	  emit fileNameSelected( fn );
  }
}

FileChooser::Mode FileChooser::mode() const
{
  return md;
}

void FileChooser::setMode( Mode m )
{
  md = m;
}

QString FileChooser::filter() const
{
  return _filter;
}

void FileChooser::setFilter ( const QString& filter )
{
  _filter = filter;
}

void FileChooser::setButtonText( const QString& txt )
{
  button->setText( txt );
  int w1 = 2*button->fontMetrics().width( txt );
  int w2 = 2*button->fontMetrics().width( " ... " );
  button->setFixedWidth( (w1 > w2 ? w1 : w2) );
}

QString FileChooser::buttonText() const
{
  return button->text();
}

// ------------------------------------------------------------------------------

AccelLineEdit::AccelLineEdit ( QWidget * parent, const char * name )
  : QLineEdit(parent, name)
{
  setText(tr("none"));
}

void AccelLineEdit::keyPressEvent ( QKeyEvent * e)
{
  QString txt;
  setText(tr("none"));

  int key = e->key();
  int state = e->state();

  if ( key == Key_Control )
    return;
  else if ( key == Key_Shift )
    return;
  else if ( key == Key_Alt )
    return;
  else if ( state == NoButton && key == Key_Backspace )
    return; // clears the edit field

  switch( state )
  {
  case ControlButton:
    txt += QAccel::keyToString(CTRL+key);
    setText(txt);
    break;
  case AltButton:
    txt += QAccel::keyToString(ALT+key);
    setText(txt);
    break;
  case ShiftButton:
    txt += QAccel::keyToString(SHIFT+key);
    setText(txt);
    break;
  case ControlButton+AltButton:
    txt += QAccel::keyToString(CTRL+ALT+key);
    setText(txt);
    break;
  case ControlButton+ShiftButton:
    txt += QAccel::keyToString(CTRL+SHIFT+key);
    setText(txt);
    break;
  case ShiftButton+AltButton:
    txt += QAccel::keyToString(SHIFT+ALT+key);
    setText(txt);
    break;
  case ControlButton+AltButton+ShiftButton:
    txt += QAccel::keyToString(CTRL+ALT+SHIFT+key);
    setText(txt);
    break;
  default:
    if ( e->stateAfter()&(ControlButton+AltButton+ShiftButton) )
      return; // if only the meta keys CTRL,ALT or SHIFT are pressed
    txt += QAccel::keyToString(key);
    setText(txt);
    break;
  }
}

// ------------------------------------------------------------------------------

CommandIconView::CommandIconView ( QWidget * parent, const char * name, WFlags f )
    : QIconView(parent, name, f)
{
  setResizeMode(Adjust);
  setItemsMovable(false);
  setWordWrapIconText(false);
  setGridX(50);
  setGridY(50);

  setSelectionMode(Extended);
  connect(this, SIGNAL ( currentChanged ( QIconViewItem * ) ), this, SLOT ( onSelectionChanged(QIconViewItem * ) ) );
}

CommandIconView::~CommandIconView ()
{
}

void CommandIconView::onSelectionChanged(QIconViewItem * item)
{
  emit emitSelectionChanged(item->text());
}

// ------------------------------------------------------------------------------

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

SpinBox::SpinBox ( QWidget* parent, const char* name )
  : QSpinBox (parent, name)
{
  setMouseTracking(true);
  d = new SpinBoxPrivate;
}

SpinBox::SpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name )
    : QSpinBox(minValue, maxValue, step, parent, name)
{
  setMouseTracking(true);
  d = new SpinBoxPrivate;
}

SpinBox::~SpinBox()
{
  delete d;
  d = 0L;
}

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

void SpinBox::mouseReleaseEvent ( QMouseEvent* )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
  d->pressed = false;
}

void SpinBox::focusOutEvent ( QFocusEvent* )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
}

void SpinBox::wheelEvent ( QWheelEvent* e )
{
  if (isEnabled())
    QSpinBox::wheelEvent(e);
}

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

FloatSpinBox::FloatSpinBox( QWidget * parent, const char * name )
  : SpinBox( parent, name )
{
  d = new FloatSpinBoxPrivate();
  updateValidator();
}

FloatSpinBox::FloatSpinBox( double minValue, double maxValue, double step,
        double value, uint precision, QWidget * parent, const char * name )
  : SpinBox( parent, name )
{
  d = new FloatSpinBoxPrivate();
  setRange( minValue, maxValue, step, precision );
  setValue( value );
}

FloatSpinBox::~FloatSpinBox() {
  delete d; d = 0;
}

void FloatSpinBox::setRange( double lower, double upper, double step, uint precision )
{
  lower = lower < upper ? lower : upper;
  upper = lower < upper ? upper : lower;
  setPrecision( precision, true ); // disable bounds checking, since
  setMinValue( lower );            // it's done in set{Min,Max}Value
  setMaxValue( upper );            // anyway and we want lower, upper
  setLineStep( step );             // and step to have the right precision
}

uint FloatSpinBox::precision() const 
{
  return d->mPrecision;
}

void FloatSpinBox::setPrecision( uint precision ) 
{
  setPrecision( precision, false );
}

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
  double maxAbsValue = std::max<double>( fabs(minValue()), fabs(maxValue()) );
  if ( maxAbsValue == 0 ) return 6; // return arbitrary value to avoid dbz...

  return int( floor( log10( double(INT_MAX) / maxAbsValue ) ) );
}

double FloatSpinBox::value() const 
{
  return d->mapToDouble( SpinBox::value() );
}

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

double FloatSpinBox::minValue() const 
{
  return d->mapToDouble( SpinBox::minValue() );
}

void FloatSpinBox::setMinValue( double value ) 
{
  bool ok = false;
  int min = d->mapToInt( value, &ok );
  if ( !ok ) return;
  SpinBox::setMinValue( min );
  updateValidator();
  d->mMinValue = value;
}

double FloatSpinBox::maxValue() const 
{
  return d->mapToDouble( SpinBox::maxValue() );
}

void FloatSpinBox::setMaxValue( double value ) 
{
  bool ok = false;
  int max = d->mapToInt( value, &ok );
  if ( !ok ) return;
  SpinBox::setMaxValue( max );
  updateValidator();
  d->mMaxValue = value;
}

double FloatSpinBox::lineStep() const 
{
  return d->mapToDouble( SpinBox::lineStep() );
}

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

QString FloatSpinBox::mapValueToText( int value ) 
{
  return QString().setNum( d->mapToDouble( value ), 'f', d->mPrecision );
}

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

void FloatSpinBox::valueChange()
{
  SpinBox::valueChange();
  emit valueChanged( d->mapToDouble( SpinBox::value() ) );
}

void FloatSpinBox::stepChange ()
{
  SpinBox::stepChange();
}

void FloatSpinBox::setValidator( const QValidator * ) 
{
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

// --------------------------------------------------------------------

PrefSpinBox::PrefSpinBox ( QWidget * parent, const char * name )
  : SpinBox(parent, name)
{
}

PrefSpinBox::~PrefSpinBox()
{
}

QCString PrefSpinBox::entryName () const
{
  return m_sPrefName;
}

QCString PrefSpinBox::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefSpinBox::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefSpinBox::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefFloatSpinBox::PrefFloatSpinBox ( QWidget * parent, const char * name )
  : FloatSpinBox(parent, name)
{
}

PrefFloatSpinBox::~PrefFloatSpinBox()
{
}

QCString PrefFloatSpinBox::entryName () const
{
  return m_sPrefName;
}

QCString PrefFloatSpinBox::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefFloatSpinBox::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefFloatSpinBox::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefLineEdit::PrefLineEdit ( QWidget * parent, const char * name )
  : QLineEdit(parent, name)
{
}

PrefLineEdit::~PrefLineEdit()
{
}

QCString PrefLineEdit::entryName () const
{
  return m_sPrefName;
}

QCString PrefLineEdit::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefLineEdit::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefLineEdit::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefFileChooser::PrefFileChooser ( QWidget * parent, const char * name )
  : FileChooser(parent, name)
{
}

PrefFileChooser::~PrefFileChooser()
{
}

QCString PrefFileChooser::entryName () const
{
  return m_sPrefName;
}

QCString PrefFileChooser::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefFileChooser::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefFileChooser::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefComboBox::PrefComboBox ( QWidget * parent, const char * name )
  : QComboBox(false, parent, name)
{
}

PrefComboBox::~PrefComboBox()
{
}

QCString PrefComboBox::entryName () const
{
  return m_sPrefName;
}

QCString PrefComboBox::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefComboBox::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefComboBox::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefListBox::PrefListBox ( QWidget * parent, const char * name, WFlags f )
  : QListBox(parent, name, f)
{
}

PrefListBox::~PrefListBox()
{
}

QCString PrefListBox::entryName () const
{
  return m_sPrefName;
}

QCString PrefListBox::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefListBox::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefListBox::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefCheckBox::PrefCheckBox ( QWidget * parent, const char * name )
  : QCheckBox(parent, name)
{
  setText( name );
}

PrefCheckBox::~PrefCheckBox()
{
}

QCString PrefCheckBox::entryName () const
{
  return m_sPrefName;
}

QCString PrefCheckBox::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefCheckBox::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefCheckBox::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefRadioButton::PrefRadioButton ( QWidget * parent, const char * name )
  : QRadioButton(parent, name)
{
  setText( name );
}

PrefRadioButton::~PrefRadioButton()
{
}

QCString PrefRadioButton::entryName () const
{
  return m_sPrefName;
}

QCString PrefRadioButton::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefRadioButton::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefRadioButton::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefButtonGroup::PrefButtonGroup ( QWidget * parent, const char * name )
  : QButtonGroup(parent, name)
{
  setTitle( name );
}

PrefButtonGroup::~PrefButtonGroup()
{
}

QCString PrefButtonGroup::entryName () const
{
  return m_sPrefName;
}

QCString PrefButtonGroup::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefButtonGroup::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefButtonGroup::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// --------------------------------------------------------------------

PrefSlider::PrefSlider ( QWidget * parent, const char * name )
  : QSlider(parent, name)
{
}

PrefSlider::~PrefSlider()
{
}

QCString PrefSlider::entryName () const
{
  return m_sPrefName;
}

QCString PrefSlider::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefSlider::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefSlider::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}

// ------------------------------------------------------------------------------

ColorButton::ColorButton(QWidget* parent, const char* name)
    : QPushButton( parent, name )
{
  connect( this, SIGNAL( clicked() ), SLOT( onChooseColor() ));
}

ColorButton::~ColorButton()
{
}

void ColorButton::setColor( const QColor& c )
{
  _col = c;
  update();
}

QColor ColorButton::color() const
{
  return _col;
}

void ColorButton::drawButtonLabel( QPainter *paint )
{
  QColor pen = isEnabled() ? hasFocus() ? palette().active().buttonText() : palette().inactive().buttonText()
                   : palette().disabled().buttonText();
  paint->setPen( pen );
  paint->setBrush( QBrush( _col ) );

  paint->drawRect( width()/4, height()/4, width()/2, height()/2 );
}

void ColorButton::onChooseColor()
{
  QColor c = QColorDialog::getColor( _col, this );
  if ( c.isValid() )
  {
    setColor( c );
    emit changed();
  }
}

// ------------------------------------------------------------------------------

PrefColorButton::PrefColorButton ( QWidget * parent, const char * name )
  : ColorButton(parent, name)
{
}

PrefColorButton::~PrefColorButton()
{
}

QCString PrefColorButton::entryName () const
{
  return m_sPrefName;
}

QCString PrefColorButton::paramGrpPath () const
{
  return m_sPrefGrp;
}

void PrefColorButton::setEntryName ( const QCString& name )
{
  m_sPrefName = name;
}

void PrefColorButton::setParamGrpPath ( const QCString& name )
{
  m_sPrefGrp = name;
}
