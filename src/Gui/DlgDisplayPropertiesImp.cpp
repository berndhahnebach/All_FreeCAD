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
#ifndef _PreComp_
# include <QDockWidget>
#endif

#include "DlgDisplayPropertiesImp.h"
#include "DlgMaterialPropertiesImp.h"
#include "DockWindowManager.h"
#include "View3DInventorViewer.h"
#include "View3DInventor.h"
#include "Command.h"
#include "Application.h"
#include "Widgets.h"
#include "Selection.h"
#include "Document.h"
#include "ViewProvider.h"
#include "WaitCursor.h"
#include "SpinBox.h"

#include <Base/Console.h>
#include <App/Application.h>
#include <App/Feature.h>
#include <App/Material.h>

using namespace Gui::Dialog;
using namespace std;


/* TRANSLATOR Gui::Dialog::DlgDisplayPropertiesImp */

/**
 *  Constructs a DlgDisplayPropertiesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgDisplayPropertiesImp::DlgDisplayPropertiesImp( QWidget* parent, Qt::WFlags fl )
  : QDialog( parent, fl )
{
    this->setupUi(this);
    textLabel1_3->hide();
    changePlot->hide();

    std::vector<Gui::ViewProvider*> views = getSelection();
    setDisplayModes(views);
    fillupMaterials();
    setMaterial(views);
    setShapeColor(views);
    setPointSize(views);
    setLineWidth(views);
    setTransparency(views);

    // embed this dialog into a dockable widget container
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    QDockWidget* dw = pDockMgr->addDockWindow("Display properties", this, Qt::AllDockWidgetAreas);
    dw->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    dw->setAllowedAreas(Qt::DockWidgetAreas());
    dw->setFloating(true);
    dw->show();

    Gui::Selection().Attach(this);
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgDisplayPropertiesImp::~DlgDisplayPropertiesImp()
{
    // no need to delete child widgets, Qt does it all for us
    Gui::Selection().Detach(this);
}

void DlgDisplayPropertiesImp::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                                       Gui::SelectionSingleton::MessageType Reason)
{
    if (Reason.Type == SelectionChanges::AddSelection ||
        Reason.Type == SelectionChanges::RmvSelection ||
        Reason.Type == SelectionChanges::ClrSelection) {
        std::vector<Gui::ViewProvider*> views = getSelection();
        setDisplayModes(views);
        setMaterial(views);
        setShapeColor(views);
        setPointSize(views);
        setLineWidth(views);
        setTransparency(views);
    }
}

/**
 * Destroys the dock window this object is embedded into without destroying itself.
 */
void DlgDisplayPropertiesImp::reject()
{
    // closes the dock window
    Gui::DockWindowManager* pDockMgr = Gui::DockWindowManager::instance();
    pDockMgr->removeDockWindow(this);
    QDialog::reject();
}

