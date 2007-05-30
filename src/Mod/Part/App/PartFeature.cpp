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
# include <BRepTools.hxx>
# include <BRepTools_ShapeSet.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <TopTools_HSequenceOfShape.hxx>
# include <STEPControl_Writer.hxx>
# include <STEPControl_Reader.hxx>
# include <TopTools_MapOfShape.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopExp.hxx>
# include <Geom_BezierCurve.hxx>
# include <Geom_BezierSurface.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Geom_SurfaceOfLinearExtrusion.hxx>
# include <Geom_SurfaceOfRevolution.hxx>
# include <Geom_Circle.hxx>
# include <Geom_ConicalSurface.hxx>
# include <Geom_CylindricalSurface.hxx>
# include <Geom_Ellipse.hxx>
# include <Geom_Hyperbola.hxx>
# include <Geom_Line.hxx>
# include <Geom_Parabola.hxx>
# include <Geom_Plane.hxx>
# include <Geom_CartesianPoint.hxx>
# include <Geom_SphericalSurface.hxx>
# include <Geom_ToroidalSurface.hxx>
#endif


#include <strstream>
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
  
  if (!_Shape.IsNull())
  {
    BRepBuilderAPI_Copy copy(_Shape);
    p->_Shape = copy.Shape();
  }
  
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
    unsigned int memsize = (sizeof(TopoDS_Shape)+sizeof(TopoDS_TShape)) * RefCountShapes(_Shape);

    // Now get a map of TopoDS_Shape objects without duplicates
    TopTools_IndexedMapOfShape M;
    TopExp::MapShapes(_Shape, M);
    for (int i=0; i<M.Extent(); i++)
    {
      const TopoDS_Shape& shape = M(i+1);
      // add the size of the underlying geomtric data
      Handle(TopoDS_TShape) tshape = shape.TShape();
      memsize += tshape->DynamicType()->Size();

      switch (shape.ShapeType())
      {
      case TopAbs_FACE:
        {
          // first, last, tolerance
          memsize += 5*sizeof(Standard_Real);
          TopoDS_Face face = TopoDS::Face(shape);
          BRepAdaptor_Surface surface(face);
          switch (surface.GetType())
          {
          case GeomAbs_Plane:
            memsize += sizeof(Geom_Plane);
            break;
          case GeomAbs_Cylinder:
            memsize += sizeof(Geom_CylindricalSurface);
            break;
          case GeomAbs_Cone:
            memsize += sizeof(Geom_ConicalSurface);
            break;
          case GeomAbs_Sphere:
            memsize += sizeof(Geom_SphericalSurface);
            break;
          case GeomAbs_Torus:
            memsize += sizeof(Geom_ToroidalSurface);
            break;
          case GeomAbs_BezierSurface:
            memsize += sizeof(Geom_BezierSurface);
            memsize += (surface.NbUKnots()+surface.NbVKnots()) * sizeof(Standard_Real);
            memsize += (surface.NbUPoles()*surface.NbVPoles()) * sizeof(Geom_CartesianPoint);
            break;
          case GeomAbs_BSplineSurface:
            memsize += sizeof(Geom_BSplineSurface);
            memsize += (surface.NbUKnots()+surface.NbVKnots()) * sizeof(Standard_Real);
            memsize += (surface.NbUPoles()*surface.NbVPoles()) * sizeof(Geom_CartesianPoint);
            break;
          case GeomAbs_SurfaceOfRevolution:
            memsize += sizeof(Geom_SurfaceOfRevolution);
            break;
          case GeomAbs_SurfaceOfExtrusion:
            memsize += sizeof(Geom_SurfaceOfLinearExtrusion);
            break;
          case GeomAbs_OtherSurface:
            // What kind of surface should this be?
            memsize += sizeof(Geom_Surface);
            break;
          default:
            break;
          }
        } break;
      case TopAbs_EDGE:
        {
          // first, last, tolerance
          memsize += 3*sizeof(Standard_Real);
          TopoDS_Edge edge = TopoDS::Edge(shape);
          BRepAdaptor_Curve curve(edge);
          switch (curve.GetType())
          {
          case GeomAbs_Line:
            memsize += sizeof(Geom_Line);
            break;
          case GeomAbs_Circle:
            memsize += sizeof(Geom_Circle);
            break;
          case GeomAbs_Ellipse:
            memsize += sizeof(Geom_Ellipse);
            break;
          case GeomAbs_Hyperbola:
            memsize += sizeof(Geom_Hyperbola);
            break;
          case GeomAbs_Parabola:
            memsize += sizeof(Geom_Parabola);
            break;
          case GeomAbs_BezierCurve:
            memsize += sizeof(Geom_BezierCurve);
            memsize += curve.NbKnots() * sizeof(Standard_Real);
            memsize += curve.NbPoles() * sizeof(Geom_CartesianPoint);
            break;
          case GeomAbs_BSplineCurve:
            memsize += sizeof(Geom_BSplineCurve);
            memsize += curve.NbKnots() * sizeof(Standard_Real);
            memsize += curve.NbPoles() * sizeof(Geom_CartesianPoint);
            break;
          case GeomAbs_OtherCurve:
            // What kind of curve should this be?
            memsize += sizeof(Geom_Curve);
            break;
          default:
            break;
          }
        } break;
      case TopAbs_VERTEX:
        {
          // tolerance
          memsize += sizeof(Standard_Real);
          memsize += sizeof(Geom_CartesianPoint);
        } break;
      default:
        break;
      }
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
    writer.Stream() << writer.ind() << "<Part file=\"" << writer.addFile("PartShape.brp", this) << "\"/>" << std::endl;
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
    unsigned long ulSize = 0; 
    std::streambuf* buf = file.rdbuf();
    if ( buf ) {
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
  std::ofstream file(fi.filePath().c_str(), std::ios::out | std::ios::binary);
  if (reader)
    reader >> file.rdbuf();
  file.close();

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


Feature::Feature(void) 
{
  ADD_PROPERTY(Shape, (TopoDS_Shape()));
}

Feature::~Feature()
{
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


PyObject *Feature::getPyObject(void)
{
 if(PythonObject.is(Py::_None())){
    // ref counter is set to 1
    PythonObject.set(new PartFeaturePy(this),false);
  }
  return Py::new_reference_to(PythonObject); 
}


