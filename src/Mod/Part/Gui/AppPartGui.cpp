/** \file AppPartGui.cpp
 *  \brief 
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 
#include "../../../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif

#include "../../../App/Application.h"

void CreateCommands(void);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}



// python intry
extern "C" {
#ifdef _DEBUG
void __declspec(dllexport) initAppPartGuiD() {
#else
void __declspec(dllexport) initAppPartGui() {
#endif

	puts("AppPart loaded");

	GetApplication();

	// instanciating the commands
	CreateCommands();


	return;
}
} // extern "C" {




/*
#include <boost/python/class_builder.hpp>
namespace python = boost::python;

std::string info() { return "hello, world"; }


BOOST_PYTHON_MODULE_INIT(getting_started1)
{
    // Create an object representing this extension module.
    python::module_builder this_module("getting_started1");

    // Add regular functions to the module.
    this_module.def(info, "Info");
    //this_module.def(square, "square");
}
*/