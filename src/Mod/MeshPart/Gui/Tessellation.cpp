/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#include "Tessellation.h"
#include "ui_Tessellation.h"
#include <App/Document.h>
#include <Gui/Command.h>
#include <Gui/BitmapFactory.h>
#include <Mod/Part/App/PartFeature.h>

using namespace MeshPartGui;

Tessellation::Tessellation(QWidget* parent)
  : QWidget(parent), ui(new Ui_Tessellation)
{
    ui->setupUi(this);
    ui->treeWidget->hide();
    Gui::Command::doCommand(Gui::Command::Doc, "import MeshPart");
}

Tessellation::~Tessellation()
{
}

bool Tessellation::accept()
{
    std::vector<Part::Feature*> shapes = Gui::Selection().getObjectsOfType<Part::Feature>();
    for (std::vector<Part::Feature*>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
        QString cmd = QString::fromAscii(
            "__doc__=FreeCAD.getDocument(\"%1\")\n"
            "__mesh__=__doc__.addObject(\"Mesh::Feature\",\"Mesh\")\n"
            "__mesh__.Mesh=MeshPart.meshFromShape(__doc__.getObject(\"%2\").Shape,%3,0,0,%4)\n"
            "del __doc__, __mesh__\n")
            .arg(QLatin1String((*it)->getDocument()->getName()))
            .arg(QLatin1String((*it)->getNameInDocument()))
            .arg(ui->spinMaxEdgeLength->value())
            .arg(ui->spinDeviation->value());
        Gui::Command::doCommand(Gui::Command::Doc, (const char*)cmd.toAscii());
    }

    return true;
}

// ---------------------------------------

TaskTessellation::TaskTessellation()
{
    widget = new Tessellation();
    Gui::TaskView::TaskBox* taskbox = new Gui::TaskView::TaskBox(
        QPixmap()/*Gui::BitmapFactory().pixmap("MeshPart_Mesher")*/,
        widget->windowTitle(), true, 0);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
}

TaskTessellation::~TaskTessellation()
{
    // automatically deleted in the sub-class
}

void TaskTessellation::open()
{
}

void TaskTessellation::clicked(int)
{
}

bool TaskTessellation::accept()
{
    return widget->accept();
}

bool TaskTessellation::reject()
{
    return true;
}

#include "moc_Tessellation.cpp"
