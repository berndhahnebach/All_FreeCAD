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
# include <Python.h>
#endif

#include <Base/Console.h>
#include <App/DocumentPy.h>
#include <App/DocumentObjectPy.h>
#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>

#include "DocumentThread.h"
#include "DocumentProtector.h"
#include "DocumentProtectorPy.h"


/* module functions */

class SandboxModule : public Py::ExtensionModule<SandboxModule>
{

public:
    SandboxModule() : Py::ExtensionModule<SandboxModule>("Sandbox")
    {
        Sandbox::DocumentProtectorPy::init_type();
        add_varargs_method("DocumentProtector",
            &SandboxModule::new_DocumentProtector,
            "DocumentProtector(Document)");
        Sandbox::DocumentObjectProtectorPy::init_type();
        add_varargs_method("DocumentObjectProtector",
            &SandboxModule::new_DocumentObjectProtector,
            "DocumentObjectProtector(DocumentObject)");
        initialize("This module is the Sandbox module"); // register with Python
    }
    
    virtual ~SandboxModule() {}

private:
    Py::Object new_DocumentProtector(const Py::Tuple& args)
    {
        PyObject* o;
        if (!PyArg_ParseTuple(args.ptr(), "O!",&(App::DocumentPy::Type), &o))
            throw Py::Exception();
        App::DocumentPy* doc = static_cast<App::DocumentPy*>(o);
        return Py::asObject(new Sandbox::DocumentProtectorPy(doc));
    }
    Py::Object new_DocumentObjectProtector(const Py::Tuple& args)
    {
        PyObject* o;
        if (!PyArg_ParseTuple(args.ptr(), "O!",&(App::DocumentObjectPy::Type), &o))
            throw Py::Exception();
        App::DocumentObjectPy* obj = static_cast<App::DocumentObjectPy*>(o);
        return Py::asObject(new Sandbox::DocumentObjectProtectorPy(obj));
    }
};


/* Python entry */
extern "C" {
void SandboxAppExport initSandbox() {

    Sandbox::DocumentProtector  ::init();
    Sandbox::SandboxObject      ::init();

    // the following constructor call registers our extension module
    // with the Python runtime system
    (void)new SandboxModule;
    Base::Console().Log("Loading Sandbox module... done\n");
}

} // extern "C"
