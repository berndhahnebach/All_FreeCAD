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
	Icon = """
			/* XPM */
			static const char *mesh_icon[]={
			"16 16 4 1",
			".	c None",
			"#	c #000000",
			"s	c #BEC2FC",
			"g	c #00FF00",
			".......##.......",
			"....#######.....",
			"..##ggg#ggg#....",
			"##ggggg#gggg##..",
			"#g#ggg#gggggg##.",
			"#gg#gg#gggg###s.",
			"#gg#gg#gg##gg#s.",
			"#ggg#####ggg#ss.",
			"#gggg##gggg#ss..",
			".#g##g#gggg#s...",
			".##ggg#ggg#ss...",
			".##gggg#g#ss....",
			"..s#####g#s.....",
			"....sss##ss.....",
			"........ss......",
			"................"};
			"""
	MenuText = "Mesh design"
	ToolTip = "Mesh design workbench"

	def Initialize(self):
		# load the module
		Log ('Loading GUI of Mesh module...')
		import MeshGui
	def GetClassName(self):
		return "MeshGui::Workbench"

Gui.addWorkbench(MeshWorkbench)
