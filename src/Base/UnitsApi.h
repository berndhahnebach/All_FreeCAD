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
#include <QString>


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
    static double translateUnit(const QString &);

    /** Units systems*/
    enum UnitSystem { 
        SI1      , /** internal (mm,kg,s) SI system (http://en.wikipedia.org/wiki/International_System_of_Units) */  
        SI2      , /** MKS (m,kg,s) SI system  */  
        Imperial1  /** the Imperial system (http://en.wikipedia.org/wiki/Imperial_units) */  
    } ;

    /** quantity types*/
    enum QuantityType{ 
        Length      ,   
        Area        ,   
        Volume      ,   
        Angle       , 
        TimeSpan    , 
        Velocity    , 
        Acceleration, 
        Mass        ,
        Temperature
    } ;

    /** @name Translation from internal to user prefs */
    //@{
    /// generate a string (UTF-8) for a quantity in user prefered system
    static QString toStrWithUserPrefs(QuantityType t,double Value);
    /// generate a python for a quantity in user prefered system
    static PyObject *toPyWithUserPrefs(QuantityType t,double Value);
    //@}

    /** @name Translation to internal regarding user prefs 
     * That means if no unit is issued the user prefs are in 
     * charge. If one unit is used the user prefs get ignored
     */
    //@{
    /// generate a value for a quantity with default user prefered system
    static double toDblWithUserPrefs(QuantityType t,const QString & Str);
    /// generate a value for a quantity with default user prefered system
    static double toDblWithUserPrefs(QuantityType t,const char* Str);
    /// generate a value for a quantity with default user prefered system
    static double toDblWithUserPrefs(QuantityType t,double UserVal);
    /// generate a value for a quantity with default user prefered system
    static double toDblWithUserPrefs(QuantityType t,PyObject *ArgObj);
   //@}

   /** @name query and set the user preferences */
    //@{
    /// set the default unit of a quantity type (e.g. m/s)
    static void setPrefOf(QuantityType t,const char* Str);
    /// get the default unit of a quantity (e.g. m/s)
    static const QString & getPrefUnitOf(QuantityType t);
    /// get the translation factor for the default unit of a quantity
    static const double getPrefFactorOf(QuantityType t);
    /// set the application defaults
    static void setDefaults(void);
   //@}

    double Result;

protected:

    // not used at the moment
    static UnitSystem  UserPrefSystem;

    /// cached factor to translate
    static double   UserPrefFactor [50] ;
    /// name of the unit the user whants to use as quantities
    static QString  UserPrefUnit   [50] ;

    // do the real work
    static double parse(const char*,bool &UsedUnit);

};

} // namespace Base


#endif // GUI_UnitsApi_H 
