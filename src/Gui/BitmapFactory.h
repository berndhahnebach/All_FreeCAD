/***************************************************************************
                          BitmapFactory.h  -  description
                             -------------------
    begin                : 2003/01/12 23:25:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file BitmapFactory.h
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
 

#ifndef __FC_BITMAP_FACTORY_H__
#define __FC_BITMAP_FACTORY_H__

#include "../Base/Factory.h"

/** The Bitmap Factory
  * the main purpose is to collect all build in Bitmaps and 
  * hold all paths for the extern bitmaps (files) to serve
  * as a single point of accessing bitmaps in FreeCAD
  */
class GuiExport FCBitmapFactory : public FCFactory
{
	public:
		enum Position
		{
			TopLeft, TopRight, BottomLeft, BottomRight
		};

		static FCBitmapFactory& Instance(void);
		static void Destruct (void);

		/// Adds a path were pixmaps can be found
		void AddPath(const char* sPath);
		/// Remove a path from the list of pixmap paths
		void RemovePath(const char* sPath);
		/// Add a build in XPM pixmap under a given name
		void AddXPM(const char* sName, const char** pXPM);
		/// Remove a build in pixmap by a given name
		void RemoveXPM(const char* sName);
		/// retrieve a pixmap by name
		QPixmap GetPixmap(const char* sName);
		QPixmap GetPixmap(const char* sName, const char* sMask, Position pos = BottomLeft);

	private:
		static FCBitmapFactory* _pcSingleton;

		FCBitmapFactory(){}
		~FCBitmapFactory(){}

		std::map<std::string,const char**> _mpXPM;
		std::vector<std::string>          _vsPaths;
};

inline GuiExport FCBitmapFactory& GetBitmapFactory(void)
{
	return FCBitmapFactory::Instance();
}

#endif // __FC_BITMAP_FACTORY_H__
