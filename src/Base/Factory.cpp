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
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else

#endif

#include "Factory.h"


FCFactory* FCFactory::_pcSingleton = NULL;



FCFactory& FCFactory::Instance(void)
{
  if (_pcSingleton == NULL)
    _pcSingleton = new FCFactory;
  return *_pcSingleton;
}
/// destruction of singelton only needed for debuging
void FCFactory::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

FCFactory::~FCFactory ()
{
  for (std::map<const std::string, FCAbstractProducer*>::iterator pI = _mpcProducers.begin(); pI != _mpcProducers.end(); pI++)
    delete pI->second;
}

void* FCFactory::Produce (const char *sClassName) const
{
  std::map<const std::string, FCAbstractProducer*>::const_iterator pProd;

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

