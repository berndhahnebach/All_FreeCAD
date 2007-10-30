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
# include <sstream>
#endif

#include <Base/PyObjectBase.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/PyCXX/Objects.hxx>

#include "Material.h"
#include "MaterialPy.h"

using namespace App;

/// Type structure of 
PyTypeObject MaterialPy::Type = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                                                  /*ob_size*/
    "App.Material",                                     /*tp_name*/
    sizeof(MaterialPy),                                 /*tp_basicsize*/
    0,                                                  /*tp_itemsize*/
    /* methods */
    PyDestructor,                                       /*tp_dealloc*/
    0,                                                  /*tp_print*/
    __getattr,                                          /*tp_getattr*/
    __setattr,                                          /*tp_setattr*/
    0,                                                  /*tp_compare*/
    __repr,                                             /*tp_repr*/
    0,                                                  /*tp_as_number*/
    0,                                                  /*tp_as_sequence*/
    0,                                                  /*tp_as_mapping*/
    0,                                                  /*tp_hash*/
    0,                                                  /*tp_call */
    0,                                                  /*tp_str  */
    0,                                                  /*tp_getattro*/
    0,                                                  /*tp_setattro*/
    /* --- Functions to access object as input/output buffer ---------*/
    0,                                                  /* tp_as_buffer */
    /* --- Flags to define presence of optional/expanded features */
    Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_CLASS,          /*tp_flags */
    "This is the Material class",                       /*tp_doc */
    0,                                                  /*tp_traverse */
    0,                                                  /*tp_clear */
    0,                                                  /*tp_richcompare */
    0,                                                  /*tp_weaklistoffset */
    0,                                                  /*tp_iter */
    0,                                                  /*tp_iternext */
    App::MaterialPy::Methods,                           /*tp_methods */
    0,                                                  /*tp_members */
    App::MaterialPy::GetterSetter,                      /*tp_getset */
    &Base::PyObjectBase::Type,                          /*tp_base */
    0,                                                  /*tp_dict */
    0,                                                  /*tp_descr_get */
    0,                                                  /*tp_descr_set */
    0,                                                  /*tp_dictoffset */
    0,                                                  /*tp_init */
    0,                                                  /*tp_alloc */
    0,                                                  /*tp_new */
    0,                                                  /*tp_free   Low-level free-memory routine */
    0,                                                  /*tp_is_gc  For PyObject_IS_GC */
    0,                                                  /*tp_bases */
    0,                                                  /*tp_mro    method resolution order */
    0,                                                  /*tp_cache */
    0,                                                  /*tp_subclasses */
    0                                                   /*tp_weaklist */
};

/// Methods structure of 
PyMethodDef MaterialPy::Methods[] = {
    {"set",
        (PyCFunction) staticCallback_set,
        Py_NEWARGS,
        "Set(string) -- Set the material.\n"
        "\n"
        "The material must be one of the following values:\n"
        "Brass, Bronze, Copper, Gold, Pewter, Plaster, Plastic, Silver, Steel, Stone, Shiny plastic,\n"
        "Satin, Metalized, Neon GNC, Chrome, Aluminium, Obsidian, Neon PHC, Jade, Ruby or Emerald.\n"
    },
    {NULL, NULL}		/* Sentinel */
};

/// Attribute structure of 
PyGetSetDef MaterialPy::GetterSetter[] = {
    {"AmbientColor",
        (getter) staticCallback_getAmbientColor,
        (setter) staticCallback_setAmbientColor, 
        "Ambient color",
        NULL
    },
    {"DiffuseColor",
        (getter) staticCallback_getDiffuseColor,
        (setter) staticCallback_setDiffuseColor, 
        "Diffuse color",
        NULL
    },
    {"SpecularColor",
        (getter) staticCallback_getSpecularColor,
        (setter) staticCallback_setSpecularColor, 
        "Speculat color",
        NULL
    },
    {"Shininess",
        (getter) staticCallback_getShininess,
        (setter) staticCallback_setShininess, 
        "Shininess",
        NULL
    },
    {"Transparency",
        (getter) staticCallback_getTransparency,
        (setter) staticCallback_setTransparency, 
        "Transparency",
        NULL
    },
    {NULL, NULL}		/* Sentinel */
};

