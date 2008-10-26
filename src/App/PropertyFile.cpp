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
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <Base/Exception.h>
#include <Base/Reader.h>
#include <Base/Writer.h>
#include <Base/Stream.h>
#include <Base/PyObjectBase.h>

#include "PropertyFile.h"
#define new DEBUG_CLIENTBLOCK
using namespace App;
using namespace Base;
using namespace std;




//**************************************************************************
// PropertyFile
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyFile , App::PropertyString);

PropertyFile::PropertyFile()
{

}

PropertyFile::~PropertyFile()
{

}



//**************************************************************************
// PropertyFileIncluded
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TYPESYSTEM_SOURCE(App::PropertyFileIncluded , App::Property);

PropertyFileIncluded::PropertyFileIncluded()
{

}

PropertyFileIncluded::~PropertyFileIncluded()
{

}


void PropertyFileIncluded::setValue(const char* sString)
{
   if (sString) {
        aboutToSetValue();
        _cValue = sString;
        hasSetValue();
    }
}

void PropertyFileIncluded::setValue(const std::string &sString)
{
    aboutToSetValue();
    _cValue = sString;
    hasSetValue();
}

const char* PropertyFileIncluded::getValue(void) const
{
     return _cValue.c_str();
}

PyObject *PropertyFileIncluded::getPyObject(void)
{
    PyObject *p = PyUnicode_DecodeUTF8(_cValue.c_str(),_cValue.size(),0);
    if (!p) throw Base::Exception("UTF8 conversion failure at PropertyString::getPyObject()");
    return p;
}

void PropertyFileIncluded::setPyObject(PyObject *value)
{
    std::string string;
    if (PyUnicode_Check(value)) {
        PyObject* unicode = PyUnicode_AsUTF8String(value);
        string = PyString_AsString(unicode);
        Py_DECREF(unicode);
    }
    else if (PyString_Check(value)) {
        string = PyString_AsString(value);
    }
    else if (PyFile_Check(value)) {
		PyObject* FileName = PyFile_Name(value);
        string = PyString_AsString(FileName);
    }
    else {
        std::string error = std::string("type must be str or file");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }

    // assign the string
    setValue(string);
}

void PropertyFileIncluded::Save (Writer &writer) const
{
	std::string val = encodeAttribute(_cValue);
    writer.Stream() << writer.ind() << "<String value=\"" <<  val <<"\"/>" << std::endl;
}

void PropertyFileIncluded::Restore(Base::XMLReader &reader)
{
    // read my Element
    reader.readElement("String");
    // get the value of my Attribute
    setValue(reader.getAttribute("value"));
}

void PropertyFileIncluded::SaveDocFile (Base::Writer &writer) const
{
    //Base::OutputStream str(writer.Stream());
    //uint32_t uCt = (uint32_t)getSize();
    //str << uCt;
    //for (std::vector<float>::const_iterator it = _lValueList.begin(); it != _lValueList.end(); ++it) {
    //    str << *it;
    //}
}

void PropertyFileIncluded::RestoreDocFile(Base::Reader &reader)
{
    //Base::InputStream str(reader);
    //uint32_t uCt=0;
    //str >> uCt;
    //std::vector<float> values(uCt);
    //for (std::vector<float>::iterator it = values.begin(); it != values.end(); ++it) {
    //    str >> *it;
    //}
    //setValues(values);
}

Property *PropertyFileIncluded::Copy(void) const
{
    PropertyFileIncluded *p= new PropertyFileIncluded();
    p->_cValue = _cValue;
    return p;
}

void PropertyFileIncluded::Paste(const Property &from)
{
    aboutToSetValue();
    _cValue = dynamic_cast<const PropertyFileIncluded&>(from)._cValue;
    hasSetValue();
}