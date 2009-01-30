/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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


#ifndef GUI_VIEW3DPY_H
#define GUI_VIEW3DPY_H

#include <Base/PyObjectBase.h>
#include <CXX/Extensions.hxx>

class SoEventCallback;

namespace Gui {

class View3DInventor;

class View3DInventorPy : public Py::PythonExtension<View3DInventorPy>
{
public:
    static void init_type(void);    // announce properties and methods

    View3DInventorPy(View3DInventor *vi);
    ~View3DInventorPy();

    Py::Object repr();
    Py::Object getattr(const char *);
    int setattr(const char *, const Py::Object &);

    Py::Object message(const Py::Tuple&);
    Py::Object fitAll(const Py::Tuple&);
    Py::Object viewBottom(const Py::Tuple&);
    Py::Object viewFront(const Py::Tuple&);
    Py::Object viewLeft(const Py::Tuple&);
    Py::Object viewRear(const Py::Tuple&);
    Py::Object viewRight(const Py::Tuple&);
    Py::Object viewTop(const Py::Tuple&);
    Py::Object viewAxometric(const Py::Tuple&);
    Py::Object dump(const Py::Tuple&);
    Py::Object setStereoType(const Py::Tuple&);
    Py::Object getStereoType(const Py::Tuple&);
    Py::Object listStereoTypes(const Py::Tuple&);
    Py::Object saveImage(const Py::Tuple&);
    Py::Object saveVectorGraphic(const Py::Tuple&);
    Py::Object getCamera(const Py::Tuple&);
    Py::Object setCamera(const Py::Tuple&);
    Py::Object getCameraType(const Py::Tuple&);
    Py::Object setCameraType(const Py::Tuple&);
    Py::Object listCameraTypes(const Py::Tuple&);
    Py::Object getCursorPos(const Py::Tuple&);
    Py::Object getObjectInfo(const Py::Tuple&);
    Py::Object getSize(const Py::Tuple&);
    Py::Object getPoint(const Py::Tuple&);
    Py::Object addEventCallback(const Py::Tuple&);
    Py::Object removeEventCallback(const Py::Tuple&);
    Py::Object setAnnotation(const Py::Tuple&);
    Py::Object removeAnnotation(const Py::Tuple&);
    Py::Object getSceneGraph(const Py::Tuple&);
    Py::Object getViewer(const Py::Tuple&);
    Py::Object addEventCallbackPivy(const Py::Tuple&);
    Py::Object removeEventCallbackPivy(const Py::Tuple&);

private:
    static void eventCallback(void * ud, SoEventCallback * n);
    static void eventCallbackPivy(void * ud, SoEventCallback * n);

private:
    typedef PyObject* (*method_varargs_handler)(PyObject *_self, PyObject *_args);
    static method_varargs_handler pycxx_handler;
    static PyObject *method_varargs_ext_handler(PyObject *_self, PyObject *_args);

private:
    View3DInventor* _view;
    friend class View3DInventor;
};

/** The View3D python class 
 */
class GuiExport View3DPy :public Base::PyObjectBase
{
    /// always start with Py_Header
    Py_Header;

protected:
    ~View3DPy();

public:
    View3DPy(View3DInventor *pcView, PyTypeObject *T = &Type);
    static PyObject *PyMake(PyObject *, PyObject *);

    //---------------------------------------------------------------------
    // python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
    //---------------------------------------------------------------------

    virtual PyObject *_repr(void);  				// the representation
    PyObject *_getattr(char *attr);					// __getattr__ function
    int _setattr(char *attr, PyObject *value);		// __setattr__ function
    PYFUNCDEF_D(View3DPy,message)
    PYFUNCDEF_D(View3DPy,fitAll)
    PYFUNCDEF_D(View3DPy,viewBottom)
    PYFUNCDEF_D(View3DPy,viewFront)
    PYFUNCDEF_D(View3DPy,viewLeft)
    PYFUNCDEF_D(View3DPy,viewRear)
    PYFUNCDEF_D(View3DPy,viewRight)
    PYFUNCDEF_D(View3DPy,viewTop)
    PYFUNCDEF_D(View3DPy,viewAxometric)
    PYFUNCDEF_D(View3DPy,dump)
    PYFUNCDEF_D(View3DPy,setStereoType)
    PYFUNCDEF_D(View3DPy,getStereoType)
    PYFUNCDEF_D(View3DPy,listStereoTypes)
    PYFUNCDEF_D(View3DPy,saveImage)
    PYFUNCDEF_D(View3DPy,saveVectorGraphic)
    PYFUNCDEF_D(View3DPy,getCamera)
    PYFUNCDEF_D(View3DPy,setCamera)
    PYFUNCDEF_D(View3DPy,getCameraType)
    PYFUNCDEF_D(View3DPy,setCameraType)
    PYFUNCDEF_D(View3DPy,listCameraTypes)
    PYFUNCDEF_D(View3DPy,getCursorPos)
    PYFUNCDEF_D(View3DPy,getObjectInfo)
    PYFUNCDEF_D(View3DPy,getSize)
    PYFUNCDEF_D(View3DPy,getPoint)
    PYFUNCDEF_D(View3DPy,addEventCallback)
    PYFUNCDEF_D(View3DPy,removeEventCallback)
    PYFUNCDEF_D(View3DPy,setAnnotation)
    PYFUNCDEF_D(View3DPy,removeAnnotation)
    PYFUNCDEF_D(View3DPy,getSceneGraph)
    PYFUNCDEF_D(View3DPy,getViewer)
    PYFUNCDEF_D(View3DPy,addEventCallbackSWIG)
    PYFUNCDEF_D(View3DPy,removeEventCallbackSWIG)

private:
    static void eventCallback(void * ud, SoEventCallback * n);
    static void eventCallbackSWIG(void * ud, SoEventCallback * n);
    Gui::View3DInventor *_pcView;
};

} // namespace Gui

#endif //GUI_VIEW3DPY_H
