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
# include <qcheckbox.h>
# include <qcombobox.h>
# include <qmap.h>
#endif

#include "DlgSettingsImageImp.h"
#include "SpinBox.h"

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


void DlgSettingsImageImp::insertMIBA()
{
  std::stringstream com;
  
//	SbVec2s size = this->getGLSize();	
//	float aspect = float(size[0]) / float(size[1]);	
	float aspect = float(_width) / float(_height);	
	float ax = 1.0f, ay = 1.0f;

	if (aspect > 1.0f)
	{
	  ax = 1.0f / aspect;
	}
	else
	{
	  ay = aspect;
	}
	  
	_Matrix[0][0] *= ay;
	_Matrix[1][1] *= ax;

  com << setw(7) << setfill(' ') << fixed;
  com << "<MIBA xmlns:xsl=\"http://www.w3.org/2001/XMLSchema-instance\" xsl:noNameSpaceSchemaLocation=\"MIBA.xsd\"> \n" ;
  com << " <View>\n"; 
  com << "  <Matrix \n"; 
  com << "     a11=" << _Matrix[0][0] <<" a12=" << _Matrix[0][1] <<" a13=" << _Matrix[0][2] <<" a14=" << _Matrix[0][3] << "\n";
  com << "     a21=" << _Matrix[1][0] <<" a22=" << _Matrix[1][1] <<" a23=" << _Matrix[1][2] <<" a24=" << _Matrix[1][3] << "\n";
  com << "     a31=" << _Matrix[2][0] <<" a32=" << _Matrix[2][1] <<" a33=" << _Matrix[2][2] <<" a34=" << _Matrix[2][3] << "\n";
  com << "     a41=" << _Matrix[3][0] <<" a42=" << _Matrix[3][1] <<" a43=" << _Matrix[3][2] <<" a44=" << _Matrix[3][3] << "\n";
  com << "   />\n" ; 
  com << " </View>\n" ; 
  com << " <CAD>\n" ; 
  com << "  <Erzeugendes_System> FreeCAD </Erzeugendes_System>\n" ;
  com << "  <Datum> 1968-12-17T04:00:00 </Datum>\n";
  com << "  <Bearbeiter> riegel </Bearbeiter>\n" ;
  com << " </CAD>\n" ;
  com << "</MIBA>\n" ;

  textEditComment->setText(QString(com.str().c_str()));

}


void DlgSettingsImageImp::insertViewMatrix()
{
  std::stringstream com;
  com << "Matrix=[" << _Matrix[0][0] <<"," << _Matrix[0][1] <<"," << _Matrix[0][2] <<"," << _Matrix[0][3] << ",";
  com << _Matrix[1][0] <<"," << _Matrix[1][1] <<"," << _Matrix[1][2] <<"," << _Matrix[1][3] << ",";
  com << _Matrix[2][0] <<"," << _Matrix[2][1] <<"," << _Matrix[2][2] <<"," << _Matrix[2][3] << ",";
  com << _Matrix[3][0] <<"," << _Matrix[3][1] <<"," << _Matrix[3][2] <<"," << _Matrix[3][3] << "]";

  textEditComment->setText(QString(com.str().c_str()));
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

  if ( !checkTransparent->isChecked() && colormap.find( comboColor->currentItem() ) != colormap.end() )
    return colormap[comboColor->currentItem()];

  // either default viewer background or transparent background
  return QColor();
}

/**
 * Sets the type of image format to \a f.
 */
void DlgSettingsImageImp::setImageFormat( SoOffscreenRenderer::Components f )
{
  switch ( f )
  {
  case SoOffscreenRenderer::LUMINANCE:
    comboImageType->setCurrentItem(0);
    checkTransparent->setChecked(false);
    break;
  case SoOffscreenRenderer::RGB:
    comboImageType->setCurrentItem(1);
    checkTransparent->setChecked(false);
    break;
  case SoOffscreenRenderer::LUMINANCE_TRANSPARENCY:
    comboImageType->setCurrentItem(0);
    checkTransparent->setChecked(true);
    break;
  case SoOffscreenRenderer::RGB_TRANSPARENCY:
    comboImageType->setCurrentItem(1);
    checkTransparent->setChecked(true);
    break;
  default:
    break;
  }
}

/**
 * Returns the current image format.
 */
SoOffscreenRenderer::Components DlgSettingsImageImp::imageFormat() const
{
  if ( !checkTransparent->isChecked() )
  {
    if ( comboImageType->currentItem() == 0 )
      return SoOffscreenRenderer::LUMINANCE;
    else
      return SoOffscreenRenderer::RGB;
  }
  else // transparent background
  {
    if ( comboImageType->currentItem() == 0 )
      return SoOffscreenRenderer::LUMINANCE_TRANSPARENCY;
    else
      return SoOffscreenRenderer::RGB_TRANSPARENCY;
  }
}

void DlgSettingsImageImp::onAdjustImageSize()
{
  // if width has changed then adjust height and vice versa, if both has changed the adjust height
  if ( _width != spinWidth->value() )
  {
    _width = spinWidth->value();
    _height = (int)((float)_width/_fRatio);
    spinHeight->setValue( _height );
  }
  else if ( _height != spinHeight->value() )
  {
    _height = spinHeight->value();
    _width = (int)((float)_height*_fRatio);
    spinWidth->setValue( _width );
  }
}

#include "DlgSettingsImage.cpp"
#include "moc_DlgSettingsImage.cpp"
