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
#include <Mod/Part/App/TopoShapePy.h>

#include <sstream>
#include <StdMeshers_Arithmetic1D.hxx>
#include <StdMeshers_AutomaticLength.hxx>
#include <StdMeshers_MaxLength.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_MaxElementArea.hxx>
#include <StdMeshers_NotConformAllowed.hxx>
#include <StdMeshers_QuadranglePreference.hxx>
#include <StdMeshers_Quadrangle_2D.hxx>
#include <StdMeshers_Regular_1D.hxx>

#include <StdMeshers_UseExisting_1D2D.hxx>
#include <StdMeshers_CompositeSegment_1D.hxx>
#include <StdMeshers_CompositeHexa_3D.hxx>
#include <StdMeshers_Deflection1D.hxx>
#include <StdMeshers_Hexa_3D.hxx>
#include <StdMeshers_LayerDistribution.hxx>
#include <StdMeshers_LengthFromEdges.hxx>
#include <StdMeshers_MaxElementVolume.hxx>
#include <StdMeshers_MEFISTO_2D.hxx>
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
#include <StdMeshers_QuadraticMesh.hxx>
#include <StdMeshers_RadialPrism_3D.hxx>
#include <StdMeshers_SegmentAroundVertex_0D.hxx>
#include <StdMeshers_SegmentLengthAroundVertex.hxx>
#include <StdMeshers_StartEndLength.hxx>
#include <StdMeshers_TrianglePreference.hxx>

using namespace Fem;


