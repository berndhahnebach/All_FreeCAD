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


#ifndef __VIEW3DINVENTORVIEWER__
#define __VIEW3DINVENTORVIEWER__


#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include <set>
#include <stack>

#include <Base/Vector3D.h>
#include <Base/Type.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/Qt/SoQtCursor.h>
#include "Selection.h"
#include "Flag.h"


class SoSeparator;
class SoShapeHints;
class SoMaterial;
class SoRotationXYZ;
class SbSphereSheetProjector;
class SoEventCallback;
class SbBox2s;

namespace Gui {

class ViewProvider;
class SoFCBackgroundGradient;
class AbstractMouseModel;


/** The Inventor viewer
 *
 */
class GuiExport View3DInventorViewer: public SoQtViewer, public Gui::SelectionSingleton::ObserverType
{
  SOQT_OBJECT_ABSTRACT_HEADER(View3DInventorViewer, SoQtViewer);

public:
  View3DInventorViewer (QWidget *parent, const char *name=NULL, SbBool embed=true, Type type= SoQtViewer::BROWSER, SbBool build=true);
  ~View3DInventorViewer();

  /// Observer message from the Selection
  virtual void OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason);

  SoDirectionalLight* getBacklight(void) const;
  void setBacklight(SbBool on);
  SbBool isBacklight(void) const;
  void setSceneGraph (SoNode *root);

  void setAnimationEnabled(const SbBool enable);
  SbBool isAnimationEnabled(void) const;

  void startAnimating(const SbVec3f& axis, float velocity);
  void stopAnimating(void);
  SbBool isAnimating(void) const;

  void setFeedbackVisibility(const SbBool enable);
  SbBool isFeedbackVisible(void) const;

  void setFeedbackSize(const int size);
  int getFeedbackSize(void) const;

  virtual void setViewing(SbBool enable);
  virtual void setCursorEnabled(SbBool enable);

  /// adds an ViewProvider to the view, e.g. from a feature
  void addViewProvider(ViewProvider*);
  /// remove a ViewProvider
  void removeViewProvider(ViewProvider*);
  /// set the ViewProvider in special edit mode
  bool setEdit(Gui::ViewProvider* p, int ModNum=0);
  /// reset from edit mode
  void resetEdit(void);

  /// Background modes for the savePicture() method
  enum eBackgroundType { 
    Current     = 0,  /**< Use the current viewer Background */
    Black       = 1,  /**< Black background */
    White       = 2,  /**< White background  */ 
    Transparent = 3,  /**< Transparent background  */
  };
  /**
   * Creates an image with width \a w and height \a h of the current scene graph
   * and exports the rendered scenegraph directly to file \a filename.
   * If \a comment is set to '$MIBA' information regarding the MIBA standard is
   * embedded to the picture, otherwise the \a comment is embedded as is.
   * The appropriate file format must support embedding meta information which
   * is provided by JPEG or PNG.
   */
  void savePicture(const char* filename, int w, int h, int eBackgroundType,
                   const char* comment) const;
  void savePicture(int w, int h, int eBackgroundType, QImage&) const;
  void saveGraphic(const char* filename, int pagesize, int eBackgroundType) const;
  /// Pick modes for picking points in the scene
  enum ePickMode {
    Lasso       = 0,  /**< Select objects using a lasso. */
    Rectangle   = 1,  /**< Select objects using a rectangle. */
    BoxZoom     = 2,  /**< Perform a box zoom. */
    Clip        = 3,  /**< Clip objects using a lasso. */
  };
  void startPicking( ePickMode = Lasso );
  void stopPicking();
  bool isPicking() const;
  const std::vector<SbVec2f>& getPickedPolygon(SbBool* clip_inner=0) const;
  std::vector<int> tessellate(const std::vector<SbVec2f>&) const;

