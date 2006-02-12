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
	def Activate(self):
		# load the module
		try:
			import MeshGui
		except:
			Err('Cannot load MeshGui')
			raise
	def GetClassName(self):
		return "MeshGui::Workbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *mesh_icon[]={\n"
			"\"16 16 4 1\",\n"
			"\".	c None\",\n"
			"\"#	c #000000\",\n"
			"\"s	c #BEC2FC\",\n"
			"\"g	c #00FF00\",\n"
			"\".......##.......\",\n"
			"\"....#######.....\",\n"
			"\"..##ggg#ggg#....\",\n"
			"\"##ggggg#gggg##..\",\n"
			"\"#g#ggg#gggggg##.\",\n"
			"\"#gg#gg#gggg###s.\",\n"
			"\"#gg#gg#gg##gg#s.\",\n"
			"\"#ggg#####ggg#ss.\",\n"
			"\"#gggg##gggg#ss..\",\n"
			"\".#g##g#gggg#s...\",\n"
			"\".##ggg#ggg#ss...\",\n"
			"\".##gggg#g#ss....\",\n"
			"\"..s#####g#s.....\",\n"
			"\"....sss##ss.....\",\n"
			"\"........ss......\",\n"
			"\"................\"};\n"]

Gui.AddWorkbenchHandler("Mesh design",MeshWorkbench())
