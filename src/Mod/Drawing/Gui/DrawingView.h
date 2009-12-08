/***************************************************************************
 *   Copyright (c) 2007 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is Drawing of the FreeCAD CAx development system.           *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A DrawingICULAR PURPOSE.  See the      *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef DrawingView_H
#define DrawingView_H

#include <Gui/MDIView.h>

class QSlider;
class QAction;
class QActionGroup;
class QPopupMenu;
class QToolBar;
class QSvgWidget;
class QScrollArea;

namespace DrawingGui
{

class DrawingGuiExport DrawingView : public Gui::MDIView
{
    Q_OBJECT

public:
    DrawingView(QWidget* parent = 0);
    virtual ~DrawingView();

    bool load (const QString & file);
    const char *getName(void) const {return "DrawingView";}
    void onUpdate(void){};

    bool onMsg(const char* pMsg,const char** ppReturn);
    bool onHasMsg(const char* pMsg) const ;
    void viewAll();

    //virtual void clearDrawing();
    //virtual int createDrawingCopy(void* pSrcPixelData, unsigned long width, unsigned long height, int format, unsigned short numSigBitsPerSample, int displayMode = IV_DISPLAY_RESET);
    //virtual int pointDrawingTo(void* pSrcPixelData, unsigned long width, unsigned long height, int format, unsigned short numSigBitsPerSample, bool takeOwnership, int displayMode = IV_DISPLAY_RESET);

    //virtual void EnableStatusBar(bool Enable);
    //virtual void EnableToolBar(bool Enable);
    //virtual void EnableMouseEvents(bool Enable);
    //virtual void EnableOneToOneAction(bool Enable);
    //virtual void EnableFitDrawingAction(bool Enable);
    //virtual void EnableColorActions(bool Enable);
    //virtual int createColorMap(int numEntriesReq = 0, bool Initialise = true);
    //virtual void clearColorMap();
    //virtual int getNumColorMapEntries() const;
    //virtual int setColorMapRGBAValue(int index, float red, float green, float blue, float alpha = 1.0);
    //virtual int setColorMapRedValue(int index, float value);
    //virtual int setColorMapGreenValue(int index, float value);
    //virtual int setColorMapBlueValue(int index, float value);
    //virtual int setColorMapAlphaValue(int index, float value);

public Q_SLOTS:
    virtual void fitDrawing();
    virtual void oneToOneDrawing();

protected Q_SLOTS:
    virtual void handleColorAct( QAction* act);
    virtual void sliderValueAdjusted(int NewValue);
    virtual void drawGraphics();

protected:
    virtual void createActions();
    virtual void showOriginalColors();
    virtual void showBrightened();
    virtual void mousePressEvent(QMouseEvent* cEvent);
    virtual void mouseDoubleClickEvent(QMouseEvent* cEvent);
    virtual void mouseMoveEvent(QMouseEvent* cEvent);
    virtual void mouseReleaseEvent(QMouseEvent* cEvent);
    virtual void wheelEvent(QWheelEvent * cEvent);
    virtual void updateStatusBar();
    virtual QString createStatusBarText();

    virtual void startDrag();
    virtual void zoom(int prevX, int prevY, int currX, int currY);
    virtual void select(int currX, int currY);
    virtual void addSelect(int currX, int currY);


    enum {
        nothing = 0,
        panning,
        zooming,
        selection,
        addselection
    } _currMode;

    QSvgWidget* _drawingView;
    QScrollArea* _scroll;

 
    int _currX;
    int _currY;
    int dragStartWCx;
    int dragStartWCy;
    float aspectRatio; /**< This is the ratio of width and height of the SVG. */

    // Action groups
    QActionGroup* _pShowColActGrp;

    // Actions
    QAction* _pFitAct;
    QAction* _pOneToOneAct;
    QAction* _pShowOrigAct;
    QAction* _pShowBrightAct;

    // Menus
    QMenu* _pContextMenu;

    // Toolbars
    QToolBar* _pStdToolBar;

    // Slider for brightness adjustment
    QSlider* _pSliderBrightAdj;
    int _sliderBrightAdjVal;

    // Flag for status bar enablement
    bool _statusBarEnabled;
    bool _mouseEventsEnabled;

};

} // namespace DrawingViewGui

#endif // DrawingView_H
