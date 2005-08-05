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
# include <TDF_Label.hxx>
#endif

#include <Base/Factory.h>
#include <Base/PyExport.h>

#include <App/Material.h>


class TFunction_Logbook;
class PyObjectBase;
class TopoDS_Shape;
class TDF_Label;
//class Standard_Integer;

namespace App
{

//class FeaturePy;
class Property;
class Document;
class FeatureDataProvider;
class FeaturePy;

/** Base class of all Feature classes in FreeCAD
 */
class AppExport Feature: public Base::PyHandler
{
public:
	/// Constructor
	Feature(void);
  virtual ~Feature();

	/** @name methodes to overide with a new feature type */
	//@{
	/** Init the Label the Feature is attached to
	 *  This methode will be called when the Feature is mounted 
	 *  to a Label in the document. It need to be overwriten in 
	 *  every Feature
	 */
	virtual void initFeature(void)=0;

	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed
	 *  0 - no mistakes were found.
	 */
	virtual int execute(TFunction_Logbook& log)=0;

  /// Returns the Name/Type of the feature
  virtual const char *type(void)=0;
  //@}


	/** @name status methodes of the feature */
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
  void setError(const char* s);
  /// checks if valid
  bool isValid(void){return _eStatus == Valid;}
  /// Recompute only this feature and makes it valid again
  void recompute(void);
  //@}


public:

	/** @name methodes for conviniant handling of Parameter (Properties) */
	//@{

  /** Insert Property
	 *  Call this method to add a Property to the Feature
	 */
	void addProperty(const char *Type, const char *Name);

  /** get Property string representation
 	 *  
	 */
  //std::string getPropertyAsString(const char *Name){};

  /** get Property type
 	 *  
	 */
  const char *getPropertyType(const char *Name);

	/** Get a Property of this Feature
	 *  Call this method to get a Property of this Feature
	 */
	//Property &GetProperty(const char *Name);

	/** Insert a Link Property
	 *  Call this method to add a Link to an other Feature to that Feature
	 */
	void setPropertyLink(Feature *pcToLink, const char *Name);

	/** get a Link Property
	 *  Call this method to get a Link to an other Feature 
	 */
	Feature *getPropertyLink(const char *Name);

  /** Get a Property and put it to float
	 *  This works with all Properties inhereting from Float and Int Properties..
	 */
	double getPropertyFloat(const char *Name);

  /** set a float Property 
	 *  ..
	 */
	void setPropertyFloat(double, const char *Name);
 
 	long getPropertyInt(const char *Name);
	void setPropertyInt(long, const char *Name);
  std::string getPropertyString(const char *Name);
	void setPropertyString(const char*, const char *Name);


	/** Get a Property and put it to float
	 *  This works with all Properties inhereting from Float and Int Properties..
	 */
	//const char *GetStringProperty(const char *Name);

  /** Set the property touched -> changed, cause recomputation in Update()
	 *  
	 */
	void TouchProperty(const char *Name);
  /// set this feature touched (cause recomputation
	void Touch(void);
	//@}


	/** @name methodes for conviniant handling of DataProvider
    * This functionality is sceduled for 0.3
    * @see FeatureDataProvider
    */
	//@{
  /// Adds a new DataProvider
  FeatureDataProvider *AddDataProvider(const char* Name);

	//@}

	/** @name methodes to change the apperance of the shape
    */
	//@{
  /// set the solid material
  void setSolidMaterial(const Material &Mat) {_solidMaterial = Mat;}
  /// get the solid material
  const Material &getSolidMaterial(void) const {return _solidMaterial;}
  /// set line size
  void setSolidTransparency(float trans) {_solidMaterial.transparency = trans;}
  /// get line Size
  float getSolidTransparency(void) const {return _solidMaterial.transparency;}
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

  /// Retrive the Feature from a Document label
  static Feature *GetFeature(const TDF_Label &l);

  const char *getName(void){return _Name.c_str();}

	virtual Base::PyObjectBase *GetPyObject(void);

  TDF_Label Label(void)
  {
    return _cFeatureLabel;
  }

  friend class FeaturePy;
  friend class Document;

protected:
	/** @name methodes used for recalculation and document handling
    *  this methodes are only called/used by the document
    */
	//@{
	/** MustExecute
	 *  We call this method to check if the object was modified to
	 *  be invoked. If the object label or an argument is modified.
	 *  If we must recompute the object - to call the method Execute().
	 */
	virtual bool MustExecute(void);

  // remove all modifikations from the property labels
  void removeModifications(void);

  /// Get called by the framework when the label is attached to the document
	void AttachLabel(const TDF_Label &rcLabel,Document*);

	/** Validate
	 * Validation of the object label, its arguments and its results.
	 */
//	virtual void Validate(TFunction_Logbook& log)=0;
	//@}

  // Material stuff
  Material    _solidMaterial;
  Material    _lineMaterial;
  float       _lineSize;
  Material    _pointMaterial;
  float       _pointSize;
  std::string _showMode;

	TDF_Label            _cFeatureLabel;
	int                  _nextFreeLabel;
  std::map<std::string,int> _PropertiesMap;

  Document* _pDoc;

  FeaturePy* pcFeaturePy;

  std::string _Name;

  Status _eStatus;
  std::string _cErrorMessage;

};


/** The FeatureFactory singleton
  */
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




} //namespace App



#endif
