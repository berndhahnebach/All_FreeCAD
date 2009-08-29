/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <HLRBRep_HLRToShape.hxx>
# include <gp_Ax2.hxx>
# include <gp_Pnt.hxx>
# include <gp_Dir.hxx>
# include <HLRBRep_Algo.hxx>


#endif

#include <Mod/Part/App/TopoShapePy.h>
#include <Base/Console.h>


using namespace Part;

static PyObject * 
project(PyObject *self, PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(TopoShapePy::Type), &pcObj))     // convert args: Python->C
        return NULL;                             // NULL triggers exception

    PY_TRY {
        TopoShapePy* pShape = static_cast<TopoShapePy*>(pcObj);
        //TopoShape* shape = new MeshObject(*pShape->getTopoShapeObjectPtr());

        Handle( HLRBRep_Algo ) brep_hlr = new HLRBRep_Algo;
        brep_hlr->Add( pShape->getTopoShapePtr()->_Shape );

        gp_Ax2 transform(gp_Pnt(0,0,0),gp_Dir(0,0,1));
        HLRAlgo_Projector projector( transform );
        brep_hlr->Projector( projector );
        brep_hlr->Update();
        brep_hlr->Hide();

        // extracting the result sets:
        HLRBRep_HLRToShape shapes( brep_hlr );

        TopoDS_Shape OuterEdges;
        OuterEdges = shapes.OutLineVCompound();

        TopoDS_Shape InnerEdges;
        InnerEdges = shapes.VCompound();

        TopoDS_Shape HiddenOuterEdges;
        HiddenOuterEdges = shapes.OutLineHCompound();

        TopoDS_Shape HiddenInnerEdges;
        HiddenInnerEdges = shapes.HCompound();

        Py::List list;
        //list.append(Py::Object(new TopoShapePy(new TopoShape(OuterEdges))));
        list.append(Py::Object(new TopoShapePy(new TopoShape(InnerEdges))));
        //list.append(Py::Object(new TopoShapePy(new TopoShape(HiddenOuterEdges))));
        list.append(Py::Object(new TopoShapePy(new TopoShape(HiddenInnerEdges))));

        return Py::new_reference_to(list);

    } PY_CATCH;

}





/* registration table  */
struct PyMethodDef Drawing_methods[] = {
   {"project"       ,project      ,METH_VARARGS,
     "(Visibly,Hidden) = project(TopoShape) -- Project a shape and return the visible/invisible parts of it."},
    {NULL, NULL}        /* end of table marker */
};