/**
 * Opens a dialog that allows to modify the 'ShapeMaterial' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_buttonUserDefinedMaterial_clicked()
{
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    DlgMaterialPropertiesImp dlg(this);
    dlg.setViewProviders(Provider);
    dlg.exec();

    buttonColor->setColor(dlg.diffuseColor->color());
}

/**
 * Sets the 'ShapeMaterial' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_changeMaterial_activated(const QString& material)
{
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    App::Material mat(Materials[material]);
    App::Color diffuseColor = mat.diffuseColor;
    buttonColor->setColor(QColor((int)(diffuseColor.r*255.0f), (int)(diffuseColor.g*255.0f), (int)(diffuseColor.b*255.0f)));

    for (std::vector<Gui::ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++) {
        App::Property* prop = (*It)->getPropertyByName("ShapeMaterial");
        if (prop && prop->getTypeId() == App::PropertyMaterial::getClassTypeId()) {
            App::PropertyMaterial* ShapeMaterial = (App::PropertyMaterial*)prop;
            ShapeMaterial->setValue(mat);
        }
    }
}

/**
 * Sets the 'Display' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_changeMode_activated(const QString& s)
{
    Gui::WaitCursor wc;
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    for (std::vector<Gui::ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++) {
        App::Property* prop = (*It)->getPropertyByName("DisplayMode");
        if (prop && prop->getTypeId() == App::PropertyEnumeration::getClassTypeId()) {
            App::PropertyEnumeration* Display = (App::PropertyEnumeration*)prop;
            Display->setValue((const char*)s.toAscii());
        }
    }
}

void DlgDisplayPropertiesImp::on_changePlot_activated(const QString&s)
{
    Base::Console().Log("Plot = %s\n",(const char*)s.toAscii());
}

/**
 * Sets the 'ShapeColor' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_buttonColor_changed()
{
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    QColor s = buttonColor->color();
    App::Color c(s.red()/255.0,s.green()/255.0,s.blue()/255.0);
    for (std::vector<Gui::ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++) {
        App::Property* prop = (*It)->getPropertyByName("ShapeColor");
        if (prop && prop->getTypeId() == App::PropertyColor::getClassTypeId()) {
            App::PropertyColor* ShapeColor = (App::PropertyColor*)prop;
            ShapeColor->setValue(c);
        }
    }
}

/**
 * Sets the 'Transparency' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_spinTransparency_valueChanged(int transparency)
{
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    for (std::vector<Gui::ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++) {
        App::Property* prop = (*It)->getPropertyByName("Transparency");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId())) {
            App::PropertyInteger* Transparency = (App::PropertyInteger*)prop;
            Transparency->setValue(transparency);
        }
    }
}

/**
 * Sets the 'PointSize' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_spinPointSize_valueChanged(int pointsize)
{
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    for (std::vector<Gui::ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++) {
        App::Property* prop = (*It)->getPropertyByName("PointSize");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId())) {
            App::PropertyFloat* PointSize = (App::PropertyFloat*)prop;
            PointSize->setValue((float)pointsize);
        }
    }
}

/**
 * Sets the 'LineWidth' property of all selected view providers.
 */
void DlgDisplayPropertiesImp::on_spinLineWidth_valueChanged(int linewidth)
{
    std::vector<Gui::ViewProvider*> Provider = getSelection();
    for (std::vector<Gui::ViewProvider*>::iterator It= Provider.begin();It!=Provider.end();It++) {
        App::Property* prop = (*It)->getPropertyByName("LineWidth");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId())) {
            App::PropertyFloat* LineWidth = (App::PropertyFloat*)prop;
            LineWidth->setValue((float)linewidth);
        }
    }
}

void DlgDisplayPropertiesImp::setDisplayModes(const std::vector<Gui::ViewProvider*>& views)
{
    QStringList commonModes, modes;
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("DisplayMode");
        if (prop && prop->getTypeId() == App::PropertyEnumeration::getClassTypeId()) {
            App::PropertyEnumeration* display = static_cast<App::PropertyEnumeration*>(prop);
            const std::vector<std::string>& value = display->getEnumVector();
            if (it == views.begin()) {
                for (std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt)
                    commonModes << QLatin1String(jt->c_str());
            }
            else {
                for (std::vector<std::string>::const_iterator jt = value.begin(); jt != value.end(); ++jt) {
                    if (commonModes.contains(QLatin1String(jt->c_str())))
                        modes << QLatin1String(jt->c_str());
                }

                commonModes = modes;
                modes.clear();
            }
        }
    }

    changeMode->clear();
    changeMode->addItems(commonModes);
    changeMode->setDisabled(commonModes.isEmpty());

    // find the display mode to activate
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("DisplayMode");
        if (prop && prop->getTypeId() == App::PropertyEnumeration::getClassTypeId()) {
            App::PropertyEnumeration* display = static_cast<App::PropertyEnumeration*>(prop);
            QString activeMode = QString::fromAscii(display->getValueAsString());
            int index = changeMode->findText(activeMode);
            if (index != -1) {
                changeMode->setCurrentIndex(index);
                break;
            }
        }
    }
}

void DlgDisplayPropertiesImp::setMaterial(const std::vector<Gui::ViewProvider*>& views)
{
    bool material = false;
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("ShapeMaterial");
        if (prop && prop->getTypeId() == App::PropertyMaterial::getClassTypeId()) {
            material = true;
            break;
        }
    }

    changeMaterial->setEnabled(material);
    buttonUserDefinedMaterial->setEnabled(material);
}

