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
  explicit Color(float R=0.0,float G=0.0, float B=0.0, float A=0.0)
    :r(R),g(G),b(B),a(A){}
  Color(unsigned long rgba)
  { setPackedValue( rgba ); }
  Color(const Color& c)
    :r(c.r),g(c.g),b(c.b),a(c.a){}

  bool operator==(const Color& c) const
  {
    return (c.r==r && c.g==g && c.b==b && c.a==a); 
  }
  bool operator!=(const Color& c) const 
  {
    return !operator==(c);
  }
  void set(float R,float G, float B, float A=0.0)
  {
    r=R;g=G;b=B;a=A;
  }
  Color& operator=(const Color& c)
  {
    r=c.r;g=c.g;b=c.b;a=c.a;
    return *this;
  }

  /**
   * Sets the color value as a 32 bit combined red/green/blue/alpha value.
   * Each component is 8 bit wide (i.e. from 0x00 to 0xff), and the red
   * value should be stored leftmost, like this: 0xRRGGBBAA.
   *
   * \sa getPackedValue().
   */
  Color& setPackedValue( unsigned long rgba)
  {
    this->set( (rgba >> 24)/255.0f, ((rgba >> 16)&0xff)/255.0f, ((rgba >> 8)&0xff)/255.0f, (rgba&0xff)/255.0f );
    return *this;
  }

  /**
   * Returns color as a 32 bit packed unsigned long in the form 0xRRGGBBAA.
   *
   *  \sa setPackedValue().
   */
  unsigned long getPackedValue() const
  {
    return ((unsigned long)(r*255.0f + 0.5f) << 24 |
            (unsigned long)(g*255.0f + 0.5f) << 16 |
            (unsigned long)(b*255.0f + 0.5f) << 8  |
            (unsigned long)(a*255.0f + 0.5f));
  }

  /// color values, public accesible
  float r,g,b,a;
};

/** Material class
 */
class MaterialPy;
class AppExport Material
{
public:
	/// Constructor
  Material(void);
  Material(const char* MatName);
  virtual ~Material();

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

private:
  MaterialPy* _materialPy;
};

} //namespace App

#endif
