#include "PreCompiled.h"

#include "Cutting.h"

#include <Gui/Command.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>




//===========================================================================
// StdCamCutting
//===========================================================================
DEF_STD_CMD(StdCamCutting);

StdCamCutting::StdCamCutting()
        :Command("Cam_Cutting")
{
    sGroup        = QT_TR_NOOP("Tools");
    sMenuText     = QT_TR_NOOP("Cutting...");
    sToolTipText  = QT_TR_NOOP("Cutting");
    sWhatsThis    = QT_TR_NOOP("Cutting");
    sStatusTip    = QT_TR_NOOP("Cutting");
    iAccel        = 0;
}

void StdCamCutting::activated(int iMsg)
{
    static QPointer<QDialog> dlg = 0;
    if (!dlg)
        dlg = new CamGui::Cutting(Gui::getMainWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void CreateCamCommands()
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new StdCamCutting());
}
