# FreeCAD install module  
# (c) 2001 Jürgen Riegel
#
# Searching and installing new packages and set up the environment at first
# start.

#***************************************************************************
#*   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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

def WriteInitialPlugin():
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


Log("FreeCAD Install running...\n\n")
# Checking on Plugin and Standard file� +++++++++++++++++++++++++++++++++++++
WriteInitialPlugin()
# Checking on FreeCAD Module path ++++++++++++++++++++++++++++++++++++++++++
ModDir = '../src/Mod'
Msg("   Using "+ModDir+" as module path!\n")
# Searching modules dirs +++++++++++++++++++++++++++++++++++++++++++++++++++
sys.path.append( ModDir )
ModDirs = dircache.listdir(ModDir)
#print ModDirs
Msg('   Searching modules...\n')
ModPar = App.ParamGet("Modules")
for Dir in ModDirs:
	if ( Dir != 'CVS'):
		FreeCAD.PrintMessage('      Found: ' + Dir + '... ')
		ModGrp = ModPar.GetGroup(Dir)
		if ( ModGrp.GetBool("Installed",0) == 1):
			Msg('already installed\n')
		else:
			FreeCAD.PrintMessage('new! install it...\n')
			InstallFile = os.path.join(os.path.join(ModDir,Dir),"Install.py")
			if ( os.path.exists(InstallFile) ):
				execfile(InstallFile)
			else:
				Wrn("         Install.py not found! "+Dir+" not installed!\n")
				
			

App.ParamGet().SetBool("Installed",1)

Log("FreeCAD Install done\n")

