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



#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#endif

#include "WidgetFactory.h"
#include "PrefWidgets.h"
#include "../Base/Console.h"


FCWidgetFactorySupplier* FCWidgetFactorySupplier::_pcSingleton = 0L;

FCWidgetFactorySupplier::FCWidgetFactorySupplier()
{
  new FCFactoryProducer<FCEditSpinBox>;
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

QWidget* FCWidgetFactorySupplier::GetWidget(const char* sClassName, QWidget* parent, const char* sPref)
{
  void* p = GetFactory().Produce(sClassName);
  // this widget class is not registered
  if (!p)
  {
#ifdef FC_DEBUG
    GetConsole().Warning("\"%s\" is not registered\n", sClassName);
#endif
    return NULL;
  }

  // set the parent to the widget 
  QWidget* w = (QWidget*)p;
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
  }

  return w;
}
