# FreeCAD init script of the Raytracing module
# (c) 2001 Juergen Riegel

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                        *
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


class RaytracingDocument:
	"Raytracing document"
	def Info(self):
		return "Raytracing document"

class RaytracingTemplate:
	"Raytracing template object"
	def Start(self):
		# switch to the right workbench
		if(App.GuiUp == 1):
			Log ('Switching to Raytracing workbench')
			Gui.WorkbenchActivate("Raytracing design")
		Log ('   Set up Raytracing document...\n')
		#get the active document
		Doc = App.DocGet()
		Main = Doc.Main

		Log ('   Init Raytracing doc handle...\n')
		#Doc.Raytracing = 1

		L1 = Main.GetLabel(1)
		L1.GetLabel(1)
		L1.Int = 1
		L1.Real = 1.0
		L1.Name = "Hallo"
		L2 = L1.GetLabel(1)
		L3 = L1.GetLabel(2)



# Get the Parameter Group of this module
ParGrp = App.ParamGet("System parameter:Modules").GetGroup("Raytracing")

# Set the needed information
ParGrp.SetString("HelpIndex",        "Raytracing/Help/index.html")
ParGrp.SetString("DocTemplateName",  "Raytracing")
ParGrp.SetString("DocTemplateScript","TemplRaytracing.py")
ParGrp.SetString("WorkBenchName",    "Raytracing Design")
ParGrp.SetString("WorkBenchModule",  "RaytracingWorkbench.py")

App.TemplateAdd("Raytracing",RaytracingTemplate())


