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

#include "PreCompiled.h"

#ifndef _PreComp_
# include <math.h>
# include <qaction.h>
# include <qapplication.h>
# include <qpopupmenu.h>
# include <qslider.h>
# include <qstatusbar.h>
#endif

#include "ImageView.h"
#include "GLImageBox.h"
#include "../App/ImageBase.h"
#include "XpmImages.h"

using namespace ImageGui;


ImageView::ImageView(QWidget* parent, const char* name)
    : MDIView(0, parent, name, WDestructiveClose)
{
    // enable mouse tracking when moving even if no buttons are pressed
    setMouseTracking(true);

    // Create an OpenGL widget for displaying images
    _pGLImageBox = new GLImageBox(this, "ImageView glimagebox");
    setCentralWidget(_pGLImageBox);

    // Create the default status bar for displaying messages and disable the gripper
    statusBar()->setSizeGripEnabled( false );

    _currMode = nothing;
    _currX = 0;
    _currY = 0;

    // Create the actions, menus and toolbars
    createActions();

    // connect other slots
    connect(_pGLImageBox, SIGNAL(drawGraphics()), this, SLOT(drawGraphics()));
}

ImageView::~ImageView()
{
    // No need to delete _pGLImageBox or other widgets as this gets done automatically by QT
}

// Create the action groups, actions, menus and toolbars
void ImageView::createActions()
{
    // Create actions
    _pFitAct = new QAction(this);
    _pFitAct->setMenuText(tr("&Fit image"));
    _pFitAct->setIconSet(QPixmap(image_stretch));
    _pFitAct->setStatusTip(tr("Stretch the image to fit the view"));
    connect(_pFitAct, SIGNAL(activated()), this, SLOT(fitImage()));

    _pOneToOneAct = new QAction(this);
    _pOneToOneAct->setMenuText(tr("&1:1 scale"));
    _pOneToOneAct->setIconSet(QPixmap(image_oneToOne));
    _pOneToOneAct->setStatusTip(tr("Display the image at a 1:1 scale"));
    connect(_pOneToOneAct, SIGNAL(activated()), this, SLOT(oneToOneImage()));

    // Create an action group for the exclusive color actions
    _pShowColActGrp = new QActionGroup (this);
    _pShowColActGrp->setExclusive(true);
    connect(_pShowColActGrp, SIGNAL(selected(QAction*)), this, SLOT(handleColorAct(QAction*)));

    _pShowOrigAct = new QAction(_pShowColActGrp);
    _pShowOrigAct->setToggleAction(true);
    _pShowOrigAct->setMenuText(tr("&Original color"));
    _pShowOrigAct->setIconSet(QPixmap(image_orig));
    _pShowOrigAct->setStatusTip(tr("Display the image with its original color(s)"));

    _pShowBrightAct = new QAction(_pShowColActGrp);
    _pShowBrightAct->setToggleAction(true);
    _pShowBrightAct->setMenuText(tr("&Brightened color"));
    _pShowBrightAct->setIconSet(QPixmap(image_bright));
    _pShowBrightAct->setStatusTip(tr("Display the image with brightened color(s)"));

    // Create the menus and add the actions
    _pContextMenu = new QPopupMenu(this);
    _pFitAct->addTo(_pContextMenu);
    _pOneToOneAct->addTo(_pContextMenu);
    _pShowOrigAct->addTo(_pContextMenu);
    _pShowBrightAct->addTo(_pContextMenu);

    // Create the toolbars and add the actions
    _pStdToolBar = new QToolBar(tr("Standard"), this);
    _pFitAct->addTo(_pStdToolBar);
    _pOneToOneAct->addTo(_pStdToolBar);
    _pShowOrigAct->addTo(_pStdToolBar);
    _pShowBrightAct->addTo(_pStdToolBar);

    // Add a slider to the toolbar (for brightness adjustment)
    _sliderBrightAdjVal = 10;
    _pSliderBrightAdj = new QSlider(0, 100, 10, _sliderBrightAdjVal, Qt::Horizontal, _pStdToolBar);
    connect(_pSliderBrightAdj, SIGNAL(valueChanged(int)), this, SLOT(sliderValueAdjusted(int)));
    _pSliderBrightAdj->hide();

    // Set the original color action to ON
    _pShowOrigAct->setOn(true);
}

