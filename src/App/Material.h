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

 


#ifndef _Material_h_
#define _Material_h_

#ifndef _PreComp_
# include <TDF_Label.hxx>
#endif

namespace Base
{
  class PyObjectBase;
}

namespace App
{


/** Color class
 */
class AppExport Color
{
public:

  Color(float R=0.0,float G=0.0, float B=0.0, float A=0.0)
    :r(R),g(G),b(B),a(A){}
  Color(const Color& c)
    :r(c.r),g(c.g),b(c.b),a(c.a){}
  bool operator==(const Color& c)
  {
    return (c.r==r && c.g==g && c.b==b && c.a==a); 
  }
  
  bool operator!=(const Color& c){return !operator==(c);}

  void set(float R,float G, float B, float A=0.0)
  {
    r=R;g=G;b=B;a=A;
  }
  Color& operator=(const Color& c)
  {
    r=c.r;g=c.g;b=c.b;a=c.a;
    return *this;
  }

  /// color values, public accesible
  float r,g,b,a;
};

/** Material class
 */
class AppExport Material
{
public:
	/// Constructor
  Material(void){}
  Material(const char* MatName){set(MatName);}
  virtual ~Material() {}

	/** Set a material by name
	 *  There are some standard materials defined which are:
   *  - Gold
   *  - Stone
   *  The Color and the other properteis of the material are defined in the range [0-1].
	 */
  void set(const char* MatName);


  Base::PyObjectBase *Material::GetPyObject(void);

  Color ambientColor;
  Color diffuseColor;
  Color specularColor;
  Color emissiveColor;
  float shininess;
  float transparency;

};

} //namespace App

#endif
