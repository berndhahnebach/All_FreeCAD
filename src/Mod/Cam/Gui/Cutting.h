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

#ifndef CAMGUI_CUTTING_H
#define CAMGUI_CUTTING_H

#include "ui_Cutting.h"

#include <Base/Vector3D.h>
#include <QProcess>

class cutting_tools;
class path_simulate;
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
    void on_CalculateZLevel_clicked();
    void on_CalculateFeatureBased_clicked();
    void on_CalculateSpiralBased_clicked();
    void on_select_shape_z_level_button_clicked();
    void on_select_shape_feature_based_button_clicked();
    void on_select_shape_spiral_based_button_clicked();
    void on_toolpath_calculation_highest_level_button_clicked();
    void on_toolpath_calculation_middle_level_button_clicked();
    void on_toolpath_calculation_lowest_level_button_clicked();
    void on_toolpath_calculation_go_button_clicked();
    void on_GenSimOut_clicked();
    void on_adaptdynainput_clicked();
    void on_start_simulation_clicked();
    void selectShape();
    bool getProcessOutput();
    void setFace(const TopoDS_Shape &aFace, const float , const float,const float);

private:
    static void zLevelCallback(void * ud, SoEventCallback * n);
    void DisplayCAMOutput();


private:
    //Instanz von der cutting-klasse auf dem Heap erzeugen
    cutting_tools *m_CuttingAlgo;
    path_simulate *m_PathSimulate;
    QProcess *m_Process;
    TopoDS_Shape m_Shape;
    bool m_timer;
    //1 means Standard, 2 means Feature Based, 3 means Spiral Based
    unsigned int m_Mode;

};

}

#endif
