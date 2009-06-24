/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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


#ifndef GUI_NAVIGATIONSTYLE_H
#define GUI_NAVIGATIONSTYLE_H

// forward declarations
class SoEvent;
class SoQtViewer;

namespace Gui {
class GuiExport NavigationStyle
{
protected:
    enum ViewerMode {
        IDLE,
        INTERACT,
        ZOOMING,
        PANNING,
        DRAGGING,
        SPINNING,
        SEEK_WAIT_MODE,
        SEEK_MODE,
        SELECTION
    };

public:
    NavigationStyle();
    virtual ~NavigationStyle();

    void setAnimationEnabled(const SbBool enable);
    SbBool isAnimationEnabled(void) const;

    void startAnimating(const SbVec3f& axis, float velocity);
    void stopAnimating(void);
    SbBool isAnimating(void) const;

    void setViewerMode(const ViewerMode newmode);
    ViewerMode getViewMode() const;
    virtual SbBool processSoEvent(SoQtViewer* viewer, const SoEvent * const ev);

protected:
    void reorientCamera(SoCamera * camera, const SbRotation & rot);
    void panCamera(SoCamera * camera,
                   float vpaspect,
                   const SbPlane & panplane,
                   const SbVec2f & previous,
                   const SbVec2f & current);
    void pan(SoCamera* camera);
    void panToCenter(const SbPlane & pplane, const SbVec2f & currpos);
    void zoom(SoCamera * camera, float diffvalue);
    void zoomByCursor(const SbVec2f & thispos, const SbVec2f & prevpos);
    void spin(const SbVec2f & pointerpos);
    ViewerMode doSpin();
    void updateSpin();

private:
    void clearLog(void);
    void addToLog(const SbVec2s pos, const SbTime time);

private:
    struct { // tracking mouse movement in a log
        short size;
        short historysize;
        SbVec2s * position;
        SbTime * time;
    } log;

    SoQtViewer* viewer;
    ViewerMode currentmode;
    SbVec2f lastmouseposition;
    SbTime prevRedrawTime;

    /** @name camera data */
    //@{
    SbPlane panningplane;
    //@}

    /** @name Spinning data */
    //@{
    SbBool spinanimatingallowed;
    int spinsamplecounter;
    SbRotation spinincrement;
    SbRotation spinRotation;
    SbSphereSheetProjector * spinprojector;
    //@}
};

} // namespace Gui
#if 0
class SoEvent;

namespace Gui {

class View3DInventorViewer;

/**
 * The navigation style base class
 * @author Werner Mayer
 */
class GuiExport AbstractStyle
{
public:

protected:
    SbBool ctrldown, shiftdown;
    SbBool button1down, button3down;
    SbTime centerTime;
};

class GuiExport CADStyle : public AbstractStyle
{
public:
    CADStyle();
    ~CADStyle();

    bool _bRejectSelection;
    bool _bSpining;
    SbTime MoveTime;
    QCursor _oldCursor;
};

} // namespace Gui
#endif
#endif // GUI_NAVIGATIONSTYLE_H 