// Enable or disable the color actions (_pShowOrigAct and _pShowBrightAct)
// By default they are enabled
// If enabling:
//      image is redisplayed in original colours (i.e color map cleared)
// If disabling
//      color map is left as is and image is not redisplayed
// This function should be used to disable the color actions when a derived class implements its own color map scheme
void ImageView::EnableColorActions(bool Enable)
{
    if (Enable == true)
    {
        _pShowOrigAct->setEnabled(true);
        _pShowBrightAct->setEnabled(true);
        if (_pShowBrightAct->isOn() == true)
            showBrightened();
        else
            showOriginalColors();
    }
    else
    {
        _pShowOrigAct->setEnabled(false);
        _pShowBrightAct->setEnabled(false);
        _pSliderBrightAdj->hide();
    }
}

// Slot function to fit (stretch/shrink) the image to the view size
void ImageView::fitImage()
{
    _pGLImageBox->stretchToFit();
}


// Slot function to display the image at a 1:1 scale"
void ImageView::oneToOneImage()
{
    _pGLImageBox->setNormal();
    _pGLImageBox->redraw();
}

// Slot function to handle the color actions
void ImageView::handleColorAct( QAction* act)
{
    if (act == _pShowOrigAct)
    {
        _pSliderBrightAdj->hide();
	    showOriginalColors();
    }
    else if (act == _pShowBrightAct)
    {
        _pSliderBrightAdj->show();
        showBrightened();
    }
}

// Show the original colors (no enhancement)
// but image will be scaled for the number of significant bits
// (i.e if 12 significant bits (in 16-bit image) a value of 4095 will be shown as white)
void ImageView::showOriginalColors()
{
    _pGLImageBox->clearColorMap();
    _pGLImageBox->redraw();
}

// Show the image with a brightness adjustment
void ImageView::showBrightened()
{
    if (createColorMap(0, false) == 0)
    {
        // Fill the color map with the preset enhancement
        int numMapEntries = getNumColorMapEntries();
        double expValue = (_sliderBrightAdjVal / 1000.0) * 256 / numMapEntries;
        for (int in = 0; in < numMapEntries; in++)
        {
            double out = 1.0 - exp (-(double)in * expValue);
            setColorMapRedValue(in, (float)out);
            setColorMapGreenValue(in, (float)out);
            setColorMapBlueValue(in, (float)out);
            setColorMapAlphaValue(in, 1.0);
        }

        // redraw
        _pGLImageBox->redraw();
    }
}

// Slot function to adjust the brightness slider's value
void ImageView::sliderValueAdjusted(int NewValue)
{
    _sliderBrightAdjVal = NewValue;
    if (_pShowBrightAct->isOn() == true)
        showBrightened();
}

// Create a color map
// (All red entries come first, then green, then blue, then alpha)
// returns 0 for OK, -1 for memory allocation error
// numRequestedEntries ... requested number of map entries (used if not greater than system maximum or 
//                         if not greater than the maximum number of intensity values in the current image).
//                         Pass zero to use the maximum possible. Always check the actual number of entries
//                         created using getNumColorMapEntries() after a call to this method.
// Initialise ... flag to initialise the map to a linear scale or not
int ImageView::createColorMap(int numEntriesReq, bool Initialise)
{
    return (_pGLImageBox->createColorMap(numEntriesReq, Initialise));
}

// Gets the number of entries in the color map (number of entries for each color)
int ImageView::getNumColorMapEntries() const
{
    return (_pGLImageBox->getNumColorMapEntries());
}

// Clears the color map
void ImageView::clearColorMap()
{
    _pGLImageBox->clearColorMap();
}

// Sets a color map RGBA value
// (All red entries come first, then green, then blue, then alpha)
// index ... index of color map RGBA entry
// red ... intensity value for this red entry (range 0 to 1)
// green ... intensity value for this green entry (range 0 to 1)
// blue ... intensity value for this blue entry (range 0 to 1)
// alpha ... intensity value for this alpha entry (range 0 to 1)
int ImageView::setColorMapRGBAValue(int index, float red, float green, float blue, float alpha)
{
    return (_pGLImageBox->setColorMapRGBAValue(index, red, green, blue, alpha));
}

