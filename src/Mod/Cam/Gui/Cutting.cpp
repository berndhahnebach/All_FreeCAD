/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
 *   Werner Mayer <wmayer@users.sourceforge.net>                           *
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

#include "Cutting.h"
#include <Mod/Cam/App/ChangeDyna.h> //Only for Testing
#include <Mod/Cam/App/path_simulate.h>
#include <QTimer>
#include <QByteArray>


#include <Base/Vector3D.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Parameter.h>

#include <Gui/ViewProvider.h>
#include <Gui/Selection.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <App/Document.h>
#include <Gui/Document.h>

#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <Mod/Cam/App/cutting_tools.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/Gui/ViewProvider.h>



using namespace CamGui;

Cutting::Cutting(QWidget* parent,Qt::WFlags fl)
        :QDialog(parent,fl),m_Process(NULL),m_PathSimulate(NULL),m_CuttingAlgo(NULL)
{
    this->setupUi(this);
    m_timer= false;
}

Cutting::~Cutting()
{
    delete m_CuttingAlgo;
    delete m_PathSimulate;
    delete m_Process;
}

bool Cutting::getProcessOutput()
{
    QByteArray result = m_Process->readAll();
    if (result.contains("Error"))
    {
        m_Process->kill();
        QMessageBox::critical(this, tr("FreeCAD CamWorkbench"),
                              tr("Fehler bei der Erzeugung\n"),
                              QMessageBox::Ok);
    }
    else if (result.contains("N o r m a l    t e r m i n a t i o n"))
    {
        QMessageBox::information(this, tr("FreeCAD CamWorkbench"),
                                 tr("Dyna-Job finished well\n"),
                                 QMessageBox::Ok);
    }

    return true;
}
void Cutting::on_adaptdynainput_clicked()
{
    //First we have to select the LS-Dyna Masterfile and the current working dir
    QString filename, path, program;
    QStringList arguments;
    filename = QFileDialog::getOpenFileName( this, "OpenDynaMaster",filename,"Ls-Dyna Keywords (*.k)" );
    if (filename.isNull())
        return;
    QFileInfo aFileInfo(filename);
    path = aFileInfo.absolutePath();
    QDir::setCurrent(path);
    program = "c:/Program Files/lsdyna/ls971d";
    arguments << " i="<< aFileInfo.fileName();
    m_Process = new QProcess(this);
    m_Process->start(program, arguments);
    //Now we check if the output is written correctly
    m_Process->waitForFinished(20000);
    aFileInfo.setFile("dyna.str");
    if (aFileInfo.size() == 0) //the file does not exist
    {
        QMessageBox::critical(this, tr("FreeCAD CamWorkbench"),
                              tr("Fehler bei der Erzeugung vom Struct File\n"),
                              QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(this, tr("FreeCAD CamWorkbench"),
                                 tr("Structured-Dyna gut erzeugt\n"),
                                 QMessageBox::Ok);
        ChangeDyna aFileChanger(m_PathSimulate->getPathTimes());
        if (aFileChanger.Read("dyna.str"))
            start_simulation->show();
    }
}

void Cutting::on_start_simulation_clicked()
{
    //check if the initial process is already killed
    m_Process->kill();
    QString program;
    QStringList arguments;
    program = "c:/Program Files/lsdyna/ls971d";
    arguments << " i=dyna2.str";
    connect(m_Process,SIGNAL(readyReadStandardError()),this,SLOT(getProcessOutput()));
    connect(m_Process,SIGNAL(readyReadStandardOutput()),this,SLOT(getProcessOutput()));
    m_Process->start(program, arguments);
}

void Cutting::selectShape()
{
    if (!m_timer)
    {
        int check_box1=0,check_box2=0;
        if (!m_Shape.IsNull())
        {
            check_box1 = QMessageBox::question(this, tr("FreeCAD CamWorkbench"),
                                               tr("You have already selected a CAD-Shape.\n"
                                                  "Do you want to make a new Selection?"),
                                               QMessageBox::Yes, QMessageBox::No);
        }
        else
        {
            check_box2 = QMessageBox::information(this, tr("FreeCAD CamWorkbench"),
                                                  tr("You have to select a CAD-Shape.\n"),
                                                  QMessageBox::Ok, QMessageBox::Cancel);
        }
        if ((check_box1 == QMessageBox::Yes) || (check_box2 == QMessageBox::Ok))
        {
            //First, remove the old selection from the Gui, so that we do not directly have the same CAD once again.
            Gui::Selection().clearCompleteSelection();
            //to make a Selection more easy, hide the dialog
            this->hide();
            QTimer::singleShot(100,this,SLOT(selectShape()));
            m_timer = true;
        }
    }
    else
    {
        std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Part::Feature::getClassTypeId());
        if ( fea.size() == 1)
        {
            m_Shape = static_cast<Part::Feature*>(fea.front())->Shape.getValue();
            //std::vector<Gui::SelectionSingleton::SelObj> aSelection = Gui::Selection().getSelection();
            this->show();
            CalculateZLevel->setEnabled(true);
            CalculateFeatureBased->setEnabled(true);
            CalculateSpiralBased->setEnabled(true);
            m_timer = false;
        }
        else
        {
            QTimer::singleShot(100,this,SLOT(selectShape()));
            m_timer = true;
        }
    }
}


