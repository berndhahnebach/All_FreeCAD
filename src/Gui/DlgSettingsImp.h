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
#include "PropertyPage.h"

namespace Gui {
namespace Dialog {

/**
 * This class implements the settings for the help-viewer
 * @see class HtmlView
 */
class DlgSettingsHtmlViewImp : public DlgSettingsBase, public Gui::Dialog::PreferencePage
{
Q_OBJECT

public:
	DlgSettingsHtmlViewImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~DlgSettingsHtmlViewImp();
};

} // namespace Dialog
} // namespace Gui

#endif // DLG_SETTINGS_IMP_H
