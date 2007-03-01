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

  PropertyString name;

  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void) const {
    return "";
  }
	/// Constructor
	DocumentObject(void);
  virtual ~DocumentObject();

  App::Document &getDocument(void) const;
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
  Base::TimeInfo getTouchTime(void) const {return touchTime;}
  /// get the view touch time
  Base::TimeInfo getTouchViewTime(void) const {return touchViewTime;}
	//@}

	virtual Base::PyObjectBase *GetPyObject(void);

protected:
  /// get called befor the value is changed
  virtual void onBevorChange(const Property* prop);
  /// get called by the container when a Proptery was changed
  virtual void onChanged(const Property* prop);


  Base::TimeInfo touchTime,touchViewTime,touchPropertyTime;

  DocumentObjectPy *pcObjectPy;
  App::Document* _pDoc;
};


} //namespace App



#endif
