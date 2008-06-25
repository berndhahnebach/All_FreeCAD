/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef PART_PROPERTYLINE_H
#define PART_PROPERTYLINE_H

#include <App/PropertyGeo.h>
#include <gp_Lin.hxx>

namespace Part
{

typedef std::pair<Base::Vector3f, Base::Vector3f> Line3f;

/** The line property class.
 * @author Werner Mayer
 */
class AppPartExport PropertyLine : public App::Property
{
    TYPESYSTEM_HEADER();

public:
    PropertyLine();
    ~PropertyLine();

    /** @name Getter/setter */
    //@{
    /// set the line
    void setValue( const Geom_Line& line );
    /// get the line
    const Geom_Line& getValue(void) const;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject(void);
    void setPyObject(PyObject *value);
    //@}

    /** @name Save/restore */
    //@{
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);

    App::Property *Copy(void) const;
    void Paste(const App::Property &from);
    unsigned int getMemSize (void) const;
    //@}

private:
    // start and end point of the line
    //Line3f _line;
    Geom_Line _line;
};

/** The line set property class.
 * @author Werner Mayer
 */
class AppPartExport PropertyLineSet : public App::PropertyLists
{
    TYPESYSTEM_HEADER();

public:
    PropertyLineSet();
    ~PropertyLineSet();

    /** @name Getter/setter */
    //@{
    void setSize(int newSize){_lValueList.resize(newSize);}   
    int getSize(void) const {return _lValueList.size();} 
    /// index operator
    const Line3f& operator[] (const int idx) const {return _lValueList.operator[] (idx);}
    void setValue( const Line3f& line );
    void set1Value (const int idx, const Line3f& value){_lValueList.operator[] (idx) = value;}
    void setValues (const std::vector<Line3f>& values);
    const std::vector<Line3f> &getValues(void) const {return _lValueList;}
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject(void);
    void setPyObject(PyObject *value);
    //@}

    /** @name Save/Restore */
    //@{
    void Save (Base::Writer &writer) const;
    void Restore(Base::XMLReader &reader);
    void SaveDocFile (Base::Writer &writer) const;
    void RestoreDocFile(Base::Reader &reader);
    //@}

    /** @name Copy/Paste */
    App::Property *Copy(void) const;
    void Paste(const App::Property &from);
    unsigned int getMemSize (void) const;
    //@}

private:
    std::vector<Line3f> _lValueList;
};

} // namespace Part

#endif // PART_PROPERTYLINE_H
