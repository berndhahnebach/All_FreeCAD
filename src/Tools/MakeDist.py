

# shell and operating system
import os,sys
sys.path.append( "E:\\Develop\\Projekte\\FreeCADWin\\src\\Tools" )

import FCFileTools

# line seperator 
ls = os.linesep
# path seperator
ps = os.pathsep
# dir seperator
ds = os.sep

# Building dist name
if(len(sys.argv) < 2):
    # reading the last Version information
    [FCVersionMajor,FCVersionMinor,FCVersionBuild,FCVersionDisDa] = open("../Version.h",'r').readlines()
    DistName = "FreeCAD_V" + FCVersionMajor[23:-1] + '.' +FCVersionMinor[23:-1] + 'B' + FCVersionBuild[23:-1]
else:
    DistName = sys.argv[1]



#DistSrc  = DistName + "_src"
#DistSrc  = DistName + "_src_bin_win"
#DistBin  = DistName + "_src_bin_win"
DistSrc  = DistName 
DistBin  = DistName 
DistDir  = "../../DistTemp/"

#====================================================================
# script asume to run in src/Tools
if not os.path.isdir(DistDir):
    os.mkdir(DistDir)
#if not os.path.isdir(DistDir+DistSrc):
#    os.mkdir(DistDir+DistSrc)
if not os.path.isdir(DistDir+DistBin):
    os.mkdir(DistDir+DistBin)
else:
    raise "Dist path already there!!"


#====================================================================
# copy src 
sys.stdout.write( 'Copy src Tree ...\n')
if not os.path.isdir(DistDir+DistSrc+'/src'):
    os.mkdir(DistDir+DistSrc+'/src')
Filter = ["^.*\\.o$",
          "^Debug$",
          "^DebugCmd$",
          "^DebugPy$",
          "^Release$",
          "^ReleaseCmd$",
          "^ReleasePy$",
          "^Attic$",
          "^.*\\.opt$",
          "^.*\\.ilg$",
          "^.*\\.ps$",
          "^.*\\.ind$",
          "^.*\\.idx$",
          "^.*\\.doc$",
          "^.*\\.dvi$",
          "^.*\\.ncb$",
          "^.*\\.aux$",
          "^.*\\.pdf$",
          "^.*\\.toc$",
          "^.*\\.exe$",
          "^.*\\.png$",
          "^.*\\.bak$",
          "^.*\\.pyc$",
          "^.*\\.dep$",
          "^.*\\.log$",
          "^.*\\.pyd$",
          "^.*\\.ilk$",
          "^.*\\.lib$",
          "^.*\\.pdb$",
          "^.*\\.exp$",
          "^.*\\.bsc$",
          "^.*\\.plg$",]
FCFileTools.cpallWithFilter('../../src',DistDir+DistSrc+'/src',FCFileTools.SetUpFilter(Filter))

#====================================================================
# copy Doc 
#sys.stdout.write( 'Copy Doc Tree ...\n')
#if not os.path.isdir(DistDir+DistSrc+'/doc'):
#    os.mkdir(DistDir+DistSrc+'/doc')
#Filter = ["^.*\\.tex$"]
#FCFileTools.cpallWithFilter('../../Doc',DistDir+DistSrc+'/doc',FCFileTools.SetUpFilter(Filter))

#====================================================================
# copy bin and lib 
#sys.stdout.write( 'Copy bin and lib Tree ...\n')
#if not os.path.isdir(DistDir+DistBin+'/bin'):
#    os.mkdir(DistDir+DistBin+'/bin')
#Filter = ["^Plugin\\.*$",
#          "^Standard\\.*$",
#          "^.*\\.xml$",
#          "^.*\\.log$",
#          "^.*\\.pdb$",
#          "^.*\\.ilk$",
#          "^.*\\.exp$",
#          "^.*\\.bsc$",
#          "^.*\\.FCScript\\..*$",
#          "^.*\\.FCParam$",
#          "^.*\\.FCScript$"]
#FCFileTools.cpallWithFilter('../../bin',DistDir+DistBin+'/bin',FCFileTools.SetUpFilter(Filter))

#====================================================================
# copy top level files

FCFileTools.cpfile("../Doc/README.html",DistDir+DistBin+"/README.html")
FCFileTools.cpfile("../Doc/INSTALL.html",DistDir+DistBin+"/INSTALL.html")
FCFileTools.cpfile("../Doc/LICENSE.GPL.html",DistDir+DistBin+"/LICENSE.GPL.html")
FCFileTools.cpfile("../Doc/LICENSE.LGPL.html",DistDir+DistBin+"/LICENSE.LGPL.html")
FCFileTools.cpfile("../Tools/BuildTool.py",DistDir+DistBin+"/BuildTool.py")


