/***************************************************************************
 *   Copyright (c) 2009 Juergen Riegel (FreeCAD@juergen-riegel.net)        *
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


#include "PreCompiled.h"
#ifdef __GNUC__
# include <unistd.h>
#endif

#include <strstream>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Base/Interpreter.h>

#include "UnitsApi.h"
//#include "UnitsApiPy.h"

using namespace Base;

// suppress annoying warnings from generated source files
#ifdef _MSC_VER
# pragma warning(disable : 4003)
# pragma warning(disable : 4018)
# pragma warning(disable : 4065)
# pragma warning(disable : 4335) // disable MAC file format warning on VC
#endif


// === static attributes  ================================================

UnitsApi::UnitSystem  UnitsApi::UserPrefSystem = SI1;

double   UnitsApi::UserPrefFactor [50];
QString  UnitsApi::UserPrefUnit   [50];


UnitsApi::UnitsApi(const char* filter)
{
    Result = parse(filter);
}

UnitsApi::UnitsApi(const std::string& filter)
{
    Result = parse(filter.c_str());
}

UnitsApi::~UnitsApi()
{
}

double UnitsApi::translateUnit(const char* str)
{
    return parse( str ); 
}

double UnitsApi::translateUnit(const QString & str)
{
    return parse( str.toUtf8() );  
}


// === static translation methodes ==========================================

QString UnitsApi::toStrWithUserPrefs(QuantityType t,double Value)
{
    return QString::fromLatin1("NULL");
}

PyObject *UnitsApi::toPyWithUserPrefs(QuantityType t,double Value)
{
    return 0;
}

double UnitsApi::toDblWithUserPrefs(QuantityType t,const QString & Str)
{
    return 0.0;
}

double UnitsApi::toDblWithUserPrefs(QuantityType t,const char* Str)
{
    return 0.0;
}

double UnitsApi::toDblWithUserPrefs(QuantityType t,double UserVal)
{
    return 0.0;
}

double UnitsApi::toDblWithUserPrefs(QuantityType t,PyObject *ArgObj)
{
    return 0.0;
}

void UnitsApi::setPrefOf(QuantityType t,const char* Str)
{
    double Factor = translateUnit(Str);
    UserPrefUnit[t] = QString::fromLatin1(Str);
    UserPrefFactor[t] = Factor;
}

const QString & UnitsApi::getPrefUnitOf(QuantityType t)
{
    return UserPrefUnit[t];
}
const double UnitsApi::getPrefFactorOf(QuantityType t)
{
    return UserPrefFactor[t];
}

void UnitsApi::setDefaults(void)
{
    setPrefOf(Length,"mm");
  
}




// === Parser & Scanner stuff ===============================================

// include the Scanner and the Parser for the filter language

UnitsApi* ActUnit=0;
double ScanResult=0;

// error func
void yyerror(char *errorinfo)
{  throw Base::Exception(errorinfo);  }

// show the parser the lexer method
#define yylex UnitsApilex
int UnitsApilex(void);

// for VC9 (isatty and fileno not supported anymore)
#ifdef _MSC_VER
int isatty (int i) {return _isatty(i);}
int fileno(FILE *stream) {return _fileno(stream);}
#endif


// Parser, defined in UnitsApi.y
#include "UnitsApi.tab.c"

// Scanner, defined in UnitsApi.l
#include "lex.UnitsApi.c"

double UnitsApi::parse(const char* buffer)
{
    YY_BUFFER_STATE my_string_buffer = UnitsApi_scan_string (buffer);
    // be aware that this parser is not reentrant! Dont use with Threats!!!
    //assert(!ActUnit);
    //ActUnit = this;
    yyparse ();
    //ActUnit = 0;
    //Ast = TopBlock;
    //TopBlock = 0;
    UnitsApi_delete_buffer (my_string_buffer);

    return ScanResult;
}
