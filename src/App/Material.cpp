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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "../Base/Exception.h"
#include "Material.h"

using namespace App;


//===========================================================================
// Material
//===========================================================================

void Material::set(const char* MatName)
{
  if(strcmp("Gold",MatName) == 0 ){
    ambientColor.set(0.3f,0.1f,0.1f);
    diffuseColor.set(0.8f,0.7f,0.2f);
    specularColor.set(0.4f,0.3f,0.1f);
    shininess = .4f;
    transparency = .0f;
  }else if(strcmp("Stone",MatName) == 0 ){
    ambientColor.set(0.0f,0.0f,0.0f);
    diffuseColor.set(0.0f,0.0f,0.0f);
    specularColor.set(0.4f,0.3f,0.1f);
    shininess = .4f;
    transparency = .0f;
  }else{
    throw Base::Exception("Unknown material");
  }
}
