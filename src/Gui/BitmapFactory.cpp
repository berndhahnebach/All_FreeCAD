/***************************************************************************
                          BitmapFactory.cpp  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file BitmapFactory.cpp
 *  \brief Produce widgets
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/



#include "PreCompiled.h"

#include "BitmapFactory.h"
#include "Tools.h"
#include "../Base/Console.h"
#include "Icons/images.cpp"
#include "Icons/BmpFactoryIcons.cpp"



FCBitmapFactory* FCBitmapFactory::_pcSingleton = NULL;

FCBitmapFactory& FCBitmapFactory::Instance(void)
{
  if (_pcSingleton == NULL)
	{
    _pcSingleton = new FCBitmapFactory;
		_pcSingleton->AddPath("../../FreeCADIcons");
		_pcSingleton->AddPath("../Icons");

		RegisterIcons();
	}

  return *_pcSingleton;
}

void FCBitmapFactory::Destruct (void)
{
  if (_pcSingleton != NULL)
    delete _pcSingleton;
}

void FCBitmapFactory::AddPath(const char* sPath)
{
	_vsPaths.push_back(sPath);
}

void FCBitmapFactory::RemovePath(const char* sPath)
{
	_vsPaths.erase(std::find<std::vector<std::string>::iterator,std::string>(_vsPaths.begin(),_vsPaths.end(),sPath));
}

void FCBitmapFactory::AddXPM(const char* sName, const char** pXPM)
{
	_mpXPM[sName] = pXPM;
}

void FCBitmapFactory::RemoveXPM(const char* sName)
{
	_mpXPM.erase(sName);
}

QPixmap FCBitmapFactory::GetPixmap(const char* sName)
{
	// first try to find it in the build in XPM
	std::map<std::string,const char**>::const_iterator It = _mpXPM.find(sName);

	if(It != _mpXPM.end())
		return QPixmap(It->second);

	// try to find it in the given directorys
	for(std::vector<std::string>::const_iterator It2 = _vsPaths.begin();It2 != _vsPaths.end();It2++)
	{
		// list dir
		QDir d( (*It2).c_str() );
		if( QFile(d.path()+QDir::separator() + sName +".bmp").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".bmp");
		if( QFile(d.path()+QDir::separator()+ sName + ".png").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".png");
		if( QFile(d.path()+QDir::separator()+ sName + ".gif").exists() )
			return QPixmap(d.path()+QDir::separator()+ sName + ".gif");
	}

	GetConsole().Warning("Can't find Pixmap:%s\n",sName);

	return QPixmap(px);

}

QPixmap FCBitmapFactory::GetPixmap(const char* sName, const char* sMask, Position pos)
{
  QPixmap p1 = GetPixmap(sName);
  QPixmap p2 = GetPixmap(sMask);

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
  p = FCTools::fillOpaqueRect(x, y, p2.width(), p2.height(), p);

  QPainter pt;
  pt.begin( &p );
  pt.setPen(Qt::NoPen);
  pt.drawRect(x, y, p2.width(), p2.height());
  pt.drawPixmap(x, y, p2);
  pt.end();

  return p;
}