void DlgDisplayPropertiesImp::fillupMaterials()
{
    Materials[QLatin1String("Brass")]         = App::Material::BRASS;
    Materials[QLatin1String("Bronze")]        = App::Material::BRONZE;
    Materials[QLatin1String("Copper")]        = App::Material::COPPER;
    Materials[QLatin1String("Gold")]          = App::Material::GOLD;
    Materials[QLatin1String("Pewter")]        = App::Material::PEWTER;
    Materials[QLatin1String("Plaster")]       = App::Material::PLASTER;
    Materials[QLatin1String("Plastic")]       = App::Material::PLASTIC;
    Materials[QLatin1String("Silver")]        = App::Material::SILVER;
    Materials[QLatin1String("Steel")]         = App::Material::STEEL;
    Materials[QLatin1String("Stone")]         = App::Material::STONE;
    Materials[QLatin1String("Shiny plastic")] = App::Material::SHINY_PLASTIC;
    Materials[QLatin1String("Satin")]         = App::Material::SATIN;
    Materials[QLatin1String("Metalized")]     = App::Material::METALIZED;
    Materials[QLatin1String("Neon GNC")]      = App::Material::NEON_GNC;
    Materials[QLatin1String("Chrome")]        = App::Material::CHROME;
    Materials[QLatin1String("Aluminium")]     = App::Material::ALUMINIUM;
    Materials[QLatin1String("Obsidian")]      = App::Material::OBSIDIAN;
    Materials[QLatin1String("Neon PHC")]      = App::Material::NEON_PHC;
    Materials[QLatin1String("Jade")]          = App::Material::JADE;
    Materials[QLatin1String("Ruby")]          = App::Material::RUBY;
    Materials[QLatin1String("Emerald")]       = App::Material::EMERALD;

    QStringList material = Materials.keys();
    material.sort();
    changeMaterial->addItem(QLatin1String("Default"));
    changeMaterial->addItems(material);
    Materials[QLatin1String("Default")]       = App::Material::DEFAULT;
}

void DlgDisplayPropertiesImp::setShapeColor(const std::vector<Gui::ViewProvider*>& views)
{
    bool shapeColor = false;
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("ShapeColor");
        if (prop && prop->getTypeId() == App::PropertyColor::getClassTypeId()) {
            App::Color c = static_cast<App::PropertyColor*>(prop)->getValue();
            QColor shape;
            shape.setRgb((int)(c.r*255.0f), (int)(c.g*255.0f),(int)(c.b*255.0f));
            buttonColor->setColor(shape);
            shapeColor = true;
            break;
        }
    }

    buttonColor->setEnabled(shapeColor);
}

void DlgDisplayPropertiesImp::setPointSize(const std::vector<Gui::ViewProvider*>& views)
{
    bool pointSize = false;
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("PointSize");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId())) {
            spinPointSize->setValue((int)static_cast<App::PropertyFloat*>(prop)->getValue());
            pointSize = true;
            break;
        }
    }

    spinPointSize->setEnabled(pointSize);
}

void DlgDisplayPropertiesImp::setLineWidth(const std::vector<Gui::ViewProvider*>& views)
{
    bool lineWidth = false;
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("LineWidth");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyFloat::getClassTypeId())) {
            spinLineWidth->setValue((int)static_cast<App::PropertyFloat*>(prop)->getValue());
            lineWidth = true;
            break;
        }
    }

    spinLineWidth->setEnabled(lineWidth);
}

void DlgDisplayPropertiesImp::setTransparency(const std::vector<Gui::ViewProvider*>& views)
{
    bool transparency = false;
    for (std::vector<Gui::ViewProvider*>::const_iterator it = views.begin(); it != views.end(); ++it) {
        App::Property* prop = (*it)->getPropertyByName("Transparency");
        if (prop && prop->getTypeId().isDerivedFrom(App::PropertyInteger::getClassTypeId())) {
            spinTransparency->setValue(static_cast<App::PropertyInteger*>(prop)->getValue());
            transparency = true;
            break;
        }
    }

    spinTransparency->setEnabled(transparency);
    horizontalSlider->setEnabled(transparency);
}

std::vector<Gui::ViewProvider*> DlgDisplayPropertiesImp::getSelection() const
{
    std::vector<Gui::ViewProvider*> views;

    // get the complete selection
    std::vector<SelectionSingleton::SelObj> sel = Selection().getCompleteSelection();
    for (std::vector<SelectionSingleton::SelObj>::iterator it = sel.begin(); it != sel.end(); ++it) {
        Gui::ViewProvider* view = Application::Instance->getDocument(it->pDoc)->getViewProvider(it->pObject);
        views.push_back(view);
    }

    return views;
}

#include "moc_DlgDisplayPropertiesImp.cpp"

