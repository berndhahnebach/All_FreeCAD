#ifndef CAMGUI_CUTTING_H
#define CAMGUI_CUTTING_H

#include "ui_Cutting.h"
#include <Base/Vector3D.h>

class cutting_tools;
class TopoDS_Shape;

namespace CamGui
{

class Cutting : public QDialog, public Ui_Cutting
{
    Q_OBJECT

public:
    Cutting(QWidget* parent,Qt::WFlags = 0);
    ~Cutting();
protected Q_SLOTS:
    void on_CalculcateZLevel_clicked();
    void on_select_shape_z_level_button_clicked();
    void on_select_shape_feature_based_button_clicked();
    void on_toolpath_calculation_highest_level_button_clicked();
    void on_toolpath_calculation_middle_level_button_clicked();
    void on_toolpath_calculation_lowest_level_button_clicked();
    void on_toolpath_calculation_go_button_clicked();
    void selectShape();
    void setFace(const TopoDS_Shape &aFace, const float , const float,const float);

private:
    static void zLevelCallback(void * ud, SoEventCallback * n);
	void DisplayCAMOutput();

private:
    //Instanz von der cutting-klasse auf dem Heap erzeugen
    cutting_tools *m_CuttingAlgo;

    TopoDS_Shape m_Shape;
    bool m_timer;
};

}

#endif
