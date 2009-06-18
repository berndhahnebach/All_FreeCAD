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




#ifndef APP_MATERIALPY_H
#define APP_MATERIALPY_H

#include <Base/PyObjectBase.h>

namespace App
{

class Material;

//===========================================================================
// MaterialPy - Python wrapper
//===========================================================================

/** The python export class for Material
 */
class AppExport MaterialPy : public Base::PyObjectBase
{
public:
    static PyTypeObject   Type;
    static PyMethodDef    Methods[];
    static PyGetSetDef    GetterSetter[];
    static PyParentObject Parents[];
    virtual PyTypeObject *GetType(void) {return &Type;};
    virtual PyParentObject *GetParents(void) {return Parents;}

public:
	MaterialPy(Material *pcMaterial, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);
	~MaterialPy();

    virtual PyObject *_repr(void);        // the representation

    /** @name callbacks and implementers for the python object methods */
    //@{
    /// callback for the set() method
    static PyObject * staticCallback_set (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the set() method
    PyObject*  set(PyObject *args);
    //@}


    /** @name callbacks and implementers for the python object methods */
    //@{
    ///getter callback for the AmbientColor attribute
    static PyObject * staticCallback_getAmbientColor (PyObject *self, void *closure);
    /// getter for the AmbientColor attribute
    Py::Tuple getAmbientColor(void) const;
    /// setter callback for the AmbientColor attribute
    static int staticCallback_setAmbientColor (PyObject *self, PyObject *value, void *closure);
    /// setter for the AmbientColor attribute
    void setAmbientColor(Py::Tuple arg);
    ///getter callback for the DiffuseColor attribute
    static PyObject * staticCallback_getDiffuseColor (PyObject *self, void *closure);
    /// getter for the DiffuseColor attribute
    Py::Tuple getDiffuseColor(void) const;
    /// setter callback for the DiffuseColor attribute
    static int staticCallback_setDiffuseColor (PyObject *self, PyObject *value, void *closure);
    /// setter for the DiffuseColor attribute
    void setDiffuseColor(Py::Tuple arg);
    ///getter callback for the EmissiveColor attribute
    static PyObject * staticCallback_getEmissiveColor (PyObject *self, void *closure);
    /// getter for the EmissiveColor attribute
    Py::Tuple getEmissiveColor(void) const;
    /// setter callback for the EmissiveColor attribute
    static int staticCallback_setEmissiveColor (PyObject *self, PyObject *value, void *closure);
    /// setter for the EmissiveColor attribute
    void setEmissiveColor(Py::Tuple arg);
    ///getter callback for the SpecularColor attribute
    static PyObject * staticCallback_getSpecularColor (PyObject *self, void *closure);
    /// getter for the SpecularColor attribute
    Py::Tuple getSpecularColor(void) const;
    /// setter callback for the SpecularColor attribute
    static int staticCallback_setSpecularColor (PyObject *self, PyObject *value, void *closure);
    /// setter for the SpecularColor attribute
    void setSpecularColor(Py::Tuple arg);
    ///getter callback for the Shininess attribute
    static PyObject * staticCallback_getShininess (PyObject *self, void *closure);
    /// getter for the Shininess attribute
    Py::Float getShininess(void) const;
    /// setter callback for the Shininess attribute
    static int staticCallback_setShininess (PyObject *self, PyObject *value, void *closure);
    /// setter for the Shininess attribute
    void setShininess(Py::Float arg);
    ///getter callback for the Transparency attribute
    static PyObject * staticCallback_getTransparency (PyObject *self, void *closure);
    /// getter for the Transparency attribute
    Py::Float getTransparency(void) const;
    /// setter callback for the Transparency attribute
    static int staticCallback_setTransparency (PyObject *self, PyObject *value, void *closure);
    /// setter for the Transparency attribute
    void setTransparency(Py::Float arg);
    //@}


	//---------------------------------------------------------------------
	// helpers for python exports goes here +++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------
  static Color getColorFromPy(PyObject *value);



  Material *_pcMaterial;

};

} //namespace App


#endif // APP_MATERIALPY_H
