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



#ifndef SKETCHER_CONSTRAINT_H
#define SKETCHER_CONSTRAINT_H


#include <Base/Persistence.h>

namespace Sketcher
{

enum ConstraintType {
    None,
    Coincident,
    Horizontal,
    Vertical
};

/// define if you whant to use the end or start point
enum PointPos {start,end,mid };

class Constraint :public Base::Persistence
{
    TYPESYSTEM_HEADER();

public:
    ~Constraint();
    Constraint();
    Constraint(const Constraint&);
    virtual Constraint *clone(void)const;


	// from base class
    virtual unsigned int getMemSize (void) const;
	virtual void Save (Base::Writer &/*writer*/) const;
    virtual void Restore(Base::XMLReader &/*reader*/);

    virtual PyObject *getPyObject(void);

    friend class Sketch;

public:
    ConstraintType Type;
    std::string Name;
    float Value;
    int First;
    PointPos FirstPos;
    int Second;
    PointPos SecondPos;
    int Extern;



};

} //namespace Part


#endif // SKETCHER_CONSTRAINT_H
