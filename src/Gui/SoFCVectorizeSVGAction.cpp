/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/FileInfo.h>
#include "SoFCVectorizeSVGAction.h"

using namespace Gui;

SoSVGVectorOutput::SoSVGVectorOutput()
{
}

SoSVGVectorOutput::~SoSVGVectorOutput()
{
    closeFile();
}

SbBool SoSVGVectorOutput::openFile (const char *filename)
{
    Base::FileInfo fi(filename);
#ifdef FC_OS_WIN32
    this->file.open(fi.toStdWString().c_str(), std::ios::out | std::ios::binary);
#else
    this->file.open(fi.filePath().c_str(), std::ios::out | std::ios::binary);
#endif

    return this->file.is_open();
}

void SoSVGVectorOutput::closeFile (void)
{
    if (this->file.is_open())
        this->file.close();
}

std::fstream& SoSVGVectorOutput::getFileStream()
{
    return this->file;
}

// -------------------------------------------------------

SO_ACTION_SOURCE(SoFCVectorizeSVGAction);

void SoFCVectorizeSVGAction::initClass(void)
{
    SO_ACTION_INIT_CLASS(SoFCVectorizeSVGAction, SoVectorizeAction);
}

SoFCVectorizeSVGAction::SoFCVectorizeSVGAction()
{
    SO_ACTION_CONSTRUCTOR(SoFCVectorizeSVGAction);
    this->setOutput(new SoSVGVectorOutput);
}

SoFCVectorizeSVGAction::~SoFCVectorizeSVGAction()
{
}

SoSVGVectorOutput *
SoFCVectorizeSVGAction::getOutput(void) const
{
    return static_cast<SoSVGVectorOutput*>(SoVectorizeAction::getOutput());
}

SoSVGVectorOutput *
SoFCVectorizeSVGAction::getSVGOutput(void) const
{
    return static_cast<SoSVGVectorOutput*>(SoVectorizeAction::getOutput());
}

void SoFCVectorizeSVGAction::printHeader(void) const
{
    std::ostream& str = this->getSVGOutput()->getFileStream();
    str << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
    str << "<!-- Created with FreeCAD (http://free-cad.sourceforge.net) -->" << std::endl;
}

void SoFCVectorizeSVGAction::printFooter(void) const
{
}

void SoFCVectorizeSVGAction::printBackground(void) const
{
}

void SoFCVectorizeSVGAction::printItem(const SoVectorizeItem * item) const
{
    SoVectorizeAction::printItem(item);
}

void SoFCVectorizeSVGAction::printViewport(void) const
{
}

