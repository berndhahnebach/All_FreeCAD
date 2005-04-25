/***************************************************************************
 *                                                                         *
 *   This is a view displaying an image or portion of an image in a box.   *
 *                                                                         *
 *   Author:    Graeme van der Vlugt                                       *
 *   Copyright: Imetric 3D GmbH                                            *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#ifndef ImageView_H
#define ImageView_H

#include <Gui/View.h>
#include "GLImageBox.h"

class QSlider;
class QAction;
class QPopupMenu;
class QToolBar;

namespace ImageGui
{

class ImageGuiExport ImageView : public Gui::MDIView
{
    Q_OBJECT

public:
    ImageView(QWidget* parent, const char* name);
    virtual ~ImageView();

    const char *getName(void){return "ImageView";}
    void onUpdate(void){};

    virtual void clearImage();
    virtual int createImageCopy(void* pSrcPixelData, unsigned long width, unsigned long height, int format);
    virtual int pointImageTo(void* pSrcPixelData, unsigned long width, unsigned long height, int format, bool takeOwnership);

protected slots:
    virtual void fitImage();
    virtual void oneToOneImage();
    virtual void handleColorAct( QAction* act);
    virtual void sliderValueAdjusted(int NewValue);
    virtual void drawGraphics();

protected:
    virtual void createActions();
    virtual void showOriginalColors();
    virtual void showBrightened();
    virtual int createIntensityMap(int numEntriesReq = 0, bool Initialise = true);
    virtual int getNumIntensityMapEntries() const;
    virtual int setIntensityMapValue(int index, float value);
    virtual void mousePressEvent(QMouseEvent* cEvent);
    virtual void mouseDoubleClickEvent(QMouseEvent* cEvent);
    virtual void mouseMoveEvent(QMouseEvent* cEvent);
    virtual void mouseReleaseEvent(QMouseEvent* cEvent);
    virtual void wheelEvent(QWheelEvent * cEvent);
    virtual void updateStatusBar();

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

    GLImageBox* _pGLImageBox;

    int _currX;
    int _currY;
    int dragStartWCx;
    int dragStartWCy;

    // Action groups
    QActionGroup* _pShowColActGrp;

    // Actions
    QAction* _pFitAct;
    QAction* _pOneToOneAct;
    QAction* _pShowOrigAct;
    QAction* _pShowBrightAct;

    // Menus
    QPopupMenu* _pContextMenu;

    // Toolbars
    QToolBar* _pStdToolBar;

    // Slider for brightness adjustment
    QSlider* _pSliderBrightAdj;
    int _sliderBrightAdjVal;

};

} // namespace ImageViewGui

#endif // ImageView_H
