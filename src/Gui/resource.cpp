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

#include "BitmapFactory.h"
#include "WidgetFactory.h"
#include "Language/LanguageFactory.h"
#include "Widgets.h"

// ----------------------------------------------------

// INCLUDE YOUR LANGUAGE FILES HERE
//
#include "Language/FreeCAD_de.h"

/**
 * Register all languages to switch between them at any later time.
 */
Gui::LanguageFactorySupplier::LanguageFactorySupplier()
{
  // ADD ALL PIXMAPS TO THE FACTORY AUTOMATICALLY
  //
  //
  Gui::BitmapFactory();

  // ADD YOUR LANGUAGES HERE
  //
  //
  new Gui::LanguageProducer("German", GetFreeCAD_de());
}

// ----------------------------------------------------

// INCLUDE YOUR PREFERENCFE PAGES HERE
//
#include "DlgPreferencesImp.h"
#include "DlgSettingsImp.h"
#include "DlgSettings3DViewImp.h"
#include "DlgGeneralImp.h"
#include "DlgEditorImp.h"
#include "DlgSettingsMacroImp.h"
#include "DlgOnlineHelpImp.h"

using namespace Gui;
using namespace Gui::Dialog;

/**
 * Registers all preference pages or widgets to create them dynamically at any later time.
 */
WidgetFactorySupplier::WidgetFactorySupplier()
{
  // ADD YOUR PREFERENCFE PAGES HERE
  //
  //
  Gui::Dialog::DlgPreferencesImp::addGroup    ( QObject::tr("FreeCAD"     ) );
  new PrefPageProducer<DlgGeneralImp>         ( QObject::tr("General"     ) );
  new PrefPageProducer<DlgSettingsEditorImp>  ( QObject::tr("Editor"      ) );
  new PrefPageProducer<DlgSettingsMacroImp>   ( QObject::tr("Macros"      ) );
  new PrefPageProducer<DlgOnlineHelpImp>      ( QObject::tr("Online help" ) );
  Gui::Dialog::DlgPreferencesImp::addGroup    ( QObject::tr("Viewer"      ) );
  new PrefPageProducer<DlgSettingsHtmlViewImp>( QObject::tr("Help Viewer" ) );
  new PrefPageProducer<DlgSettings3DViewImp>  ( QObject::tr("3D View"     ) );

  // ADD YOUR PREFERENCE WIDGETS HERE
  //
  //
  new WidgetProducer<PrefSpinBox>;
  new WidgetProducer<PrefLineEdit>;
  new WidgetProducer<PrefComboBox>;
  new WidgetProducer<PrefListBox>;
  new WidgetProducer<PrefCheckBox>;
  new WidgetProducer<PrefRadioButton>;
  new WidgetProducer<PrefSlider>;
  new WidgetProducer<FCCmdView>;
  new WidgetProducer<FCAccelLineEdit>;
  new WidgetProducer<FCColorButton>;
}
