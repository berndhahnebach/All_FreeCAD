/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include <Inventor/C/basic.h>
#include <Inventor/SbBox2s.h>
#include <Inventor/SbVec2f.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbPlane.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbTime.h>
#include <QCursor>
#include <Base/BaseClass.h>

// forward declarations
class SoEvent;
class SoQtViewer;
class SoCamera;
class SoSensor;
class SbSphereSheetProjector;

namespace Gui {

class View3DInventorViewer;
class AbstractMouseModel;

/**
 * The navigation style base class
 * @author Werner Mayer
 */
class GuiExport NavigationStyle : public Base::BaseClass
{
    TYPESYSTEM_HEADER();

public:
    enum ViewerMode {
        IDLE,
        INTERACT,
        ZOOMING,
        BOXZOOM,
        PANNING,
        DRAGGING,
        SPINNING,
        SEEK_WAIT_MODE,
        SEEK_MODE,
        SELECTION
    };

    enum ePickMode {
        Lasso       = 0,  /**< Select objects using a lasso. */
        Rectangle   = 1,  /**< Select objects using a rectangle. */
        BoxZoom     = 2,  /**< Perform a box zoom. */
        Clip        = 3,  /**< Clip objects using a lasso. */
    };

public:
    NavigationStyle();
    virtual ~NavigationStyle();

    void setViewer(View3DInventorViewer*);

    void setAnimationEnabled(const SbBool enable);
    SbBool isAnimationEnabled(void) const;

    void startAnimating(const SbVec3f& axis, float velocity);
    void stopAnimating(void);
    SbBool isAnimating(void) const;

    void updateAnimation();
    void redraw();

    void setCameraOrientation(const SbRotation& rot);
    void boxZoom(const SbBox2s& box);
    virtual void viewAll();

    void setViewingMode(const ViewerMode newmode);
    int getViewingMode() const;
    virtual SbBool processSoEvent(const SoEvent * const ev);

    void setPopupMenuEnabled(const SbBool on);
    SbBool isPopupMenuEnabled(void) const;

    void startPicking(ePickMode = Lasso);
    void stopPicking();
    SbBool isPicking() const;
    const std::vector<SbVec2f>& getPickedPolygon(SbBool* clip_inner=0) const;

protected:
    void initialize();
    void finalize();

    void interactiveCountInc(void);
    void interactiveCountDec(void);
    int getInteractiveCount(void) const;

    SbBool isViewing(void) const;
    void setViewing(SbBool);
    SbBool isSeekMode(void) const;
    void setSeekMode(SbBool enable);
    SbBool seekToPoint(const SbVec2s screenpos);
    void seekToPoint(const SbVec3f& scenepos);

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
    SbBool doSpin();

    SbBool handleEventInForeground(const SoEvent* const e);
    virtual void openPopupMenu(const SbVec2s& position);

    void clearLog(void);
    void addToLog(const SbVec2s pos, const SbTime time);

protected:
    struct { // tracking mouse movement in a log
        short size;
        short historysize;
        SbVec2s * position;
        SbTime * time;
    } log;

    View3DInventorViewer* viewer;
    ViewerMode currentmode;
    SbVec2f lastmouseposition;
    SbPlane panningplane;
    SbTime prevRedrawTime;
    SbTime centerTime;
    SbBool menuenabled;
    SbBool ctrldown, shiftdown, altdown;
    SbBool button1down, button2down, button3down;

    /** @name Mouse model */
    //@{
    AbstractMouseModel* pcMouseModel;
    std::vector<SbVec2f> pcPolygon;
    SbBool clipInner;
    //@}

    /** @name Spinning data */
    //@{
    SbBool spinanimatingallowed;
    int spinsamplecounter;
    SbRotation spinincrement;
    SbRotation spinRotation;
    SbSphereSheetProjector * spinprojector;
    //@}

private:
    struct NavigationStyleP* pimpl;
    friend struct NavigationStyleP;
};

class GuiExport InventorNavigationStyle : public NavigationStyle {
    typedef NavigationStyle inherited;

    TYPESYSTEM_HEADER();

public:
    InventorNavigationStyle();
    ~InventorNavigationStyle();

    SbBool processSoEvent(const SoEvent * const ev);
};

class GuiExport CADNavigationStyle : public NavigationStyle {
    typedef NavigationStyle inherited;

    TYPESYSTEM_HEADER();

public:
    CADNavigationStyle();
    ~CADNavigationStyle();

    SbBool processSoEvent(const SoEvent * const ev);

    bool _bRejectSelection;
    bool _bSpining;
    SbTime MoveTime;
    QCursor _oldCursor;
};

} // namespace Gui

#endif // GUI_NAVIGATIONSTYLE_H 
