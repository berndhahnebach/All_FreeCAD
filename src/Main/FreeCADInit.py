# FreeCAD init module  
# (c) 2001 Jürgen Riegel
#
# Gathering all the information to start FreeCAD
# This is one of two init scripts, the second one
# runs when the gui is up



# imports the one and only
import FreeCAD

# some often used shortcuts (for lazy people like me ;-)
App = FreeCAD
Log = FreeCAD.PrintLog
Msg = FreeCAD.PrintMessage
Err = FreeCAD.PrintError

try:
	import sys,os
except:
	Err("Seams the python standard libs are not installed, bailing out!")
	Err("Please (re)install python 2.1.x!")
	raise

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
        ModConfFName=os.path.join(Dir,'Module.cfg')
        Log('\t\tOpening:' + ModConfFName+'\n')
        Pars.read(ModConfFName)
        Log('\t\tModule exports:\n')
        for Exp in Pars.options("Exports"):
            Log('\t\t\t' + Exp + '\n')
        self.Name               = Get(Pars,"Module","Name")
        self.DocTemplateName    = Get(Pars,"Exports","DocTemplateName")
        self.DocTemplateScript  = Get(Pars,"Exports","DocTemplateScript")
        self.WorkBenchName 	    = Get(Pars,"Exports","WorkBenchName")
        self.WorkBenchModule    = Get(Pars,"Exports","WorkBenchModule")
           

#python standard functions
import dircache,os

Log ('\nFreeCAD init running....\n')

# Searching modules ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ModDir = '../src/Mod'
ModDirs = dircache.listdir(ModDir)
#print ModDirs
Msg('\tSearching modules...\n')
ModuleList = []
for Dir in ModDirs:
	if ( Dir != 'CVS'):
		FreeCAD.PrintMessage('\t\tFound: ' + Dir + '\n')
		ModDirDir=os.path.join(ModDir,Dir);
		ModuleList.append(FCModule( ModDirDir ))
		sys.path.append( ModDirDir )
# Attaching the list to the FreeCAD main module
FreeCAD.Modules = ModuleList

def GetTemplateList():
    l = []
    for i in ModuleList:
        if (i.DocTemplateName != ''):
            l.append(i.DocTemplateName)
    return l


Log ('\nFreeCAD init done\n')
   
    


