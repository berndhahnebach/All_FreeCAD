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
#ifndef _PreComp_
# include <TopoDS_Edge.hxx>
# include <TopoDS_Shape.hxx>
# include <TopExp.hxx>
# include <TopTools_ListOfShape.hxx>
# include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#endif

#include <Gui/Qt4All.h>

#include "DlgFilletEdges.h"
#include "../App/PartFeature.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Command.h>

using namespace PartGui;

FilletRadiusDelegate::FilletRadiusDelegate(QObject *parent) : QItemDelegate(parent)
{
}

QWidget *FilletRadiusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
{
    if (index.column() < 1)
        return 0;

    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0.0);
    editor->setMaximum(100.0);
    editor->setSingleStep(0.1);

    return editor;
}

void FilletRadiusDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void FilletRadiusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    //double value = spinBox->value();
    QString value = QString::fromAscii("%1").arg(spinBox->value(),0,'f',2);

    model->setData(index, value, Qt::EditRole);
}

void FilletRadiusDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
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

    // set tree view with three columns
    QStandardItemModel* model = new FilletRadiusModel(this);
    model->insertColumns(0,3);
    model->setHeaderData(0, Qt::Horizontal, tr("Edges to fillet"), Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, tr("Start radius"), Qt::DisplayRole);
    model->setHeaderData(2, Qt::Horizontal, tr("End radius"), Qt::DisplayRole);
    ui.treeView->setRootIsDecorated(false);
    ui.treeView->setItemDelegate(new FilletRadiusDelegate(this));
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

void DlgFilletEdges::on_shapeObject_activated(int index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
    model->removeRows(0, model->rowCount());

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
        int count = 0;
        for (int i=1; i<= edge2Face.Extent(); ++i) {
            int index = i-1;
            // set the hash value as user data to use it in accept()
            TopoDS_Shape edge = edge2Face.FindKey(i);
            if (edge2Face.FindFromIndex(i).Extent() == 2)
                count++;
        }
        model->insertRows(0, count);
        int index = 0;
        for (int i=1; i<= edge2Face.Extent(); ++i, ++index) {
            // set the hash value as user data to use it in accept()
            TopoDS_Shape edge = edge2Face.FindKey(i);
            if (edge2Face.FindFromIndex(i).Extent() == 2) {
                int id = edge.HashCode(IntegerLast());
                model->setData(model->index(index, 0), QVariant(tr("Edge <%1>").arg(id)));
                model->setData(model->index(index, 0), QVariant(id), Qt::UserRole);
                model->setData(model->index(index, 0), Qt::Checked, Qt::CheckStateRole);
                model->setData(model->index(index, 1), QVariant(QString::fromAscii("%1").arg(1.0,0,'f',2)));
                model->setData(model->index(index, 2), QVariant(QString::fromAscii("%1").arg(1.0,0,'f',2)));
            }
        }
    }

    ui.okButton->setEnabled(model->rowCount() > 0);
}

void DlgFilletEdges::on_filletType_activated(int index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
    if (index == 0) {
        model->setHeaderData(1, Qt::Horizontal, tr("Radius"), Qt::DisplayRole);
        ui.treeView->hideColumn(2);
        ui.filletEndRadius->hide();
    }
    else {
        model->setHeaderData(1, Qt::Horizontal, tr("Start radius"), Qt::DisplayRole);
        ui.treeView->showColumn(2);
        ui.filletEndRadius->show();
    }

    ui.treeView->resizeColumnToContents(0);
    ui.treeView->resizeColumnToContents(1);
    ui.treeView->resizeColumnToContents(2);
}

void DlgFilletEdges::on_filletStartRadius_valueChanged(double radius)
{
    QAbstractItemModel* model = ui.treeView->model();
    QString text = QString::fromAscii("%1").arg(radius,0,'f',2);
    for (int i=0; i<model->rowCount(); ++i) {
        QVariant value = model->index(i,0).data(Qt::CheckStateRole);
        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());

        // is item checked
        if (checkState & Qt::Checked) {
            model->setData(model->index(i, 1), QVariant(text));
        }
    }
}

void DlgFilletEdges::on_filletEndRadius_valueChanged(double radius)
{
    QAbstractItemModel* model = ui.treeView->model();
    QString text = QString::fromAscii("%1").arg(radius,0,'f',2);
    for (int i=0; i<model->rowCount(); ++i) {
        QVariant value = model->index(i,0).data(Qt::CheckStateRole);
        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());

        // is item checked
        if (checkState & Qt::Checked) {
            model->setData(model->index(i, 2), QVariant(text));
        }
    }
}

void DlgFilletEdges::accept()
{
    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    QAbstractItemModel* model = ui.treeView->model();
    bool end_radius = !ui.treeView->isColumnHidden(2);
    bool todo = false;

    QString shape, type, name;
    int index = ui.shapeObject->currentIndex();
    shape = ui.shapeObject->itemData(index).toString();
    type = QString::fromAscii("Part::Fillet");
    name = QString::fromAscii(activeDoc->getUniqueObjectName("Fillet").c_str());

    activeDoc->openTransaction("Extrude");
    QString code = QString::fromAscii(
        "FreeCAD.ActiveDocument.addObject(\"%1\",\"%2\")\n"
        "FreeCAD.ActiveDocument.%2.Base = FreeCAD.ActiveDocument.%3\n"
        "__fillets__ = []\n")
        .arg(type).arg(name).arg(shape);
    for (int i=0; i<model->rowCount(); ++i) {
        QVariant value = model->index(i,0).data(Qt::CheckStateRole);
        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());

        // is item checked
        if (checkState & Qt::Checked) {
            // the hash code of the edge
            int id = model->index(i,0).data(Qt::UserRole).toInt();
            double r1 = model->index(i,1).data().toDouble();
            double r2 = r1;
            if (end_radius)
                r2 = model->index(i,2).data().toDouble();
            code += QString::fromAscii(
                "__fillets__.append((%1,%2,%3))\n")
                .arg(id).arg(r1,0,'f',2).arg(r2,0,'f',2);
            todo = true;
        }
    }

    if (!todo) {
        QMessageBox::warning(this, tr("No edge selected"),
            tr("No edge entity is checked to fillet.\n"
               "Please check one or more edge entities first."));
        return;
    }

    code += QString::fromAscii(
        "FreeCAD.ActiveDocument.%1.Contour = __fillets__\n"
        "del __fillets__\n"
        "FreeCADGui.ActiveDocument.%2.Visibility = False\n")
        .arg(name).arg(shape);
    Gui::Application::Instance->runPythonCode((const char*)code.toAscii());
    activeDoc->commitTransaction();
    activeDoc->recompute();

    QDialog::accept();
}

#include "moc_DlgFilletEdges.cpp"
