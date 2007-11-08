# Part gui init module  
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



class PartWorkbench ( Workbench ):
	"Part workbench object"
	MenuText = "Part design"
	ToolTip = "Part design workbench"
	def Activate(self):
		# load the module
		try:
			Log ('Loading GUI of Part module...')
			import PartGui
			import Part
		except:
			Err('Cannot load PartGui')
			raise
	def GetClassName(self):
		return "PartGui::Workbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *Part_Box[]={\n"
			"\"16 16 3 1\",\n"
			"\". c None\",\n"
			"\"# c #000000\",\n"
			"\"a c #c6c642\",\n"
			"\"................\",\n"
			"\".......#######..\",\n"
			"\"......#aaaaa##..\",\n"
			"\".....#aaaaa###..\",\n"
			"\"....#aaaaa##a#..\",\n"
			"\"...#aaaaa##aa#..\",\n"
			"\"..#aaaaa##aaa#..\",\n"
			"\".########aaaa#..\",\n"
			"\".#aaaaa#aaaaa#..\",\n"
			"\".#aaaaa#aaaa##..\",\n"
			"\".#aaaaa#aaa##...\",\n"
			"\".#aaaaa#aa##....\",\n"
			"\".#aaaaa#a##... .\",\n"
			"\".#aaaaa###......\",\n"
			"\".########.......\",\n"
			"\"................\"};\n"]

Gui.addWorkbench("Part design",PartWorkbench())
