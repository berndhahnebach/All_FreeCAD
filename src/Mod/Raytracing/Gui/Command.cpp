/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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
# include <qaction.h>
# include <BRepPrimAPI_MakeBox.hxx>
# include <TopoDS_Shape.hxx>
# include <TNaming_Builder.hxx>
# include <AIS_InteractiveContext.hxx>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>
#include <Gui/View.h>


#include <Gui/Icons/images.cpp>


#include <Inventor/SoInput.h>
//#include <Inventor/SoCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

  

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdRaytracingTest THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingWriteCamera);

CmdRaytracingWriteCamera::CmdRaytracingWriteCamera()
  :CppCommand("Raytracing_WriteCamera")
{
  sAppModule    = "Raytracing";
  sGroup        = "Raytracing";
  sMenuText     = "Write camera position";
  sToolTipText  = "Write the camera positon of the active 3D view in PovRay format to a file";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingWriteCamera::activated(int iMsg)
{
  const char* ppReturn=0;

  getAppWnd()->sendMsgToActiveView("GetCamera",&ppReturn);

  Base::Console().Log("GetCamera MSG send:\n%s",ppReturn);

  SoInput in;
  in.setBuffer((void*)ppReturn,strlen(ppReturn));

  //if (!in.openFile(filename)) { exit(1); }

  SoCamera * Cam;
  SoDB::read(&in,(SoNode*&)Cam);

  if (!Cam)
    throw Base::Exception("CmdRaytracingWriteCamera::activated(): Could not read Camera information from ASCII stream....\n");

  // root-node returned from SoDB::readAll() has initial zero
  // ref-count, so reference it before we start using it to
  // avoid premature destruction.
  Cam->ref();

  SbRotation camrot = Cam->orientation.getValue();

  SbVec3f upvec(0, 1, 0); // init to default up vector
  camrot.multVec(upvec, upvec);

  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  camrot.multVec(lookat, lookat);

  //float Dist = Cam->focalDistance.getValue();
  //lookat *= Dist;

  SbVec3f pos = Cam->position.getValue();

  std::stringstream out;
  out << "camera {\n"
      << "  location  <" << pos.getValue()[0]    <<"," << pos.getValue()[2]    <<"," << pos.getValue()[1]    <<">\n" 
      << "  direction <" << lookat.getValue()[0] <<"," << lookat.getValue()[2] <<"," << lookat.getValue()[1] <<">\n" 
      << "  up        <" << upvec.getValue()[0]  <<"," << upvec.getValue()[2]  <<"," << upvec.getValue()[1]  <<">\n" 
//      << "  right     x*image_width/image_height\n"
      << "}\n";

  Base::Console().Log("Pov Camera out:\n%s",out.str().c_str());
//camera {
//    location <2300,650,-3000.1032> 
//    location <600+clock,650,-3000.1032> 
//    right     x*image_width/image_height
//    look_at  center
//    sky y
//    angle 35
//}


  // Bring ref-count of root-node back to zero to cause the
  // destruction of the scene.
  Cam->unref();
  // (Upon self-destructing, the root-node will also de-reference
  // it's children nodes, so they also self-destruct, and so on
  // recursively down the scenegraph hierarchy until the complete
  // scenegraph has self-destructed and thereby returned all
  // memory resources it was using.)



/*
  QString fn = Gui::FileDialog::getSaveFileName( QString::null, "POV include (*.inc);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
	if (! fn.isEmpty() )
	{
    Base::Console().Log("File name: %s",fn.latin1());
	}
*/
}

bool CmdRaytracingWriteCamera::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("GetCamera");
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdRaytracingWriteCamera());
}
