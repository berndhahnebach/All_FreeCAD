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
# if defined (_POSIX_C_SOURCE)
#   undef  _POSIX_C_SOURCE
# endif // (re-)defined in pyconfig.h
# include <Python.h>
# include <qapplication.h>
# include <qstatusbar.h>
# include <qfiledialog.h>
# include <qobjectlist.h>
# include <qmessagebox.h>
# include <qprinter.h>
# include <qtimer.h>
#endif

#ifdef FC_USE_OCAFBROWSER
# include <DebugBrowser.hxx>
#endif

#include "../Base/Exception.h"
#include "../Base/Interpreter.h"
#include "../Base/Sequencer.h"
#include "../App/Document.h"
#include "Action.h"
#include "Process.h"
#include "Application.h"
#include "Document.h"
#include "Splashscreen.h"
#include "Command.h"
#include "MainWindow.h"
#include "WhatsThis.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"
#include "View.h"

//#include "DlgDocTemplatesImp.h"
#include "DlgParameterImp.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
#include "Macro.h"
#include "DlgPreferencesImp.h"
#include "DlgCustomizeImp.h"
#include "Widgets.h"
#include "NetworkRetriever.h"
#include "GuiConsole.h"

using Base::Console;
using Base::Sequencer;
using namespace Gui;



//===========================================================================
// Std_Recompute
//===========================================================================

DEF_STD_CMD(StdCmdFeatRecompute);

StdCmdFeatRecompute::StdCmdFeatRecompute()
  :Command("Std_Recompute")
{
  // seting the
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Recompute");
  sToolTipText  = QT_TR_NOOP("Recompute feature or document");
  sWhatsThis    = QT_TR_NOOP("Recompute feature or document");
  sStatusTip    = QT_TR_NOOP("Recompute feature or document");
  sPixmap       = "Open";
  iAccel        = Qt::CTRL+Qt::Key_R;
}

void StdCmdFeatRecompute::activated(int iMsg)
{
  // fill the list of registered endings

}


namespace Gui {

void CreateFeatCommands(void)
{
  CommandManager &rcCmdMgr = Application::Instance->commandManager();

  rcCmdMgr.addCommand(new StdCmdFeatRecompute());


}

} // namespace Gui
