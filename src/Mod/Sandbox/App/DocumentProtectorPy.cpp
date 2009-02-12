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

#include "DocumentProtectorPy.h"
#include "DocumentProtector.h"

#include <Base/Exception.h>
#include <App/DocumentPy.h>
#include <App/DocumentObject.h>
#include <App/DocumentObjectPy.h>

using namespace Sandbox;


void DocumentProtectorPy::init_type()
{
    behaviors().name("DocumentProtectorPy");
    behaviors().doc("Python binding class for the document protector class");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    behaviors().supportGetattr();
    behaviors().supportSetattr();

    add_varargs_method("addObject",&DocumentProtectorPy::addObject,"addObject(type,name)");
    add_varargs_method("recompute",&DocumentProtectorPy::recompute,"recompute()");
}

DocumentProtectorPy::DocumentProtectorPy(App::DocumentPy *doc)
{
    _dp = new DocumentProtector(doc->getDocumentPtr());
}

DocumentProtectorPy::~DocumentProtectorPy()
{
    delete _dp;
}

Py::Object DocumentProtectorPy::repr()
{
    std::string s;
    std::ostringstream s_out;
    if (!_dp)
        throw Py::RuntimeError("Cannot print representation of deleted object");
    s_out << "Document protector";
    return Py::String(s_out.str());
}

DocumentProtectorPy::method_varargs_handler DocumentProtectorPy::pycxx_handler = 0;

PyObject *DocumentProtectorPy::method_varargs_ext_handler(PyObject *_self_and_name_tuple, PyObject *_args)
{
    try {
        return pycxx_handler(_self_and_name_tuple, _args);
    }
    catch (const Base::Exception& e) {
        throw Py::Exception(e.what());
    }
    catch (const std::exception& e) {
        throw Py::Exception(e.what());
    }
    catch(...) {
        throw Py::Exception("Unknown C++ exception");
    }
}

Py::Object DocumentProtectorPy::getattr(const char * attr)
{
    if (!_dp) {
        std::string s;
        std::ostringstream s_out;
        s_out << "Cannot access attribute '" << attr << "' of deleted object";
        throw Py::RuntimeError(s_out.str());
    }
    else {
        Py::Object obj = Py::PythonExtension<DocumentProtectorPy>::getattr(attr);
        if (PyCFunction_Check(obj.ptr())) {
            PyCFunctionObject* op = reinterpret_cast<PyCFunctionObject*>(obj.ptr());
            if (!pycxx_handler)
                pycxx_handler = op->m_ml->ml_meth;
            op->m_ml->ml_meth = method_varargs_ext_handler;
        }
        return obj;
    }
}

int DocumentProtectorPy::setattr(const char * attr, const Py::Object & value)
{
    if (!_dp) {
        std::string s;
        std::ostringstream s_out;
        s_out << "Cannot access attribute '" << attr << "' of deleted object";
        throw Py::RuntimeError(s_out.str());
    }
    else {
        return Py::PythonExtension<DocumentProtectorPy>::setattr(attr, value);
    }
}

Py::Object DocumentProtectorPy::addObject(const Py::Tuple& args)
{
    char* type;
    char* name="";
    if (!PyArg_ParseTuple(args.ptr(), "s|s",&type, &name))
        throw Py::Exception();
    // release the global interpreter lock
    PyThreadState* state = PyEval_SaveThread();
    App::DocumentObject* obj = _dp->addObject(type, name);
    // grab the global interpreter lock again
    PyEval_RestoreThread(state);
    if (!obj) {
        std::string s;
        std::ostringstream s_out;
        s_out << "Couldn't create an object of type '" << type << "'";
        throw Py::RuntimeError(s_out.str());
    }
    return Py::asObject(obj->getPyObject());
}

Py::Object DocumentProtectorPy::recompute(const Py::Tuple& args)
{
    if (!PyArg_ParseTuple(args.ptr(), ""))
        throw Py::Exception();
    // release the global interpreter lock
    PyThreadState* state = PyEval_SaveThread();
    _dp->recompute();
    // grab the global interpreter lock again
    PyEval_RestoreThread(state);
    return Py::None();
}
