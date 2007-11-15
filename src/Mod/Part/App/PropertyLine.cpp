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

void PropertyLine::setValue(const gp_Lin& line)
{
  aboutToSetValue();
  _line = line;
  hasSetValue();
}

const gp_Lin& PropertyLine::getValue(void) const 
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
  return sizeof(gp_Lin);
}

void PropertyLine::Save (Base::Writer &writer) const
{
    const gp_Dir& dir = _line.Direction();
    const gp_Pnt& loc = _line.Location();
    writer.Stream() << writer.ind() << "<PropertyLine bX=\"" 
                    <<  loc.X() << "\" bY=\"" <<  loc.Y() << "\" bZ=\"" <<  loc.Z() << writer.ind() 
                    << "\" eX=\"" <<  dir.X() << "\" eY=\"" <<  dir.Y() << "\" eZ=\"" <<  dir.Z() <<"\"/>" << std::endl;
}

void PropertyLine::Restore(Base::XMLReader &reader)
{
    // read my element
    reader.readElement("PropertyLine");

    // get the value of my Attribute
    gp_Pnt loc; gp_Dir dir;
    loc.SetX(reader.getAttributeAsFloat("bX"));
    loc.SetY(reader.getAttributeAsFloat("bY"));
    loc.SetZ(reader.getAttributeAsFloat("bZ"));
    dir.SetX(reader.getAttributeAsFloat("eX"));
    dir.SetY(reader.getAttributeAsFloat("eY"));
    dir.SetZ(reader.getAttributeAsFloat("eZ"));

    setValue(gp_Lin(loc, dir));
}

// --------------------------------------------------------

TYPESYSTEM_SOURCE(Part::PropertyLineSet , App::PropertyLists);

PropertyLineSet::PropertyLineSet()
{
}

PropertyLineSet::~PropertyLineSet()
{
}

void PropertyLineSet::setValue(const gp_Lin& lValue)
{
  aboutToSetValue();
  _lValueList.resize(1);
  _lValueList[0]=lValue;
  hasSetValue();
}

void PropertyLineSet::setValues(const std::vector<gp_Lin>& values)
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
        int nSize = PyList_Size(value);
        std::vector<gp_Lin> lines;
        lines.resize(nSize);

        for (int i=0; i<nSize;++i) {
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
  writer.Stream() << writer.ind() << "<LineSet file=\"" << writer.addFile(getName(), this) << "\"/>" << std::endl;
}

void PropertyLineSet::Restore(Base::XMLReader &reader)
{
  reader.readElement("LineSet");
  std::string file (reader.getAttribute("file") );
  reader.addFile(file.c_str(),this);
}

void PropertyLineSet::SaveDocFile (Base::Writer &writer) const
{
    unsigned long uCt = getSize();
    writer.Stream().write((const char*)&uCt, sizeof(unsigned long));
    if (uCt > 0)
    writer.Stream().write((const char*)&(_lValueList[0]), uCt*sizeof(gp_Lin));
}

void PropertyLineSet::RestoreDocFile(Base::Reader &reader)
{
    unsigned long uCt=ULONG_MAX;
    reader.read((char*)&uCt, sizeof(unsigned long));
    std::vector<gp_Lin> values(uCt);
    if (uCt > 0)
    reader.read((char*)&(values[0]), uCt*sizeof(gp_Lin));
    setValues(values);
}