  void setEditing(SbBool edit);
  SbBool isEditing() const { return this->editing; }
  void setEditingCursor (const SoQtCursor& cursor);
  void setEditingCursor (const QCursor& cursor);
  void setRedirectToSceneGraph(SbBool redirect) { this->redirected = redirect; }
  SbBool isRedirectedToSceneGraph() const { return this->redirected; }

  /**
   * Writes the current scenegraph to an Inventor file, either in ascii or binary. 
   */
  bool dumpToFile( const char* filename, bool binary ) const;

  // calls a PickAction on the scene graph
  bool pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm) const;
  SoPickedPoint* pickPoint(const SbVec2s& pos) const;
  void pubSeekToPoint(const SbVec2s& pos);
  void pubSeekToPoint(const SbVec3f& pos);
  /**
   * Set up a callback function \a cb which will be invoked for the given eventtype. 
   * \a userdata will be given as the first argument to the callback function. 
   */
  void addEventCallback(SoType eventtype, SoEventCallbackCB * cb, void* userdata = 0);
  /**
   * Unregister the given callback function \a cb.
   */
  void removeEventCallback(SoType eventtype, SoEventCallbackCB * cb, void* userdata = 0);
  ViewProvider* getViewProviderByPath(SoPath*) const;
  std::vector<ViewProvider*> getViewProvidersOfType(const Base::Type& typeId) const;

  /** @name Clipping plane
   */
  //@{
  /** Returns the view direction from the user's eye point in direction to the
   * viewport which is actually the negative normal of the near plane.
   * The vector is normalized to length of 1.
   */
  SbVec3f getViewDirection() const;
  /** Returns the up direction */
  SbVec3f getUpDirection() const;
  /** Returns the 3d point on the focal plane to the given 2d point. */
  SbVec3f getPointOnScreen(const SbVec2s&) const;
  /** Returns the near plane represented by its normal and base point. */
  void getNearPlane(SbVec3f& rcPt, SbVec3f& rcNormal) const;
  /** Returns the far plane represented by its normal and base point. */
  void getFarPlane(SbVec3f& rcPt, SbVec3f& rcNormal) const;
  /** Adds or remove a manipulator to/from the scenegraph. */
  void toggleClippingPlane();
  /** Checks whether a clipping plane is set or not. */
  bool hasClippingPlane() const;
  /** Project the given normalized 2d point onto the near plane */
  SbVec3f projectOnNearPlane(const SbVec2f&) const;
  /** Project the given normalized 2d point onto the far plane */
  SbVec3f projectOnFarPlane(const SbVec2f&) const;
  //@}

  /** 
   * Zooms the viewport to the size of the bounding box. 
   */
  void boxZoom(const SbBox2s&);
  /**
   * Reposition the current camera so we can see the complete scene.
   */
  void viewAll();
  /**
   * Reposition the current camera so we can see all selected objects 
   * of the scene. Therefore we search for all SOFCSelection nodes, if
   * none of them is selected nothing happens.
   */
  void viewSelection();

  /** @name Modus handling of the viewer
    * Here the you can switch on/off several features
    * and modies of the Viewer
    */
  //@{
  enum ViewerMod {
      ShowCoord=1,       /**< Enables the Coordinate system in the corner. */
      ShowFPS  =2,       /**< Enables the Frams per Second counter. */
      SimpleBackground=4,/**< switch to a simple background. */
      DisallowRotation=8,/**< switch of the rotation. */
      DisallowPanning=16,/**< switch of the panning. */
      DisallowZooming=32,/**< switch of the zooming. */
     };
  //@}


  /** @name Draw routines */
  //@{
  void drawRect (int x, int y, int w, int h);
  void drawLine (int x1, int y1, int x2, int y2);
  //@}

  void setGradientBackgroud(bool b);
  void setGradientBackgroudColor(const SbColor& fromColor, const SbColor& toColor);
  void setGradientBackgroudColor(const SbColor& fromColor, const SbColor& toColor, const SbColor& midColor);
  void setEnabledFPSCounter(bool b);

  void setMouseModel(int i){_iMouseModel = i;}

  void openPopupMenu(const SbVec2s& position);
  void setPopupMenuEnabled(const SbBool on);
  SbBool isPopupMenuEnabled(void) const;

