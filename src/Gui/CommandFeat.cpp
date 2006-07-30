/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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

#include <App/DocumentObject.h>

#include "Application.h"
#include "Command.h"
#include "Document.h"
#include "Selection.h"
#include "ViewProvider.h"
#include "ViewProviderDocumentObject.h"

using namespace Gui;



//===========================================================================
// Std_Recompute
//===========================================================================

DEF_STD_CMD(StdCmdFeatRecompute);

StdCmdFeatRecompute::StdCmdFeatRecompute()
  :Command("Std_Recompute")
{
  // seting the
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Recompute");
  sToolTipText  = QT_TR_NOOP("Recompute feature or document");
  sWhatsThis    = QT_TR_NOOP("Recompute feature or document");
  sStatusTip    = QT_TR_NOOP("Recompute feature or document");
  sPixmap       = "Open";
  iAccel        = Qt::CTRL+Qt::Key_R;
}

void StdCmdFeatRecompute::activated(int iMsg)
{
  // fill the list of registered endings

}

//===========================================================================
// Std_RandomColor
//===========================================================================

DEF_STD_CMD_A(StdCmdRandomColor);

StdCmdRandomColor::StdCmdRandomColor()
  :Command("Std_RandomColor")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("Random color");
  sToolTipText  = QT_TR_NOOP("Random color");
  sWhatsThis    = QT_TR_NOOP("Random color");
  sStatusTip    = QT_TR_NOOP("Random color");
}

void StdCmdRandomColor::activated(int iMsg)
{
  // get the complete selection
  std::vector<SelectionSingleton::SelObj> sel = Selection().getCompleteSelection();
  for ( std::vector<SelectionSingleton::SelObj>::iterator it = sel.begin(); it != sel.end(); ++it ) {
    // get the Gui related document of an object
    Gui::Document* doc = Gui::Application::Instance->getDocument(it->pDoc);
    // get the view provider of an object over its Gui document
    ViewProvider* vp = doc->getViewProvider(it->pObject);
    if ( vp && vp->getTypeId().isDerivedFrom(ViewProviderDocumentObject::getClassTypeId()) ) {
      // set the <r,g,b> color triples for each object
      ViewProviderDocumentObject* obj = (ViewProviderDocumentObject*)vp;
      float fMax = (float)RAND_MAX;
      float fRed = (float)rand();
      float fGrn = (float)rand();
      float fBlu = (float)rand();
      obj->ShapeColor.setValue(fRed/fMax, fGrn/fMax, fBlu/fMax);
    }
  }
}

bool StdCmdRandomColor::isActive(void)
{
  return ( Gui::Selection().size() != 0 );
}


namespace Gui {

void CreateFeatCommands(void)
{
  CommandManager &rcCmdMgr = Application::Instance->commandManager();

  rcCmdMgr.addCommand(new StdCmdFeatRecompute());
  rcCmdMgr.addCommand(new StdCmdRandomColor());


}

} // namespace Gui
