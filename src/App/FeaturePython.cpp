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


#include <Base/Console.h>
#include <Base/Interpreter.h>
#include "FeaturePython.h"
#include "FeaturePythonPy.h"

#define new DEBUG_CLIENTBLOCK
using namespace App;
using namespace std;


PROPERTY_SOURCE(App::FeaturePython, App::AbstractFeature)


FeaturePython::FeaturePython()
{
}

DocumentObjectExecReturn *FeaturePython::execute(void)
{
    // Run the callback function of the Python object. There is no need to handle any exceptions here as the calling
    // instance does this for us.
    Base::Interpreter().runMethodVoid(PythonObject.ptr(), "execute");

    return DocumentObject::StdReturn;
}

void FeaturePython::getPropertyMap(std::map<std::string,Property*> &Map) const
{
    // get the properties of the base class first and insert the dynamic properties afterwards
    AbstractFeature::getPropertyMap(Map);
    for ( std::map<std::string,Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it )
        Map[it->first] = it->second;
}

Property *FeaturePython::getPropertyByName(const char* name) const
{
    std::map<std::string,Property*>::const_iterator it = objectProperties.find( name );
    if ( it != objectProperties.end() )
        return it->second;
    return AbstractFeature::getPropertyByName( name );
}

const char* FeaturePython::getName(const Property* prop) const
{
    for ( std::map<std::string,Property*>::const_iterator it = objectProperties.begin(); it != objectProperties.end(); ++it ) {
        if (it->second == prop)
            return it->first.c_str();
    }
    return AbstractFeature::getName(prop);
}

void FeaturePython::addDynamicProperty(const char* type, const char* name)
{
    Property* pcObject = (Property*) Base::Type::createInstanceByName(type,true);
    if (pcObject) {
        if (!pcObject->getTypeId().isDerivedFrom(Property::getClassTypeId())) {
            delete pcObject;
            std::stringstream str;
            str << "'" << type << "' is not a property type";
            throw Base::Exception(str.str());
        }

        // get unique name
        string ObjectName;
        if (name)
            ObjectName = getUniquePropertyName(name);
        else
            ObjectName = getUniquePropertyName(type);

        pcObject->setContainer(this);
        objectProperties[ObjectName] = pcObject;
    }
}

string FeaturePython::getUniquePropertyName(const char *Name) const
{
    // strip illegal chars
    string CleanName;
    const char *It=Name;

    // check for first character whether it's a digit
    if ((*It != '\0') && (*It>=48 && *It<=57))
        CleanName = "_";

    while (*It != '\0') {
        if (   (*It>=48 && *It<=57)  // Numbers
                ||(*It>=65 && *It<=90)   // Upercase letters
                ||(*It>=97 && *It<=122)  // Upercase letters
           ) {
            CleanName += *It;
        }
        else {
            // All other letters gets replaced
            CleanName += '_';
        }
        It++;
    }

    std::map<std::string,Property*>::const_iterator pos;

    // name in use?
    pos = objectProperties.find(CleanName);

    if (pos == objectProperties.end())
        // if not, name is OK
        return CleanName;
    else {
        // find highest suffix
        int nSuff = 0;
        for (pos = objectProperties.begin();pos != objectProperties.end();++pos) {
            const string &rclObjName = pos->first;
            if (rclObjName.substr(0, CleanName.size()) == CleanName) { // Prefix gleich
                string clSuffix(rclObjName.substr(CleanName.size()));
                if (clSuffix.size() > 0) {
                    int nPos = clSuffix.find_first_not_of("0123456789");
                    if (nPos==-1)
                        nSuff = max<int>(nSuff, atol(clSuffix.c_str()));
                }
            }
        }
        std::stringstream str;
        str << CleanName << ++nSuff;
        return str.str();
    }

}

PyObject *FeaturePython::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject.set(new FeaturePythonPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}
