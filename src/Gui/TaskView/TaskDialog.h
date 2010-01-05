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


#ifndef GUI_TASKVIEW_TASKDIALOG_H
#define GUI_TASKVIEW_TASKDIALOG_H

#include <map>
#include <string>
#include <vector>

#include <QDialogButtonBox>

#include <Gui/iisTaskPanel/include/iisTaskPanel>
#include <Gui/Selection.h>

namespace App {

}

namespace Gui {
namespace TaskView {

class TaskContent;

/// Father class of content with header and Icon
class GuiExport TaskDialog : public QObject
{
    Q_OBJECT

public:
    TaskDialog();
    ~TaskDialog();

    std::vector<QWidget*> &getDlgContent(void);

    /// tells the framework which buttons whisched for the dialog
    virtual QDialogButtonBox::StandardButtons getStandardButtons(void){return QDialogButtonBox::Ok|QDialogButtonBox::Cancel;}


public:
    /// is called the TaskView when the dialog is opened
    virtual void open();
    /// is called by the framework if an button is clicked which has no accept or rject role
    virtual void clicked(QAbstractButton *);
    /// is called by the framework if the dialog is accepted (Ok)
    virtual void accept();
    /// is called by the framework if the dialog is rejected (Cancel)
    virtual void reject();
    /// is called by the framework if the user press the help button 
    virtual void helpRequested();

protected:
    /// List of TaskBoxes of that dialog
    std::vector<QWidget*> Content;

};



} //namespace TaskView
} //namespace Gui

#endif // GUI_TASKVIEW_TASKDIALOG_H
