/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qapplication.h>
# include <qdragobject.h>
# include <qfileinfo.h>
# include <qtimer.h>
# include <qvbox.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/actions/SoGetPrimitiveCountAction.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/events/SoEvent.h>
# include <Inventor/fields/SoSFString.h>
# include <Inventor/fields/SoSFColor.h>
#endif

#include "View3DInventor.h"
#include "View3DInventorViewer.h"
#include "Document.h"
#include "Application.h"
#include "MainWindow.h"
#include "MenuManager.h"
#include <App/Feature.h>
#include <Base/Console.h>

// build in Inventor
#include "Inventor/Qt/viewers/SoQtExaminerViewer.h"
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

#include "View3DInventorExamples.h"
#include "ViewProviderFeature.h"
#include "SoFCSelectionAction.h"

#include <locale>


using namespace Gui;


View3DInventor::View3DInventor( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags )
    :MDIView( pcDocument,parent, name, wflags)
{
  // important for highlighting 
  setMouseTracking(true);
  // accept drops on the window, get handled in dropEvent, dragEnterEvent   
  setAcceptDrops(true);
  
  // attache Parameter Observer
  hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/View");
  hGrp->Attach(this);

  // create the inventor widget and set the defaults
  _viewer = new View3DInventorViewer(this);
  setViewerDefaults();
  // check whether a perspective or orthogrphic camera should be set
  if ( hGrp->GetBool("Orthographic", false) )
    _viewer->setCameraType(SoOrthographicCamera::getClassTypeId());
  else
    _viewer->setCameraType(SoPerspectiveCamera::getClassTypeId());
  _viewer->show();

  stopSpinTimer = new QTimer(this);
  connect(stopSpinTimer, SIGNAL(timeout()), this, SLOT(stopSpinning()));
}

View3DInventor::~View3DInventor()
{
  hGrp->Detach(this);
  delete _viewer;
}

void View3DInventor::setViewerDefaults(void)
{
  Base::Console().Log("Act: View3DInventor::setViewerDefaults()\n");

  _viewer->setStereoOffset(hGrp->GetFloat("EyeDistance"      ,65.0));
  _viewer->bDrawAxisCross = hGrp->GetBool("CornerCoordSystem",true);
  _viewer->bAllowSpining =  hGrp->GetBool("UseAutoRotation"  ,true);
  _viewer->setGradientBackgroud( (hGrp->GetBool("Gradient",true)) );
#ifndef OLD_COLOR_STYLE
  unsigned long col = hGrp->GetUnsigned("BackgroundColor",0);
#else
  long col = hGrp->GetInt("BackgroundColor",0);
#endif
  float r,g,b;
#ifndef OLD_COLOR_STYLE
  r = ((col >> 24) & 0xff) / 255.0; g = ((col >> 16) & 0xff) / 255.0; b = ((col >> 8) & 0xff) / 255.0;
#else
  r = (col & 0xff) / 255.0; g = ((col >> 8) & 0xff) / 255.0; b = ((col >> 16) & 0xff) / 255.0;
#endif
  _viewer->setBackgroundColor(SbColor(r, g, b));

#ifndef OLD_COLOR_STYLE
  unsigned long col2 = hGrp->GetUnsigned("BackgroundColor2",2139082239); // default color (lila)
  unsigned long col3 = hGrp->GetUnsigned("BackgroundColor3",ULONG_MAX); // default color (white)
#else
  long col2 = hGrp->GetInt("BackgroundColor2",13467519); // default color (lila)
  long col3 = hGrp->GetInt("BackgroundColor3",16777215); // default color (white)
#endif
  float r2,g2,b2,r3,g3,b3;
#ifndef OLD_COLOR_STYLE
  r2 = ((col2 >> 24) & 0xff) / 255.0; g2 = ((col2 >> 16) & 0xff) / 255.0; b2 = ((col2 >> 8) & 0xff) / 255.0;
  r3 = ((col3 >> 24) & 0xff) / 255.0; g3 = ((col3 >> 16) & 0xff) / 255.0; b3 = ((col3 >> 8) & 0xff) / 255.0;
#else
  r2 = (col2 & 0xff) / 255.0; g2 = ((col2 >> 8) & 0xff) / 255.0; b2 = ((col2 >> 16) & 0xff) / 255.0;
  r3 = (col3 & 0xff) / 255.0; g3 = ((col3 >> 8) & 0xff) / 255.0; b3 = ((col3 >> 16) & 0xff) / 255.0;
#endif
  _viewer->setGradientBackgroudColor( SbColor(r2, g2, b2), SbColor(r3, g3, b3) );

  if(hGrp->GetBool("UseAntialiasing"  ,false))
    _viewer->getGLRenderAction()->setSmoothing(true);
  else
    _viewer->getGLRenderAction()->setSmoothing(false);
  _viewer->setEnabledFPSCounter(hGrp->GetBool("ShowFPS",false));
}

