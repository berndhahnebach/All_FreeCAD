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


#ifndef GUI_DIALOG_DLGDISPLAYPROPERTIES_IMP_H
#define GUI_DIALOG_DLGDISPLAYPROPERTIES_IMP_H

#include <vector>

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#ifndef __Qt3All__
# include "Qt3All.h"
#endif

#ifndef __InventorAll__
# include "InventorAll.h"
#endif


#include "ui_DlgDisplayProperties.h"
#include <App/Material.h>

namespace App
{
  class DocumentObject;
}

namespace Gui {

  class ViewProvider;
  class Command;

namespace Dialog {

/**
 * The DlgDisplayPropertiesImp class implements a dialog containing all available document
 * templates to create a new document.
 * \author Jürgen Riegel
 */
class DlgDisplayPropertiesImp : public QDialog, public Ui_DlgDisplayProperties
{
    Q_OBJECT

public:
  DlgDisplayPropertiesImp( QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~DlgDisplayPropertiesImp();

public Q_SLOTS:
  void on_changeMaterial_activated(const QString&);
  void on_changeMode_activated(const QString&);
  void on_changePlot_activated(const QString&);
  void on_buttonColor_changed();
  void on_spinTransparency_valueChanged(int);
  void on_spinPointSize_valueChanged(double);
  void on_spinLineWidth_valueChanged(double);
  void on_buttonUserDefinedMaterial_clicked();

protected:
  void accept();
  void reject();

protected:
  std::vector<ViewProvider*> Provider;
  QMap<QString, App::Material::MaterialType> Materials;
};

} // namespace Dialog
} // namespace Gui

#endif // GUI_DIALOG_DLGDISPLAYPROPERTIES_IMP_H
