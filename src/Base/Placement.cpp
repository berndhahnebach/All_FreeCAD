/***************************************************************************
 *   Copyright (c) 2006 Juergen Riegel                                     *
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


#include "Placement.h"
#include "Rotation.h"

using namespace Base;

Placement::Placement()
{
    _q[0]=_q[1]=_q[2]=0.0;_q[3]=1.0;
}

Base::Matrix4D Placement::toMatrix(void) const
{
    Base::Rotation rot((float)this->_q[0],(float)this->_q[1],(float)this->_q[2],(float)this->_q[3]);
    Base::Matrix4D matrix;
    rot.getValue(matrix);
    matrix[0][3] = this->_pos.x;
    matrix[1][3] = this->_pos.y;
    matrix[2][3] = this->_pos.z;
    return matrix;
}

void Placement::fromMatrix(const Base::Matrix4D& matrix)
{
    Base::Rotation rot(matrix);
    rot.getValue(this->_q[0],this->_q[1],this->_q[2],this->_q[3]);
    this->_pos.x = matrix[0][3];
    this->_pos.y = matrix[1][3];
    this->_pos.z = matrix[2][3];
}
