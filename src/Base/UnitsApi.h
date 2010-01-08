/***************************************************************************
 *   Copyright (c) 2009 Juergen Riegel  (FreeCAD@juergen-riegel.net>              *
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


#ifndef GUI_UnitsApi_H
#define GUI_UnitsApi_H

#include <string>
#include <Python.h>


namespace Base {
    

/**
 * The UnitsApi
 */
class BaseExport UnitsApi 
{

public:
    /** Constructs a UnitsApi object. */
    UnitsApi(const char* filter);
    UnitsApi(const std::string& filter);
    virtual ~UnitsApi();

    /// raw parser interface to calculat units (only from and to internal)
    static double translateUnit(const char*);
    static double translateUnit(const std::string&);

    /** Units systems*/
    enum UnitSystem { 
        SI      , /** the SI system (http://en.wikipedia.org/wiki/International_System_of_Units) */  
        Imperial  /** the Imperial system (http://en.wikipedia.org/wiki/Imperial_units) */  
    } ;

    /** @name Translation from internal to user prefs */
    //@{
    /// generate a string for a length quantity in user prefered system
    static std::string toStrWithUserPrefsLength(double Value);
    /// generate a string for a angle quantity in user prefered system
    static std::string toStrWithUserPrefsAngle(double Value);
   //@}

   /** @name Translation to internal regarding user prefs 
     * That means if no unit is issued the user prefs are in 
     * charge. If one unit is used the user prefs get ignored
     */
    //@{
    /// generate a value for a length quantity with default user prefered system
    static double toDblWithUserPrefsLength(const char* Str);
    /// generate a value for a length quantity with default user prefered system
    static double toDblWithUserPrefsLength(double UserVal);
    /// generate a value for a length quantity with default user prefered system
    static double toDblWithUserPrefsLength(PyObject *ArgObj);
    /// generate a value for a angle quantity with default user prefered system
    static double toDblWithUserPrefsAngle(const char* Str);
   //@}


    double Result;

protected:

    // not used at the moment
    static UnitSystem  UserPrefSystem;

    /// cached factor to translate
    static double      UserPrefLengthFactor;
    /// name of the unit the user whants to use as standard length
    static std::string UserPrefLengthSymbol;

    /// cached factor to translate
    static double      UserPrefAngleFactor;
    /// name of the unit the user whants to use as standard angle
    static std::string UserPrefAngleSymbol;

    // do the real work
    static double parse(const char*);

};

} // namespace Base


#endif // GUI_UnitsApi_H 
