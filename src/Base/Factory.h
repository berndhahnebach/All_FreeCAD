/** \file Factory.h
 *  \brief 
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#ifndef _FACTORY_H_
#define _FACTORY_H_

#include<typeinfo>
#include<string>
#include<map>
#include"../Config.h"

/// Abstract base class of all producers
class  BaseExport FCAbstractProducer
{ 
    public:
    virtual void* Produce (void) const = 0;
};


 
/** The Factory singelton
  * This class has the purpos to produce at runtime instances of 
  * of class not known at compile time. It holds a map of so called
  * producers which are abel to pruce an instance of a special class.
  * Producer can be registert by runtime through e.g. Application modueles
  */
class BaseExport FCFactory
{
public:
   /// produce a class with the given name
   void* Produce (const char* sClassName) const;
   /// Adds a new prducer instance
   void AddProducer (const char* sClassName, FCAbstractProducer *pcProducer);
   /// destruction 
   ~FCFactory ();

   static FCFactory& Instance(void);
   static void Destruct (void);
   
private:
   static FCFactory* _pcSingleton;
   
protected:
#	pragma warning( disable : 4251 )
# if _MSC_VER >= 1300
   std::map<std::string, FCAbstractProducer*> _mpcProducers;
# else
   std::map<const std::string, FCAbstractProducer*> _mpcProducers;
# endif
   
   FCFactory (void){}

};

inline BaseExport FCFactory& GetFactory(void)
{
	return FCFactory::Instance();
}

/** Producer template class
  * this is a conviniance template to produce very easy a pruducer instance
  * for the use in the FCFactory class.
  * \code
  * \endcode
  * @see FCFactory
  */
template <class CLASS>
# if _MSC_VER >= 1300
class FCFactoryProducer: public FCAbstractProducer
# else
class BaseExport FCFactoryProducer: public FCAbstractProducer
# endif
{
															  /// Constructor
public:
	FCFactoryProducer (void)
	{
		FCFactory::Instance().AddProducer(typeid(CLASS).name(), this);
	}

	virtual ~FCFactoryProducer (void){}

	/// Produce a instance
	virtual void* Produce (void) const
	{ 
	  return (void*)(new CLASS);
	}

};

#endif