// Sets a color map red value
// (All red entries come first, then green, then blue, then alpha)
// index ... index of color map red entry
// value ... intensity value for this red entry (range 0 to 1)
int ImageView::setColorMapRedValue(int index, float value)
{
    return (_pGLImageBox->setColorMapRedValue(index, value));
}

// Sets a color map green value
// (All red entries come first, then green, then blue, then alpha)
// index ... index of color map green entry
// value ... intensity value for this green entry (range 0 to 1)
int ImageView::setColorMapGreenValue(int index, float value)
{
    return (_pGLImageBox->setColorMapGreenValue(index, value));
}

// Sets a color map blue value
// (All red entries come first, then green, then blue, then alpha)
// index ... index of color map blue entry
// value ... intensity value for this blue entry (range 0 to 1)
int ImageView::setColorMapBlueValue(int index, float value)
{
    return (_pGLImageBox->setColorMapBlueValue(index, value));
}

// Sets a color map alpha value
// (All red entries come first, then green, then blue, then alpha)
// index ... index of color map alpha entry
// value ... intensity value for this alpha entry (range 0 to 1)
int ImageView::setColorMapAlphaValue(int index, float value)
{
    return (_pGLImageBox->setColorMapAlphaValue(index, value));
}

// Clears the image data
void ImageView::clearImage()
{
    _pGLImageBox->clearImage();
    _pGLImageBox->redraw(); // clears view
}

// Load image by copying the pixel data
// The image object inside this view object will take ownership of the copied pixel data
// (the source image is still controlled by the caller)
// If numSigBitsPerSample = 0 then the full range is assumed to be significant
// Returns:
//		 0 for OK
//		-1 for invalid color format
//		-2 for memory allocation error
int ImageView::createImageCopy(void* pSrcPixelData, unsigned long width, unsigned long height, int format, unsigned short numSigBitsPerSample, bool reset)
{
    int ret = _pGLImageBox->createImageCopy(pSrcPixelData, width, height, format, numSigBitsPerSample, reset);
    if (_pShowBrightAct->isOn() == true)
        showBrightened();
    else
        showOriginalColors();
    return ret;
}

// Make the image object inside this view object point to another image source
// If takeOwnership is false then:
//      This object will not own (control) or copy the pixel data
//      (the source image is still controlled by the caller)
// Else if takeOwnership is true then:
//      This object will take ownership (control) of the pixel data
//      (the source image is not (should not be) controlled by the caller anymore)
//      In this case the memory must have been allocated with the new operator (because this class will use the delete operator)
// If numSigBitsPerSample = 0 then the full range is assumed to be significant
// Returns:
//		 0 for OK
//		-1 for invalid color format
int ImageView::pointImageTo(void* pSrcPixelData, unsigned long width, unsigned long height, int format, unsigned short numSigBitsPerSample, bool takeOwnership, bool reset)
{
    int ret = _pGLImageBox->pointImageTo(pSrcPixelData, width, height, format, numSigBitsPerSample, takeOwnership, reset);
    if (_pShowBrightAct->isOn() == true)
        showBrightened();
    else
        showOriginalColors();
    return ret;
}

// Mouse press event
void ImageView::mousePressEvent(QMouseEvent* cEvent)
{
    // Mouse event coordinates are relative to top-left of image view (including toolbar!)

    // Get current cursor position relative to top-left of image box

    QPoint offset = _pGLImageBox->pos();

    int box_x = cEvent->x() - offset.x();

    int box_y = cEvent->y() - offset.y();



    _currX = box_x;
    _currY = box_y;
    switch(cEvent->stateAfter())
    {
        case Qt::MidButton:
            _currMode = panning;
            startDrag();
            break;
        case Qt::LeftButton | Qt::MidButton:
            _currMode = zooming;
            break;
        case Qt::LeftButton:
            _currMode = selection;
            break;
        case Qt::LeftButton | Qt::ShiftButton:
            _currMode = addselection;
            break;
        case Qt::RightButton:
             _pContextMenu->exec(cEvent->globalPos());
            break;
        default:
            _currMode = nothing;
    }
}

