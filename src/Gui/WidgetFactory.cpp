/***************************************************************************
                          WidgetFactory.cpp  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file WidgetFactory.cpp
 *  \brief Produce widgets
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



#include "PreCompiled.h"

#include "WidgetFactory.h"
#include "PrefWidgets.h"
#include "../Base/Console.h"
#include "DlgPreferencesImp.h"
#include "DlgSettingsImp.h"
#include "DlgSettings3DViewImp.h"
#include "DlgGeneralImp.h"
#include "DlgEditorImp.h"
#include "DlgSettingsMacroImp.h"
#include "DlgOnlineHelpImp.h"


FCWidgetFactory* FCWidgetFactory::_pcSingleton = NULL;

FCWidgetFactory& FCWidgetFactory::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FCWidgetFactory;
  return *_pcSingleton;
}

void FCWidgetFactory::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

QWidget* FCWidgetFactory::ProduceWidget (const char* sName) const
{
	QWidget* w = (QWidget*)Produce(sName);

  // this widget class is not registered
  if (!w)
  {
#ifdef FC_DEBUG
    GetConsole().Warning("\"%s\" is not registered\n", sName);
#endif
    return NULL;
  }

  try
  {
    dynamic_cast<QWidget*>(w);
  }
  catch (...)
  {
#ifdef FC_DEBUG
    GetConsole().Error("%s does not inherit from \"QWidget\"\n", sName);
#endif
		delete w;
		return NULL;
  }

  return w;
}


QWidget* FCWidgetFactory::ProducePrefWidget(const char* sName, QWidget* parent, const char* sPref)
{
  QWidget* w = ProduceWidget(sName);
  // this widget class is not registered
  if (!w)
    return NULL; // no valid QWidget object

  // set the parent to the widget 
  w->reparent(parent, QPoint(0,0));

  try
  {
    dynamic_cast<FCWidgetPrefs*>(w)->setPrefName(sPref);
    dynamic_cast<FCWidgetPrefs*>(w)->restorePreferences();
  }
  catch (...)
  {
#ifdef FC_DEBUG
    GetConsole().Error("%s does not inherit from \"FCWidgetPrefs\"\n", w->className());
#endif
		delete w;
		return NULL;
  }

  return w;
}

// ----------------------------------------------------

template <class CLASS>
FCWidgetProducer<CLASS>::FCWidgetProducer (const QString& caption) : mCaption(caption)
{
	FCWidgetFactory::Instance().AddProducer(/*typeid(CLASS).name()*/mCaption.latin1(), this);
	FCDlgPreferencesImp::addPage(caption);
}

// ----------------------------------------------------

template <class CLASS>
FCPrefWidgetProducer<CLASS>::FCPrefWidgetProducer ()
{
	FCWidgetFactory::Instance().AddProducer(typeid(CLASS).name(), this);
}

// ----------------------------------------------------

FCWidgetFactorySupplier* FCWidgetFactorySupplier::_pcSingleton = 0L;

FCWidgetFactorySupplier::FCWidgetFactorySupplier()
{
  // ADD YOUR PREFERENCFE PAGES HERE
  //
  //
	FCDlgPreferencesImp::addGroup("FreeCAD");
  new FCWidgetProducer<FCDlgGeneral>       ("General"     );
  new FCWidgetProducer<FCDlgEditorSettings>("Editor"      );
  new FCWidgetProducer<FCDlgSettingsMacro> ("Macros"      );
  new FCWidgetProducer<FCOnlineHelp>       ("Online help" );
	FCDlgPreferencesImp::addGroup("Viewer");
  new FCWidgetProducer<FCDlgSettings>      ("Help Viewer" );
  new FCWidgetProducer<FCDlgSettings3DView>("3D View"     );

	// ADD YOUR PREFERENCE WIDGETS HERE
	//
	//
	new FCPrefWidgetProducer<FCPrefSpinBox>;
	new FCPrefWidgetProducer<FCLineEdit>;
	new FCPrefWidgetProducer<FCComboBox>;
	new FCPrefWidgetProducer<FCListBox>;
	new FCPrefWidgetProducer<FCCheckBox>;
	new FCPrefWidgetProducer<FCRadioButton>;
	new FCPrefWidgetProducer<FCSlider>;
}

FCWidgetFactorySupplier & FCWidgetFactorySupplier::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCWidgetFactorySupplier;
	}

  return *_pcSingleton;
}
