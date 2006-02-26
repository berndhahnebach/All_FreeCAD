/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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




#ifndef _Feature_h_
#define _Feature_h_

#ifndef _PreComp_
# include <string>
# include <vector>
#endif

#include <Base/Factory.h>
#include <Base/PyExport.h>
#include <Base/TimeInfo.h>

#include <App/DocumentObject.h>
#include <App/Material.h>
#include <App/PropertyStandard.h>

class TFunction_Logbook;
class PyObjectBase;
class TopoDS_Shape;
class TDF_Label;
//class Standard_Integer;

namespace Base
{
  class Vector3D;
  class Matrix4D;
}

using Base::Vector3D;
using Base::Matrix4D;

namespace App
{

//class FeaturePy;
class Property;
class Document;
class FeatureDataProvider;
class FeaturePy;

/** Base class of all Feature classes in FreeCAD
 */
class AppExport Feature: public App::DocumentObject
{
    PROPERTY_HEADER(App::Feature);

public:


  PropertyString name;

	/// Constructor
	Feature(void);
  virtual ~Feature();

  virtual void onChanged(Property* prop);


	/** @name methods to overide with a new feature type */
	//@{

	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed
	 *  0 - no mistakes were found.
	 */
	virtual int execute(void)=0;

  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void){return "";}

  //@}

	/** @name status methods of the feature */
	//@{
  /// status types
  enum Status 
  {
    Valid,    /**< enum The Feature is Valid */
    New,      /**< enum The Feature is new */
    Inactive ,/**< enum Will not recalculated */
    Recompute,/**< enum Fetature is in recomputation*/
    Error     /**< enum Feture is in error state */
  };

  /// gets the status
  Status getStatus(void) const { return _eStatus;}
  /// set the status, e.g. after recoputation in Execute()
 // void setStatus(const Status &s){_eStatus = s;}
  /// get the status Message
  const char *getStatusString(void) const;//{return _cStatusMessage.c_str();}
  /// get the error Message (if any)
  const char *getErrorString(void) const{return _cErrorMessage.c_str();}
  /// set an error on recoputation
  void setError(const char* pMsg,...);
  /// checks if valid
  bool isValid(void){return _eStatus == Valid;}
  /// Recompute only this feature and makes it valid again
  void recompute(void);
  //@}


  /** Set the property touched -> changed, cause recomputation in Update()
	 *  
	 */
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


	/** @name methods to change the apperance of the shape
    */
	//@{
  /// set the solid material
  void setSolidMaterial(const Material &Mat) {_solidMaterial = Mat;}
  /// get the solid material
  const Material &getSolidMaterial(void) const {return _solidMaterial;}
  /// set line size
  virtual void setTransparency(float trans) {_solidMaterial.transparency = trans;}
  /// get line Size
  virtual float getTransparency(void) const {return _solidMaterial.transparency;}
  /// set color
  virtual void setColor(const Color &c) {_solidMaterial.diffuseColor = c;}
  /// get color
  virtual const Color &getColor(void) const {return _solidMaterial.diffuseColor;}
  /// set the line material
  void setLineMaterial(const Material &Mat) {_lineMaterial = Mat;}
  /// get the line material
  const Material &getLineMaterial(void) const {return _lineMaterial;}
  /// set the line material
  void setPointMaterial(const Material &Mat) {_pointMaterial = Mat;}
  /// get the line material
  const Material &getPointMaterial(void) const {return _pointMaterial;}
  /// set point size
  void setPointSize(float size) {_pointSize = size;}
  /// get point Size
  float getPointSize(void) const {return _pointSize;}
  /// set line size
  void setLineSize(float size) {_lineSize = size;}
  /// get line Size
  float getLineSize(void) const {return _lineSize;}
  /// get show mode
  const char* getShowMode(void) const {return _showMode.c_str();}
  /// get show mode
  void setShowMode(const char* Mode) {_showMode = Mode;}

	//@}


	virtual Base::PyObjectBase *GetPyObject(void);

  friend class FeaturePy;
  friend class Document;

	/** MustExecute
	 *  We call this method to check if the object was modified to
	 *  be invoked. If the object label or an argument is modified.
	 *  If we must recompute the object - to call the method Execute().
	 */
	virtual bool MustExecute(void);

protected:
	/** @name methods used for recalculation and document handling
    *  this methods are only called/used by the document
    */
	//@{

  // remove all modifikations from the property labels
  void removeModifications(void);

  /// Get called by the framework when the label is attached to the document
	void AttachLabel(const TDF_Label &rcLabel,Document*);

	//@}

	/** @name Material
    */
	//@{
  Material    _solidMaterial;
  Material    _lineMaterial;
  float       _lineSize;
  Material    _pointMaterial;
  float       _pointSize;
  std::string _showMode;
	//@}

  Base::TimeInfo touchTime,touchViewTime,touchPropertyTime;

	//TDF_Label            _cFeatureLabel;
	int                  _nextFreeLabel;

  struct FeatEntry {
    int Label;
    Base::TimeInfo T;
  };
  
  std::map<std::string,FeatEntry> _PropertiesMap;

  FeaturePy* pcFeaturePy;

  //std::string _Name;

  Status _eStatus;
  std::string _cErrorMessage;

};


/** The FeatureFactory singleton
  */

  /*
class AppExport FeatureFactorySingleton : public Base::Factory
{
public:
	static FeatureFactorySingleton& Instance(void);
	static void Destruct (void);

    /// produce the widget using the factory
	Feature *Produce (const char* sName) const;

private:
	static FeatureFactorySingleton* _pcSingleton;

	FeatureFactorySingleton(){}
	~FeatureFactorySingleton(){}
};

inline AppExport FeatureFactorySingleton& FeatureFactory(void)
{
	return FeatureFactorySingleton::Instance();
}

// --------------------------------------------------------------------

template <class CLASS>
class FeatureProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		FeatureProducer ()
		{
			App::FeatureFactory().AddProducer(typeid(CLASS).name(), this);
		}

		virtual ~FeatureProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}
};


*/

} //namespace App



#endif
