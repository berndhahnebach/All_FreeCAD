/***************************************************************************
                          Tools.cpp  -  description
                             -------------------
    begin                : 2003/02/23 10:19:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Tools.cpp
 *  \brief A set tool classes
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/02/23
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


#ifndef _PreComp_
# include <qpixmap.h>
#endif

#include "Tools.h"
#include <qobjcoll.h>


QPixmap FCTools::resize(int w, int h, QPixmap p)
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

void FCTools::clearToolButtons(QToolBar* tb)
{
  if ( !tb->children() )
  	return;
  QObjectListIt it( *tb->children() );
  QObject * obj;
  while( (obj=it.current()) != 0 ) 
  {
  	++it;
	  if ( obj->isWidgetType() )
    {
      if ( obj->inherits("QToolButton") || obj->inherits("QToolBarSeparator") )
  	    delete obj;
    }
  }
}
