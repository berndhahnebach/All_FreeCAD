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
	  emit fileNameChanged( fn );
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

  if (e->ascii() == 0)
    return; // only meta key pressed

  int key = e->key();

  // I hope this works for every keyboard
  if ( key == Key_mu )
    key = Key_M;
  else if ( key == Key_At )
    key = Key_Q;

  // ignore these keys
  if ( key < Key_0 || key > Key_Z )
     return;

  switch(e->state())
  {
  case ControlButton:
    txt += QAccel::keyToString(CTRL+key);
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
  case ControlButton+AltButton+ShiftButton:
    txt += QAccel::keyToString(CTRL+ALT+SHIFT+key);
    setText(txt);
    break;
  case AltButton:
  case ShiftButton:
    break;
  default:// CTRL
    if ( key != Key_Backspace && key != Key_Delete)
    {
      txt += QAccel::keyToString(CTRL+key);
      setText(txt);
    }
    break;
  }
}

// ------------------------------------------------------------------------------

CommandView::CommandView ( QWidget * parent, const char * name, WFlags f )
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

CommandView::~CommandView ()
{
}

void CommandView::onSelectionChanged(QIconViewItem * item)
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

void SpinBox::mouseReleaseEvent ( QMouseEvent* e )
{
  if (QWidget::mouseGrabber() == this)
    releaseMouse();
  d->pressed = false;
}

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

FloatSpinBox::FloatSpinBox ( QWidget * parent, const char * name )
  : SpinBox(parent, name)
{
  d = new FloatSpinBoxPrivate;

  d->m_pValidator = new QDoubleValidator((double)minValue(), (double)maxValue(), 2, this, name );
  setValidator(d->m_pValidator);

  setDecimals(0);
}

FloatSpinBox::FloatSpinBox ( int minValue, int maxValue, int step, QWidget* parent, const char* name )
    : SpinBox(minValue, maxValue, step, parent, name)
{
  d = new FloatSpinBoxPrivate;

  d->m_pValidator = new QDoubleValidator((double)minValue, (double)maxValue, 2, this, name );
  setValidator(d->m_pValidator);

  setDecimals(0);
}

FloatSpinBox::~FloatSpinBox()
{
  delete d;
}

uint FloatSpinBox::decimals() const
{
  return d->m_pValidator->decimals();
}

void FloatSpinBox::setDecimals(uint i)
{
  d->m_pValidator->setDecimals(i);
  d->m_fDivisor = pow(10.0, double(i));
  d->m_fEpsilon = 1.0 / pow(10.0, double(i+1));
}

void FloatSpinBox::setMinValue(double value)
{
  double fMax = d->m_fDivisor * value;
  fMax = std::max<double>(fMax, (double)-INT_MAX);
  SpinBox::setMinValue(int(fMax));
}

void FloatSpinBox::setMaxValue(double value)
{
  double fMin = d->m_fDivisor * value;
  fMin = std::min<double>(fMin, (double)INT_MAX);
  SpinBox::setMaxValue(int(fMin));
}

double FloatSpinBox::minValue () const
{
  return SpinBox::minValue() / double(d->m_fDivisor);
}

double FloatSpinBox::maxValue () const
{
  return SpinBox::maxValue() / double(d->m_fDivisor);
}

QString FloatSpinBox::mapValueToText(int value)
{
  return QString::number(double(value) / d->m_fDivisor, 'f', d->m_pValidator->decimals());
}

int FloatSpinBox::mapTextToValue( bool* ok )
{
  double fEps = value() > 0.0 ? d->m_fEpsilon : - d->m_fEpsilon;
  return int(text().toDouble() * d->m_fDivisor + fEps);
}

void FloatSpinBox::valueChange()
{
  SpinBox::valueChange();
  emit valueFloatChanged( value() );
}

void FloatSpinBox::setValue(double value)
{
  double fEps = value > 0.0 ? d->m_fEpsilon : - d->m_fEpsilon;
  double fValue = d->m_fDivisor * value + fEps;
  fValue = std::min<double>(fValue, (double) INT_MAX);
  fValue = std::max<double>(fValue, (double)-INT_MAX);
  SpinBox::setValue(int(fValue));
}

double FloatSpinBox::value() const
{
  return SpinBox::value() / double(d->m_fDivisor);
}

void FloatSpinBox::stepChange ()
{
  SpinBox::stepChange();
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
  : QComboBox(parent, name)
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
  QColor c = QColorDialog::getColor( palette().active().background(), this );
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
