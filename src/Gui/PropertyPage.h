/***************************************************************************
                          PropertyPage.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file PropertyPage.h
 *  \brief The property page base class
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



#ifndef __PROPERTY_PAGE__H__
#define __PROPERTY_PAGE__H__

#include "PrefWidgets.h"

/** Base class for property pages.
 * The pages of @ref FCDlgCustomize and @ref FCDlgPreferencesImp
 * should inherit FCPropertyPage to use apply/cancel mechanism.
 * Inherited classes should reimplement @ref apply() and
 * @ref cancel().
 */
class FCPropertyPage
{
  protected:
    FCPropertyPage();

  public:
    virtual ~FCPropertyPage() {}
    bool isModified();
    void setModified(bool b);
    void onApply();
    void onCancel();

  protected:
    virtual void apply();
    virtual void cancel();

  private:
    bool bChanged;
};

class FCPreferencePage : public FCPropertyPage, public FCWidgetPrefsManager
{
  public:
    FCPreferencePage();
    virtual ~FCPreferencePage();
};

#endif // __PROPERTY_PAGE__H__
