/***************************************************************************
 *   Copyright (c) 2007                                                    *
 *   Joachim Zettler <Joachim.Zettler@gmx.de>                              *
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

#ifndef STUFF_H
#define STUFF_H

//Funktor für sort von pair<float,TopoDS_Compound> damit die Pairs von groß nach klein sortiert werden
bool inline FloatWirePairHuge(const std::pair<float,TopoDS_Wire>& _Left, const std::pair<float,TopoDS_Wire>& _Right)
{
    return _Left.first > _Right.first;
}



bool inline FloatHuge(const float _Left, const float _Right)
{
    return _Left > _Right;
}

struct edge_container
{
    TopoDS_Edge edge;
    gp_Pnt firstPoint;
    gp_Pnt lastPoint;

};

struct projectPointContainer
{
    gp_Pnt point;
    gp_Dir normalvector;
    TopoDS_Face face;
};

#endif

