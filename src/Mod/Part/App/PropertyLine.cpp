/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/Stream.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>

#include "PropertyLine.h"
#include "LinePy.h"

using namespace Part;

TYPESYSTEM_SOURCE(Part::PropertyLine , App::Property);

PropertyLine::PropertyLine()
{
}

PropertyLine::~PropertyLine()
{
}

void PropertyLine::setValue(const Line3f& line)
{
    aboutToSetValue();
    _line = line;
    hasSetValue();
}

const Line3f& PropertyLine::getValue(void) const 
{
    return _line;
}

PyObject *PropertyLine::getPyObject(void)
{
    return new LinePy(this->_line);
}

void PropertyLine::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(LinePy::Type))) {
        LinePy  *pcObject = (LinePy*)value;
        setValue( pcObject->value() );
    }
    else {
        std::string error = std::string("type must be 'Line', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

App::Property *PropertyLine::Copy(void) const
{
    PropertyLine* p = new PropertyLine();
    p->_line = this->_line;
    return p;
}

void PropertyLine::Paste(const App::Property &from)
{
    aboutToSetValue();
    _line = dynamic_cast<const PropertyLine&>(from)._line;
    hasSetValue();
}

unsigned int PropertyLine::getMemSize (void) const
{
    return sizeof(Line3f);
}

void PropertyLine::Save (Base::Writer &writer) const
{
    const Base::Vector3f& pt1 = _line.first;
    const Base::Vector3f& pt2 = _line.second;
    writer.Stream() << writer.ind() << "<PropertyLine bX=\"" 
                    <<  pt1.x << "\" bY=\"" << pt1.y << "\" bZ=\"" << pt1.z << writer.ind() 
                    << "\" eX=\"" << pt2.x << "\" eY=\"" << pt2.y << "\" eZ=\"" << pt2.z <<"\"/>" << std::endl;
}

void PropertyLine::Restore(Base::XMLReader &reader)
{
    // read my element
    reader.readElement("PropertyLine");

    // get the value of my Attribute
    Line3f line;
    line.first.x = float(reader.getAttributeAsFloat("bX"));
    line.first.y = float(reader.getAttributeAsFloat("bY"));
    line.first.z = float(reader.getAttributeAsFloat("bZ"));
    line.second.x = float(reader.getAttributeAsFloat("eX"));
    line.second.y = float(reader.getAttributeAsFloat("eY"));
    line.second.z = float(reader.getAttributeAsFloat("eZ"));

    setValue(line);
}

// --------------------------------------------------------

TYPESYSTEM_SOURCE(Part::PropertyLineSet , App::PropertyLists);

PropertyLineSet::PropertyLineSet()
{
}

PropertyLineSet::~PropertyLineSet()
{
}

void PropertyLineSet::setValue(const Line3f& lValue)
{
    aboutToSetValue();
    _lValueList.resize(1);
    _lValueList[0]=lValue;
    hasSetValue();
}

void PropertyLineSet::setValues(const std::vector<Line3f>& values)
{
    aboutToSetValue();
    _lValueList = values;
    hasSetValue();
}

PyObject *PropertyLineSet::getPyObject(void)
{
    PyObject* list = PyList_New(	getSize() );
    for(int i = 0;i<getSize(); i++)
        PyList_SetItem( list, i, new LinePy(_lValueList[i]));
    return list;
}

void PropertyLineSet::setPyObject(PyObject *value)
{
    if (PyList_Check(value)) {
        Py_ssize_t nSize = PyList_Size(value);
        std::vector<Line3f> lines;
        lines.resize(nSize);

        for (Py_ssize_t i=0; i<nSize;++i) {
            PyObject* item = PyList_GetItem(value, i);
            PropertyLine val;
            val.setPyObject(item);
            lines[i] = val.getValue();
        }

        setValues(lines);
    }
    else if (PyObject_TypeCheck(value, &(LinePy::Type))) {
        LinePy  *pcObject = (LinePy*)value;
        setValue(pcObject->value());
    }
    else {
        std::string error = std::string("type must be 'Line' or list of 'Line', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

App::Property *PropertyLineSet::Copy(void) const
{
    PropertyLineSet* p = new PropertyLineSet();
    p->_lValueList = this->_lValueList;
    return p;
}

void PropertyLineSet::Paste(const App::Property &from)
{
    aboutToSetValue();
    _lValueList = dynamic_cast<const PropertyLineSet&>(from)._lValueList;
    hasSetValue();
}

unsigned int PropertyLineSet::getMemSize (void) const
{
    return sizeof(gp_Lin)*_lValueList.size();
}

void PropertyLineSet::Save (Base::Writer &writer) const
{
    if (!writer.isForceXML()) {
        writer.Stream() << writer.ind() << "<LineSet file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
    }
}

void PropertyLineSet::Restore(Base::XMLReader &reader)
{
    reader.readElement("LineSet");
    std::string file (reader.getAttribute("file"));
    if (!file.empty()) {
        // initate a file read
        reader.addFile(file.c_str(),this);
    }
}

void PropertyLineSet::SaveDocFile (Base::Writer &writer) const
{
    Base::OutputStream str(writer.Stream());
    uint32_t uCt = (uint32_t)getSize();
    str << uCt;
    for (std::vector<Line3f>::const_iterator it = _lValueList.begin(); it != _lValueList.end(); ++it) {
        str << it->first.x << it->first.y << it->first.z;
        str << it->second.x << it->second.y << it->second.z;
    }
}

void PropertyLineSet::RestoreDocFile(Base::Reader &reader)
{
    Base::InputStream str(reader);
    uint32_t uCt = 0;
    str >> uCt;
    std::vector<Line3f> values(uCt);
    for (uint32_t i=0; i < uCt; i++) {
        float x1,y1,z1,x2,y2,z2;
        str >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
        values[i].first.Set(x1,y1,z1);
        values[i].second.Set(x2,y2,z2);
    }
    setValues(values);
}
