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
# include <QString>
#endif

#include "ui_TaskSketcherConstrains.h"
#include "TaskSketcherConstrains.h"
#include "ViewProviderSketch.h"

#include <Mod/Sketcher/App/SketchObject.h>

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Gui/BitmapFactory.h>
#include <Base/Console.h>
#include <boost/bind.hpp>



using namespace SketcherGui;
using namespace Gui::TaskView;

TaskSketcherConstrains::TaskSketcherConstrains(ViewProviderSketch *sketchView)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"),tr("Constraints"),true, 0)
    , sketchView(sketchView)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskSketcherConstrains();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    //QObject::connect(sketchView,SIGNAL(ConstraintsChanged()),this,SLOT(ConstraintsChanged()));
    c = sketchView->ConstraintsChanged.connect(boost::bind(&SketcherGui::TaskSketcherConstrains::ConstraintsChanged, this));

    this->groupLayout()->addWidget(proxy);
}

TaskSketcherConstrains::~TaskSketcherConstrains()
{
    c.disconnect();
    delete ui;
}

void TaskSketcherConstrains::ConstraintsChanged(void)
{
    QIcon horiz( Gui::BitmapFactory().pixmap("Constraint_Horizontal") );
    QIcon vert ( Gui::BitmapFactory().pixmap("Constraint_Vertical") );
    QIcon lock ( Gui::BitmapFactory().pixmap("Sketcher_ConstrainLock") );
    QIcon coinc( Gui::BitmapFactory().pixmap("Constraint_PointOnPoint") );
    QIcon para ( Gui::BitmapFactory().pixmap("Constraint_Parallel") );

    assert(sketchView);
    // Build up ListView with the constraints
    const std::vector< Sketcher::Constraint * > &vals = sketchView->getSketchObject()->Constraints.getValues();

    ui->listWidgetConstraints->clear();
    std::string name;
    char buf[50];
    int i=1;
	for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it,++i){
        if((*it)->Name == ""){
            sprintf(buf,"Constraint%d",i);
            name = buf;
        }else
            name =(*it)->Name;

        switch((*it)->Type){
            case Sketcher::Horizontal:
                ui->listWidgetConstraints->addItem(new QListWidgetItem(horiz,QString::fromLatin1(name.c_str())));
                break;
            case Sketcher::Vertical:
                ui->listWidgetConstraints->addItem(new QListWidgetItem(vert,QString::fromLatin1(name.c_str())));
                break;
            case Sketcher::Coincident:
                ui->listWidgetConstraints->addItem(new QListWidgetItem(coinc,QString::fromLatin1(name.c_str())));
                break;
            case Sketcher::Parallel:
                ui->listWidgetConstraints->addItem(new QListWidgetItem(para,QString::fromLatin1(name.c_str())));
                break;
            default:
                ui->listWidgetConstraints->addItem(new QListWidgetItem(QString::fromLatin1(name.c_str())));
                break;
        }

    }

}


void TaskSketcherConstrains::changeEvent(QEvent *e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}



#include "moc_TaskSketcherConstrains.cpp"
