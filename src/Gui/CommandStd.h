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

#include "../Base/Export.h"

#include "Command.h"


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



#endif