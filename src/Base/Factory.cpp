/** \file Factory.cpp
 *  \brief 
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  
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


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <list>
#endif


#include "Factory.h"
#include "Console.h"

using namespace Base;


Factory::~Factory ()
{
#ifdef _MSC_VER
# if _MSC_VER >= 1300
  for (std::map<std::string, AbstractProducer*>::iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
# else
  for (std::map<const std::string, AbstractProducer*>::iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
# endif
#else
  for (std::map<const std::string, AbstractProducer*>::iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
#endif
    delete pI->second;
}

void* Factory::Produce (const char *sClassName) const
{
#ifdef _MSC_VER
# if _MSC_VER >= 1300
  std::map<std::string, AbstractProducer*>::const_iterator pProd;
# else
  std::map<const std::string, AbstractProducer*>::const_iterator pProd;
# endif
#else
  std::map<const std::string, AbstractProducer*>::const_iterator pProd;
#endif

  pProd = _mpcProducers.find(sClassName);
  if (pProd != _mpcProducers.end())
    return pProd->second->Produce();
  else
    return NULL;
}

void Factory::AddProducer (const char *sClassName, AbstractProducer *pcProducer)
{
  _mpcProducers[sClassName] = pcProducer;
}

bool Factory::CanProduce(const char* sClassName) const
{
	return (_mpcProducers.find(sClassName) != _mpcProducers.end());
}

std::list<std::string> Factory::CanProduce() const
{
	std::list<std::string> lObjects;

#ifdef _MSC_VER
# if _MSC_VER >= 1300
  for (std::map<std::string, AbstractProducer*>::const_iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
# else
  for (std::map<const std::string, AbstractProducer*>::const_iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
# endif
#else
  for (std::map<const std::string, AbstractProducer*>::const_iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
#endif
	{
		lObjects.push_back(pI->first);
	}

	return lObjects;
}

// ----------------------------------------------------

ScriptFactorySingleton* ScriptFactorySingleton::_pcSingleton = NULL;



ScriptFactorySingleton& ScriptFactorySingleton::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new ScriptFactorySingleton;
  return *_pcSingleton;
}

void ScriptFactorySingleton::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

const char* ScriptFactorySingleton::ProduceScript (const char* sScriptName) const
{
	const char* script = (const char*)Produce(sScriptName);

	if ( !script )
	{
#ifdef FC_DEBUG
		Console().Warning("\"%s\" is not registered\n", sScriptName);
#endif
		return ""; // no data
	}

	return script;
}
