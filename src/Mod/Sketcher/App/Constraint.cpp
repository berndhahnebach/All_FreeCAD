/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2008     *
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

#include <Base/Writer.h>
#include <Base/Reader.h>


#include "Constraint.h"
#include "ConstraintPy.h"


using namespace Sketcher;
using namespace Base;


TYPESYSTEM_SOURCE(Sketcher::Constraint, Base::Persistence)


Constraint::Constraint()
: Type(None),
  Name(""),
  Value(0.0),
  First(-1),
  FirstPos(none),
  Second(-1),
  SecondPos(none),
  Extern(-1),
  LabelDistance(10.f)
{
}

Constraint::Constraint(const Constraint& from)
: Type(from.Type),
  Name(from.Name),
  Value(from.Value),
  First(from.First),
  FirstPos(from.FirstPos),
  Second(from.Second),
  SecondPos(from.SecondPos),
  Extern(from.Extern),
  LabelDistance(from.LabelDistance)
{
}

Constraint::~Constraint()
{
}

Constraint *Constraint::clone(void) const
{
    return new Constraint(*this);
}

PyObject *Constraint::getPyObject(void)
{
    return new ConstraintPy(new Constraint(*this));
}

unsigned int Constraint::getMemSize (void) const
{
    return 0;
}

void Constraint::Save (Writer &writer) const
{
    writer.Stream() << writer.ind() << "<Constrain "
    << "Name=\""          <<  Name            << "\" "
    << "Type=\""          <<  (int)Type       << "\" "
    << "Value=\""         <<  Value           << "\" "
    << "First=\""         <<  First           << "\" "
    << "FirstPos=\""      <<  (int)  FirstPos << "\" "
    << "Second=\""        <<  Second          << "\" "
    << "SecondPos=\""     <<  (int) SecondPos << "\" "
    << "Extern=\""        <<  Extern          << "\" "
    << "LabelDistance=\"" << LabelDistance<< "\" />"
    << std::endl;
}

void Constraint::Restore(XMLReader &reader)
{
    reader.readElement("Constrain");
    Name      = reader.getAttribute("Name");
    Type      = (ConstraintType)  reader.getAttributeAsInteger("Type");
    Value     = reader.getAttributeAsFloat("Value");
    First     = reader.getAttributeAsInteger("First");
    FirstPos  = (PointPos)  reader.getAttributeAsInteger("FirstPos");
    Second    = reader.getAttributeAsInteger("Second");
    SecondPos = (PointPos)  reader.getAttributeAsInteger("SecondPos");
    Extern    = reader.getAttributeAsInteger("Extern");

    // Read the distance a constraint label has been moved
    if (reader.hasAttribute("LabelDistance"))
        LabelDistance = reader.getAttributeAsFloat("LabelDistance");
    else
        LabelDistance = 0.0;
}
