/** \file DlgTipOfTheDayImp.h
 *  \brief The tip of the day dialog
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Werner Mayer (werner.wm.mayer@gmx.de) 2000 - 2003                 *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Werner Mayer 2002                                                     *
 ***************************************************************************/


#ifndef DLG_TIP_OF_THE_DAY_IMP_H__
#define DLG_TIP_OF_THE_DAY_IMP_H__

#include "DlgTipOfTheDay.h"

class TipOfTheDayDlgImp : public TipOfTheDayDlg
{
	public:
		TipOfTheDayDlgImp( QWidget* parent = 0, const char* name = 0);
		~TipOfTheDayDlgImp();

		void onNext();
};

#endif // DLG_TIP_OF_THE_DAY_IMP_H__