HypothesisPy::HypothesisPy(boost::shared_ptr<SMESH_Hypothesis> h)
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

    add_varargs_method("setLibName", &SMESH_HypothesisPy<T>::setLibName, "setLibName(String)");
    add_varargs_method("getLibName", &SMESH_HypothesisPy<T>::getLibName, "String getLibName()");
    add_varargs_method("setParameters", &SMESH_HypothesisPy<T>::setParameters, "setParameters(String)");
    add_varargs_method("getParameters", &SMESH_HypothesisPy<T>::getParameters, "String getParameters()");
    add_varargs_method("setLastParameters", &SMESH_HypothesisPy<T>::setLastParameters, "setLastParameters(String)");
    add_varargs_method("getLastParameters", &SMESH_HypothesisPy<T>::getLastParameters, "String getLastParameters()");
    add_varargs_method("clearParameters", &SMESH_HypothesisPy<T>::clearParameters, "clearParameters()");
    add_varargs_method("isAuxiliary", &SMESH_HypothesisPy<T>::isAuxiliary, "Bool isAuxiliary()");
    add_varargs_method("setParametersByMesh", &SMESH_HypothesisPy<T>::setParametersByMesh, "setParametersByMesh(Mesh,Shape)");
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
Py::Object SMESH_HypothesisPy<T>::setLibName(const Py::Tuple& args)
{
    std::string libName = (std::string)Py::String(args[0]);
    hypothesis<SMESH_Hypothesis>()->SetLibName(libName.c_str());
    return Py::None();
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::getLibName(const Py::Tuple& args)
{
    return Py::String(hypothesis<SMESH_Hypothesis>()->GetLibName());
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::setParameters(const Py::Tuple& args)
{
    std::string paramName = (std::string)Py::String(args[0]);
    hypothesis<SMESH_Hypothesis>()->SetParameters(paramName.c_str());
    return Py::None();
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::getParameters(const Py::Tuple& args)
{
    return Py::String(hypothesis<SMESH_Hypothesis>()->GetParameters());
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::setLastParameters(const Py::Tuple& args)
{
    std::string paramName = (std::string)Py::String(args[0]);
    hypothesis<SMESH_Hypothesis>()->SetLastParameters(paramName.c_str());
    return Py::None();
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::getLastParameters(const Py::Tuple& args)
{
    return Py::String(hypothesis<SMESH_Hypothesis>()->GetLastParameters());
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::clearParameters(const Py::Tuple& args)
{
    hypothesis<SMESH_Hypothesis>()->ClearParameters();
    return Py::None();
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::setParametersByMesh(const Py::Tuple& args)
{
    PyObject *mesh, *shape;
    if (!PyArg_ParseTuple(args.ptr(), "O!O!",
        &(Fem::FemMeshPy::Type), &mesh,
        &(Part::TopoShapePy::Type), &shape))
        throw Py::Exception();
    Fem::FemMesh* m = static_cast<Fem::FemMeshPy*>(mesh)->getFemMeshPtr();
    const TopoDS_Shape& s = static_cast<Part::TopoShapePy*>(shape)->getTopoShapePtr()->_Shape;
    return Py::Boolean(hypothesis<SMESH_Hypothesis>()->SetParametersByMesh(m->getSMesh(), s));
}

template<class T>
Py::Object SMESH_HypothesisPy<T>::isAuxiliary(const Py::Tuple& args)
{
    return Py::Boolean(hypothesis<SMESH_Hypothesis>()->IsAuxiliary());
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
    hypothesis<StdMeshers_Arithmetic1D>()->
        SetLength((double)Py::Float(args[0]), (bool)Py::Boolean(args[1]));
    return Py::None();
}

Py::Object StdMeshers_Arithmetic1DPy::getLength(const Py::Tuple& args)
{
    int start;
    if (!PyArg_ParseTuple(args.ptr(), "i",&start))
        throw Py::Exception();
    return Py::Float(hypothesis<StdMeshers_Arithmetic1D>()->
        GetLength(start ? true : false));
}

// ----------------------------------------------------------------------------

void StdMeshers_AutomaticLengthPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_AutomaticLength");
    behaviors().doc("StdMeshers_AutomaticLength");

    add_varargs_method("setFineness", &StdMeshers_AutomaticLengthPy::setFineness, "setFineness()");
    add_varargs_method("getFineness", &StdMeshers_AutomaticLengthPy::getFineness, "getFineness()");
    add_varargs_method("getLength", &StdMeshers_AutomaticLengthPy::getLength, "getLength()");
}

StdMeshers_AutomaticLengthPy::StdMeshers_AutomaticLengthPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(0)
{
}

StdMeshers_AutomaticLengthPy::~StdMeshers_AutomaticLengthPy()
{
}

Py::Object StdMeshers_AutomaticLengthPy::setFineness(const Py::Tuple& args)
{
    double fine = (double)Py::Float(args[0]);
    hypothesis<StdMeshers_AutomaticLength>()->SetFineness(fine);
    return Py::None();
}

Py::Object StdMeshers_AutomaticLengthPy::getFineness(const Py::Tuple& args)
{
    return Py::Float(hypothesis<StdMeshers_AutomaticLength>()->GetFineness());
}

namespace Py {
    typedef ExtensionObject<Fem::FemMeshPy>         FemMesh;
    typedef ExtensionObject<Part::TopoShapePy>      TopoShape;
    template<> bool FemMesh::accepts (PyObject *pyob) const
    {
        return (pyob && PyObject_TypeCheck(pyob, &(Fem::FemMeshPy::Type)));
    }
    template<> bool TopoShape::accepts (PyObject *pyob) const
    {
        return (pyob && PyObject_TypeCheck(pyob, &(Part::TopoShapePy::Type)));
    }
}

Py::Object StdMeshers_AutomaticLengthPy::getLength(const Py::Tuple& args)
{
    Py::FemMesh mesh(args[0]);
    Py::Object shape_or_double(args[1]);

    Fem::FemMesh* m = mesh.extensionObject()->getFemMeshPtr();
    if (shape_or_double.type() == Py::Float().type()) {
        double len = (double)Py::Float(shape_or_double);
        return Py::Float(hypothesis<StdMeshers_AutomaticLength>()->GetLength(m->getSMesh(),len));
    }
    else {
        Py::TopoShape shape(shape_or_double);
        const TopoDS_Shape& s = shape.extensionObject()->getTopoShapePtr()->_Shape;
        return Py::Float(hypothesis<StdMeshers_AutomaticLength>()->GetLength(m->getSMesh(),s));
    }

    throw Py::Exception();
}

// ----------------------------------------------------------------------------

void StdMeshers_NotConformAllowedPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_NotConformAllowed");
    behaviors().doc("StdMeshers_NotConformAllowed");
}

StdMeshers_NotConformAllowedPy::StdMeshers_NotConformAllowedPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_NotConformAllowed(hypId, studyId, gen))
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
    hypothesis<StdMeshers_MaxLength>()->SetLength((double)Py::Float(args[0]));
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::getLength(const Py::Tuple& args)
{
    return Py::Float(hypothesis<StdMeshers_MaxLength>()->GetLength());
}

Py::Object StdMeshers_MaxLengthPy::havePreestimatedLength(const Py::Tuple& args)
{
    return Py::Boolean(hypothesis<StdMeshers_MaxLength>()->HavePreestimatedLength());
}

Py::Object StdMeshers_MaxLengthPy::getPreestimatedLength(const Py::Tuple& args)
{
    return Py::Float(hypothesis<StdMeshers_MaxLength>()->GetPreestimatedLength());
}

Py::Object StdMeshers_MaxLengthPy::setPreestimatedLength(const Py::Tuple& args)
{
    hypothesis<StdMeshers_MaxLength>()->SetPreestimatedLength((double)Py::Float(args[0]));
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::setUsePreestimatedLength(const Py::Tuple& args)
{
    hypothesis<StdMeshers_MaxLength>()->SetUsePreestimatedLength((bool)Py::Boolean(args[0]));
    return Py::None();
}

Py::Object StdMeshers_MaxLengthPy::getUsePreestimatedLength(const Py::Tuple& args)
{
    return Py::Boolean(hypothesis<StdMeshers_MaxLength>()->GetUsePreestimatedLength());
}

// ----------------------------------------------------------------------------

void StdMeshers_LocalLengthPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_LocalLength");
    behaviors().doc("StdMeshers_LocalLength");

    add_varargs_method("setLength", &StdMeshers_LocalLengthPy::setLength, "setLength()");
    add_varargs_method("getLength", &StdMeshers_LocalLengthPy::getLength, "getLength()");
    add_varargs_method("setPrecision", &StdMeshers_LocalLengthPy::setPrecision, "setPrecision()");
    add_varargs_method("getPrecision", &StdMeshers_LocalLengthPy::getPrecision, "getPrecision()");
}

StdMeshers_LocalLengthPy::StdMeshers_LocalLengthPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_LocalLength(hypId, studyId, gen))
{
}

StdMeshers_LocalLengthPy::~StdMeshers_LocalLengthPy()
{
}

Py::Object StdMeshers_LocalLengthPy::setLength(const Py::Tuple& args)
{
    hypothesis<StdMeshers_LocalLength>()->SetLength((double)Py::Float(args[0]));
    return Py::None();
}

Py::Object StdMeshers_LocalLengthPy::getLength(const Py::Tuple& args)
{
    return Py::Float(hypothesis<StdMeshers_LocalLength>()->GetLength());
}

Py::Object StdMeshers_LocalLengthPy::setPrecision(const Py::Tuple& args)
{
    hypothesis<StdMeshers_LocalLength>()->SetPrecision((double)Py::Float(args[0]));
    return Py::None();
}

Py::Object StdMeshers_LocalLengthPy::getPrecision(const Py::Tuple& args)
{
    return Py::Float(hypothesis<StdMeshers_LocalLength>()->GetPrecision());
}

// ----------------------------------------------------------------------------

void StdMeshers_MaxElementAreaPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_MaxElementArea");
    behaviors().doc("StdMeshers_MaxElementArea");

    add_varargs_method("setMaxArea", &StdMeshers_MaxElementAreaPy::setMaxArea, "setMaxArea()");
    add_varargs_method("getMaxArea", &StdMeshers_MaxElementAreaPy::getMaxArea, "getMaxArea()");
}

StdMeshers_MaxElementAreaPy::StdMeshers_MaxElementAreaPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_MaxElementArea(hypId, studyId, gen))
{
}

StdMeshers_MaxElementAreaPy::~StdMeshers_MaxElementAreaPy()
{
}

Py::Object StdMeshers_MaxElementAreaPy::setMaxArea(const Py::Tuple& args)
{
    hypothesis<StdMeshers_MaxElementArea>()->SetMaxArea((double)Py::Float(args[0]));
    return Py::None();
}

Py::Object StdMeshers_MaxElementAreaPy::getMaxArea(const Py::Tuple& args)
{
    return Py::Float(hypothesis<StdMeshers_MaxElementArea>()->GetMaxArea());
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

// ----------------------------------------------------------------------------

void StdMeshers_Quadrangle_2DPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_Quadrangle_2D");
    behaviors().doc("StdMeshers_Quadrangle_2D");
}

StdMeshers_Quadrangle_2DPy::StdMeshers_Quadrangle_2DPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_Quadrangle_2D(hypId, studyId, gen))
{
}

StdMeshers_Quadrangle_2DPy::~StdMeshers_Quadrangle_2DPy()
{
}

// ----------------------------------------------------------------------------

void StdMeshers_Regular_1DPy::init_type(void)
{
    SMESH_HypothesisPyBase::init_type();
    behaviors().name("StdMeshers_Regular_1D");
    behaviors().doc("StdMeshers_Regular_1D");
}

StdMeshers_Regular_1DPy::StdMeshers_Regular_1DPy(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_HypothesisPyBase(new StdMeshers_Regular_1D(hypId, studyId, gen))
{
}

StdMeshers_Regular_1DPy::~StdMeshers_Regular_1DPy()
{
}
