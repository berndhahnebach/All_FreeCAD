/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de)          *
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




#ifndef _DocumentObject_h_
#define _DocumentObject_h_

#include <App/PropertyContainer.h>
#include <App/PropertyStandard.h>

#include <Base/TimeInfo.h>


namespace App
{
  class Document;
  class DocumentObjectPy;

/** Base class of all Classes handled in the Document
 */
class AppExport DocumentObject: public App::PropertyContainer
{
    PROPERTY_HEADER(App::DocumentObject);

public:

//  PropertyString showMode;
  PropertyString name;
//  PropertyBool visibility;

  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void){return "";}


	/// Constructor
	DocumentObject(void);
  virtual ~DocumentObject();

  App::Document &getDocument(void);
  void setDocument(App::Document* doc);


  /** Set the property touched -> changed, cause recomputation in Update()
	 *
	 */
	//@{
	//void TouchProperty(const char *Name);
  /// set this feature touched (cause recomputation on depndend features)
	void Touch(void);
  /// set the view parameter of this feature touched (cause recomputation of representation)
	void TouchView(void);
  /// get the touch time
  Base::TimeInfo getTouchTime(void){return touchTime;}
  /// get the view touch time
  Base::TimeInfo getTouchViewTime(void){return touchViewTime;}
	//@}


  	/** @name methods to change the appearance of the shape
    */
	//@{
//  /// set the solid material
//  void setSolidMaterial(const Material &Mat) {_solidMaterial = Mat;}
//  /// get the solid material
//  const Material &getSolidMaterial(void) const {return _solidMaterial;}
//  /// set line size
//  virtual void setTransparency(float trans) {_solidMaterial.transparency = trans;}
//  /// get line Size
//  virtual float getTransparency(void) const {return _solidMaterial.transparency;}
//  /// set color
//  virtual void setColor(const Color &c) {_solidMaterial.diffuseColor = c;}
//  /// get color
//  virtual const Color &getColor(void) const {return _solidMaterial.diffuseColor;}
//  /// set the line material
//  void setLineMaterial(const Material &Mat) {_lineMaterial = Mat;}
//  /// get the line material
//  const Material &getLineMaterial(void) const {return _lineMaterial;}
//  /// set the line material
//  void setPointMaterial(const Material &Mat) {_pointMaterial = Mat;}
//  /// get the line material
//  const Material &getPointMaterial(void) const {return _pointMaterial;}
//  /// set point size
//  void setPointSize(float size) {_pointSize = size;}
//  /// get point Size
//  float getPointSize(void) const {return _pointSize;}
//  /// set line size
//  void setLineSize(float size) {_lineSize = size;}
//  /// get line Size
//  float getLineSize(void) const {return _lineSize;}
//  /// get show mode
//  const char* getShowMode(void) const {/*return _showMode.c_str();*/return showMode.getValue();}
//  /// get show mode
//  void setShowMode(const char* Mode) {/*_showMode = Mode;*/showMode.setValue(Mode);}

	//@}

	virtual Base::PyObjectBase *GetPyObject(void);


protected:
  /// get called befor the value is changed
  virtual void onBevorChange(const Property* prop);
  /// get called by the container when a Proptery was changed
  virtual void onChanged(const Property* prop);

 /** @name Material
  */
	//@{
//  Material    _solidMaterial;
//  Material    _lineMaterial;
//  float       _lineSize;
//  Material    _pointMaterial;
//  float       _pointSize;
	//@}

  Base::TimeInfo touchTime,touchViewTime,touchPropertyTime;


  DocumentObjectPy *pcObjectPy;

  App::Document* _pDoc;

};


} //namespace App



#endif
