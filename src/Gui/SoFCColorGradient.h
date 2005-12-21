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


#ifndef COIN_SOFCCOLORGRADIENT_H
#define COIN_SOFCCOLORGRADIENT_H

#include <Inventor/nodes/SoSeparator.h>
#include <App/ColorModel.h>

class SoCoordinate3;
class SoMFString;
class SbVec2s;

namespace Gui {

class GuiExport SoFCColorGradient : public SoSeparator {
  typedef SoSeparator inherited;

  SO_NODE_HEADER(Gui::SoFCColorGradient);

public:
  static void initClass(void);
  SoFCColorGradient(void);

  void setViewerSize( const SbVec2s& size );

  /**
   * Sets the range of the colorbar from the maximum \a fMax to the minimum \a fMin.
   * \a prec indicates the post decimal positions, \a prec should be in between 0 and 6.
   */
  void setRange( float fMin, float fMax, int prec=3 );
  /**
   * Sets the color model of the underlying color gradient to \a tModel. \a tModel either can
   * be \c TRIA, \c INVERSE_TRIA or \c GRAY
   */
  void setColorModel (App::ColorGradient::TColorModel tModel);
  /**
   * Sets the color style of the underlying color gradient to \a tStyle. \a tStyle either can
   * be \c FLOW or \c ZERO_BASED
   */
  void setColorStyle (App::ColorGradient::TStyle tStyle);

  unsigned short getColorIndex (float fVal) const { return _cColGrad.getColorIndex(fVal);  }
  App::Color getColor (float fVal) const { return _cColGrad.getColor(fVal); }
  bool isVisible (float fVal) const;
  float getMinValue (void) const { return _cColGrad.getMinValue(); }
  float getMaxValue (void) const { return _cColGrad.getMaxValue(); }
  unsigned long countColors (void) const { return _cColGrad.getCountColors(); }

//  virtual void handleEvent(SoHandleEventAction * action);
//  virtual void GLRenderBelowPath(SoGLRenderAction * action);
//  virtual void GLRenderInPath(SoGLRenderAction * action);

protected:
  virtual ~SoFCColorGradient();
  void setMarkerLabel( const SoMFString& label );
  void rebuild();
//  virtual void redrawHighlighted(SoAction * act, SbBool  flag);

  SoCoordinate3* coords;
  SoSeparator* labels;

private:
  float _fPosX, _fPosY;
  bool  _bOutInvisible;
  App::ColorGradient _cColGrad;
};

} // namespace Gui


#endif // COIN_SOFCCOLORGRADIENT_H

