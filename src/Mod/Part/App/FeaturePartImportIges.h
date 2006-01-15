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



#ifndef __FeaturePartImportIges_H__
#define __FeaturePartImportIges_H__


#include "PartFeature.h"

namespace Part
{


class FeaturePartImportIges :public PartFeature
{
  PROPERTY_HEADER(Part::FeaturePartImportIges);

public:
  FeaturePartImportIges();

  App::PropertyString FileName;

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  /// Returns the Name/Type of the feature
  virtual const char *type(void){return "PartImportIges";};
  //@}
};



}




#endif // __FeaturePartImportIges_H__
