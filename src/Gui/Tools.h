/***************************************************************************
                          Tools.h  -  description
                             -------------------
    begin                : 2003/02/23 10:19:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Tools.h
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
 

#ifndef __FC_TOOLS_H__
#define __FC_TOOLS_H__

class QPixmap;

/**
 * A helper class
 */
class FCTools
{
  public:
    /** Resizes the area of a pixmap
     * If the size is greater than the old one the pixmap
     * will be placed in the center. The border area will be filled
     * up with current style's base color. 
     */
    static QPixmap fillUp(int w, int h, QPixmap p);
    /** Does the same as @ref fillUp() but does not move the pixmap
     * to the center.
     */
    static QPixmap resize(int w, int h, QPixmap p);
    /** Clears only the tool buttons and the separators of a toolbar */
    static void clearToolButtons(QToolBar* tb);
};

#endif // __FC_TOOLS_H__
