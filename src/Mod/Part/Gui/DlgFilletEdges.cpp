/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
#include <Gui/Qt4All.h>

#include "DlgFilletEdges.h"
#include "../App/PartFeature.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Command.h>

using namespace PartGui;

DlgFilletEdges::DlgFilletEdges(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl)
{
    ui.setupUi(this);
    ui.okButton->setDisabled(true);
    findShapes();

    QStringList labels;
    labels << tr("Edges to fillet") << tr("Radius") << tr("End radius");
    ui.treeWidget->setHeaderLabels(labels);
    ui.treeWidget->headerItem()->setTextAlignment(0, Qt::AlignLeft);
    ui.treeWidget->headerItem()->setTextAlignment(1, Qt::AlignLeft);
    ui.treeWidget->headerItem()->setTextAlignment(2, Qt::AlignLeft);
    ui.treeWidget->hideColumn(2);
    ui.treeWidget->setRootIsDecorated(false);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgFilletEdges::~DlgFilletEdges()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgFilletEdges::findShapes()
{
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    if (!activeDoc) return;

    std::vector<App::DocumentObject*> objs = activeDoc->getObjectsOfType
        (Part::Feature::getClassTypeId());
    int index = 1;
    for (std::vector<App::DocumentObject*>::iterator it = objs.begin(); it!=objs.end(); ++it, ++index) {
        ui.shapeObject->addItem(QString::fromUtf8((*it)->Label.getValue()));
        ui.shapeObject->setItemData(index, QString::fromAscii((*it)->getNameInDocument()));
    }
}

void DlgFilletEdges::accept()
{
#if 0
    App::Document* activeDoc = App::GetApplication().getActiveDocument();

    QString shape, type, name;
    int index = ui.comboBox->currentIndex();
    shape = ui.comboBox->itemData(index).toString();
    type = QString::fromAscii("Part::Extrusion");
    name = QString::fromAscii(activeDoc->getUniqueObjectName("Extrude").c_str());
    double len = ui.dirLen->value();

    activeDoc->openTransaction("Extrude");
    QString code = QString::fromAscii(
        "FreeCAD.ActiveDocument.addObject(\"%1\",\"%2\")\n"
        "FreeCAD.ActiveDocument.%2.Base = FreeCAD.ActiveDocument.%3\n"
        "FreeCAD.ActiveDocument.%2.Dir = (%4,%5,%6)\n")
        .arg(type).arg(name).arg(shape)
        .arg(ui.dirX->value()*len)
        .arg(ui.dirY->value()*len)
        .arg(ui.dirZ->value()*len);
    Gui::Application::Instance->runPythonCode((const char*)code.toAscii());
    activeDoc->commitTransaction();
    activeDoc->recompute();
#endif
    
    QDialog::accept();
}

void DlgFilletEdges::on_shapeObject_activated(int index)
{
    ui.treeWidget->clear();
    ui.okButton->setEnabled(index > 0);

    QByteArray name = ui.shapeObject->itemData(index).toByteArray();
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc)
        return;
    App::DocumentObject* part = doc->getObject((const char*)name);
    if (part && part->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
        TopoDS_Shape myShape = static_cast<Part::Feature*>(part)->Shape.getValue();
        // build up map edge->face
        TopTools_IndexedDataMapOfShapeListOfShape edge2Face;
        TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, edge2Face);
        for (int i=1; i<= edge2Face.Extent(); ++i) {
            QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
            item->setCheckState(0, Qt::Unchecked);
            item->setText(0, tr("Edge<XX>"));
            //item->setFlags(item->flags() | Qt::ItemIsEditable);
            QDoubleSpinBox* r1 = new QDoubleSpinBox(ui.treeWidget);
            r1->setAutoFillBackground(true);
            ui.treeWidget->setItemWidget(item, 1, r1);
            QDoubleSpinBox* r2 = new QDoubleSpinBox(ui.treeWidget);
            r2->setAutoFillBackground(true);
            ui.treeWidget->setItemWidget(item, 2, r2);
            //QCheckBox* box = new QCheckBox(ui.treeWidget);
            //box->setText(tr("Edge<XX>"));
            //ui.treeWidget->setItemWidget(item, 0, box);
        }
    }
}

void DlgFilletEdges::on_filletType_activated(int index)
{
    if (index == 0) {
        ui.treeWidget->headerItem()->setText(1, tr("Radius"));
        ui.treeWidget->hideColumn(2);
    }
    else {
        ui.treeWidget->headerItem()->setText(1, tr("Start radius"));
        ui.treeWidget->showColumn(2);
    }

    ui.treeWidget->resizeColumnToContents(0);
    ui.treeWidget->resizeColumnToContents(1);
    ui.treeWidget->resizeColumnToContents(2);
}

void DlgFilletEdges::on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column)
{
    if (column < 1)
        return;
    ui.treeWidget->editItem(item,column);
}

#include "moc_DlgFilletEdges.cpp"
