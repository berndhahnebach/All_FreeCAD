/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#include "HypothesisPy.h"
#include "FemMeshPy.h"

#include <sstream>
#include <StdMeshers_Arithmetic1D.hxx>
#include <StdMeshers_AutomaticLength.hxx>
#include <StdMeshers_MaxLength.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_UseExisting_1D2D.hxx>
#include <StdMeshers_CompositeSegment_1D.hxx>
#include <StdMeshers_CompositeHexa_3D.hxx>
#include <StdMeshers_Deflection1D.hxx>
#include <StdMeshers_Hexa_3D.hxx>
#include <StdMeshers_LayerDistribution.hxx>
#include <StdMeshers_LengthFromEdges.hxx>
#include <StdMeshers_MaxElementArea.hxx>
#include <StdMeshers_MaxElementVolume.hxx>
#include <StdMeshers_MEFISTO_2D.hxx>
#include <StdMeshers_NotConformAllowed.hxx>
#include <StdMeshers_NumberOfLayers.hxx>
#include <StdMeshers_NumberOfSegments.hxx>
#include <StdMeshers_Prism_3D.hxx>
#include <StdMeshers_Projection_1D.hxx>
#include <StdMeshers_Projection_2D.hxx>
#include <StdMeshers_Projection_3D.hxx>
#include <StdMeshers_ProjectionSource1D.hxx>
#include <StdMeshers_ProjectionSource2D.hxx>
#include <StdMeshers_ProjectionSource3D.hxx>
//#include <StdMeshers_Propagation.hxx>
#include <StdMeshers_QuadranglePreference.hxx>
#include <StdMeshers_Quadrangle_2D.hxx>
#include <StdMeshers_QuadraticMesh.hxx>
#include <StdMeshers_RadialPrism_3D.hxx>
#include <StdMeshers_Regular_1D.hxx>
#include <StdMeshers_SegmentAroundVertex_0D.hxx>
#include <StdMeshers_SegmentLengthAroundVertex.hxx>
#include <StdMeshers_StartEndLength.hxx>
#include <StdMeshers_TrianglePreference.hxx>

using namespace Fem;


HypothesisPy::HypothesisPy(SMESH_Hypothesis* h)
  : hyp(h)
{
}

HypothesisPy::~HypothesisPy()
{
}

// ----------------------------------------------------------------------------

template<class T>
void SMESH_HypothesisPy<T>::init_type(void)
{
    // you must have overwritten the virtual functions
    SMESH_HypothesisPy<T>::behaviors().supportRepr();
    SMESH_HypothesisPy<T>::behaviors().supportGetattr();
    SMESH_HypothesisPy<T>::behaviors().supportSetattr();
    SMESH_HypothesisPy<T>::behaviors().type_object()->tp_new = &PyMake;
}

template<class T>
SMESH_HypothesisPy<T>::SMESH_HypothesisPy(SMESH_Hypothesis* h) : hyp(h)
{
}

template<class T>
SMESH_HypothesisPy<T>::~SMESH_HypothesisPy()
{
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::getattr(const char *name)
{
    if (strcmp(name,"this") == 0)
        return Hypothesis(Py::asObject(new HypothesisPy(this->getHypothesis())));
    return Py::PythonExtension<T>::getattr(name);
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::repr()
{
    std::stringstream str;
    str << hyp->GetName() << ", " << hyp->GetID();
    return Py::String(str.str());
}

template<class T>
PyObject *SMESH_HypothesisPy<T>::PyMake(struct _typeobject *type, PyObject * args, PyObject * kwds)
{
    int hypId;
    PyObject* obj;
    if (!PyArg_ParseTuple(args, "iO!",&hypId,&(FemMeshPy::Type),&obj))
        return 0;
    FemMesh* mesh = static_cast<FemMeshPy*>(obj)->getFemMeshPtr();
    return new T(hypId, 1, mesh->getGenerator());
}

// ----------------------------------------------------------------------------

void StdMeshers_Arithmetic1DPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_Arithmetic1D");
    behaviors().doc("StdMeshers_Arithmetic1D");

    add_varargs_method("setLength", &StdMeshers_Arithmetic1DPy::setLength, "setLength()");
    add_varargs_method("getLength", &StdMeshers_Arithmetic1DPy::getLength, "getLength()");
}

StdMeshers_Arithmetic1DPy::StdMeshers_Arithmetic1DPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_Arithmetic1D(hypId, studyId, gen))
{
}