PyObject * MaterialPy::staticCallback_set (PyObject *self, PyObject *args, PyObject *kwd)
{
    try { // catches all exeptions coming up from c++ and generate a python exeption
        return ((MaterialPy*)self)->set(args);
    }
    catch(Base::Exception &e) // catch the FreeCAD exeptions
    {
        std::string str;
        str += "FreeCAD exception thrown (";
        str += e.what();
        str += ")";
        e.ReportException();
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(boost::filesystem::filesystem_error &e) // catch boost filesystem exeption
    {
        std::string str;
        str += "File system exeption thrown (";
        //str += e.who();
        //str += ", ";
        str += e.what();
        str += ")\n";
        Base::Console().Error(str.c_str());
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(const char *e) // catch simple string exceptions
    {
        Base::Console().Error(e);
        PyErr_SetString(PyExc_Exception,e);
        return NULL;
    }
    // in debug not all exceptions will be catched to get the attentation of the developer!
#ifndef DONT_CATCH_CXX_EXCEPTIONS 
    catch(std::exception &e) // catch other c++ exeptions
    {
        std::string str;
        str += "FC++ exception thrown (";
        str += e.what();
        str += ")";
        Base::Console().Error(str.c_str());
        PyErr_SetString(PyExc_Exception,str.c_str());
        return NULL;
    }
    catch(...)  // catch the rest!
    {
        PyErr_SetString(PyExc_Exception,"Unknown C++ exception");
        return NULL;
    }
#endif
}

PyObject * MaterialPy::staticCallback_getAmbientColor (PyObject *self, void *closure)
{
    try {
        return Py::new_reference_to(((MaterialPy*)self)->getAmbientColor());
    } catch (const Py::Exception&) {
        // The exception text is already set
        return NULL;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while reading attribute 'AmbientColor' of object 'Material'");
        return NULL;
    }
}

int MaterialPy::staticCallback_setAmbientColor (PyObject *self, PyObject *value, void *closure)
{
    try {
        ((MaterialPy*)self)->setAmbientColor(Py::Tuple(value,false));
        return 0;
    } catch (const Py::Exception&) {
        // The exception text is already set
        return -1;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while writing attribute 'AmbientColor' of object 'Material'");
        return -1;
    }
}

PyObject * MaterialPy::staticCallback_getDiffuseColor (PyObject *self, void *closure)
{
    try {
        return Py::new_reference_to(((MaterialPy*)self)->getDiffuseColor());
    } catch (const Py::Exception&) {
        // The exception text is already set
        return NULL;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while reading attribute 'DiffuseColor' of object 'Material'");
        return NULL;
    }
}

int MaterialPy::staticCallback_setDiffuseColor (PyObject *self, PyObject *value, void *closure)
{
    try {
        ((MaterialPy*)self)->setDiffuseColor(Py::Tuple(value,false));
        return 0;
    } catch (const Py::Exception&) {
        // The exception text is already set
        return -1;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while writing attribute 'DiffuseColor' of object 'Material'");
        return -1;
    }
}

PyObject * MaterialPy::staticCallback_getSpecularColor (PyObject *self, void *closure)
{
    try {
        return Py::new_reference_to(((MaterialPy*)self)->getSpecularColor());
    } catch (const Py::Exception&) {
        // The exception text is already set
        return NULL;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while reading attribute 'SpecularColor' of object 'Material'");
        return NULL;
    }
}

int MaterialPy::staticCallback_setSpecularColor (PyObject *self, PyObject *value, void *closure)
{
    try {
        ((MaterialPy*)self)->setSpecularColor(Py::Tuple(value,false));
        return 0;
    } catch (const Py::Exception&) {
        // The exception text is already set
        return -1;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while writing attribute 'SpecularColor' of object 'Material'");
        return -1;
    }
}

PyObject * MaterialPy::staticCallback_getShininess (PyObject *self, void *closure)
{
    try {
        return Py::new_reference_to(((MaterialPy*)self)->getShininess());
    } catch (const Py::Exception&) {
        // The exception text is already set
        return NULL;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while reading attribute 'Shininess' of object 'Material'");
        return NULL;
    }
}

int MaterialPy::staticCallback_setShininess (PyObject *self, PyObject *value, void *closure)
{
    try {
        ((MaterialPy*)self)->setShininess(Py::Float(value,false));
        return 0;
    } catch (const Py::Exception&) {
        // The exception text is already set
        return -1;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while writing attribute 'Shininess' of object 'Material'");
        return -1;
    }
}

PyObject * MaterialPy::staticCallback_getTransparency (PyObject *self, void *closure)
{
    try {
        return Py::new_reference_to(((MaterialPy*)self)->getTransparency());
    } catch (const Py::Exception&) {
        // The exception text is already set
        return NULL;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while reading attribute 'Transparency' of object 'Material'");
        return NULL;
    }
}

int MaterialPy::staticCallback_setTransparency (PyObject *self, PyObject *value, void *closure)
{
    try {
        ((MaterialPy*)self)->setTransparency(Py::Float(value,false));
        return 0;
    } catch (const Py::Exception&) {
        // The exception text is already set
        return -1;
    } catch (...) {
        PyErr_SetString(PyExc_Exception, "Unknown exception while writing attribute 'Transparency' of object 'Material'");
        return -1;
    }
}

//--------------------------------------------------------------------------
// Parents structure
//--------------------------------------------------------------------------
PyParentObject App::MaterialPy::Parents[] = {&PyObjectBase::Type, NULL};

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
MaterialPy::MaterialPy(Material *pcMaterial, PyTypeObject *T)
    : PyObjectBase(0, T), _pcMaterial(pcMaterial)
{

}

PyObject *MaterialPy::PyMake(PyObject *ignored, PyObject *args)  // Python wrapper
{
    // never create such objects with the constructor
    return 0;
}

//--------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------
MaterialPy::~MaterialPy()                                // Everything handled in parent
{
}

//--------------------------------------------------------------------------
// MaterialPy representation
//--------------------------------------------------------------------------
PyObject *MaterialPy::_repr(void)
{
    std::stringstream a;
    a << "<Material object at " << _pcMaterial << ">";
    return Py_BuildValue("s", a.str().c_str());
}

PyObject*  MaterialPy::set(PyObject *args)
{
    char *pstr;
    if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
        return NULL;                             // NULL triggers exception 

    PY_TRY{
        _pcMaterial->set(pstr);
    }PY_CATCH;

    Py_Return;
}

Py::Tuple MaterialPy::getAmbientColor(void) const
{
    Py::Tuple tuple(4);
    tuple.setItem(0, Py::Float(_pcMaterial->ambientColor.r));
    tuple.setItem(1, Py::Float(_pcMaterial->ambientColor.g));
    tuple.setItem(2, Py::Float(_pcMaterial->ambientColor.b));
    tuple.setItem(3, Py::Float(_pcMaterial->ambientColor.a));
    return tuple;
}

void  MaterialPy::setAmbientColor(Py::Tuple arg)
{
    Color c;
    c.r = (float)Py::Float(arg.getItem(0));
    c.g = (float)Py::Float(arg.getItem(1));
    c.b = (float)Py::Float(arg.getItem(2));
    if (arg.size() == 4)
    c.a = (float)Py::Float(arg.getItem(3));
    _pcMaterial->ambientColor = c;
}

Py::Tuple MaterialPy::getDiffuseColor(void) const
{
    Py::Tuple tuple(4);
    tuple.setItem(0, Py::Float(_pcMaterial->diffuseColor.r));
    tuple.setItem(1, Py::Float(_pcMaterial->diffuseColor.g));
    tuple.setItem(2, Py::Float(_pcMaterial->diffuseColor.b));
    tuple.setItem(3, Py::Float(_pcMaterial->diffuseColor.a));
    return tuple;
}

void  MaterialPy::setDiffuseColor(Py::Tuple arg)
{
    Color c;
    c.r = (float)Py::Float(arg.getItem(0));
    c.g = (float)Py::Float(arg.getItem(1));
    c.b = (float)Py::Float(arg.getItem(2));
    if (arg.size() == 4)
    c.a = (float)Py::Float(arg.getItem(3));
    _pcMaterial->diffuseColor = c;
}

Py::Tuple MaterialPy::getSpecularColor(void) const
{
    Py::Tuple tuple(4);
    tuple.setItem(0, Py::Float(_pcMaterial->specularColor.r));
    tuple.setItem(1, Py::Float(_pcMaterial->specularColor.g));
    tuple.setItem(2, Py::Float(_pcMaterial->specularColor.b));
    tuple.setItem(3, Py::Float(_pcMaterial->specularColor.a));
    return tuple;
}

void  MaterialPy::setSpecularColor(Py::Tuple arg)
{
    Color c;
    c.r = (float)Py::Float(arg.getItem(0));
    c.g = (float)Py::Float(arg.getItem(1));
    c.b = (float)Py::Float(arg.getItem(2));
    if (arg.size() == 4)
    c.a = (float)Py::Float(arg.getItem(3));
    _pcMaterial->specularColor = c;
}

Py::Float MaterialPy::getShininess(void) const
{
    return Py::Float(_pcMaterial->shininess);
}

void MaterialPy::setShininess(Py::Float arg)
{
    _pcMaterial->shininess = (float)arg;
}

Py::Float MaterialPy::getTransparency(void) const
{
    return Py::Float(_pcMaterial->transparency);
}

void MaterialPy::setTransparency(Py::Float arg)
{
    _pcMaterial->transparency = (float)arg;
}
