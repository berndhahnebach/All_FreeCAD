/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <assert.h>
# include <stdio.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Macro.h"

#include <Base/Interpreter.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Application.h>

#include "DockWindowManager.h"
#include "MainWindow.h"
#include "PythonConsole.h"

using namespace Gui;


MacroManager::MacroManager()
  : openMacro(false), recordGui(true), guiAsComment(true),scriptToPyConsole(false),pyConsole(0)
{
    // Attach to the Parametergroup regarding macros
    this->params = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Macro");
    this->params->Attach(this);
    this->params->NotifyAll();
}

MacroManager::~MacroManager()
{
    this->params->Detach(this);
}

void MacroManager::OnChange(Base::Subject<const char*> &rCaller, const char * sReason)
{
    this->recordGui         = this->params->GetBool("RecordGui", true);
    this->guiAsComment      = this->params->GetBool("GuiAsComment", true);
    this->scriptToPyConsole = this->params->GetBool("ScriptToPyConsole", false);
}

void MacroManager::open(MacroType eType,const char *sName)
{
    // check 
    assert(!this->openMacro);
    assert(eType == File);

    // Convert from Utf-8
    this->macroName = QString::fromUtf8(sName);
    if (!this->macroName.endsWith(".FCMacro"))
        this->macroName += ".FCMacro";

    this->macroInProgress = "";
    this->openMacro = true;

    Base::Console().Log("CmdM: Open macro: %s\n", sName);
}

void MacroManager::commit(void)
{
    QFile file(this->macroName);
    if (file.open(QFile::WriteOnly)) 
    {
        // sort import lines and avoid duplicates
        QTextStream str(&file);
        QStringList lines = this->macroInProgress.split('\n');
        QStringList import; import << "import FreeCAD\n";
        QStringList body;

        QStringList::Iterator it;
        for ( it = lines.begin(); it != lines.end(); ++it )
        {
            if ( (*it).startsWith("import ") || (*it).startsWith("#import ") )
            {
                if (import.indexOf( *it + '\n' ) == -1)
                    import.push_back( *it + '\n' );
            }
            else
            {
                body.push_back( *it + '\n' );
            }
        }

        QString header = "# Macro Begin: ";
        header += this->macroName;
        header += " +++++++++++++++++++++++++++++++++++++++++++++++++\n";

        QString footer = "# Macro End: ";
        footer += this->macroName;
        footer += " +++++++++++++++++++++++++++++++++++++++++++++++++\n";

        // write the data to the text file
        str << header;
        for ( it = import.begin(); it != import.end(); ++it )
            str << (*it);
        str << '\n';
        for ( it = body.begin(); it != body.end(); ++it )
            str << (*it);
        str << footer;
    }

    Base::Console().Log("CmdM: Commit macro: %s\n",(const char*)this->macroName.toUtf8());

    this->macroInProgress = "";
    this->macroName = "";
    this->openMacro = false;
}

void MacroManager::cancel(void)
{
    Base::Console().Log("CmdM: Cancel macro: %s\n",(const char*)this->macroName.toUtf8());

    this->macroInProgress = "";
    this->macroName = "";
    this->openMacro = false;
}

void MacroManager::addLine(LineType Type, const char* sLine)
{
    if (this->openMacro)
    {
        if(Type == Gui)
        {
            if (this->recordGui && this->guiAsComment)
                this->macroInProgress += "#";
            else if (!this->recordGui)
                return; // ignore Gui commands
        }

        this->macroInProgress += sLine;
        this->macroInProgress += "\n";
    }

    if (this->scriptToPyConsole) {
        // search for the Python console
        if (!this->pyConsole)
            this->pyConsole = Gui::getMainWindow()->findChild<Gui::PythonConsole*>();
        // Python console found?
        if (this->pyConsole)
            this->pyConsole->printStatement(sLine);
    }
}

void MacroManager::setModule(const char* sModule)
{
    if (this->openMacro && sModule && *sModule != '\0')
    {
        this->macroInProgress += "import ";
        this->macroInProgress += sModule;
        this->macroInProgress += "\n";
    }
}

void MacroManager::run(MacroType eType,const char *sName)
{
    try
    {
        //FIXME: The given path name is expected to be Utf-8 but std::ifstream isn't able to open
        //the file if contains non-ASCII characters. So, we open it with QFile which works reliably.
        //Base::Interpreter().runFile(sName);
        QString fileName = QString::fromUtf8(sName);
        QFile file(fileName);
        if (file.open(QFile::ReadOnly))
        {
            QTextStream str(&file);
            QString content = str.readAll();
            Base::Interpreter().runString(content.toAscii());
        }
        else
        {
            std::string err = "Unknown file: ";
            err += sName;
            err += "\n";
            throw Base::Exception(err);
        }
    }
    catch (const Base::Exception& e)
    {
        qWarning(e.what());
    }
}
