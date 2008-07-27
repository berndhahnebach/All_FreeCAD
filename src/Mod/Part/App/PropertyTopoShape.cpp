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
# include <BRepAdaptor_Curve.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <BRepBndLib.hxx>
# include <Bnd_Box.hxx>
# include <BRepTools.hxx>
# include <BRepTools_ShapeSet.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <TopTools_HSequenceOfShape.hxx>
# include <TopTools_MapOfShape.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopExp.hxx>
# include <Standard_Failure.hxx>
#endif


#include <strstream>
#include <Base/Console.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>

#include "PropertyTopoShape.h"
#include "TopoShapePy.h"

using namespace Part;

TYPESYSTEM_SOURCE(Part::PropertyPartShape , App::PropertyComplexGeoData);

PropertyPartShape::PropertyPartShape()
{
}

PropertyPartShape::~PropertyPartShape()
{
}

void PropertyPartShape::setValue(const TopoShape& sh)
{
    aboutToSetValue();
    _Shape = sh;
    hasSetValue();
}

void PropertyPartShape::setValue(TopoDS_Shape m)
{
    aboutToSetValue();
    _Shape._Shape = m;
    hasSetValue();
}

TopoDS_Shape PropertyPartShape::getValue(void)const 
{
    return _Shape._Shape;
}

Base::BoundBox3f PropertyPartShape::getBoundingBox() const
{
    Base::BoundBox3f box;
    if (_Shape._Shape.IsNull())
        return box;
    try {
        // If the shape is empty an exception may be thrown
        Bnd_Box bounds;
        BRepBndLib::Add(_Shape._Shape, bounds);
        bounds.SetGap(0.0);
        Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
        bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);

        box.MinX = (float)xMin;
        box.MaxX = (float)xMax;
        box.MinY = (float)yMin;
        box.MaxY = (float)yMax;
        box.MinZ = (float)zMin;
        box.MaxZ = (float)zMax;
    }
    catch (Standard_Failure) {
    }

    return box;
}

void PropertyPartShape::getFaces(std::vector<Base::Vector3d> &aPoints,
                                 std::vector<Data::ComplexGeoData::FacetTopo> &aTopo,
                                 float accuracy, uint16_t flags) const
{
    _Shape.getFaces(aPoints, aTopo, accuracy, flags);
}

void PropertyPartShape::setLocation(const TopLoc_Location& loc)
{
    // Note: The internal shapes do not change but the internal location only,
    // we actually do not need to save the shapes.
    //aboutToSetValue();
    _Shape._Shape.Location(loc);
    //hasSetValue();
}

PyObject *PropertyPartShape::getPyObject(void)
{
    return new TopoShapePy(new TopoShape(_Shape._Shape));
}

void PropertyPartShape::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(TopoShapePy::Type))) {
        TopoShapePy *pcObject = static_cast<TopoShapePy*>(value);
        setValue(*pcObject->getTopoShapePtr());
    }
    else {
        std::string error = std::string("type must be 'Shape', not ");
        error += value->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

App::Property *PropertyPartShape::Copy(void) const
{
    PropertyPartShape *prop = new PropertyPartShape();
    prop->_Shape = this->_Shape;
    if (!_Shape._Shape.IsNull()) {
        BRepBuilderAPI_Copy copy(_Shape._Shape);
        prop->_Shape._Shape = copy.Shape();
    }

    return prop;
}

void PropertyPartShape::Paste(const App::Property &from)
{
    aboutToSetValue();
    _Shape = dynamic_cast<const PropertyPartShape&>(from)._Shape;
    hasSetValue();
}

unsigned int PropertyPartShape::getMemSize (void) const
{
    return _Shape.getMemSize();
}

void PropertyPartShape::Save (Base::Writer &writer) const
{
    if(!writer.isForceXML()) {
        //See SaveDocFile(), RestoreDocFile()
        writer.Stream() << writer.ind() << "<Part file=\"" 
                        << writer.addFile("PartShape.brp", this)
                        << "\"/>" << std::endl;
    }
}

void PropertyPartShape::Restore(Base::XMLReader &reader)
{
    reader.readElement("Part");
    std::string file (reader.getAttribute("file") );

    if (!file.empty()) {
        // initate a file read
        reader.addFile(file.c_str(),this);
    }
}

void PropertyPartShape::SaveDocFile (Base::Writer &writer) const
{
    // Before writing to the project we clean all triangulation data to save memory
    BRepTools::Clean(_Shape._Shape);

    // create a temporary file and copy the content to the zip stream
    Base::FileInfo fi(Base::FileInfo::getTempFileName().c_str());

    if (!BRepTools::Write(_Shape._Shape,(const Standard_CString)fi.filePath().c_str())) {
        // Note: Do NOT throw an exception here because if the tmp. file could
        // not be created we should not abort.
        // We only print an error message but continue writing the next files to the
        // stream...
        Base::Console().Error("Cannot save BRep file '%s'", fi.filePath().c_str());
    }

    Base::ifstream file(fi, std::ios::in | std::ios::binary);
    if (file){
        unsigned long ulSize = 0; 
        std::streambuf* buf = file.rdbuf();
        if (buf) {
            unsigned long ulCurr;
            ulCurr = buf->pubseekoff(0, std::ios::cur, std::ios::in);
            ulSize = buf->pubseekoff(0, std::ios::end, std::ios::in);
            buf->pubseekoff(ulCurr, std::ios::beg, std::ios::in);
        }

        // read in the ASCII file and write back to the stream
        std::strstreambuf sbuf(ulSize);
        file >> &sbuf;
        writer.Stream() << &sbuf;
    }

    file.close();
    // remove temp file
    fi.deleteFile();
}

void PropertyPartShape::RestoreDocFile(Base::Reader &reader)
{
    BRep_Builder builder;

    // create a temporary file and copy the content from the zip stream
    Base::FileInfo fi(Base::FileInfo::getTempFileName().c_str());

    // read in the ASCII file and write back to the file stream
    Base::ofstream file(fi, std::ios::out | std::ios::binary);
    if (reader)
        reader >> file.rdbuf();
    file.close();

    // read the shape from the temp file
    TopoDS_Shape shape;
    if (!BRepTools::Read(shape, (const Standard_CString)fi.filePath().c_str(), builder)) {
        // Note: Do NOT throw an exception here because if the tmp. created file could
        // not be read it's NOT an indication for an invalid input stream 'reader'.
        // We only print an error message but continue reading the next files from the
        // stream...
        Base::Console().Error("Cannot read BRep file '%s'", fi.filePath().c_str());
    }

    // delete the temp file
    fi.deleteFile();

    setValue(shape);
}
