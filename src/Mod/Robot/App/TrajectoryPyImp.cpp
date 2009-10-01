
#include "PreCompiled.h"

#include <Base/VectorPy.h>
#include <Base/PlacementPy.h>
#include "Mod/Robot/App/Trajectory.h"

// inclusion of the generated files (generated out of TrajectoryPy.xml)
#include "TrajectoryPy.h"
#include "TrajectoryPy.cpp"

using namespace Robot;

// returns a string which represents the object e.g. when printed in python
std::string TrajectoryPy::representation(void) const
{
    std::stringstream str;

    str << "Trajectory [";
    str << "size:" << getTrajectoryPtr()->getSize() << " ";
    str << "]";

    return str.str();
}

PyObject *TrajectoryPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of TrajectoryPy and the Twin object 
    return new TrajectoryPy(new Trajectory);
}

// constructor method
int TrajectoryPy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    touched = false;
    return 0;
}


PyObject* TrajectoryPy::insertWaypoint(PyObject * args)
{
/*  
    if (PyArg_ParseTuple(args, "O!", &(Base::PlacementPy::Type), &o)) {
        Base::Placement *plm = static_cast<Base::PlacementPy*>(o)->getPlacementPtr();
        *(getPlacementPtr()) = *plm;
        return 0;
    }

    PyErr_Clear();
    PyObject* d;
    double angle;
    if (PyArg_ParseTuple(args, "O!dO!", &(Base::VectorPy::Type), &d, &angle,
                                        &(Base::VectorPy::Type), &o)) {
        Base::Rotation rot(static_cast<Base::VectorPy*>(d)->value(), angle);
		*getPlacementPtr() = Base::Placement(static_cast<Base::VectorPy*>(o)->value(),rot);
        
        return 0;
    }

    PyErr_SetString(PyExc_Exception, "empty parameter list, matrix or placement expected");
    return -1;
*/

    PyObject *plm;
    if (!PyArg_ParseTuple(args, "O!", &(Base::PlacementPy::Type), &plm))
        return NULL;
    Base::Placement pos = (*static_cast<Base::PlacementPy*>(plm)->getPlacementPtr());

    getTrajectoryPtr()->addWaypoint(Robot::Waypoint("Pt",pos));
    getTrajectoryPtr()->generateTrajectory();
    //touched = true;

    return 0;
}

PyObject* TrajectoryPy::position(PyObject * args)
{
    double pos;
    if (!PyArg_ParseTuple(args, "d", &pos))
        return NULL;

    // check if trajectory touched! if yes, recompute.
    //if(touched == true){
    //    getTrajectoryPtr()->generateTrajectory();
    //    touched = false;
    //}


    return (new Base::PlacementPy(new Base::Placement(getTrajectoryPtr()->getPosition(pos))));

}

PyObject* TrajectoryPy::velocity(PyObject * args)
{
    double pos;
    if (!PyArg_ParseTuple(args, "d", &pos))
        return NULL;

    // check if trajectory touched! if yes, recompute.
    //if(touched == true){
    //    getTrajectoryPtr()->generateTrajectory();
    //    touched = false;
    //}
    // return velocity as float
    return Py::new_reference_to(Py::Float(getTrajectoryPtr()->getVelocity(pos)));
}



Py::Float TrajectoryPy::getDuration(void) const
{
    //if(touched == true){
    //    getTrajectoryPtr()->generateTrajectory();
    //    //touched = false;
    //}

    return Py::Float(getTrajectoryPtr()->getLength());
}

Py::List TrajectoryPy::getWaypoints(void) const
{
    // check if trajectory touched! if yes, recompute.
    //if(touched == true){
    //    getTrajectoryPtr()->generateTrajectory();
    //    //touched = false;
    //}
    return Py::List();
}

PyObject *TrajectoryPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int TrajectoryPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


