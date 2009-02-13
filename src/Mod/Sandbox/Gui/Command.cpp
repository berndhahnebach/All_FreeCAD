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
#include <Gui/FileDialog.h>

#include <Mod/Sandbox/App/DocumentThread.h>
#include <Mod/Sandbox/App/DocumentProtector.h>
#include <Mod/Mesh/App/MeshFeature.h>


DEF_STD_CMD(CmdSandboxDocumentThread);

CmdSandboxDocumentThread::CmdSandboxDocumentThread()
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

void CmdSandboxDocumentThread::activated(int iMsg)
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

DEF_STD_CMD(CmdSandboxDocThreadWithSeq);

CmdSandboxDocThreadWithSeq::CmdSandboxDocThreadWithSeq()
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

void CmdSandboxDocThreadWithSeq::activated(int iMsg)
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

DEF_STD_CMD(CmdSandboxDocThreadBusy);

CmdSandboxDocThreadBusy::CmdSandboxDocThreadBusy()
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

void CmdSandboxDocThreadBusy::activated(int iMsg)
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

DEF_STD_CMD(CmdSandboxDocumentNoThread);

CmdSandboxDocumentNoThread::CmdSandboxDocumentNoThread()
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

void CmdSandboxDocumentNoThread::activated(int iMsg)
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

DEF_STD_CMD(CmdSandboxWorkerThread);

CmdSandboxWorkerThread::CmdSandboxWorkerThread()
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

void CmdSandboxWorkerThread::activated(int iMsg)
{
    Sandbox::WorkerThread* wt = new Sandbox::WorkerThread();
    QObject::connect(wt, SIGNAL(finished()), wt, SLOT(deleteLater()));
    wt->start();
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxPythonLockThread);

CmdSandboxPythonLockThread::CmdSandboxPythonLockThread()
  :Command("Sandbox_PythonLockThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Locked Python threads");
    sToolTipText  = QT_TR_NOOP("Use Python's thread module where each thread is locked");
    sWhatsThis    = QT_TR_NOOP("Use Python's thread module where each thread is locked");
    sStatusTip    = QT_TR_NOOP("Use Python's thread module where each thread is locked");
}

void CmdSandboxPythonLockThread::activated(int iMsg)
{
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
        "for i in range(2):\n"
        "    thread.start_new(adder,(doc,))\n"
        "\n"
        "time.sleep(1)\n"
    );
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxPythonNolockThread);

CmdSandboxPythonNolockThread::CmdSandboxPythonNolockThread()
  :Command("Sandbox_NolockPython")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Unlocked Python threads");
    sToolTipText  = QT_TR_NOOP("Use Python's thread module where each thread is unlocked");
    sWhatsThis    = QT_TR_NOOP("Use Python's thread module where each thread is unlocked");
    sStatusTip    = QT_TR_NOOP("Use Python's thread module where each thread is unlocked");
}

void CmdSandboxPythonNolockThread::activated(int iMsg)
{
    doCommand(Doc,
        "import thread, time, Sandbox\n"
        "def adder(doc):\n"
        "    dp=Sandbox.DocumentProtector(doc)\n"
        "    dp.addObject(\"Mesh::Ellipsoid\",\"Mesh\")\n"
        "    dp.recompute()\n"
        "\n"
        "doc=App.newDocument()\n"
        "for i in range(2):\n"
        "    thread.start_new(adder,(doc,))\n"
        "\n"
        "time.sleep(1)\n"
    );
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxPyQtThread);

CmdSandboxPyQtThread::CmdSandboxPyQtThread()
  :Command("Sandbox_PyQtThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("PyQt threads");
    sToolTipText  = QT_TR_NOOP("Use PyQt's thread module");
    sWhatsThis    = QT_TR_NOOP("Use PyQt's thread module");
    sStatusTip    = QT_TR_NOOP("Use PyQt's thread module");
}

void CmdSandboxPyQtThread::activated(int iMsg)
{
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
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxPythonThread);

CmdSandboxPythonThread::CmdSandboxPythonThread()
  :Command("Sandbox_PythonThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Python threads");
    sToolTipText  = QT_TR_NOOP("Use class PythonThread running Python code in its run() method");
    sWhatsThis    = QT_TR_NOOP("Use class PythonThread running Python code in its run() method");
    sStatusTip    = QT_TR_NOOP("Use class PythonThread running Python code in its run() method");
}

