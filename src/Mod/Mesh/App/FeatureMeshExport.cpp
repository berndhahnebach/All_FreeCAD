/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/gzstream.h>
#include <Base/Sequencer.h>
#include "FeatureMeshExport.h"
#include "MeshFeature.h"

#include "Core/MeshIO.h"


using namespace Mesh;
using namespace MeshCore;

PROPERTY_SOURCE(Mesh::Export, App::AbstractFeature)

Export::Export(void)
{
    ADD_PROPERTY(Source  ,(0));
    ADD_PROPERTY(FileName,(""));
    ADD_PROPERTY(Format  ,(""));
}

int Export::execute(void)
{
    Mesh::Feature *pcFeat  = dynamic_cast<Mesh::Feature*>(Source.getValue());
    if(!pcFeat || pcFeat->getStatus() != Valid) {
        setError("Cannot export invalid mesh feature '%s'", pcFeat->name.getValue());
        return 1;
    }

    // ask for write permission
    Base::FileInfo fi(FileName.getValue());
    Base::FileInfo di(fi.dirPath().c_str());
    if ( fi.exists() && fi.isWritable() == false || di.exists() == false || di.isWritable() == false ) {
        setError("No write permission for file '%s'",FileName.getValue());
        return 1;
    }

    if ( fi.hasExtension("bms") ) {
        std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );
        pcFeat->Mesh.getValue().Write( str );
    } else if ( fi.hasExtension("stl") || fi.hasExtension("ast") ) {
        std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );
        MeshOutput aWriter(pcFeat->Mesh.getValue());

        // write file
        bool ok = false;
        if ( std::string(Format.getValue()) == "ASCII STL" )
            ok = aWriter.SaveAsciiSTL( str );
        else // "Binary STL"
            ok = aWriter.SaveBinarySTL( str );

        if ( !ok ) {
            setError("Export of STL mesh to file '%s' failed",FileName.getValue());
            return 1;
        }
    } else if ( fi.hasExtension("obj")  ) {
        std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );
        MeshCore::MeshOutput aWriter(pcFeat->Mesh.getValue());

        // write file
        if ( !aWriter.SaveOBJ(str) ) {
            setError("Export of OBJ mesh to file '%s' failed",FileName.getValue());
            return 1;
        }
    } else if ( fi.hasExtension("iv")  ) {
        std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );
        MeshCore::MeshOutput aWriter(pcFeat->Mesh.getValue());

        // write file
        if ( !aWriter.SaveInventor(str) ) {
            setError("Export of Inventor mesh to file '%s' failed",FileName.getValue());
            return 1;
        }
    } else if ( fi.hasExtension("wrl") || fi.hasExtension("vrml") ) {
        std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );
        MeshCore::SaveMeshVRML aWriter(pcFeat->Mesh.getValue());

        // write file
        App::Material rclMat;
        if ( !aWriter.Save(str,rclMat) ) {
            setError("Export of VRML mesh to file '%s' failed",FileName.getValue());
            return 1;
        }
    } else if ( fi.hasExtension("wrz") ) {
        // Compressed VRML is nothing else than a GZIP'ped VRML ascii file
        Base::ogzstream gzip( FileName.getValue(), std::ios::out | std::ios::binary );
        MeshCore::SaveMeshVRML aWriter(pcFeat->Mesh.getValue());

        // write file
        App::Material rclMat;
        if ( !aWriter.Save(gzip,rclMat) ) {
            setError("Export of compressed VRML mesh to file '%s' failed",FileName.getValue());
            return 1;
        }
    } else if ( fi.hasExtension("nas") || fi.hasExtension("bdf") ) {
        std::ofstream str( FileName.getValue(), std::ios::out | std::ios::binary );
        MeshCore::MeshOutput aWriter(pcFeat->Mesh.getValue());

        // write file
        if ( !aWriter.SaveNastran(str) ) {
            setError("Export of NASTRAN mesh to file '%s' failed",FileName.getValue());
            return 1;
        }
    } else {
        setError("File format '%s' not supported", fi.extension().c_str());
        return 1;
    }

    return 0;
}
