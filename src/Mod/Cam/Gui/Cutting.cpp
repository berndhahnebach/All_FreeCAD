#include "PreCompiled.h"

#include "Cutting.h"

#include <Mod/Cam/App/cutting_tools.h>
#include <Mod/Part/App/PartFeature.h>
#include <Base/Vector3D.h>
#include <Gui/ViewProvider.h>
#include <Gui/Selection.h>
#include <Gui/Application.h>
#include <App/Document.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <QTimer.h>

#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <Gui/Document.h>


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
                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        }
        else
        {
            check_box2 = QMessageBox::information(this, tr("FreeCAD CamWorkbench"),
                                                  tr("You have to select a CAD-Shape.\n"),
                                                  QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
        }
        if ((check_box1 == QMessageBox::Yes) | (check_box2 == QMessageBox::Ok))
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


void Cutting::selectFace()
{
	//check if a Shape is selected
	std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Part::Feature::getClassTypeId());
    if ( fea.size() == 1)
	{
		//get Hash Code of Selected Face inside the selected Shape and also the Coordinates of the click
		TopoDS_Shape aShape;
		if(aShape.ShapeType() != TopAbs_FACE)
			return;

		TopoDS_Face tempFace = TopoDS::Face(//Selected Face in the viewer);
		Base::Vector3f clickPoint = //Where did we click??
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

#include "moc_Cutting.cpp"
