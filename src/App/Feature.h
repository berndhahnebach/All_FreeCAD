/** \file $RCSfile$
 *  \brief The attribute module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 


#ifndef _Feature_h_
#define _Feature_h_

#include "../Base/Factory.h"


class TFunction_Logbook;

class FCPyObject;


namespace App
{


/** Base class of all Feature classes in FreeCAD
 */
class AppExport Feature
{
protected:

	/** @name methodes used for reclculation (update) */
	//@{
	/** MustExecute
	 *  We call this method to check if the object was modified to
	 *  be invoked. If the object label or an argument is modified,
	 *  we must recompute the object - to call the method Execute().
	 */
	virtual bool MustExecute(const TFunction_Logbook& log) const =0;

	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed, if there are no any mistakes occurred we return 0:
	 *  0 - no mistakes were found.
	 */
	virtual Standard_Integer Execute(TFunction_Logbook& log) const=0;

	/** Validate
	 * Validation of the object label, its arguments and its results.
	 */
	virtual void Validate(TFunction_Logbook& log) const=0;
	//@}

	virtual FCPyObject *GetPyObject(void);


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
			FeatureFactory::Instance().AddProducer(typeid(CLASS).name(), this);
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
