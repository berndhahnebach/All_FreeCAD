/** \file DlgSettings3DViewImp.h
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#ifndef __DLGSETTINGS3DVIEWIMP_H
#define __DLGSETTINGS3DVIEWIMP_H
#include "DlgSettings3DView.h"

namespace Gui {
namespace Dialog {

class DlgSettings3DViewImp : public DlgSettings3DView, public Gui::Dialog::PreferencePage
{ 
    Q_OBJECT


    virtual void ChooseDir();
    virtual void WarnInventor(bool);


public:
    DlgSettings3DViewImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettings3DViewImp();

};

} // namespace Dialog
} // namespace Gui

#endif // __DLGSETTINGS3DVIEWIMP_H
