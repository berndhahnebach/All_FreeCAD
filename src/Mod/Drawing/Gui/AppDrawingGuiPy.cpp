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
# include <qimage.h>
#endif

#include "DrawingView.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <App/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/BitmapFactory.h>

using namespace DrawingGui;


/* module functions */
static PyObject * 
open(PyObject *self, PyObject *args) 
{
    const char* Name;
    if (! PyArg_ParseTuple(args, "s",&Name))
        return NULL; 
    
    PY_TRY {
        Base::FileInfo file(Name);
        if (file.hasExtension("svg") || file.hasExtension("svgz")) {
            QString fileName = QString::fromUtf8(Name);
            // Displaying the image in a view
            DrawingView* view = new DrawingView(Gui::getMainWindow());
            view->load(fileName);
            view->setWindowIcon(Gui::BitmapFactory().pixmap("colors"));
            view->setWindowTitle(QObject::tr("Drawing viewer"));
            view->resize( 400, 300 );
            Gui::getMainWindow()->addWindow(view);
        } else {
            PyErr_SetString(PyExc_Exception, "unknown file type");
            return NULL;
        }
    } PY_CATCH;

	Py_Return; 
}


/* module functions */
static PyObject *
insert(PyObject *self, PyObject *args)
{
  // not supported to insert an image (by dropping on an image view)
  // hence do nothing
  Py_Return;
}

/* registration table  */
struct PyMethodDef DrawingGui_Import_methods[] = {
    {"open"       ,open ,       1},				/* method name, C func ptr, always-tuple */
    {"insert"     ,insert,      1},
    {NULL, NULL}                   /* end of table marker */
};