protected:
  unsigned long             currMod;
  std::stack<unsigned long> ModStack;

  std::set<ViewProvider*> _ViewProviderSet;
  ViewProvider* inEdit;

  virtual void actualRedraw(void);
  virtual void setSeekMode(SbBool enable);
  virtual void afterRealizeHook(void);
  virtual void processEvent(QEvent * event);
  virtual SbBool processSoEvent(const SoEvent * const ev);
  virtual SbBool processSoEvent1(const SoEvent * const ev);
  virtual SbBool processSoEvent2(const SoEvent * const ev);

  void panToCenter(const SbPlane & panningplane, const SbVec2f & currpos);
  void printDimension();

  static void clearBuffer(void * userdata, SoAction * action);
  static void interactionStartCB(void * data, SoQtViewer * viewer);
  static void interactionFinishCB(void * data, SoQtViewer * viewer);
  static void interactionLoggerCB(void * ud, SoAction* action);

  bool _bRejectSelection;
  SbTime MoveTime;
  SbTime CenterTime;

  SoFCBackgroundGradient *pcBackGround;

private:
  static void selectCB(void * closure, SoPath * p);
  static void deselectCB(void * closure, SoPath * p);
  static void tessCB(void * v0, void * v1, void * v2, void * cbdata);

private:
  SoSeparator * backgroundroot;
  SoSeparator * foregroundroot;
  SoRotationXYZ * arrowrotation;
  SoDirectionalLight* backlight;

  SoSeparator * pcViewProviderRoot;
  SoEventCallback* pEventCallback;
  AbstractMouseModel* pcMouseModel;
  std::vector<SbVec2f> pcPolygon;
  SbBool clipInner;

  // Seek functionality
  SoTimerSensor * seeksensor;
  float seekperiod;
  SbBool inseekmode;
  SbBool seektopoint;
  SbVec3f camerastartposition, cameraendposition;
  SbRotation camerastartorient, cameraendorient;
  float seekdistance;
  SbBool seekdistanceabs;



  void initialize();
  void finalize();
  void reorientCamera(const SbRotation & rotation);
  void spin(const SbVec2f & pointerpos);
  void pan(SoCamera * cam,float aspectratio, const SbPlane & panningplane, const SbVec2f & currpos, const SbVec2f & prevpos);
  void zoom(SoCamera * cam, const float diffvalue);
  void zoomByCursor(const SbVec2f & mousepos, const SbVec2f & prevpos);

  SbVec2f lastmouseposition;
  SbPlane panningplane;

  SbBool spinanimatingallowed;
  SbVec2f lastspinposition;
  int spinsamplecounter;
  SbRotation spinincrement;
  SbSphereSheetProjector * spinprojector;

  SbRotation spinRotation;
  bool _bSpining;
  int _iMouseModel;
  QCursor _oldCursor;
  SbBool axiscrossEnabled;
  int axiscrossSize;

  void drawAxisCross(void);
  static void drawArrow(void);

  struct { // tracking mouse movement in a log
    short size;
    short historysize;
    SbVec2s * position;
    SbTime * time;
  } log;

  SbBool button1down;
  SbBool button3down;
  SbBool ctrldown, shiftdown;
  SbBool editing;
  QCursor editCursor;
  SbBool redirected;
  SbBool menuenabled;

  void clearLog(void);
  void addToLog(const SbVec2s pos, const SbTime time);

  SbTime prevRedrawTime;

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

  void setCursorRepresentation(int mode);

public:
    void addFlag(Flag*, FlagLayout::Position);

private:
    FlagLayout* _flaglayout;
};

} // namespace Gui

#endif  //__VIEW3DINVENTORVIEWER__

