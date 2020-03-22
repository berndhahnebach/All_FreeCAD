# ***************************************************************************
# *   (c) 2020 Carlo Pavan                                                  *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   FreeCAD is distributed in the hope that it will be useful,            *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with FreeCAD; if not, write to the Free Software        *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

"""This module provides the Draft Dimension Style tool.
"""
## @package gui_style_dimension
# \ingroup DRAFT
# \brief This module provides the Draft Dimension Style tool.

import FreeCAD as App
import FreeCADGui as Gui
from PySide import QtCore
from . import gui_base
from draftutils import utils
from draftobjects.dimensionstyle import make_dimension_style



class GuiCommandDimensionStyle(gui_base.GuiCommandSimplest):
    """
    The command creates a dimension style object
    """
    def __init__(self):
        super().__init__(name="Dimension style")

    def GetResources(self):
        _msg = ("Creates a new dimension style.\n"
                "The object stores dimension preferences into the document."
                )
        return {'Pixmap'  : 'Draft_Annotation_Style',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Draft", "Dimension Style"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Draft", _msg)}

    def IsActive(self):
        if Gui.ActiveDocument:
            return True
        else:
            return False

    def Activated(self):
        sel = Gui.Selection.getSelection()

        if len(sel) == 1:
            if utils.get_type(sel[0]) == 'Dimension':
                make_dimension_style(sel[0])
        
        make_dimension_style()


Gui.addCommand('Draft_DimensionStyle', GuiCommandDimensionStyle())
