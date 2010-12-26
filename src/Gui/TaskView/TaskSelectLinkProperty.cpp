/***************************************************************************
 *   Copyright (c) 2010 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <algorithm>
# include <boost/bind.hpp>
#endif

#include "ui_TaskSelectLinkProperty.h"
#include "TaskSelectLinkProperty.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Gui/SelectionFilter.h>
#include <Base/Console.h>

using namespace Gui::TaskView;

TaskSelectLinkProperty::TaskSelectLinkProperty(const char *sFilter,App::Property *prop,QWidget *parent)
    : TaskBox(Gui::BitmapFactory().pixmap("mouse_pointer"),tr("edit selection"),true, parent),Filter(0),LinkSub(0),LinkList(0)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskSelectLinkProperty();
    ui->setupUi(proxy);

    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
    Gui::Selection().Attach(this);

    ui->Remove->setIcon(Gui::BitmapFactory().pixmap("delete"));
    ui->Add->setIcon(Gui::BitmapFactory().pixmap("add"));
    ui->Invert->setIcon(Gui::BitmapFactory().pixmap("edit_remove"));
    ui->Help->setIcon(Gui::BitmapFactory().pixmap("help-browser"));

    // property have to be set! 
    assert(prop);
    if(prop->getTypeId().isDerivedFrom(App::PropertyLinkSub::getClassTypeId())){
        LinkSub = dynamic_cast<App::PropertyLinkSub *>(prop);
    }


    setFilter(sFilter);

}

TaskSelectLinkProperty::~TaskSelectLinkProperty()
{
    delete ui;
    Gui::Selection().Detach(this);
}

/// @cond DOXERR


bool TaskSelectLinkProperty::setFilter(const char * sFilter)
{
    Filter = new SelectionFilter(sFilter);
    return Filter->isValid();
}


void TaskSelectLinkProperty::activate(void)
{
    // first clear the selection
    Gui::Selection().clearSelection();
    // set the gate for the filter 
    Gui::Selection().addSelectionGate(new SelectionFilterGate(Filter));

    // In case of LinkSub property 
    if(LinkSub){



    }

    checkSelectionStatus();

}


bool TaskSelectLinkProperty::accept(void)
{


    // clear selection and remove gate (return to normal operation)
    Gui::Selection().clearSelection();
    Gui::Selection().rmvSelectionGate();
    return true;
}

bool TaskSelectLinkProperty::reject(void)
{


    // clear selection and remove gate (return to normal operation)
    Gui::Selection().clearSelection();
    Gui::Selection().rmvSelectionGate();
    return true;
}

void TaskSelectLinkProperty::checkSelectionStatus(void)
{
    QPalette palette;

    if(Filter->match()){
        palette.setBrush(QPalette::Base,QColor(200,250,200));
        on_selectionFit();
    }else{
        palette.setBrush(QPalette::Base,QColor(250,200,200));
        on_selectionMisfit();
    }
    //ui->listWidget->setAutoFillBackground(true);
    ui->listWidget->setPalette(palette);

}


void TaskSelectLinkProperty::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                              Gui::SelectionSingleton::MessageType Reason)
{
    if (Reason.Type == SelectionChanges::AddSelection ||
        Reason.Type == SelectionChanges::RmvSelection ||
        Reason.Type == SelectionChanges::SetSelection ||
        Reason.Type == SelectionChanges::ClrSelection) {
            ui->listWidget->clear();
            std::vector<Gui::SelectionSingleton::SelObj> sel = Gui::Selection().getSelection();
            for( std::vector<Gui::SelectionSingleton::SelObj>::const_iterator it=sel.begin();it!=sel.end();++it){
                std::string temp;
                temp += it->FeatName;
                temp += "::";
                temp += it->SubName;
                new QListWidgetItem(QString::fromAscii(temp.c_str()), ui->listWidget);
            }
            checkSelectionStatus();
    }
}
/// @endcond


void TaskSelectLinkProperty::on_Remove_clicked(bool)
{
}

void TaskSelectLinkProperty::on_Add_clicked(bool)
{
}

void TaskSelectLinkProperty::on_Invert_clicked(bool)
{
}

void TaskSelectLinkProperty::on_Help_clicked(bool)
{
}



#include "moc_TaskSelectLinkProperty.cpp"
