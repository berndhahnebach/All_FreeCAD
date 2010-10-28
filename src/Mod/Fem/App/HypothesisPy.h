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

#ifndef FEM_HYPOTHESISPY_H
#define FEM_HYPOTHESISPY_H

#include <CXX/Extensions.hxx>
#include <boost/shared_ptr.hpp>

class SMESH_Hypothesis;
class SMESH_Gen;

namespace Fem {

class HypothesisPy : public Py::PythonExtension<HypothesisPy>
{
public:
    typedef Py::PythonExtension<HypothesisPy> HypothesisPyBase;
    HypothesisPy(SMESH_Hypothesis*);
    virtual ~HypothesisPy();
    SMESH_Hypothesis* getHypothesis() const
    { return hyp; }

private:
    SMESH_Hypothesis* hyp;
};

typedef Py::ExtensionObject<HypothesisPy> Hypothesis;

template <class T>
class SMESH_HypothesisPy : public Py::PythonExtension<T>
{
public:
    typedef SMESH_HypothesisPy<T> SMESH_HypothesisPyBase;
    static void init_type(void);    // announce properties and methods

    SMESH_HypothesisPy(SMESH_Hypothesis*);
    virtual ~SMESH_HypothesisPy();

    Py::Object getattr(const char *name);
    Py::Object repr();
    SMESH_Hypothesis* getHypothesis() const
    { return hyp.get(); }

private:
    static PyObject *PyMake(struct _typeobject *, PyObject *, PyObject *);

private:
    boost::shared_ptr<SMESH_Hypothesis> hyp;
};

class StdMeshers_Arithmetic1DPy : public SMESH_HypothesisPy<StdMeshers_Arithmetic1DPy>
{
public:
    static void init_type(void);
    StdMeshers_Arithmetic1DPy(int hypId, int studyId, SMESH_Gen* gen);
    ~StdMeshers_Arithmetic1DPy();

    Py::Object setLength(const Py::Tuple& args);
    Py::Object getLength(const Py::Tuple& args);
};

class StdMeshers_AutomaticLengthPy : public SMESH_HypothesisPy<StdMeshers_AutomaticLengthPy>
{
public:
    static void init_type(void);
    StdMeshers_AutomaticLengthPy(int hypId, int studyId, SMESH_Gen* gen);
    ~StdMeshers_AutomaticLengthPy();
};

class StdMeshers_NotConformAllowedPy : public SMESH_HypothesisPy<StdMeshers_NotConformAllowedPy>
{
public:
    static void init_type(void);
    StdMeshers_NotConformAllowedPy(int hypId, int studyId, SMESH_Gen* gen);
    ~StdMeshers_NotConformAllowedPy();
};

class StdMeshers_MaxLengthPy : public SMESH_HypothesisPy<StdMeshers_MaxLengthPy>
{
public:
    static void init_type(void);
    StdMeshers_MaxLengthPy(int hypId, int studyId, SMESH_Gen* gen);
    ~StdMeshers_MaxLengthPy();

    Py::Object setLength(const Py::Tuple& args);
    Py::Object getLength(const Py::Tuple& args);
    Py::Object havePreestimatedLength(const Py::Tuple& args);
    Py::Object getPreestimatedLength(const Py::Tuple& args);
    Py::Object setPreestimatedLength(const Py::Tuple& args);
    Py::Object setUsePreestimatedLength(const Py::Tuple& args);
    Py::Object getUsePreestimatedLength(const Py::Tuple& args);
};

class StdMeshers_QuadranglePreferencePy : public SMESH_HypothesisPy<StdMeshers_QuadranglePreferencePy>
{
public:
    static void init_type(void);
    StdMeshers_QuadranglePreferencePy(int hypId, int studyId, SMESH_Gen* gen);
    ~StdMeshers_QuadranglePreferencePy();
};

} // namespace Fem

#endif // FEM_HYPOTHESISPY_H
