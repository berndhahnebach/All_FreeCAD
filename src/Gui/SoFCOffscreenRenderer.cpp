/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <stdlib.h>
# include <qimage.h>
# include <qstrlist.h>
# include <Inventor/lists/SbList.h>
# include <Inventor/lists/SbPList.h>
# include <Inventor/SbString.h>
#endif

#include <Inventor/C/basic.h>
#include "SoFCOffscreenRenderer.h"

using namespace Gui;


#define MAX_EXT_LEN 255

static int qimage_set_save_format(const char * ext, char * buf)
{
  strncpy(buf, ext, MAX_EXT_LEN);
  buf[MAX_EXT_LEN] = 0;
  
  int i = 0;
  // convert to upper case
  while (buf[i] != 0) {
    buf[i] = toupper(buf[i]);
    i++;
  }
  
  // Qt specifies the jpg extension as JPEG
  if (strcmp(buf, "JPG") == 0) strcpy(buf, "JPEG");
  
  QStrList olist = QImage::outputFormats();
  const char * qtext = olist.getFirst();
  while (qtext) {
    if (strcmp(buf, qtext) == 0) return 1;
    qtext = olist.next(); 
  }
  // clear save format
  buf[0] = 0;
  return 0;
}

// ---------------------------------------------------------------

SoFCOffscreenRenderer::SoFCOffscreenRenderer (const SbViewportRegion &viewportregion)
  : SoOffscreenRenderer(viewportregion)
{
}

SoFCOffscreenRenderer::SoFCOffscreenRenderer (SoGLRenderAction *action)
  : SoOffscreenRenderer(action)
{
}

SoFCOffscreenRenderer::~SoFCOffscreenRenderer()
{
}

SbBool 	SoFCOffscreenRenderer::writeToImage (QImage& img, const char * filetypeext) const
{
  char ext[MAX_EXT_LEN+1];  
  qimage_set_save_format(filetypeext, ext);  
  if (ext[0] == 0) {
    return false;
  }

	const unsigned char * bytes = getBuffer();
  SbVec2s size = getViewportRegion().getViewportSizePixels();
  int numcomponents = (int) this->getComponents();
	int width  = (int)size[0];
	int height = (int)size[1];;

  QImage image(width, height, 32);
  if (numcomponents == 2 || numcomponents == 4) image.setAlphaBuffer(TRUE);
  else image.setAlphaBuffer(FALSE);
  QRgb * bits = (QRgb*) image.bits();
  
  for (int y = 0; y < height; y++) {
    const unsigned char * line = 
      &bytes[width*numcomponents*(height-(y+1))];
    for (int x = 0; x < width; x++) {
      switch (numcomponents) {
      default:
      case 1:
        *bits++ = qRgb(line[0], line[0], line[0]);
        break;
      case 2:
        *bits++ = qRgba(line[0], line[0], line[0], line[1]);
        break;
      case 3:
        *bits++ = qRgb(line[0], line[1], line[2]);
        break;
      case 4:
        *bits++ = qRgba(line[0], line[1], line[2], line[3]);
        break;
      }
      line += numcomponents;
    }
  }

	img=image;

  return true;
}

SbBool SoFCOffscreenRenderer::writeToImageFile (const SbString&  filename, const SbName &  filetypeextension ) const
{
  if ( isWriteSupported( filetypeextension ) )
  {
    return writeToFile( filename, filetypeextension );
  }
  else if ( strcmp(filetypeextension.getString(), "EPS") == 0 || strcmp(filetypeextension.getString(), "PS") == 0 )
  {
    return writeToPostScript( filename.getString() );
  }
  else if ( strcmp(filetypeextension.getString(), "RGB") == 0 || strcmp(filetypeextension.getString(), "SGI") == 0 )
  {
    return writeToRGB( filename.getString() );
  }
  else // try to convert into a QImage and save then
  {
    QImage img;
    if ( writeToImage ( img, filetypeextension.getString() ) )
    {
      return img.save( filename.getString(), filetypeextension.getString() );
    }
  }

  return false;
}

QStringList SoFCOffscreenRenderer::getWriteImageFiletypeInfo()
{
  QStringList formats;

  // get all supported formats by Coin3D
  int num = getNumWriteFiletypes();
  for (int i=0; i < num; i++)
  {
#if   (COIN_MAJOR_VERSION < 2) // Coin3D <= 1.x
    SbList<SbName> extlist;
#elif (COIN_MAJOR_VERSION < 3) // Coin3D <= 2.x
# if  (COIN_MINOR_VERSION < 3) // Coin3D <= 2.2.x
    SbList<SbName> extlist;
# else                         // Coin3D >= 2.3.x
    SbPList extlist;
# endif                        
#else                          // Coin3D >= 3.x
    SbPList extlist;
#endif

    SbString fullname, description;
    getWriteFiletypeInfo(i, extlist, fullname, description);

    for (int j=0; j < extlist.getLength(); j++)
    {
      QString ext = (const char*) extlist[j];
      if ( formats.findIndex( ext.upper() ) == -1 )
        formats << ext.upper();
    }
  }

  // add now all further QImage formats
  QStringList qtformats = QImage::outputFormatList();
  for ( QStringList::Iterator it = qtformats.begin(); it != qtformats.end(); ++it )
  {
    // not supported? then append
    if ( isWriteSupported( (*it).latin1() ) == false && formats.findIndex(*it) == -1 )
      formats << *it;
  }

  // now add PostScript and SGI RGB
  if ( formats.findIndex("EPS") == -1 )
    formats << "EPS";
  if ( formats.findIndex("SGI") == -1 )
    formats << "SGI";

  formats.sort();

  return formats;
}
