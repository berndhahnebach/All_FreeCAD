#ifndef _CAM_GUI_CUTTING_H
#define _CAM_GUI_CUTTING_H

#include "ui_Cutting.h"

namespace CamGui{

 class Cutting : public QDialog, public Ui_Cutting
 {
	 Q_OBJECT
 public:
  Cutting(QWidget* parent,Qt::WFlags = 0);
  ~Cutting();
 protected Q_SLOTS:
	void on_CalculcateZLevel_clicked();
	void on_toolpath_calculation_highest_level_button_clicked();
    void on_toolpath_calculation_middle_level_button_clicked();
    void on_toolpath_calculation_lowest_level_button_clicked();
	void on_toolpath_calculation_go_button_clicked();

 private:
	//Instanz von der cutting-klasse auf dem Heap erzeugen

 
 };

}
#endif