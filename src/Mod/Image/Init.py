# FreeCAD init script of the Image module
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


class ImageDocument:
	"Image document"
	def Info(self):
		return "Image document"

class ImageTemplate:
	"Image template object"
	def Start(self):
		# switch to the right workbench
		if(App.GuiUp == 1):
			Log ('Switching to Image workbench')
			Gui.WorkbenchActivate("Image")
		Log ('   Set up Image document...\n')
		#get the active document
		Doc = App.DocGet()
		Main = Doc.Main

		Log ('   Init Image doc handle...\n')
		#Doc.Image = 1

		L1 = Main.GetLabel(1)
		L1.GetLabel(1)
		L1.Int = 1
		L1.Real = 1.0
		L1.Name = "Hallo"
		L2 = L1.GetLabel(1)
		L3 = L1.GetLabel(2)



# Get the Parameter Group of this module
ParGrp = App.ParamGet("System parameter:Modules").GetGroup("Image")

# Set the needed information
ParGrp.SetString("HelpIndex",        "Image/Help/index.html")
ParGrp.SetString("DocTemplateName",  "Image")
ParGrp.SetString("DocTemplateScript","TemplImage.py")
ParGrp.SetString("WorkBenchName",    "Image")
ParGrp.SetString("WorkBenchModule",  "ImageWorkbench.py")

App.TemplateAdd("Image",ImageTemplate())


