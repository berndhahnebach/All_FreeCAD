/***************************************************************************
                          PrefWidgets.cpp  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file PrefWidgets.cpp
 *  \brief A set of FreeCAD widgets
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/



#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qstring.h>
#	include <qurl.h>
#	include <ctype.h>
#endif

#include "PrefWidgets.h"
#include "WidgetFactory.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

#include <cmath>
#include <qvalidator.h>



FCWidgetPrefs::FCWidgetPrefs(const char * name, bool bAttach) : pHandler(NULL)
{
  m_sPrefGrp = "Widget Preferences";
  m_sPrefName = "_____default_____";

  if (name)
    m_sPrefName = name;

  setUseUserParameter();
  if (hPrefGrp.IsValid() && bAttach)
  {
    hPrefGrp->Attach(this);
  }

  // install a handler for automation stuff
  pHandler = new FCWidgetPrefsHandler(this);
}

FCWidgetPrefs::~FCWidgetPrefs()
{
  hPrefGrp->Detach(this);
#ifdef FC_DEBUG
  if (m_sPrefName == "_____default_____")
  {
    GetConsole().Warning("Wrong widget name!. It must not be \"_____default_____\"\n");
    throw;
  }
#endif
}

void FCWidgetPrefs::setPrefName(QString pref)
{
  hPrefGrp->Detach(this);
  m_sPrefName = pref; 
  setUseUserParameter();
  hPrefGrp->Attach(this);
}

QString FCWidgetPrefs::getPrefName()
{
  return m_sPrefName; 
}

void FCWidgetPrefs::setUseSystemParameter()
{
  hPrefGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("Windows")->GetGroup(m_sPrefGrp.latin1());
  hPrefGrp = hPrefGrp->GetGroup(m_sPrefName.latin1());
}

void FCWidgetPrefs::setUseUserParameter()
{
  hPrefGrp = GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Windows")->GetGroup(m_sPrefGrp.latin1());
  hPrefGrp = hPrefGrp->GetGroup(m_sPrefName.latin1());
}

FCWidgetPrefsHandler* FCWidgetPrefs::getHandler()
{
  return pHandler;
}

void FCWidgetPrefs::installHandler(FCWidgetPrefsHandler* h)
{
  delete pHandler;
  pHandler = h;
}

FCParameterGrp::handle FCWidgetPrefs::getParamGrp()
{
  return hPrefGrp;
}

void FCWidgetPrefs::OnChange(FCSubject &rCaller)
{
  restorePreferences();
}

///////////////////////////////////////////////////////////////////////////////////

FCWidgetPrefsHandler::FCWidgetPrefsHandler(FCWidgetPrefs* p) : pPref(p)
{
}

void FCWidgetPrefsHandler::save()
{
  pPref->savePreferences();
  pPref->getParamGrp()->Notify();
}

void FCWidgetPrefsHandler::restore()
{
  pPref->restorePreferences();
}

///////////////////////////////////////////////////////////////////////////////////

FCEditSpinBox::FCEditSpinBox ( QWidget * parent, const char * name )
: QSpinBox(parent, name), FCWidgetPrefs(name), m_pValidator(NULL), m_iAccuracy(2), m_fDivisor(0), m_fEpsilon(0)
{
  double fMinValue = -10.0f;
  double fMaxValue =  10.0f;
  double fStep     =  0.1f;

  m_pValidator = new QDoubleValidator(fMinValue, fMaxValue, m_iAccuracy+1, this, QString(name) + "FCEditSpinBoxValidator" );
  setValidator(m_pValidator);
  
  m_fDivisor = int(pow(10.0, double(m_iAccuracy)));
  m_fEpsilon = 1.0 / pow(10.0, double(m_iAccuracy+1));
  QSpinBox::setMaxValue(int(fMaxValue * m_fDivisor));
  QSpinBox::setMinValue(int(fMinValue * m_fDivisor));
  QSpinBox::setLineStep(int(fStep     * m_fDivisor));
}

FCEditSpinBox::~FCEditSpinBox() 
{
  delete m_pValidator; 
}

void FCEditSpinBox::restorePreferences()
{
  double fVal;
  if (m_iAccuracy == 0)
    fVal = (double)hPrefGrp->GetInt(getPrefName().latin1(), 0);
  else
    fVal = (double)hPrefGrp->GetFloat(getPrefName().latin1(), 0.0f);

  setValueFloat(fVal);
}

void FCEditSpinBox::savePreferences()
{
  if (m_iAccuracy == 0)
    hPrefGrp->SetInt(getPrefName().latin1(), (int)getValueFloat());
  else
    hPrefGrp->SetFloat(getPrefName().latin1(), getValueFloat());
}

int FCEditSpinBox::getAccuracy() const
{
  return m_iAccuracy;
}

void FCEditSpinBox::setAccuracy(int i)
{
  m_iAccuracy = i;
  m_fDivisor = int(pow(10.0, double(m_iAccuracy)));
  m_fEpsilon = 1.0 / pow(10.0, double(m_iAccuracy+1));
}

double FCEditSpinBox::getValueFloat() const
{ 
  return QSpinBox::value() / double(m_fDivisor); 
}

void FCEditSpinBox::setMinValueFloat(double value)
{ 
  QSpinBox::setMinValue(int(m_fDivisor * value)); 
}

void FCEditSpinBox::setMaxValueFloat(double value)
{ 
  QSpinBox::setMaxValue(int(m_fDivisor * value)); 
}

double FCEditSpinBox::getMinValueFloat () const
{
  return QSpinBox::minValue() / double(m_fDivisor); 
}

double FCEditSpinBox::getMaxValueFloat () const
{
  return QSpinBox::maxValue() / double(m_fDivisor); 
}

QString FCEditSpinBox::mapValueToText(int value)
{ 
  return QString::number(double(value) / m_fDivisor, 'f', m_iAccuracy); 
}

int FCEditSpinBox::mapTextToValue(bool*)
{ 
  double fEps = value() > 0.0 ? m_fEpsilon : -m_fEpsilon;
  return int(text().toDouble() * m_fDivisor + fEps); 
}

void FCEditSpinBox::valueChange()
{
  QSpinBox::valueChange();
  emit valueChanged( QSpinBox::value() / double(m_fDivisor) );
}

void FCEditSpinBox::setValueFloat(double value)
{ 
  double fEps = value > 0.0 ? m_fEpsilon : -m_fEpsilon;
  QSpinBox::setValue(int(m_fDivisor * value + fEps)); 
}

void FCEditSpinBox::stepChange () 
{
  QSpinBox::stepChange();
}

///////////////////////////////////////////////////////////////////////////////////

FCLineEdit::FCLineEdit ( QWidget * parent, const char * name )
: QLineEdit(parent, name), FCWidgetPrefs(name)
{
}

FCLineEdit::~FCLineEdit()
{
}

void FCLineEdit::restorePreferences()
{
  std::string text = hPrefGrp->GetASCII(getPrefName().latin1(), "");
  setText(text.c_str());
}

void FCLineEdit::savePreferences()
{
  hPrefGrp->SetASCII(getPrefName().latin1(), text().latin1());
}

///////////////////////////////////////////////////////////////////////////////////

FCComboBox::FCComboBox ( QWidget * parent, const char * name )
: QComboBox(parent, name), FCWidgetPrefs(name)
{
}

FCComboBox::~FCComboBox()
{
}

void FCComboBox::restorePreferences()
{
  clear();
  std::vector<std::string> items = hPrefGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPrefGrp->GetInt(getPrefName().latin1(), 0);
  setCurrentItem(item);
}

void FCComboBox::savePreferences()
{
  for (int i = 0; i < count(); i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, text(i).latin1());
  }

  hPrefGrp->SetInt(getPrefName().latin1(), currentItem());
}

///////////////////////////////////////////////////////////////////////////////////

FCListBox::FCListBox ( QWidget * parent, const char * name, WFlags f )
: QListBox(parent, name, f), FCWidgetPrefs(name)
{
}

FCListBox::~FCListBox()
{
}

void FCListBox::restorePreferences()
{
  clear();
  std::vector<std::string> items = hPrefGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPrefGrp->GetInt(getPrefName().latin1(), 0);
  setCurrentItem(item);
}

void FCListBox::savePreferences()
{
  int size = int(count());
  for (int i = 0; i < size; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, item(i)->text().latin1());
  }

  hPrefGrp->SetInt(getPrefName().latin1(), currentItem());
}

///////////////////////////////////////////////////////////////////////////////////

FCCheckBox::FCCheckBox ( QWidget * parent, const char * name )
: QCheckBox(parent, name), FCWidgetPrefs(name)
{
}

FCCheckBox::~FCCheckBox()
{
}

void FCCheckBox::restorePreferences()
{
  bool enable = hPrefGrp->GetBool(getPrefName().latin1(), false);
  setChecked(enable);
}

void FCCheckBox::savePreferences()
{
  hPrefGrp->SetBool(getPrefName().latin1(), isChecked());
}

///////////////////////////////////////////////////////////////////////////////////

FCRadioButton::FCRadioButton ( QWidget * parent, const char * name )
: QRadioButton(parent, name), FCWidgetPrefs(name)
{
}

FCRadioButton::~FCRadioButton()
{
}

void FCRadioButton::restorePreferences()
{
  bool enable = hPrefGrp->GetBool(getPrefName().latin1(), false);
  setChecked(enable);
}

void FCRadioButton::savePreferences()
{
  hPrefGrp->SetBool(getPrefName().latin1(), isChecked());
}

#include "moc_PrefWidgets.cpp"
