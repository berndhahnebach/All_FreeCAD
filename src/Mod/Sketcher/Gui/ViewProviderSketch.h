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


#ifndef __ViewProviderSketcher_H__
#define __ViewProviderSketcher_H__

#include <Mod/Part/Gui/ViewProvider2DObject.h>

 

class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace Gui {
  class View3DInventorViewer;
  class SoFCSelection;
}

namespace Sketcher {
	class SketchFlatInterface;
	class SketchObject;
}

namespace SketcherGui {


class SketcherGuiExport ViewProviderSketch:public PartGui::ViewProvider2DObject
{
  PROPERTY_HEADER(PartGui::ViewProviderSketch);

public:
	/// constructor
	ViewProviderSketch();
	/// destructor
	virtual ~ViewProviderSketch();

	//App::PropertyBool ShowGrid;

	virtual void attach(App::DocumentObject *);
	virtual void updateData(const App::Property*);


	virtual bool setEdit(int ModNum);
	virtual void unsetEdit(void);

	/// Is called by the tree if the user double click on the object
	virtual bool doubleClicked(void);

	/// is called when the Provider is in edit and the mouse is moved
	virtual bool mouseMove(const Base::Vector3f &pNear, const Base::Vector3f &pFar, SoPickedPoint* Point);
	/// is called when the Provider is in edit and a key event ocours. Only ESC ends edit.
	virtual bool keyPressed(int key);
	/// is called when the Provider is in edit and the mouse is clicked 
	virtual bool mouseButtonPressed(int Button, bool pressed, const Base::Vector3f &pNear, const Base::Vector3f &pFar, SoPickedPoint* Point);
	/// draw the sketch in the inventor nodes
	void draw(void);

	/// give the coordinates of a line on the sketch plane in sketcher (2D) coordinates
	void CoordsOnSketchPlane(double &u, double &v,const Base::Vector3f &pNear, const Base::Vector3f &pFar);
    /// helper to detect preselection
    bool HandlePreselection(SoPickedPoint* Point);
	/// set constrain table
	enum {
		CONSTRAIN_LOCK,
		CONSTRAIN_HORIZONTAL,
		CONSTRAIN_VERTICAL
	};
	/// is called by the GuiCommands to set the constraints on the selected items:
	bool setConstrainOnSelected(int Constrain);

	/// mode table
	enum {
		STATUS_NONE,
		STATUS_SKETCH_CreateArc,
		STATUS_SKETCH_CreateCircle,
		STATUS_SKETCH_CreateLine,
		STATUS_SKETCH_CreatePolyline,
		STATUS_SKETCH_CreateRectangle,
		STATUS_SKETCH_CreateText,
		STATUS_SKETCH_DraggPoint,
		STATUS_SKETCH_DoLine
	};
	/// is called by GuiCommands to set the drawing mode
	void setSketchMode(int mode);
	/// get the sketch mode
	int getSketchMode(void){return Mode;}


    Sketcher::SketchObject* getSketchObject(void);

protected:


  /// get called by the container whenever a property has been changed
  virtual void onChanged(const App::Property* prop);

  // helper
  void createEditInventorNodes(void);

  // modes while sketching
  int Mode;
  // dragged point
  int DraggPoint;

  int PreselectCurve;
  int PreselectPoint;
  // pointer to the Solver
  Sketcher::SketchFlatInterface *SketchFlat;

  // nodes for the visuals 
  SoMaterial * PointsMaterials;
  SoMaterial * CurvesMaterials;
  SoMaterial * LinesMaterials;
  SoCoordinate3 *PointsCoordinate;
  SoCoordinate3 *CurvesCoordinate;
  SoCoordinate3 *LinesCoordinate;
  SoLineSet* CurveSet;
  SoLineSet* LineSet;
  SoPointSet* PointSet;
  SoSeparator  *EditRoot;
};

} // namespace PartGui


#endif // __ViewProviderSketcher_H__

