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
#include "Application.h"
#include "../Base/Console.h"

using Base::Console;
using namespace Gui;

/** Constructs a preference widget with the name \a name. If \a bInstall
 * is true a PrefWidgetHandler is installed. By default \a bInstall is set 
 * to true.
 */
PrefWidget::PrefWidget( bool bInstall )
 : WindowParameter(""), pHandler(0L)
{
  if (bInstall)
  {
    // install a handler for automation stuff
    pHandler = new PrefWidgetHandler(this);
  }
}

/**
 * Destroys the widget and detaches it from its parameter group.
 */
PrefWidget::~PrefWidget()
{
  if (getWindowParameter().IsValid())
    getWindowParameter()->Detach(this);
  delete pHandler;
}

/** Sets the preference name to \a name. */
void PrefWidget::setEntryName( const QString& name )
{
  m_sPrefName = name;
}

/** Returns the widget's preference name. */
QString PrefWidget::entryName() const
{
  return m_sPrefName;
}

/** Sets the preference path to \a path. */
void PrefWidget::setParamGrpPath( const QString& path )
{
#ifdef FC_DEBUG
  if (getWindowParameter().IsValid())
  {
    Base::Console().Warning("Widget already attached\n");
  }
#endif

  if ( setGroupName( path.latin1() ) )
  {
    m_sPrefGrp = path;
    assert(getWindowParameter().IsValid());
    getWindowParameter()->Attach(this);
  }
}

/** Returns the widget's preferences path. */
QString PrefWidget::paramGrpPath() const
{
  return m_sPrefGrp;
}

/** Returns the last installed handler. */
PrefWidgetHandler* PrefWidget::getHandler()
{
  return pHandler;
}

/** Installs a new handler \a h. */
void PrefWidget::installHandler(PrefWidgetHandler* h)
{
  delete pHandler;
  pHandler = h;
}

/** 
 * This method is called if one ore more values in the parameter settings are changed 
 * where getParamGrp() points to. 
 * Note: This method is called for each parameter inside the parameter group. So
 * you have to filter out the appropriate parameter with the name \a sReason.
 * \a rCaller calls this method.
 */
void PrefWidget::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  if (QString(sReason) == m_sPrefName)
    restorePreferences();
}

// --------------------------------------------------------------------

PrefWidgetHandler::PrefWidgetHandler( PrefWidget* p ) : pPref(p)
{
}

/**
 * Saves the current preferences of the managed preference widget and emits 
 * the saved() signal..
 * All preference widget attached to the same parameter group are notified.
 */
void PrefWidgetHandler::onSave()
{
  pPref->savePreferences();
  if ( pPref->getWindowParameter().IsValid() )
    pPref->getWindowParameter()->Notify(pPref->entryName().latin1());

  emit saved();
}

/**
 * Restores the preferences of the managed widget and emits the restored() signal.
 */
void PrefWidgetHandler::onRestore()
{
  pPref->restorePreferences();
  emit restored();
}

// --------------------------------------------------------------------

PrefSpinBox::PrefSpinBox ( QWidget * parent, const char * name )
  : FloatSpinBox(parent, name), PrefWidget(name)
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

  double fVal;
  if (decimals() == 0)
    fVal = (double)getWindowParameter()->GetInt(entryName().latin1(), value());
  else
    fVal = (double)getWindowParameter()->GetFloat(entryName().latin1(), valueFloat());

  setValueFloat(fVal);
}

void PrefSpinBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  if (decimals() == 0)
    getWindowParameter()->SetInt(entryName().latin1(), (int)valueFloat());
  else
    getWindowParameter()->SetFloat(entryName().latin1(), valueFloat());
}

QString PrefSpinBox::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefSpinBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefSpinBox::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefSpinBox::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefLineEdit::PrefLineEdit ( QWidget * parent, const char * name )
  : QLineEdit(parent, name), PrefWidget(name)
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

  std::string txt = getWindowParameter()->GetASCII(entryName().latin1(), text().latin1());
  setText(txt.c_str());
}

