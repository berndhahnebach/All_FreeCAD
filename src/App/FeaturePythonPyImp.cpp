/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer@users.sourceforge.net>        *
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

#include "FeaturePython.h"

// inclusion of the generated files (generated out of FeaturePythonPy.xml)
#include "FeaturePythonPy.h"
#include "FeaturePythonPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *FeaturePythonPy::representation(void) const
{
    return "Python feature object>";
}

PyObject*  FeaturePythonPy::addProperty(PyObject *args)
{
    char *sType,*sName=0;
    if (!PyArg_ParseTuple(args, "ss", &sType,&sName))     // convert args: Python->C
        return NULL;                             // NULL triggers exception 

    PY_TRY {
        getFeaturePythonPtr()->addDynamicProperty(sType,sName);
    } PY_CATCH;

    Py_Return;
}

PyObject*  FeaturePythonPy::supportedProperties(PyObject *args)
{
    if (!PyArg_ParseTuple(args, ""))     // convert args: Python->C 
        return NULL;                    // NULL triggers exception
    
    std::vector<Base::Type> ary;
    Base::Type::getAllDerivedFrom(App::Property::getClassTypeId(), ary);
    Py::List res;
    for (std::vector<Base::Type>::iterator it = ary.begin(); it != ary.end(); ++it)
        res.append(Py::String(it->getName()));
    return Py::new_reference_to(res);
}

/** Invokes the registered callback function.
 * To register a callback function in Python do it as follows:
 * \code
 *  # Create a document and add a FeaturePython object
 *  d=FreeCAD.newDocument()
 *  f=d.addObject("App::FeaturePython")
 *
 *  def myCallback(arg):
 *      FreeCAD.PrintMessage("Hello, World!")
 *
 *  # Save the callback function
 *  f.Execute = myCallback
 *
 *  # Performing a recomputation of the document invokes the callback function
 *  d.recompute()
 * \endcode
 *
 * \note You must not pass any parameters to the callback function, its argument list must be empty.
 */
PyObject*  FeaturePythonPy::execute(PyObject * /*args*/)
{
    Py::Object executeCallback = getFeaturePythonPtr()->executeMethod;
    if ( executeCallback.isCallable() ) {
        try {
            Py::Callable method(executeCallback);
            Py::Tuple args(1);
            args.setItem(0, Py::Object(this));
            method.apply(args);
        }
        catch (const Py::Exception&) {
            return NULL;
        }

        Py_Return;
    } else {
        PyErr_SetString(PyExc_NotImplementedError , "FeaturePython.execute not implemented");
        return NULL;
    }
}

Py::Object FeaturePythonPy::getExecute(void) const
{
    return getFeaturePythonPtr()->executeMethod;
}

void FeaturePythonPy::setExecute(Py::Object value)
{
    if ( value == Py::None()) {
        getFeaturePythonPtr()->executeMethod = value;
    } else {
        // Here we can check whether 'value' is a method or function but we cannot check the argument list
        // This will do Python for us in the execute method (and throws an exception if not empty).
        if (!value.isCallable()) {
            throw Py::TypeError("Value must be callable");
        }

        getFeaturePythonPtr()->executeMethod = value;
    }
}

PyObject *FeaturePythonPy::getCustomAttributes(const char* attr) const
{
    PY_TRY{
        if (Base::streq(attr, "__dict__")){
            PyObject* dict = FeaturePy::getCustomAttributes(attr);
            if (dict){
                const std::map<std::string,Property*>& Map = getFeaturePythonPtr()->objectProperties;
                for ( std::map<std::string,App::Property*>::const_iterator it = Map.begin(); it != Map.end(); ++it )
                    PyDict_SetItem(dict, PyString_FromString(it->first.c_str()), PyString_FromString(""));
            }
            return dict;
        }
        // search in object PropertyList
        std::map<std::string,Property*>::const_iterator pos = getFeaturePythonPtr()->objectProperties.find(attr);

        if (pos != getFeaturePythonPtr()->objectProperties.end()) {
            Property *prop = getFeaturePythonPtr()->objectProperties[attr];
            return prop->getPyObject();
        }

    } PY_CATCH;

    return 0;
}

int FeaturePythonPy::setCustomAttributes(const char* attr, PyObject *value)
{
    // search in object PropertyList
    std::map<std::string,Property*>::const_iterator pos = getFeaturePythonPtr()->objectProperties.find(attr);

    if (pos == getFeaturePythonPtr()->objectProperties.end())
        return FeaturePy::setCustomAttributes(attr, value);
    else {
        Property *prop = getFeaturePythonPtr()->objectProperties[attr];

        try {
            prop->setPyObject(value);
        } catch (Base::Exception &exc) {
            PyErr_Format(PyExc_AttributeError, "Attribute (Name: %s) error: '%s' ", attr, exc.what());
            return -1;
        } catch (...) {
            PyErr_Format(PyExc_AttributeError, "Unknown error in attribute %s", attr);
            return -1;
        }

        return 0;
    }
}
