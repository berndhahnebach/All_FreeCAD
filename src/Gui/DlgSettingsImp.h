/***************************************************************************
                          DlgSettingsImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief 
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
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




#ifndef DLG_SETTINGS_IMP_H
#define DLG_SETTINGS_IMP_H

#include "DlgSettings.h"
#include "Window.h"
#include "PropertyPage.h"

/**
 * This class implements the settings for the help-viewer
 * @see class FCHtmlView
 */
class FCDlgSettings : public FCDlgSettingsBase, public FCPreferencePage
{
  Q_OBJECT

  public:
    FCDlgSettings( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgSettings();
};

#endif // DLG_SETTINGS_IMP_H