void Cutting::setFace(const TopoDS_Shape& aShape, const float x, const float y, const float z)
{
    //check if a Shape is selected
    std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Part::Feature::getClassTypeId());
    if ( fea.size() == 1)
    {
        int test = aShape.ShapeType();
        //get Hash Code of Selected Face inside the selected Shape and also the Coordinates of the click
        if (aShape.ShapeType() != TopAbs_FACE)
        {
            QMessageBox::information(this, tr("FreeCAD CamWorkbench"), tr("You have to select a Face!!\n"));
            return;
        }

        TopoDS_Face tempFace = TopoDS::Face(aShape);
        //Now search for the Hash-Code in the m_Shape
        TopExp_Explorer anExplorer;
        TopoDS_Face aselectedFace;

        //pickPoint.Set(x,y,z);
        for (anExplorer.Init(m_Shape,TopAbs_FACE);anExplorer.More();anExplorer.Next())
        {
            if (tempFace.HashCode(IntegerLast()) == anExplorer.Current().HashCode(IntegerLast()))
            {
                m_CuttingAlgo->SetMachiningOrder(TopoDS::Face(anExplorer.Current()),x,y,z);
                break;
            }
        }
    }
}

void Cutting::on_CalculateZLevel_clicked()
{
    //Cutting-Klasse instanzieren
    if(m_CuttingAlgo == NULL)
    m_CuttingAlgo = new cutting_tools(m_Shape);
    else
    {
        delete m_CuttingAlgo;
        m_CuttingAlgo = new cutting_tools(m_Shape);
    }
    m_Mode = 1;
    CalculateFeatureBased->setEnabled(false);
    CalculateSpiralBased->setEnabled(false);
    toolpath_calculation_highest_level_button->setEnabled(true);
}

void Cutting::on_CalculateFeatureBased_clicked()
{
    if(m_CuttingAlgo == NULL)
    m_CuttingAlgo = new cutting_tools(m_Shape);
    else
    {
        delete m_CuttingAlgo;
        m_CuttingAlgo = new cutting_tools(m_Shape);
    }
    m_Mode = 2;
    toolpath_calculation_highest_level_button->setEnabled(true);
    CalculateZLevel->setEnabled(false);
    CalculateSpiralBased->setEnabled(false);
}

void Cutting::on_CalculateSpiralBased_clicked()
{
    if(m_CuttingAlgo == NULL)
    m_CuttingAlgo = new cutting_tools(m_Shape);
    else
    {
        delete m_CuttingAlgo;
        m_CuttingAlgo = new cutting_tools(m_Shape);
    }
    m_Mode = 3;//
    toolpath_calculation_highest_level_button->setEnabled(true);
    CalculateZLevel->setEnabled(false);
    CalculateFeatureBased->setEnabled(false);

}

void Cutting::on_select_shape_z_level_button_clicked()
{
    selectShape();
}

void Cutting::on_select_shape_feature_based_button_clicked()
{
    selectShape();
}

void Cutting::on_select_shape_spiral_based_button_clicked()
{
    selectShape();
}

