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


#ifndef DlgDisplayPropertiesImp_H
#define DlgDisplayPropertiesImp_H

#include <vector>
#include <Inventor/SbLinear.h>
#include <qmap.h>

#include "DlgDisplayProperties.h"
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
class DlgDisplayPropertiesImp : public DlgDisplayProperties
{
    Q_OBJECT

public:
  DlgDisplayPropertiesImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgDisplayPropertiesImp();

public slots:
  virtual void onChangeMaterial(const QString&);
  virtual void onChangeMode(const QString&);
  virtual void onChangePlot(const QString&);
  virtual void onColorChange();
  virtual void onChangeTrans(int);
  virtual void onChangePointSize(double);
  virtual void onChangeLineWidth(double);
  virtual void onUserDefinedMaterial();

protected:
  virtual void accept();
  virtual void reject();

protected:
  std::vector<ViewProvider*> Provider;
  QMap<QString, App::Material::MaterialType> Materials;
};

} // namespace Dialog
} // namespace Gui

#endif // DlgDisplayPropertiesImp_H
