

# shell and operating system
import os,sys
#sys.path.append( "E:\\Develop\\Projekte\\FreeCADWin\\src\\Tools" )

import DistTools

# line seperator 
ls = os.linesep
# path seperator
ps = os.pathsep
# dir seperator
ds = os.sep

DistName = DistTools.BuildDistName()

#DistSrc  = DistName + "_src"
#DistSrc  = DistName + "_src_bin_win"
#DistBin  = DistName + "_src_bin_win"
DistSrc  = DistName 
DistBin  = DistName 
DistDir  = "../../DistTemp/"

#====================================================================
# script asume to run in src/Tools

DistTools.EnsureDir(DistDir)

if (DistTools.EnsureDir(DistDir+DistBin) == 1):
    raise "Dist path already there!!"

#====================================================================
# copy src 
sys.stdout.write( 'Copy src Tree ...\n')
DistTools.EnsureDir(DistDir+DistSrc+'/src')
DistTools.cpallWithFilter('../../src',DistDir+DistSrc+'/src',DistTools.SetUpFilter(DistTools.SrcFilter))

#====================================================================
# copy bin and lib 
sys.stdout.write( 'Copy bin and lib Tree ...\n')
DistTools.EnsureDir(DistDir+DistBin+'/bin')
DistTools.cpallWithFilter('../../bin',DistDir+DistBin+'/bin',DistTools.SetUpFilter(DistTools.BinFilter))
DistTools.EnsureDir(DistDir+DistBin+'/lib')
DistTools.cpallWithFilter('../../lib',DistDir+DistBin+'/lib',DistTools.SetUpFilter(DistTools.LibFilter))

#====================================================================
# copy Modules
sys.stdout.write( 'Copy modul Tree ...\n')
DistTools.EnsureDir(DistDir+DistBin+'/Mod')
DistTools.cpallWithFilter('../../src/Mod',DistDir+DistBin+'/Mod',DistTools.SetUpFilter(DistTools.ModFilter))

#====================================================================
# copy 3rd party libs

LibPos= os.getenv("FREECADLIB")
print LibPos

#====================================================================
# copy top level files

DistTools.cpfile("../Doc/README.html",DistDir+DistBin+"/README.html")
DistTools.cpfile("../Doc/INSTALL.html",DistDir+DistBin+"/INSTALL.html")
DistTools.cpfile("../Doc/LICENSE.GPL.html",DistDir+DistBin+"/LICENSE.GPL.html")
DistTools.cpfile("../Doc/LICENSE.LGPL.html",DistDir+DistBin+"/LICENSE.LGPL.html")
#DistTools.cpfile("../Tools/BuildTool.py",DistDir+DistBin+"/BuildTool.py")

#====================================================================
# generating the sfx file

BuildNbr = DistTools.GetBuildNbr()
Version  = DistTools.GetVersion()
BuildDate = DistTools.GetBuildDate()

Sfx = open("sfx.tmp",'w')

Sfx.write("Title=FreeCAD "+Version+" Build "+BuildNbr+" \n");
Sfx.write("Text \n");
Sfx.write("{ \n");
Sfx.write("FreeCAD "+Version+" \n");
Text = open("InstallMsg.txt",'r')
Sfx.write(Text.read())
Text.close()
#Sfx.write(" \n");
#Sfx.write(" [Version:"+Version+"|Build:"+BuildNbr+"|Date:"+BuildDate+"] \n");
Sfx.write("} \n");
Sfx.write("Path=FreeCAD \n");
Sfx.write('Shortcut=S, '+DistBin+'\FreeCAD.bat, FreeCAD, "FreeCAD '+Version+'", "FreeCAD 0.1" \n');
Sfx.write("Overwrite=1 \n");

Sfx.close()


#====================================================================
# ziping a archiv
Cmd = "rar.exe a -sfx -zsfx.tmp -df -ep1 "+DistDir+DistBin+".exe "+ DistDir+DistBin
print Cmd
os.popen(Cmd)

