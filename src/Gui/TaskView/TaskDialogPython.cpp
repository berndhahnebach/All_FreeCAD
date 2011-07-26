/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <sstream>
#endif

#include "TaskDialogPython.h"
#include "TaskView.h"

#include <Gui/BitmapFactory.h>
#include <Gui/Control.h>
#include <Gui/WidgetFactory.h>
#include <Base/Interpreter.h>
#include <Base/Console.h>
#include <CXX/Objects.hxx>

using namespace Gui;
using namespace Gui::TaskView;

ControlPy* ControlPy::instance = 0;

ControlPy* ControlPy::getInstance()
{
    if (!instance)
        instance = new ControlPy();
    return instance;
}

void ControlPy::init_type()
{
    behaviors().name("Control");
    behaviors().doc("Control for task dialogs");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    behaviors().supportGetattr();
    behaviors().supportSetattr();
    add_varargs_method("showDialog",&ControlPy::showDialog,"showDialog()");
    add_varargs_method("activeDialog",&ControlPy::activeDialog,"activeDialog()");
    add_varargs_method("closeDialog",&ControlPy::closeDialog,"closeDialog()");
    add_varargs_method("isAllowedAlterDocument",&ControlPy::isAllowedAlterDocument,"isAllowedAlterDocument()");
    add_varargs_method("isAllowedAlterView",&ControlPy::isAllowedAlterView,"isAllowedAlterView()");
    add_varargs_method("isAllowedAlterSelection",&ControlPy::isAllowedAlterSelection,"isAllowedAlterSelection()");
}

ControlPy::ControlPy()
{
}

ControlPy::~ControlPy()
{
}

Py::Object ControlPy::repr()
{
    std::string s;
    std::ostringstream s_out;
    s_out << "Control Task Dialog";
    return Py::String(s_out.str());
}

Py::Object ControlPy::showDialog(const Py::Tuple& args)
{
    Gui::TaskView::TaskDialog* act = Gui::Control().activeDialog();
    if (act)
        throw Py::Exception("Active task dialog found");
    TaskDialogPython* dlg = new TaskDialogPython(args[0]);
    Gui::Control().showDialog(dlg);
    return Py::None();
}

Py::Object ControlPy::activeDialog(const Py::Tuple& args)
{
    Gui::TaskView::TaskDialog* dlg = Gui::Control().activeDialog();
    return Py::Boolean(dlg!=0);
}

Py::Object ControlPy::closeDialog(const Py::Tuple&)
{
    Gui::Control().closeDialog();
    return Py::None();
}

Py::Object ControlPy::isAllowedAlterDocument(const Py::Tuple&)
{
    bool ok = Gui::Control().isAllowedAlterDocument();
    return Py::Boolean(ok);
}

Py::Object ControlPy::isAllowedAlterView(const Py::Tuple&)
{
    bool ok = Gui::Control().isAllowedAlterView();
    return Py::Boolean(ok);
}

Py::Object ControlPy::isAllowedAlterSelection(const Py::Tuple&)
{
    bool ok = Gui::Control().isAllowedAlterSelection();
    return Py::Boolean(ok);
}

// ------------------------------------------------------------------

TaskDialogPython::TaskDialogPython(const Py::Object& o) : dlg(o)
{
    UiLoader loader;
#if QT_VERSION >= 0x040500
    loader.setLanguageChangeEnabled(true);
#endif
    QString fn, icon;
    Py::Callable method(dlg.getAttr(std::string("ui")));
    Py::Tuple args(0);
    std::string path = (std::string)Py::String((method.apply(args)));
    fn = QString::fromUtf8(path.c_str());

    QFile file(fn);
    QWidget* form = 0;
    if (file.open(QFile::ReadOnly))
        form = loader.load(&file, 0);
    file.close();
    if (form) {
        Gui::TaskView::TaskBox* taskbox = new Gui::TaskView::TaskBox(
            QPixmap(icon), form->windowTitle(), true, 0);
        taskbox->groupLayout()->addWidget(form);
        Content.push_back(taskbox);
    }
    else {
        Base::Console().Error("Failed to load UI file from '%s'\n",
            (const char*)fn.toUtf8());
    }
}

