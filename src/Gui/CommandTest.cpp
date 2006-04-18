/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <qwaitcondition.h>
#endif

#include "../Base/Exception.h"
#include "../Base/Interpreter.h"
#include "../App/Document.h"
#include "../App/Feature.h"

#include "Application.h"
#include "MainWindow.h"
#include "Document.h"
#include "Command.h"

#include "Inventor/Qt/viewers/SoQtExaminerViewer.h"
#include "Inventor/Qt/SoQt.h"

#include "FileDialog.h"
#include "ProgressBar.h"
#include "Macro.h"


using namespace Gui;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Std_Test1
//===========================================================================
DEF_STD_CMD_A(FCCmdTest1);

FCCmdTest1::FCCmdTest1()
  :Command("Std_Test1")
{
  sGroup        = "Standard-Test";
  sMenuText     = "Test1";
  sToolTipText  = "Test function 1";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool1";
  iAccel        = Qt::CTRL+Qt::Key_T;
}

void FCCmdTest1::activated(int iMsg)
{

}

bool FCCmdTest1::isActive(void)
{
  //return (GetActiveOCCDocument()!=NULL);
  return true;
}

//===========================================================================
// Std_Test2
//===========================================================================
DEF_STD_CMD_A(FCCmdTest2);

FCCmdTest2::FCCmdTest2()
	:Command("Std_Test2")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Test2";
	sToolTipText	= "Test function 2";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool2";
	iAccel			= 0;
}


void FCCmdTest2::activated(int iMsg)
{


}

bool FCCmdTest2::isActive(void)
{
  return (getDocument()!=NULL);
}

//===========================================================================
// Std_Test3
//===========================================================================
DEF_STD_CMD_A(FCCmdTest3);

FCCmdTest3::FCCmdTest3()
	:Command("Std_Test3")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Test3";
	sToolTipText	= "Test function 3";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool3";
	iAccel			= 0;
}

void FCCmdTest3::activated(int iMsg)
{

  App::Document *pcDoc = getDocument();
	if(!pcDoc) return;

}


bool FCCmdTest3::isActive(void)
{
  return (getDocument()!=NULL);
}

//===========================================================================
// Std_Test4
//===========================================================================

DEF_STD_CMD_A(FCCmdTest4);

FCCmdTest4::FCCmdTest4()
	:Command("Std_Test4")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Test4";
	sToolTipText	= "Test function 4";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool4";
	iAccel			= 0;
}

void FCCmdTest4::activated(int iMsg)
{

  App::Document *pcDoc = getDocument();
	if(!pcDoc) return;

}


bool FCCmdTest4::isActive(void)
{
  return (getDocument()!=NULL);
}

//===========================================================================
// Std_Test5
//===========================================================================
DEF_STD_CMD_A(FCCmdTest5);

FCCmdTest5::FCCmdTest5()
	:Command("Std_Test5")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Test5";
	sToolTipText	= "Test function 5";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool5";
	iAccel			= 0;
}

void FCCmdTest5::activated(int iMsg)
{

  App::Document *pcDoc = getDocument();
	if(!pcDoc) return;

}


bool FCCmdTest5::isActive(void)
{
  return (getDocument()!=NULL);
}


//===========================================================================
// Std_Test6
//===========================================================================
DEF_STD_CMD_A(FCCmdTest6);

FCCmdTest6::FCCmdTest6()
	:Command("Std_Test6")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Test6";
	sToolTipText	= "Test function 6";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool6";
	iAccel			= 0;
}

void FCCmdTest6::activated(int iMsg)
{

  App::Document *pcDoc = getDocument();
	if(!pcDoc) return;

}


bool FCCmdTest6::isActive(void)
{
  return (getDocument()!=NULL);
}


//===========================================================================
// Std_TestProgress1
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress1);

CmdTestProgress1::CmdTestProgress1()
	:Command("Std_TestProgress1")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Breakable bar";
	sToolTipText	= "Test a breakable progress bar";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool7";
	iAccel			= 0;
}

