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

#include <Base/Console.h>

#include "PrefWidgets.h"
#include "FileDialog.h"

using Base::Console;
using namespace Gui;

/** Constructs a preference widget. 
 */
PrefWidget::PrefWidget()
 : WindowParameter(""), m_bKeepPref(false)
{
}

/**
 * Destroys the widget and detaches it from its parameter group.
 */
PrefWidget::~PrefWidget()
{
  if (getWindowParameter().IsValid())
    getWindowParameter()->Detach(this);
}

/** Sets the preference name to \a name. */
void PrefWidget::setEntryName( const QByteArray& name )
{
  m_sPrefName = name;
}

/** Returns the widget's preference name. */
QByteArray PrefWidget::entryName() const
{
  return m_sPrefName;
}

/** Sets the preference path to \a path. */
void PrefWidget::setParamGrpPath( const QByteArray& path )
{
#ifdef FC_DEBUG
  if (getWindowParameter().IsValid())
  {
    if ( paramGrpPath() != path )
      Base::Console().Warning("Widget already attached\n");
  }
#endif

  if ( paramGrpPath() != path )
  {
    if ( setGroupName( path ) )
    {
      m_sPrefGrp = path;
      assert(getWindowParameter().IsValid());
      getWindowParameter()->Attach(this);
    }
  }
}

/** Returns the widget's preferences path. */
QByteArray PrefWidget::paramGrpPath() const
{
  return m_sPrefGrp;
}

/** 
 * This method is called if one ore more values in the parameter settings are changed 
 * where getParamGrp() points to. 
 * Note: This method is called for each parameter inside the parameter group. So
 * you have to filter out the appropriate parameter with the name \a sReason.
 * \a rCaller calls this method.
 */
void PrefWidget::OnChange(Base::Subject<const char*> &rCaller, const char * sReason)
{
  if ( QString(sReason) == QString(m_sPrefName) )
    restorePreferences();
}

/**
 * Saves the current preferences of the widget.
 * All preference widget attached to the same parameter group are notified.
 */
void PrefWidget::onSave()
{
  savePreferences();
  if ( getWindowParameter().IsValid() )
    getWindowParameter()->Notify( entryName() );
#ifdef FC_DEBUG
  else
    qFatal( "No parameter group specified!" );
#endif
}

/**
 * Restores the preferences of the widget.
 */
void PrefWidget::onRestore()
{
#ifdef FC_DEBUG
  if ( getWindowParameter().IsNull() )
    qWarning( "No parameter group specified!" );
#endif
  restorePreferences();
}

/**
 * If \a b is true the preferences that are set to this widget will be kept, if \a b
 * is false the preferences are lost after calling the onRestore() method.
 */
void PrefWidget::setKeepPreference( bool b )
{
  m_bKeepPref = b;
}

/**
 * Returns true if the preferences are kept, false otherwise.
 */
bool PrefWidget::isKeepPreference() const
{
  return m_bKeepPref;
}

// --------------------------------------------------------------------

PrefSpinBox::PrefSpinBox ( QWidget * parent )
  : QSpinBox(parent), PrefWidget()
{
}

PrefSpinBox::~PrefSpinBox()
{
}

void PrefSpinBox::restorePreferences()
{
  if ( getWindowParameter().IsNull() )
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  int nVal = getWindowParameter()->GetInt( entryName(), QSpinBox::value() );
  setValue( nVal );
}

void PrefSpinBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetInt( entryName() , (int)value() );
}

QByteArray PrefSpinBox::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefSpinBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefSpinBox::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefSpinBox::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefDoubleSpinBox::PrefDoubleSpinBox ( QWidget * parent )
  : QDoubleSpinBox(parent), PrefWidget()
{
}

PrefDoubleSpinBox::~PrefDoubleSpinBox()
{
}

void PrefDoubleSpinBox::restorePreferences()
{
  if ( getWindowParameter().IsNull() )
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  double fVal = (double)getWindowParameter()->GetFloat( entryName() , value() );
  setValue(fVal);
}

void PrefDoubleSpinBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetFloat( entryName(), value() );
}

QByteArray PrefDoubleSpinBox::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefDoubleSpinBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefDoubleSpinBox::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefDoubleSpinBox::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefLineEdit::PrefLineEdit ( QWidget * parent )
  : QLineEdit(parent), PrefWidget()
{
}

PrefLineEdit::~PrefLineEdit()
{
}

void PrefLineEdit::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  std::string txt = text().toStdString();
  txt = getWindowParameter()->GetASCII( entryName(), txt.c_str() );
  setText(txt.c_str());
}

void PrefLineEdit::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetASCII( entryName(), text().toAscii() );
}

QByteArray PrefLineEdit::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefLineEdit::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefLineEdit::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefLineEdit::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefFileChooser::PrefFileChooser ( QWidget * parent )
  : FileChooser(parent), PrefWidget()
{
}

PrefFileChooser::~PrefFileChooser()
{
}

void PrefFileChooser::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  std::string txt = getWindowParameter()->GetASCII( entryName(), fileName().toAscii() );
  setFileName(txt.c_str());
}

void PrefFileChooser::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetASCII( entryName(), fileName().toAscii() );
}

QByteArray PrefFileChooser::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefFileChooser::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefFileChooser::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefFileChooser::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefComboBox::PrefComboBox ( QWidget * parent )
  : QComboBox(parent), PrefWidget()
{
}

PrefComboBox::~PrefComboBox()
{
}

void PrefComboBox::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  ParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup( entryName() );

  if ( !isKeepPreference() )
  {
    std::vector<std::string> items = hPGrp->GetASCIIs("Item");

    if (items.size() > 0)
      clear();

    for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
      addItem(it->c_str());

    int item = hPGrp->GetInt("currentItem", currentIndex());
    setCurrentIndex(item);
  }
  else
  {
    // due to i18n we must make sure that 'txt' is an item of the combobox
    // otherwise we'll override the text of the current item
    QString txt = hPGrp->GetASCII("currentText", currentText().toAscii() ).c_str();
    for ( int i = 0; i < count(); i++ )
    {
  	  if ( itemText( i ) == txt )
      {
	      setCurrentIndex( i );
  	    break;
      }
    }
  }
}

void PrefComboBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  ParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup( entryName() );
  hPGrp->Clear();

  if ( !isKeepPreference() )
  {
    int size = int(count());
    for (int i = 0; i < size; i++)
    {
      char szBuf[200];
      sprintf(szBuf, "Item%d", i);
      hPGrp->SetASCII(szBuf, itemText(i).toAscii());
    }

    hPGrp->SetInt("currentItem", currentIndex());
  }
  else
  {
    hPGrp->SetASCII("currentText", currentText().toAscii());
  }
}

QByteArray PrefComboBox::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefComboBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefComboBox::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefComboBox::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

//PrefListBox::PrefListBox ( QWidget * parent )
//  : Q3ListBox(parent), PrefWidget()
//{
//}
//
//PrefListBox::~PrefListBox()
//{
//}
//
//void PrefListBox::restorePreferences()
//{
//  if (getWindowParameter().IsNull())
//  {
//    Console().Warning("Cannot restore!\n");
//    return;
//  }
//
//  ParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup( entryName() );
//  std::vector<std::string> items = hPGrp->GetASCIIs("Item");
//
//  if (items.size() > 0)
//    clear();
//
//  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
//    insertItem(it->c_str());
//
//  int item = hPGrp->GetInt("currentItem", currentItem());
//  setCurrentItem(item);
//}
//
//void PrefListBox::savePreferences()
//{
//  if (getWindowParameter().IsNull())
//  {
//    Console().Warning("Cannot save!\n");
//    return;
//  }
//
//  ParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup( entryName() );
//  hPGrp->Clear();
//
//  int size = int(count());
//  for (int i = 0; i < size; i++)
//  {
//    char szBuf[200];
//    sprintf(szBuf, "Item%d", i);
//    hPGrp->SetASCII(szBuf, text(i).latin1());
//  }
//
//  hPGrp->SetInt("currentItem", currentItem());
//}
//
//QByteArray PrefListBox::entryName () const
//{
//  return PrefWidget::entryName();
//}
//
//QByteArray PrefListBox::paramGrpPath () const
//{
//  return PrefWidget::paramGrpPath();
//}
//
//void PrefListBox::setEntryName ( const QByteArray& name )
//{
//  PrefWidget::setEntryName(name);
//}
//
//void PrefListBox::setParamGrpPath ( const QByteArray& name )
//{
//  PrefWidget::setParamGrpPath(name);
//}

