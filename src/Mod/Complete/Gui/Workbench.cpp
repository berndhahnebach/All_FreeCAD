/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qobject.h>
#endif

#include "Workbench.h"
#include <Gui/ToolBarManager.h>
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>
#include <Gui/DockWindowManager.h>
#include <Gui/Application.h>
#include <Gui/Action.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <Gui/ToolBoxManager.h>
#include <App/Document.h>
#include <App/DocumentObject.h>


using namespace CompleteGui;

TYPESYSTEM_SOURCE(CompleteGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{
}

Workbench::~Workbench()
{
}



void Workbench::setupContextMenu(const char* recipient,Gui::MenuItem* item) const
{
    if (strcmp(recipient,"View") == 0)
    {
        Gui::MenuItem* StdViews = new Gui::MenuItem();
        StdViews->setCommand( "Standard views" );

        *StdViews << "Std_ViewAxo" << "Separator" << "Std_ViewFront" << "Std_ViewTop" << "Std_ViewRight"
                  << "Std_ViewRear" << "Std_ViewBottom" << "Std_ViewLeft";

        *item << "Std_ViewFitAll" << "Std_ViewFitSelection" << StdViews
              << "Separator" << "Std_ViewDockUndockFullscreen";

        if ( Gui::Selection().countObjectsOfType(App::DocumentObject::getClassTypeId()) > 0 )
            *item << "Separator" << "Std_SetMaterial" << "Std_ToggleVisibility" << "Std_TreeSelection" 
                  << "Std_RandomColor" << "Separator" << "Std_Delete";
    }
    else if (strcmp(recipient,"Tree") == 0)
    {
        if ( Gui::Selection().countObjectsOfType(App::DocumentObject::getClassTypeId()) > 0 )
            *item << "Std_SetMaterial" << "Std_ToggleVisibility" 
                  << "Std_RandomColor" << "Separator" << "Std_Delete";
    }
}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    // Setup the default menu bar
    Gui::MenuItem* menuBar = new Gui::MenuItem;

    // File
    Gui::MenuItem* file = new Gui::MenuItem( menuBar );
    file->setCommand(QT_TR_NOOP("&File"));
    *file << "Std_New" << "Std_Open" << "Std_Import" << "Std_Save" << "Std_SaveAs"  << "Std_ProjectInfo" 
          //<< "Separator" << "Std_Print" << "Std_PrintPdf"
          << "Separator" << "Std_RecentFiles" << "Separator" << "Std_Quit";

    // Edit
    Gui::MenuItem* edit = new Gui::MenuItem( menuBar );
    edit->setCommand(QT_TR_NOOP("&Edit"));
    *edit << "Std_Undo" << "Std_Redo" << "Separator" << "Std_Cut" << "Std_Copy" << "Std_Paste" 
          << "Separator" << "Std_Refresh" << "Std_SelectAll" << "Std_Delete"
          << "Separator" << "Std_DlgPreferences";

    // Standard views
    Gui::MenuItem* stdviews = new Gui::MenuItem;
    stdviews->setCommand(QT_TR_NOOP("Standard views"));
    *stdviews << "Std_ViewFitAll" << "Std_ViewFitSelection" << "Std_ViewAxo"
              << "Separator" << "Std_ViewFront" << "Std_ViewRight"
              << "Std_ViewTop" << "Separator" << "Std_ViewRear" 
              << "Std_ViewLeft" << "Std_ViewBottom";

    // stereo
    Gui::MenuItem* view3d = new Gui::MenuItem;
    view3d->setCommand(QT_TR_NOOP("&3D View"));
    *view3d << "Std_ViewIvStereoRedGreen" << "Std_ViewIvStereoQuadBuff" 
            << "Std_ViewIvStereoInterleavedRows" << "Std_ViewIvStereoInterleavedColumns" 
            << "Std_ViewIvStereoOff" << "Separator" << "Std_ViewIvIssueCamPos";

    // zoom
    Gui::MenuItem* zoom = new Gui::MenuItem;
    zoom->setCommand(QT_TR_NOOP("&Zoom"));
    *zoom << "Std_ViewZoomIn" << "Std_ViewZoomOut" << "Separator" << "Std_ViewBoxZoom";

    // View
    Gui::MenuItem* view = new Gui::MenuItem( menuBar );
    view->setCommand(QT_TR_NOOP("&View"));
    *view << "Std_ViewCreate" << "Std_OrthographicCamera" << "Std_PerspectiveCamera" << "Separator" 
          << stdviews << "Std_FreezeViews" << "Separator" << view3d << zoom
          << "Std_ViewDockUndockFullscreen" << "Std_ToggleClipPlane" << "Separator"
          << "Std_SetMaterial" << "Std_ToggleVisibility" << "Std_RandomColor" << "Separator" 
          << "Std_MeasureDistance" << "Separator" 
          << "Std_Workbench" << "Std_ToolBarMenu" << "Std_DockViewMenu" << "Separator" 
          << "Std_ViewStatusBar" << "Std_UserInterface";

    // Tools
    Gui::MenuItem* tool = new Gui::MenuItem( menuBar );
    tool->setCommand(QT_TR_NOOP("&Tools"));
    *tool << "Std_CommandLine" << "Std_DlgParameter" << "Separator" << "Std_DlgMacroRecord"
          << "Std_DlgMacroStop" << "Std_DlgMacroExecute" << "Std_DlgMacroExecuteDirect" 
          << "Separator" << "Std_ViewScreenShot" << "Separator" << "Std_DlgCustomize";

    // Windows
    Gui::MenuItem* wnd = new Gui::MenuItem( menuBar );
    wnd->setCommand(QT_TR_NOOP("&Windows"));
    *wnd << "Std_CloseActiveWindow" << "Std_CloseAllWindows" << "Separator" << "Std_ActivateNextWindow"
         << "Std_ActivatePrevWindow" << "Separator" << "Std_TileWindows" << "Std_CascadeWindows"
         << "Std_ArrangeIcons" << "Separator" << "Std_WindowsMenu" << "Std_Windows";

    // Separator
    Gui::MenuItem* sep = new Gui::MenuItem( menuBar );
    sep->setCommand( "Separator" );

    // Help
    Gui::MenuItem* helpWebsites = new Gui::MenuItem;
    helpWebsites->setCommand(QT_TR_NOOP("&Online-help"));
    *helpWebsites << "Std_OnlineHelpWebsite" << "Std_FreeCADWebsite" << "Std_PythonWebsite";
    
    Gui::MenuItem* help = new Gui::MenuItem( menuBar );
    help->setCommand(QT_TR_NOOP("&Help"));
    *help << "Std_OnlineHelp" << "Std_OnlineHelpPython" << "Std_PythonHelp"
          << helpWebsites  << "Std_TipOfTheDay"<< "Separator" << "Std_About"
          << "Std_AboutQt" << "Separator" << "Std_WhatsThis" << "Std_DescriptionMode";

    return menuBar;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = new Gui::ToolBarItem;

    // File
    Gui::ToolBarItem* file = new Gui::ToolBarItem( root );
    file->setCommand(QT_TR_NOOP("File"));
    *file << "Std_New" << "Std_Open" << "Std_Save" << "Std_Print" << "Separator" << "Std_Cut"
          << "Std_Copy" << "Std_Paste" << "Separator" << "Std_Undo" << "Std_Redo" << "Separator"
          << "Std_Refresh" << "Separator" << "Std_Workbench" << "Std_WhatsThis";

    // Macro
    Gui::ToolBarItem* macro = new Gui::ToolBarItem( root );
    macro->setCommand(QT_TR_NOOP("Macro"));
    *macro << "Std_DlgMacroRecord" << "Std_DlgMacroStop" << "Std_DlgMacroExecute"
           << "Std_DlgMacroExecuteDirect";

    // View
    Gui::ToolBarItem* view = new Gui::ToolBarItem( root );
    view->setCommand(QT_TR_NOOP("View"));
    *view << "Std_ViewFitAll" << "Separator" << "Std_ViewAxo" << "Separator" << "Std_ViewFront" 
          << "Std_ViewRight" << "Std_ViewTop" << "Separator" << "Std_ViewRear" << "Std_ViewLeft" 
          << "Std_ViewBottom";

    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    Gui::ToolBarItem* root = new Gui::ToolBarItem;

    //// View
    //ToolBarItem* view = new ToolBarItem( root );
    //view->setCommand(QT_TR_NOOP("Standard views"));
    //*view << "Std_ViewFitAll" << "Std_ViewFitSelection" << "Std_ViewAxo" << "Separator"
    //      << "Std_ViewFront" << "Std_ViewRight" << "Std_ViewTop" << "Separator"
    //      << "Std_ViewRear" << "Std_ViewLeft" << "Std_ViewBottom";
    //// Special Ops
    //ToolBarItem* macro = new ToolBarItem( root );
    //macro->setCommand(QT_TR_NOOP("Special Ops"));
    //*macro << "Std_DlgParameter" << "Std_DlgPreferences" << "Std_DlgMacroRecord" << "Std_DlgMacroStop" 
    //       << "Std_DlgMacroExecute" << "Std_DlgCustomize" << "Std_CommandLine";


    return root;
}
