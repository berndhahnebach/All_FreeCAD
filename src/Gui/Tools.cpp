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
# include <qdir.h>
# include <qfileinfo.h>
# include <qobjectlist.h>
# include <qpainter.h>
# include <qpalette.h>
# include <qtoolbar.h>
#endif

#include "Tools.h"

using namespace Gui;

QPixmap Tools::resize(int w, int h, QPixmap p)
{
  QPixmap pix = p;

  if (pix.width() == 0 || pix.height() == 0)
    return pix; // do not resize a null pixmap

  QPalette pal = qApp->palette();
  QColorGroup g = pal.disabled();

  QPixmap pm = pix;
  pm.resize(w,h);
  pm.fill(g.light());

  QPainter pt;
  pt.begin( &pm );
  pt.setPen( g.light() );
  pt.drawPixmap(1, 1, pix);
  pt.setPen( g.text() );
  pt.drawPixmap(0, 0, pix);
  pt.end();
  return pm;
}

QPixmap Tools::fillUp(int w, int h, QPixmap p)
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

  if (pix.mask())
  {
    bitBlt(&mask, x, y, pix.mask(), 0, 0, pix.width(), pix.height(), Qt::CopyROP, false);
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

QPixmap Tools::fillOpaqueRect(int x, int y, int w, int h, QPixmap p)
{
  if (!p.mask())
    return p; // sorry, but cannot do anything

  QBitmap b = *p.mask();

  // modify the mask
  QPainter pt;
  pt.begin(&b);
  pt.fillRect(x, y, w, h, Qt::color1); // make opaque
  pt.end();

  p.setMask(b);

  return p;
}

QPixmap Tools::fillTransparentRect(int x, int y, int w, int h, QPixmap p)
{
  if (!p.mask())
    return p; // sorry, but cannot do anything

  QBitmap b = *p.mask();

  // modify the mask
  QPainter pt;
  pt.begin(&b);
  pt.fillRect(x, y, w, h, Qt::color0); // make transparent
  pt.end();

  p.setMask(b);

  return p;
}

int Tools::getURLType(const QString& url)
{
  QFileInfo fi(url);

  // valid and existing file
  if (fi.isFile() && fi.exists())
    return 0;

  QString s = url.lower();
  const QFileInfoList* roots = QDir::drives();
  QListIterator<QFileInfo> i(*roots);
  QFileInfo* pfi;
  while ( (pfi = *i) )
  {
    QString fp = pfi->filePath().lower();
    ++i;
    if ( s.startsWith( fp ) )
      return 1;
  }

  return 2;
}
