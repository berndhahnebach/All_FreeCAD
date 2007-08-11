/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qapplication.h>
# include <qbitmap.h>
# include <qimage.h>
# include <qpainter.h>
# include <qpalette.h>
# include <QPixmap>
#endif

// gcc
#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "Tools.h"

using namespace Gui;

//TODO: Move all functions to BitmapFactory
QPixmap Tools::resize(int w, int h, const QPixmap& p)
{
  QPixmap pix = p;

  if (pix.width() == 0 || pix.height() == 0)
    return pix; // do not resize a null pixmap

  QPalette pal = qApp->palette();
  //QColorGroup g = pal.disabled();
  QColor dl = pal.color(QPalette::Disabled, QPalette::Light);
  QColor dt = pal.color(QPalette::Disabled, QPalette::Text);

  QPixmap pm = pix;
  pm = QPixmap(w,h);
  //pm.fill(g.light());
  pm.fill(dl);

  QPainter pt;
  pt.begin( &pm );
  //pt.setPen( g.light() );
  pt.setPen( dl );
  pt.drawPixmap(1, 1, pix);
  //pt.setPen( g.text() );
  pt.setPen( dt );
  pt.drawPixmap(0, 0, pix);
  pt.end();
  return pm;
}

QPixmap Tools::fillUp(int w, int h, const QPixmap& p)
{
  if (p.width() == 0 || p.height() == 0)
    w = 1;

  QPixmap pix = p;
  int x = pix.width () > w ? 0 : (w - pix.width ())/2;
  int y = pix.height() > h ? 0 : (h - pix.height())/2;

  if (x == 0 && y == 0)
    return pix;

  QPixmap pm (w,h);
  QBitmap mask (w,h);
  mask.fill(Qt::color0);

  QBitmap bm = pix.mask();
  if (!bm.isNull())
  {
    //TODO Search for the Qt4 counterpart
    QPainter painter(&mask);
    painter.drawPixmap(QPoint(x, y), bm, QRect(0, 0, pix.width(), pix.height()));
    //bitBlt(&mask, x, y, &bm, 0, 0, pix.width(), pix.height(), /*Qt::CopyROP,*/ false);
    pm.setMask(mask);
  }
  else
  {
    pm.setMask(mask);
    pm = fillOpaqueRect(x, y, pix.width(), pix.height(), pm);
  }

  QPainter pt;
  pt.begin( &pm );
  pt.drawPixmap(x, y, pix);
  pt.end();
  return pm;
}

QPixmap Tools::fillOpaqueRect(int x, int y, int w, int h, const QPixmap& p)
{
  QBitmap b = p.mask();
  if (b.isNull())
    return p; // sorry, but cannot do anything

  QPixmap pix = p;

  // modify the mask
  QPainter pt;
  pt.begin(&b);
  pt.fillRect(x, y, w, h, Qt::color1); // make opaque
  pt.end();

  pix.setMask(b);

  return pix;
}

QPixmap Tools::fillTransparentRect(int x, int y, int w, int h, const QPixmap& p)
{
  QBitmap b = p.mask();
  if (b.isNull())
    return p; // sorry, but cannot do anything

  QPixmap pix = p;

  // modify the mask
  QPainter pt;
  pt.begin(&b);
  pt.fillRect(x, y, w, h, Qt::color0); // make transparent
  pt.end();

  pix.setMask(b);

  return pix;
}

QPixmap Tools::merge( const QPixmap& p1, const QPixmap& p2, bool vertical )
{
  int width = 0;
  int height = 0;

  int x = 0;
  int y = 0;

  if ( vertical )
  {
    y = p1.height();
    width  = qMax( p1.width(), p2.width() );
    height = p1.height() + p2.height();
  }
  else
  {
    x = p1.width();
    width  = p1.width() + p2.width();
    height = qMax( p1.height(), p2.height() );
  }

  QPixmap res( width, height );
  QBitmap mask( width, height );
  QBitmap mask1 = p1.mask();
  QBitmap mask2 = p2.mask();
  mask.fill( Qt::color0 );

  QPainter* pt1 = new QPainter(&res);
  pt1->drawPixmap(0, 0, p1);
  pt1->drawPixmap(x, y, p2);
  delete pt1;
  //bitBlt( &res,  0, 0, &p1 );
  //bitBlt( &res,  x, y, &p2 );

  QPainter* pt2 = new QPainter(&mask);
  pt2->drawPixmap(0, 0, mask1);
  pt2->drawPixmap(x, y, mask2);
  delete pt2;
  //bitBlt( &mask, 0, 0, &mask1 );
  //bitBlt( &mask, x, y, &mask2 );

  res.setMask( mask );
  return res;
}

QPixmap Tools::disabled( const QPixmap& p )
{
  QStyleOption opt;
  opt.palette = QApplication::palette();
  return QApplication::style()->generatedIconPixmap(QIcon::Disabled, p, &opt);
}

void Tools::convert( const QImage& p, SoSFImage& img )
{
  SbVec2s size;
  size[0] = p.width();
  size[1] = p.height();

  int buffersize = p.numBytes();
  int numcomponents = buffersize / ( size[0] * size[1] );

  // allocate image data
  img.setValue(size, numcomponents, NULL);

  unsigned char * bytes = img.startEditing(size, numcomponents);

  int width  = (int)size[0];
  int height = (int)size[1];

  for (int y = 0; y < height; y++) 
  {
    unsigned char * line = 
      &bytes[width*numcomponents*(height-(y+1))];
    for (int x = 0; x < width; x++) 
    {
      QRgb rgb = p.pixel(x,y);
      switch (numcomponents) 
      {
      default:
        break;
      case 1:
        line[0] = qGray( rgb );
        break;
      case 2:
        line[0] = qGray( rgb );
        line[1] = qAlpha( rgb );
        break;
      case 3:
        line[0] = qRed( rgb );
        line[1] = qGreen( rgb );
        line[2] = qBlue( rgb );
        break;
      case 4:
        line[0] = qRed( rgb );
        line[1] = qGreen( rgb );
        line[2] = qBlue( rgb );
        line[3] = qAlpha( rgb );
        break;
      }

      line += numcomponents;
    }
  }

  img.finishEditing();
}

void Tools::convert( const SoSFImage& p, QImage& img )
{
  SbVec2s size;
  int numcomponents;

  const unsigned char * bytes = p.getValue( size, numcomponents );
  
  int width  = (int)size[0];
  int height = (int)size[1];

  img = QImage(width, height, QImage::Format_RGB32);

  QRgb * bits = (QRgb*) img.bits();
  for (int y = 0; y < height; y++) 
  {
    const unsigned char * line = 
      &bytes[width*numcomponents*(height-(y+1))];
    for (int x = 0; x < width; x++) 
    {
      switch (numcomponents) 
      {
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
}

