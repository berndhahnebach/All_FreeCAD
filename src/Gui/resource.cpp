/***************************************************************************
                          resource.cpp  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file resource.cpp
 *  \brief Include the resources stuff here
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
#include "BitmapFactory.h"
#include "WidgetFactory.h"
#include "Language/LanguageFactory.h"
#include "Widgets.h"

// ----------------------------------------------------

// INCLUDE YOUR LANGUAGE FILES HERE
//
#include "Language/FreeCAD_de.h"

FCLanguageFactorySupplier::FCLanguageFactorySupplier()
{
  // ADD ALL PIXMAPS TO THE FACTORY AUTOMATICALLY
  //
  //
	GetBitmapFactory();

  // ADD YOUR LANGUAGES HERE
  //
  //
	new FCLanguageProducer("German", GetFreeCAD_de());
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

FCWidgetFactorySupplier::FCWidgetFactorySupplier()
{
  // ADD YOUR PREFERENCFE PAGES HERE
  //
  //
	FCDlgPreferencesImp::addGroup( QObject::tr("FreeCAD") );
  new FCPrefPageProducer<FCDlgGeneral>       ( QObject::tr("General"     ) );
  new FCPrefPageProducer<FCDlgEditorSettings>( QObject::tr("Editor"      ) );
  new FCPrefPageProducer<FCDlgSettingsMacro> ( QObject::tr("Macros"      ) );
  new FCPrefPageProducer<FCOnlineHelp>       ( QObject::tr("Online help" ) );
	FCDlgPreferencesImp::addGroup( QObject::tr("Viewer") );
  new FCPrefPageProducer<FCDlgSettings>      ( QObject::tr("Help Viewer" ) );
  new FCPrefPageProducer<FCDlgSettings3DView>( QObject::tr("3D View"     ) );

	// ADD YOUR PREFERENCE WIDGETS HERE
	//
	//
	new FCWidgetProducer<FCPrefSpinBox>;
	new FCWidgetProducer<FCLineEdit>;
	new FCWidgetProducer<FCComboBox>;
	new FCWidgetProducer<FCListBox>;
	new FCWidgetProducer<FCCheckBox>;
	new FCWidgetProducer<FCRadioButton>;
	new FCWidgetProducer<FCSlider>;
	new FCWidgetProducer<FCCmdView>;
	new FCWidgetProducer<FCAccelLineEdit>;
	new FCWidgetProducer<FCColorButton>;
}
