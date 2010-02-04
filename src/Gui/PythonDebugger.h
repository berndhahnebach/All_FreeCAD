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


#ifndef GUI_PYTHONDEBUG_H
#define GUI_PYTHONDEBUG_H

#include <CXX/Extensions.hxx>
#include <frameobject.h>

namespace Gui {

/**
 * @author Werner Mayer
 */
class GuiExport PythonDebugModule : public Py::ExtensionModule<PythonDebugModule>
{
public:
    static void init_module(void);

    PythonDebugModule();
    virtual ~PythonDebugModule();

private:
    Py::Object getFunctionCallCount(const Py::Tuple &a);
    Py::Object getExceptionCount(const Py::Tuple &a);
    Py::Object getLineCount(const Py::Tuple &a);
    Py::Object getFunctionReturnCount(const Py::Tuple &a);
};

/**
 * @author Werner Mayer
 */
class GuiExport PythonDebugStdout : public Py::PythonExtension<PythonDebugStdout> 
{
public:
    static void init_type(void);    // announce properties and methods

    PythonDebugStdout();
    ~PythonDebugStdout();

    Py::Object repr();
    Py::Object write(const Py::Tuple&);
    Py::Object flush(const Py::Tuple&);
};

/**
 * @author Werner Mayer
 */
class GuiExport PythonDebugStderr : public Py::PythonExtension<PythonDebugStderr> 
{
public:
    static void init_type(void);    // announce properties and methods

    PythonDebugStderr();
    ~PythonDebugStderr();

    Py::Object repr();
    Py::Object write(const Py::Tuple&);
};

/**
 * @author Werner Mayer
 */
class GuiExport PythonDebugExcept : public Py::PythonExtension<PythonDebugExcept> 
{
public:
    static void init_type(void);    // announce properties and methods

    PythonDebugExcept();
    ~PythonDebugExcept();

    Py::Object repr();
    Py::Object excepthook(const Py::Tuple&);
};

class GuiExport PythonDebugger : public QObject
{
    Q_OBJECT

public:
    PythonDebugger();
    ~PythonDebugger();
    bool start();
    bool stop();
    void next();
    void tryStop();

Q_SIGNALS:
    void signalNextStep();

private:
    static int tracer_callback(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg);

private:
    struct PythonDebuggerP* d;
};

} // namespace Gui

#endif // GUI_PYTHONDEBUG_H
