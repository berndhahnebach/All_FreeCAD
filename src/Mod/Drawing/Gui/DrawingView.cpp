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
#ifndef _PreComp_
# include <QFileInfo>
# include <strstream>
#endif

#include "DrawingView.h"
#include <Gui/FileDialog.h>
#include <Base/gzstream.h>

using namespace DrawingGui;

class DrawingScrollArea : public QScrollArea
{
public:
    DrawingScrollArea(QWidget* parent)
        : QScrollArea(parent)
    {
    }

    ~DrawingScrollArea()
    {
    }
protected:
    void wheelEvent(QWheelEvent* e)
    {
#if QT_VERSION >= 0x040200
        e->ignore();
#endif
    }
};

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
  _scroll = new DrawingScrollArea(this);
  _drawingView = new QSvgWidget(_scroll);
  _drawingView->setBackgroundRole(QPalette::Base);
  //this->_drawingView->setFocus();
  //scroll->setFocusProxy(_drawingView);
//  _drawingView->setFocusProxy(scroll);
  _scroll->setWidget(_drawingView);
  //_drawingView->resize(20, 20);
  setCentralWidget(_scroll);

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

bool DrawingView::onMsg(const char* pMsg, const char** ppReturn)
{
  if(strcmp("ViewFit",pMsg) == 0 ){
    fitDrawing();
    return true;
// comment out on older Inventor
  } /*else if(strcmp("Example1",pMsg) == 0 ){
    SoSeparator * root = new SoSeparator;
    Texture3D(root);
    _viewer->setSceneGraph(root);
    return true;
  }else if(strcmp("Example2",pMsg) == 0 ){
    SoSeparator * root = new SoSeparator;
    LightManip(root);
    _viewer->setSceneGraph(root);
    return true;
  }else if(strcmp("Example3",pMsg) == 0 ){
    SoSeparator * root = new SoSeparator;
    AnimationTexture(root);
    _viewer->setSceneGraph(root);
    return true;
  }else if(strcmp("GetCamera",pMsg) == 0 ){
    SoCamera * Cam = _viewer->getCamera();
    *ppReturn = writeNodesToString(Cam).c_str();
    return true;
  }else if(strncmp("SetCamera",pMsg,9) == 0 ){
    return setCamera(pMsg+10);
  }else if(strncmp("Dump",pMsg,4) == 0 ){
    dump(pMsg+5);
    return true;
  }else if(strcmp("ViewBottom",pMsg) == 0 ){
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(-1, 0, 0, 0);
    _viewer->viewAll();
    return true;
  }else if(strcmp("ViewFront",pMsg) == 0 ){
    float root = (float)(sqrt(2.0)/2.0);
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(-root, 0, 0, -root);
    _viewer->viewAll();
    return true;
  }else if(strcmp("ViewLeft",pMsg) == 0 ){
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(0.5, 0.5, 0.5, 0.5);
    _viewer->viewAll();
    return true;
  }else if(strcmp("ViewRear",pMsg) == 0 ){
    float root = (float)(sqrt(2.0)/2.0);
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(0, root, root, 0);
    _viewer->viewAll();
    return true;
  }else if(strcmp("ViewRight",pMsg) == 0 ){
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(-0.5, 0.5, 0.5, -0.5);
    _viewer->viewAll();
    return true;
  }else if(strcmp("ViewTop",pMsg) == 0 ){
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(0, 0, 0, 1);
    _viewer->viewAll();
    return true;
  }else if(strcmp("ViewAxo",pMsg) == 0 ){
    float root = (float)(sqrt(3.0)/4.0);
    SoCamera* cam = _viewer->getCamera();
    cam->orientation.setValue(-0.333333f, -0.166666f, -0.333333f, -root);
    _viewer->viewAll();
    return true;
  }else if(strcmp("OrthographicCamera",pMsg) == 0 ){
    _viewer->setCameraType(SoOrthographicCamera::getClassTypeId());
    return true;
  }else if(strcmp("PerspectiveCamera",pMsg) == 0 ){
    _viewer->setCameraType(SoPerspectiveCamera::getClassTypeId());
    return true;
  }else  if(strcmp("Undo",pMsg) == 0 ){
    getGuiDocument()->undo(1);
    return true;
  }else  if(strcmp("Redo",pMsg) == 0 ){
    getGuiDocument()->redo(1);
    return true;
  }else */

  return false;
}