/// Observer message from the ParameterGrp
void View3DInventor::OnChange(ParameterGrp::SubjectType &rCaller,ParameterGrp::MessageType Reason)
{
  if ( strcmp(Reason,"DisablePreselection") == 0 ) {
    const ParameterGrp& rclGrp = ((ParameterGrp&)rCaller);
    SoFCEnableHighlightAction cAct( !rclGrp.GetBool("DisablePreselection", false) );
    cAct.apply(_viewer->getSceneGraph());
  } else if ( strcmp(Reason,"DisableSelection") == 0) {
    const ParameterGrp& rclGrp = ((ParameterGrp&)rCaller);
    SoFCEnableSelectionAction cAct( !rclGrp.GetBool("DisableSelection", false) );
    cAct.apply(_viewer->getSceneGraph());
  } else if ( strcmp(Reason,"HighlightColor") == 0) {
    float transparency;
    SbColor highlightColor(0.1f, 0.1f, 0.8f);
#ifndef OLD_COLOR_STYLE
    unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
    int a = (highlight)&0xff;
    highlight = hGrp->GetUnsigned("HighlightColor", highlight);
    highlight += a;
    highlightColor.setPackedValue((uint32_t)highlight, transparency);
#else
    unsigned long highlight = (unsigned long)(highlightColor.getPackedValue());
    int r = (highlight >> 24)&0xff;
    int g = (highlight >> 16)&0xff;
    int b = (highlight >>  8)&0xff;
    int a = (highlight)&0xff;
    highlight = (b << 16) | (g << 8) | r;
    highlight = hGrp->GetInt("HighlightColor", highlight);
    b = (highlight >> 16)&0xff;
    g = (highlight >>  8)&0xff;
    r = (highlight)&0xff;
    highlight = (r << 24) | (g << 16) | (b << 8) | a;
    highlightColor.setPackedValue((uint32_t)highlight, transparency);
#endif
    SoSFColor col; col.setValue(highlightColor);
    SoFCHighlightColorAction cAct( col );
    cAct.apply(_viewer->getSceneGraph());
  } else if ( strcmp(Reason,"SelectionColor") == 0) {
    float transparency;
    SbColor selectionColor(0.1f, 0.5f, 0.1f);
#ifndef OLD_COLOR_STYLE
    unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
    int a = (selection)&0xff;
    selection = hGrp->GetUnsigned("SelectionColor", selection);
    selection += a;
    selectionColor.setPackedValue((uint32_t)selection, transparency);
#else
    unsigned long selection = (unsigned long)(selectionColor.getPackedValue());
    int r = (selection >> 24)&0xff;
    int g = (selection >> 16)&0xff;
    int b = (selection >>  8)&0xff;
    int a = (selection)&0xff;
    selection = (b << 16) | (g << 8) | r;
    selection = hGrp->GetInt("SelectionColor", selection);
    b = (selection >> 16)&0xff;
    g = (selection >>  8)&0xff;
    r = (selection)&0xff;
    selection = (r << 24) | (g << 16) | (b << 8) | a;
    selectionColor.setPackedValue((uint32_t)selection, transparency);
#endif
    SoSFColor col; col.setValue(selectionColor);
    SoFCSelectionColorAction cAct( col );
    cAct.apply(_viewer->getSceneGraph());
  } else {
    setViewerDefaults();
  }
}

