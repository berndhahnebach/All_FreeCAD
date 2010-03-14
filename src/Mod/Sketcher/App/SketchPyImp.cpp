
#include "PreCompiled.h"

#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/GeometryCurvePy.h>
#include <Mod/Part/App/LinePy.h>

#include "Sketch.h"

// inclusion of the generated files (generated out of SketchPy.xml)
#include "SketchPy.h"
#include "SketchPy.cpp"

using namespace Sketcher;
using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string SketchPy::representation(void) const
{
    return std::string("<Sketch object>");
}

PyObject *SketchPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of SketchPy and the Twin object 
    return new SketchPy(new Sketch());
}

// constructor method
int SketchPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

// +++ methodes implementer ++++++++++++++++++++++++++++++++++++++++++++++++

PyObject* SketchPy::solve(PyObject *args)
{
    getSketchPtr()->solve();
    Py_Return; 
}

PyObject* SketchPy::addGeometry(PyObject *args)
{
    PyObject *pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj))
        return 0;

    if (PyObject_TypeCheck(pcObj, &(LinePy::Type))) {
        GeomLineSegment *line = new GeomLineSegment(*static_cast<LinePy*>(pcObj)->getGeomLineSegmentPtr());
        this->getSketchPtr()->addGeometry(line);
    }
    Py_Return; 
}

PyObject* SketchPy::addHorizontalConstraint(PyObject *args)
{
    int index;
    char* name=0;
    if (!PyArg_ParseTuple(args, "i|s", &index,&name))
        return 0;

    return Py::new_reference_to(Py::Int(getSketchPtr()->addHorizontalConstraint(index,name)));

}

// +++ attributes implementer ++++++++++++++++++++++++++++++++++++++++++++++++

Py::Int SketchPy::getConstraint(void) const
{
    //return Py::Int();
    throw Py::AttributeError("Not yet implemented");
}

Py::Tuple SketchPy::getConstraints(void) const
{
    //return Py::Tuple();
    throw Py::AttributeError("Not yet implemented");
}

Py::Tuple SketchPy::getGeometries(void) const
{
    std::vector<Part::GeomCurve *> geoms = getSketchPtr()->getGeometry();

    Py::Tuple tuple(geoms.size());
    int i=0;
    for(std::vector<Part::GeomCurve *>::iterator it=geoms.begin();it!=geoms.end();++it,i++){
        if((*it)->getTypeId()== GeomLineSegment::getClassTypeId()){ // add a line
            GeomLineSegment *lineSeg = dynamic_cast<GeomLineSegment*>(*it);
            tuple[i] = Py::Object(new LinePy(lineSeg));
        }else{
            assert(0); // not implemented type in the sketch!
        }
    }
    return tuple;
}


// +++ custom attributes implementer ++++++++++++++++++++++++++++++++++++++++


PyObject *SketchPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int SketchPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


