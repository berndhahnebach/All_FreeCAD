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



FCWidgetPrefs::FCWidgetPrefs(const char * name) : pHandler(NULL)
{
  if (name)
  {
    m_sPrefName = name;
    pHandler = new FCWidgetPrefsHandler(this);
    FCWidgetPrefsManager::Instance().Attach(m_sPrefName.latin1(), pHandler);
  }
  else
    m_sPrefName = getDefaultName();

  m_sPrefGrp = "Widget Preferences";
  setUseSystemParameter();
}

FCWidgetPrefs::~FCWidgetPrefs()
{
  FCWidgetPrefsManager::Instance().Detach(m_sPrefName.latin1(), pHandler);
}

void FCWidgetPrefs::setPrefName(QString pref)
{
  try
  {
    // you must choose another name
    if (pref == getDefaultName())
    {
      char szBuf[200];
      sprintf(szBuf, "You must choose another name for this item, not \"%s\"\n", pref.latin1());
      throw FCException(szBuf);
    }

    m_sPrefName = pref; 
    pHandler = new FCWidgetPrefsHandler(this);
    FCWidgetPrefsManager::Instance().Attach(m_sPrefName.latin1(), pHandler);
  }
  catch (const FCException&)
  {
    GetConsole().Error("\"%s\" is a reserved name, you must choose another name\n", pref.latin1());
#ifdef _DEBUG
    throw;
#endif
  }
}

QString FCWidgetPrefs::getPrefName()
{
  return m_sPrefName; 
}

QString FCWidgetPrefs::getDefaultName()
{
  return "Default";
}

void FCWidgetPrefs::setUseSystemParameter()
{
  hPrefGrp = GetApplication().GetSystemParameter().GetGroup(m_sPrefGrp.latin1());
}

void FCWidgetPrefs::setUseUserParameter()
{
  hPrefGrp = GetApplication().GetUserParameter().GetGroup(m_sPrefGrp.latin1());
}

FCWidgetPrefsHandler* FCWidgetPrefs::getHandler()
{
  return pHandler;
}

///////////////////////////////////////////////////////////////////////////////////

FCWidgetPrefsHandler::FCWidgetPrefsHandler(FCWidgetPrefs* p) : pPref(p)
{
}

void FCWidgetPrefsHandler::Save()
{
  pPref->savePreferences();
  emit saved(pPref->getPrefName().latin1());
}

void FCWidgetPrefsHandler::Restore()
{
  pPref->restorePreferences();
}

///////////////////////////////////////////////////////////////////////////////////

FCWidgetPrefsManager* FCWidgetPrefsManager::_pcSingleton = 0L;

FCWidgetPrefsManager::FCWidgetPrefsManager()
{
}

FCWidgetPrefsManager & FCWidgetPrefsManager::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCWidgetPrefsManager;
	}

  return *_pcSingleton;
}

void FCWidgetPrefsManager::slotSave(const FCstring& name)
{
  if (_sHandlers.find(name) != _sHandlers.end())
  {
    FCvector<FCWidgetPrefsHandler*> aHandlers = _sHandlers[name];
    for (FCvector<FCWidgetPrefsHandler*>::iterator it = aHandlers.begin(); it != aHandlers.end(); ++it)
      (*it)->Restore();
  }
}

void FCWidgetPrefsManager::Attach(const FCstring& name, FCWidgetPrefsHandler* w)
{
	_sHandlers[name].push_back(w);
  connect(w, SIGNAL(saved(const FCstring&)), SLOT(slotSave(const FCstring&)));
}

void FCWidgetPrefsManager::Detach(const FCstring& name, FCWidgetPrefsHandler* w)
{
  if (_sHandlers.find(name) != _sHandlers.end())
  {
    FCvector<FCWidgetPrefsHandler*>& aHandlers = _sHandlers[name];
    for (FCvector<FCWidgetPrefsHandler*>::iterator it = aHandlers.begin(); it != aHandlers.end(); ++it)
    {
      if (*it == w)
      {
        aHandlers.erase(it);
        break;
      }
    }
  }
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

  // restore the saved data
  restorePreferences();
}

FCEditSpinBox::~FCEditSpinBox() 
{
  delete m_pValidator; 
}

void FCEditSpinBox::restorePreferences()
{
  double fVal = hPrefGrp->GetFloat(getPrefName().latin1(), 0.0f);
  setValueFloat(fVal);
}

void FCEditSpinBox::savePreferences()
{
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

///////////////////////////////////////////////////////////////////////////////////

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

TestDialog::TestDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MyDialog" );
    resize( 188, 76 ); 
    setProperty( "caption", tr( "MyDialog" ) );
    setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
    MyDialogLayout = new QGridLayout( this ); 
    MyDialogLayout->setSpacing( 6 );
    MyDialogLayout->setMargin( 11 );

    SpinBox1 = new FCEditSpinBox( this, "Test" );
//    SpinBox1 = (FCEditSpinBox*)GetWidgetFactorySupplier().GetWidget(typeid(FCEditSpinBox).name(), this, "Test");

    MyDialogLayout->addMultiCellWidget( SpinBox1, 0, 0, 0, 1 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setProperty( "text", tr( "&OK" ) );
    buttonOk->setProperty( "autoDefault", QVariant( TRUE, 0 ) );
    buttonOk->setProperty( "default", QVariant( TRUE, 0 ) );

    MyDialogLayout->addWidget( buttonOk, 1, 0 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setProperty( "text", tr( "&Cancel" ) );
    buttonCancel->setProperty( "autoDefault", QVariant( TRUE, 0 ) );

    MyDialogLayout->addWidget( buttonCancel, 1, 1 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    connect( buttonOk, SIGNAL( clicked() ), SpinBox1->getHandler(), SLOT( Save() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TestDialog::~TestDialog()
{
    // no need to delete child widgets, Qt does it all for us
}


#include "moc_PrefWidgets.cpp"
