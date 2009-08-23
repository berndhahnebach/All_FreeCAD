/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QFileInfo>
#endif

#include <Base/PyObjectBase.h>
#include <Base/Interpreter.h>
#include <Gui/Application.h>
#include <Gui/EditorView.h>
#include <Gui/TextEdit.h>
#include <Gui/MainWindow.h>

#include "PovrayHighlighter.h"

using namespace RaytracingGui;

/// write project file
static PyObject *
open(PyObject *self, PyObject *args)
{
    // only used to open Povray files
    const char* Name;
    const char* DocName;
    if (!PyArg_ParseTuple(args, "s|s",&Name, &DocName))
        return NULL;
    PY_TRY {
        QString fileName = QString::fromUtf8(Name);
        QFileInfo fi;
        fi.setFile(fileName);
        QString ext = fi.completeSuffix().toLower();
        QList<Gui::EditorView*> views = Gui::getMainWindow()->findChildren<Gui::EditorView*>();
        for (QList<Gui::EditorView*>::Iterator it = views.begin(); it != views.end(); ++it) {
            if ((*it)->fileName() == fileName) {
                (*it)->setFocus();
                Py_Return;
            }
        }

        if (ext == QLatin1String("pov") || ext == QLatin1String("inc")) {
            Gui::TextEditor* editor = new Gui::TextEditor();
            (void)new PovrayHighlighter(editor);
            Gui::EditorView* edit = new Gui::EditorView(editor, Gui::getMainWindow());
            edit->open(fileName);
            edit->resize(400, 300);
            Gui::getMainWindow()->addWindow(edit);
        }
    } PY_CATCH;

    Py_Return;
}

/* registration table  */
struct PyMethodDef RaytracingGui_methods[] = {
    {"open"       ,open    ,METH_VARARGS,
     "open(string) -- Create a new text document and load the file into the document."},
    {"insert"     ,open    ,METH_VARARGS,
     "insert(string,string) -- Create a new text document and load the file into the document."},
    {NULL, NULL}
};
