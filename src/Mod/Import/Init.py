# FreeCAD init script of the Import module  
# (c) 2001 Juergen Riegel

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                       *   
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU Lesser General Public License (LGPL)    *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Lesser General Public License for more details.                   *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        * 
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#*   Juergen Riegel 2002                                                   *
#***************************************************************************/



# Get the Parameter Group of this module
ParGrp = App.ParamGet("System parameter:Modules").GetGroup("Import")

# Set the needed information
ParGrp.SetString("HelpIndex",        "Import/Help/index.html")
ParGrp.SetString("DocTemplateName",  "Import")
ParGrp.SetString("DocTemplateScript","TemplImport.py")
ParGrp.SetString("WorkBenchName",    "Import Design")
ParGrp.SetString("WorkBenchModule",  "ImportWorkbench.py")

# Append the open handler
FreeCAD.EndingAdd("igs","Import")
FreeCAD.EndingAdd("iges","Import")
FreeCAD.EndingAdd("step","Import")
FreeCAD.EndingAdd("stp","Import")
FreeCAD.EndingAdd("brp","Import")
FreeCAD.EndingAdd("brep","Import")



