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


#include "Factory.h"
#include "Console.h"


FCFactory::~FCFactory ()
{
# if _MSC_VER >= 1300
  for (std::map<std::string, FCAbstractProducer*>::iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
# else
  for (std::map<const std::string, FCAbstractProducer*>::iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
# endif
    delete pI->second;
}

void* FCFactory::Produce (const char *sClassName) const
{
# if _MSC_VER >= 1300
  std::map<std::string, FCAbstractProducer*>::const_iterator pProd;
# else
  std::map<const std::string, FCAbstractProducer*>::const_iterator pProd;
# endif

  pProd = _mpcProducers.find(sClassName);
  if (pProd != _mpcProducers.end())
    return pProd->second->Produce();
  else
    return NULL;
}

void FCFactory::AddProducer (const char *sClassName, FCAbstractProducer *pcProducer)
{
  _mpcProducers[sClassName] = pcProducer;
}

// ----------------------------------------------------

FCScriptFactory* FCScriptFactory::_pcSingleton = NULL;



FCScriptFactory& FCScriptFactory::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FCScriptFactory;
  return *_pcSingleton;
}

void FCScriptFactory::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

const char* FCScriptFactory::ProduceScript (const char* sScriptName) const
{
	const char* script = (const char*)Produce(sScriptName);

	if ( !script )
	{
#ifdef FC_DEBUG
		GetConsole().Warning("\"%s\" is not registered\n", sScriptName);
#endif
		return ""; // no data
	}

	return script;
}
