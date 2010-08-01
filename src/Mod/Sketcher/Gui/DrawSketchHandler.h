/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef SKETCHERGUI_DrawSketchHandler_H
#define SKETCHERGUI_DrawSketchHandler_H

#include <Base/Tools2D.h>

class QPixmap;

namespace Gui {
    class View3DInventorViewer;
    class SoFCSelection;
}

namespace Sketcher {
    class Sketch;
    class SketchObject;
}

namespace SketcherGui {

class ViewProviderSketch;

/** Handler to create new sketch geometry
  * This class has to be reimplemented to create geometry in the 
  * sketcher while its in editing.
  */
class SketcherGuiExport DrawSketchHandler
{
public:
    DrawSketchHandler(ViewProviderSketch *viewp=0);
    virtual ~DrawSketchHandler();

    virtual void activated(ViewProviderSketch *sketchgui){};
    virtual void mouseMove(Base::Vector2D onSketchPos)=0;
    virtual bool pressButton(Base::Vector2D onSketchPos)=0;
    virtual bool releaseButton(Base::Vector2D onSketchPos)=0;

    friend class ViewProviderSketch;

    Sketcher::SketchObject* getObject(void);

protected:
    // helpers
    void setCursor( const QPixmap &p,int x,int y );
    void unsetCursor(void);

    void doCommand(const char* sCmd,...);
    void openCommand(const char* sCmdName);

    ViewProviderSketch *sketchgui;
    QCursor oldCursor;
};


} // namespace SketcherGui


#endif // SKETCHERGUI_DrawSketchHandler_H

