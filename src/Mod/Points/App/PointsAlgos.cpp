/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
#ifdef FC_OS_LINUX
# include <unistd.h>
#endif
# include <sstream>
#endif


#include "PointsAlgos.h"
#include "Points.h"

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Console.h>
#include <Base/Sequencer.h>


using namespace Points;

void PointsAlgos::Load(PointKernel &points, const char *FileName)
{
    Base::FileInfo File(FileName);
  
    // checking on the file
    if (!File.isReadable())
        throw Base::Exception("File to load not existing or not readable");
    
    if (File.extension() == "asc" ||File.extension() == "ASC")
        LoadAscii(points,FileName);
    else
        throw Base::Exception("Unknown ending");
}

void PointsAlgos::LoadAscii(PointKernel &points, const char *FileName)
{
    float x,y,z;
    int LineCnt=0;
    std::string line;
    std::ifstream tmp_str(FileName);

    Base::Sequencer().start( "Counting lines...", 10 );
    Base::Sequencer().next();
    // estemating size
    while(std::getline(tmp_str,line))
        LineCnt++;
    Base::Sequencer().stop();

    // resize the PointKernel
    points.resize(LineCnt);

    Base::Sequencer().start( "Loading points...", LineCnt );
  
    // again to the beginning
    std::ifstream file(FileName);
    LineCnt = 0;

    try {
        // read file
        std::stringstream str;
        while (std::getline(file, line)) {
            str.str(line);
            str >> x >> y >> z;
            if (!str) {
                // a comment line
                str.clear();
            }
            else {
                points[LineCnt] = Base::Vector3f(x,y,z);
                Base::Sequencer().next();
                LineCnt++;
            }
        }
    }
    catch(...) {
        points.clear();
        throw Base::Exception("Reading in points failed.");
    }

    // now remove the last points from the kernel
    // Note: first we allocate memory corresponding to the number of lines (points and comments)
    //       and read in the file twice. But then the size of the kernel is too high
    if (LineCnt < (int)points.size())
        points.erase(points.begin() + LineCnt, points.end());

    Base::Sequencer().stop();
}
