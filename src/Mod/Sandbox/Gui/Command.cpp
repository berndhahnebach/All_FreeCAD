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
# include <QCalendarWidget>
# include <QColorDialog>
# include <QObject>
# include <QEventLoop>
# include <QLabel>
# include <QTimer>
# include <QImage>
# include <QImageReader>
# include <QPainter>
# include <Inventor/nodes/SoAnnotation.h>
# include <Inventor/nodes/SoImage.h>
#endif

#include <Base/Console.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

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

DEF_STD_CMD(CmdSandboxDocumentTestThread);

CmdSandboxDocumentTestThread::CmdSandboxDocumentTestThread()
  :Command("Sandbox_TestThread")
{
    sAppModule    = "Sandbox";
    sGroup        = QT_TR_NOOP("Sandbox");
    sMenuText     = QT_TR_NOOP("Test thread");
    sToolTipText  = QT_TR_NOOP("Sandbox Test function");
    sWhatsThis    = QT_TR_NOOP("Sandbox Test function");
    sStatusTip    = QT_TR_NOOP("Sandbox Test function");
    sPixmap       = "Std_Tool1";
}

void CmdSandboxDocumentTestThread::activated(int iMsg)
{
    App::GetApplication().newDocument("Thread");
    Sandbox::DocumentTestThread* dt = new Sandbox::DocumentTestThread();
    QObject::connect(dt, SIGNAL(finished()), dt, SLOT(deleteLater()));
    dt->start();
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
    const char* className() const
    { return "CmdSandboxEventLoop"; }
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
    const char* className() const
    { return "CmdSandboxMeshLoader"; }
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

//===========================================================================
// Std_GrabWidget
//===========================================================================
DEF_STD_CMD_A(CmdTestGrabWidget);

CmdTestGrabWidget::CmdTestGrabWidget()
  : Command("Std_GrabWidget")
{
    sGroup          = "Standard-Test";
    sMenuText       = "Grab widget";
    sToolTipText    = "Grab widget";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    iAccel          = 0;
}

void CmdTestGrabWidget::activated(int iMsg)
{
    QCalendarWidget* c = new QCalendarWidget();
    c->hide();
    QPixmap p = QPixmap::grabWidget(c, c->rect());
    QLabel* label = new QLabel();
    label->resize(c->size());
    label->setPixmap(p);
    label->show();
    delete c;
}

bool CmdTestGrabWidget::isActive(void)
{
    return true;
}

//===========================================================================
// Std_ImageNode
//===========================================================================
DEF_3DV_CMD(CmdTestImageNode);

class RenderArea : public QWidget
{
private:
    QPainterPath path;
    int penWidth;
    QColor penColor;

public:
    RenderArea(const QPainterPath &path, QWidget *parent=0)
      : QWidget(parent), path(path), penColor(0,0,127)
    {
        penWidth = 2;
        setBackgroundRole(QPalette::Base);
    }

    QSize minimumSizeHint() const
    {
        return QSize(50, 50);
    }

    QSize sizeHint() const
    {
        return QSize(100, 30);
    }

    void setFillRule(Qt::FillRule rule)
    {
        path.setFillRule(rule);
        update();
    }

    void setPenWidth(int width)
    {
        penWidth = width;
        update();
    }

    void setPenColor(const QColor &color)
    {
        penColor = color;
        update();
    }

    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.scale(width() / 100.0, height() / 100.0);
        painter.translate(50.0, 50.0);
        painter.translate(-50.0, -50.0);

        painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.setBrush(QBrush(QColor(0,85,255), Qt::SolidPattern));
        painter.drawPath(path);
        painter.setPen(Qt::white);
        painter.drawText(25, 40, 70, 20, Qt::AlignHCenter|Qt::AlignVCenter,
            QString::fromAscii("Distance: 2.784mm"));
        //QPainterPath text;
        //text.addText(25,55,QFont(), QString::fromAscii("Distance"));
        //painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
        //painter.drawPath(text);
    }
};

CmdTestImageNode::CmdTestImageNode()
  : Command("Std_ImageNode")
{
    sGroup          = "Standard-Test";
    sMenuText       = "SoImage node";
    sToolTipText    = "SoImage node";
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    iAccel          = 0;
}
#include <QFontMetrics>
void CmdTestImageNode::activated(int iMsg)
{
    QString text = QString::fromAscii("Distance: 2.7jgiorjgor84mm");
    QFont font;
    QFontMetrics fm(font);
    int w = fm.width(text);
    int h = fm.height();


    QPainterPath roundRectPath;
    //roundRectPath.moveTo(80.0, 35.0);
    //roundRectPath.arcTo(70.0, 30.0, 10.0, 10.0, 0.0, 90.0);
    //roundRectPath.lineTo(25.0, 30.0);
    //roundRectPath.arcTo(20.0, 30.0, 10.0, 10.0, 90.0, 90.0);
    //roundRectPath.lineTo(20.0, 65.0);
    //roundRectPath.arcTo(20.0, 60.0, 10.0, 10.0, 180.0, 90.0);
    //roundRectPath.lineTo(75.0, 70.0);
    //roundRectPath.arcTo(70.0, 60.0, 10.0, 10.0, 270.0, 90.0);
    roundRectPath.moveTo(100.0, 5.0);
    roundRectPath.arcTo(90.0, 0.0, 10.0, 10.0, 0.0, 90.0);
    roundRectPath.lineTo(5.0, 0.0);
    roundRectPath.arcTo(0.0, 0.0, 10.0, 10.0, 90.0, 90.0);
    roundRectPath.lineTo(0.0, 95.0);
    roundRectPath.arcTo(0.0, 90.0, 10.0, 10.0, 180.0, 90.0);
    roundRectPath.lineTo(95.0, 100.0);
    roundRectPath.arcTo(90.0, 90.0, 10.0, 10.0, 270.0, 90.0);
    roundRectPath.closeSubpath();


    QLabel* l = new QLabel();
    //l.setText(QLatin1String("Distance: 2.784mm"));
    //QPixmap p = QPixmap::grabWidget(&l, 0,0,100,100);
    //l.show();
    //QPixmap p = Gui::BitmapFactory().pixmap("edit-cut");

    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
    SoImage* node = new SoImage();

    QImage image(w+10,h+10,QImage::Format_ARGB32_Premultiplied);// = p.toImage();
    image.fill(0x00000000);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(QColor(0,0,127), 2, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.setBrush(QBrush(QColor(0,85,255), Qt::SolidPattern));
    QRectF rectangle(0.0, 0.0, w+10, h+10);
    painter.drawRoundedRect(rectangle, 5, 5);
    //painter.drawRect(rectangle);
    //painter.drawPath(roundRectPath);
    painter.setPen(QColor(255,255,255));
    painter.drawText(5,h+3, text);
    painter.end();
    //l->setPixmap(QPixmap::fromImage(image));
    //l->show();
    //RenderArea* ra = new RenderArea(roundRectPath);
    //ra->show();

    //QPixmap p = QPixmap::grabWidget(ra, 0,0,100,30);
    //image = p.toImage();

    SoSFImage texture;
    Gui::BitmapFactory().convert(image, texture);
    node->image = texture;
    SoAnnotation* anno = new SoAnnotation();
    anno->addChild(node);
    static_cast<SoGroup*>(viewer->getSceneGraph())->addChild(anno);
}

void CreateSandboxCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdSandboxDocumentThread());
    rcCmdMgr.addCommand(new CmdSandboxDocumentTestThread());
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
    rcCmdMgr.addCommand(new CmdTestGrabWidget());
    rcCmdMgr.addCommand(new CmdTestImageNode());
}
