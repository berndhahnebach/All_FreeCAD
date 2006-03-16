/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
# include <assert.h>
# include <string>
#endif

#include <stdlib.h>

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Builder3D.h"
#include "Exception.h"
#include "Vector3D.h"
#include "Console.h"

using namespace Base;


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
Builder3D::Builder3D()
:bStartEndOpen(false)
{
  result << "Separator { ";
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
Builder3D::~Builder3D()
{
}


//**************************************************************************
// points handling

/**
 * Starts the definition of point set with the given point size and color.
 * If posible make not to much startPoints() and endPoints() calls. Try to put all
 * points in one set. 
 * @see endPoints()
 * @param pointSize the point size in pixel the points are showed.
 * @param color_r red part of the point color (0.0 - 1.0).
 * @param color_g green part of the point color (0.0 - 1.0).
 * @param color_b blue part of the point color (0.0 - 1.0).
 */
void Builder3D::startPoints(short pointSize, float color_r,float color_g,float color_b)
{
 bStartEndOpen = true;
 result << "Separator { ";
 result <<   "Material { ";
 result <<     "diffuseColor " << color_r << " "<< color_g << " "<< color_b  ;
 result <<   "} ";
 result <<   "MaterialBinding { value PER_PART } ";
 result <<   "DrawStyle { pointSize " << pointSize << "} ";
 result <<   "Coordinate3 { ";
 result <<     "point [ ";

}

/// insert a point in an point set
void Builder3D::addPoint(float x, float y, float z)
{
  result << x << " " << y << " " << z << ",";
}


/// add a vector to a point set
void Builder3D::addPoint(const Vector3D &vec)
{
  addPoint(vec.x,vec.y,vec.z);
}
/**
 * Ends the point set operations and write the resulting inventor string.
 * @see startPoints()
 */
void Builder3D::endPoints(void)
{
  result  <<      "] ";
  result  <<     "} ";
  result  <<   "PointSet { } ";
  result  << "} ";
  bStartEndOpen = false;
}

void Builder3D::addSinglePoint(float x, float y, float z,short pointSize, float color_r,float color_g,float color_b)
{
  // addSinglePoint() not between startXXX() and endXXX() allowed
  assert( bStartEndOpen == false );

  result << "Separator { ";
  result <<   "Material { ";
  result <<     "diffuseColor " << color_r << " "<< color_g << " "<< color_b  ;
  result <<   "} ";
  result <<   "MaterialBinding { value PER_PART } ";
  result <<   "DrawStyle { pointSize " << pointSize << "} ";
  result <<   "Coordinate3 { ";
  result <<     "point [ ";
  result << x << " " << y << " " << z << ",";
  result <<      "] ";
  result <<     "} ";
  result <<   "PointSet { } ";
  result << "} ";

}

void Builder3D::addSinglePoint(const Base::Vector3D &vec, short pointSize, float color_r,float color_g,float color_b)
{
  addSinglePoint(vec.x, vec.y , vec.z, pointSize, color_r, color_g, color_b);
}

//**************************************************************************
// text handling


/**
 * Add a Text with a given position to the 3D set. The origion is the 
 * lower leftmost corner.
 * @param pos_x,pos_y,pos_z origin of the text
 * @param text the text to display.
 * @param color_r red part of the text color (0.0 - 1.0).
 * @param color_g green part of the text color (0.0 - 1.0).
 * @param color_b blue part of the text color (0.0 - 1.0).
 */
void Builder3D::addText(float pos_x, float pos_y , float pos_z,const char * text, float color_r,float color_g,float color_b)
{
  // addSinglePoint() not between startXXX() and endXXX() allowed
  assert( bStartEndOpen == false );

  result << "Separator { "  
         <<   "Material { diffuseColor " << color_r << " "<< color_g << " "<< color_b << "} " 
         <<   "Transform { translation " << pos_x << " "<< pos_y << " "<< pos_z << "} "
         <<   "Text2 { string \" " << text << "\" " << "} "
         << "} ";

}

void Builder3D::addText(const Base::Vector3D &vec,const char * text, float color_r,float color_g,float color_b)
{
  addText(vec.x, vec.y , vec.z,text, color_r,color_g,color_b);
}

void Builder3D::addText(const Base::Vector3D &vec, float color_r, float color_g, float color_b, const char * format, ...)
{
  // temp buffer
  char* txt = (char*) malloc(strlen(format)+4024);

  va_list namelessVars;
  va_start(namelessVars, format);  // Get the "..." vars
  vsprintf(txt, format, namelessVars);
  va_end(namelessVars);

  addText(vec, txt, color_r, color_g, color_b);
}

void Builder3D::clear ()
{
  // Under VC6 string::clear() doesn't exist, under gcc stringstream::str() returns a copy not a reference
#if defined(_MSC_VER) && _MSC_VER >= 1400
  result.str().clear();
#endif
  result.clear();
}

//**************************************************************************
// line/arrow handling

void Builder3D::addSingleLine(Vector3D pt1, Vector3D pt2, short lineSize, float color_r,float color_g,float color_b, unsigned short linePattern)
{
  char lp[20];
  sprintf(lp, "0x%x", linePattern);
  //char lp[20] = "0x";
  //itoa(linePattern, buf, 16);
  //strcat(lp, buf);

  result << "Separator { "
         <<   "Material { diffuseColor " << color_r << " "<< color_g << " "<< color_b << "} " 
         <<   "DrawStyle { lineWidth " << lineSize << " linePattern " << lp << " } "
         <<   "Coordinate3 { "
         <<     "point [ "
         <<        pt1.x << " " << pt1.y << " " << pt1.z << ","
         <<        pt2.x << " " << pt2.y << " " << pt2.z 
         <<     "] "
         <<   "} "
         <<   "LineSet { } "
         << "} ";
}

void Builder3D::addSingleArrow(Vector3D pt1, Vector3D pt2, short lineSize, float color_r,float color_g,float color_b, unsigned short linePattern)
{
    float pi = 3.1415926535f;
    float pih = pi / 2.0f;
    float pi2 = pi * 2.0f;
    float l = (pt2 - pt1).Length();
    float cl = l / 10.0f;
    float cr = cl / 2.0f;

    Vector3D dir = pt2 - pt1;
    dir.Normalize();
    dir.Scale(l-cl, l-cl, l-cl);
    Vector3D pt2s = pt1 + dir;
    dir.Normalize();
    dir.Scale(l-cl/2.0f, l-cl/2.0f, l-cl/2.0f);
    Vector3D cpt = pt1 + dir;

    Vector3D rot = Vector3D(0.0f, 1.0f, 0.0f) % dir;
    rot.Normalize();
    float a = Vector3D(0.0f, 1.0f, 0.0f).GetAngle(dir);

    result << "Separator { "
         <<   "Material { diffuseColor " << color_r << " "<< color_g << " "<< color_b << "} " 
         <<   "DrawStyle { lineWidth " << lineSize << "} "
         <<   "Coordinate3 { "
         <<     "point [ "
         <<        pt1.x << " " << pt1.y << " " << pt1.z << ","
         <<        pt2s.x << " " << pt2s.y << " " << pt2s.z 
         <<     "] "
         <<   "} "
         <<   "LineSet { } "
         <<   "Transform { "
         <<     "translation " << cpt.x << " " << cpt.y << " " << cpt.z << " "
         <<     "rotation " << rot.x << " " << rot.y << " " << rot.z << " " << a
         <<   "} "
         <<   "Cone { bottomRadius " << cr << " height " << cl << "} "
         << "} ";

}

//**************************************************************************
// triangle handling

void Builder3D::addSingleTriangle(Vector3D pt0, Vector3D pt1, Vector3D pt2, bool filled, short lineSize, float color_r, float color_g, float color_b)
{
  std::string fs = "";
  if (filled)
  {
    fs = "IndexedFaceSet { coordIndex[ 0, 1, 2, -1 ] } ";
  }

    result << "Separator { "
         <<   "Material { diffuseColor " << color_r << " "<< color_g << " "<< color_b << "} " 
         <<   "DrawStyle { lineWidth " << lineSize << "} "
         <<   "Coordinate3 { "
         <<     "point [ "
         <<        pt0.x << " " << pt0.y << " " << pt0.z << ","
         <<        pt1.x << " " << pt1.y << " " << pt1.z << ","
         <<        pt2.x << " " << pt2.y << " " << pt2.z << ","
         <<     "] "
         <<   "} "
         <<   "LineSet { } "
         <<   fs
         << "} ";

}

//**************************************************************************
// output handling

/**
 * Save the resulting inventor 3D representation to the Console().Log() facility.
 * In DEBUG mode the Gui (if runing) will trigger on that and show the representation in 
 * the active Viewer/Document. It shows only one representation on time. If you need to 
 * show more then one representation use saveToFile() instead.
 * @see saveToFile()
 */
void Builder3D::saveToLog(void)
{
  result <<   "} ";
  Console().Log("Vdbg: %s \n",result.str().c_str());
}

/**
 * Save the resulting inventor 3D representation to a file. Ending shut be *.iv.
 * That eneables you to show the result in a Inventor Viewer or in FreeCAD by:
 * /code
 * Gui.document().addAnnotation("Debug","MyFile.iv")
 * /endcode
 * @see saveToFile()
 */
void Builder3D::saveToFile(const char* FileName)
{
  result <<   "} ";
  std::ofstream  file(FileName);
  if(!file) 
    throw Exception("Builder3D::saveToFile(): Can not open file...");

  file << "#Inventor V2.1 ascii " << std::endl;
  file << result.str();
}


