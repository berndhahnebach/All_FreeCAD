/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_TASKVIEW_TaskSelectLinkProperty_H
#define GUI_TASKVIEW_TaskSelectLinkProperty_H

#include "TaskView.h"
#include <Gui/Selection.h>
#include <boost/signals.hpp>


class Ui_TaskSelectLinkProperty;

namespace App {
class Property;
}

namespace Gui {
class ViewProvider;
namespace TaskView {

class GuiExport TaskSelectLinkProperty : public TaskBox, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    TaskSelectLinkProperty(QWidget *parent = 0);
    ~TaskSelectLinkProperty();
    /// Observer message from the Selection
    void OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                  Gui::SelectionSingleton::MessageType Reason);

private Q_SLOTS:
    void on_Remove_clicked(bool);
    void on_Add_clicked(bool);
    void on_Invert_clicked(bool);
    void on_Help_clicked(bool);

private:
    QWidget* proxy;
    Ui_TaskSelectLinkProperty* ui;
};

} //namespace TaskView
} //namespace Gui

#endif // GUI_TASKVIEW_TASKAPPERANCE_H
