/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QEventLoop>
# include <QCoreApplication>
# include <QTimer>
#endif

#include "PythonDebugger.h"
#include <Base/Interpreter.h>

using namespace Gui;

void PythonDebugModule::init_module(void)
{
    PythonDebugStdout::init_type();
    PythonDebugStderr::init_type();
    PythonDebugExcept::init_type();
    static PythonDebugModule* mod = new PythonDebugModule();
}

PythonDebugModule::PythonDebugModule()
  : ExtensionModule<PythonDebugModule>("FreeCADDbg")
{
    add_varargs_method("getFunctionCallCount", &PythonDebugModule::getFunctionCallCount,
        "Get the total number of function calls executed and the number executed since the last call to this function.");
    add_varargs_method("getExceptionCount", &PythonDebugModule::getExceptionCount,
        "Get the total number of exceptions and the number executed since the last call to this function.");
    add_varargs_method("getLineCount", &PythonDebugModule::getLineCount,
        "Get the total number of lines executed and the number executed since the last call to this function.");
    add_varargs_method("getFunctionReturnCount", &PythonDebugModule::getFunctionReturnCount,
        "Get the total number of function returns executed and the number executed since the last call to this function.");

    initialize( "The FreeCAD Python debug module" );

    Py::Dict d(moduleDictionary());
    Py::Object out(Py::asObject(new PythonDebugStdout()));
    d["StdOut"] = out;
    Py::Object err(Py::asObject(new PythonDebugStderr()));
    d["StdErr"] = err;
}

PythonDebugModule::~PythonDebugModule()
{
}

Py::Object PythonDebugModule::getFunctionCallCount(const Py::Tuple &a)
{
    return Py::None();
}

Py::Object PythonDebugModule::getExceptionCount(const Py::Tuple &a)
{
    return Py::None();
}

Py::Object PythonDebugModule::getLineCount(const Py::Tuple &a)
{
    return Py::None();
}

Py::Object PythonDebugModule::getFunctionReturnCount(const Py::Tuple &a)
{
    return Py::None();
}

// -----------------------------------------------------

void PythonDebugStdout::init_type()
{
    behaviors().name("PythonDebugStdout");
    behaviors().doc("Redirection of stdout to FreeCAD's Python debugger window");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    add_varargs_method("write",&PythonDebugStdout::write,"write to stdout");
    add_varargs_method("flush",&PythonDebugStdout::flush,"flush the output");
}

PythonDebugStdout::PythonDebugStdout()
{
}

PythonDebugStdout::~PythonDebugStdout()
{
}

Py::Object PythonDebugStdout::repr()
{
    std::string s;
    std::ostringstream s_out;
    s_out << "PythonDebugStdout";
    return Py::String(s_out.str());
}

Py::Object PythonDebugStdout::write(const Py::Tuple& args)
{
    char *msg;
    PyObject* pObj;
    //args contains a single parameter which is the string to write.
    if (!PyArg_ParseTuple(args.ptr(), "Os:OutputString", &pObj, &msg))
        throw Py::Exception();

    if (strlen(msg) > 0)
    {
        //send it to our stdout
        printf(msg);

        //send it to the debugger as well
        //g_DebugSocket.SendMessage(eMSG_OUTPUT, msg);
    }
    return Py::None();
}

Py::Object PythonDebugStdout::flush(const Py::Tuple&)
{
    return Py::None();
}

// -----------------------------------------------------

void PythonDebugStderr::init_type()
{
    behaviors().name("PythonDebugStderr");
    behaviors().doc("Redirection of stderr to FreeCAD's Python debugger window");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    add_varargs_method("write",&PythonDebugStderr::write,"write to stderr");
}

PythonDebugStderr::PythonDebugStderr()
{
}

PythonDebugStderr::~PythonDebugStderr()
{
}

Py::Object PythonDebugStderr::repr()
{
    std::string s;
    std::ostringstream s_out;
    s_out << "PythonDebugStderr";
    return Py::String(s_out.str());
}

Py::Object PythonDebugStderr::write(const Py::Tuple& args)
{
    char *msg;
    PyObject* pObj;
    //args contains a single parameter which is the string to write.
    if (!PyArg_ParseTuple(args.ptr(), "Os:OutputDebugString", &pObj, &msg))
        throw Py::Exception();

    if (strlen(msg) > 0)
    {
        //send the message to our own stderr
        //dprintf(msg);

        //send it to the debugger as well
        //g_DebugSocket.SendMessage(eMSG_TRACE, msg);
    }

    return Py::None();
}

// -----------------------------------------------------

void PythonDebugExcept::init_type()
{
    behaviors().name("PythonDebugExcept");
    behaviors().doc("Custom exception handler");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    add_varargs_method("fc_excepthook",&PythonDebugExcept::excepthook,"Custom exception handler");
}

PythonDebugExcept::PythonDebugExcept()
{
}

PythonDebugExcept::~PythonDebugExcept()
{
}

Py::Object PythonDebugExcept::repr()
{
    std::string s;
    std::ostringstream s_out;
    s_out << "PythonDebugExcept";
    return Py::String(s_out.str());
}

