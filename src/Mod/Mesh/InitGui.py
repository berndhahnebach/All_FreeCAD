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
			"static const char *curv_info[]={\n"
			"\"22 22 11 1\",\n"
			"\"a c #000000\",\n"
			"\"g c #0000ff\",\n"
			"\"f c #008080\",\n"
			"\"c c #008000\",\n"
			"\"d c #00ffff\",\n"
			"\"# c #808080\",\n"
			"\". c None\",\n"
			"\"e c #ff0000\",\n"
			"\"h c #ffaa00\",\n"
			"\"b c #000000\",\n"
			"\"k c #ffff00\",\n"
			"\"......................\",\n"
			"\"......................\",\n"
			"\".......#aaaa#.........\",\n"
			"\".bb...aa....aa........\",\n"
			"\".bcb.a#ddd...#a...eee.\",\n"
			"\".bcc#add......a#..eee.\",\n"
			"\"..bcacd........a..hhh.\",\n"
			"\"..bcacdb.......a..hhh.\",\n"
			"\"..bcacccb......a..kkk.\",\n"
			"\"..bcaccccb...d.a..kkk.\",\n"
			"\"..bc#accccb.d.a#..ccc.\",\n"
			"\"...bca#ccccdd#a...ccc.\",\n"
			"\"...bccaaccccaaaa..fff.\",\n"
			"\"...bccc#aaaa#.aaa.fff.\",\n"
			"\"...bccccccccbb.aa.ddd.\",\n"
			"\"....bccccbbb....a.ddd.\",\n"
			"\"....bcbbb.........ggg.\",\n"
			"\".....bb...........ggg.\",\n"
			"\"......................\",\n"
			"\"......................\",\n"
			"\"......................\",\n"
			"\"......................\"};\n"]

Gui.AddWorkbenchHandler("Mesh design",MeshWorkbench())