void View3DInventor::onRename(Gui::Document *pDoc)
{
  MDIView::onRename(pDoc);
  SoSFString name;
  name.setValue(pDoc->getDocument()->getName());
  SoFCDocumentAction cAct(name);
  cAct.apply(_viewer->getSceneGraph());
}

void View3DInventor::onUpdate(void)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::View3DInventor::onUpdate()");
#endif
  update();  
  _viewer->render();
}

const char *View3DInventor::getName(void) const
{
  return "View3DInventor";
}

// buffer acrobatics for inventor ****************************************************
static char * buffer;
static size_t buffer_size = 0;
static std::string cReturnString;

static void *
buffer_realloc(void * bufptr, size_t size)
{
  buffer = (char *)realloc(bufptr, size);
  buffer_size = size;
  return buffer;
}

const std::string &View3DInventor::writeNodesToString(SoNode * root)
{
  SoOutput out;
  buffer = (char *)malloc(1024);
  buffer_size = 1024;
  out.setBuffer(buffer, buffer_size, buffer_realloc);

  SoWriteAction wa(&out);
  wa.apply(root);

  cReturnString =buffer;
  free(buffer);
  return cReturnString;
}

// **********************************************************************************

bool View3DInventor::onMsg(const char* pMsg, const char** ppReturn)
{
  if(strcmp("ViewFit",pMsg) == 0 ){
    _viewer->viewAll();
    return true;
// comment out on older Inventor
#if (SOQT_MAJOR_VERSION >= 1 && SOQT_MINOR_VERSION >= 2)
  }else if(strcmp("SetStereoRedGreen",pMsg) == 0 ){
    _viewer->setStereoType(SoQtViewer::STEREO_ANAGLYPH);
    return true;
  }else if(strcmp("SetStereoQuadBuff",pMsg) == 0 ){
    _viewer->setStereoType(SoQtViewer::STEREO_QUADBUFFER );
    return true;
  }else if(strcmp("SetStereoInterleavedRows",pMsg) == 0 ){
    _viewer->setStereoType(SoQtViewer::STEREO_INTERLEAVED_ROWS );
    return true;
  }else if(strcmp("SetStereoInterleavedColumns",pMsg) == 0 ){
    _viewer->setStereoType(SoQtViewer::STEREO_INTERLEAVED_COLUMNS  );
    return true;
  }else if(strcmp("SetStereoOff",pMsg) == 0 ){
    _viewer->setStereoType(SoQtViewer::STEREO_NONE );
    return true;
#else
  }else if(strcmp("SetStereoRedGreen",pMsg) == 0 ){
    Base::Console().Warning("Use SoQt 1.2.x or later!\n");
    return true;
  }else if(strcmp("SetStereoQuadBuff",pMsg) == 0 ){
    Base::Console().Warning("Use SoQt 1.2.x or later!\n");
    return true;
  }else if(strcmp("SetStereoInterleavedRows",pMsg) == 0 ){
    Base::Console().Warning("Use SoQt 1.2.x or later!\n");
    return true;
  }else if(strcmp("SetStereoInterleavedColumns",pMsg) == 0 ){
    Base::Console().Warning("Use SoQt 1.2.x or later!\n");
    return true;
  }else if(strcmp("SetStereoOff",pMsg) == 0 ){
    Base::Console().Warning("Use SoQt 1.2.x or later!\n");
    return true;
#endif
  }else if(strcmp("Example1",pMsg) == 0 ){
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
  }else 

  return false;
}


bool View3DInventor::onHasMsg(const char* pMsg) const
{
  if(strcmp("Undo",pMsg) == 0 ){
    return getAppDocument()->getAvailableUndos() > 0;
  }else if(strcmp("Redo",pMsg) == 0 ){
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
  }else if(strcmp("ViewFit",pMsg) == 0 ){
    return true;
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
  }
  return false;
}

