/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2005     *
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

 


#ifndef _FeatureDataProvider_h_
#define _FeatureDataProvider_h_

#ifndef _PreComp_
# include <TDF_Label.hxx>
#endif


#include "../Base/Factory.h"
#include "../Base/PyExport.h"

/*
class TFunction_Logbook;
class PyObjectBase;
class FeaturePy;
class TopoDS_Shape;
*/

namespace App
{

class Property;
class Document;

/** Base class of all Feature classes in FreeCAD
 */
class AppExport FeatureDataProvider: public Base::PyHandler
{
public:
	/// Constructor
	FeatureDataProvider(void);
  virtual ~FeatureDataProvider() {}

	/** Init the Label the FeatureDataProvider is attached to
	 *  This methode will be called when the FeatureDataProvider is mounted 
	 *  to a Label in the document. It need to be overwriten in 
	 *  every FeatureDataProvider
	 */
	virtual void InitLabel(const TDF_Label &rcLabel)=0;

	/// Get called by the framework when the label is attached to the document
	void AttachLabel(const TDF_Label &rcLabel,Document*);

  /// Returns the Name/Type of the FeatureDataProvider
  virtual const char *Type(void)=0;

  // returns the python object of this FeatureDataProvider
	virtual Base::PyObjectBase *GetPyObject(void);

  /// returns the Label of this FeatureDataProvider
  TDF_Label Label(void)
  {
    return _cFeatureDataProviderLabel;
  }

  friend class FeaturePy;

protected:
	TDF_Label            _cFeatureDataProviderLabel;
  Document* _pDoc;
};


/** The FeatureFactory singleton
  */
class AppExport FeatureDataProviderFactorySingleton : public Base::Factory
{
public:
	static FeatureDataProviderFactorySingleton& Instance(void);
	static void Destruct (void);

    /// produce the widget using the factory
	FeatureDataProvider *Produce (const char* sName) const;

private:
	static FeatureDataProviderFactorySingleton* _pcSingleton;

	FeatureDataProviderFactorySingleton(){}
	~FeatureDataProviderFactorySingleton(){}
};

inline AppExport FeatureDataProviderFactorySingleton& FeatureFactory(void)
{
	return FeatureDataProviderFactorySingleton::Instance();
}

// --------------------------------------------------------------------

template <class CLASS>
class FeatureDataProviderProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		FeatureDataProviderProducer ()
		{
			App::FeatureDataProviderFactory().AddProducer(typeid(CLASS).name(), this);
		}

		virtual ~FeatureDataProviderProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}
};




} //namespace App



#endif
