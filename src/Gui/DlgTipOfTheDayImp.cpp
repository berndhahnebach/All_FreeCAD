/** \file DlgTipOfTheDayImp.cpp
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


#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qcheckbox.h>
#endif

#include "../Base/Parameter.h"
#include "Application.h"
#include "DlgTipOfTheDayImp.h"

TipOfTheDayDlgImp::TipOfTheDayDlgImp( QWidget* parent, const char* name )
	: TipOfTheDayDlg( parent, name, true, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool tips = hGrp->GetBool("Tipoftheday", true);
	checkShowTips->setChecked(tips);
}

TipOfTheDayDlgImp::~TipOfTheDayDlgImp()
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  hGrp->SetBool("Tipoftheday", checkShowTips->isChecked());
}

void TipOfTheDayDlgImp::onNext()
{
	textTip->setText("Tip of the day");
}

#include "DlgTipOfTheDay.cpp"
#include "moc_DlgTipOfTheDay.cpp"
