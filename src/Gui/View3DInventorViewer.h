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

#ifndef _PreComp_
# include <set>
# include <stack>
#endif

#include <Base/Vector3D.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>
#include "Selection.h"


class SoSeparator;
class SoShapeHints;
class SoMaterial;
class SoRotationXYZ;
class SbSphereSheetProjector;

namespace Gui {

class ViewProviderInventor;
class SoFCBackgroundGradient;


/** The Inventor viewer
 *  
 */
class GuiExport View3DInventorViewer: public SoQtViewer, public Gui::SelectionSingelton::ObserverType
{

  SOQT_OBJECT_ABSTRACT_HEADER(View3DInventorViewer, SoQtViewer);

public:
  View3DInventorViewer (QWidget *parent, const char *name=NULL, SbBool embed=true, Type type= SoQtViewer::BROWSER, SbBool build=true);
  ~View3DInventorViewer();

  /// Observer message from the Selection
  virtual void OnChange(Gui::SelectionSingelton::SubjectType &rCaller,Gui::SelectionSingelton::MessageType Reason);


  /// adds an ViewProvider to the view, e.g. from a feature
  void addViewProvider(ViewProviderInventor*);
  /// remove a ViewProvider
  void removeViewProvider(ViewProviderInventor*);
  /**
   * Creates an image with width \a w and height \a h of the current scene graph. Pixels per inch is set to \a r.
   */
  QImage makeScreenShot( int w, int h, float r, int c, const QColor& ) const;
  /**
   * An overloaded method that does basically the same as the method above unless it exports the rendered scenegraph directly
   * to file \a filename with the extension \a filetypeextension.
   */
  bool makeScreenShot( const SbString& filename, const SbName& filetypeextension, int w, int h, float r, int c, const QColor& ) const;
 
  // calls a PickAction on the scene graph
  bool pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm);


  /** @name Clipping plane
   */
  //@{
  /** Returns the view direction from the viewport in direction to the user's eye point which is actually the normal of the front clipping plane. The vector
   * is normalized to length of 1.
   */
  Base::Vector3D getViewDirection() const;
  /** Returns the front clipping plane represented by its normal and base point. */
  void getFrontClippingPlane( Base::Vector3D& rcPt, Base::Vector3D& rcNormal ) const;
  /** Returns the back clipping plane represented by its normal and base point. */
  void getBackClippingPlane( Base::Vector3D& rcPt, Base::Vector3D& rcNormal ) const;
  //@}

  /** @name Modus handling of the viewer
    * Here the you can switch on/off several features
    * and modies of the Viewer
    */
  //@{

  enum ViewerMod {
      ShowCoord=1,       /**< Enables the Coordinate system in the corner. */  
      ShowFPS  =2,       /**< Enables the Frams per Second counter. */  
      SimpleBackround=4, /**< switch to a simple background. */  
      DisallowRotation=8,/**< switch of the rotation. */  
      DisallowPaning=16, /**< switch of the rotation. */  
      DisallowZooming=32,/**< switch of the rotation. */  
     }; 

  //@{


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

  std::set<ViewProviderInventor*> _ViewProviderSet;

  virtual void actualRedraw(void);
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
};

} // namespace Gui

#endif  //__VIEW3DINVENTORVIEWER__