void Cutting::on_toolpath_calculation_highest_level_button_clicked()
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view)
    {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, this);
        QMessageBox::information(this, tr("FreeCAD CamWorkbench"), tr("You have to pick a point.\n"));
        this->hide();
    }
    toolpath_calculation_middle_level_button->setEnabled(true);
    toolpath_calculation_lowest_level_button->setEnabled(true);
}

void Cutting::on_toolpath_calculation_middle_level_button_clicked()
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view)
    {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, this);
        QMessageBox::information(this, tr("FreeCAD CamWorkbench"), tr("You have to pick a point.\n"));
        this->hide();
    }
}

void Cutting::on_toolpath_calculation_lowest_level_button_clicked()
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view)
    {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, this);
        QMessageBox::information(this, tr("FreeCAD CamWorkbench"), tr("You have to pick a point.\n"));
        this->hide();
    }
    toolpath_calculation_go_button->setEnabled(true);
}

void Cutting::on_toolpath_calculation_go_button_clicked()
{
    //Do the actual Cut
    //First transfer the settings to the Cutting_tools class
    m_CuttingAlgo->m_UserSettings.cad_radius = cad_radius_box->value();
    m_CuttingAlgo->m_UserSettings.correction_factor = correction_factor_box->value();
    m_CuttingAlgo->m_UserSettings.level_distance = level_distance_box->value();
    m_CuttingAlgo->m_UserSettings.limit_angle = limit_angle_box->value();
    m_CuttingAlgo->m_UserSettings.master_radius = master_radius_box->value();
    m_CuttingAlgo->m_UserSettings.sheet_thickness = sheet_thickness_box->value();
    m_CuttingAlgo->m_UserSettings.slave_radius = slave_radius_box->value();
    m_CuttingAlgo->m_UserSettings.max_Vel = max_vel->value();
    m_CuttingAlgo->m_UserSettings.max_Acc = max_acc->value();
    m_CuttingAlgo->m_UserSettings.spring_pretension = spring_pretension->value();
    
    if(!m_CuttingAlgo->arrangecuts_ZLEVEL())
    {std::cout << "Konnte nicht sauber schneiden" << std::endl;}

    bool ok = true; 
    switch (m_Mode)
    {
    case 1:
        ok = m_CuttingAlgo->OffsetWires_Standard();
        break;
    case 2:
        ok = m_CuttingAlgo->OffsetWires_FeatureBased();
        break;
    case 3:
        ok = m_CuttingAlgo->OffsetWires_Spiral();
        break;
    }
    if(!ok)
    {
        QMessageBox::critical(this, tr("FreeCAD CamWorkbench"),
                              tr("Irgendwas stimmt nicht. Nochmal alles neu versuchen\n"),
                              QMessageBox::Ok);
        delete m_CuttingAlgo;
        m_CuttingAlgo = new cutting_tools(m_Shape);
        return;
    }
    DisplayCAMOutput();
    GenRobotOut->setEnabled(true);
    GenSimOut->setEnabled(true);
}

void Cutting::on_GenSimOut_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Simulation-Path Output-Directory"),"d:/",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (dir.isNull())
        return;
    QDir::setCurrent(dir);
    if(m_PathSimulate != NULL) delete m_PathSimulate;//If it exists already
    m_PathSimulate = new path_simulate(*(m_CuttingAlgo->getOutputhigh()),*(m_CuttingAlgo->getOutputlow()),m_CuttingAlgo->m_UserSettings);
    if (m_PathSimulate->MakePathSimulate())
        adaptdynainput->setEnabled(true);

}

