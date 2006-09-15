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

#include <qcursor.h>
#include <set>
#include <stack>

#include <Base/Vector3D.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>
#include "Selection.h"


class SoSeparator;
class SoShapeHints;
class SoMaterial;
class SoRotationXYZ;
class SbSphereSheetProjector;
class SoEventCallback;
class SbBox2f;

namespace Gui {

class ViewProvider;
class SoFCBackgroundGradient;


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


  /// adds an ViewProvider to the view, e.g. from a feature
  void addViewProvider(ViewProvider*);
  /// remove a ViewProvider
  void removeViewProvider(ViewProvider*);
  /**
   * Creates an image with width \a w and height \a h of the current scene graph. Pixels per inch is set to \a r.
   */
  QImage makeScreenShot( int w, int h, float r, int c, const QColor& ) const;
  /**
   * An overloaded method that does basically the same as the method above unless it exports the rendered scenegraph directly
   * to file \a filename with the extension \a filetypeextension.
   * Creates also a XML stream regarding MIBA standard. To embed in a picture comment field (e.g. JPEG).
   */
  bool makeScreenShot( const SbString& filename, const SbName& filetypeextension, int w, int h, float r, int c, const QColor& ) const;
  /**
   * Writes the current scenegraph to an Inventor file, either in ascii or binary. 
   */
  bool dumpToFile( const char* filename, bool binary ) const;

  // calls a PickAction on the scene graph
  bool pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm) const;
  SoPickedPoint* pickPoint(const SbVec2s& pos) const;


  /** @name Clipping plane
   */
  //@{
  /** Returns the view direction from the viewport in direction to the user's eye point which is actually the normal of the front clipping plane. The vector
   * is normalized to length of 1.
   */
  Base::Vector3f getViewDirection() const;
  /** Returns the front clipping plane represented by its normal and base point. */
  void getFrontClippingPlane( Base::Vector3f& rcPt, Base::Vector3f& rcNormal ) const;
  /** Returns the back clipping plane represented by its normal and base point. */
  void getBackClippingPlane( Base::Vector3f& rcPt, Base::Vector3f& rcNormal ) const;
  /** Adds or remove a manipulator to/from the scenegraph. */
  void toggleClippingPlane();
  /** Checks whether a clipping plane is set or not. */
  bool hasClippingPlane() const;
  //@}

  /** 
   * Zooms the viewport to the size of the bounding box. 
   * FIXME: This method is due to testing purposes public but should become protected later on.
   */
  void boxZoom( const SbBox2f& );

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
  void drawRect ( int x, int y, int w, int h, QPainter* p = 0 );
  void drawNode ( int x, int y, int w, int h, QPainter* p = 0 );
  void drawLine ( int x1, int y1, int x2, int y2, QPainter* p = 0 );
  void drawCircle ( int x, int y, int r, QPainter* p = 0 );
  void drawText ( int x, int y, const QString & str, QPainter* p = 0 );
  //@}

  bool bDrawAxisCross;
  bool bAllowSpining;

  void setGradientBackgroud(bool b);
  void setGradientBackgroudColor( const SbColor& fromColor, const SbColor& toColor );
  void setEnabledFPSCounter(bool b);
  void stopSpinning();

protected:
  unsigned long             currMod;
  std::stack<unsigned long> ModStack;

  /*
  static void sFinishSelectionCallback(void *,SoSelection *);
  virtual void finishSelectionCallback(SoSelection *);
  static void sMadeSelection(void *,SoPath *);
  virtual void madeSelection(SoPath *);
  static void sUnmadeSelection(void *,SoPath *);
  virtual void unmadeSelection(SoPath *);
*/
  virtual void openPopupMenu(const SbVec2s& position);
  void setPopupMenuEnabled(const SbBool on);
  SbBool isPopupMenuEnabled(void) const;

  std::set<ViewProvider*> _ViewProviderSet;

  virtual void actualRedraw(void);
  virtual void processEvent(QEvent * event);
  virtual SbBool processSoEvent(const SoEvent * const ev);
  /// gets called when the container widget's size  has changed
  virtual void sizeChanged  ( const SbVec2s& );

  void reorientCamera(const SbRotation & rotation);
  void pan(SoCamera * cam,float aspectratio, const SbPlane & panningplane, const SbVec2f & currpos, const SbVec2f & prevpos);
  void zoom(SoCamera * cam, const float diffvalue);
  void spin(const SbVec2f & pointerpos);
  void panToCenter(const SbPlane & panningplane, const SbVec2f & currpos);
  void printDimension();

  static void clearBuffer(void * userdata, SoAction * action);
  static void interactionStartCB(void * data, SoQtViewer * viewer);
  static void interactionFinishCB(void * data, SoQtViewer * viewer);
  static void interactionLoggerCB(void * ud, SoAction* action);

  SbVec2f lastmouseposition;
  SbPlane panningplane;

  SbBool spinanimatingallowed;
  SbVec2f lastspinposition;
  int spinsamplecounter;
  SbRotation spinincrement;
  SbSphereSheetProjector * spinprojector;
  SbTime prevRedrawTime;
  SbRotation spinRotation;
  void clearLog(void);
  void addToLog(const SbVec2s pos, const SbTime time);

  struct { // tracking mouse movement in a log
    short size;
    short historysize;
    SbVec2s * position;
    SbTime * time;
  } log;


  SbBool axiscrossEnabled;
  int axiscrossSize;
  void drawAxisCross(void);
  static void drawArrow(void);

  bool _bSpining;
  bool _bRejectSelection;
  SbBool MenuEnabled;
  SbTime MoveTime;
  SbTime CenterTime;

  SoFCBackgroundGradient *pcBackGround;

private:
  SoSeparator * backgroundroot;
  SoSeparator * foregroundroot;
  SoRotationXYZ * arrowrotation;

  SoSeparator * pcViewProviderRoot;
  QCursor _oldCursor;
};

} // namespace Gui

#endif  //__VIEW3DINVENTORVIEWER__

