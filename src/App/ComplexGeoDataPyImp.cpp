
#include "PreCompiled.h"

#include "ComplexGeoData.h"

// inclusion of the generated files (generated out of ComplexGeoDataPy.xml)
#include "ComplexGeoDataPy.h"
#include "ComplexGeoDataPy.cpp"
#include <Base/BoundBoxPy.h>
#include <Base/MatrixPy.h>
#include <Base/PlacementPy.h>

using namespace Data;
using namespace Base;

// returns a string which represent the object e.g. when printed in python
const char *ComplexGeoDataPy::representation(void) const
{
    return "<ComplexGeoData object>";
}

Py::Object ComplexGeoDataPy::getBoundBox(void) const
{
    return Py::Object(new BoundBoxPy(new BoundBox3d(getComplexGeoDataPtr()->getBoundBox())));
}

Py::Object ComplexGeoDataPy::getPlacement(void) const
{
    Base::Placement trf = getComplexGeoDataPtr()->getPlacement();
    return Py::Object(new Base::PlacementPy(trf));
}

void  ComplexGeoDataPy::setPlacement(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::PlacementPy::Type))) {
        Base::Placement* trf = static_cast<Base::PlacementPy*>(p)->getPlacementPtr();
        getComplexGeoDataPtr()->setPlacement(*trf);
    }
    else {
        std::string error = std::string("type must be 'Placement', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

Py::Object ComplexGeoDataPy::getMatrix(void) const
{
    Base::Matrix4D mat = getComplexGeoDataPtr()->getTransform();
    return Py::Object(new Base::MatrixPy(mat));
}

void  ComplexGeoDataPy::setMatrix(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::MatrixPy::Type))) {
        Base::Matrix4D mat = static_cast<Base::MatrixPy*>(p)->value();
        getComplexGeoDataPtr()->setTransform(mat);
    }
    else {
        std::string error = std::string("type must be 'Matrix', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

PyObject *ComplexGeoDataPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ComplexGeoDataPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}
