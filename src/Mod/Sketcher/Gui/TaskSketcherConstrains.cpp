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
#include "ui_InsertDatum.h"
#include "TaskSketcherConstrains.h"
#include "ViewProviderSketch.h"

#include <Mod/Sketcher/App/SketchObject.h>

#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Base/Console.h>
#include <boost/bind.hpp>



using namespace SketcherGui;
using namespace Gui::TaskView;

// helper class to store additional information about the listWidget entry.
class ConstraintItem:public QListWidgetItem
{
public:
    ConstraintItem(const QIcon & icon, const QString & text,int ConstNbr,Sketcher::ConstraintType t):QListWidgetItem(icon,text),ConstraintNbr(ConstNbr),Type(t){}
    ConstraintItem(const QString & text,int ConstNbr,Sketcher::ConstraintType t):QListWidgetItem(text),ConstraintNbr(ConstNbr),Type(t){}
    int ConstraintNbr;
    Sketcher::ConstraintType Type;
};

TaskSketcherConstrains::TaskSketcherConstrains(ViewProviderSketch *sketchView)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"),tr("Constraints"),true, 0)
    , sketchView(sketchView)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskSketcherConstrains();
    ui->setupUi(proxy);
    //QMetaObject::connectSlotsByName(this);

    // connecting the needed signals
    QObject::connect(
        ui->comboBoxFilter, SIGNAL(currentIndexChanged(int)),
        this              , SLOT  (on_comboBoxFilter_currentIndexChanged(int))
       );
    QObject::connect(
        ui->listWidgetConstraints, SIGNAL(itemSelectionChanged()),
        this                     , SLOT  (on_listWidgetConstraints_itemSelectionChanged())
       );
    QObject::connect(
        ui->listWidgetConstraints, SIGNAL(itemActivated(QListWidgetItem *)),
        this                     , SLOT  (on_listWidgetConstraints_itemActivated(QListWidgetItem *))
       );
    //QObject::connect(
    //    ui->listWidgetConstraints, SIGNAL(entered(const QModelIndex &)),
    //    this                     , SLOT  (on_listWidgetConstraints_entered(const QModelIndex &))
    //   );

    connectionConstraintsChanged = sketchView->signalConstraintsChanged.connect(boost::bind(&SketcherGui::TaskSketcherConstrains::slotConstraintsChanged, this));

    this->groupLayout()->addWidget(proxy);
 
    Gui::Selection().Attach(this);

    slotConstraintsChanged();
}

TaskSketcherConstrains::~TaskSketcherConstrains()
{
    Gui::Selection().Detach(this);
    connectionConstraintsChanged.disconnect();
    delete ui;
}

/// @cond DOXERR
void TaskSketcherConstrains::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                              Gui::SelectionSingleton::MessageType msg)
{
    std::string temp;
    if (msg.Type == Gui::SelectionChanges::ClrSelection) {
        ui->listWidgetConstraints->clearSelection ();
    }
    else if (msg.Type == Gui::SelectionChanges::AddSelection) {
        // is it this object??
        if(strcmp(msg.pDocName,sketchView->getSketchObject()->getDocument()->getName())==0
            &&strcmp(msg.pObjectName,sketchView->getSketchObject()->getNameInDocument())== 0) {
                if(msg.pSubName){
                    std::string shapetype(msg.pSubName);
                    if (shapetype.size() > 10 && shapetype.substr(0,10) == "Constraint") {
                        //int index=std::atoi(&shapetype[10]);
                        //ui->listWidgetConstraints->setCurrentIndex(QModelIndex(0));
                    }

                }
        }        
    }
    else if (msg.Type == Gui::SelectionChanges::RmvSelection) {}
    else if (msg.Type == Gui::SelectionChanges::SetSelection) {}       
}
/// @endcond DOXERR

void TaskSketcherConstrains::on_comboBoxFilter_currentIndexChanged(int)
{
    slotConstraintsChanged();
}

void TaskSketcherConstrains::on_listWidgetConstraints_itemSelectionChanged(void)
{
    
}