void ImageView::mouseDoubleClickEvent(QMouseEvent* cEvent)
{
    // Mouse event coordinates are relative to top-left of image view (including toolbar!)

    // Get current cursor position relative to top-left of image box

    QPoint offset = _pGLImageBox->pos();

    int box_x = cEvent->x() - offset.x();

    int box_y = cEvent->y() - offset.y();



    _currX = box_x;
    _currY = box_y;
    if(cEvent->button() == Qt::MidButton)
    {
        double icX = _pGLImageBox->WCToIC_X(_currX);
        double icY = _pGLImageBox->WCToIC_Y(_currY);
        //int pixX = (int)floor(icX + 0.5);
        //int pixY = (int)floor(icY + 0.5);
        _pGLImageBox->setZoomFactor(_pGLImageBox->getZoomFactor(), true, (int)floor(icX + 0.5), (int)floor(icY + 0.5));
        _pGLImageBox->redraw();
    }
}

// Mouse move event
void ImageView::mouseMoveEvent(QMouseEvent* cEvent)
{
    QApplication::flushX();


    // Mouse event coordinates are relative to top-left of image view (including toolbar!)

    // Get current cursor position relative to top-left of image box

    QPoint offset = _pGLImageBox->pos();

    int box_x = cEvent->x() - offset.x();

    int box_y = cEvent->y() - offset.y();


    switch(_currMode)
    {
        case nothing:
            break;
        case panning:
            _pGLImageBox->relMoveWC(box_x - dragStartWCx, box_y - dragStartWCy);
            break;
        case zooming:
            zoom(_currX, _currY, box_x, box_y);
            break;
        default:
            break;
    }

    _currX = box_x;
    _currY = box_y;


    // Update the status bar
    updateStatusBar();
}

// Mouse release event
void ImageView::mouseReleaseEvent(QMouseEvent* cEvent)
{
    // Mouse event coordinates are relative to top-left of image view (including toolbar!)

    // Get current cursor position relative to top-left of image box

    QPoint offset = _pGLImageBox->pos();

    int box_x = cEvent->x() - offset.x();

    int box_y = cEvent->y() - offset.y();



    switch(_currMode)
    {
        case selection:
            select(box_x, box_y);
            break;
        case addselection:
            addSelect(box_x, box_y);
            break;
        default:
            break;
    }
    _currMode = nothing;
}

// Mouse wheel event
void ImageView::wheelEvent(QWheelEvent * cEvent)
{
    // Mouse event coordinates are relative to top-left of image view (including toolbar!)

    // Get current cursor position relative to top-left of image box

    QPoint offset = _pGLImageBox->pos();

    int box_x = cEvent->x() - offset.x();

    int box_y = cEvent->y() - offset.y();



    // Zoom around centrally displayed image point
    int numTicks = cEvent->delta() / 120;
    int ICx, ICy;
    _pGLImageBox->getCentrePoint(ICx, ICy);
    _pGLImageBox->setZoomFactor(_pGLImageBox->getZoomFactor() * pow(2.0, (double)numTicks), true, ICx, ICy);
    _pGLImageBox->redraw();

    _currX = box_x;
    _currY = box_y;

    // Update the status bar
    updateStatusBar();
}

// Update the status bar with the image parameters for the current mouse position
void ImageView::updateStatusBar()
{
    // Create the text string to display in the status bar
    QString txt = createStatusBarText();

   // Update status bar with new text
   statusBar()->message(txt);
}