Py::Object PythonDebugExcept::excepthook(const Py::Tuple& args)
{
    PyObject *exc, *value, *tb;
    if (!PyArg_UnpackTuple(args.ptr(), "excepthook", 3, 3, &exc, &value, &tb))
        throw Py::Exception();

    PyErr_NormalizeException(&exc, &value, &tb);

    PyErr_Display(exc, value, tb);
/*
    if (eEXCEPTMODE_IGNORE != g_eExceptionMode)
    {
        assert(tb);

        if (tb && (tb != Py_None))
        {
            //get the pointer to the frame held by the bottom traceback object - this
            //should be where the exception occurred.
            tracebackobject* pTb = (tracebackobject*)tb;
            while (pTb->tb_next != NULL) 
            {
                pTb = pTb->tb_next;
            }
            PyFrameObject* frame = (PyFrameObject*)PyObject_GetAttr((PyObject*)pTb, PyString_FromString("tb_frame"));
            EnterBreakState(frame, (PyObject*)pTb);
        }
    }*/

    return Py::None();
}

// -----------------------------------------------------

namespace Gui {
class PythonDebuggerPy : public Py::PythonExtension<PythonDebuggerPy> 
{
public:
    PythonDebuggerPy(PythonDebugger* d) : dbg(d) { }
    ~PythonDebuggerPy() {}
    PythonDebugger* dbg;
};

struct PythonDebuggerP {
    PyObject* out_o;
    PyObject* err_o;
    PyObject* exc_o;
    PyObject* out_n;
    PyObject* err_n;
    PyObject* exc_n;
    bool init, trystop;
    QEventLoop loop;
    PyObject* pydbg;
};
}

PythonDebugger::PythonDebugger()
{
    Base::PyGILStateLocker lock;
    d = new PythonDebuggerP();
    d->init = false;
    d->out_n = new PythonDebugStdout();
    d->err_n = new PythonDebugStderr();
    PythonDebugExcept* err = new PythonDebugExcept();
    Py::Object func = err->getattr("fc_excepthook");
    d->exc_n = Py::new_reference_to(func);
    Py_DECREF(err);
    d->pydbg = new PythonDebuggerPy(this);
}

PythonDebugger::~PythonDebugger()
{
    Py_DECREF(d->out_n);
    Py_DECREF(d->err_n);
    Py_DECREF(d->exc_n);
    Py_DECREF(d->pydbg);
    delete d;
}

bool PythonDebugger::start()
{
    if (d->init)
        return false;
    d->init = true;
    d->trystop = false;
    Base::PyGILStateLocker lock;
    d->out_o = PySys_GetObject("stdout");
    d->err_o = PySys_GetObject("stderr");
    d->exc_o = PySys_GetObject("excepthook");

    PySys_SetObject("stdout", d->out_n);
    PySys_SetObject("stderr", d->err_n);
    PySys_SetObject("excepthook", d->exc_o);

    PyEval_SetTrace(tracer_callback, d->pydbg);
    return true;
}

bool PythonDebugger::stop()
{
    if (!d->init)
        return false;
    Base::PyGILStateLocker lock;
    PyEval_SetTrace(NULL, NULL);
    PySys_SetObject("stdout", d->out_o);
    PySys_SetObject("stderr", d->err_o);
    PySys_SetObject("excepthook", d->exc_o);
    d->init = false;
    return true;
}

void PythonDebugger::tryStop()
{
    d->trystop = true;
}

void PythonDebugger::next()
{
    signalNextStep();
}

// http://www.koders.com/cpp/fidBA6CD8A0FE5F41F1464D74733D9A711DA257D20B.aspx?s=PyEval_SetTrace
// http://code.google.com/p/idapython/source/browse/trunk/python.cpp
// http://www.koders.com/cpp/fid191F7B13CF73133935A7A2E18B7BF43ACC6D1784.aspx?s=PyEval_SetTrace
// http://stuff.mit.edu/afs/sipb/project/python/src/python2.2-2.2.2/Modules/_hotshot.c
int PythonDebugger::tracer_callback(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{
    PythonDebuggerPy* pydbg = static_cast<PythonDebuggerPy*>(obj);
    if (pydbg->dbg->d->trystop)
        PyErr_SetInterrupt();
    QCoreApplication::processEvents();
    int no;

    no = frame->f_tstate->recursion_depth;
    switch (what) {
    case PyTrace_CALL:
        return 0;
    case PyTrace_RETURN:
        return 0;
    case PyTrace_LINE:
        {
            //PyObject *str;
            //str = PyObject_Str(frame->f_code->co_filename);
            no = frame->f_lineno;
            int line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);
            //if (str) {
            //    Base::Console().Message("PROFILING: %s:%d\n", PyString_AsString(str), frame->f_lineno);
            //    Py_DECREF(str);
            //}
    // For testing only
    QEventLoop loop;
    QObject::connect(pydbg->dbg, SIGNAL(signalNextStep()), &loop, SLOT(quit()));
    loop.exec();
            return 0;
        }
    case PyTrace_EXCEPTION:
        return 0;
    case PyTrace_C_CALL:
        return 0;
    case PyTrace_C_EXCEPTION:
        return 0;
    case PyTrace_C_RETURN:
        return 0;
    default:
        /* ignore PyTrace_EXCEPTION */
        break;
    }
    return 0;
    return 0;
}

#include "moc_PythonDebugger.cpp"
