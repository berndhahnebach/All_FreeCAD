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
# include <qbuttongroup.h>
# include <qcheckbox.h>
# include <qcombobox.h>
# include <qdatetime.h>
# include <qmap.h>
# include <qpushbutton.h>
# include <qtextedit.h>
# include <iomanip>
# include <sstream>
#endif

#include "DlgSettingsImageImp.h"
#include "SpinBox.h"
#define new DEBUG_CLIENTBLOCK
using namespace Gui::Dialog;
using namespace std;

/**
 *  Constructs a DlgSettingsImageImp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgSettingsImageImp::DlgSettingsImageImp( QWidget* parent, const char* name, WFlags fl )
  : DlgSettingsImage( parent, name, fl )
{
  SbVec2s res = SoOffscreenRenderer::getMaximumResolution();
  spinWidth->setMaxValue((int)res[0]);
  spinHeight->setMaxValue((int)res[1]);

  _width = width();
  _height = height();
  _fRatio = (float)_width/(float)_height;
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsImageImp::~DlgSettingsImageImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/**
 * Sets the image size to (\a w, \a h).
 */
void DlgSettingsImageImp::setImageSize( int w, int h )
{
  spinWidth->setValue( w );
  spinHeight->setValue( h );

  // As the image size is in pixel why shouldn't _width and _height be integers?
  _width  = w;
  _height = h;
  _fRatio = (float)_width/(float)_height;
}

/**
 * Sets the image size to \a s.
 */
void DlgSettingsImageImp::setImageSize( const QSize& s )
{
  spinWidth->setValue( s.width() );
  spinHeight->setValue( s.height() );

  // As the image size is in pixel why shouldn't _width and _height be integers?
  _width  = s.width();
  _height = s.height();
  _fRatio = (float)_width/(float)_height;
}

/**
 * Returns the currenty set image size.
 */
QSize DlgSettingsImageImp::imageSize() const
{
  return QSize( spinWidth->value(), spinHeight->value() );
}

/**
 * Returns the currently set image width.
 */
int DlgSettingsImageImp::imageWidth() const
{
  return spinWidth->value();
}

/**
 * Returns the currently set image height.
 */
int DlgSettingsImageImp::imageHeight() const
{
  return spinHeight->value();
}

/**
 * Returns the comment of the picture. If for the currently selected image format no comments are supported
 * QString::null is returned.
 */
QString DlgSettingsImageImp::comment() const
{
  if ( !textEditComment->isEnabled() )
    return QString::null;
  else
    return textEditComment->text();
}

int DlgSettingsImageImp::backgroundType() const
{
  return comboBackground->currentItem();
}

void DlgSettingsImageImp::onSelectedFilter( const QString& filter )
{
  bool ok = ( filter.startsWith("JPG") || filter.startsWith("JPEG") || filter.startsWith("PNG") );	
  buttonGroupComment->setEnabled( ok );
}

void DlgSettingsImageImp::adjustImageSize(float fRatio)
{
  // if width has changed then adjust height and vice versa, if both has changed then adjust width
  if ( _height != spinHeight->value() )
  {
    _height = spinHeight->value();
    _width = (int)((float)_height*fRatio);
    spinWidth->setValue( (int)_width );
  } else // if( _width != spinWidth->value() )
  {
    _width = spinWidth->value();
    _height = (int)((float)_width/fRatio);
    spinHeight->setValue( (int)_height );
  }
}

void DlgSettingsImageImp::onRatioScreen()
{
  adjustImageSize(_fRatio);
}

void DlgSettingsImageImp::onRatio4x3()
{
  adjustImageSize(4.0f/3.0f);
}

void DlgSettingsImageImp::onRatio16x9()
{
  adjustImageSize(16.0f/9.0f);
}

void DlgSettingsImageImp::onRatio1x1()
{
  adjustImageSize(1.0f);
}


#include "DlgSettingsImage.cpp"
#include "moc_DlgSettingsImage.cpp"
