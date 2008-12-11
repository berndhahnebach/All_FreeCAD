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
#if 0
// forward declarations
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
    AbstractStyle();
    virtual ~AbstractStyle();

    virtual SbBool processSoEvent(const SoEvent * const ev, View3DInventorViewer*);

protected:
    SbVec2f lastmouseposition;
    SbPlane panningplane;
    SbBool ctrldown, shiftdown;
    SbBool button1down, button3down;
    SbBool editing, allowSpining;;
    SbTime CenterTime;

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

    ViewerMode currentmode;
    void setMode(const ViewerMode mode);
};

class GuiExport ExaminerStyle : public AbstractStyle
{
public:
    ExaminerStyle();
    ~ExaminerStyle();

    SbBool processSoEvent(const SoEvent * const ev, View3DInventorViewer*);
};

class GuiExport CADStyle : public AbstractStyle
{
public:
    CADStyle();
    ~CADStyle();

    SbBool processSoEvent(const SoEvent * const ev, View3DInventorViewer*);
    bool _bRejectSelection;
    bool _bSpining;
    SbTime MoveTime;
    QCursor _oldCursor;
};

} // namespace Gui
#endif
#endif // GUI_NAVIGATIONSTYLE_H 