bool DrawingView::onHasMsg(const char* pMsg) const
{
  if(strcmp("ViewFit",pMsg) == 0 ){
    return true;
  }/*else if(strcmp("Redo",pMsg) == 0 ){
    return getAppDocument()->getAvailableRedos() > 0; 
  }else if(strcmp("SetStereoRedGreen",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetStereoQuadBuff",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetStereoInterleavedRows",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetStereoInterleavedColumns",pMsg) == 0 ){
    return true;
  }else if(strcmp("SetStereoOff",pMsg) == 0 ){
    return true;
  }else if(strcmp("Example1",pMsg) == 0 ){
    return true;
  }else if(strcmp("Example2",pMsg) == 0 ){
    return true;
  }else if(strcmp("Example3",pMsg) == 0 ){
    return true;
  }else if(strcmp("Undo",pMsg) == 0 ){
    return getAppDocument()->getAvailableUndos() > 0;
  }else if(strcmp("ViewBottom",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewFront",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewLeft",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewRear",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewRight",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewTop",pMsg) == 0 ){
    return true;
  }else if(strcmp("ViewAxo",pMsg) == 0 ){
    return true;
  }else if(strcmp("GetCamera",pMsg) == 0 ){
    return true;
  }else if(strncmp("SetCamera",pMsg,9) == 0 ){
    return true;
  }else if(strncmp("Dump",pMsg,4) == 0 ){
    return true;
  }*/
  return false;
}

void DrawingView::load (const QString & file)
{
    QFileInfo fi(file);
    QString suffix = fi.suffix().toLower();
    if (suffix == "svg") {
        this->_drawingView->load(file);
    } else if (suffix == "svgz") {
        QByteArray contents;
        Gui::ByteArrayStream buf(contents);
        Base::igzstream gzip(file.toUtf8());
        gzip >> &buf;
        gzip.close();
        this->_drawingView->load(contents);
    }

    QSize size = this->_drawingView->renderer()->defaultSize();
    this->aspectRatio = (float)size.width() / (float)size.height();
    this->_drawingView->resize(size);
}

// Create the action groups, actions, menus and toolbars
void DrawingView::createActions()
{
 
}







// Slot function to fit (stretch/shrink) the Drawing to the view size
void DrawingView::fitDrawing()
{
       QSize DrawSize = this->_drawingView->size();
       QSize ScrollSize = this->_scroll->size();
       float ratio = DrawSize.width()/float(DrawSize.height());
       if(ratio < ScrollSize.width()/float(ScrollSize.height()))
       {
         float height = ScrollSize.height() - 2.0;
         float width = ratio * height;
         DrawSize.setWidth ((int)width);
         DrawSize.setHeight((int)height);
       }else{
         float width = ScrollSize.width()- 2.0;
         float height = width / ratio;
         DrawSize.setWidth ((int)width);
         DrawSize.setHeight((int)height);
       }

       //float height = std::max<float>(factor * (float)size.height(),10.0f);
       //float width = this->aspectRatio * height;
       //size.setWidth ((int)width);
       //size.setHeight((int)height);
       this->_drawingView->resize(DrawSize);

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
              // _pContextMenu->exec(cEvent->globalPos());
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
       float numTicks = (float)(-cEvent->delta())/240.0;
       float factor = pow(2.0f, numTicks);
       QSize size = this->_drawingView->size();
       float height = std::max<float>(factor * (float)size.height(),10.0f);
       float width = this->aspectRatio * height;
       size.setWidth ((int)width);
       size.setHeight((int)height);
       this->_drawingView->resize(size);
       //int ICx, ICy;
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
        //int ICx, ICy;
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


