/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include "UnitTestPy.h"
#include "UnitTestImp.h"
#include "qrc_Test.cpp"

#include <Gui/Language/Translator.h>
#include <Base/Console.h>

static PyObject* addTest(PyObject *self, PyObject *args)          
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception

    TestGui::UnitTestDialog* dlg = TestGui::UnitTestDialog::instance();
    if (pstr)
        dlg->addUnitTest(QString::fromLatin1(pstr));
    dlg->show();
    dlg->raise();
    Py_Return;       
}

static PyObject* setTest(PyObject *self, PyObject *args)          
{
    char *pstr=0;
    if (!PyArg_ParseTuple(args, "|s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception

    TestGui::UnitTestDialog* dlg = TestGui::UnitTestDialog::instance();
    if (pstr)
        dlg->setUnitTest(QString::fromLatin1(pstr));
    dlg->show();
    dlg->raise();
    Py_Return;       
}

/* registration table  */
static struct PyMethodDef TestGui_methods[] = {
    {"addTest", addTest, 1},       
    {"setTest", setTest, 1},       
    {NULL, NULL}                   /* end of table marker */
};

void loadTestResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Test);
    Gui::Translator::instance()->refresh();
}

/* Python entry */
extern "C" {
void AppTestGuiExport initQtUnitGui()
{
    if(PyType_Ready(&TestGui::UnitTestPy::Type) < 0) return;
    PyObject* pyModule = Py_InitModule("QtUnitGui", TestGui_methods);   /* mod name, table ptr */
    union PyType_Object pyDlgType = {&TestGui::UnitTestPy::Type};
    PyModule_AddObject(pyModule, "UnitTest", pyDlgType.o);
    Base::Console().Log("Loading GUI of Test module... done\n");

    // add resources and reloads the translators
    loadTestResource();
    return;
}

} // extern "C"
