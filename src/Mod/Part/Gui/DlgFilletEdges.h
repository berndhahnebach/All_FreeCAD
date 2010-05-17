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

#ifndef PARTGUI_DLGFILLETEDGES_H
#define PARTGUI_DLGFILLETEDGES_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include <QStandardItemModel>
#include <QItemDelegate>

namespace PartGui {

class Ui_DlgFilletEdges;
class FilletRadiusDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    FilletRadiusDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, 
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class FilletRadiusModel : public QStandardItemModel
{
    Q_OBJECT

public:
    FilletRadiusModel(QObject * parent = 0);

    Qt::ItemFlags flags (const QModelIndex & index) const;
    bool setData (const QModelIndex & index, const QVariant & value,
                  int role = Qt::EditRole);
Q_SIGNALS:
    void toogleCheckState(const QModelIndex&);
};

class DlgFilletEdgesP;
class DlgFilletEdges : public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    DlgFilletEdges(QWidget* parent = 0, Qt::WFlags fl = 0);
    ~DlgFilletEdges();
    bool accept();

protected:
    void findShapes();

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg);
    void onDeleteObject(const App::DocumentObject&);
    void onDeleteDocument(const App::Document&);

private Q_SLOTS:
    void on_shapeObject_activated(int);
    void on_selectAllButton_clicked();
    void on_selectNoneButton_clicked();
    void on_filletType_activated(int);
    void on_filletStartRadius_valueChanged(double);
    void on_filletEndRadius_valueChanged(double);
    void toogleCheckState(const QModelIndex&);

private:
    std::auto_ptr<Ui_DlgFilletEdges> ui;
    std::auto_ptr<DlgFilletEdgesP> d;
};

class FilletEdgesDialog : public QDialog
{
    Q_OBJECT

public:
    FilletEdgesDialog(QWidget* parent = 0, Qt::WFlags fl = 0);
    ~FilletEdgesDialog();
    void accept();

private:
    DlgFilletEdges* widget;
};

class TaskBoxFilletEdges : public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    TaskBoxFilletEdges(QWidget *parent = 0);
    ~TaskBoxFilletEdges();
    bool accept();

private:
    DlgFilletEdges* widget;
};

class TaskFilletEdges : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskFilletEdges();
    ~TaskFilletEdges();

public:
    virtual void open();
    virtual void clicked(QAbstractButton *);
    virtual bool accept();
    virtual bool reject();

    virtual QDialogButtonBox::StandardButtons getStandardButtons() const
    { return QDialogButtonBox::Ok|QDialogButtonBox::Cancel; }

private:
    TaskBoxFilletEdges* taskbox;
};

} // namespace PartGui

#endif // PARTGUI_DLGFILLETEDGES_H
