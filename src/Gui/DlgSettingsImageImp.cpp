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
# include <qmap.h>
#endif

#include "DlgSettingsImageImp.h"
#include "SpinBox.h"

using namespace Gui::Dialog;

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

  _width = w;
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

  _width = s.width();
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
 * Sets the value of pixel per inch to \r. 
 */
void DlgSettingsImageImp::setPixelsPerInch ( float r )
{
  spinResolution->setValue( r );
}

/**
 * Returns the current value for pixel per inch.
 */
float DlgSettingsImageImp::pixelsPerInch() const
{
  return spinResolution->value();
}

/**
 * Sets the background color of the image to \a c.
 */
void DlgSettingsImageImp::setImageBackgroundColor( const QColor& c )
{
  QMap<int, QColor> colormap;
  colormap[1] = Qt::white;
  colormap[2] = Qt::black;
  colormap[3] = Qt::red;
  colormap[4] = Qt::green;
  colormap[5] = Qt::blue;
  colormap[6] = Qt::cyan;
  colormap[7] = Qt::magenta;
  colormap[8] = Qt::yellow;
  colormap[9] = Qt::gray;

  comboColor->setCurrentItem(0);
  for ( QMap<int, QColor>::Iterator it = colormap.begin(); it != colormap.end(); ++it )
  {
    if ( it.data() == c )
    {
      comboColor->setCurrentItem(it.key());
      break;
    }
  }
}

/**
 * Returns the current background color.
 */
QColor DlgSettingsImageImp::imageBackgroundColor() const
{
  QMap<int, QColor> colormap;
  colormap[1] = Qt::white;
  colormap[2] = Qt::black;
  colormap[3] = Qt::red;
  colormap[4] = Qt::green;
  colormap[5] = Qt::blue;
  colormap[6] = Qt::cyan;
  colormap[7] = Qt::magenta;
  colormap[8] = Qt::yellow;
  colormap[9] = Qt::gray;

  if ( colormap.find( comboColor->currentItem() ) != colormap.end() )
    return colormap[comboColor->currentItem()];

  // default viewer background
  return QColor();
}

/**
 * Sets the type of image format to \a f.
 */
void DlgSettingsImageImp::setImageFormat( SoOffscreenRenderer::Components f )
{
  comboImageType->setCurrentItem(((int)f)-1);
}

/**
 * Returns the current image format.
 */
SoOffscreenRenderer::Components DlgSettingsImageImp::imageFormat() const
{
  return SoOffscreenRenderer::Components(comboImageType->currentItem()+1);
}

void DlgSettingsImageImp::onAdjustImageSize()
{
  // if width has changed then adjust height and vice versa, if both has changed the adjust height
  if ( _width != spinWidth->value() )
  {
    _width = spinWidth->value();
    _height = _width/_fRatio;
    spinHeight->setValue( _height );
  }
  else if ( _height != spinHeight->value() )
  {
    _height = spinHeight->value();
    _width = _height*_fRatio;
    spinWidth->setValue( _width );
  }
}

#include "DlgSettingsImage.cpp"
#include "moc_DlgSettingsImage.cpp"
