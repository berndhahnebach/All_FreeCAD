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


#ifndef COIN_SOFCCOLORBAR_H
#define COIN_SOFCCOLORBAR_H

#include <Inventor/nodes/SoSeparator.h>
#include <Base/Observer.h>
#include <App/ColorModel.h>
#include <qdatetime.h>
#include <vector>

class SoSwitch;
class SoEventCallback;
class SbVec2s;
class SoHandleEventAction;

namespace Gui {
class SoFCColorGradient;

// abstract colorbar class
class SoFCColorBarBase : public SoSeparator, public App::ValueFloatToRGB {
  typedef SoSeparator inherited;

  SO_NODE_HEADER(Gui::SoFCColorBarBase);

public:
  static void initClass(void);

  virtual bool isVisible (float fVal) const { return false; };
  virtual float getMinValue (void) const { return 0.0f; };
  virtual float getMaxValue (void) const { return 0.0f; };
  virtual bool customize(){ return false; };
  virtual App::Color getColor(float) const { return App::Color(); }
  virtual const char* getColorBarName() const { return ""; };

protected:
  SoFCColorBarBase (void);
  virtual ~SoFCColorBarBase ();
};

class GuiExport SoFCColorBar : public SoSeparator, public Base::Subject<int> {
  typedef SoSeparator inherited;

  SO_NODE_HEADER(Gui::SoFCColorBar);

public:
  static void initClass(void);
  SoFCColorBar(void);

  void setViewerSize( const SbVec2s& size );
  SoFCColorBarBase* getActiveBar() const;
  SoFCColorGradient* getGradient() const { return pGradient; }

  void  handleEvent (SoHandleEventAction *action);

protected:
  virtual ~SoFCColorBar();

private:
  static void eventCallback(void * userdata, SoEventCallback * node);

private:
  float _fMaxX, _fMinX, _fMaxY, _fMinY;
  QTime _timer;

  SoSwitch* pColorMode; 
  std::vector<SoFCColorBarBase*> _colorBars;
  SoFCColorGradient* pGradient;
};

} // namespace Gui


#endif // COIN_SOFCCOLORBAR_H

