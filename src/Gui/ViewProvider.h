/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __VIEWPROVIDER_H__
#define __VIEWPROVIDER_H__

#include <map>
#include <vector>
#include <string>
#include <qpixmap.h>

class SoNode;
class SoPath;
class QListViewItem;
class SoSeparator;
class SoEvent;
class SoSwitch;
class SoTransform;
class SbMatrix;


namespace Base {
  class Matrix4D;
}
namespace App {
  class Color;
}

#include <App/PropertyContainer.h>

namespace Gui {

class View3DInventorViewer;



class GuiExport ViewProvider: public App::PropertyContainer
{
    PROPERTY_HEADER(Gui::ViewProvider);

public:
  /// constructor.
  ViewProvider();

  /// destructor.
  virtual ~ViewProvider();

  // returns the root node of the Provider (3D)
  virtual SoSeparator* getRoot(void){return pcRoot;}
  // returns the root node of the Provider (3D)
  virtual SoSeparator* getFrontRoot(void){return 0;}
  // returns the root node of the Provider (3D)
  virtual SoSeparator* getBackRoot(void){return 0;}
  // returns the TreeLabel
  virtual QListViewItem* getTreeItem(QListViewItem* parent){return 0;}
  // returns the TreeLabel
  virtual QPixmap getIcon(void) const;


  /** update the content of the ViewProvider
    * this methode have to implement the recalcualtion
    * of the ViewProvider. There are different reasons to 
    * update. E.g. only the view attribute has changed, or
    * the data has manipulated.
    */
  virtual void update(void)=0;

protected:
	/** @name Display modes
   * Mainly controls a SoSwitch which selects the display modes.
   * The number of display modes doesn't necessarily match with the number of view modes.
   * E.g. various modes like Gaussian curvature, mean curvature or gray values are displayed
   * by one display mode that handles color values.
   */
	//@{
  /// Adds a new display mode 
  void addDisplayMode( SoNode *node, const char* type );
  /// Activates the display mode \a type
  void setDisplayMode( const char* type );
  /// Returns a list of added display modes
  std::vector<std::string> getDisplayModes() const;
  //@}

public:
	/** @name mode methods of the feature 
    */
	//@{
  /// set the viewing mode
  virtual void setMode(const char* ModeName);
  /// get the viewing mode name
  std::string getModeName(void) const;
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void)=0;

  virtual void setEdit(void){};
  virtual void unsetEdit(void){};
  virtual const char* getEditModeName(void){return 0;}

  virtual void hide(void);
  virtual void show(void);
  virtual bool isShow(void);
  //@}

  
  /** @name direct handling methods
    *  This group of methods is to direct influenc the 
    *  apierence of the viewed content. Its only for fast
    *  interactions! If you whant to set the visual parameters
    *  you have to do it on the object viewed by this Provider!
    */
  //@{
  /// Set the transparency
  virtual void setTransparency(float)=0;
  /// Set the color
  virtual void setColor(const App::Color &c)=0;
  /// Set the point size
  virtual void setPointSize(float)=0;
  /// Set the line width
  virtual void setLineWidth(float)=0;

  /// set the viewing transformation of the provider
  virtual void setTransformation(const Base::Matrix4D &rcMatrix);
  virtual void setTransformation(const SbMatrix &rcMatrix);
  //@}

  /** event handling if the feature is in edit mode
    * The Viewer will cast all left and right mouse 
    * events to that methode. If return true it will
    * also handled by the viewer (selection & context menue)
    */
  virtual bool handleEvent(const SoEvent * const ev,View3DInventorViewer &Viewer){return false;}

  /// freqently called by the framework to check status changes in the data
  virtual bool testStatus(void){return false;}

protected:
  SoSeparator *pcRoot;

  /// this is transformation for the provider
  SoTransform *pcTransform;

  const char* sPixmap;

  /// this is the mode switch, all the different viewing modes are collected here
  SoSwitch    *pcModeSwitch;

private:
  int _iActualMode;
  std::string _sCurrentMode;
  std::map<std::string, int> _sDisplayModes;
};

} // namespace Gui

#endif // __VIEWPROVIDER_H__

