/** \file $RCSfile$
 *  \brief The console module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
#ifndef __CommandStd_h__
#define __CommandStd_h__


#include "Command.h"
//#ifdef __linux
#  include "qstatusbar.h" //invalid use of type `class QStratusBar'
#  include "qfiledlg.h" //QFileDialog undeclared
#  include "qprinter.h" //aggregate QPrinter has incomplete type and cannot be initialized
//#endif


class FCCmdOpen: public FCCommand
{
public:
	FCCmdOpen():FCCommand("Std_Open"){}
	virtual void CmdProfile(char** sMenuText, 
		                    char** sToolTipText, 
							char** sWhatsThis, 
							char** sStatusTip, 
							QPixmap &cPixmap, 
							int &iAccel);
	virtual void Activated(void);
	//virtual void Toogled  (bool);
};


DEF_STD_CMD(FCCmdNew   ,"Std_New");
DEF_STD_CMD(FCCmdSave  ,"Std_Save");
DEF_STD_CMD(FCCmdSaveAs,"Std_SaveAs");
DEF_STD_CMD(FCCmdPrint ,"Std_Print");
DEF_STD_CMD(FCCmdUndo  ,"Std_Undo");
DEF_STD_CMD(FCCmdRedo  ,"Std_Redo");
DEF_STD_CMD(FCCmdCut   ,"Std_Cut");
DEF_STD_CMD(FCCmdCopy  ,"Std_Copy");
DEF_STD_CMD(FCCmdPaste ,"Std_Paste");


#endif