TaskDialogPython::~TaskDialogPython()
{
    Base::PyGILStateLocker lock;
    this->dlg = Py::None();
}

void TaskDialogPython::open()
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("open"))) {
            Py::Callable method(dlg.getAttr(std::string("open")));
            Py::Tuple args(0);
            method.apply(args);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::open: %s\n", e.what());
    }
}

void TaskDialogPython::clicked(int i)
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("clicked"))) {
            Py::Callable method(dlg.getAttr(std::string("clicked")));
            Py::Tuple args(1);
            args.setItem(0, Py::Int(i));
            method.apply(args);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::clicked: %s\n", e.what());
    }
}

bool TaskDialogPython::accept()
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("accept"))) {
            Py::Callable method(dlg.getAttr(std::string("accept")));
            Py::Tuple args(0);
            Py::Boolean ret(method.apply(args));
            return (bool)ret;
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::accept: %s\n", e.what());
    }

    return TaskDialog::accept();
}

bool TaskDialogPython::reject()
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("reject"))) {
            Py::Callable method(dlg.getAttr(std::string("reject")));
            Py::Tuple args(0);
            Py::Boolean ret(method.apply(args));
            return (bool)ret;
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::reject: %s\n", e.what());
    }

    return TaskDialog::reject();
}

void TaskDialogPython::helpRequested()
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("helpRequested"))) {
            Py::Callable method(dlg.getAttr(std::string("helpRequested")));
            Py::Tuple args(0);
            method.apply(args);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::helpRequested: %s\n", e.what());
    }
}

QDialogButtonBox::StandardButtons TaskDialogPython::getStandardButtons(void) const
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("getStandardButtons"))) {
            Py::Callable method(dlg.getAttr(std::string("getStandardButtons")));
            Py::Tuple args(0);
            Py::Int ret(method.apply(args));
            int value = (int)ret;
            return QDialogButtonBox::StandardButtons(value);
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::getStandardButtons: %s\n", e.what());
    }

    return TaskDialog::getStandardButtons();
}

void TaskDialogPython::modifyStandardButtons(QDialogButtonBox*)
{
}

bool TaskDialogPython::isAllowedAlterDocument(void) const
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("isAllowedAlterDocument"))) {
            Py::Callable method(dlg.getAttr(std::string("isAllowedAlterDocument")));
            Py::Tuple args(0);
            Py::Boolean ret(method.apply(args));
            return (bool)ret;
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::isAllowedAlterDocument: %s\n", e.what());
    }

    return TaskDialog::isAllowedAlterDocument();
}

bool TaskDialogPython::isAllowedAlterView(void) const
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("isAllowedAlterView"))) {
            Py::Callable method(dlg.getAttr(std::string("isAllowedAlterView")));
            Py::Tuple args(0);
            Py::Boolean ret(method.apply(args));
            return (bool)ret;
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::isAllowedAlterView: %s\n", e.what());
    }

    return TaskDialog::isAllowedAlterView();
}

bool TaskDialogPython::isAllowedAlterSelection(void) const
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("isAllowedAlterSelection"))) {
            Py::Callable method(dlg.getAttr(std::string("isAllowedAlterSelection")));
            Py::Tuple args(0);
            Py::Boolean ret(method.apply(args));
            return (bool)ret;
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::isAllowedAlterSelection: %s\n", e.what());
    }

    return TaskDialog::isAllowedAlterSelection();
}

bool TaskDialogPython::needsFullSpace() const
{
    Base::PyGILStateLocker lock;
    try {
        if (dlg.hasAttr(std::string("needsFullSpace"))) {
            Py::Callable method(dlg.getAttr(std::string("needsFullSpace")));
            Py::Tuple args(0);
            Py::Boolean ret(method.apply(args));
            return (bool)ret;
        }
    }
    catch (Py::Exception&) {
        Base::PyException e; // extract the Python error text
        Base::Console().Error("TaskDialogPython::needsFullSpace: %s\n", e.what());
    }

    return TaskDialog::needsFullSpace();
}