void PrefLineEdit::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetASCII(entryName().latin1(), text().latin1());
}

QString PrefLineEdit::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefLineEdit::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefLineEdit::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefLineEdit::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefComboBox::PrefComboBox ( QWidget * parent, const char * name )
  : QComboBox(parent, name), PrefWidget(name)
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

  FCParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup(entryName().latin1());
  std::vector<std::string> items = hPGrp->GetASCIIs("Item");

  if (items.size() > 0)
    clear();

  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPGrp->GetInt("currentItem", currentItem());
  setCurrentItem(item);
}

void PrefComboBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  FCParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup(entryName().latin1());
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

QString PrefComboBox::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefComboBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefComboBox::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefComboBox::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefListBox::PrefListBox ( QWidget * parent, const char * name, WFlags f )
  : QListBox(parent, name, f), PrefWidget(name)
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

  FCParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup(entryName().latin1());
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

  FCParameterGrp::handle  hPGrp = getWindowParameter()->GetGroup(entryName().latin1());
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

QString PrefListBox::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefListBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefListBox::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefListBox::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefCheckBox::PrefCheckBox ( QWidget * parent, const char * name )
  : QCheckBox(parent, name), PrefWidget(name)
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

  bool enable = getWindowParameter()->GetBool(entryName().latin1(), isChecked());
  setChecked(enable);
}

void PrefCheckBox::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetBool(entryName().latin1(), isChecked());
}

QString PrefCheckBox::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefCheckBox::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefCheckBox::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefCheckBox::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefRadioButton::PrefRadioButton ( QWidget * parent, const char * name )
  : QRadioButton(parent, name), PrefWidget(name)
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

  bool enable = getWindowParameter()->GetBool(entryName().latin1(), isChecked());
  setChecked(enable);
}

void PrefRadioButton::savePreferences()
{
  if (getWindowParameter().IsNull())
  {
    Console().Warning("Cannot save!\n");
    return;
  }

  getWindowParameter()->SetBool(entryName().latin1(), isChecked());
}

QString PrefRadioButton::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefRadioButton::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefRadioButton::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefRadioButton::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefSlider::PrefSlider ( QWidget * parent, const char * name )
  : QSlider(parent, name), PrefWidget(name)
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

  FCParameterGrp::handle hPrefs = getWindowParameter()->GetGroup(entryName().latin1());
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

  FCParameterGrp::handle hPrefs = getWindowParameter()->GetGroup(entryName().latin1());
  hPrefs->SetInt("Orientation", int(orientation()));
  hPrefs->SetInt("MinValue", minValue());
  hPrefs->SetInt("MaxValue", maxValue());
  hPrefs->SetInt("Value", value());
}

QString PrefSlider::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefSlider::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefSlider::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefSlider::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

// --------------------------------------------------------------------

PrefColorButton::PrefColorButton ( QWidget * parent, const char * name )
  : ColorButton(parent, name), PrefWidget(name)
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
  long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();

  lcol = getWindowParameter()->GetInt( entryName().latin1(), lcol );
  int b = lcol >> 16;  lcol -= b << 16;
  int g = lcol >> 8;   lcol -= g << 8;
  int r = lcol;

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
  long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();

  getWindowParameter()->SetInt( entryName().latin1(), lcol );
}

QString PrefColorButton::entryName () const
{
  return PrefWidget::entryName();
}

QString PrefColorButton::paramGrpPath () const
{
  return PrefWidget::paramGrpPath();
}

void PrefColorButton::setEntryName ( const QString& name )
{
  PrefWidget::setEntryName(name);
}

void PrefColorButton::setParamGrpPath ( const QString& name )
{
  PrefWidget::setParamGrpPath(name);
}

#include "moc_PrefWidgets.cpp"
