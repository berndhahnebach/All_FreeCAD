/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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

#include "DlgSettings3DViewImp.h"
#include "PrefWidgets.h"
#include <Base/Console.h>

using namespace Gui::Dialog;

/**
 *  Constructs a DlgSettings3DViewImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettings3DViewImp::DlgSettings3DViewImp(QWidget* parent)
    : PreferencePage( parent )
{
    this->setupUi(this);
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettings3DViewImp::~DlgSettings3DViewImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgSettings3DViewImp::saveSettings()
{
    prefCheckBox8->onSave();
    CheckBox_CornerCoordSystem->onSave();
    CheckBox_ShowFPS->onSave();
    CheckBox_UseAutoRotation->onSave();
    FloatSpinBox_EyeDistance->onSave();
    SelectionColor_Background->onSave();
    backgroundColorFrom->onSave();
    backgroundColorTo->onSave();
    backgroundColorMid->onSave();
    radioButtonSimple->onSave();
    radioButtonGradient->onSave();
    checkMidColor->onSave();
    radioPerspective->onSave();
    radioOrthographic->onSave();
    checkBoxPreselection->onSave();
    checkBoxSelection->onSave();
    HighlightColor->onSave();
    SelectionColor->onSave();
    comboNavigationStyle->onSave();
}

void DlgSettings3DViewImp::loadSettings()
{
    prefCheckBox8->onRestore();
    CheckBox_CornerCoordSystem->onRestore();
    CheckBox_ShowFPS->onRestore();
    CheckBox_UseAutoRotation->onRestore();
    FloatSpinBox_EyeDistance->onRestore();
    SelectionColor_Background->onRestore();
    backgroundColorFrom->onRestore();
    backgroundColorTo->onRestore();
    backgroundColorMid->onRestore();
    radioButtonSimple->onRestore();
    radioButtonGradient->onRestore();
    checkMidColor->onRestore();
    radioPerspective->onRestore();
    radioOrthographic->onRestore();
    checkBoxPreselection->onRestore();
    checkBoxSelection->onRestore();
    HighlightColor->onRestore();
    SelectionColor->onRestore();
    comboNavigationStyle->onRestore();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettings3DViewImp::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettings3DViewImp.cpp"