bool View3DInventor::setCamera(const char* pCamera)
{
  SoCamera * CamViewer = _viewer->getCamera();
  if(!CamViewer) {
    Base::Console().Warning("No camera set so far...");
    return false;
  }

  SoInput in;
  in.setBuffer((void*)pCamera,strlen(pCamera));

  SoNode * Cam;
  SoDB::read(&in,(SoNode*&)Cam);

  if (!Cam){
    Base::Console().Error("Camera settings failed to read: %s\n",pCamera);
    return false;
  }

  // toogle between persepective and orthographic camera
  if (Cam->getTypeId() != CamViewer->getTypeId())
  {
    _viewer->setCameraType(Cam->getTypeId());
    CamViewer = _viewer->getCamera();
  }

  SoPerspectiveCamera  * CamViewerP = 0;
  SoOrthographicCamera * CamViewerO = 0;

  if (CamViewer->getTypeId() == SoPerspectiveCamera::getClassTypeId()) {
    CamViewerP = (SoPerspectiveCamera *)CamViewer;  // safe downward cast, knows the type
  }else if (CamViewer->getTypeId() == SoOrthographicCamera::getClassTypeId()) {
    CamViewerO = (SoOrthographicCamera *)CamViewer;  // safe downward cast, knows the type
  }

  if (Cam->getTypeId() == SoPerspectiveCamera::getClassTypeId()) {
    if(CamViewerP){
      CamViewerP->position      = ((SoPerspectiveCamera *)Cam)->position;
      CamViewerP->orientation   = ((SoPerspectiveCamera *)Cam)->orientation;
      CamViewerP->nearDistance  = ((SoPerspectiveCamera *)Cam)->nearDistance;
      CamViewerP->farDistance   = ((SoPerspectiveCamera *)Cam)->farDistance;
      CamViewerP->focalDistance = ((SoPerspectiveCamera *)Cam)->focalDistance;
    }
    else
      Base::Console().Error("Camera type missmatch");
  }else if (Cam->getTypeId() == SoOrthographicCamera::getClassTypeId()) {
    if(CamViewerO){
      CamViewerO->viewportMapping  = ((SoOrthographicCamera *)Cam)->viewportMapping;
      CamViewerO->position         = ((SoOrthographicCamera *)Cam)->position;
      CamViewerO->orientation      = ((SoOrthographicCamera *)Cam)->orientation;
      CamViewerO->nearDistance     = ((SoOrthographicCamera *)Cam)->nearDistance;
      CamViewerO->farDistance      = ((SoOrthographicCamera *)Cam)->farDistance;
      CamViewerO->focalDistance    = ((SoOrthographicCamera *)Cam)->focalDistance;
      CamViewerO->aspectRatio      = ((SoOrthographicCamera *)Cam)->aspectRatio ;
      CamViewerO->height           = ((SoOrthographicCamera *)Cam)->height;
    }
    else
      Base::Console().Error("Camera type missmatch");
  }

  return true;
}

void View3DInventor::toggleClippingPlane()
{
  _viewer->toggleClippingPlane();
}

bool View3DInventor::hasClippingPlane() const
{
  return _viewer->hasClippingPlane();
}

void View3DInventor::setCursor(const QCursor& aCursor)
{
  _viewer->getWidget()->setCursor(aCursor);
}

void View3DInventor::dump(const char* filename)
{
  SoGetPrimitiveCountAction action;
  action.setCanApproximate(true);
  action.apply(_viewer->getSceneGraph());

  if ( action.getTriangleCount() > 100000 || action.getPointCount() > 30000 || action.getLineCount() > 10000 )
    _viewer->dumpToFile(filename,true);
  else
    _viewer->dumpToFile(filename,false);
}

void View3DInventor::showActiveView( MDIView* view )
{
  if ( view != this ) {
    bool canStartTimer = (!isTopLevel() && (view->isMaximized() || view->isFullScreen()));
    if ( canStartTimer ) {
      // do a sinlge shot event (maybe insert a checkbox in viewer settings)
      int msecs = hGrp->GetInt("StopSpinningIfDeactivated", 3000);
      if (msecs >= 0) // if < 0 do not stop rotation
        stopSpinTimer->start(msecs, true);
    } else if ( stopSpinTimer->isActive() ) {
      // If the active is not maximized anymore we can also stop the timer
      stopSpinTimer->stop();
    }
  } else if ( stopSpinTimer->isActive() ) {
    // if the window becomes active before the timeout event is emitted we stop the timer
    stopSpinTimer->stop();
  }
}

