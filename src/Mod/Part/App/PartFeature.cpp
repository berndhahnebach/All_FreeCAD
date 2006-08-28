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
# include <TopTools_HSequenceOfShape.hxx>
# include <STEPControl_Writer.hxx>
# include <STEPControl_Reader.hxx>
#endif

#include <BRepTools_ShapeSet.hxx>
#include <locale>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>

#include "PartFeature.h"
#include "PartFeaturePy.h"
#include "TopologyPy.h"

using namespace Part;

//#define PROPERTY_IO_BREP

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
  p->_Shape = _Shape;
  return p;
}

void PropertyPartShape::Paste(const App::Property &from)
{
  aboutToSetValue();
  _Shape = dynamic_cast<const PropertyPartShape&>(from)._Shape;
  hasSetValue();

}

/// FIXME I have no Idea how to compute the size of a topoShape!
unsigned int PropertyPartShape::getMemSize (void) const
{
  return 0;
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
#ifdef PROPERTY_IO_BREP
    writer << writer.ind() << "<Part file=\"" << writer.addFile("PartShape.brp", this) << "\"/>" << std::endl;
#else
    writer << writer.ind() << "<Part file=\"" << writer.addFile("PartShape.stp", this) << "\"/>" << std::endl;
#endif
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
  //FIXME: Test with several data!
#ifdef PROPERTY_IO_BREP
  // needed to avoid STL exception 'missing locale facet', but don't know whether this has side effects
  writer.imbue(std::locale::empty());
  // Before writing to the project we clean all triangulation data to save memory
  BRepTools::Clean(_Shape);
  BRepTools::Write(_Shape, writer);
#else
  try {
    // create a temporary file and copy the content to the zip stream
    Base::FileInfo fi;
    std::string filename = fi.getTempFileName();

    STEPControl_Writer aWriter;
    aWriter.Transfer(_Shape, STEPControl_AsIs);
    if (aWriter.Write((const Standard_CString)filename.c_str()) != IFSelect_RetDone)
      return;

    std::ifstream file( filename.c_str(), std::ios::in | std::ios::binary );
    if (file){
      char line[200];
      while (!file.eof()) {
        file.getline(line,200);
        writer << line << std::endl;
      }
    }
  } catch( const Base::Exception& e) {
    throw e;
  }
#endif
}

void PropertyPartShape::RestoreDocFile(Base::Reader &reader)
{
  //FIXME: Test with several data!
#ifdef PROPERTY_IO_BREP
  // needed to avoid STL exception 'missing locale facet', but don't know whether this has side effects
  reader.imbue(std::locale::empty());
  BRep_Builder builder;
  BRepTools::Read(_Shape, reader, builder);
#else
  try {
    // create a temporary file and copy the content from the zip stream
    Base::FileInfo fi;
    std::string filename = fi.getTempFileName();

    std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
    if (file){
      char line[200];
      while (!reader.eof()) {
        reader.getline(line,200);
        file << line << std::endl;
      }
      file.close();
    }

    STEPControl_Reader aReader;
    Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
    if (aReader.ReadFile((const Standard_CString)filename.c_str()) != IFSelect_RetDone)
      return;
  
    // Root transfers
    Standard_Integer nbr = aReader.NbRootsForTransfer();
    for ( Standard_Integer n = 1; n<= nbr; n++)
    {
      // Collecting resulting entities
      aReader.TransferRoot(n);
      Standard_Integer nbs = aReader.NbShapes();
      if (nbs == 0) {
        aHSequenceOfShape.Nullify();
        return;
      } else {
        for (Standard_Integer i =1; i<=nbs; i++) 
        {
          _Shape=aReader.Shape(i);
          aHSequenceOfShape->Append(_Shape);
        }
      }
    }
  } catch( const Base::Exception& e) {
    throw e;
  }
#endif
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


