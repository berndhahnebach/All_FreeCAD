# FreeCAD init module  
# (c) 2001 Jürgen Riegel
#
# Gathering all the information to start FreeCAD
# This is one of two init scripts, the second one
# runs when the gui is up

# using FreeCAD output (not print)
import sys

import FreeCAD

# classes to manage loadable modules
from ConfigParser import *

def Get(Pars,Section,Option,Default=""):
    if(Pars.has_option(Section,Option)):
        return Pars.get(Section,Option)
    else:
        return Default


class FCModule:
    def __init__ (self,Dir):
        Pars = ConfigParser()
        FreeCAD.PrintLog('\t\tOpening:' + Dir + '\\Module.cfg\n')
        Pars.read(Dir + "\\Module.cfg")
        FreeCAD.PrintLog('\t\tModule exports:\n')
        for Exp in Pars.options("Exports"):
            FreeCAD.PrintLog('\t\t\t' + Exp + '\n')
        self.Name               = Get(Pars,"Module","Name")
        self.DocTemplateName    = Get(Pars,"Exports","DocTemplateName")
        self.DocTemplateScript  = Get(Pars,"Exports","DocTemplateScript")
        self.WorkBenchName 	    = Get(Pars,"Exports","WorkBenchName")
        self.WorkBenchModule    = Get(Pars,"Exports","WorkBenchModule")
           

#python standard functions
import dircache,os

print dircache.listdir(os.curdir)

FreeCAD.PrintLog ('\nFreeCAD init running....\n')

# Searching modules ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ModDir = '../src/Mod'
ModDirs = dircache.listdir(ModDir)
print ModDirs
FreeCAD.PrintMessage('\tSearching modules...\n')
ModuleList = []
for Dir in ModDirs:
    FreeCAD.PrintMessage('\t\tFound: ' + Dir + '\n')
    ModuleList.append(FCModule( ModDir + '\\' + Dir ))
    sys.path.append( ModDir + '\\' + Dir )
# Attaching the list to the FreeCAD main module
FreeCAD.Modules = ModuleList

def GetTemplateList():
    l = []
    for i in ModuleList:
        if (i.DocTemplateName != ''):
            l.append(i.DocTemplateName)
    return l



FreeCAD.PrintLog ('\nFreeCAD init done\n')
   
    


