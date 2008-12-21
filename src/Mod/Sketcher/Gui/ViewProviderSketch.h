/***************************************************************************
 *   Copyright (c) 2004 J�rgen Riegel <juergen.riegel@web.de>              *
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
	virtual bool DoubleClicked(void);

	/// is called when the Provider is in edit and the mouse is moved
	virtual bool MouseMove(const Base::Vector3f &pos, const Base::Vector3f &norm);
	/// is called when the Provider is in edit and a key event ocours. Only ESC ends edit.
	virtual bool KeyPresst(int key);
	/// is called when the Provider is in edit and the mouse is klicked 
	virtual bool MouseButtonPresst(int Button, bool Presst, const Base::Vector3f &pos, const Base::Vector3f &norm);


protected:
  /// get called by the container whenever a property has been changed
  virtual void onChanged(const App::Property* prop);

};

} // namespace PartGui


#endif // __ViewProviderSketcher_H__

