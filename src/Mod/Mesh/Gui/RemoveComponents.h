/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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


#ifndef MESHGUI_REMOVECOMPONENTS_H
#define MESHGUI_REMOVECOMPONENTS_H

#include <QDialog>
#include <App/DocumentObserver.h>

// forward declarations
class SoEventCallback;
namespace App { class DocumentObject; }
namespace MeshGui {
class ViewProviderMesh;
class Ui_RemoveComponents;

/**
 * Non-modal dialog to de/select components, regions, the complete or single faces
 * of a mesh and delete them.
 * @author Werner Mayer
 */
class MeshGuiExport RemoveComponents : public QDialog, public App::DocumentObjectObserver
{
    Q_OBJECT

public:
    RemoveComponents(QWidget* parent = 0, Qt::WFlags fl = 0);
    ~RemoveComponents();
    void reject();

public Q_SLOTS:
    void on_selectRegion_clicked();
    void on_selectAll_clicked();
    void on_selectComponents_clicked();
    void on_deselectRegion_clicked();
    void on_deselectAll_clicked();
    void on_deselectComponents_clicked();
    void on_deleteButton_clicked();

private:
    void cancelObservation();
    std::list<ViewProviderMesh*> getViewProviders() const;

    static void selectGLCallback(void * ud, SoEventCallback * n);

private:
    struct iotaGen;
    Ui_RemoveComponents* ui;
    bool selectRegion;
};

}

#endif // MESHGUI_REMOVECOMPONENTS_H
