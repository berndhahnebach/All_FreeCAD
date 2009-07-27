/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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


#include <FCConfig.h>

#if HAVE_CONFIG_H
#   include <config.h>
#endif // HAVE_CONFIG_H

#include <Python.h>

// FreeCAD Base header
#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <App/Application.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>


#ifdef FC_OS_WIN32
#   define MainExport __declspec(dllexport)
#else
#   define MainExport
#endif

extern "C"
{
    void MainExport initFreeCADGui()
    {
        try {
            Base::Interpreter().loadModule("FreeCAD");
            App::Application::Config()["AppIcon"] = "FCIcon";
            App::Application::Config()["ConsoleBanner"] = "(c) Juergen Riegel, Werner Mayer 2001-2009\n";
            Gui::Application::initApplication();
            static Gui::Application *app = new Gui::Application();
        }
        catch (const Base::Exception& e) {
            std::string appName = App::Application::Config()["ExeName"];
            std::stringstream msg;
            msg << "While initializing " << appName << " the  following exception occurred: '"
                << e.what() << "'\n\n";
            msg << "\nPlease contact the application's support team for more information.\n\n";
            std::cout << "Initialization of " << appName << " failed:\n" << msg.str();
        }
        catch (...) {
            std::string appName = App::Application::Config()["ExeName"];
            std::stringstream msg;
            msg << "Unknown runtime error occurred while initializing " << appName << ".\n\n";
            msg << "\nPlease contact the application's support team for more information.\n\n";
            std::cout << "Initialization of " << appName << " failed:\n" << msg.str();
        }
    }
} // extern "C"

