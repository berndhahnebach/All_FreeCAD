/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qdir.h>
# include <qpainter.h>
#endif

#include "BitmapFactory.h"
#include "Tools.h"
#include "../Base/Console.h"
#include "Icons/images.cpp"
#include "Icons/Feature.xpm"
#include "Icons/Icons.h"
#include "Icons/Document.xpm"
#include "Icons/BmpFactoryIcons.cpp"
#define new DEBUG_CLIENTBLOCK
using namespace Gui;

BitmapFactoryInst* BitmapFactoryInst::_pcSingleton = NULL;

BitmapFactoryInst& BitmapFactoryInst::instance(void)
{
  if (_pcSingleton == NULL)
  {
    _pcSingleton = new BitmapFactoryInst;
    _pcSingleton->addPath("../../FreeCADIcons");
    _pcSingleton->addPath("../Icons");

    RegisterIcons();
  }

  return *_pcSingleton;
}

void BitmapFactoryInst::destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

void BitmapFactoryInst::addPath(const char* sPath)
{
  _vsPaths.push_back( sPath );
}

void BitmapFactoryInst::removePath(const char* sPath)
{
  _vsPaths.erase( _vsPaths.find( sPath ) );
}

void BitmapFactoryInst::addXPM(const char* sName, const char** pXPM)
{
  _mpXPM[sName] = pXPM;
}

void BitmapFactoryInst::addFileFormat(const char* sFormat, const char** pXPM)
{
  QString format(sFormat);
  _mpXPM[format.upper()] = pXPM;
}

void BitmapFactoryInst::removeXPM(const char* sName)
{
  _mpXPM.erase(sName);
}

QPixmap* BitmapFactoryInst::fileFormat(const char* sFormat)
{
  // first try to find it in the build in XPM
  QMap<QString,const char**>::ConstIterator It = _mpXPM.find( sFormat );

  QPixmap* px=0;
  if(It != _mpXPM.end())
  {
    px = new QPixmap( It.data() );
  }

  return px;
}

QPixmap BitmapFactoryInst::pixmap(const char* sName)
{
  // first try to find it in the build in XPM
  QMap<QString,const char**>::ConstIterator It = _mpXPM.find(sName);

  if(It != _mpXPM.end())
    return QPixmap( It.data() );

  // try to find it in the given directorys
  for ( QStringList::ConstIterator It2 = _vsPaths.begin(); It2 != _vsPaths.end(); It2++ )
  {
    // list dir
    QDir d( *It2 );
    if( QFile(d.path()+QDir::separator() + sName +".bmp").exists() )
      return QPixmap(d.path()+QDir::separator()+ sName + ".bmp");
    if( QFile(d.path()+QDir::separator()+ sName + ".png").exists() )
      return QPixmap(d.path()+QDir::separator()+ sName + ".png");
    if( QFile(d.path()+QDir::separator()+ sName + ".gif").exists() )
      return QPixmap(d.path()+QDir::separator()+ sName + ".gif");
  }

  Base::Console().Warning("Can't find Pixmap:%s\n",sName);

  return QPixmap(px);

}

QPixmap BitmapFactoryInst::pixmap(const char* sName, const char* sMask, Position pos)
{
  QPixmap p1 = pixmap(sName);
  QPixmap p2 = pixmap(sMask);

  int x = 0, y = 0;

  switch (pos)
  {
    case TopLeft:
      break;
    case TopRight:
      x = p1.width () - p2.width ();
      break;
    case BottomLeft:
      y = p1.height() - p2.height();
      break;
    case BottomRight:
      x = p1.width () - p2.width ();
      y = p1.height() - p2.height();
      break;
  }

  QPixmap p = p1;
  p = Tools::fillOpaqueRect(x, y, p2.width(), p2.height(), p);

  QPainter pt;
  pt.begin( &p );
  pt.setPen(Qt::NoPen);
  pt.drawRect(x, y, p2.width(), p2.height());
  pt.drawPixmap(x, y, p2);
  pt.end();

  return p;
}

QStringList BitmapFactoryInst::pixmapNames() const
{
  QStringList names;
  for ( QMap<QString,const char**>::ConstIterator It = _mpXPM.begin(); It != _mpXPM.end(); ++It )
  {
    names << It.key();
  }
  return names;
}
