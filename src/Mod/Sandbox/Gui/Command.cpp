/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <QObject>
# include <QEventLoop>
# include <QTimer>
#endif

#include <Base/Console.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>

#include <Mod/Sandbox/App/DocumentThread.h>
#include <Mod/Sandbox/App/DocumentProtector.h>


DEF_STD_CMD(CmdSandboxThread1);

CmdSandboxThread1::CmdSandboxThread1()
  :Command("Sandbox_Thread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Run several threads");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool1";
}

void CmdSandboxThread1::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    for (int i=0; i<5; i++) {
        Sandbox::DocumentThread* dt = new Sandbox::DocumentThread();
        dt->setObjectName(QString::fromAscii("MyMesh_%1").arg(i));
        QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
        dt->start();
    }
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread2);

CmdSandboxThread2::CmdSandboxThread2()
  :Command("Sandbox_SeqThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Thread and sequencer");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool2";
}

void CmdSandboxThread2::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    Sandbox::DocumentThread* dt = new Sandbox::DocumentThread();
    dt->setObjectName(QString::fromAscii("MyMesh"));
    QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
    dt->start();
#ifdef FC_DEBUG
    int max = 10000;
#else
    int max = 100000000;
#endif
    Base::SequencerLauncher seq("Do something meaningful...", max);
    double val=0;
    for (int i=0; i<max; i++) {
        for (int j=0; j<max; j++) {
            val = sin(0.12345);
        }
        seq.next(true);
    }
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread3);

CmdSandboxThread3::CmdSandboxThread3()
  :Command("Sandbox_BlockThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Thread and no sequencer");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool3";
}

void CmdSandboxThread3::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    Sandbox::DocumentThread* dt = new Sandbox::DocumentThread();
    dt->setObjectName(QString::fromAscii("MyMesh"));
    QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
    dt->start();
#ifdef FC_DEBUG
    int max = 10000;
#else
    int max = 100000000;
#endif
    double val=0;
    for (int i=0; i<max; i++) {
        for (int j=0; j<max; j++) {
            val = sin(0.12345);
        }
    }
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread4);

CmdSandboxThread4::CmdSandboxThread4()
  :Command("Sandbox_NoThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("GUI thread");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool4";
}

void CmdSandboxThread4::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    App::Document* doc = App::GetApplication().getActiveDocument();
    Sandbox::DocumentProtector dp(doc);
    App::DocumentObject* obj = dp.addObject("Mesh::Cube", "MyCube");
    dp.recompute();
    App::GetApplication().closeDocument("Thread");
    // this forces an exception
    App::DocumentObject* obj2 = dp.addObject("Mesh::Cube", "MyCube");
    dp.recompute();
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread5);

CmdSandboxThread5::CmdSandboxThread5()
  :Command("Sandbox_WorkerThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Worker thread");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool1";
}

void CmdSandboxThread5::activated(int iMsg)
{
    Sandbox::WorkerThread* wt = new Sandbox::WorkerThread();
    QObject::connect(wt, SIGNAL(finished()), wt, SLOT(deleteLater()));
    wt->start();
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread6);

CmdSandboxThread6::CmdSandboxThread6()
  :Command("Sandbox_Python")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Python thread");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool6";
}

void CmdSandboxThread6::activated(int iMsg)
{
#if 0 // deadlock???
    doCommand(Doc,
        "import thread, time, Sandbox\n"
        "def adder(doc):\n"
        "    lock.acquire()\n"
        "    dp=Sandbox.DocumentProtector(doc)\n"
        "    dp.addObject(\"Mesh::Ellipsoid\",\"Mesh\")\n"
        "    dp.recompute()\n"
        "    lock.release()\n"
        "\n"
        "lock=thread.allocate_lock()\n"
        "doc=App.newDocument()\n"
        "for i in range(1):\n"
        "    thread.start_new(adder,(doc,))\n"
        "\n"
        "time.sleep(1)\n"
    );
    doCommand(Doc,
        "from PyQt4 import QtCore; import Sandbox\n"
        "class Thread(QtCore.QThread):\n"
        "    def run(self):\n"
        "        dp=Sandbox.DocumentProtector(doc)\n"
        "        dp.addObject(\"Mesh::Ellipsoid\",\"Mesh\")\n"
        "        dp.recompute()\n"
        "\n"
        "doc=App.newDocument()\n"
        "threads=[]\n"
        "for i in range(2):\n"
        "    thread=Thread()\n"
        "    threads.append(thread)\n"
        "    thread.start()\n"
        "\n"
    );
#endif
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread7);

CmdSandboxThread7::CmdSandboxThread7()
  :Command("Sandbox_Dialog")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Thread and modal dialog");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool7";
}

void CmdSandboxThread7::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    Sandbox::DocumentThread* dt = new Sandbox::DocumentThread();
    dt->setObjectName(QString::fromAscii("MyMesh"));
    QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
    dt->start();
    //QFileDialog::getOpenFileName();
    QColorDialog::getColor(Qt::white,Gui::getMainWindow());
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxThread8);

CmdSandboxThread8::CmdSandboxThread8()
  :Command("Sandbox_FileDialog")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Thread and file dialog");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool7";
}

void CmdSandboxThread8::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    Sandbox::DocumentThread* dt = new Sandbox::DocumentThread();
    dt->setObjectName(QString::fromAscii("MyMesh"));
    QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
    dt->start();
    QFileDialog::getOpenFileName();
}

// -------------------------------------------------------------------------------

class CmdSandboxEventLoop : public Gui::Command
{
public:
    CmdSandboxEventLoop();
protected:
    void activated(int iMsg);
    bool isActive(void);
private:
    QEventLoop loop;
};

CmdSandboxEventLoop::CmdSandboxEventLoop()
  :Command("Sandbox_EventLoop")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Local event loop");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool6";
}

void CmdSandboxEventLoop::activated(int iMsg)
{
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    timer.start(5000); // 5s timeout
    loop.exec();
    Base::Console().Message("CmdSandboxEventLoop: timeout\n");
}

bool CmdSandboxEventLoop::isActive(void)
{
    return (!loop.isRunning());
}

void CreateSandboxCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdSandboxThread1());
    rcCmdMgr.addCommand(new CmdSandboxThread2());
    rcCmdMgr.addCommand(new CmdSandboxThread3());
    rcCmdMgr.addCommand(new CmdSandboxThread4());
    rcCmdMgr.addCommand(new CmdSandboxThread5());
    rcCmdMgr.addCommand(new CmdSandboxThread6());
    rcCmdMgr.addCommand(new CmdSandboxThread7());
    rcCmdMgr.addCommand(new CmdSandboxThread8());
    rcCmdMgr.addCommand(new CmdSandboxEventLoop);
}
