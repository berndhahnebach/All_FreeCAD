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

  // returns the root node of the 
  SoSeparator* getRoot(void){return pcRoot;}

  // 
  virtual void selected(View3DInventorViewer *, SoPath *){};
  virtual void unselected(View3DInventorViewer *, SoPath *){};

  /** @name direct handling methodes
    *  This group of methodes is to direct influenc the 
    *  apierence of the viewed content. Its only for fast
    *  interactions! If you whant to set the visual parameters
    *  you have to do it on the object viewed by this Provider!
    */
  //@{
  /// Set the transparency
  virtual void setTransparency(float)=0;
  //@}



protected:
  SoSeparator *pcRoot;

};

} // namespace Gui

#endif // __VIEWPROVIDER_H__

