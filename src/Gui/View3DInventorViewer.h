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
#endif

#include "Inventor/Qt/viewers/SoQtViewer.h"


class SoSeparator;
class SoShapeHints;
class SoMaterial;
class SoRotationXYZ;
class SbSphereSheetProjector;

namespace Gui {

class ViewProviderInventor;


/** The Inventor viewer
 *  
 */
class View3DInventorViewer: public SoQtViewer
{
public:
  View3DInventorViewer (QWidget *parent, const char *name=NULL, SbBool embed=true, Type type= SoQtViewer::BROWSER, SbBool build=true);
  ~View3DInventorViewer();

  /// adds an ViewProvider to the view, e.g. from a feature
  void addViewProvider(ViewProviderInventor*);
  /// remove a ViewProvider
  void removeViewProvider(ViewProviderInventor*);
    

protected:
  static void sFinishSelectionCallback(void *,SoSelection *);
  virtual void finishSelectionCallback(SoSelection *);
  static void sMadeSelection(void *,SoPath *);
  virtual void madeSelection(SoPath *);
  static void sUnmadeSelection(void *,SoPath *);
  virtual void unmadeSelection(SoPath *);


  std::set<ViewProviderInventor*> _ViewProviderSet;

  virtual void actualRedraw(void);
  virtual SbBool processSoEvent(const SoEvent * const ev);

  
bool View3DInventorViewer::pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm);

  void reorientCamera(const SbRotation & rotation);
  void pan(SoCamera * cam,float aspectratio, const SbPlane & panningplane, const SbVec2f & currpos, const SbVec2f & prevpos);
  void zoom(SoCamera * cam, const float diffvalue);
  void spin(const SbVec2f & pointerpos);
  void panToCenter(const SbPlane & panningplane, const SbVec2f & currpos);

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
  SbTime MoveTime;
  SbTime CenterTime;

private:
  SoSeparator* createColorLegend() const;
  SoSeparator* setMarkerLabel(float x, float y, float z, const char* text) const;

private:
  SoSeparator * bckgroundroot;
  SoSeparator * foregroundroot;
  SoRotationXYZ * arrowrotation;

  SoSeparator * pcViewProviderRoot;
  SoSelection * pcSelection;


};

} // namespace Gui

#endif  //__VIEW3DINVENTORVIEWER__

