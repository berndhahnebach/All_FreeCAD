/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <Inventor/SoDB.h>
# include <Inventor/SoInput.h>
# include <Inventor/nodes/SoSeparator.h>
#endif

#include "ViewProviderInventorObject.h"
#include <App/InventorObject.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>
#include <sstream>
#include <strstream>

using namespace Gui;

PROPERTY_SOURCE(Gui::ViewProviderInventorObject, Gui::ViewProviderDocumentObject)

ViewProviderInventorObject::ViewProviderInventorObject()
{
    pcBuffer = new SoSeparator();
    pcBuffer->ref();
    pcFile = new SoSeparator();
    pcFile->ref();
}

ViewProviderInventorObject::~ViewProviderInventorObject()
{
    pcBuffer->unref();
    pcFile->unref();
}

void ViewProviderInventorObject::attach(App::DocumentObject *pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);
    SoGroup* pcFileBuf = new SoGroup();
    pcFileBuf->addChild(pcBuffer);
    pcFileBuf->addChild(pcFile);
    addDisplayMaskMode(pcFileBuf, "FileBuffer");
    addDisplayMaskMode(pcBuffer, "Buffer");
    addDisplayMaskMode(pcFile, "File");
}

void ViewProviderInventorObject::setDisplayMode(const char* ModeName)
{
  if ( strcmp("File+Buffer",ModeName)==0 )
    setDisplayMaskMode("FileBuffer");
  else if ( strcmp("Buffer",ModeName)==0 )
    setDisplayMaskMode("Buffer");
  else if ( strcmp("File",ModeName)==0 )
    setDisplayMaskMode("File");
  ViewProviderDocumentObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderInventorObject::getDisplayModes(void) const
{
  std::vector<std::string> StrList;
  StrList.push_back("File+Buffer");
  StrList.push_back("Buffer");
  StrList.push_back("File");
  return StrList;
}

void ViewProviderInventorObject::updateData(const App::Property* prop)
{
    App::InventorObject* ivObj = static_cast<App::InventorObject*>(pcObject);
    if (prop == &ivObj->Buffer) {
        // read from buffer
        SoInput in;
        std::string buffer = ivObj->Buffer.getValue();
        pcBuffer->removeAllChildren();
        if (buffer.empty()) return;
        in.setBuffer((void *)buffer.c_str(), buffer.size());
        SoSeparator * node = SoDB::readAll(&in);
        if (node) pcBuffer->addChild(node);
    }
    if (prop == &ivObj->FileName) {
        // read also from file
        const char* filename = ivObj->FileName.getValue();
        QFile file(QString::fromUtf8(filename));
        SoInput in;
        pcFile->removeAllChildren();
        if (file.open(QFile::ReadOnly)) {
            QByteArray buffer = file.readAll();
            in.setBuffer((void *)buffer.constData(), buffer.length());
            SoSeparator * node = SoDB::readAll(&in);
            if (node) pcFile->addChild(node);
        }
    }
}
