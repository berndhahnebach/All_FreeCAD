# FreeCAD init script of the part module  
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


class PartDocument:
	"Part document"
	def Info(self):
		return "Part document"
		
class PartTemplate:
	"Part template object"
	def Start(self):
		# switch to the right workbench
		if(App.GuiUp == 1):
			Log ('Switching to Part workbench')
			Gui.WorkbenchActivate("Part design")
		Log ('   Set up part document...\n')
		#get the active document
		Doc = App.DocGet()
		Main = Doc.Main

		Log ('   Init part doc handle...\n')		
		#Doc.Part = 1
		
		L1 = Main.GetLabel(1)
		L1.GetLabel(1)
		L1.Int = 1
		L1.Real = 1.0
		L1.Name = "Hallo"
		L2 = L1.GetLabel(1)
		L3 = L1.GetLabel(2)
            


# Get the Parameter Group of this module
ParGrp = App.ParamGet("System parameter:Modules").GetGroup("Part")

# Set the needed information
ParGrp.SetString("HelpIndex",        "Part/Help/index.html")
ParGrp.SetString("DocTemplateName",  "Part")
ParGrp.SetString("DocTemplateScript","TemplPart.py")
ParGrp.SetString("WorkBenchName",    "Part Design")
ParGrp.SetString("WorkBenchModule",  "PartWorkbench.py")

App.TemplateAdd("Part",PartTemplate())


