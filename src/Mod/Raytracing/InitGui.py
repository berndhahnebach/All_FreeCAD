# Raytracing gui init module
# (c) 2003 Juergen Riegel
#
# Gathering all the information to start FreeCAD
# This is the second one of three init scripts, the third one
# runs when the gui is up

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                       *
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
#*   Juergen Riegel 2002                                                   *
#***************************************************************************/



class RaytracingWorkbench ( Workbench ):
	"Raytracing workbench object"
	def Activate(self):
		# load the module
		try:
			Log ('Loading RaytracingGui module')
			import PartGui
			import RaytracingGui
		except:
			Err('Cannot load RaytracingGui')
			raise
	def GetClassName(self):
		return "RaytracingGui::Workbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static char *arrows[]={\n"
			"\"16 16 3 1\",\n"
			"\"# c None\",\n"
			"\". c None\",\n"
			"\"a c #0000FF\",\n"
			"\"...........##...\",\n"
			"\"...........#a#..\",\n"
			"\"...##.......#a#a\",\n"
			"\"..aaa#.......#aa\",\n"
			"\"..#aaa#......#aa\",\n"
			"\"...#aaa#........\",\n"
			"\"....#aaa#.......\",\n"
			"\".....#aaa#...#..\",\n"
			"\"......#aaa#.##..\",\n"
			"\".......#aaa#a#..\",\n"
			"\"........#aaaa#..\",\n"
			"\"##.......#aaa#..\",\n"
			"\"#a#.....#aaaa#..\",\n"
			"\".#a##...#aaaa#..\",\n"
			"\"..#aa...........\",\n"
			"\"..aaa...........\"};\n"]

Gui.AddWorkbenchHandler("Raytracing",RaytracingWorkbench())
