/***************************************************************************
                          WidgetFactory.h  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file WidgetFactory.h
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
 

#ifndef __FC_WIDGET_FACTORY_H__
#define __FC_WIDGET_FACTORY_H__

#include "../Base/Factory.h"
#include <typeinfo>

/** The widget factory supplier class
 */
class FCWidgetFactorySupplier
{
  private:
	  // Singleton
	  FCWidgetFactorySupplier();
  	static FCWidgetFactorySupplier *_pcSingleton;

  public:
  	static FCWidgetFactorySupplier &Instance(void);
	  friend FCWidgetFactorySupplier &GetWidgetFactorySupplier(void); 

  public:
    QWidget* GetWidget(const char* sClassName, QWidget* parent, const char* sPref);
};

inline FCWidgetFactorySupplier &GetWidgetFactorySupplier(void)
{
  return FCWidgetFactorySupplier::Instance();
}

#endif // __FC_WIDGET_FACTORY_H__
