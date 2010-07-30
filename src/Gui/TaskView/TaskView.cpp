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
# include <boost/bind.hpp>
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
    : iisTaskBox(icon, title, expandable, parent), wasShown(false)
{
    setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
}

TaskBox::~TaskBox()
{
}

void TaskBox::showEvent(QShowEvent*)
{
    wasShown = true;
}

void TaskBox::hideGroupBox()
{
    if (!wasShown) {
        // get approximate height
        int h=0;
        int ct = groupLayout()->count();
        for (int i=0; i<ct; i++) {
            QLayoutItem* item = groupLayout()->itemAt(i);
            if (item && item->widget()) {
                QWidget* w = item->widget();
                h += w->height();
            }
        }

        m_tempHeight = m_fullHeight = h;
        // For the very first time the group gets shown
        // we cannot do the animation because the layouting
        // is not yet fully done
        m_foldDelta = 0;
    }
    else {
        m_tempHeight = m_fullHeight = myGroup->height();
        m_foldDelta = m_fullHeight / myScheme->groupFoldSteps;
    }

    m_foldStep = 0.0;
    m_foldDirection = -1;

    // make sure to have the correct icon
    bool block = myHeader->blockSignals(true);
    myHeader->fold();
    myHeader->blockSignals(block);

    myDummy->setFixedHeight(0);
    myDummy->hide();
    myGroup->hide();

    m_foldPixmap = QPixmap();
    setFixedHeight(myHeader->height());
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void TaskBox::actionEvent (QActionEvent* e)
{
    QAction *action = e->action();
    switch (e->type()) {
    case QEvent::ActionAdded:
        {
            iisIconLabel *label = new iisIconLabel(
                action->icon(), action->text(), this);
            this->addIconLabel(label);
            connect(label,SIGNAL(clicked()),action,SIGNAL(triggered()));
            break;
        }
    case QEvent::ActionChanged:
        {
            // cannot change anything
            break;
        }
    case QEvent::ActionRemoved:
        {
            // cannot change anything
            break;
        }
    default:
        break;
    }
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

    Gui::Selection().Attach(this);
    App::GetApplication().signalActiveDocument.connect
        (boost::bind(&Gui::TaskView::TaskView::slotActiveDocument, this, _1));
}

TaskView::~TaskView()
{
     Gui::Selection().Detach(this);
}

void TaskView::slotActiveDocument(const App::Document& doc)
{
    updateWatcher();
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

    if(!ActiveDialog)
        updateWatcher();

}

void TaskView::showDialog(TaskDialog *dlg)
{
    // if trying to open the same dialog twice nothing needs to be done
    if (ActiveDialog == dlg)
        return;
    assert(!ActiveDialog);
    assert(!ActiveCtrl);

    // remove the TaskWatcher as long the Dialog is up
    removeTaskWatcher();

    // first creat the control element set it up and wire it:
    ActiveCtrl = new TaskEditControl(this);
    ActiveCtrl->buttonBox->setStandardButtons(dlg->getStandardButtons());

    // make conection to the needed signals
    connect(ActiveCtrl->buttonBox,SIGNAL(accepted()),
            this,SLOT(accept()));
    connect(ActiveCtrl->buttonBox,SIGNAL(rejected()),
            this,SLOT(reject()));
    connect(ActiveCtrl->buttonBox,SIGNAL(helpRequested()),
            this,SLOT(helpRequested()));
    connect(ActiveCtrl->buttonBox,SIGNAL(clicked(QAbstractButton *)),
            this,SLOT(clicked(QAbstractButton *)));

    const std::vector<QWidget*>& cont = dlg->getDialogContent();

    // give task dialog to customize the button box
    dlg->modifyStandardButtons(ActiveCtrl->buttonBox);
    taskPanel->addWidget(ActiveCtrl);

    for (std::vector<QWidget*>::const_iterator it=cont.begin();it!=cont.end();++it){
        taskPanel->addWidget(*it);
    }

    if (!dlg->needsFullSpace())
        taskPanel->addStretch();

    // set as active Dialog
    ActiveDialog = dlg;

    ActiveDialog->open();
}

void TaskView::removeDialog(void)
{
    if (ActiveCtrl) {
        taskPanel->removeWidget(ActiveCtrl);
        delete ActiveCtrl;
        ActiveCtrl = 0;
    }

    if (ActiveDialog) {
        const std::vector<QWidget*> &cont = ActiveDialog->getDialogContent();
        for(std::vector<QWidget*>::const_iterator it=cont.begin();it!=cont.end();++it){
            taskPanel->removeWidget(*it);
        }
        delete ActiveDialog;
        ActiveDialog = 0;
    }

    taskPanel->removeStretch();

    // put the watcher back in control
    addTaskWatcher();
}

void TaskView::updateWatcher(void)
{
    // add all widghets for all watcher to the task view
    for(std::vector<TaskWatcher*>::iterator it=ActiveWatcher.begin();it!=ActiveWatcher.end();++it){

        bool match = (*it)->shouldShow();
        std::vector<QWidget*> &cont = (*it)->getWatcherContent();
        for(std::vector<QWidget*>::iterator it2=cont.begin();it2!=cont.end();++it2){
            if(match)
                (*it2)->show();
            else
                (*it2)->hide();
        }

    }
}

void TaskView::addTaskWatcher(const std::vector<TaskWatcher*> &Watcher)
{
    // remove and delete the old set of TaskWatcher
    for(std::vector<TaskWatcher*>::iterator it=ActiveWatcher.begin();it!=ActiveWatcher.end();++it)
        delete *it;

    ActiveWatcher = Watcher;
    addTaskWatcher();
}

void TaskView::addTaskWatcher(void)
{
    // add all widghets for all watcher to the task view
    for(std::vector<TaskWatcher*>::iterator it=ActiveWatcher.begin();it!=ActiveWatcher.end();++it){
        std::vector<QWidget*> &cont = (*it)->getWatcherContent();
        for(std::vector<QWidget*>::iterator it2=cont.begin();it2!=cont.end();++it2){
           taskPanel->addWidget(*it2);
           (*it2)->show();
        }
    }
    taskPanel->addStretch();
    updateWatcher();
}

void TaskView::removeTaskWatcher(void)
{
    // remove all widghets
    for(std::vector<TaskWatcher*>::iterator it=ActiveWatcher.begin();it!=ActiveWatcher.end();++it){
        std::vector<QWidget*> &cont = (*it)->getWatcherContent();
        for(std::vector<QWidget*>::iterator it2=cont.begin();it2!=cont.end();++it2){
            (*it2)->hide();
            taskPanel->removeWidget(*it2);
        }

    }
    taskPanel->removeStretch();
}

void TaskView::accept()
{
    if (ActiveDialog->accept())
        removeDialog();
}

void TaskView::reject()
{
    if (ActiveDialog->reject())
        removeDialog();
}

void TaskView::helpRequested()
{
    ActiveDialog->helpRequested();
}

void TaskView::clicked (QAbstractButton * button)
{
    int id = ActiveCtrl->buttonBox->standardButton(button);
    ActiveDialog->clicked(id);
}


#include "moc_TaskView.cpp"