void Cutting::on_GenRobotOut_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select File Output-Directory"),"d:/",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    QDir::setCurrent(dir);
    if(m_PathSimulate != NULL) delete m_PathSimulate;
      m_PathSimulate = new path_simulate(*(m_CuttingAlgo->getOutputhigh()),*(m_CuttingAlgo->getOutputlow()),m_CuttingAlgo->m_UserSettings);
      m_PathSimulate->MakePathRobot();
    

}
void Cutting::DisplayCAMOutput()
{
    BRep_Builder BB;
    TopoDS_Compound aCompound1,aCompound2;
    BB.MakeCompound(aCompound1);
    BB.MakeCompound(aCompound2);
    TopoDS_Edge anEdge;
    const std::vector<Handle_Geom_BSplineCurve>* topCurves;
    const std::vector<Handle_Geom_BSplineCurve>* botCurves;
    std::vector<Handle_Geom_BSplineCurve>::const_iterator an_it1;
    topCurves = m_CuttingAlgo->getOutputhigh();
    botCurves = m_CuttingAlgo->getOutputlow();
    for (an_it1 = topCurves->begin();an_it1!=topCurves->end();an_it1++)
    {
        BB.MakeEdge(anEdge,*an_it1,0.01);
        BB.Add(aCompound1,anEdge);

    }
    for (an_it1 = botCurves->begin();an_it1!=botCurves->end();an_it1++)
    {
        BB.MakeEdge(anEdge,*an_it1,0.01);
        BB.Add(aCompound2,anEdge);
    }

    App::Document* doc = App::GetApplication().getActiveDocument();
    App::DocumentObject* obj = doc->addObject("Part::Feature","Master-Tool");
    App::DocumentObject* obj1 = doc->addObject("Part::Feature","Slave-Tool");

    Part::Feature* part1 = static_cast<Part::Feature*>(obj);
    Part::Feature* part2 = static_cast<Part::Feature*>(obj1);
    part1->setShape(aCompound1);
    part2->setShape(aCompound2);






    //
    //      for (unsigned int i=0;i<aTestOutput.size();++i)
    //      {
    //          BB.Add(aCompound,anEdge);
    //      }

    //anewCuttingEnv.OffsetWires_Standard(10.0);

    //std::vector<Handle_Geom_BSplineCurve> topCurves;
    //std::vector<Handle_Geom_BSplineCurve> botCurves;
    //std::vector<Handle_Geom_BSplineCurve>::iterator an_it;
    //topCurves = *(anewCuttingEnv.getOutputhigh());
    //botCurves = *(anewCuttingEnv.getOutputlow());
    //for (unsigned int i=0;i<topCurves.size();++i)
    //{
    //    GeomAdaptor_Curve aCurveAdaptor(topCurves[i]);
    //    GCPnts_QuasiUniformDeflection aPointGenerator(aCurveAdaptor,0.1);
    //    for (int t=1;t<=aPointGenerator.NbPoints();++t)
    //    {
    //        anoutput << (aPointGenerator.Value(t)).X() <<","<< (aPointGenerator.Value(t)).Y() <<","<<(aPointGenerator.Value(t)).Z()<<std::endl;
    //    }
    //}
    //for (unsigned int i=0;i<botCurves.size();++i)
    //{
    //    GeomAdaptor_Curve aCurveAdaptor(botCurves[i]);
    //    GCPnts_QuasiUniformDeflection aPointGenerator(aCurveAdaptor,0.1);
    //    for (int t=1;t<=aPointGenerator.NbPoints();++t)
    //    {
    //        anoutput2 << (aPointGenerator.Value(t)).X() <<","<< (aPointGenerator.Value(t)).Y() <<","<<(aPointGenerator.Value(t)).Z()<<std::endl;
    //    }
    //}
    //anoutput.close();
    //anoutput2.close();

}



void Cutting::zLevelCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    Cutting* that = reinterpret_cast<Cutting*>(ud);

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP)
    {
        n->setHandled();
        view->setEditing(false);
        view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, that);
        that->show();
    }
    else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN)
    {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL)
        {
            QMessageBox::warning(Gui::getMainWindow(),"z level", "No shape picked!");
            return;
        }

        n->setHandled();

        // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
        // really from the mesh we render and not from any other geometry
        Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
        if (!vp || !vp->getTypeId().isDerivedFrom(PartGui::ViewProviderPart::getClassTypeId()))
            return;
        PartGui::ViewProviderPart* vpp = static_cast<PartGui::ViewProviderPart*>(vp);
        TopoDS_Shape sh = vpp->getShape(point);
        if (!sh.IsNull())
        {
            // ok a shape was picked
            n->setHandled();
            view->setEditing(false);
            view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, that);
            that->show();
            SbVec3f pt = point->getPoint();
            that->setFace(sh, pt[0],pt[1],pt[2]);
        }
    }
}

#include "moc_Cutting.cpp"