void View3DInventor::stopSpinning()
{
  _viewer->stopSpinning();
}

/**
 * Drops the event \a e and writes the right Python command.
 */
void View3DInventor::dropEvent ( QDropEvent * e )
{
  if ( QUriDrag::canDecode(e) )
  {
    QStringList fn;
    QUriDrag::decodeLocalFiles(e, fn);

    App::Document* pDoc = getAppDocument();
    if ( pDoc ) 
    {
      for ( QStringList::Iterator it = fn.begin(); it != fn.end(); ++it ) {

        QFileInfo info(*it);
        if ( info.exists() && info.isFile() )
        {
          if ( App::GetApplication().hasOpenType( info.extension().latin1() ) )
            Application::Instance->import(info.absFilePath().latin1(), pDoc->getName());
        }
      }
    }
  }else
    MDIView::dropEvent(e);
}

void View3DInventor::dragEnterEvent ( QDragEnterEvent * e )
{
  // Here we must allow uri drafs and check them in dropEvent
  if ( QUriDrag::canDecode(e) )
    e->accept();
  else
    e->ignore();
}

void View3DInventor::setCurrentViewMode( ViewMode b )
{
  ViewMode curmode = _actualMode;
  MDIView::setCurrentViewMode( b );

  // This widget becomes the focus proxy of the embedded GL widget.if we leave 
  // the 'Normal' mode. If we reenter 'Normal' mode the focus proxy is reset to 0.
  // If we change from 'TopLevel' mode to 'Fullscreen' mode or vice versa nothing
  // happens.
  // Grabbing keyboard when leaving 'Normal' mode (as done in a recent version) should
  // be avoided because when two or more windows are either in 'TopLevel' or 'Fullscreen'
  // mode only the last window gets all key event even if it is not the active one.
  //
  // It is important to set the focus proxy to get all key events otherwise we would loose
  // control after redirecting the first key event to the GL widget.
  // We redirect these events in keyPressEvent() and keyReleaseEvent().
  if ( curmode == Normal ) {
    _viewer->getGLWidget()->setFocusProxy(this);
  } else if ( b == Normal ) {
    _viewer->getGLWidget()->setFocusProxy(0);
  }
}

bool View3DInventor::eventFilter(QObject* o, QEvent* e)
{
  // As long as this widget is a top-level widget (either 'TopLevel' or 'Fullscrenn' mode) we 
  // redirect any accel event to the main window that handles such events.
  // In case the event isn't handled by any widget we receive it again as a key event.
  if ( _actualMode != Normal && o == this && e->type() == QEvent::Accel ) {
    QApplication::sendEvent(getMainWindow(),e);
    return true;
  } else {
    return MDIView::eventFilter(o,e);
  }
}

void View3DInventor::keyPressEvent ( QKeyEvent* e )
{
  if ( _actualMode != Normal )
  {
    // If the widget is in fullscreen mode then we can return to normal mode either
    // by pressing the matching accelerator or ESC. 
    if ( e->key() == Key_Escape )
    {
      setCurrentViewMode(Normal);
    }
    else
    {
      // Note: The key events should be redirected directly to the GL widget and not the main window
      // otherwise the first redirected key event always disappears in hyperspace.
      //
      // send the event to the GL widget that converts to and handles an SoEvent
      QWidget* w = _viewer->getGLWidget();
      QApplication::sendEvent(w,e);
    }
  }
  else // the occupied key events F,D and U are are "eaten" by the main window
  {
    QMainWindow::keyPressEvent( e );
  }
}

void View3DInventor::keyReleaseEvent ( QKeyEvent* e )
{
  if ( _actualMode != Normal )
  {
    // send the event to the GL widget that converts to and handles an SoEvent
    QWidget* w = _viewer->getGLWidget();
    QApplication::sendEvent(w,e);
  }
  else
  {
    QMainWindow::keyReleaseEvent( e );
  }
}



#include "moc_View3DInventor.cpp"
