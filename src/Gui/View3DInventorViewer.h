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

#include "Inventor/Qt/viewers/SoQtViewer.h"


class SoSeparator;
class SoShapeHints;
class SoMaterial;


namespace Gui {




/** The Inventor viewer
 *  
 */
class View3DInventorViewer: public SoQtViewer
{
public:
    View3DInventorViewer (QWidget *parent, const char *name=NULL, SbBool embed=true, Type type= SoQtViewer::BROWSER, SbBool build=true);
    ~View3DInventorViewer();

protected:
  virtual void actualRedraw(void);
  virtual SbBool processSoEvent(const SoEvent * const ev);

  void View3DInventorViewer::pan(SoCamera * cam,float aspectratio, const SbPlane & panningplane, const SbVec2f & currpos, const SbVec2f & prevpos);
  void View3DInventorViewer::zoom(SoCamera * cam, const float diffvalue);
  void View3DInventorViewer::spin(const SbVec2f & pointerpos);



private:
  SoSeparator * bckgroundroot;
  SoSeparator * foregroundroot;
  SoRotationXYZ * arrowrotation;

};

} // namespace Gui

#endif  //__VIEW3DINVENTORVIEWER__

