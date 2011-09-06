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
#if defined(__MINGW32__)
# define WNT // avoid conflict with GUID
#endif
#ifndef _PreComp_
# include <climits>
# include <Python.h>
# include <BRep_Builder.hxx>
# include <Handle_TDocStd_Document.hxx>
# include <Handle_XCAFApp_Application.hxx>
# include <TDocStd_Document.hxx>
# include <XCAFApp_Application.hxx>
# include <XCAFDoc_DocumentTool.hxx>
# include <XCAFDoc_ShapeTool.hxx>
# include <XCAFDoc_ColorTool.hxx>
# include <TDF_Label.hxx>
# include <TDF_LabelSequence.hxx>
# include <TDF_ChildIterator.hxx>
# include <TDataStd_Name.hxx>
# include <Quantity_Color.hxx>
# include <STEPCAFControl_Reader.hxx>
# include <STEPCAFControl_Writer.hxx>
# include <IGESCAFControl_Reader.hxx>
# include <IGESCAFControl_Writer.hxx>
# include <IGESControl_Controller.hxx>
# include <Interface_Static.hxx>
# include <Transfer_TransientProcess.hxx>
# include <XSControl_WorkSession.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <TopTools_MapOfShape.hxx>
# include <TopExp_Explorer.hxx>
# include <TDataStd_Shape.hxx>
#endif

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObjectPy.h>
#include <Gui/Application.h>
#include <Mod/Part/Gui/ViewProvider.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/ProgressIndicator.h>

class ImportXCAF
{
public:
    ImportXCAF(Handle_TDocStd_Document h, App::Document* d, const std::string& name)
        : hdoc(h), doc(d), default_name(name)
    {
        aShapeTool = XCAFDoc_DocumentTool::ShapeTool (hdoc->Main());
        hColors = XCAFDoc_DocumentTool::ColorTool(hdoc->Main());
    }