void TaskSketcherConstrains::on_listWidgetConstraints_itemActivated(QListWidgetItem *item)
{
    ConstraintItem *it = dynamic_cast<ConstraintItem*>(item);

    // if its the right constraint
    if (it->Type == Sketcher::Distance) {
        const std::vector< Sketcher::Constraint * > &vals = sketchView->getSketchObject()->Constraints.getValues();
        Sketcher::Constraint *Const = vals[it->ConstraintNbr];
        assert(Const->Type == it->Type);
        double datum = Const->Value;

        QDialog dlg(this);
        Ui::InsertDatum ui_ins_datum;
        ui_ins_datum.setupUi(&dlg);

        ui_ins_datum.lineEdit->setText(QString::number(datum));
        if (dlg.exec()) {
            double newDatum = ui_ins_datum.lineEdit->text().toDouble();
            Gui::Command::openCommand("Add sketch constraints");
            Gui::Command::doCommand(Gui::Command::Doc,"App.ActiveDocument.%s.setDatum(%f,%i)",
                      sketchView->getObject()->getNameInDocument(),
                      newDatum,it->ConstraintNbr);
            // use recompute() instead of execute() to properly handle placement
            //sketchView->getSketchObject()->recompute();
        }
    }
}

//void TaskSketcherConstrains::on_listWidgetConstraints_entered(const QModelIndex &index)
//{
//    index;
//}


void TaskSketcherConstrains::slotConstraintsChanged(void)
{
    QIcon horiz( Gui::BitmapFactory().pixmap("Constraint_Horizontal") );
    QIcon vert ( Gui::BitmapFactory().pixmap("Constraint_Vertical") );
    QIcon lock ( Gui::BitmapFactory().pixmap("Sketcher_ConstrainLock") );
    QIcon coinc( Gui::BitmapFactory().pixmap("Constraint_PointOnPoint") );
    QIcon para ( Gui::BitmapFactory().pixmap("Constraint_Parallel") );
    QIcon tang ( Gui::BitmapFactory().pixmap("Constraint_Tangent") );
    QIcon dist ( Gui::BitmapFactory().pixmap("Constraint_Length") );

    assert(sketchView);
    // Build up ListView with the constraints
    const std::vector< Sketcher::Constraint * > &vals = sketchView->getSketchObject()->Constraints.getValues();

    ui->listWidgetConstraints->clear();
    QString name;

    int Filter = ui->comboBoxFilter->currentIndex();

    int i=1;
    for(std::vector< Sketcher::Constraint * >::const_iterator it= vals.begin();it!=vals.end();++it,++i){
        if ((*it)->Name == "")
            name = QString(QString::fromLatin1("Constraint%1")).arg(i);
        else
            name = QString(QString::fromLatin1((*it)->Name.c_str()));

        switch((*it)->Type){
            case Sketcher::Horizontal:
                if(Filter<2 || (*it)->Name != "")
                    ui->listWidgetConstraints->addItem(new ConstraintItem(horiz,name,i-1,(*it)->Type));
                break;
            case Sketcher::Vertical:
                if(Filter<2 || (*it)->Name != "")
                    ui->listWidgetConstraints->addItem(new ConstraintItem(vert,name,i-1,(*it)->Type));
                break;
            case Sketcher::Coincident:
                if(Filter<1 || (*it)->Name != "")
                    ui->listWidgetConstraints->addItem(new ConstraintItem(coinc,name,i-1,(*it)->Type));
                break;
            case Sketcher::Parallel:
                if(Filter<2 || (*it)->Name != "")
                    ui->listWidgetConstraints->addItem(new ConstraintItem(para,name,i-1,(*it)->Type));
                break;
            case Sketcher::Tangent:
                if(Filter<2 || (*it)->Name != "")
                    ui->listWidgetConstraints->addItem(new ConstraintItem(tang,name,i-1,(*it)->Type));
                break;
            case Sketcher::Distance:
                if(Filter<3 || (*it)->Name != ""){
                    name = QString(QString::fromLatin1("%1 (%2)")).arg(name,14).arg((*it)->Value);
                    ui->listWidgetConstraints->addItem(new ConstraintItem(dist,name,i-1,(*it)->Type));
                }
                break;
            default:
                ui->listWidgetConstraints->addItem(new ConstraintItem(name,i-1,(*it)->Type));
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