StdMeshers_Arithmetic1DPy::~StdMeshers_Arithmetic1DPy()
{
}

Py::Object StdMeshers_Arithmetic1DPy::setLength(const Py::Tuple& args)
{
    static_cast<StdMeshers_Arithmetic1D*>(getHypothesis())->
        SetLength((double)Py::Float(args[0]), (bool)Py::Boolean(args[1]));
    return Py::None();
}

Py::Object StdMeshers_Arithmetic1DPy::getLength(const Py::Tuple& args)
{
    int start;
    if (!PyArg_ParseTuple(args.ptr(), "i",&start))
        throw Py::Exception();
    return Py::Float(static_cast<StdMeshers_Arithmetic1D*>(getHypothesis())->
        GetLength(start ? true : false));
}

// ----------------------------------------------------------------------------

void StdMeshers_AutomaticLengthPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_AutomaticLength");
    behaviors().doc("StdMeshers_AutomaticLength");
}

StdMeshers_AutomaticLengthPy::StdMeshers_AutomaticLengthPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(0)
{
}

StdMeshers_AutomaticLengthPy::~StdMeshers_AutomaticLengthPy()
{
}

// ----------------------------------------------------------------------------

void StdMeshers_NotConformAllowedPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_NotConformAllowed");
    behaviors().doc("StdMeshers_NotConformAllowed");
}

StdMeshers_NotConformAllowedPy::StdMeshers_NotConformAllowedPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(0)
{
}

StdMeshers_NotConformAllowedPy::~StdMeshers_NotConformAllowedPy()
{
}

// ----------------------------------------------------------------------------

void StdMeshers_MaxLengthPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_MaxLength");
    behaviors().doc("StdMeshers_MaxLength");

    add_varargs_method("setLength", &StdMeshers_MaxLengthPy::setLength, "setLength()");
    add_varargs_method("getLength", &StdMeshers_MaxLengthPy::getLength, "getLength()");
    add_varargs_method("havePreestimatedLength", &StdMeshers_MaxLengthPy::havePreestimatedLength, "havePreestimatedLength()");
    add_varargs_method("getPreestimatedLength", &StdMeshers_MaxLengthPy::getPreestimatedLength, "getPreestimatedLength()");
    add_varargs_method("setPreestimatedLength", &StdMeshers_MaxLengthPy::setPreestimatedLength, "setPreestimatedLength()");
    add_varargs_method("setUsePreestimatedLength", &StdMeshers_MaxLengthPy::setUsePreestimatedLength, "setUsePreestimatedLength()");
    add_varargs_method("getUsePreestimatedLength", &StdMeshers_MaxLengthPy::getUsePreestimatedLength, "getUsePreestimatedLength()");
}

StdMeshers_MaxLengthPy::StdMeshers_MaxLengthPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_MaxLength(hypId, studyId, gen))
{
}

StdMeshers_MaxLengthPy::~StdMeshers_MaxLengthPy()
{
}

Py::Object StdMeshers_MaxLengthPy::setLength(const Py::Tuple& args)
{
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::getLength(const Py::Tuple& args)
{
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::havePreestimatedLength(const Py::Tuple& args)
{
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::getPreestimatedLength(const Py::Tuple& args)
{
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::setPreestimatedLength(const Py::Tuple& args)
{
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::setUsePreestimatedLength(const Py::Tuple& args)
{
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::getUsePreestimatedLength(const Py::Tuple& args)
{
    return Py::None();
}

// ----------------------------------------------------------------------------

void StdMeshers_QuadranglePreferencePy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_QuadranglePreference");
    behaviors().doc("StdMeshers_QuadranglePreference");
}

StdMeshers_QuadranglePreferencePy::StdMeshers_QuadranglePreferencePy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_QuadranglePreference(hypId, studyId, gen))
{
}

StdMeshers_QuadranglePreferencePy::~StdMeshers_QuadranglePreferencePy()
{
}
