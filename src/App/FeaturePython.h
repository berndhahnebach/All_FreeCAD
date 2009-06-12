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


#include <App/DocumentObject.h>
#include <App/DynamicProperty.h>
#include <App/PropertyPythonObject.h>

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

    /** @name Access properties */
    //@{
    /// get all properties of the class (including parent)
    virtual void getPropertyMap(std::map<std::string,Property*> &Map) const;
    /// find a property by its name
    virtual Property *getPropertyByName(const char* name) const;
    /// get the name of a property
    virtual const char* getName(const Property* prop) const;
    //@}

    /** @name Property attributes */
    //@{
    /// get the Type of a Property
    short getPropertyType(const Property* prop) const;
    /// get the Type of a named Property
    short getPropertyType(const char *name) const;
    /// get the Group of a Property
    const char* getPropertyGroup(const Property* prop) const;
    /// get the Group of a named Property
    const char* getPropertyGroup(const char *name) const;
    /// get the Group of a Property
    const char* getPropertyDocumentation(const Property* prop) const;
    /// get the Group of a named Property
    const char* getPropertyDocumentation(const char *name) const;
    /// check if the property is read-only
    bool isReadOnly(const Property* prop) const;
    /// check if the nameed property is read-only
    bool isReadOnly(const char *name) const;
    /// check if the property is hidden
    bool isHidden(const Property* prop) const;
    /// check if the named property is hidden
    bool isHidden(const char *name) const;
    //@}

    /** @name Property serialization */
    //@{
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);
    //@}

    PyObject *getPyObject(void);
    void setPyObject(PyObject *);

    friend class FeaturePythonPy;

protected:
    virtual void onChanged(const Property* prop);

private:
    DynamicProperty *props;
    PropertyPythonObject Proxy;
};

} //namespace App

#endif // APP_FEATUREPYTHON_H
