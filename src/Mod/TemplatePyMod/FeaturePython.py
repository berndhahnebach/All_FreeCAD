"""
Examples for a feature class and its view provider.

***************************************************************************
*   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
*                                                                         *
*   This file is part of the FreeCAD CAx development system.              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License (GPL)            *
*   as published by the Free Software Foundation; either version 2 of     *
*   the License, or (at your option) any later version.                   *
*   for detail see the LICENCE text file.                                 *
*                                                                         *
*   FreeCAD is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU Library General Public License for more details.                  *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with FreeCAD; if not, write to the Free Software        *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
*   USA                                                                   *
*                                                                         *
***************************************************************************
"""

__author__ = "Werner Mayer <werner.wm.mayer@gmx.de>"

import FreeCAD, FreeCADGui

class FeaturePython:
	def execute(self, fp):
		FreeCAD.Console.PrintMessage("Hello, World!\n")

class ViewProviderFeaturePython:
	def updateData(self, vp, prop):
		FreeCAD.Console.PrintMessage(str(prop))


def test():
	FreeCAD.newDocument()
	a=FreeCAD.ActiveDocument.addObject("App::FeaturePython","Feature")
	a.Proxy=FeaturePython()
	a.ViewObject.Proxy=ViewProviderFeaturePython()
