/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef DlgParameter_H
#define DlgParameter_H

#include "DlgParameter.h"
#include "../Base/Parameter.h"

#ifndef _PreComp_
# include <qlistview.h>
#endif 

namespace Gui {
namespace Dialog {

class ParameterLabelGroup;

/**
 * The DlgParameterImp class implements a dialog showing all parameters in a list view.
 * \author Jürgen Riegel
 */
class DlgParameterImp : public DlgParameter
{ 
    Q_OBJECT

public:
  DlgParameterImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgParameterImp();

public slots:
  virtual void onAddText();
  virtual void onDeleteItem();
  virtual void onAddFloat();
  virtual void onAddGroup();
  virtual void onAddInteger();
  virtual void onCleatValue();
  virtual void onInsertFromFile();
  virtual void onLoadParameterSet();
  virtual void onParameterSetChange(const QString& rcString);

protected:
  ParameterLabelGroup *_pcMainLabel;
};

/** The link between the Tree and the shown Label.
 * Every (shown) Label in the FCDocument class get it 
 * associated FCTreeLabel which controls the visibility 
 * and the functions of the Label.
 *
 * \author Jürgen Riegel
 */
class ParameterLabelGroup : public QListViewItem
{
public:
  /// Constructor
  ParameterLabelGroup( ParameterLabelGroup * parent, const FCHandle<FCParameterGrp> &hcGrp );
  ParameterLabelGroup( QListView* parent, const FCHandle<FCParameterGrp> &hcGrp);

  void fillUp(void);

protected:
  void activate (); 

  FCHandle<FCParameterGrp> _hcGrp;
};

} // namespace Dialog
} // namespace Gui

#endif // DlgParameter_H
