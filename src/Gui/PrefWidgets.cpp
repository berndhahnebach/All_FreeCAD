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
#endif

#include "PrefWidgets.h"
#include "FileDialog.h"
#include "../Base/Console.h"

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
void PrefWidget::setEntryName( const QCString& name )
{
  m_sPrefName = name;
}

/** Returns the widget's preference name. */
QCString PrefWidget::entryName() const
{
  return m_sPrefName;
}

/** Sets the preference path to \a path. */
void PrefWidget::setParamGrpPath( const QCString& path )
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
QCString PrefWidget::paramGrpPath() const
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

PrefSpinBox::PrefSpinBox ( QWidget * parent, const char * name )
  : SpinBox(parent, name), PrefWidget()
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

QCString PrefSpinBox::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefSpinBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefSpinBox::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefSpinBox::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefFloatSpinBox::PrefFloatSpinBox ( QWidget * parent, const char * name )
  : FloatSpinBox(parent, name), PrefWidget()
{
}

PrefFloatSpinBox::~PrefFloatSpinBox()
{
}

void PrefFloatSpinBox::restorePreferences()
{
  if ( getWindowParameter().IsNull() )
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  double fVal = (double)getWindowParameter()->GetFloat( entryName() , value() );
  setValue(fVal);
}

void PrefFloatSpinBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetFloat( entryName(), value() );
}

QCString PrefFloatSpinBox::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefFloatSpinBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefFloatSpinBox::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefFloatSpinBox::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefLineEdit::PrefLineEdit ( QWidget * parent, const char * name )
  : QLineEdit(parent, name), PrefWidget()
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

  std::string txt = getWindowParameter()->GetASCII( entryName(), text().latin1() );
  setText(txt.c_str());
}

void PrefLineEdit::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetASCII( entryName(), text().latin1() );
}

QCString PrefLineEdit::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefLineEdit::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefLineEdit::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefLineEdit::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefFileChooser::PrefFileChooser ( QWidget * parent, const char * name )
  : FileChooser(parent, name), PrefWidget()
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

  std::string txt = getWindowParameter()->GetASCII( entryName(), fileName().latin1() );
  setFileName(txt.c_str());
}

void PrefFileChooser::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetASCII( entryName(), fileName().latin1() );
}

QCString PrefFileChooser::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefFileChooser::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefFileChooser::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefFileChooser::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefComboBox::PrefComboBox ( QWidget * parent, const char * name )
  : QComboBox(false, parent, name), PrefWidget()
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
      insertItem(it->c_str());

    int item = hPGrp->GetInt("currentItem", currentItem());
    setCurrentItem(item);
  }
  else
  {
    // due to i18n we must make sure that 'txt' is an item of the combobox
    // otherwise we'll override the text of the current item
    QString txt = hPGrp->GetASCII("currentText", currentText().latin1() ).c_str();
    for ( int i = 0; i < count(); i++ )
    {
  	  if ( text( i ) == txt )
      {
	      setCurrentItem( i );
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
      hPGrp->SetASCII(szBuf, text(i).latin1());
    }

    hPGrp->SetInt("currentItem", currentItem());
  }
  else
  {
    hPGrp->SetASCII("currentText", currentText().latin1());
  }
}

QCString PrefComboBox::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefComboBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefComboBox::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefComboBox::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefListBox::PrefListBox ( QWidget * parent, const char * name, WFlags f )
  : QListBox(parent, name, f), PrefWidget()
{
}

PrefListBox::~PrefListBox()
{
}

void PrefListBox::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  ParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup( entryName() );
  std::vector<std::string> items = hPGrp->GetASCIIs("Item");

  if (items.size() > 0)
    clear();

  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPGrp->GetInt("currentItem", currentItem());
  setCurrentItem(item);
}

void PrefListBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  ParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup( entryName() );
  hPGrp->Clear();

  int size = int(count());
  for (int i = 0; i < size; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Item%d", i);
    hPGrp->SetASCII(szBuf, text(i).latin1());
  }

  hPGrp->SetInt("currentItem", currentItem());
}

QCString PrefListBox::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefListBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefListBox::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefListBox::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefCheckBox::PrefCheckBox ( QWidget * parent, const char * name )
  : QCheckBox(parent, name), PrefWidget()
{
  setText( name );
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

QCString PrefCheckBox::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefCheckBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefCheckBox::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefCheckBox::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefRadioButton::PrefRadioButton ( QWidget * parent, const char * name )
  : QRadioButton(parent, name), PrefWidget()
{
  setText( name );
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

QCString PrefRadioButton::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefRadioButton::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefRadioButton::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefRadioButton::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefButtonGroup::PrefButtonGroup ( QWidget * parent, const char * name )
  : QButtonGroup(parent, name), PrefWidget()
{
  setTitle( name );
}

PrefButtonGroup::~PrefButtonGroup()
{
}

void PrefButtonGroup::restorePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot restore!\n");
    return;
  }

  int id = getWindowParameter()->GetInt( entryName(), selectedId() );
  setButton( id );
}

void PrefButtonGroup::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetInt ( entryName(), selectedId() );
}

QCString PrefButtonGroup::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefButtonGroup::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefButtonGroup::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefButtonGroup::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefSlider::PrefSlider ( QWidget * parent, const char * name )
  : QSlider(parent, name), PrefWidget()
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
  int min = hPrefs->GetInt("MinValue", minValue());
  int max = hPrefs->GetInt("MaxValue", maxValue());
  int val = hPrefs->GetInt("Value", value());
  setMinValue(min);
  setMaxValue(max);
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
  hPrefs->SetInt("MinValue", minValue());
  hPrefs->SetInt("MaxValue", maxValue());
  hPrefs->SetInt("Value", value());
}

QCString PrefSlider::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefSlider::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefSlider::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefSlider::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefColorButton::PrefColorButton ( QWidget * parent, const char * name )
  : ColorButton(parent, name), PrefWidget()
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

#ifndef OLD_COLOR_STYLE
  unsigned long lcol = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);

  lcol = getWindowParameter()->GetUnsigned( entryName(), lcol );
  int r = (lcol >> 24)&0xff;
  int g = (lcol >> 16)&0xff;
  int b = (lcol >>  8)&0xff;
#else
  long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();

  lcol = getWindowParameter()->GetInt( entryName(), lcol );
  int b = lcol >> 16;  lcol -= b << 16;
  int g = lcol >> 8;   lcol -= g << 8;
  int r = lcol;
#endif

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
#ifndef OLD_COLOR_STYLE
  // (r,g,b,a) with a = 255 (opaque)
  unsigned long lcol = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8) | 255;
#else
  long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();
#endif

  getWindowParameter()->SetUnsigned( entryName(), lcol );
}

QCString PrefColorButton::entryName () const
{
  return PrefWidget::entryName();
}

QCString PrefColorButton::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefColorButton::setEntryName ( const QCString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefColorButton::setParamGrpPath ( const QCString& name )
{
  PrefWidget::setParamGrpPath(name);
}

#include "moc_PrefWidgets.cpp"
