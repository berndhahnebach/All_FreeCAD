/** \file DlgParameterImp.h
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


#ifndef DlgParameter_H
#define DlgParameter_H

#include "DlgParameter.h"
#include "../Base/Parameter.h"

#if QT_VERSION > 230
# include <qlistview.h>
#endif 

class 	ParameterLabelGroup;

class DlgParameter : public ParameterDialog
{ 
    Q_OBJECT

public:
    DlgParameter( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgParameter();

public slots:
    virtual void OnAddText();
    virtual void DeleteItem();
    virtual void OnAddFloat();
    virtual void OnAddGroup();
    virtual void OnAddInteger();
    virtual void OnCleatValue();
    virtual void OnInsertFromFile();
    virtual void OnLoadParameterSet();
    virtual void OnParameterSetChange(const QString& rcString);

protected:
	ParameterLabelGroup *_pcMainLabel;

};

/** The link between the Tree and the shown Label.
 *  Every (shown) Label in the FCDocument class get it 
 *  associated FCTreeLabel which controls the visibility 
 *  and the functions of the Label.
 */
class ParameterLabelGroup : public QListViewItem
{
public:
	/// Constructor
    ParameterLabelGroup( ParameterLabelGroup * parent, const FCHandle<FCParameterGrp> &hcGrp );
	ParameterLabelGroup( QListView* parent, const FCHandle<FCParameterGrp> &hcGrp);
    /// Opens the Leafs and generate them.
    //void setOpen( bool );

	void FillUp(void);

protected:
	void activate (); 

	FCHandle<FCParameterGrp> _hcGrp;

};


#endif // DlgParameter_H