void CmdSandboxPythonThread::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    for (int i=0; i<5; i++) {
        Sandbox::PythonThread* pt = new Sandbox::PythonThread();
        pt->setObjectName(QString::fromAscii("MyMesh_%1").arg(i));
        QObject::connect(pt, SIGNAL(finished()), pt, SLOT(deleteLater()));
        pt->start();
    }
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxPythonMainThread);

CmdSandboxPythonMainThread::CmdSandboxPythonMainThread()
  :Command("Sandbox_PythonMainThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Python main thread");
    sToolTipText  = QT_TR_NOOP("Run python code in main thread");
    sWhatsThis    = QT_TR_NOOP("Run python code in main thread");
    sStatusTip    = QT_TR_NOOP("Run python code in main thread");
}

void CmdSandboxPythonMainThread::activated(int iMsg)
{
    doCommand(Doc,
        "import Sandbox\n"
        "doc=App.newDocument()\n"
        "dp=Sandbox.DocumentProtector(doc)\n"
        "dp.addObject(\"Mesh::Ellipsoid\",\"Mesh\")\n"
        "dp.recompute()\n"
    );
}

// -------------------------------------------------------------------------------

DEF_STD_CMD(CmdSandboxDocThreadWithDialog);

CmdSandboxDocThreadWithDialog::CmdSandboxDocThreadWithDialog()
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

void CmdSandboxDocThreadWithDialog::activated(int iMsg)
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

DEF_STD_CMD(CmdSandboxDocThreadWithFileDlg);

CmdSandboxDocThreadWithFileDlg::CmdSandboxDocThreadWithFileDlg()
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

void CmdSandboxDocThreadWithFileDlg::activated(int iMsg)
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

// -------------------------------------------------------------------------------

class CmdSandboxMeshLoader : public Gui::Command
{
public:
    CmdSandboxMeshLoader();
protected:
    void activated(int iMsg);
    bool isActive(void);
private:
    QEventLoop loop;
};

CmdSandboxMeshLoader::CmdSandboxMeshLoader()
  :Command("Sandbox_MeshLoad")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Load mesh in thread");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool6";
}

void CmdSandboxMeshLoader::activated(int iMsg)
{
    // use current path as default
    QStringList filter;
    filter << QObject::tr("All Mesh Files (*.stl *.ast *.bms *.obj)");
    filter << QObject::tr("Binary STL (*.stl)");
    filter << QObject::tr("ASCII STL (*.ast)");
    filter << QObject::tr("Binary Mesh (*.bms)");
    filter << QObject::tr("Alias Mesh (*.obj)");
    filter << QObject::tr("Inventor V2.1 ascii (*.iv)");
    //filter << "Nastran (*.nas *.bdf)";
    filter << QObject::tr("All Files (*.*)");

    // Allow multi selection
    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(),
        QObject::tr("Import mesh"), QString(), filter.join(QLatin1String(";;")));

    Sandbox::MeshLoaderThread thread(fn);
    QObject::connect(&thread, SIGNAL(finished()), &loop, SLOT(quit()));

    thread.start();
    loop.exec();

    Base::Reference<Mesh::MeshObject> data = thread.getMesh();
    App::Document* doc = App::GetApplication().getActiveDocument();
    Mesh::Feature* mesh = static_cast<Mesh::Feature*>(doc->addObject("Mesh::Feature","Mesh"));
    mesh->Mesh.setValuePtr((Mesh::MeshObject*)data);
    mesh->purgeTouched();
}

bool CmdSandboxMeshLoader::isActive(void)
{
    return (hasActiveDocument() && !loop.isRunning());
}

void CreateSandboxCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdSandboxDocumentThread());
    rcCmdMgr.addCommand(new CmdSandboxDocThreadWithSeq());
    rcCmdMgr.addCommand(new CmdSandboxDocThreadBusy());
    rcCmdMgr.addCommand(new CmdSandboxDocumentNoThread());
    rcCmdMgr.addCommand(new CmdSandboxWorkerThread());
    rcCmdMgr.addCommand(new CmdSandboxPythonLockThread());
    rcCmdMgr.addCommand(new CmdSandboxPythonNolockThread());
    rcCmdMgr.addCommand(new CmdSandboxPyQtThread());
    rcCmdMgr.addCommand(new CmdSandboxPythonThread());
    rcCmdMgr.addCommand(new CmdSandboxPythonMainThread());
    rcCmdMgr.addCommand(new CmdSandboxDocThreadWithDialog());
    rcCmdMgr.addCommand(new CmdSandboxDocThreadWithFileDlg());
    rcCmdMgr.addCommand(new CmdSandboxEventLoop);
    rcCmdMgr.addCommand(new CmdSandboxMeshLoader);
}
