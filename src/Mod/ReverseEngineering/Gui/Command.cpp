/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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

#include <sstream>

#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>

#include "../App/ApproxSurface.h"

using namespace std;

DEF_STD_CMD_A(CmdApproxSurface);

CmdApproxSurface::CmdApproxSurface()
  : Command("Reen_ApproxSurface")
{
    sAppModule      = "Reen";
    sGroup          = QT_TR_NOOP("Reverse Engineering");
    sMenuText       = QT_TR_NOOP("Approximate surface...");
    sToolTipText    = QT_TR_NOOP("Approximate a B-Spline surface");
    sWhatsThis      = sToolTipText;
    sStatusTip      = sToolTipText;
    sPixmap         = "actions/FitSurface";
}

void CmdApproxSurface::activated(int iMsg)
{
    std::vector<App::DocumentObject*> obj = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
    const Mesh::MeshObject& mesh = static_cast<Mesh::Feature*>(obj[0])->Mesh.getValue();
    if (mesh.countSegments() > 0) {
        const Mesh::Segment& segm = mesh.getSegment(0);
        const std::vector<unsigned long>& inds = segm.getIndices();
        MeshCore::MeshFacetIterator f_iter(mesh.getKernel());
        std::set<unsigned long> points;
        for (std::vector<unsigned long>::const_iterator it = inds.begin(); it != inds.end(); ++it) {
            f_iter.Set(*it);
            MeshCore::MeshFacet face = f_iter.GetIndices();
            points.insert(face._aulPoints[0]);
            points.insert(face._aulPoints[1]);
            points.insert(face._aulPoints[2]);
        }

        std::stringstream str;
        str << "__points=[]" << std::endl;
        for (std::set<unsigned long>::iterator it=points.begin(); it != points.end(); ++it) {
            Mesh::MeshPoint p = mesh.getPoint(*it);
            str << "__points.append((" << p.x << "," << p.y << "," << p.z << "))" << std::endl;
        }

        str << "import ReverseEngineering" << std::endl;
        str << "__spline = ReverseEngineering.approxSurface(__points)" << std::endl;
        str << "App.ActiveDocument.addObject(\"Part::Feature\",\"Surface\").Shape"
               "=__spline.toShape(0.0,1.0,0.0,1.0)" << std::endl;
        str << "App.ActiveDocument.recompute()" << std::endl;
        str << "del __points" << std::endl;
        str << "del __spline" << std::endl;
        
        openCommand("Fit surface");
        doCommand(Gui::Command::Doc, str.str().c_str());
        commitCommand(); 
        updateActive();
    }
}

bool CmdApproxSurface::isActive(void)
{
    if (getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1)
        return true;
    return false;
}

void CreateReverseEngineeringCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdApproxSurface());
}
