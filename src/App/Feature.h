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

#include "../Base/Factory.h"
#include "../Base/PyExport.h"


class TFunction_Logbook;
class FCPyObject;
class FeaturePy;
class TopoDS_Shape;

namespace App
{

class Property;
class Document;

/** Base class of all Feature classes in FreeCAD
 */
class AppExport Feature: public Base::PyHandler
{
public:
	/// Constructor
	Feature(void);
  virtual ~Feature() {}

	/** Init the Label the Feature is attached to
	 *  This methode will be called when the Feature is mounted 
	 *  to a Label in the document. It need to be overwriten in 
	 *  every Feature
	 */
	virtual void InitLabel(const TDF_Label &rcLabel)=0;

	/// Get called by the framework when the label is attached to the document
	void AttachLabel(const TDF_Label &rcLabel);

	/** @name methodes used for recalculation (update) */
	//@{
	/** MustExecute
	 *  We call this method to check if the object was modified to
	 *  be invoked. If the object label or an argument is modified,
	 *  we must recompute the object - to call the method Execute().
	 */
	virtual bool MustExecute(const TFunction_Logbook& log);

	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed
	 *  0 - no mistakes were found.
	 */
	virtual Standard_Integer Execute(TFunction_Logbook& log)=0;

	/** Validate
	 * Validation of the object label, its arguments and its results.
	 */
	virtual void Validate(TFunction_Logbook& log)=0;
	//@}


	/** @name methodes for conviniant handling of Parameter (Properties) */
	//@{
	/** Insert Property
	 *  Call this method to add a Property to the Feature
	 */
	void AddProperty(const char *Type, const char *Name, const char *InitString);

	/** Get a Property of this Feature
	 *  Call this method to get a Property of this Feature
	 */
	Property &GetProperty(const char *Name);

	/** Get a Property and put it to float
	 *  This works with all Properties inhereting from Float and Int Properties..
	 */
	double GetFloatProperty(const char *Name);
  
	/** Get a Property and put it to float
	 *  This works with all Properties inhereting from Float and Int Properties..
	 */
	const char *GetStringProperty(const char *Name);

  /** Set the property touched -> changed, cause recomputation in Update()
	 *  
	 */
	void TouchProperty(const char *Name);
	//@}

  /// Retrive the Feature from a Document label
  static Feature *GetFeature(const TDF_Label &l);


  void SetDoc(Document*);

  void SetShape(TopoDS_Shape &Shape);

	virtual Base::FCPyObject *GetPyObject(void);

  TDF_Label Label(void)
  {
    return _cFeatureLabel;
  }

  friend class FeaturePy;

protected:
	TDF_Label            _cFeatureLabel;
	int                  _nextFreeLabel;
  std::map<std::string,int> _PropertiesMap;

  Document* _pDoc;
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
