/***************************************************************************
 *                                                                         *
 *   This is a view displaying an Drawing or portion of an Drawing in a box.   *
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

#include "DrawingView.h"

using namespace DrawingGui;


/* TRANSLATOR DrawingGui::DrawingView */

DrawingView::DrawingView(QWidget* parent)
  : MDIView(0, parent)
{
  // enable mouse tracking when moving even if no buttons are pressed
  setMouseTracking(true);

  // enable the mouse events
  _mouseEventsEnabled = true; 

  // Create the default status bar for displaying messages
  //EnableStatusBar(true);

  // Create an OpenGL widget for displaying Drawings
  //_pGLDrawingBox = new GLDrawingBox(this);

  //setCentralWidget(_pGLDrawingBox);

  _currMode = nothing;
  _currX = 0;
  _currY = 0;

  // Create the actions, menus and toolbars
  createActions();

  // connect other slots
  //connect(_pGLDrawingBox, SIGNAL(drawGraphics()), this, SLOT(drawGraphics()));
}

DrawingView::~DrawingView()
{
  // No need to delete _pGLDrawingBox or other widgets as this gets done automatically by QT
}

// Create the action groups, actions, menus and toolbars
void DrawingView::createActions()
{
 
}







// Slot function to fit (stretch/shrink) the Drawing to the view size
void DrawingView::fitDrawing()
{
}


// Slot function to display the Drawing at a 1:1 scale"
void DrawingView::oneToOneDrawing()
{
}

// Slot function to handle the color actions
void DrawingView::handleColorAct( QAction* act)
{
  if (act == _pShowOrigAct)
  {
    _pSliderBrightAdj->setEnabled(false);
    showOriginalColors();
  }
  else if (act == _pShowBrightAct)
  {
    _pSliderBrightAdj->setEnabled(true);
    showBrightened();
  }
}

// Show the original colors (no enhancement)
// but Drawing will be scaled for the number of significant bits
// (i.e if 12 significant bits (in 16-bit Drawing) a value of 4095 will be shown as white)
void DrawingView::showOriginalColors()
{
}

// Show the Drawing with a brightness adjustment
void DrawingView::showBrightened()
{
}

// Slot function to adjust the brightness slider's value
void DrawingView::sliderValueAdjusted(int NewValue)
{
    _sliderBrightAdjVal = NewValue;
    if (_pShowBrightAct->isChecked() == true)
        showBrightened();
}





// Mouse press event
void DrawingView::mousePressEvent(QMouseEvent* cEvent)
{
   if (_mouseEventsEnabled == true)
   {
      // Mouse event coordinates are relative to top-left of Drawing view (including toolbar!)
      // Get current cursor position relative to top-left of Drawing box
      QPoint offset;// = _pGLDrawingBox->pos();
      int box_x = cEvent->x() - offset.x();
      int box_y = cEvent->y() - offset.y();
      _currX = box_x;
      _currY = box_y;
      switch(cEvent->buttons())
      {
          case Qt::MidButton:
              _currMode = panning;
              startDrag();
              break;
          //case Qt::LeftButton | Qt::MidButton:
          //    _currMode = zooming;
          //    break;
          case Qt::LeftButton:
              if (cEvent->modifiers() & Qt::ShiftModifier)
                  _currMode = addselection;
              else
                _currMode = selection;
              break;
          case Qt::RightButton:
               _pContextMenu->exec(cEvent->globalPos());
              break;
          default:
              _currMode = nothing;
      }
   }
}

void DrawingView::mouseDoubleClickEvent(QMouseEvent* cEvent)
{
   if (_mouseEventsEnabled == true)
   {
       // Mouse event coordinates are relative to top-left of Drawing view (including toolbar!)
       // Get current cursor position relative to top-left of Drawing box
       QPoint offset;// = _pGLDrawingBox->pos();
       int box_x = cEvent->x() - offset.x();
       int box_y = cEvent->y() - offset.y();
       _currX = box_x;
       _currY = box_y;
       if(cEvent->button() == Qt::MidButton)
       {
//           double icX = _pGLDrawingBox->WCToIC_X(_currX);
  //         double icY = _pGLDrawingBox->WCToIC_Y(_currY);
           //int pixX = (int)floor(icX + 0.5);
           //int pixY = (int)floor(icY + 0.5);
//           _pGLDrawingBox->setZoomFactor(_pGLDrawingBox->getZoomFactor(), true, (int)floor(icX + 0.5), (int)floor(icY + 0.5));
//           _pGLDrawingBox->redraw();
       }
   }
}

