# Mesh gui init module
# (c) 2004 Werner Mayer
#
# Gathering all the information to start FreeCAD
# This is the second one of three init scripts, the third one
# runs when the gui is up

#***************************************************************************
#*   (c) Werner Mayer <werner.wm.mayer@gmx.de> 2004                        *
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        * 
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#*   Werner Mayer 2004                                                     *
#***************************************************************************/



class MeshWorkbench ( Workbench ):
	"Mesh workbench object"
	def Start(self):
		# load the module
		try:
			Log ('Loading MeshGui module')
			import MeshGui
		except:
			Err('   cannot load MeshGui')
			raise
		else:	
			Log ('   Set up Mesh toolbar...\n')
			list = ["Mesh_Test"]
			Gui.ToolbarAppendItems("MeshTools", list, 0)

			Log ('   Setup Mesh commandbar...\n')
			list = ["Mesh_Test"]
			Gui.CommandbarAppendItems("MeshTools", list, 0)

			Log ('   Setup Mesh menues...\n')

	def Stop(self):
		Log ('   Stopping Mesh workbench...\n')
		Gui.CommandbarDelete("MeshTools")
		Gui.ToolbarDelete("MeshTools")


#Gui.WorkbenchAdd("Mesh design",MeshWorkbench())
Gui.WorkbenchAdd("Mesh design",MeshWorkbench())
