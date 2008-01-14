#include "PreCompiled.h"

#include "Cutting.h"

using namespace CamGui;

Cutting::Cutting(QWidget* parent,Qt::WFlags fl)
:QDialog(parent,fl)  
{
	this->setupUi(this);
}

Cutting::~Cutting()
{

}


void Cutting::on_CalculcateZLevel_clicked()
{
	//Cutting-Klasse instanzieren
	toolpath_calculation_highest_level_button->setEnabled(true);
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
	toolpath_calculation_go_button->setEnabled(true);

}

#include "moc_cutting.cpp"