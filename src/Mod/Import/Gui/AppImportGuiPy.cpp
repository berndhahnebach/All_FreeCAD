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
# include <Python.h>
# include <Handle_TDocStd_Document.hxx>
# include <Handle_XCAFApp_Application.hxx>
# include <TDocStd_Document.hxx>
# include <XCAFApp_Application.hxx>
# include <XCAFDoc_DocumentTool.hxx>
# include <XCAFDoc_ShapeTool.hxx>
# include <XCAFDoc_ColorTool.hxx>
# include <TDF_Label.hxx>
# include <Quantity_Color.hxx>
# include <STEPCAFControl_Writer.hxx>
#endif

#include <Base/PyObjectBase.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObjectPy.h>
#include <Gui/Application.h>
#include <MOd/Part/Gui/ViewProvider.h>
#include <Mod/Part/App/PartFeature.h>


/* module functions */
static PyObject * exporter(PyObject *self, PyObject *args)
{
    PyObject* object;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os",&object,&filename))
        return NULL;

    Handle(XCAFApp_Application) hApp = XCAFApp_Application::GetApplication();
    Handle(TDocStd_Document) hDoc;
    hApp->NewDocument(TCollection_ExtendedString("MDTV-CAF"), hDoc);
    Handle_XCAFDoc_ShapeTool hShapeTool = XCAFDoc_DocumentTool::ShapeTool(hDoc->Main());
    Handle_XCAFDoc_ColorTool hColors = XCAFDoc_DocumentTool::ColorTool(hDoc->Main());
    TDF_Label top_label = hShapeTool->NewShape(); // this is the "root" label for the assembly

    Py::List list(object);
    for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
        PyObject* item = (*it).ptr();
        if (PyObject_TypeCheck(item, &(App::DocumentObjectPy::Type))) {
            App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(item)->getDocumentObjectPtr();
            if (obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
                Part::Feature* part = static_cast<Part::Feature*>(obj);
                const TopoDS_Shape& shape = part->Shape.getValue();

                // Add shape
                TDF_Label col_label = hShapeTool->AddShape(shape, Standard_False);

                // Add color information
                Quantity_Color col;
                Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(part);
                if (vp && vp->isDerivedFrom(PartGui::ViewProviderPart::getClassTypeId())) {
                    App::Color color = static_cast<PartGui::ViewProviderPart*>(vp)->ShapeColor.getValue();
                    Quantity_Parameter mat[3];
                    mat[0] = color.r;
                    mat[1] = color.g;
                    mat[2] = color.b;
                    col.SetValues(mat[0],mat[1],mat[2],Quantity_TOC_RGB);
                    hColors->SetColor(col_label, col, XCAFDoc_ColorGen);
                }
            }
        }
    }

    STEPCAFControl_Writer writer;
    writer.Transfer(hDoc, STEPControl_AsIs);
    writer.Write(filename);

    Py_Return;
}

/* registration table  */
struct PyMethodDef ImportGui_Import_methods[] = {
    {"export"     ,exporter  ,METH_VARARGS,
     "export(list,string) -- Export a list of objects into a single file."},
    {NULL, NULL}                   /* end of table marker */
};
