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
#include<list>
#include"../Config.h"

/// Abstract base class of all producers
class BaseExport FCAbstractProducer
{
	public:
		virtual void* Produce (void) const = 0;
};


 
/**
  * This class has the purpose to produce at runtime instances 
  * of classes not known at compile time. It holds a map of so called
  * producers which are able to produce an instance of a special class.
  * Producer can be registered at runtime through e.g. application modules
  */
class BaseExport FCFactory
{
	public:
		/// Adds a new prducer instance
		void AddProducer (const char* sClassName, FCAbstractProducer *pcProducer);
		bool CanProduce(const char* sClassName) const;
		std::list<std::string> CanProduce() const;

	protected:
		/// produce a class with the given name
		void* Produce (const char* sClassName) const;

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#endif
#ifdef _MSC_VER
# if _MSC_VER >= 1300
	   std::map<std::string, FCAbstractProducer*> _mpcProducers;
# else
		 std::map<const std::string, FCAbstractProducer*> _mpcProducers;
# endif
#else
		 std::map<const std::string, FCAbstractProducer*> _mpcProducers;
#endif
   
		/// construction
		FCFactory (void){}
		/// destruction 
		virtual ~FCFactory ();
};

// --------------------------------------------------------------------

/** The ScriptFactory singleton
  */
class BaseExport FCScriptFactory : public FCFactory
{
	public:
		static FCScriptFactory& Instance(void);
		static void Destruct (void);

		const char* ProduceScript (const char* sScriptName) const;

	private:
		static FCScriptFactory* _pcSingleton;

		FCScriptFactory(){}
		~FCScriptFactory(){}
};

inline BaseExport FCScriptFactory& GetScriptFactory(void)
{
	return FCScriptFactory::Instance();
}

// --------------------------------------------------------------------

/** Producer template class
  * this is a convenience template to produce very easy a producer instance
  * for the use in the FCFactory class.
  * \code
  * \endcode
  * @see FCFactory
  */
class BaseExport FCScriptProducer: public FCAbstractProducer
{
public:
	/// Constructor
	FCScriptProducer (const char* name, const char* script) : mScript(script)
	{
		FCScriptFactory::Instance().AddProducer(name, this);
	}

	virtual ~FCScriptProducer (void){}

	/// Produce an instance
	virtual void* Produce (void) const
	{ 
	  return (void*)mScript;
	}

	private:
		const char* mScript;
};

#endif