void CmdTestProgress1::activated(int iMsg)
{
	try
	{
		unsigned long steps = 1000;
		Base::Sequencer().start("Starting progress bar", steps);
		
		for (unsigned long i=0; i<steps;i++)
		{
			Base::Sequencer().next(true);
			QWaitCondition().wait(30);
		}

		Base::Sequencer().stop();
	}
	catch (...)
	{
	}
}

bool CmdTestProgress1::isActive(void)
{
	return ( !Base::Sequencer().isRunning() );
}

//===========================================================================
// Std_TestProgress2
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress2);

CmdTestProgress2::CmdTestProgress2()
	:Command("Std_TestProgress2")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Unbreakable bar";
	sToolTipText	= "Test a unbreakable progress bar";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool7";
	iAccel			= 0;
}

void CmdTestProgress2::activated(int iMsg)
{
	try
	{
		unsigned long steps = 1000;
		Base::Sequencer().start("Starting progress bar", steps);
		
		for (unsigned long i=0; i<steps;i++)
		{
			Base::Sequencer().next(false);
			QWaitCondition().wait(10);
		}

		Base::Sequencer().stop();
	}
	catch (...)
	{
	}
}

bool CmdTestProgress2::isActive(void)
{
	return ( !Base::Sequencer().isRunning() );
}

//===========================================================================
// Std_TestProgress3
//===========================================================================
DEF_STD_CMD_A(CmdTestProgress3);

CmdTestProgress3::CmdTestProgress3()
	:Command("Std_TestProgress3")
{
	sGroup			= "Standard-Test";
	sMenuText		= "Nested progress bar";
	sToolTipText	= "Test nested progress bar";
	sWhatsThis		= sToolTipText;
	sStatusTip		= sToolTipText;
	sPixmap			= "Std_Tool8";
	iAccel			= 0;
}

void CmdTestProgress3::activated(int iMsg)
{
	try
	{
    // level 1
		unsigned long steps = 5;
		Base::Sequencer().start("Starting progress bar", steps);
		for (unsigned long i=0; i<steps;i++)
		{
      QWaitCondition().wait(200);
			Base::Sequencer().next(true);

      // level 2
      unsigned long steps = 6;
      Base::Sequencer().start("Starting progress bar", steps);
      for (unsigned long j=0; j<steps;j++)
      {
        QWaitCondition().wait(150);
        Base::Sequencer().next(true);

        // level 3
        unsigned long steps = 7;
        Base::Sequencer().start("Starting progress bar", steps);
        for (unsigned long k=0; k<steps;k++)
        {
          QWaitCondition().wait(100);
          Base::Sequencer().next(true);

          // level 4
          unsigned long steps = 8;
          Base::Sequencer().start("Starting progress bar", steps);
          for (unsigned long l=0; l<steps;l++)
          {
            QWaitCondition().wait(5);
            Base::Sequencer().next(true);
          }
          Base::Sequencer().stop();
        }
        Base::Sequencer().stop();
      }
      Base::Sequencer().stop();
		}
		Base::Sequencer().stop();
	}
	catch (...)
	{
	}
}


bool CmdTestProgress3::isActive(void)
{
	return ( !Base::Sequencer().isRunning() );
}


namespace Gui {

void CreateTestCommands(void)
{
	CommandManager &rcCmdMgr = Application::Instance->commandManager();

	rcCmdMgr.addCommand(new FCCmdTest1());
	rcCmdMgr.addCommand(new FCCmdTest2());
	rcCmdMgr.addCommand(new FCCmdTest3());
	rcCmdMgr.addCommand(new FCCmdTest4());
	rcCmdMgr.addCommand(new FCCmdTest5());
	rcCmdMgr.addCommand(new FCCmdTest6());
	rcCmdMgr.addCommand(new CmdTestProgress1());
	rcCmdMgr.addCommand(new CmdTestProgress2());
  rcCmdMgr.addCommand(new CmdTestProgress3());
}

} // namespace Gui



