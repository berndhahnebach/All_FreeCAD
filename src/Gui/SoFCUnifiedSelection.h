/***************************************************************************
 *   Copyright (c) 2005 Jürgen Riegel <juergen.riegel@web.de>              *
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

#ifndef GUI_SOFCUNIFIEDSELECTION_H
#define GUI_SOFCUNIFIEDSELECTION_H

# ifdef FC_OS_MACOSX
# include <OpenGL/gl.h>
# else
# ifdef FC_OS_WIN32
#  include <windows.h>
# endif
# include <GL/gl.h>
# endif

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/nodes/SoLightModel.h>
#include "View3DInventorViewer.h"

class SoFullPath;
class SoPickedPoint;


namespace Gui {


/**  Unified Selection node
 *  This is the new selection node for the 3D Viewer which will 
 *  gradually remove all the low level selection nodes in the view
 *  provider. The handling of the highlighting and the selection will
 *  be unified here. 
 *  \author Jürgen Riegel
 */
class GuiExport SoFCUnifiedSelection : public SoSeparator {
    typedef SoSeparator inherited;

    SO_NODE_HEADER(Gui::SoFCUnifiedSelection);

public:
    static void initClass(void);
    static void finish(void);
    SoFCUnifiedSelection(void);

    enum HighlightModes {
        AUTO, ON, OFF
    };

    enum SelectionModes {
        SEL_ON, SEL_OFF
    };

    enum Selected {
        NOTSELECTED, SELECTED
    };

    enum Styles {
        EMISSIVE, EMISSIVE_DIFFUSE, BOX
    };

    SbBool isHighlighted(void) const {return highlighted;}

    SoSFColor colorHighlight;
    SoSFColor colorSelection;
    SoSFEnum style;
    SoSFEnum selected;
    SoSFEnum highlightMode;
    SoSFEnum selectionMode;

    SoSFString documentName;
    SoSFString objectName;
    SoSFString subElementName;

    //virtual void doAction(SoAction *action);
    //virtual void GLRender(SoGLRenderAction * action);

    virtual void handleEvent(SoHandleEventAction * action);
    //virtual void GLRenderBelowPath(SoGLRenderAction * action);
    //virtual void GLRenderInPath(SoGLRenderAction * action);
    //static  void turnOffCurrentHighlight(SoGLRenderAction * action);

    friend class View3DInventorViewer;
protected:
    virtual ~SoFCUnifiedSelection();
    //virtual void redrawHighlighted(SoAction * act, SbBool flag);
    //virtual SbBool readInstance(SoInput *  in, unsigned short  flags); 

    View3DInventorViewer *viewer;
private:
    //static void turnoffcurrent(SoAction * action);
    //void setOverride(SoGLRenderAction * action);
    //SbBool isHighlighted(SoAction *action);
    //SbBool preRender(SoGLRenderAction *act, GLint &oldDepthFunc);
    const SoPickedPoint* getPickedPoint(SoHandleEventAction*) const;

    static SoFullPath * currenthighlight;

    SbBool highlighted;
    SoColorPacker colorpacker;

    SbBool bShift;
    SbBool bCtrl;
};


} // namespace Gui

#endif // !GUI_SOFCSELECTION_H
