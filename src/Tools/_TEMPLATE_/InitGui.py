# _TEMPLATE_ gui init module
# (c) 2003 Juergen Riegel
#
# Gathering all the information to start FreeCAD
# This is the second one of three init scripts, the third one
# runs when the gui is up

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                        *
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



class _TEMPLATE_Workbench ( Workbench ):
	"_TEMPLATE_ workbench object"
	def Start(self):
		# load the module
		try:
			Log ('Loading _TEMPLATE_Gui module')
			import _TEMPLATE_Gui
		except:
			Err('   can not load _TEMPLATE_Gui')
			raise
		else:	
			Log ('   Set up _TEMPLATE_ toolbar...\n')
			list = ["_TEMPLATE__Test"]
			Gui.ToolbarAppendItems("_TEMPLATE_Tools", list, 0)

			Log ('   Set up _TEMPLATE_ commandbar...\n')
			list = ["_TEMPLATE__Test"]
			Gui.CommandbarAppendItems("_TEMPLATE_Tools", list, 0)

			Log ('   Set up _TEMPLATE_ menues...\n')

	def Stop(self):
		Log ('   Stoping _TEMPLATE_ workbench...\n')
		Gui.CommandbarDelete("_TEMPLATE_Tools")
		Gui.ToolbarDelete("_TEMPLATE_Tools")


#Gui.WorkbenchAdd("_TEMPLATE_ design",_TEMPLATE_Workbench())
Gui.WorkbenchAdd("_TEMPLATE_ design",_TEMPLATE_Workbench())
