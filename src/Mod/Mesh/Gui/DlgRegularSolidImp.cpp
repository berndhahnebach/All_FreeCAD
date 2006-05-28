/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qcheckbox.h>
# include <qcombobox.h>
# include <qmessagebox.h>
#endif

#include <Base/PyExportImp.h>
#include <Base/Interpreter.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/PrefWidgets.h>

#include "DlgRegularSolidImp.h"

using namespace MeshGui;

/**
 *  Constructs a MeshGui::DlgRegularSolidImp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MeshGui::DlgRegularSolidImp::DlgRegularSolidImp( QWidget* parent, const char* name, bool modal, WFlags fl )
 : DlgRegularSolid( parent, name, modal, fl )
{
  Gui::Application::Instance->activeDocument()->openCommand("Build regular solid");
  Gui::Command::doCommand(Gui::Command::Doc, "import Mesh,BuildRegularGeoms");
}

/**
 *  Destroys the object and frees any allocated resources
 */
MeshGui::DlgRegularSolidImp::~DlgRegularSolidImp()
{
  // no need to delete child widgets, Qt does it all for us
  Gui::Application::Instance->activeDocument()->commitCommand();
}

/**
 * Builds a mesh solid from the currently active solid type.
 */
void MeshGui::DlgRegularSolidImp::buildSolid()
{
  try {
    QString cmd;
    if ( comboBox1->currentItem() == 0 ) {         // cube
      App::Document* doc = App::GetApplication().getActiveDocument();
      std::string name = doc->getUniqueObjectName("Cube");
      cmd.sprintf(
          "App.activeDocument().addObject(\"Mesh::Cube\",\"%s\")\n"
          "App.activeDocument().%s.Length=%.2f\n"
          "App.activeDocument().%s.Width=%.2f\n"
          "App.activeDocument().%s.Height=%.2f\n"
          ,name.c_str(),name.c_str(),boxLength->value(),
           name.c_str(),boxWidth->value(),
           name.c_str(),boxHeight->value());
    } else if ( comboBox1->currentItem() == 1 ) {  // cylinder
      App::Document* doc = App::GetApplication().getActiveDocument();
      std::string name = doc->getUniqueObjectName("Cylinder");
      cmd.sprintf(
          "App.activeDocument().addObject(\"Mesh::Cylinder\",\"%s\")\n"
          "App.activeDocument().%s.Radius=%.2f\n"
          "App.activeDocument().%s.Length=%.2f\n"
          "App.activeDocument().%s.EdgeLength=%.2f\n"
          "App.activeDocument().%s.Closed=%s\n"
          "App.activeDocument().%s.Sampling=%d\n"
          ,name.c_str(),name.c_str(),cylinderRadius->value(),
           name.c_str(),cylinderLength->value(),
           name.c_str(),cylinderEdgeLength->value(),
           name.c_str(),(cylinderClosed->isChecked()?"True":"False"),
           name.c_str(),cylinderCount->value());
    } else if ( comboBox1->currentItem() == 2 ) {  // cone
      App::Document* doc = App::GetApplication().getActiveDocument();
      std::string name = doc->getUniqueObjectName("Cone");
      cmd.sprintf(
          "App.activeDocument().addObject(\"Mesh::Cone\",\"%s\")\n"
          "App.activeDocument().%s.Radius1=%.2f\n"
          "App.activeDocument().%s.Radius2=%.2f\n"
          "App.activeDocument().%s.Length=%.2f\n"
          "App.activeDocument().%s.EdgeLength=%.2f\n"
          "App.activeDocument().%s.Closed=%s\n"
          "App.activeDocument().%s.Sampling=%d\n"
          ,name.c_str(),name.c_str(),coneRadius1->value(),
           name.c_str(),coneRadius2->value(),
           name.c_str(),coneLength->value(),
           name.c_str(),coneEdgeLength->value(),
           name.c_str(),(coneClosed->isChecked()?"True":"False"),
           name.c_str(),coneCount->value());
    } else if ( comboBox1->currentItem() == 3 ) {  // sphere
      App::Document* doc = App::GetApplication().getActiveDocument();
      std::string name = doc->getUniqueObjectName("Sphere");
      cmd.sprintf(
          "App.activeDocument().addObject(\"Mesh::Sphere\",\"%s\")\n"
          "App.activeDocument().%s.Radius=%.2f\n"
          "App.activeDocument().%s.Sampling=%d\n"
          ,name.c_str(),name.c_str(),sphereRadius->value(),
           name.c_str(),sphereCount->value());
    } else if ( comboBox1->currentItem() == 4 ) {  // ellipsoid
      App::Document* doc = App::GetApplication().getActiveDocument();
      std::string name = doc->getUniqueObjectName("Ellipsoid");
      cmd.sprintf(
          "App.activeDocument().addObject(\"Mesh::Ellipsoid\",\"%s\")\n"
          "App.activeDocument().%s.Radius1=%.2f\n"
          "App.activeDocument().%s.Radius2=%.2f\n"
          "App.activeDocument().%s.Sampling=%d\n"
          ,name.c_str(),name.c_str(),ellipsoidRadius1->value(),
           name.c_str(),ellipsoidRadius2->value(),
           name.c_str(),ellipsoidCount->value());
    } else if ( comboBox1->currentItem() == 5 ) {  // toroid
      App::Document* doc = App::GetApplication().getActiveDocument();
      std::string name = doc->getUniqueObjectName("Torus");
      cmd.sprintf(
          "App.activeDocument().addObject(\"Mesh::Torus\",\"%s\")\n"
          "App.activeDocument().%s.Radius1=%.2f\n"
          "App.activeDocument().%s.Radius2=%.2f\n"
          "App.activeDocument().%s.Sampling=%d\n"
          ,name.c_str(),name.c_str(),toroidRadius1->value(),
           name.c_str(),toroidRadius2->value(),
           name.c_str(),toroidCount->value());
    }

    // Execute the Python block
    Gui::Command::doCommand(Gui::Command::Doc, cmd.ascii());
    Gui::Application::Instance->activeDocument()->getDocument()->recompute();
  } catch (const Base::PyException& e) {
    QMessageBox::warning(this, tr("Building solid"), e.what());
  }
}

// -------------------------------------------------------------

SingleDlgRegularSolidImp* SingleDlgRegularSolidImp::_instance=0;

SingleDlgRegularSolidImp* SingleDlgRegularSolidImp::instance()
{
  // not initialized?
  if(!_instance)
  {
    _instance = new SingleDlgRegularSolidImp( Gui::getMainWindow(), "Regular solid",Qt::WDestructiveClose);
  }

  return _instance;
}

void SingleDlgRegularSolidImp::destruct ()
{
  if ( _instance != 0 )
  {
    SingleDlgRegularSolidImp *pTmp = _instance;
    _instance = 0;
    delete pTmp;
  }
}

bool SingleDlgRegularSolidImp::hasInstance()
{
  return _instance != 0;
}

/**
 *  Constructs a SingleDlgRegularSolidImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
SingleDlgRegularSolidImp::SingleDlgRegularSolidImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgRegularSolidImp( parent, name, false, fl )
{
}

/**
 *  Destroys the object and frees any allocated resources
 */
SingleDlgRegularSolidImp::~SingleDlgRegularSolidImp()
{
  _instance = 0;
}

#include "DlgRegularSolid.cpp"
#include "moc_DlgRegularSolid.cpp"

