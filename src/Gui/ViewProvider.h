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

#ifndef _PreComp_
# include <vector>
#endif

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

namespace Gui {

class View3DInventorViewer;


/** Base class of all view provider
 *  \author Jürgen Riegel
 */
class GuiExport ViewProvider
{
public:
  /**
   * A constructor.
   * A more elaborate description of the constructor.
   */
  ViewProvider();

  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  virtual ~ViewProvider();

	/** @name status methodes of the feature */
	//@{
  /// set the viewing mode
  virtual void setMode(const char* ModeName)=0;
  /// set the viewing mode as int
  virtual void setMode(int Mode)=0;
  /// get the viewing mode as int
  virtual int getMode(void)=0;
  /// get the viewing mode as int
  virtual std::string getModeName(void)=0;

  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void)=0;
  /// change types
  enum ChangeType 
  {
    All,
    View
  };

  /** update the content of the ViewProvider
    * this methode have to implement the recalcualtion
    * of the ViewProvider. There are different reasons to 
    * update. E.g. only the view attribute has changed, or
    * the data has manipulated.
    */
  virtual void update(const ChangeType&)=0;

	/** @name editing interface 
    * this methodes handling the edition of a 
    * viewprovider. Better the underlaing data.
    * This means also to handle Events of the viewer
    * in a special way.
    */
	//@{
  /** set the ViewProvider in edit mode
    * that means it get all left and right mouse down and 
    * mouse move events first and can handle it.
    * @param bOn true when switch on, false when switch of
    * @ret   false if the provider whants not go edit, or can not
    */

  virtual void hide(void){};
  virtual void show(void){};
  virtual bool isShow(void){return false;};

  virtual void setEdit(void);
  virtual void unsetEdit(void);
  virtual const char* getEditModeName(void){return 0;}

  //@}

};


class GuiExport ViewProviderTree:public ViewProvider
{
public:
  /**
   * A constructor.
   * A more elaborate description of the constructor.
   */
  ViewProviderTree();

  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  virtual ~ViewProviderTree();

  QListViewItem* create();
};


class GuiExport ViewProviderInventor:public ViewProvider
{
public:
  /// constructor.
  ViewProviderInventor();

  /// destructor.
  virtual ~ViewProviderInventor();

  // returns the root node of the Provider (3D)
  virtual SoSeparator* getRoot(void){return pcRoot;}
  // returns the root node of the Provider (3D)
  virtual SoSeparator* getFrontRoot(void){return 0;}
  // returns the root node of the Provider (3D)
  virtual SoSeparator* getBackRoot(void){return 0;}


	/** @name mode methodes of the feature 
    * manly stiar a SoSwitch (pcModeSwitch) which select the 
    * viewing mode.
    * @see pcModeSwitch
    */
	//@{
  /// set the viewing mode
  virtual void setMode(const char* ModeName);
  /// set the viewing mode as int
  virtual void setMode(int Mode);
  /// get the viewing mode as int
  virtual int getMode(void);
  /// get the viewing mode as int
  virtual std::string getModeName(void);
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void)=0;

  virtual void hide(void);
  virtual void show(void);
  virtual bool isShow(void);



  //@}

  
  /** @name direct handling methodes
    *  This group of methodes is to direct influenc the 
    *  apierence of the viewed content. Its only for fast
    *  interactions! If you whant to set the visual parameters
    *  you have to do it on the object viewed by this Provider!
    */
  //@{
  /// Set the transparency
  virtual void setTransparency(float)=0;

  /// Set the color
  virtual void setColor(const App::Color &c)=0;

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



protected:
  SoSeparator *pcRoot;

  /// this is the mode switch, all the different viewing modes are collected here
  SoSwitch    *pcModeSwitch;

  /// this is the mode switch, all the different viewing modes are collected here
  SoTransform *pcTransform;

  int _iActualMode;

};

} // namespace Gui

#endif // __VIEWPROVIDER_H__

