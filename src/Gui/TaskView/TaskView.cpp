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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "TaskView.h"
#include "TaskDialog.h"
#include "TaskAppearance.h"
#include "TaskEditControl.h"
#include <Gui/Document.h>
#include <Gui/Application.h>
#include <Gui/ViewProvider.h>
#include <Gui/Control.h>

using namespace Gui::TaskView;

//**************************************************************************
//**************************************************************************
// TaskContent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//**************************************************************************
//**************************************************************************
// TaskWidget
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskWidget::TaskWidget( QWidget *parent)
    : QWidget(parent)
{
    
}

TaskWidget::~TaskWidget()
{
}



//**************************************************************************
//**************************************************************************
// TaskBox
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskBox::TaskBox(const QPixmap &icon, const QString &title, bool expandable, QWidget *parent)
    : iisTaskBox(icon, title, expandable, parent)
{
    setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
}

TaskBox::~TaskBox()
{
}


//**************************************************************************
//**************************************************************************
// TaskView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskView::TaskView(QWidget *parent)
    : QScrollArea(parent),ActiveDialog(0),ActiveCtrl(0)
{
    //addWidget(new TaskEditControl(this));
    //addWidget(new TaskAppearance(this));
    //addStretch();
    taskPanel = new iisTaskPanel(this);
    taskPanel->setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
    this->setWidget(taskPanel);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setMinimumWidth(200);
}

TaskView::~TaskView()
{
}

void TaskView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                        Gui::SelectionSingleton::MessageType Reason)
{
    std::string temp;

    if (Reason.Type == SelectionChanges::AddSelection) {
        //Gui::Document* pDoc = Application::Instance->getDocument(Reason.pDocName);
        //Gui::ViewProvider *pVP = pDoc->getViewProviderByName(Reason.pObjectName);
    }
    else if (Reason.Type == SelectionChanges::ClrSelection) {
    }
    else if (Reason.Type == SelectionChanges::SetSelection) {
    }
    else if (Reason.Type == SelectionChanges::RmvSelection) {
    }
}

void TaskView::showDialog(TaskDialog *dlg)
{
    assert(!ActiveDialog);
    assert(!ActiveCtrl);

    // first creat the control element set it up and wire it:
    ActiveCtrl = new TaskEditControl(this);
    ActiveCtrl->buttonBox->setStandardButtons(dlg->getStandardButtons());

    // make conection to the needed signals
    QObject::connect(ActiveCtrl->buttonBox    ,SIGNAL(accepted()),this,SLOT(accept()));
    QObject::connect(ActiveCtrl->buttonBox    ,SIGNAL(rejected()),this,SLOT(reject()));
    QObject::connect(ActiveCtrl->buttonBox    ,SIGNAL(helpRequested()),this,SLOT(helpRequested()));
    QObject::connect(ActiveCtrl->buttonBox    ,SIGNAL(clicked(QAbstractButton *)),this,SLOT(clicked(QAbstractButton *)));

    std::vector<QWidget*> &cont = dlg->getDlgContent();

    taskPanel->addWidget(ActiveCtrl);

    for(std::vector<QWidget*>::iterator it=cont.begin();it!=cont.end();++it){
        taskPanel->addWidget(*it);
    }
    taskPanel->addStretch();

    // set as active Dialog
    ActiveDialog = dlg;

    ActiveDialog->open();
}

void TaskView::removeDialog(void)
{
    taskPanel->removeWidget(ActiveCtrl);
    delete ActiveCtrl;
    ActiveCtrl = 0;

    std::vector<QWidget*> &cont = ActiveDialog->getDlgContent();
    for(std::vector<QWidget*>::iterator it=cont.begin();it!=cont.end();++it){
        taskPanel->removeWidget(*it);
    }
    taskPanel->removeStretch();
    // signal control the end of the dialog
    Gui::Control().dlgDone();
    delete ActiveDialog;
    ActiveDialog = 0;
}

void TaskView::addTaskWatcher(std::vector<TaskWatcher*> &Watcher)
{
    ActiveWatcher = Watcher;

    for(std::vector<TaskWatcher*>::iterator it=ActiveWatcher.begin();it!=ActiveWatcher.end();++it){
        std::vector<QWidget*> &cont = (*it)->getWatcherContent();
        for(std::vector<QWidget*>::iterator it2=cont.begin();it2!=cont.end();++it){
            taskPanel->addWidget(*it2);
        }

    }
    taskPanel->addStretch();

}

void TaskView::removeTaskWatcher(void)
{
    for(std::vector<TaskWatcher*>::iterator it=ActiveWatcher.begin();it!=ActiveWatcher.end();++it){
        std::vector<QWidget*> &cont = (*it)->getWatcherContent();
        for(std::vector<QWidget*>::iterator it2=cont.begin();it2!=cont.end();++it){
            taskPanel->removeWidget(*it2);
        }

    }
    taskPanel->removeStretch();
}

void TaskView::accept()
{
    ActiveDialog->accept();
    removeDialog();
}

void TaskView::reject()
{
    ActiveDialog->reject();
    removeDialog();
}

void TaskView::helpRequested()
{
   ActiveDialog->helpRequested();
}

void TaskView::clicked ( QAbstractButton * button )
{
    ActiveDialog->clicked(button);
}


#include "moc_TaskView.cpp"
