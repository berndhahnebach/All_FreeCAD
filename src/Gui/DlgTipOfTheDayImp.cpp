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
# include <qcheckbox.h>
#endif

#include "../Base/Parameter.h"
#include "Application.h"
#include "DlgTipOfTheDayImp.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgTipOfTheDayImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgTipOfTheDayImp::DlgTipOfTheDayImp( QWidget* parent, const char* name )
  : DlgTipOfTheDayBase( parent, name, true, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool tips = hGrp->GetBool("Tipoftheday", true);
  checkShowTips->setChecked(tips);
}

/** Destroys the object and frees any allocated resources */
DlgTipOfTheDayImp::~DlgTipOfTheDayImp()
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  hGrp->SetBool("Tipoftheday", checkShowTips->isChecked());
}

/** Show next tip taking from database */
void DlgTipOfTheDayImp::onNext()
{
  textTip->setText("Tip of the day");
}

#include "DlgTipOfTheDay.cpp"
#include "moc_DlgTipOfTheDay.cpp"
