/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_VIEWPROVIDERPYTHONFEATURE_H
#define GUI_VIEWPROVIDERPYTHONFEATURE_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif


#include "ViewProviderDocumentObject.h"
#include <App/PropertyPythonObject.h>


namespace Gui {

class GuiExport ViewProviderPythonFeature : public ViewProviderDocumentObject
{
    PROPERTY_HEADER(Gui::ViewProviderPythonFeature);

public:
    /// constructor.
    ViewProviderPythonFeature();
    /// destructor.
    virtual ~ViewProviderPythonFeature();

    // Returns the icon
    QIcon getIcon() const;

    /** @name Update data methods*/
    //@{
    virtual void attach(App::DocumentObject *pcObject);
    virtual void updateData(const App::Property*);
    //@}

    /** @name Display methods */
    //@{
    /// get the default display mode
    virtual const char* getDefaultDisplayMode() const;
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;
    /// set the display mode
    virtual void setDisplayMode(const char* ModeName);
    //@}

    /** @name Edit methods*/
    //@{
    /// is called by the document when the provider goes in edit mode
    virtual bool setEdit(int ModNum = 0);
    /// is called when you loose the edit mode
    virtual void unsetEdit(void);
    /// is called when the provider is in edit and a key event occurs. Only ESC ends edit.
    virtual bool keyPressed(int key){return false;}
    /// Is called by the tree if the user double click on the object
    virtual bool doubleClicked(void){return false;}
    /// is called when the provider is in edit and the mouse is moved
    virtual bool mouseMove(const SbVec3f &pos, const SbVec3f &norm, const SoPickedPoint* pp)
    { return false; }
    /// is called when the Provider is in edit and the mouse is clicked 
    virtual bool mouseButtonPressed(int Button, bool pressed, const SbVec3f &pos,
                                    const SbVec3f &norm, const SoPickedPoint* pp)
    { return false; }
    //@}

    /** @name Selection methods */
    //@{
    virtual void select(SoPath*) {}
    virtual void deselect(SoPath*) {}
    //@}

    /** @name Property methods */
    //@{
    /// get all properties of the class (including parent)
    virtual void getPropertyMap(std::map<std::string,App::Property*> &Map) const;
    /// find a property by its name
    virtual App::Property *getPropertyByName(const char* name) const;
    /// get the name of a property
    virtual const char* getName(const App::Property* prop) const;
    App::Property* addDynamicProperty(const char* type, const char* name=0);
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);
    PyObject* getPyObject();
    //@}

private:
    virtual void onChanged(const App::Property* prop);
    std::string getUniquePropertyName(const char *Name) const;

private:
    std::map<std::string,App::Property*> objectProperties;
    App::PropertyPythonObject Proxy;
};


} // namespace Gui

#endif // GUI_VIEWPROVIDERPYTHONFEATURE_H

