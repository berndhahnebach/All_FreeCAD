/***************************************************************************
 *   Copyright (c) 2010 Jürgen Riegel <juergen.riegel@web.de>              *
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
#include <Standard_math.hxx>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Interpreter.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Macro.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/View3DInventor.h>

#include <Mod/Part/App/Geometry.h>


#include "DrawSketchHandler.h"
#include "ViewProviderSketch.h"


using namespace SketcherGui;
using namespace Sketcher;


//**************************************************************************
// Construction/Destruction

DrawSketchHandler::DrawSketchHandler()
{

}

DrawSketchHandler::~DrawSketchHandler()
{

}


//**************************************************************************
// Helpers

void DrawSketchHandler::setCursor( const QPixmap &p,int x,int y )
{
    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
 
        oldCursor = viewer->getWidget()->cursor();
        QCursor cursor(p, 4, 4);

        viewer->getWidget()->setCursor(cursor);
    }

}

void DrawSketchHandler::unsetCursor(void)
{
    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        viewer->getWidget()->setCursor(oldCursor);
    }
}

/// Run a App level Action
void DrawSketchHandler::doCommand(const char* sCmd,...)
{
    // temp buffer
    size_t format_len = std::strlen(sCmd)+4024;
    char* format = (char*) malloc(format_len);
    va_list namelessVars;
    va_start(namelessVars, sCmd);  // Get the "..." vars
    vsnprintf(format, format_len, sCmd, namelessVars);
    va_end(namelessVars);

    Gui::Application::Instance->macroManager()->addLine(Gui::MacroManager::Base,format);

    try {
        Base::Interpreter().runString(format);
    }
    catch (Base::Exception e) {
        Base::Console().Error("%s\n", e.what());
    }
    catch (...) {
       Base::Console().Error("Unknowen Exception in DrawSketchHandler::doCommand()");
    }

#ifdef FC_LOGUSERACTION
    Base::Console().Log("CmdC: %s\n",format);
#endif
    free (format);
}

void DrawSketchHandler::openCommand(const char* sCmdName)
{
    // Using OpenCommand with no active document !
    assert(Gui::Application::Instance->activeDocument());
    assert(sCmdName);

    Gui::Application::Instance->activeDocument()->openCommand(sCmdName);
}
