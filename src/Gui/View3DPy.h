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


#include "../Base/PyObjectBase.h"

class SoEventCallback;

namespace Gui {

class View3DInventor;

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
    static void cleanupSWIG();

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
    PYFUNCDEF_D(View3DPy,setViewDirection)
    PYFUNCDEF_D(View3DPy,getViewDirection)
    PYFUNCDEF_D(View3DPy,setFocalDistance)
    PYFUNCDEF_D(View3DPy,getFocalDistance)
    PYFUNCDEF_D(View3DPy,dump)
    PYFUNCDEF_D(View3DPy,setStereoType)
    PYFUNCDEF_D(View3DPy,getStereoType)
    PYFUNCDEF_D(View3DPy,listStereoTypes)
    PYFUNCDEF_D(View3DPy,saveImage)
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
    PYFUNCDEF_D(View3DPy,addEventCallbackSWIG)
    PYFUNCDEF_D(View3DPy,removeEventCallbackSWIG)

private:
    static void eventCallback(void * ud, SoEventCallback * n);
    static void eventCallbackSWIG(void * ud, SoEventCallback * n);
    Gui::View3DInventor *_pcView;
};

} // namespace App

