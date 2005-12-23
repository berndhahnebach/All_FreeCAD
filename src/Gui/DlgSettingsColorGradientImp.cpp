/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qmessagebox.h>
#endif

#include "DlgSettingsColorGradientImp.h"
#include "SpinBox.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgSettingsColorGradientImp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgSettingsColorGradientImp::DlgSettingsColorGradientImp( QWidget* parent, const char* name, bool modal, WFlags fl )
  : DlgSettingsColorGradientBase( parent, name, modal, fl )
{
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsColorGradientImp::~DlgSettingsColorGradientImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgSettingsColorGradientImp::setColorModel( App::ColorGradient::TColorModel tModel)
{
  switch (tModel)
  {
  case App::ColorGradient::TRIA:
    comboBoxModel->setCurrentItem(0);
    break;
  case App::ColorGradient::INVERSE_TRIA:
    comboBoxModel->setCurrentItem(1);
    break;
  case App::ColorGradient::GRAY:
    comboBoxModel->setCurrentItem(2);
    break;
  case App::ColorGradient::INVERSE_GRAY:
    comboBoxModel->setCurrentItem(3);
    break;
  }
}

App::ColorGradient::TColorModel DlgSettingsColorGradientImp::colorModel() const
{
  int item = comboBoxModel->currentItem();
  if ( item == 0 )
    return App::ColorGradient::TRIA;
  else if ( item == 1 )
    return App::ColorGradient::INVERSE_TRIA;
  else if ( item == 2 )
    return App::ColorGradient::GRAY;
  else
    return App::ColorGradient::INVERSE_GRAY;
}

void DlgSettingsColorGradientImp::setColorStyle( App::ColorGradient::TStyle tStyle )
{
  switch ( tStyle )
  {
  case App::ColorGradient::FLOW:
    buttonGroupStyle->setButton(0);
    break;
  case App::ColorGradient::ZERO_BASED:
    buttonGroupStyle->setButton(1);
    break;
  }
}

App::ColorGradient::TStyle DlgSettingsColorGradientImp::colorStyle() const
{
  int id = buttonGroupStyle->selectedId();
  if ( id == 0 )
    return App::ColorGradient::FLOW;
  else
    return App::ColorGradient::ZERO_BASED;
}

void DlgSettingsColorGradientImp::setOutGrayed( bool grayed )
{
  checkBoxGrayed->setChecked( grayed );
}

bool DlgSettingsColorGradientImp::isOutGrayed() const
{
  return checkBoxGrayed->isChecked();
}

void DlgSettingsColorGradientImp::setOutInvisible( bool invisible )
{
  checkBoxInvisible->setChecked( invisible );
}

bool DlgSettingsColorGradientImp::isOutInvisible() const
{
  return checkBoxInvisible->isChecked();
}

void DlgSettingsColorGradientImp::setRange( float fMin, float fMax )
{
  floatSpinBoxMax->setValue( fMax );
  floatSpinBoxMin->setValue( fMin );
}

void DlgSettingsColorGradientImp::getRange( float& fMin, float& fMax) const
{
  fMax = floatSpinBoxMax->value();
  fMin = floatSpinBoxMin->value();
}

void DlgSettingsColorGradientImp::setNumberOfLabels(int val)
{
  spinBoxLabel->setValue( val );
}

int DlgSettingsColorGradientImp::numberOfLabels() const
{
  return spinBoxLabel->value();
}

void DlgSettingsColorGradientImp::onValidateMaximum()
{
  double fMax = floatSpinBoxMax->value();
  double fMin = floatSpinBoxMin->value();
  if ( fMax <= fMin )
  {
    QMessageBox::warning(this, tr("Wrong parameter"), tr("The maximum value must be higher than '%1'.").arg(fMin));
    floatSpinBoxMax->blockSignals(true);
    floatSpinBoxMax->setValue(fMin+0.01);
    floatSpinBoxMax->blockSignals(false);
  }
}

void DlgSettingsColorGradientImp::onValidateMinimum()
{
  double fMax = floatSpinBoxMax->value();
  double fMin = floatSpinBoxMin->value();
  if ( fMax <= fMin )
  {
    QMessageBox::warning(this, tr("Wrong parameter"), tr("The minimum value must be lower than '%1'.").arg(fMax));
    floatSpinBoxMin->blockSignals(true);
    floatSpinBoxMin->setValue(fMax-0.01);
    floatSpinBoxMin->blockSignals(false);
  }
}

#include "DlgSettingsColorGradient.cpp"
#include "moc_DlgSettingsColorGradient.cpp"
