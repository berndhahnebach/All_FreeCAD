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

#include "DlgSettingsColorGradientImp.h"
#include "SpinBox.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgSettingsColorGradientImp */

/**
 *  Constructs a DlgSettingsColorGradientImp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgSettingsColorGradientImp::DlgSettingsColorGradientImp( QWidget* parent, Qt::WFlags fl )
  : QDialog( parent, fl )
{
  this->setupUi(this);
  //connect(floatSpinBoxMin, SIGNAL(valueChanged(double)), this, SLOT(onValidateMinimum()));
  //connect(floatSpinBoxMax, SIGNAL(valueChanged(double)), this, SLOT(onValidateMaximum()));
  connect(spinBoxDecimals, SIGNAL(valueChanged(int)), this, SLOT(onSetDecimals(int)));
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
    comboBoxModel->setCurrentIndex(0);
    break;
  case App::ColorGradient::INVERSE_TRIA:
    comboBoxModel->setCurrentIndex(1);
    break;
  case App::ColorGradient::GRAY:
    comboBoxModel->setCurrentIndex(2);
    break;
  case App::ColorGradient::INVERSE_GRAY:
    comboBoxModel->setCurrentIndex(3);
    break;
  }
}

App::ColorGradient::TColorModel DlgSettingsColorGradientImp::colorModel() const
{
  int item = comboBoxModel->currentIndex();
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
    radioButtonFlow->setChecked(true);
    break;
  case App::ColorGradient::ZERO_BASED:
    radioButtonZero->setChecked(true);
    break;
  }
}

App::ColorGradient::TStyle DlgSettingsColorGradientImp::colorStyle() const
{
  return radioButtonZero->isChecked() ? App::ColorGradient::ZERO_BASED : App::ColorGradient::FLOW;
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
  floatSpinBoxMax->blockSignals(true);
  floatSpinBoxMax->setValue( fMax );
  floatSpinBoxMax->blockSignals(false);
  floatSpinBoxMin->blockSignals(true);
  floatSpinBoxMin->setValue( fMin );
  floatSpinBoxMin->blockSignals(false);
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

void DlgSettingsColorGradientImp::setNumberOfDecimals(int val)
{
  spinBoxDecimals->setValue(val);
  onSetDecimals(val);
}

int DlgSettingsColorGradientImp::numberOfDecimals() const
{
  return spinBoxDecimals->value();
}

void DlgSettingsColorGradientImp::accept()
{
  double fMax = floatSpinBoxMax->value();
  double fMin = floatSpinBoxMin->value();

  if (fMax <= fMin) {
    QMessageBox::warning(this, tr("Wrong parameter"), tr("The maximum value must be higher than the minimum value."));
  }
  else {
    QDialog::accept();
  }
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

void DlgSettingsColorGradientImp::onSetDecimals(int dec)
{
  floatSpinBoxMax->setDecimals(dec);
  floatSpinBoxMin->setDecimals(dec);
  double step = pow(0.1,dec);
  floatSpinBoxMax->setSingleStep(step);
  floatSpinBoxMin->setSingleStep(step);
}

#include "moc_DlgSettingsColorGradientImp.cpp"
