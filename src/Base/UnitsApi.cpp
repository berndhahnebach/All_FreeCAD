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



UnitsApi::UnitsApi(const char* filter)
  : Filter(filter)
{
    parse();
}

UnitsApi::UnitsApi(const std::string& filter)
  : Filter(filter)
{
     parse();
}

UnitsApi::~UnitsApi()
{
}


bool UnitsApi::match(void)
{
    //assert(Ast);

    //for (std::vector< Node_Object *>::iterator it= Ast->Objects.begin();it!=Ast->Objects.end();++it){
    //    //const char* name = (*it)->Namespace->c_str();
    //    //const char* type = (*it)->ObjectType->c_str();
    //    int min;
    //    int max;

    //    if((*it)->Slice){
    //        min          = (*it)->Slice->Min;
    //        max          = (*it)->Slice->Max;
    //    }else{
    //        min          = 1;
    //        max          = 1;
    //    }

    //    std::string type_name = *(*it)->Namespace + "::" + *(*it)->ObjectType;
    //    std::vector<Gui::SelectionObject> temp = Gui::Selection().getSelectionEx(0,type_name.c_str());
    //    if ((int)temp.size()<min || (int)temp.size()>max)
    //        return false;
    //    Result.push_back(temp);
    //}
    return true;
}


void UnitsApi::addError(const char* e)
{
    Errors+=e;
    Errors += '\n';
}



// === Parser & Scanner stuff ===============================================

// include the Scanner and the Parser for the filter language

UnitsApi* ActUnit=0;
//Node_Block *TopBlock=0;

// error func
void yyerror(char *errorinfo)
	{  ActUnit->addError(errorinfo);  }

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

bool UnitsApi::parse(void)
{
    Errors = "";
    YY_BUFFER_STATE my_string_buffer = UnitsApi_scan_string (Filter.c_str());
    // be aware that this parser is not reentrant! Dont use with Threats!!!
    assert(!ActUnit);
    ActUnit = this;
    int my_parse_result  = yyparse ();
    ActUnit = 0;
    //Ast = TopBlock;
    //TopBlock = 0;
    UnitsApi_delete_buffer (my_string_buffer);

    return true;
}