    void loadShapes()
    {
        // collect sequence of labels to display
        TDF_LabelSequence shapeLabels, colorLabels;
        aShapeTool->GetFreeShapes (shapeLabels);
        hColors->GetColors(colorLabels);

        // set presentations and show
        for (Standard_Integer i=1; i <= shapeLabels.Length(); i++ ) {
            // get the shapes and attributes
            const TDF_Label& label = shapeLabels.Value(i);
            loadShapes(label);
        }
        std::map<Standard_Integer, TopoDS_Shape>::iterator it;
        // go through solids
        for (it = mySolids.begin(); it != mySolids.end(); ++it) {
            createShape(it->second, true, true);
        }
        // go through shells
        for (it = myShells.begin(); it != myShells.end(); ++it) {
            createShape(it->second, true, true);
        }
        // go through compounds
        for (it = myCompds.begin(); it != myCompds.end(); ++it) {
            createShape(it->second, true, true);
        }
        // do the rest
        if (!myShapes.empty()) {
            BRep_Builder builder;
            TopoDS_Compound comp;
            builder.MakeCompound(comp);
            for (it = myShapes.begin(); it != myShapes.end(); ++it) {
                builder.Add(comp, it->second);
            }
            createShape(comp, true, false);
        }
    }

private:
    void createShape(const TopoDS_Shape& shape, bool perface=false, bool setname=false) const
    {
        Part::Feature* part;
        part = static_cast<Part::Feature*>(doc->addObject("Part::Feature", default_name.c_str()));
        part->Shape.setValue(shape);
        std::map<Standard_Integer, Quantity_Color>::const_iterator jt;
        jt = myColorMap.find(shape.HashCode(INT_MAX));
        if (jt != myColorMap.end()) {
            Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(part);
            if (vp && vp->isDerivedFrom(PartGui::ViewProviderPart::getClassTypeId())) {
                App::Color color;
                color.r = jt->second.Red();
                color.g = jt->second.Green();
                color.b = jt->second.Blue();
                static_cast<PartGui::ViewProviderPart*>(vp)->ShapeColor.setValue(color);
            }
        }

        // set label name if defined
        if (setname && !myNameMap.empty()) {
            std::map<Standard_Integer, std::string>::const_iterator jt;
            jt = myNameMap.find(shape.HashCode(INT_MAX));
            if (jt != myNameMap.end()) {
                part->Label.setValue(jt->second);
            }
        }

        // check for colors per face
        if (perface && !myColorMap.empty()) {
            TopTools_IndexedMapOfShape faces;
            TopExp_Explorer xp(shape,TopAbs_FACE);
            while (xp.More()) {
                faces.Add(xp.Current());
                xp.Next();
            }

            bool found_face_color = false;
            std::vector<App::Color> faceColors;
            faceColors.resize(faces.Extent(), App::Color(0.8f,0.8f,0.8f));
            xp.Init(shape,TopAbs_FACE);
            while (xp.More()) {
                jt = myColorMap.find(xp.Current().HashCode(INT_MAX));
                if (jt != myColorMap.end()) {
                    int index = faces.FindIndex(xp.Current());
                    App::Color color;
                    color.r = jt->second.Red();
                    color.g = jt->second.Green();
                    color.b = jt->second.Blue();
                    faceColors[index-1] = color;
                    found_face_color = true;
                }
                xp.Next();
            }

            if (found_face_color) {
                Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(part);
                if (vp && vp->isDerivedFrom(PartGui::ViewProviderPartExt::getClassTypeId())) {
                    static_cast<PartGui::ViewProviderPartExt*>(vp)->DiffuseColor.setValues(faceColors);
                }
            }
        }
    }
    void loadShapes(const TDF_Label& label)
    {
        TopoDS_Shape aShape;
        if (aShapeTool->GetShape(label,aShape)) {
            if (aShapeTool->IsTopLevel(label)) {
                int ctSolids = 0, ctShells = 0, ctComps = 0;
                // add the shapes
                TopExp_Explorer xp;
                for (xp.Init(aShape, TopAbs_SOLID); xp.More(); xp.Next(), ctSolids++)
                    this->mySolids[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                for (xp.Init(aShape, TopAbs_SHELL, TopAbs_SOLID); xp.More(); xp.Next(), ctShells++)
                    this->myShells[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                // if no solids and no shells were found then go for compounds
                if (ctSolids == 0 && ctShells == 0) {
                    for (xp.Init(aShape, TopAbs_COMPOUND); xp.More(); xp.Next(), ctComps++)
                        this->myCompds[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                }
                if (ctComps == 0) {
                    for (xp.Init(aShape, TopAbs_FACE, TopAbs_SHELL); xp.More(); xp.Next())
                        this->myShapes[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                    for (xp.Init(aShape, TopAbs_WIRE, TopAbs_FACE); xp.More(); xp.Next())
                        this->myShapes[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                    for (xp.Init(aShape, TopAbs_EDGE, TopAbs_WIRE); xp.More(); xp.Next())
                        this->myShapes[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                    for (xp.Init(aShape, TopAbs_VERTEX, TopAbs_EDGE); xp.More(); xp.Next())
                        this->myShapes[xp.Current().HashCode(INT_MAX)] = (xp.Current());
                }
            }

            // getting color
            Quantity_Color col;
            if (hColors->GetColor(label, XCAFDoc_ColorGen, col) ||
                hColors->GetColor(label, XCAFDoc_ColorSurf, col) ||
                hColors->GetColor(label, XCAFDoc_ColorCurv, col)) {
                // add defined color
                myColorMap[aShape.HashCode(INT_MAX)] = col;
            }

            // getting names
            Handle(TDataStd_Name) name;
            if (label.FindAttribute(TDataStd_Name::GetID(),name)) {
                TCollection_ExtendedString extstr = name->Get();
                char* str = new char[extstr.LengthOfCString()+1];
                extstr.ToUTF8CString(str);
                std::string label(str);
                if (!label.empty())
                    myNameMap[aShape.HashCode(INT_MAX)] = label;
                delete [] str;
            }

            if (label.HasChild()) {
                TDF_ChildIterator it;
                for (it.Initialize(label); it.More(); it.Next()) {
                    loadShapes(it.Value());
                }
            }
        }
    }

private:
    Handle_TDocStd_Document hdoc;
    App::Document* doc;
    Handle_XCAFDoc_ShapeTool aShapeTool;
    Handle_XCAFDoc_ColorTool hColors;
    std::string default_name;
    std::map<Standard_Integer, TopoDS_Shape> mySolids;
    std::map<Standard_Integer, TopoDS_Shape> myShells;
    std::map<Standard_Integer, TopoDS_Shape> myCompds;
    std::map<Standard_Integer, TopoDS_Shape> myShapes;
    std::map<Standard_Integer, Quantity_Color> myColorMap;
    std::map<Standard_Integer, std::string> myNameMap;
};

/* module functions */
static PyObject * importer(PyObject *self, PyObject *args)
{
    const char* Name;
    const char* DocName;
    if (!PyArg_ParseTuple(args, "ss",&Name,&DocName))
        return 0;

    PY_TRY {
        //Base::Console().Log("Insert in Part with %s",Name);
        Base::FileInfo file(Name);

        App::Document *pcDoc = App::GetApplication().getDocument(DocName);
        if (!pcDoc) {
            pcDoc = App::GetApplication().newDocument(DocName);
        }

        Handle(XCAFApp_Application) hApp = XCAFApp_Application::GetApplication();
        Handle(TDocStd_Document) hDoc;
        hApp->NewDocument(TCollection_ExtendedString("MDTV-CAF"), hDoc);

        if (file.hasExtension("stp") || file.hasExtension("step")) {
            STEPCAFControl_Reader aReader;
            aReader.SetColorMode(true);
            aReader.SetNameMode(true);
            aReader.SetLayerMode(true);
            if (aReader.ReadFile((Standard_CString)Name) != IFSelect_RetDone) {
                PyErr_SetString(PyExc_Exception, "cannot read STEP file");
                return 0;
            }

            Handle_Message_ProgressIndicator pi = new Part::ProgressIndicator(100);
            aReader.Reader().WS()->MapReader()->SetProgress(pi);
            pi->NewScope(100, "Reading STEP file...");
            pi->Show();
            aReader.Transfer(hDoc);
            pi->EndScope();
        }
        else if (file.hasExtension("igs") || file.hasExtension("iges")) {
            IGESControl_Controller::Init();
            Interface_Static::SetIVal("read.surfacecurve.mode",3);
            IGESCAFControl_Reader aReader;
            if (aReader.ReadFile((Standard_CString)Name) != IFSelect_RetDone) {
                PyErr_SetString(PyExc_Exception, "cannot read IGES file");
                return 0;
            }

            Handle_Message_ProgressIndicator pi = new Part::ProgressIndicator(100);
            aReader.WS()->MapReader()->SetProgress(pi);
            pi->NewScope(100, "Reading IGES file...");
            pi->Show();
            aReader.Transfer(hDoc);
            pi->EndScope();
        }
        else {
            PyErr_SetString(PyExc_Exception, "no supported file format");
            return 0;
        }

        ImportXCAF xcaf(hDoc, pcDoc, file.fileNamePure());
        xcaf.loadShapes();
        pcDoc->recompute();

    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
    PY_CATCH

    Py_Return;
}

static PyObject * exporter(PyObject *self, PyObject *args)
{
    PyObject* object;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os",&object,&filename))
        return NULL;

    PY_TRY {
        Handle(XCAFApp_Application) hApp = XCAFApp_Application::GetApplication();
        Handle(TDocStd_Document) hDoc;
        hApp->NewDocument(TCollection_ExtendedString("MDTV-CAF"), hDoc);
        Handle_XCAFDoc_ShapeTool hShapeTool = XCAFDoc_DocumentTool::ShapeTool(hDoc->Main());
        Handle_XCAFDoc_ColorTool hColors = XCAFDoc_DocumentTool::ColorTool(hDoc->Main());

        TDF_Label rootLabel= TDF_TagSource::NewChild(hDoc->Main());

        Py::List list(object);
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            PyObject* item = (*it).ptr();
            if (PyObject_TypeCheck(item, &(App::DocumentObjectPy::Type))) {
                App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(item)->getDocumentObjectPtr();
                if (obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
                    Part::Feature* part = static_cast<Part::Feature*>(obj);
                    const TopoDS_Shape& shape = part->Shape.getValue();

                    // Add shape and name
                    //TDF_Label shapeLabel = hShapeTool->AddShape(shape, Standard_False);
                    TDF_Label shapeLabel= TDF_TagSource::NewChild(rootLabel);
                    TDataStd_Shape::Set(shapeLabel, shape);
                    TDataStd_Name::Set(shapeLabel, TCollection_ExtendedString(part->Label.getValue(), 1));

                    // Add color information
                    Quantity_Color col;
                    Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(part);
                    bool per_face = false;
                    if (vp && vp->isDerivedFrom(PartGui::ViewProviderPartExt::getClassTypeId())) {
                        const std::vector<App::Color>& c = static_cast<PartGui::ViewProviderPartExt*>
                            (vp)->DiffuseColor.getValues();
                        // define color per face
                        if (c.size() > 1) {
                            per_face = true;
                            std::set<int> face_index;
                            TopTools_IndexedMapOfShape faces;
                            TopExp_Explorer xp(shape,TopAbs_FACE);
                            while (xp.More()) {
                                face_index.insert(faces.Add(xp.Current()));
                                xp.Next();
                            }

                            xp.Init(shape,TopAbs_FACE);
                            while (xp.More()) {
                                int index = faces.FindIndex(xp.Current());
                                if (face_index.find(index) != face_index.end()) {
                                    face_index.erase(index);
                                    TDF_Label faceLabel= TDF_TagSource::NewChild(shapeLabel);
                                    TDataStd_Shape::Set(faceLabel, xp.Current());

                                    const App::Color& color = c[index-1];
                                    Quantity_Parameter mat[3];
                                    mat[0] = color.r;
                                    mat[1] = color.g;
                                    mat[2] = color.b;
                                    col.SetValues(mat[0],mat[1],mat[2],Quantity_TOC_RGB);
                                    hColors->SetColor(faceLabel, col, XCAFDoc_ColorSurf);
                                }
                                xp.Next();
                            }
                        }
                    }
                    if (!per_face && vp && vp->isDerivedFrom(PartGui::ViewProviderPart::getClassTypeId())) {
                        App::Color color = static_cast<PartGui::ViewProviderPart*>(vp)->ShapeColor.getValue();
                        Quantity_Parameter mat[3];
                        mat[0] = color.r;
                        mat[1] = color.g;
                        mat[2] = color.b;
                        col.SetValues(mat[0],mat[1],mat[2],Quantity_TOC_RGB);
                        hColors->SetColor(shapeLabel, col, XCAFDoc_ColorGen);
                    }
                }
                else {
                    Base::Console().Message("'%s' is not a shape, export will be ignored.\n", obj->Label.getValue());
                }
            }
        }

        Base::FileInfo file(filename);
        if (file.hasExtension("stp") || file.hasExtension("step")) {
            STEPCAFControl_Writer writer;
            writer.Transfer(hDoc, STEPControl_AsIs);
            writer.Write(filename);
        }
        else if (file.hasExtension("igs") || file.hasExtension("iges")) {
            IGESControl_Controller::Init();
            IGESCAFControl_Writer writer;
            writer.Transfer(hDoc);
            writer.Write(filename);
        }
    }
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        PyErr_SetString(PyExc_Exception, e->GetMessageString());
        return 0;
    }
    PY_CATCH

    Py_Return;
}

/* registration table  */
struct PyMethodDef ImportGui_Import_methods[] = {
    {"insert"     ,importer  ,METH_VARARGS,
     "insert(string,string) -- Insert the file into the given document."},
    {"export"     ,exporter  ,METH_VARARGS,
     "export(list,string) -- Export a list of objects into a single file."},
    {NULL, NULL}                   /* end of table marker */
};