// Mouse move event
void DrawingView::mouseMoveEvent(QMouseEvent* cEvent)
{
    QApplication::flush();

   // Mouse event coordinates are relative to top-left of Drawing view (including toolbar!)
   // Get current cursor position relative to top-left of Drawing box
   QPoint offset ;//= _pGLDrawingBox->pos();
   int box_x = cEvent->x() - offset.x();
   int box_y = cEvent->y() - offset.y();
   if (_mouseEventsEnabled == true)
   {
       switch(_currMode)
       {
           case nothing:
               break;
           case panning:
               //_pGLDrawingBox->relMoveWC(box_x - dragStartWCx, box_y - dragStartWCy);
               break;
           case zooming:
               zoom(_currX, _currY, box_x, box_y);
               break;
           default:
               break;
       }
   }
   _currX = box_x;
   _currY = box_y;

   // Update the status bar
   updateStatusBar();
}

// Mouse release event
void DrawingView::mouseReleaseEvent(QMouseEvent* cEvent)
{
   if (_mouseEventsEnabled == true)
   {
       // Mouse event coordinates are relative to top-left of Drawing view (including toolbar!)
       // Get current cursor position relative to top-left of Drawing box
       QPoint offset;// = _pGLDrawingBox->pos();
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
}

// Mouse wheel event
void DrawingView::wheelEvent(QWheelEvent * cEvent)
{
   if (_mouseEventsEnabled == true)
   {
       // Mouse event coordinates are relative to top-left of Drawing view (including toolbar!)
       // Get current cursor position relative to top-left of Drawing box
       QPoint offset;// = _pGLDrawingBox->pos();
       int box_x = cEvent->x() - offset.x();
       int box_y = cEvent->y() - offset.y();

       // Zoom around centrally displayed Drawing point
       int numTicks = cEvent->delta() / 120;
       int ICx, ICy;
       //_pGLDrawingBox->getCentrePoint(ICx, ICy);
       //_pGLDrawingBox->setZoomFactor(_pGLDrawingBox->getZoomFactor() / pow(2.0, (double)numTicks), true, ICx, ICy);
       //_pGLDrawingBox->redraw();
       _currX = box_x;
       _currY = box_y;

       // Update the status bar
       updateStatusBar();
   }
}

// Update the status bar with the Drawing parameters for the current mouse position
void DrawingView::updateStatusBar()
{
    if (_statusBarEnabled == true)
	{
        // Create the text string to display in the status bar
        QString txt = createStatusBarText();

        // Update status bar with new text
        statusBar()->showMessage(txt);
	}
}

// Create the text to display in the status bar.
// Gets called by updateStatusBar()
// Override this function in a derived class to add your own text
QString DrawingView::createStatusBarText()
{
    QString txt;
/*    // Get some Drawing parameters
    //unsigned short numDrawingSamples = _pGLDrawingBox->getDrawingNumSamplesPerPix();
    double zoomFactor = _pGLDrawingBox->getZoomFactor();
    double icX = _pGLDrawingBox->WCToIC_X(_currX);
    double icY = _pGLDrawingBox->WCToIC_Y(_currY);
    int pixX = (int)floor(icX + 0.5);
    int pixY = (int)floor(icY + 0.5);
    int colorFormat = _pGLDrawingBox->getDrawingFormat();

   // Create text for status bar
    if ((colorFormat == IB_CF_GREY8) || 
        (colorFormat == IB_CF_GREY16) || 
        (colorFormat == IB_CF_GREY32))
    {
        double grey_value;
        if (_pGLDrawingBox->getDrawingSample(pixX, pixY, 0, grey_value) == 0)
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  %s = %d  |  %s = %0.1lf", 
                        icX, icY, (const char*)tr("grey").toAscii(), (int)grey_value, (const char*)tr("zoom").toAscii(), zoomFactor);
        else
            txt.sprintf("x,y = %s  |  %s = %0.1lf", (const char*)tr("outside Drawing").toAscii(), (const char*)tr("zoom").toAscii(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_RGB24) || 
             (colorFormat == IB_CF_RGB48))
    {
        double red, green, blue;
        if ((_pGLDrawingBox->getDrawingSample(pixX, pixY, 0, red) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 1, green) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 2, blue) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", (const char*)tr("outside Drawing").toAscii(), (const char*)tr("zoom").toAscii(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgb = %d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, (const char*)tr("zoom").toAscii(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_BGR24) || 
             (colorFormat == IB_CF_BGR48))
    {
        double red, green, blue;
        if ((_pGLDrawingBox->getDrawingSample(pixX, pixY, 0, blue) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 1, green) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 2, red) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", (const char*)tr("outside Drawing").toAscii(), (const char*)tr("zoom").toAscii(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgb = %d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, (const char*)tr("zoom").toAscii(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_RGBA32) || 
             (colorFormat == IB_CF_RGBA64))
    {
        double red, green, blue, alpha;
        if ((_pGLDrawingBox->getDrawingSample(pixX, pixY, 0, red) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 1, green) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 2, blue) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 3, alpha) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", (const char*)tr("outside Drawing").toAscii(), (const char*)tr("zoom").toAscii(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgba = %d,%d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, (int)alpha, (const char*)tr("zoom").toAscii(), zoomFactor);
    }
    else if ((colorFormat == IB_CF_BGRA32) || 
             (colorFormat == IB_CF_BGRA64))
    {
        double red, green, blue, alpha;
        if ((_pGLDrawingBox->getDrawingSample(pixX, pixY, 0, blue) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 1, green) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 2, red) != 0) ||
            (_pGLDrawingBox->getDrawingSample(pixX, pixY, 3, alpha) != 0))
            txt.sprintf("x,y = %s  |  %s = %0.1lf", (const char*)tr("outside Drawing").toAscii(), (const char*)tr("zoom").toAscii(), zoomFactor);
        else
            txt.sprintf("x,y = %0.2lf,%0.2lf  |  rgba = %d,%d,%d,%d  |  %s = %0.1lf", 
                        icX, icY, (int)red, (int)green, (int)blue, (int)alpha, (const char*)tr("zoom").toAscii(), zoomFactor);
    }
*/
    return txt;
}

// Starts a mouse drag in the Drawing - stores some initial positions
void DrawingView::startDrag()
{
    //_pGLDrawingBox->fixBasePosCurr(); // fixes current Drawing position as base position
    dragStartWCx = _currX;
    dragStartWCy = _currY;
}

// Zoom the Drawing using vertical mouse movement to define a zoom factor
void DrawingView::zoom(int prevX, int prevY, int currX, int currY)
{
    // Check we have more of a vertical shift than a hz one
    int dx = currX - prevX;
    int dy = currY - prevY;
    if (abs(dy) > abs(dx))
    {
        // Get centrally displayed Drawing point
        int ICx, ICy;
        //_pGLDrawingBox->getCentrePoint(ICx, ICy);

        // Compute zoom factor multiplier
        double zoomFactorMultiplier = 1.05;
        if (currY > prevY)
            zoomFactorMultiplier = 0.95;

        // Zoom around centrally displayed Drawing point
        //_pGLDrawingBox->setZoomFactor(_pGLDrawingBox->getZoomFactor() * zoomFactorMultiplier, true, ICx, ICy);
        //_pGLDrawingBox->redraw();
    }
}

// Select at the given position
void DrawingView::select(int currX, int currY)
{
    // base class implementation does nothing
    // override this method and implement selection capability if required
}

// Add selection at the given position
void DrawingView::addSelect(int currX, int currY)
{
    // base class implementation does nothing
    // override this method and implement selection capability if required
}

// Draw any 2D graphics necessary
// Use GLDrawingBox::ICToWC_X and ICToWC_Y methods to transform Drawing coordinates into widget coordinates (which 
// must be used by the OpenGL vertex commands).
void DrawingView::drawGraphics()
{
    // base class implementation does nothing

    // override this method and implement OpenGL drawing commands to draw any needed graphics on top of the Drawing

    /* Example: draw a red line from Drawing coordinates (100,100) to (120,120)
    glColor3ub((GLubyte)255, (GLubyte)0, (GLubyte)0);
    glBegin(GL_LINES);
    glVertex2d(_pGLDrawingBox->ICToWC_X(100.0), _pGLDrawingBox->ICToWC_Y(100.0));
    glVertex2d(_pGLDrawingBox->ICToWC_X(120.0), _pGLDrawingBox->ICToWC_Y(120.0));
    glEnd();
    */
}

#include "moc_DrawingView.cpp"


