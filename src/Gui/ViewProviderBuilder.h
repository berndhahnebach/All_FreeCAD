/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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


#ifndef GUI_VIEWPROVIDERBUILDER_H
#define GUI_VIEWPROVIDERBUILDER_H

#include <vector>
#include <map>
#include <memory>
#include <Base/Type.h>
#include <Base/Factory.h>

class SoNode;

namespace App {
    class Property;
}

namespace Gui
{

class GuiExport ViewProviderBuilder
{
public:
    /// Constructor
    ViewProviderBuilder(void);
    virtual ~ViewProviderBuilder();
    virtual void buildNodes(const App::Property*, std::vector<SoNode*>&) const = 0;

    static void addBuilder(const Base::Type&, Base::AbstractProducer*);
    static std::auto_ptr<ViewProviderBuilder> createBuilder(const Base::Type&);

private:
    static std::map<Base::Type, Base::AbstractProducer*> _producers;
};

class GuiExport ViewProviderColorBuilder : public ViewProviderBuilder
{
public:
    /// Constructor
    ViewProviderColorBuilder(void);
    virtual ~ViewProviderColorBuilder();
    virtual void buildNodes(const App::Property*, std::vector<SoNode*>&) const;
};

template <class CLASS>
class ViewProviderProducer : public Base::AbstractProducer
{
public:
    /** 
     * Register a special type of widget to the WidgetFactoryInst.
     */
    ViewProviderProducer (const Base::Type& type)
    {
        ViewProviderBuilder::addBuilder(type, this);
    }

    ~ViewProviderProducer (){}

    /** 
     * Creates an instance of the specified widget.
     */
    virtual void* Produce (void) const
    {
        return (new CLASS);
    }
};

} //namespace Gui


#endif // GUI_VIEWPROVIDERBUILDER_H
