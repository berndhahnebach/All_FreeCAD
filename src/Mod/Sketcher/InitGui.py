# Sketcher gui init module  
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



class SketcherWorkbench ( Workbench ):
	"Sketcher workbench object"
	Icon = """
                        /* XPM */
                        static char * sketcher_xpm[] = {
                        "16 16 9 1",
                        " 	c None",
                        ".	c #C24A2E",
                        "+	c #676966",
                        "@	c #A58A3B",
                        "#	c #9B8A59",
                        "$	c #939490",
                        "%	c #C7AC59",
                        "&	c #BAB9B2",
                        "*	c #E6E7E4",
                        "                ",
                        "  #%@%@%@%@%@&  ",
                        "  &#@###@###@&  ",
                        "  ************  ",
                        " &*+&&&&&+****  ",
                        " &*++$***$****# ",
                        " &&$*&&**$**#&..",
                        " &$+$$+$$$#%%#. ",
                        " *$***$*#%%@#&  ",
                        " &$***$%%@#&**& ",
                        " &$**$+##$&***& ",
                        " &*++$********& ",
                        " &************& ",
                        " &&&&&&&&&&&&&& ",
                        " $$$$$$$$$$$$$+ ",
                        "                "};
			"""
	MenuText = "Sketcher"
	ToolTip = "Sketcher workbench"

	def Initialize(self):
		# load the module
		import SketcherGui
		import Sketcher
	def GetClassName(self):
		return "SketcherGui::Workbench"

Gui.addWorkbench(SketcherWorkbench())
