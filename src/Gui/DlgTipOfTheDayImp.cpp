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
# include <qfile.h>
# include <qlabel.h>
# include <qlayout.h>
# include <qtextstream.h>
#endif

#include "DlgTipOfTheDayImp.h"
#include "Application.h"

#include "../Base/Parameter.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgTipOfTheDayImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgTipOfTheDayImp::DlgTipOfTheDayImp( QWidget* parent, const char* name, bool modal, WFlags fl )
  : DlgTipOfTheDayBase( parent, name, modal, fl | WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  bool tips = hGrp->GetBool("Tipoftheday", true);
  checkShowTips->setChecked(tips);

  // Since the resize mode of DlgTipOfTheDayBase does not
  // work properly so set this by hand 
  setMinimumSize(QSize(320, 250));
  layout()->setResizeMode( QLayout::FreeResize );

  reload();
  onNext();
}

/** Destroys the object and frees any allocated resources */
DlgTipOfTheDayImp::~DlgTipOfTheDayImp()
{
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().GetGroup("BaseApp")->GetGroup("WindowSettings");
  hGrp->SetBool("Tipoftheday", checkShowTips->isChecked());
}

/** Shows next tip taken from the Tip-of-the-day site. */
void DlgTipOfTheDayImp::onNext()
{
  _iCurrentTip = ( _iCurrentTip + 1 ) % _lTips.size();
  textTip->setText( _lTips[_iCurrentTip] );
}

/** Reloads all tips from Tip-of-the-day site. */
void DlgTipOfTheDayImp::reload()
{
  // search for the Wiki Tip-of-the-day site
  FCParameterGrp::handle hGrp = GetApplication().GetSystemParameter().
                                GetGroup("BaseApp")->GetGroup("WindowSettings");

  QString home = GetApplication().GetHomePath();
  QString path = hGrp->GetASCII("OnlineDocDir", "/doc/free-cad.sourceforge.net/").c_str();
  QString file = home + path + "index.php@TipOfTheDay.html";

  QFile f ( file );
  QTextStream txt( &f );

  if ( f.open( IO_ReadOnly ) )
  {
    do
    {
      QString inp = txt.readLine();
      if (inp.startsWith("<li> "))
      {
        _lTips << inp;
      }
    }
    while ( !txt.eof() );
  }

  _iCurrentTip = 0;

  // show standard tip if site is not found
  if ( _lTips.empty() )
  {
    _lTips << QString("If you want to make use of the Tip-of-the-day you must download "
                      "FreeCAD's online help from <a href=""http://free-cad.sourceforge.net/index.html"">"
                      "http://free-cad.sourceforge.net/index.html</a>."
                      "To download the online help press Help -> Download online help.");
  }
}

#include "DlgTipOfTheDay.cpp"
#include "moc_DlgTipOfTheDay.cpp"
