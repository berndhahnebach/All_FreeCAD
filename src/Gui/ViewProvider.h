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

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

class SoNode;
class SoPath;
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
class ViewProviderPy;
class ObjectItem;



class GuiExport ViewProvider : public App::PropertyContainer
{
    PROPERTY_HEADER(Gui::ViewProvider);

public:
    /// constructor.
    ViewProvider();

    /// destructor.
    virtual ~ViewProvider();

    // returns the root node of the Provider (3D)
    virtual SoSeparator* getRoot(void){return pcRoot;}
    // returns the root for the Annotations. 
    SoSeparator* getAnnotation(void);
    // returns the root node of the Provider (3D)
    virtual SoSeparator* getFrontRoot(void) const {return 0;}
    // returns the root node of the Provider (3D)
    virtual SoSeparator* getBackRoot(void) const {return 0;}
    virtual void select(SoPath*) {}
    virtual void deselect(SoPath*) {}
    virtual QIcon getIcon(void) const;

    /** update the content of the ViewProvider
     * this method have to implement the recalcualtion
     * of the ViewProvider. There are different reasons to 
     * update. E.g. only the view attribute has changed, or
     * the data has manipulated.
     */
    void update(const App::Property*);
    virtual void updateData(const App::Property*)=0;

    std::string toString() const;
    PyObject* getPyObject();

    /** @name Display mode methods 
     */
    //@{
    std::string getActiveDisplayMode(void) const;
    /// set the display mode
    virtual void setDisplayMode(const char* ModeName);
    /// get the default display mode
    virtual const char* getDefaultDisplayMode() const=0;
    /// returns a list of all possible display modes
    virtual std::vector<std::string> getDisplayModes(void) const=0;

    virtual void setEdit(void){};
    virtual void unsetEdit(void){};
    virtual const char* getEditModeName(void){return 0;}

    virtual void hide(void);
    virtual void show(void);
    virtual bool isShow(void) const;
    //@}

    /** @name direct handling methods
     *  This group of methods is to direct influence the 
     *  appearence of the viewed content. It's only for fast
     *  interactions! If you want to set the visual parameters
     *  you have to do it on the object viewed by this provider!
     */
    //@{
    /// set the viewing transformation of the provider
    virtual void setTransformation(const Base::Matrix4D &rcMatrix);
    virtual void setTransformation(const SbMatrix &rcMatrix);
    SbMatrix convert(const Base::Matrix4D &rcMatrix) const;
    //@}

protected:
    /** @name Display mask modes
     * Mainly controls an SoSwitch node which selects the display mask modes.
     * The number of display mask modes doesn't necessarily match with the number
     * of display modes.
     * E.g. various display modes like Gaussian curvature, mean curvature or gray
     * values are displayed by one display mask mode that handles color values.
     */
    //@{
    /// Adds a new display mask mode 
    void addDisplayMaskMode( SoNode *node, const char* type );
    /// Activates the display mask mode \a type
    void setDisplayMaskMode( const char* type );
    /// Returns a list of added display mask modes
    std::vector<std::string> getDisplayMaskModes() const;
    //@}
	/// The root Separator of the ViewProvider
    SoSeparator *pcRoot;
    /// this is transformation for the provider
    SoTransform *pcTransform;
    const char* sPixmap;
    /// this is the mode switch, all the different viewing modes are collected here
    SoSwitch    *pcModeSwitch;
    /// The root separator for annotations
    SoSeparator *pcAnnotation;

private:
    int _iActualMode;
    std::string _sCurrentMode;
    std::map<std::string, int> _sDisplayMaskModes;
    ViewProviderPy* _pyObject;
};

} // namespace Gui

#endif // __VIEWPROVIDER_H__

