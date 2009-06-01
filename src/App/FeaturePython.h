/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2006     *
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



#ifndef APP_FEATUREPYTHON_H
#define APP_FEATUREPYTHON_H


#include "DocumentObject.h"
#include "PropertyPythonObject.h"

namespace App
{

class Property;
class FeaturePythonPy;

class FeaturePython : public DocumentObject
{
    PROPERTY_HEADER(App::FeaturePython);

public:
    FeaturePython();  
    virtual ~FeaturePython();  

    /** @name methods overide Feature */
    //@{
    short mustExecute() const;
    /// recalculate the Feature
    virtual DocumentObjectExecReturn *execute(void);
    /// returns the type name of the ViewProvider
    virtual const char* getViewProviderName(void) const {
        return "Gui::ViewProviderPythonFeature";
    }
    /// get all properties of the class (including parent)
    virtual void getPropertyMap(std::map<std::string,Property*> &Map) const;
    /// find a property by its name
    virtual Property *getPropertyByName(const char* name) const;
    /// get the name of a property
    virtual const char* getName(const Property* prop) const;
    //@}

    Property* addDynamicProperty(const char* type, const char* name=0);
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);

    PyObject *getPyObject(void);

    friend class FeaturePythonPy;

protected:
    virtual void onChanged(const Property* prop);

private:
    std::string getUniquePropertyName(const char *Name) const;

private:
    std::map<std::string,Property*> objectProperties;
    PropertyPythonObject Proxy;
};

} //namespace App

#endif // APP_FEATUREPYTHON_H
