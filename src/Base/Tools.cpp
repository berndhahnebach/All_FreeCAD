/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#ifndef _PreComp_
#endif

#include <csignal>
#include "Tools.h"
#include "Exception.h"

using namespace Base;

SignalHandler::SignalHandler()
{
    segv_handler = std::signal(SIGSEGV,segfault_handler);
    abrt_handler = std::signal(SIGABRT,segfault_handler);
}

SignalHandler::~SignalHandler()
{
    std::signal(SIGSEGV,segv_handler);
    std::signal(SIGABRT,abrt_handler);
}

void SignalHandler::segfault_handler(int sig)
{
    switch (sig) {
        case SIGSEGV:
#ifdef FC_DEBUG
            std::cerr << "Illegal storage access, try to continue..." << std::endl;
#endif
            throw Base::AccessViolation();
            break;
        case SIGABRT:
#ifdef FC_DEBUG
            std::cerr << "Abnormal program termination, try to continue..." << std::endl;
#endif
            throw Base::AbnormalProgramTermination();
            break;
        default:
#ifdef FC_DEBUG
            std::cerr << "Unknown error occurred, try to continue..." << std::endl;
#endif
            throw Base::Exception("Unknown error occurred");
            break;
    }
}
