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

#include "DlgSettings3DViewPartImp.h"
#include "ViewProvider.h"
#include <Gui/PrefWidgets.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Base/Console.h>

using namespace PartGui;

/**
 *  Constructs a DlgSettings3DViewPartImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgSettings3DViewPartImp::DlgSettings3DViewPartImp( QWidget* parent )
  : PreferencePage( parent )
{
    this->setupUi(this);
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgSettings3DViewPartImp::~DlgSettings3DViewPartImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgSettings3DViewPartImp::saveSettings()
{
    prefFloatSpinBox1->onSave();
    prefCheckBox8->onSave();
    prefCheckBox3->onSave();

    float deviation = (float)prefFloatSpinBox1->value();
    bool nonormal = prefCheckBox8->isChecked();
    bool quality = prefCheckBox3->isChecked();

    // search for Part view providers and apply the new settings
    std::vector<App::Document*> docs = App::GetApplication().getDocuments();
    for (std::vector<App::Document*>::iterator it = docs.begin(); it != docs.end(); ++it) {
        Gui::Document* doc = Gui::Application::Instance->getDocument(*it);
        std::vector<Gui::ViewProvider*> views = doc->getViewProvidersOfType(ViewProviderPart::getClassTypeId());
        for (std::vector<Gui::ViewProvider*>::iterator jt = views.begin(); jt != views.end(); ++jt) {
            if (static_cast<ViewProviderPart*>(*jt)->Deviation.getValue() != deviation)
                static_cast<ViewProviderPart*>(*jt)->Deviation.setValue(deviation);
            if (static_cast<ViewProviderPart*>(*jt)->NoPerVertexNormals.getValue() != nonormal)
                static_cast<ViewProviderPart*>(*jt)->NoPerVertexNormals.setValue(nonormal);
            if (static_cast<ViewProviderPart*>(*jt)->QualityNormals.getValue() != quality)
                static_cast<ViewProviderPart*>(*jt)->QualityNormals.setValue(quality);
        }
    }
}

void DlgSettings3DViewPartImp::loadSettings()
{
    prefFloatSpinBox1->onRestore();
    prefCheckBox8->onRestore();
    prefCheckBox3->onRestore();
}

#include "moc_DlgSettings3DViewPartImp.cpp"
