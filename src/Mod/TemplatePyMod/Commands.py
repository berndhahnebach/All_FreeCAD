# FreeCAD TemplatePyMod module  
# (c) 2007 Juergen Riegel LGPL

#


# import FreeCAD modules
import FreeCAD, FreeCADGui, TestApp

#---------------------------------------------------------------------------
# The command classes
#---------------------------------------------------------------------------

class TemplatePyMod_Cmd1:
    "Example command class"
    def Activated(self):
        print "TemplatePyMod_Cmd1 activated ;-) "

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1', 'MenuText': 'Cmd1', 'ToolTip': 'Very unimportand example command'}



#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------
FreeCADGui.addCommand('TemplatePyMod_Cmd1'        ,TemplatePyMod_Cmd1())