// Create the text to display in the status bar.
// Gets called by updateStatusBar()
// Override this function in a derived class to add your own text
QString ImageView::createStatusBarText()
{
    // Get some image parameters
    //unsigned short numImageSamples = _pGLImageBox->getImageNumSamplesPerPix();
    double zoomFactor = _pGLImageBox->getZoomFactor();
    double icX = _pGLImageBox->WCToIC_X(_currX);
    double icY = _pGLImageBox->WCToIC_Y(_currY);
    int pixX = (int)floor(icX + 0.5);
    int pixY = (int)floor(icY + 0.5);
    int colorFormat = _pGLImageBox->getImageFormat();

   // Create text for status bar
    QString txt;
    if ((colorFormat == IB_CF_GREY8) || 
        (colorFormat == IB_CF_GREY16) || 
        (colorFormat == IB_CF_GREY32))
    {
        double grey_value;
        if (_pGLImageBox->getImageSample(pixX, pixY, 0, grey_value) == 0)
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  %s = %d  |  %s = %0.1lf", 
                        icX, icY, tr("grey").latin1(), (int)grey_value, tr("zoom").latin1(), zoomFactor);
        else
            txt.sprintf("x,y = %s  |  %s = %0.1lf", tr("outside image").latin1(), tr("zoom").latin1(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_RGB24) || 
             (colorFormat == IB_CF_RGB48))
    {
        double red, green, blue;
        if ((_pGLImageBox->getImageSample(pixX, pixY, 0, red) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 1, green) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 2, blue) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", tr("outside image").latin1(), tr("zoom").latin1(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgb = %d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, tr("zoom").latin1(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_BGR24) || 
             (colorFormat == IB_CF_BGR48))
    {
        double red, green, blue;
        if ((_pGLImageBox->getImageSample(pixX, pixY, 0, blue) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 1, green) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 2, red) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", tr("outside image").latin1(), tr("zoom").latin1(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgb = %d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, tr("zoom").latin1(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_RGBA32) || 
             (colorFormat == IB_CF_RGBA64))
    {
        double red, green, blue, alpha;
        if ((_pGLImageBox->getImageSample(pixX, pixY, 0, red) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 1, green) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 2, blue) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 3, alpha) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", tr("outside image").latin1(), tr("zoom").latin1(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgba = %d,%d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, (int)alpha, tr("zoom").latin1(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_BGRA32) || 
             (colorFormat == IB_CF_BGRA64))
    {
        double red, green, blue, alpha;
        if ((_pGLImageBox->getImageSample(pixX, pixY, 0, blue) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 1, green) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 2, red) != 0) ||
            (_pGLImageBox->getImageSample(pixX, pixY, 3, alpha) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", tr("outside image").latin1(), tr("zoom").latin1(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgba = %d,%d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, (int)alpha, tr("zoom").latin1(), zoomFactor);
    }

    return txt;
}

// Starts a mouse drag in the image - stores some initial positions
void ImageView::startDrag()
{
    _pGLImageBox->fixBasePosCurr(); // fixes current image position as base position
    dragStartWCx = _currX;
    dragStartWCy = _currY;
}

// Zoom the image using vertical mouse movement to define a zoom factor
void ImageView::zoom(int prevX, int prevY, int currX, int currY)
{
    // Check we have more of a vertical shift than a hz one
    int dx = currX - prevX;
    int dy = currY - prevY;
    if (fabs(dy) > fabs(dx))
    {
        // Get centrally displayed image point
        int ICx, ICy;
        _pGLImageBox->getCentrePoint(ICx, ICy);

        // Compute zoom factor multiplier
        double zoomFactorMultiplier = 1.05;
        if (currY > prevY)
            zoomFactorMultiplier = 0.95;

        // Zoom around centrally displayed image point
        _pGLImageBox->setZoomFactor(_pGLImageBox->getZoomFactor() * zoomFactorMultiplier, true, ICx, ICy);
        _pGLImageBox->redraw();
    }
}

// Select at the given position
void ImageView::select(int currX, int currY)
{
    // base class implementation does nothing
    // override this method and implement selection capability if required
}

// Add selection at the given position
void ImageView::addSelect(int currX, int currY)
{
    // base class implementation does nothing
    // override this method and implement selection capability if required
}

// Draw any 2D graphics necessary
// Use GLImageBox::ICToWC_X and ICToWC_Y methods to transform image coordinates into widget coordinates (which 
// must be used by the OpenGL vertex commands).
void ImageView::drawGraphics()
{
    // base class implementation does nothing

    // override this method and implement OpenGL drawing commands to draw any needed graphics on top of the image

    /* Example: draw a red line from image coordinates (100,100) to (120,120)
    glColor3ub((GLubyte)255, (GLubyte)0, (GLubyte)0);
    glBegin(GL_LINES);
    glVertex2d(_pGLImageBox->ICToWC_X(100.0), _pGLImageBox->ICToWC_Y(100.0));
    glVertex2d(_pGLImageBox->ICToWC_X(120.0), _pGLImageBox->ICToWC_Y(120.0));
    glEnd();
    */
}

#include "moc_ImageView.cpp"


