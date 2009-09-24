
#include "PreCompiled.h"


#include <Base/PlacementPy.h>
#include <Base/Placement.h>
#include <Base/Exception.h>

#include "Mod/Robot/App/Waypoint.h"


// inclusion of the generated files (generated out of WaypointPy.xml)
#include "WaypointPy.h"
#include "WaypointPy.cpp"

using namespace Robot;
using namespace Base;

// returns a string which represents the object e.g. when printed in python
std::string WaypointPy::representation(void) const
{
    double A,B,C;
    PlacementPy::PointerType ptr = reinterpret_cast<PlacementPy::PointerType>(_pcTwinPointer);
    std::stringstream str;
    ptr->getRotation().getEuler(A,B,C);

    str << "Waypoint [Pos=(";
    str << ptr->getPosition().x << ","<< ptr->getPosition().y << "," << ptr->getPosition().z;
    str << "), Euler=(" << A << "," << B << "," << C << ")]";

    return str.str();
}

PyObject *WaypointPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of WaypointPy and the Twin object 
    return new WaypointPy(new Waypoint);
}

// constructor method
int WaypointPy::PyInit(PyObject* args, PyObject* kwd)
{
    PyObject *pos;
    char *name="P";
    char *type = "PTP";
    float vel = 100;
    int cont = 0;
    int tool=0;
    int base=0;

    static char *kwlist[] = {"type","name", "vel", "cont", "tool", "base",  NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwd, "O!|ssfiii", kwlist,
                                      &(Base::PlacementPy::Type), &pos, // the placement object
                                      &type, &name, &vel, &cont, &tool, &base ))
        return -1;

    getWaypointPtr()->EndPos = *static_cast<Base::PlacementPy*>(pos)->getPlacementPtr();
    getWaypointPtr()->Name   = name;
    std::string typeStr(type);
    if(typeStr=="PTP")
        getWaypointPtr()->Type = Waypoint::PTP;
    else if(typeStr=="LIN")
        getWaypointPtr()->Type = Waypoint::LIN;
    else if(typeStr=="CIRC")
        getWaypointPtr()->Type = Waypoint::CIRC;
    else if(typeStr=="WAIT")
        getWaypointPtr()->Type = Waypoint::WAIT;
    else 
        getWaypointPtr()->Type = Waypoint::UNDEF;
    getWaypointPtr()->Velocity = vel;
    getWaypointPtr()->Cont = cont?true:false;
    getWaypointPtr()->Tool = tool;
    getWaypointPtr()->Base = base;

    return 0;
}




Py::String WaypointPy::getName(void) const
{
    return Py::String(getWaypointPtr()->Name.c_str());
}

void WaypointPy::setName(Py::String arg)
{
    getWaypointPtr()->Name = arg.as_std_string();
}

Py::String WaypointPy::getType(void) const
{
    if(getWaypointPtr()->Type == Waypoint::PTP)
        return Py::String("PTP");
    else if(getWaypointPtr()->Type == Waypoint::LIN)
        return Py::String("LIN");
    else if(getWaypointPtr()->Type == Waypoint::CIRC)
        return Py::String("CIRC");
    else if(getWaypointPtr()->Type == Waypoint::WAIT)
        return Py::String("WAIT");
    else if(getWaypointPtr()->Type == Waypoint::UNDEF)
        return Py::String("UNDEF");

}

void WaypointPy::setType(Py::String arg)
{
    std::string typeStr(arg.as_std_string());
    if(typeStr=="PTP")
        getWaypointPtr()->Type = Waypoint::PTP;
    else if(typeStr=="LIN")
        getWaypointPtr()->Type = Waypoint::LIN;
    else if(typeStr=="CIRC")
        getWaypointPtr()->Type = Waypoint::CIRC;
    else if(typeStr=="WAIT")
        getWaypointPtr()->Type = Waypoint::WAIT;
    else
        throw Base::Exception("Unknown waypoint type! Only: PTP,LIN,CIRC,WAIT are allowed.");

}


Py::Object WaypointPy::getPos(void) const
{
    return Py::Object(new PlacementPy(new Placement(getWaypointPtr()->EndPos)));
}

void WaypointPy::setPos(Py::Object arg)
{


}

Py::Boolean WaypointPy::getCont(void) const
{
    return Py::Boolean();
}

void WaypointPy::setCont(Py::Boolean /*arg*/)
{

}

Py::Int WaypointPy::getTool(void) const
{
    return Py::Int();
}

void WaypointPy::setTool(Py::Int /*arg*/)
{

}

Py::Int WaypointPy::getBase(void) const
{
    return Py::Int();
}

void WaypointPy::setBase(Py::Int /*arg*/)
{

}

PyObject *WaypointPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int WaypointPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


