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
# include <qstatusbar.h>
#endif

#include "ImageView.h"
#include "GLImageBox.h"
#include "../App/ImageBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace ImageGui;

ImageView::ImageView(QWidget* parent, const char* name)
    : MDIView(0, parent, name, WDestructiveClose)
{
    // enable mouse tracking when moving even if no buttons are pressed
    setMouseTracking(true);

    // Create an OpenGL widget for displaying images
    _pGLImageBox = new GLImageBox(this, "ImageView glimagebox");
    setCentralWidget(_pGLImageBox);

    // Create the default status bar for displaying messages
    statusBar();

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

// Create the actions, menus and toolbars
void ImageView::createActions()
{
    // Create actions
    _pFitAct = new QAction(tr("&Fit image"), tr("Ctrl+F"), this);
    _pFitAct->setStatusTip(tr("Stretch the image to fit the view"));
    connect(_pFitAct, SIGNAL(activated()), this, SLOT(fitImage()));

    _pOneToOneAct = new QAction(tr("&1:1 scale"), tr("Ctrl+1"), this);
    _pOneToOneAct->setStatusTip(tr("Display the image at a 1:1 scale"));
    connect(_pOneToOneAct, SIGNAL(activated()), this, SLOT(oneToOneImage()));

    _pShowOrigAct = new QAction(tr("&Original color"), tr("Ctrl+O"), this);
    _pShowOrigAct->setStatusTip(tr("Display the image with its original color(s)"));
    connect(_pShowOrigAct, SIGNAL(activated()), this, SLOT(showOriginalColors()));

    _pShowBrightAct = new QAction(tr("&Brightened color"), tr("Ctrl+B"), this);
    _pShowBrightAct->setStatusTip(tr("Display the image with brightened color(s)"));
    connect(_pShowBrightAct, SIGNAL(activated()), this, SLOT(showBrightened()));

    // Create the menus and add the actions
    _pContextMenu = new QPopupMenu(this);
    _pFitAct->addTo(_pContextMenu);
    _pOneToOneAct->addTo(_pContextMenu);
    _pShowOrigAct->addTo(_pContextMenu);
    _pShowBrightAct->addTo(_pContextMenu);
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
}

// Show the original colors (no enhancement)
void ImageView::showOriginalColors()
{
    _pGLImageBox->clearIntensityMap();
}

// Show the image with a preset brightness enhancement
// Useful for looking at very dark images
void ImageView::showBrightened()
{
    if (createIntensityMap() == 0)
    {
        // Fill the intensity map with the preset enhancement
        int numMapEntries = getNumIntensityMapEntries();
        float out;
        float lower = 50.0 * (float)numMapEntries / 256.0;
        float upper = 170.0 * (float)numMapEntries / 256.0;
        float above_lower = 205.0 * (float)numMapEntries / 256.0;
        float above_upper = 85.0 * (float)numMapEntries / 256.0;
        for (int in = 0; in < numMapEntries; in++)
        {
            if ((float)in < lower)
                out = ((float)in * upper) / lower;
            else
                out = upper + (((float)in - lower) * above_upper) / above_lower;
            setIntensityMapValue(in, out / (float)(numMapEntries - 1));
        }
    }
}

// Create a linear intensity map
// returns 0 for OK, -1 for memory allocation error
// numRequestedEntries ... requested number of map entries (used if not greater than system maximum or 
//                         if not greater than the maximum number of intensity values in the current image).
//                         Pass zero to use the maximum possible. Always check the actual number of entries
//                         created using getNumIntensityMapEntries() after a call to this method.
int ImageView::createIntensityMap(int numEntriesReq)
{
    return (_pGLImageBox->createIntensityMap());
}

// Gets the number of entries in the intensity map
int ImageView::getNumIntensityMapEntries() const
{
    return (_pGLImageBox->getNumIntensityMapEntries());
}

// Sets an intensity map value
// index ... index of intensity map entry
// value ... intensity value for this entry (range 0 to 1)
int ImageView::setIntensityMapValue(int index, float value)
{
    return (_pGLImageBox->setIntensityMapValue(index, value));
}


// Clears the image data
void ImageView::clearImage()
{
    _pGLImageBox->clearImage();
}

// Load image by copying the pixel data
// The image object inside this view object will take ownership of the copied pixel data
// (the source image is still controlled by the caller)
// Returns:
//		 0 for OK
//		-1 for invalid color format
//		-2 for memory allocation error
int ImageView::createImageCopy(void* pSrcPixelData, unsigned long width, unsigned long height, int format)
{
    return (_pGLImageBox->createImageCopy(pSrcPixelData, width, height, format));
}

// Make the image object inside this view object point to another image source
// If takeOwnership is false then:
//      This object will not own (control) or copy the pixel data
//      (the source image is still controlled by the caller)
// Else if takeOwnership is true then:
//      This object will take ownership (control) of the pixel data
//      (the source image is not (should not be) controlled by the caller anymore)
//      In this case the memory must have been allocated with the new operator (because this class will use the delete operator)
// Returns:
//		 0 for OK
//		-1 for invalid color format
int ImageView::pointImageTo(void* pSrcPixelData, unsigned long width, unsigned long height, int format, bool takeOwnership)
{
    return (_pGLImageBox->pointImageTo(pSrcPixelData, width, height, format, takeOwnership));
}

// Mouse press event
void ImageView::mousePressEvent(QMouseEvent* cEvent)
{
    _currX = cEvent->x();
    _currY = cEvent->y();
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
    _currX = cEvent->x();
    _currY = cEvent->y();
    if(cEvent->button() == Qt::MidButton)
    {
        double icX = _pGLImageBox->WCToIC_X(_currX);
        double icY = _pGLImageBox->WCToIC_Y(_currY);
//        int pixX = (int)floor(icX + 0.5);
//        int pixY = (int)floor(icY + 0.5);
        _pGLImageBox->setZoomFactor(_pGLImageBox->getZoomFactor(), true, (int)floor(icX + 0.5), (int)floor(icY + 0.5));
    }
}

// Mouse move event
void ImageView::mouseMoveEvent(QMouseEvent* cEvent)
{
    QApplication::flushX();

    switch(_currMode)
    {
        case nothing:
            break;
        case panning:
            _pGLImageBox->relMoveWC(cEvent->x() - dragStartWCx, cEvent->y() - dragStartWCy);
            break;
        case zooming:
            zoom(_currX, _currY, cEvent->x(), cEvent->y());
            break;
        default:
            break;
    }

    _currX = cEvent->x();
    _currY = cEvent->y();


    // Update the status bar
    updateStatusBar();
}

// Mouse release event
void ImageView::mouseReleaseEvent(QMouseEvent* cEvent)
{
    switch(_currMode)
    {
        case selection:
            select(cEvent->x(), cEvent->y());
            break;
        case addselection:
            addSelect(cEvent->x(), cEvent->y());
            break;
        default:
            break;
    }
    _currMode = nothing;
}

// Mouse wheel event
void ImageView::wheelEvent(QWheelEvent * cEvent)
{
    // Zoom around centrally displayed image point
    int numTicks = cEvent->delta() / 120;
    int ICx, ICy;
    _pGLImageBox->getCentrePoint(ICx, ICy);
    _pGLImageBox->setZoomFactor(_pGLImageBox->getZoomFactor() * pow(2.0, (double)numTicks), true, ICx, ICy);

    _currX = cEvent->x();
    _currY = cEvent->y();

    // Update the status bar
    updateStatusBar();
}

// Update the status bar with the image parameters for the current mouse position
void ImageView::updateStatusBar()
{
    // Get some image parameters
//    unsigned short numImageSamples = _pGLImageBox->getImageNumSamplesPerPix();
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

   // Update status bar with new text
   statusBar()->message(txt);
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


