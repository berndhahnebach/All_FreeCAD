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

#include "ProgressIndicator.h"

using namespace Part;
/*!
  \code
  #include <XSControl_WorkSession.hxx>
  #include <Transfer_TransientProcess.hxx>

  STEPControl_Reader aReader;
  Handle_Message_ProgressIndicator pi = new ProgressIndicator(100);

  pi->NewScope(20, "Loading STEP file...");
  pi->Show();
  aReader.ReadFile("myfile.stp");
  pi->EndScope();

  Handle(StepData_StepModel) Model = aReader.StepModel();
  pi->NewScope(80, "Translating...");
  pi->Show();
  aReader.WS()->MapReader()->SetProgress(pi);
  Standard_Integer nbr = aReader.NbRootsForTransfer();
  for ( Standard_Integer n = 1; n<= nbr; n++) {
    ...
  }

  pi->EndScope();
  \encode
 */
ProgressIndicator::ProgressIndicator (int theMaxVal)
{
    this->cancel = Standard_False;
    this->myProgress = new Base::SequencerLauncher("", theMaxVal);
    SetScale (0, theMaxVal, 1);
}

ProgressIndicator::~ProgressIndicator ()
{
    delete this->myProgress;
}

Standard_Boolean ProgressIndicator::Show (const Standard_Boolean theForce)
{
    if (theForce) {
        Handle(TCollection_HAsciiString) aName = GetScope(1).GetName(); //current step
        if (!aName.IsNull())
            this->myProgress->setText (aName->ToCString());
    }

    //Standard_Real aPc = GetPosition(); //always within [0,1]
    //Standard_Integer aVal = (Standard_Integer)(aPc * GetValue());
    try {
        this->myProgress->next();
    }
    catch (const Base::AbortException&) {
        this->cancel = Standard_True;
    }

    return Standard_True;
}

Standard_Boolean ProgressIndicator::UserBreak()
{
    return this->cancel;
}
