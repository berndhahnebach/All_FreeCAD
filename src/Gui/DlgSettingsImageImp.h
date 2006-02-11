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


#ifndef GUI_DIALOG_DLGSETTINGSIMAGE_IMP_H
#define GUI_DIALOG_DLGSETTINGSIMAGE_IMP_H

#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SbMatrix.h>
#include <qcolor.h>

#include "DlgSettingsImage.h"

namespace Gui {
namespace Dialog {
/**
 * The DlgSettings3DViewImp class implements a preference page to change settings
 * for the Inventor viewer.
 * @author Werner Mayer
 */
class DlgSettingsImageImp : public DlgSettingsImage
{
public:
  DlgSettingsImageImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DlgSettingsImageImp();

  /** @name Image dimensions */
  //@{
  void setImageSize( int, int );
  void setImageSize( const QSize& );
  QSize imageSize() const;
  int imageWidth() const;
  int imageHeight() const;

  void setPixelsPerInch ( float );
  float pixelsPerInch() const;
  //@}

  /** @name Image background */
  //@{
  void setImageBackgroundColor( const QColor& );
  QColor imageBackgroundColor() const;
  void setImageFormat( SoOffscreenRenderer::Components  );
  SoOffscreenRenderer::Components imageFormat() const;
  void onSelectedFilter( const QString& );
  //@}

  /** @name Matrix handling (MIBA) */
  //@{
  void setMatrix(const SbMatrix &Matrix){_Matrix = Matrix;}
  virtual void onInsertDateTime();
  virtual void onInsertMIBA();
  virtual void onInsertViewMatrix();
  //@}

protected:
  void onAdjustImageSize();

private:
  float _fRatio;
  int _width, _height;
  SbMatrix _Matrix;
};

} // namespace Dialog
} // namespace Gui

#endif // GUI_DIALOG_DLGSETTINGSIMAGE_IMP_H
