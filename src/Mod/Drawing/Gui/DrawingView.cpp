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


#include "PreCompiled.h"
#ifndef _PreComp_
# include <QAction>
# include <QApplication>
# include <QBuffer>
# include <QFileInfo>
# include <QFileDialog>
# include <QPainter>
# include <QPrinter>
# include <QPrintDialog>
# include <QScrollArea>
# include <QSlider>
# include <QStatusBar>
# include <QSvgRenderer>
# include <QSvgWidget>
# include <QWheelEvent>
# include <strstream>
#endif

#include "DrawingView.h"
#include <Base/Stream.h>
#include <Base/gzstream.h>
#include <Gui/WaitCursor.h>

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

    _scroll = new DrawingScrollArea(this);
    _drawingView = new QSvgWidget(_scroll);
    //_drawingView->setBackgroundRole(QPalette::Base);
    QPalette p(_drawingView->palette());
    p.setColor(QPalette::Background, Qt::white);
    _drawingView->setPalette(p);
    //this->_drawingView->setFocus();
    //scroll->setFocusProxy(_drawingView);
    //_drawingView->setFocusProxy(scroll);
    _scroll->setWidget(_drawingView);
    //_drawingView->resize(20, 20);
    setCentralWidget(_scroll);

    _currMode = nothing;
    _currX = 0;
    _currY = 0;

    // Create the actions, menus and toolbars
    createActions();
}

DrawingView::~DrawingView()
{
}

bool DrawingView::onMsg(const char* pMsg, const char** ppReturn)
{
    if (strcmp("ViewFit",pMsg) == 0) {
        fitDrawing();
        return true;
    }
    return false;
}

bool DrawingView::onHasMsg(const char* pMsg) const
{
    if (strcmp("ViewFit",pMsg) == 0)
        return true;
    else if (strcmp("Print",pMsg) == 0)
        return true; 
    else if (strcmp("PrintPdf",pMsg) == 0)
        return true; 
    return false;
}

void DrawingView::viewAll()
{
    fitDrawing();
}

void DrawingView::printPdf()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export PDF"), QString(), tr("PDF file (*.pdf)"));
    if (!filename.isEmpty()) {
        Gui::WaitCursor wc;
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);
        printer.setOrientation(QPrinter::Landscape);
        print(&printer);
    }
}

void DrawingView::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Landscape);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        print(&printer);
    }
}

void DrawingView::print(QPrinter* printer)
{
    QPainter p(printer);
    QRect rect = printer->pageRect();
    this->_drawingView->renderer()->render(&p, rect);
    p.end();
}

bool DrawingView::load (const QString & file)
{
    QFileInfo fi(file);
    QString suffix = fi.suffix().toLower();
    bool ok;
    if (suffix == QLatin1String("svg")) {
        ok = this->_drawingView->renderer()->load(file);
    }
    else if (suffix == QLatin1String("svgz")) {
        QByteArray contents;
        QBuffer buffer(&contents);
        buffer.open(QIODevice::WriteOnly);
        Base::IODeviceOStreambuf buf(&buffer);
        Base::igzstream gzip(file.toUtf8());
        gzip >> &buf;
        gzip.close();
        ok = this->_drawingView->renderer()->load(contents);
    }

    if (ok) {
        QSize size = this->_drawingView->renderer()->defaultSize();
        this->aspectRatio = (float)size.width() / (float)size.height();
    }
    else {
        this->aspectRatio = 1.0f;
    }

    return ok;
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
    if (ratio < ScrollSize.width()/float(ScrollSize.height())) {
        float height = ScrollSize.height() - 2.0;
        float width = ratio * height;
        DrawSize.setWidth ((int)width);
        DrawSize.setHeight((int)height);
    }
    else {
        float width = ScrollSize.width()- 2.0;
        float height = width / ratio;
        DrawSize.setWidth ((int)width);
        DrawSize.setHeight((int)height);
    }

    this->_drawingView->resize(DrawSize);
}

// Slot function to display the Drawing at a 1:1 scale"
void DrawingView::oneToOneDrawing()
{
    QSize size = this->_drawingView->renderer()->defaultSize();
    if (size.isValid())
        this->_drawingView->resize(size);
    else
        this->_drawingView->resize(this->size());
}

// Mouse press event
void DrawingView::mousePressEvent(QMouseEvent* cEvent)
{
    if (_mouseEventsEnabled == true) {
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
    if (_mouseEventsEnabled == true) {
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
    if (_mouseEventsEnabled == true) {
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
    if (_mouseEventsEnabled == true) {
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
    if (_statusBarEnabled == true) {
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
}

#include "moc_DrawingView.cpp"


