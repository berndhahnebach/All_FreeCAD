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
	Err("See www.python.org for details")
	raise

# classes to manage loadable modules
from ConfigParser import *

def Get(Pars,Section,Option,Default=""):
    if(Pars.has_option(Section,Option)):
        return Pars.get(Section,Option)
    else:
        return Default

def CheckInitialPlugin():
	Plugin = """
! Description of available plugins
! ********************************
!
a148e300-5740-11d1-a904-080036aaa103.Location: FWOSPlugin
!
! base document drivers plugin
!
ad696000-5b34-11d1-b5ba-00a0c9064368.Location: PAppStdPlugin
ad696001-5b34-11d1-b5ba-00a0c9064368.Location: PAppStdPlugin
ad696002-5b34-11d1-b5ba-00a0c9064368.Location: PAppStdPlugin
47b0b826-d931-11d1-b5da-00a0c9064368.Location: PAppStdPlugin
47b0b827-d931-11d1-b5da-00a0c9064368.Location: PAppStdPlugin
"""

	Standard = """
formatlist:FreeCad-Std
#
# standard attributes
#
MDTV-Standard.Description: Standard Document Version 1.0
MDTV-Standard.FileExtension: std
MDTV-Standard.StoragePlugin: ad696000-5b34-11d1-b5ba-00a0c9064368
MDTV-Standard.RetrievalPlugin: ad696001-5b34-11d1-b5ba-00a0c9064368
MDTV-StandardSchema: ad696002-5b34-11d1-b5ba-00a0c9064368
#
# Standard Shape from designer
#
MDTV-ShapeSchema: e4a0c608-9e3f-11d1-b5ca-00a0c9064368
MDTV-Shape.StoragePlugin: 5ff7dc00-8840-11d1-b5c2-00a0c9064368
MDTV-Shape.RetrievalPlugin: 5ff7dc01-8840-11d1-b5c2-00a0c9064368
MDTV-Shape.FileExtension: brp
MDTV-Shape.DataType: Shape Document Representation
MDTV-Shape.VersionDataType: Shape_Model
MDTV-Shape.Description: Shape Document Version 1.0
MDTV-Shape.Domain: EUCLID/Quantum
MDTV-Shape Document Version 1.0.Icon: MDTV-Shape


#
# OCAF MFC Sampel
#
Ocaf-Sample.Description: Stars Document Version 1.0
Ocaf-Sample.FileExtension: sta
Ocaf-Sample.StoragePlugin: ad696000-5b34-11d1-b5ba-00a0c9064368
Ocaf-Sample.RetrievalPlugin: ad696001-5b34-11d1-b5ba-00a0c9064368
Ocaf-SampleSchema: ad696002-5b34-11d1-b5ba-00a0c9064368
Ocaf-Sample.AttributeStoragePlugin: 47b0b826-d931-11d1-b5da-00a0c9064368
Ocaf-Sample.AttributeRetrievalPlugin: 47b0b827-d931-11d1-b5da-00a0c9064368



#
# FreeCAD Std document (uses std attributes so far)
#
FreeCad-Std.Description: FreeCAD Std 1.0
FreeCad-Std.FileExtension: FCStd
FreeCad-Std.StoragePlugin: ad696000-5b34-11d1-b5ba-00a0c9064368
FreeCad-Std.RetrievalPlugin: ad696001-5b34-11d1-b5ba-00a0c9064368
FreeCad-StdSchema: ad696002-5b34-11d1-b5ba-00a0c9064368
FreeCad-Std.AttributeStoragePlugin: 47b0b826-d931-11d1-b5da-00a0c9064368
FreeCad-Std.AttributeRetrievalPlugin: 47b0b827-d931-11d1-b5da-00a0c9064368
"""
	Log("    Checking on Plugin files...\n")

	try:
		f = open("Plugin","r")
	except:
		Log("   Plugin non existing, write it\n")
		f = open("Plugin","w")
		f.write(Plugin)
		f.close
	
	try:
		f = open("Standard","r")
	except:
		Log("   Standard non existing, write it\n")
		f = open("Standard","w")
		f.write(Standard)
		f.close



def CheckEnvs():
	try:
		# Testing env variables ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		Log("    Checking on Environment...\n")
		envs = ['CSF_MDTVFONTDIRECTORY','CSF_MDTVTEXTURESDIRECTORY','CSF_PLUGINDEFAULTS','CSF_SHMESSAGESTD','CSF_STANDARDDEFAULTS','CSF_UNITSDEFINITION','CSF_UNITSLEXICON','CSF_XSMESSAGESTD']
		for i in envs :
			try:
				os.getenv(i)
			except :
				Err('         The OpenCasCade environment Variables ' + i + ' is not set, please reinstall Open CasCade 4.0 from www.opencascade.org!\n')
				raise
	except :
		Err("\n\n        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n")
		Err("        At least one test fails, bailing out!\n")
		Err("        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n")
		raise

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

# checking if the files Plugin and Standard are there. otherwise write it
CheckInitialPlugin()
# checks if the OCC environment is set
CheckEnvs()

# Searching modules ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ModDir = '../src/Mod'
ModDirs = dircache.listdir(ModDir)
#print ModDirs
Msg('   Searching modules...\n')
ModuleList = []
for Dir in ModDirs:
	if ( Dir != 'CVS'):
		FreeCAD.PrintMessage('    Found: ' + Dir + '\n')
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
   
    