// --------------------------------------------------------------------

PrefCheckBox::PrefCheckBox ( QWidget * parent )
  : QCheckBox(parent), PrefWidget()
{
}

PrefCheckBox::~PrefCheckBox()
{
}

void PrefCheckBox::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  bool enable = getWindowParameter()->GetBool( entryName(), isChecked() );
  setChecked(enable);
}

void PrefCheckBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetBool( entryName(), isChecked() );
}

QByteArray PrefCheckBox::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefCheckBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefCheckBox::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefCheckBox::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefRadioButton::PrefRadioButton ( QWidget * parent )
  : QRadioButton(parent), PrefWidget()
{
}

PrefRadioButton::~PrefRadioButton()
{
}

void PrefRadioButton::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  bool enable = getWindowParameter()->GetBool( entryName(), isChecked() );
  setChecked(enable);
}

void PrefRadioButton::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetBool( entryName() , isChecked() );
}

QByteArray PrefRadioButton::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefRadioButton::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefRadioButton::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefRadioButton::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefSlider::PrefSlider ( QWidget * parent )
  : QSlider(parent), PrefWidget()
{
}

PrefSlider::~PrefSlider()
{
}

void PrefSlider::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  ParameterGrp::handle hPrefs = getWindowParameter()->GetGroup( entryName() );
  int o = hPrefs->GetInt("Orientation", orientation());
  setOrientation(Qt::Orientation(o));
  int min = hPrefs->GetInt("MinValue", minimum());
  int max = hPrefs->GetInt("MaxValue", maximum());
  int val = hPrefs->GetInt("Value", value());
  setMinimum(min);
  setMaximum(max);
  setValue(val);
}

void PrefSlider::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  ParameterGrp::handle hPrefs = getWindowParameter()->GetGroup( entryName() );
  hPrefs->SetInt("Orientation", int(orientation()));
  hPrefs->SetInt("MinValue", minimum());
  hPrefs->SetInt("MaxValue", maximum());
  hPrefs->SetInt("Value", value());
}

QByteArray PrefSlider::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefSlider::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefSlider::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefSlider::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefColorButton::PrefColorButton ( QWidget * parent )
  : ColorButton(parent), PrefWidget()
{
}

PrefColorButton::~PrefColorButton()
{
}

void PrefColorButton::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  QColor col = color();

  unsigned long lcol = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);

  lcol = getWindowParameter()->GetUnsigned( entryName(), lcol );
  int r = (lcol >> 24)&0xff;
  int g = (lcol >> 16)&0xff;
  int b = (lcol >>  8)&0xff;

  setColor(QColor(r,g,b));
}

void PrefColorButton::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  QColor col = color();
  // (r,g,b,a) with a = 255 (opaque)
  unsigned long lcol = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8) | 255;

  getWindowParameter()->SetUnsigned( entryName(), lcol );
}

QByteArray PrefColorButton::entryName () const
{
  return PrefWidget::entryName();
}

QByteArray PrefColorButton::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefColorButton::setEntryName ( const QByteArray& name )
{
  PrefWidget::setEntryName(name);
}

void PrefColorButton::setParamGrpPath ( const QByteArray& name )
{
  PrefWidget::setParamGrpPath(name);
}

#include "moc_PrefWidgets.cpp"
