
#include "PreCompiled.h"

#include <gp_Lin.hxx>
#include <Geom_Line.hxx>
#include <GC_MakeLine.hxx>

#include "Mod/Part/App/Geometry.h"
#include <Base/VectorPy.h>

// inclusion of the generated files (generated out of LinePy.xml)
#include "LinePy.h"
#include "LinePy.cpp"

using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
const char *LinePy::representation(void) const
{
    return "<GeomLine object>";
}

PyObject *LinePy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of LinePy and the Twin object 
    return new LinePy(new GeomLine);
}

// constructor method
int LinePy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject *pLine;
    PyObject *pV1, *pV2;
    if (PyArg_ParseTuple(args, "O!", &(LinePy::Type), &pLine)) {
        //LinePy* pcLine = static_cast<LinePy*>(pLine);
        //static_cast<LinePy*>(self)->_Line = pcLine->_Line;
        LinePy* pcLine = static_cast<LinePy*>(pLine);
        Handle_Geom_Line lin1 = Handle_Geom_Line::DownCast
            (pcLine->getGeomLinePtr()->handle());
        Handle_Geom_Line lin2 = Handle_Geom_Line::DownCast
            (this->getGeomLinePtr()->handle());
        lin2->SetLin(lin1->Lin());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!O!", &(Base::VectorPy::Type), &pV1,
                                       &(Base::VectorPy::Type), &pV2)) {
        Base::Vector3d v1 = static_cast<Base::VectorPy*>(pV1)->value();
        Base::Vector3d v2 = static_cast<Base::VectorPy*>(pV2)->value();
        GC_MakeLine ml(gp_Pnt(v1.x,v1.y,v1.z),
                       gp_Dir(v2.x,v2.y,v2.z));
        if (!ml.IsDone()) {
            PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(ml.Status()));
            return -1;
        }

        Handle_Geom_Line line = Handle_Geom_Line::DownCast(getGeomLinePtr()->handle());
        line->SetLin(ml.Value()->Lin());
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "")) {
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Line constructor accepts:\n"
        "-- empty parameter list\n"
        "-- Line\n"
        "-- Vector, Vector");
    return -1;
}

PyObject* LinePy::setStartPoint(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject* LinePy::setEndPoint(PyObject * /*args*/)
{
    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return 0;
}

PyObject *LinePy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int LinePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
