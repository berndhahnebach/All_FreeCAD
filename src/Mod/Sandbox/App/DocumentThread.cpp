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
#endif

#include "DocumentThread.h"
#include "DocumentProtector.h"

#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Document.h>

using namespace Sandbox;


DocumentThread::DocumentThread(QObject* parent)
  : QThread(parent)
{
}

DocumentThread::~DocumentThread()
{
}

void DocumentThread::run()
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    DocumentProtector dp(doc);
    App::DocumentObject* obj = dp.addObject("Mesh::Ellipsoid", (const char*)objectName().toAscii());
    dp.recompute();
}

// --------------------------------------

WorkerThread::WorkerThread(QObject* parent)
  : QThread(parent)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::run()
{
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
