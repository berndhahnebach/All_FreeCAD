

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
# copy top level files

DistTools.cpfile("../Doc/README.html",DistDir+DistBin+"/README.html")
DistTools.cpfile("../Doc/INSTALL.html",DistDir+DistBin+"/INSTALL.html")
DistTools.cpfile("../Doc/LICENSE.GPL.html",DistDir+DistBin+"/LICENSE.GPL.html")
DistTools.cpfile("../Doc/LICENSE.LGPL.html",DistDir+DistBin+"/LICENSE.LGPL.html")
#DistTools.cpfile("../Tools/BuildTool.py",DistDir+DistBin+"/BuildTool.py")

#====================================================================
# ziping a archiv
os.popen("rar.exe a "+DistDir+DistBin+".rar "+ DistDir+DistBin)

