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
#include <QStandardItemModel>

#include "DlgFilletEdges.h"
#include "../App/PartFeature.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Command.h>

using namespace PartGui;

RadiusDelegate::RadiusDelegate(QObject *parent) : QItemDelegate(parent)
{
}

QWidget *RadiusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
{
    if (index.column() < 1)
        return 0; // first column is not editable
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0.0);
    editor->setMaximum(100.0);

    return editor;
}

void RadiusDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void RadiusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    //double value = spinBox->value();
    QString value = spinBox->text();

    model->setData(index, value, Qt::EditRole);
}

void RadiusDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

// --------------------------------------------------------------

DlgFilletEdges::DlgFilletEdges(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl)
{
    ui.setupUi(this);
    ui.okButton->setDisabled(true);
    findShapes();

    QStandardItemModel* model = new QStandardItemModel(this);
    model->insertColumns(0,3);
    model->setHeaderData(0, Qt::Horizontal, tr("Edges to fillet"), Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, tr("Start radius"), Qt::DisplayRole);
    model->setHeaderData(2, Qt::Horizontal, tr("End radius"), Qt::DisplayRole);
    ui.treeView->setRootIsDecorated(false);
    ui.treeView->setItemDelegate(new RadiusDelegate(this));
    ui.treeView->setModel(model);
    QHeaderView* header = ui.treeView->header();
    header->setResizeMode(0, QHeaderView::Stretch);
    header->setDefaultAlignment(Qt::AlignLeft);
    header->setMovable(false);
    on_filletType_activated(0);
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
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
    model->removeRows(0, model->rowCount());
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

        // populate the model
        model->insertRows(0, edge2Face.Extent());
        for (int i=1; i<= edge2Face.Extent(); ++i) {
            int index = i-1;
            model->setData(model->index(index, 0), QVariant(tr("Edge <XXX>")));
            model->setData(model->index(index, 1), QVariant(QString::fromAscii("%1").arg(1.0,0,'f',2)));
            model->setData(model->index(index, 2), QVariant(QString::fromAscii("%1").arg(1.0,0,'f',2)));
            //QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget);
            //item->setCheckState(0, Qt::Unchecked);
            //item->setText(0, tr("Edge<XX>"));
            ////item->setFlags(item->flags() | Qt::ItemIsEditable);
            //QDoubleSpinBox* r1 = new QDoubleSpinBox(ui.treeWidget);
            //r1->setAutoFillBackground(true);
            //ui.treeWidget->setItemWidget(item, 1, r1);
            //QDoubleSpinBox* r2 = new QDoubleSpinBox(ui.treeWidget);
            //r2->setAutoFillBackground(true);
            //ui.treeWidget->setItemWidget(item, 2, r2);
            ////QCheckBox* box = new QCheckBox(ui.treeWidget);
            ////box->setText(tr("Edge<XX>"));
            ////ui.treeWidget->setItemWidget(item, 0, box);
        }
    }
}

void DlgFilletEdges::on_filletType_activated(int index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
    if (index == 0) {
        model->setHeaderData(1, Qt::Horizontal, tr("Radius"), Qt::DisplayRole);
        ui.treeView->hideColumn(2);
    }
    else {
        model->setHeaderData(1, Qt::Horizontal, tr("Start radius"), Qt::DisplayRole);
        ui.treeView->showColumn(2);
    }

    ui.treeView->resizeColumnToContents(0);
    ui.treeView->resizeColumnToContents(1);
    ui.treeView->resizeColumnToContents(2);
}

#include "moc_DlgFilletEdges.cpp"
