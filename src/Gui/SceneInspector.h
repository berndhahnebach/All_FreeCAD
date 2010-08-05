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

#ifndef GUI_SCENEINSPECTOR_H
#define GUI_SCENEINSPECTOR_H

#include <QStandardItemModel>
#include <QDialog>

class SoNode;

namespace Gui {
namespace Dialog {

class Ui_SceneInspector;
class SceneModel : public QStandardItemModel
{
    Q_OBJECT

public:
    SceneModel(QObject* parent);
    virtual ~SceneModel();

    int columnCount (const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData (int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);
    void setNode(SoNode* node);
    Qt::ItemFlags flags (const QModelIndex & index) const;

private:
    void setNode(QModelIndex, SoNode*);
};

class DlgInspector : public QDialog
{
    Q_OBJECT

public:
    DlgInspector(QWidget* parent = 0, Qt::WFlags fl = 0);
    ~DlgInspector();

    void setNode(SoNode* node);

private Q_SLOTS:
    void on_refreshButton_clicked();

private:
    Ui_SceneInspector* ui;
};

} // namespace Dialog
} // namespace Gui

#endif // GUI_SCENEINSPECTOR_H
