/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include "DlgExtrusion.h"
#include "../App/PartFeature.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/ViewProvider.h>

using namespace PartGui;

DlgExtrusion::DlgExtrusion(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl)
{
    ui.setupUi(this);
    ui.okButton->setDisabled(true);
    findShapes();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgExtrusion::~DlgExtrusion()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgExtrusion::findShapes()
{
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) return;
    Gui::Document* activeGui = Gui::Application::Instance->getDocument(activeDoc);

    std::vector<App::DocumentObject*> objs = activeDoc->getObjectsOfType
        (Part::Feature::getClassTypeId());
    for (std::vector<App::DocumentObject*>::iterator it = objs.begin(); it!=objs.end(); ++it) {
        TopoDS_Shape shape = static_cast<Part::Feature*>(*it)->Shape.getValue();
        TopAbs_ShapeEnum type = shape.ShapeType();
        if (type == TopAbs_VERTEX || type == TopAbs_EDGE ||
            type == TopAbs_WIRE || type == TopAbs_FACE ||
            type == TopAbs_SHELL) {
            QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
            item->setText(0, QString::fromUtf8((*it)->Label.getValue()));
            item->setData(0, Qt::UserRole, QString::fromAscii((*it)->getNameInDocument()));
            Gui::ViewProvider* vp = activeGui->getViewProvider(*it);
            if (vp)
                item->setIcon(0, vp->getIcon());
        }
    }
}

void DlgExtrusion::accept()
{
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    activeDoc->openTransaction("Extrude");

    QString shape, type, name;
    QList<QTreeWidgetItem *> items = ui.treeWidget->selectedItems();
    for (QList<QTreeWidgetItem *>::iterator it = items.begin(); it != items.end(); ++it) {
        shape = (*it)->data(0, Qt::UserRole).toString();
        type = QString::fromAscii("Part::Extrusion");
        name = QString::fromAscii(activeDoc->getUniqueObjectName("Extrude").c_str());
        double len = ui.dirLen->value();

        QString code = QString::fromAscii(
            "FreeCAD.ActiveDocument.addObject(\"%1\",\"%2\")\n"
            "FreeCAD.ActiveDocument.%2.Base = FreeCAD.ActiveDocument.%3\n"
            "FreeCAD.ActiveDocument.%2.Dir = (%4,%5,%6)\n"
            "FreeCADGui.ActiveDocument.%3.Visibility = False\n")
            .arg(type).arg(name).arg(shape)
            .arg(ui.dirX->value()*len)
            .arg(ui.dirY->value()*len)
            .arg(ui.dirZ->value()*len);
        Gui::Application::Instance->runPythonCode((const char*)code.toAscii());
    }

    activeDoc->commitTransaction();
    activeDoc->recompute();

    QDialog::accept();
}

void DlgExtrusion::on_treeWidget_itemSelectionChanged()
{
    ui.okButton->setEnabled(!ui.treeWidget->selectedItems().isEmpty());
}

void DlgExtrusion::on_checkNormal_toggled(bool b)
{
    ui.dirX->setDisabled(b);
    ui.dirY->setDisabled(b);
    ui.dirZ->setDisabled(b);
}

#include "moc_DlgExtrusion.cpp"
