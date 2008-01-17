#include "PreCompiled.h"

#include "Cutting.h"
#include <QTimer.h>

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

#include <Mod/Cam/App/cutting_tools.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/Gui/ViewProvider.h>


using namespace CamGui;

Cutting::Cutting(QWidget* parent,Qt::WFlags fl)
        :QDialog(parent,fl)
{
    this->setupUi(this);
    m_timer= false;
}

Cutting::~Cutting()
{

}

#include <string>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Surface.hxx>
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
            CalculcateZLevel->setEnabled(true);
            CalculcateFeatureBased->setEnabled(true);
            m_timer = false;
        }
        else
        {
            QTimer::singleShot(100,this,SLOT(selectShape()));
            m_timer = true;
        }
    }
}


void Cutting::selectFace(TopoDS_Shape aShape, float x, float y, float z)
{
	//check if a Shape is selected
	std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Part::Feature::getClassTypeId());
    if ( fea.size() == 1)
	{
		//get Hash Code of Selected Face inside the selected Shape and also the Coordinates of the click
		if(aShape.ShapeType() != TopAbs_FACE)
			return;

		TopoDS_Face tempFace = TopoDS::Face(aShape);
		//Now search for the Hash-Code in the m_Shape
		TopExp_Explorer anExplorer;
		TopoDS_Face aselectedFace;
		for(anExplorer.Init(m_Shape,TopAbs_FACE);anExplorer.More();anExplorer.Next())
		{
			if(tempFace.HashCode(IntegerLast()) == anExplorer.Current().HashCode(IntegerLast()))
			{
				aselectedFace = TopoDS::Face(anExplorer.Current());
				break;
			}
		}
		//check ob der Z-Level wirklich stimmt
		BRepAdaptor_Surface aAdaptor_Surface;
		aAdaptor_Surface.Initialize(aselectedFace);
		Standard_Real FirstUParameter, LastUParameter,FirstVParameter,LastVParameter;
		gp_Pnt first,second,third;
		FirstUParameter = aAdaptor_Surface.FirstUParameter();
		FirstVParameter = aAdaptor_Surface.FirstVParameter();
		aAdaptor_Surface.D0(FirstUParameter,FirstVParameter,first);
	}
			//std::string s1,s2;
			//s1 = aSelection.front().SubName;
			//string::size_type pos = s1.rfind('e');
			//s2 = s1.substr(pos+1,string::npos);
			//int i = atoi(s2.c_str());
}

void Cutting::on_CalculcateZLevel_clicked()
{
    //Cutting-Klasse instanzieren
    m_CuttingAlgo = new cutting_tools(m_Shape);
    toolpath_calculation_highest_level_button->setEnabled(true);
}

void Cutting::on_select_shape_z_level_button_clicked()
{
    selectShape();
}

void Cutting::on_select_shape_feature_based_button_clicked()
{
    selectShape();
}

void Cutting::on_toolpath_calculation_highest_level_button_clicked()
{
    //Do something
    toolpath_calculation_middle_level_button->setEnabled(true);
    toolpath_calculation_lowest_level_button->setEnabled(true);


    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view) {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, this);
        QMessageBox::information(this, tr("FreeCAD CamWorkbench"), tr("You have to pick a point.\n"));
        this->hide();
     }
}

void Cutting::on_toolpath_calculation_middle_level_button_clicked()
{
    //Do something

}

void Cutting::on_toolpath_calculation_lowest_level_button_clicked()
{
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
}

void Cutting::zLevelCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    Cutting* that = reinterpret_cast<Cutting*>(ud);

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        view->setEditing(false);
        view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, that);
        that->show();
    }
    else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
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
        if (!sh.IsNull()) {
            // ok a shape was picked
            n->setHandled();
            view->setEditing(false);
            view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), zLevelCallback, that);
            that->show();
            SbVec3f pt = point->getPoint();
            that->selectFace(sh, pt[0],pt[1],pt[2]);
        }
    }
}

#include "moc_Cutting.cpp"
