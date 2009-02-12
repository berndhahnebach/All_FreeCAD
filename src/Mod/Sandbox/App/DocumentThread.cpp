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

#include <Base/Console.h>
#include <Base/Sequencer.h>
#include <Base/Interpreter.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Mod/Mesh/App/Mesh.h>

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

// --------------------------------------

PythonThread::PythonThread(QObject* parent)
  : QThread(parent)
{
}

PythonThread::~PythonThread()
{
}

void PythonThread::run()
{
    Base::PyGILStateLocker locker;
    try {
#if 0
        PyObject *module, *dict;
        module = PyImport_AddModule("__main__");
        dict = PyModule_GetDict(module);
        PyObject* dict_copy = PyDict_Copy(dict);

        std::string buf;
        buf = std::string(
            "import Sandbox\n"
            "doc=App.ActiveDocument\n"
            "dp=Sandbox.DocumentProtector(doc)\n"
            "dp.addObject(\"Mesh::Ellipsoid\",\"Mesh\")\n"
            "dp.recompute()\n");
        PyObject *presult = PyRun_String(buf.c_str(), Py_file_input, dict_copy, dict_copy);
        Py_DECREF(dict_copy);
        msleep(10);
#else

        Base::Interpreter().runString(
            "import Sandbox, Mesh, MeshGui\n"
            "doc=App.ActiveDocument\n"
            "dp=Sandbox.DocumentProtector(doc)\n"
            "dp.addObject(\"Mesh::Ellipsoid\",\"Mesh\")\n"
            "dp.recompute()\n");
        msleep(10);
#endif
    }
    catch (const Base::PyException& e) {
        Base::Console().Error(e.what());
    }
}

// --------------------------------------

MeshLoaderThread::MeshLoaderThread(const QString& fn, QObject* parent)
  : QThread(parent), filename(fn)
{
}

MeshLoaderThread::~MeshLoaderThread()
{
}

Base::Reference<Mesh::MeshObject> MeshLoaderThread::getMesh() const
{
    return this->mesh;
}

void MeshLoaderThread::run()
{
    this->mesh = new Mesh::MeshObject();
    this->mesh->load((const char*)filename.toUtf8());
}

