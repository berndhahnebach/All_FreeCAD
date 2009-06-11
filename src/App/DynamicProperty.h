/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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


#ifndef APP_DYNAMICPROPERTY_H
#define APP_DYNAMICPROPERTY_H

#include <App/PropertyContainer.h>

namespace Base {
class Writer;
class XMLWriter;
}

namespace App
{

class AppExport DynamicProperty : public PropertyContainer
{
    PROPERTY_HEADER(App::DynamicProperty);

public:
    struct PropData {
        Property* property;
        std::string group;
        std::string doc;
        short attr;
        bool hidden;
        bool readonly;
    };

    DynamicProperty();
    virtual ~DynamicProperty();

    /// get all properties of the class (including parent)
    void getPropertyMap(std::map<std::string,Property*> &Map) const;
    /// find a property by its name
    Property *getPropertyByName(const char* name) const;
    /// get the name of a property
    const char* getName(const Property* prop) const;
    //@}

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

    Property* addDynamicProperty(const char* type, const char* name=0, const char* group=0,
                                 const char* doc=0, short attr=0);
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);
    unsigned int getMemSize (void) const;

private:
    std::string getUniquePropertyName(const char *Name) const;

private:
    std::map<std::string,PropData> objectProperties;
};

} // namespace App

#endif // APP_DYNAMICPROPERTY_H
