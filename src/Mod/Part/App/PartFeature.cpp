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
# include <BRepTools.hxx>
# include <BRepTools_ShapeSet.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <TopTools_HSequenceOfShape.hxx>
# include <STEPControl_Writer.hxx>
# include <STEPControl_Reader.hxx>
# include <TopTools_MapOfShape.hxx>
# include <TopoDS_Iterator.hxx>
#endif


#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include "PartFeature.h"
#include "PartFeaturePy.h"
#include "TopologyPy.h"

using namespace Part;

TYPESYSTEM_SOURCE(Part::PropertyPartShape , App::Property);

PropertyPartShape::PropertyPartShape()
{

}

PropertyPartShape::~PropertyPartShape()
{
}

void PropertyPartShape::setValue(TopoDS_Shape m)
{
  aboutToSetValue();
  _Shape = m;
  hasSetValue();
}

TopoDS_Shape PropertyPartShape::getValue(void)const 
{
	return _Shape;
}

PyObject *PropertyPartShape::getPyObject(void)
{
  return new TopoShapePy(_Shape);
}

void PropertyPartShape::setPyObject(PyObject *value)
{
  if( PyObject_TypeCheck(value, &(TopoShapePy::Type)) ) {
   	TopoShapePy  *pcObject = (TopoShapePy*)value;
    setValue(pcObject->getTopoShape());
  }
}

App::Property *PropertyPartShape::Copy(void) const
{
  PropertyPartShape *p= new PropertyPartShape();
  BRepBuilderAPI_Copy copy(_Shape);
  p->_Shape = copy.Shape();
  return p;
}

void PropertyPartShape::Paste(const App::Property &from)
{
  aboutToSetValue();
  _Shape = dynamic_cast<const PropertyPartShape&>(from)._Shape;
  hasSetValue();

}

unsigned int PropertyPartShape::RefCountShapes(const TopoDS_Shape& aShape) const
{
  unsigned int size = 1; // this shape
  TopoDS_Iterator it;
  // go through all direct children
  for (it.Initialize(aShape, false, false);it.More(); it.Next())
  {
    size += RefCountShapes(it.Value());
  }

  return size;
}

unsigned int PropertyPartShape::getMemSize (void) const
{
  if ( !_Shape.IsNull() )
  {
    // Count total amount of references of TopoDS_Shape objects
    unsigned int memsize = sizeof(TopoDS_Shape) * RefCountShapes(_Shape);

    // Now get a map of TopoDS_Shape objects without duplicates
    TopTools_IndexedMapOfShape M;
    TopExp::MapShapes(_Shape, M);
    for (int i=0; i<M.Extent(); i++)
    {
      // add the size of the underlying geomtric data
      Handle(TopoDS_TShape) sh = M(i+1).TShape();
      memsize += sh->DynamicType()->Size();
    }

    // estimated memory usage
    return memsize;
  }

  // in case the shape is invalid
  return sizeof(TopoDS_Shape);
}

void PropertyPartShape::Save (Base::Writer &writer) const
{
//  if( writer.isForceXML() )
//  {
//    writer << writer.ind() << "<Part>" << std::endl;
//    MeshCore::MeshDocXML saver(*_pcMesh);
//    saver.Save(writer);
//  }else{
    //See SaveDocFile(), RestoreDocFile()
    writer << writer.ind() << "<Part file=\"" << writer.addFile("PartShape.brp", this) << "\"/>" << std::endl;
//  }
}

void PropertyPartShape::Restore(Base::XMLReader &reader)
{
  reader.readElement("Part");
  std::string file (reader.getAttribute("file") );
//
//  if(file == "")
//  {
//    // read XML
//    MeshCore::MeshDocXML restorer(*_pcMesh);
//    restorer.Restore(reader);
//  }else{
    // initate a file read
    reader.addFile(file.c_str(),this);
//  }
}

void PropertyPartShape::SaveDocFile (Base::Writer &writer) const
{
  // Before writing to the project we clean all triangulation data to save memory
  BRepTools::Clean(_Shape);

  // create a temporary file and copy the content to the zip stream
  Base::FileInfo fi(Base::FileInfo::getTempFileName().c_str());

  if (! BRepTools::Write(_Shape,(const Standard_CString)fi.filePath().c_str()))
    throw Base::Exception("PropertyPartShape::SaveDocFile: Can't save file...");

  std::ifstream file( fi.filePath().c_str(), std::ios::in | std::ios::binary );
  if (file){
    char line[2000];
    while (!file.eof()) {
      file.getline(line,1999);
      writer << line << std::endl;
    }
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

  std::ofstream file(fi.filePath().c_str(), std::ios::out | std::ios::binary);
  if (file){
    char line[2000];
    while (!reader.eof()) {
      reader.getline(line,1999);
      file << line << std::endl;
    }
    file.close();
  }

  // read the shape from the temp file
  if (! BRepTools::Read(_Shape, (const Standard_CString)fi.filePath().c_str(), builder))
    throw Base::Exception("PropertyPartShape::RestoreDocFile(): Cant read file...");

  // delete the temp file
  fi.deleteFile();
}

//===========================================================================
// Feature
//===========================================================================

PROPERTY_SOURCE(Part::Feature, App::AbstractFeature)


Feature::Feature(void) : _featurePy(0)
{
  ADD_PROPERTY(Shape, (TopoDS_Shape()));
}

Feature::~Feature()
{
  if ( _featurePy )
  {
    _featurePy->setInvalid();
    _featurePy->DecRef();
  }
}

int Feature::execute(void)
{
  return 0;
}


void Feature::setShape(const TopoDS_Shape &Shape)
{
  this->Shape.setValue(Shape);
}

TopoDS_Shape Feature::getShape(void)
{
  return Shape.getValue();
}


Base::PyObjectBase *Feature::GetPyObject(void)
{
  if ( !_featurePy ) {
    _featurePy = new PartFeaturePy(this);
  }

  _featurePy->IncRef();
  return _featurePy;
}


