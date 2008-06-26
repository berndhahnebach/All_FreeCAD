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


#ifndef PART_TOPOSHAPE_H
#define PART_TOPOSHAPE_H

#include <TopoDS_Shape.hxx>
#include <App/ComplexGeoData.h>

namespace Part
{

/** The representation for a CAD Shape
 */
class AppPartExport TopoShape : public Data::ComplexGeoData
{
    TYPESYSTEM_HEADER();

public:
    TopoShape();
    TopoShape(const TopoDS_Shape&);
    TopoShape(const TopoShape&);
    ~TopoShape();

    void operator = (const TopoShape&);

    void setTransform(const Base::Matrix4D& rclTrf);
    Base::Matrix4D getTransform(void) const;

    Base::BoundBox3d getBoundBox(void)const;

    /** @name Save/restore */
    //@{
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);

    void SaveDocFile (Base::Writer &writer) const;
    void RestoreDocFile(Base::Reader &reader);
    unsigned int getMemSize (void) const;
    //@}

    /** @name Input/Output */
    //@{
    void read(const char *FileName);
    bool exportIges(const char *FileName) const;
    bool exportStep(const char *FileName) const;
    bool exportBrep(const char *FileName) const;
    bool exportStl(const char *FileName) const;
    //@}

    /** @name Query*/
    //@{
    bool isNull() const;
    bool isValid() const;
    //@}

    /** @name Boolean operation*/
    //@{
    TopoDS_Shape cut(TopoDS_Shape) const;
    TopoDS_Shape common(TopoDS_Shape) const;
    TopoDS_Shape fuse(TopoDS_Shape) const;
    TopoDS_Shape section(TopoDS_Shape) const;
    //@}
    TopoDS_Shape transform(const Base::Matrix4D&) const;

    TopoDS_Shape _Shape;
};

} //namespace Part


#endif // PART_TOPOSHAPE_H
