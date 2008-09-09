/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <QDir>
# include <QLibraryInfo>
# include <QMessageBox>
# include <QProcess>
# include <QTextStream>
#endif

#include "Assistant.h"
#include <Base/Console.h>

using namespace Gui;

Assistant::Assistant()
    : proc(0)
{
}

Assistant::~Assistant()
{
    if (proc && proc->state() == QProcess::Running) {
        proc->terminate();
        proc->waitForFinished(3000);
    }
}

void Assistant::showDocumentation(const QString &page)
{
    //Base::Console().Message("Open page %s\n", (const char*)page.toUtf8());
    //return;
    if (!startAssistant())
        return;
    QTextStream str(proc);
    str << QLatin1String("SetSource qthelp://com.freecad/doc/")
        << page << QLatin1Char('\0') << endl;
}

bool Assistant::startAssistant()
{
    if (!proc)
        proc = new QProcess();

    if (proc->state() != QProcess::Running) {
        QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
        //QString app = QLatin1String("C:/Temp/qt/");
#if !defined(Q_OS_MAC)
        app += QLatin1String("assistant");
#else
        app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

        QStringList args;
        args << QLatin1String("-collectionFile")
             << QLatin1String("freecad.qhc")
             << QLatin1String("-enableRemoteControl");
#if 0
        args.append(QLatin1String("-server"));
        args.append(QLatin1String("-profile"));
        args.append(QLatin1String("test.adp"));
#endif

        proc->start(app, args);

        if (!proc->waitForStarted()) {
            QMessageBox::critical(0, QObject::tr("FreeCAD Help"),
            QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
            return false;
        }
    }
    
    return true;
}
