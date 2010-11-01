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
# include <Standard_math.hxx>
# include <Inventor/nodes/SoTranslation.h>
# include <Inventor/nodes/SoText2.h>
# include <Inventor/nodes/SoFont.h>
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
#include <Mod/Sketcher/App/SketchObject.h>


#include "DrawSketchHandler.h"
#include "ViewProviderSketch.h"


using namespace SketcherGui;
using namespace Sketcher;


//**************************************************************************
// Construction/Destruction

DrawSketchHandler::DrawSketchHandler()
:sketchgui(0)
{

}

DrawSketchHandler::~DrawSketchHandler()
{

}

void DrawSketchHandler::quit(void)
{
    assert(sketchgui);
    sketchgui->drawEdit(std::vector<Base::Vector2D>());
    resetPositionText();
    unsetCursor();
    sketchgui->purgeHandler();
}

//**************************************************************************
// Helpers

Sketcher::SketchObject* DrawSketchHandler::getObject(void)
{
    return dynamic_cast<Sketcher::SketchObject*>(sketchgui->getObject());
}

int DrawSketchHandler::getHighestVertexIndex(void)
{
	return getObject()->Geometry.getSize()*2 - 1;
}

int DrawSketchHandler::getHighestCurveIndex(void)
{
	return getObject()->Geometry.getSize() - 1;
}

void DrawSketchHandler::setCursor( const QPixmap &p,int x,int y )
{
    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
 
        oldCursor = viewer->getWidget()->cursor();
        QCursor cursor(p, 4, 4);
		actCursor = cursor;

        viewer->getWidget()->setCursor(cursor);
    }

}

void DrawSketchHandler::applyCursor(void )
{
    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        viewer->getWidget()->setCursor(actCursor);
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

void DrawSketchHandler::setPositionText(const Base::Vector2D &Pos)
{
    sketchgui->setPositionText(Pos);

}
void DrawSketchHandler::resetPositionText(void)
{
    sketchgui->resetPositionText();
}
