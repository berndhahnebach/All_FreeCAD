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
# include <fcntl.h>
# include <TopTools_HSequenceOfShape.hxx>
# include <STEPControl_Writer.hxx>
# include <STEPControl_Reader.hxx>
# include <StepData_StepModel.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Shape.hxx>
# include <TopoDS_Shell.hxx>
# include <TopoDS_Solid.hxx>
# include <TopExp_Explorer.hxx>
#endif

#include <Handle_XSControl_WorkSession.hxx>
#include <Handle_XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_TransferReader.hxx>

#include <Base/Console.h>
#include <Base/Sequencer.h>
#include <App/Document.h>


#include "ImportStep.h"
#include "PartFeature.h"

using namespace Part;

int Part::ImportStepParts(App::Document *pcDoc, const char* Name)
{
    STEPControl_Reader aReader;
    TopoDS_Shape aShape;
    Base::FileInfo fi(Name);

    if (!fi.isReadable()) {
        Base::Console().Log("ImportStep() not able to open %s!\n",Name);
        throw Base::Exception("Cannot open file Step file");
    }

    // just do show the wait cursor when the Gui is up
    Base::SequencerLauncher seq("Load STEP", 1);
    seq.next();

    if (aReader.ReadFile((Standard_CString)Name) != IFSelect_RetDone) {
        throw Base::Exception("Cannot open file Step file");
    }

    // Root transfers
    Standard_Integer nbr = aReader.NbRootsForTransfer();
    //aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
    for (Standard_Integer n = 1; n<= nbr; n++) {
        Base::Console().Log("STEP: Transfering Root %d\n",n);
        aReader.TransferRoot(n);
    }
    // Collecting resulting entities
    Standard_Integer nbs = aReader.NbShapes();
    if (nbs == 0) {
        throw Base::Exception("No shapes found in file ");
    }
    else {
        //Handle(StepData_StepModel) Model = aReader.StepModel();
        //Handle_XSControl_WorkSession ws = aReader.WS();
        //Handle_XSControl_TransferReader tr = ws->TransferReader();

        for (Standard_Integer i=1; i<=nbs; i++) {
            Base::Console().Log("STEP:   Transfering Shape %d\n",i);
            aShape = aReader.Shape(i);

            TopExp_Explorer ex;

            // load each solid as an own object
            for (ex.Init(aShape, TopAbs_SOLID); ex.More(); ex.Next())
            {
                // get the shape 
                const TopoDS_Solid& aSolid = TopoDS::Solid(ex.Current());

                std::string name = fi.fileNamePure();
                //Handle_Standard_Transient ent = tr->EntityFromShapeResult(aSolid, 3);
                //if (!ent.IsNull()) {
                //    name += ws->Model()->StringLabel(ent)->ToCString();
                //}

                Part::Feature *pcFeature = static_cast<Part::Feature*>(pcDoc->addObject("Part::Feature", name.c_str()));
                pcFeature->Shape.setValue(aSolid);
            }
            // load all non-solids now
            for (ex.Init(aShape, TopAbs_SHELL, TopAbs_SOLID); ex.More(); ex.Next())
            {
                // get the shape 
                const TopoDS_Shell& aShell = TopoDS::Shell(ex.Current());

                std::string name = fi.fileNamePure();
                //Handle_Standard_Transient ent = tr->EntityFromShapeResult(aShell, 3);
                //if (!ent.IsNull()) {
                //    name += ws->Model()->StringLabel(ent)->ToCString();
                //}

                Part::Feature *pcFeature = static_cast<Part::Feature*>(pcDoc->addObject("Part::Feature", name.c_str()));
                pcFeature->Shape.setValue(aShell);
            }
        }
    }

    return 0;
}
