
#include "PreCompiled.h"

#include "Geom_TrimmedCurve.hxx"

// inclusion of the generated files (generated out of ArcPy.xml)
#include "ArcPy.h"
#include "ArcPy.cpp"
#include "CirclePy.h"

#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>
#include <GC_MakeArcOfCircle.hxx>

using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
const char *ArcPy::representation(void) const
{
    return "<Arc object>";
}

PyObject *ArcPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // never create such objects with the constructor
    return new ArcPy(new GeomTrimmedCurve());
}

// constructor method
int ArcPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject* o;
    double u1, u2;
    int sense=1;

    if (PyArg_ParseTuple(args, "O!dd|i", &(Part::CirclePy::Type), &o, &u1, &u2, &sense)) {
        try {
            Handle_Geom_Circle circle = Handle_Geom_Circle::DownCast
                (static_cast<CirclePy*>(o)->getGeomCirclePtr()->handle());
            GC_MakeArcOfCircle arc(circle->Circ(), u1, u2, sense);
            if (!arc.IsDone()) {
                PyErr_SetString(PyExc_Exception, gce_ErrorStatusText(arc.Status()));
                return -1;
            }

            getGeomTrimmedCurvePtr()->setHandle(arc.Value());
            return 0;
        }
        catch (const Standard_Failure& e) {
            PyErr_SetString(PyExc_Exception, e.GetMessageString());
            return -1;
        }
    }

    PyErr_SetString(PyExc_TypeError, "Arc constructor expects a curve and a parameter range");
    return -1;
}

PyObject *ArcPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ArcPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
