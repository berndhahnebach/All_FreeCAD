/** \file DlgDocTamplatesImp.h
 *  \brief Implementation of the Doc Tamplate Dialog
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.cpp
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

#ifndef DLGDOCTEMPLATESIMP_H
#define DLGDOCTEMPLATESIMP_H
#include "DlgDocTemplates.h"
#include "Window.h"

class FCCommand;

class DlgDocTemplatesImp : virtual public DlgDocTemplates, public Gui::WindowParameter
{ 
    Q_OBJECT

public:

    DlgDocTemplatesImp( FCCommand* pcCmd,QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgDocTemplatesImp();

/*	// for linux one dummy constructor:
#ifdef FC_OS_LINUX
	DlgDocTemplatesImp(int dummy, QWidget* parent,  const char* name, bool modal, WFlags fl )
		:WindowParameter(name){DlgDocTemplatesImp(parent,name,modal,fl);}
#endif */

public slots:
    virtual void ChoseFile();
    virtual void Validate();
    virtual void ViewChange(QIconViewItem*);
    virtual void EditFile();
    virtual void IconDoubleClick(QIconViewItem*);

protected:
	FCCommand* _pcCmd;
};

#endif // DLGDOCTEMPLATESIMP_H
