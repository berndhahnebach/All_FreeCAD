

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

DistName = DistTools.BuildSetupName()

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

try:
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
    DistTools.cpfile("FreeCAD.bat",DistDir+DistBin+'/bin/FreeCAD.bat')
    #====================================================================
    # copy Modules
    sys.stdout.write( 'Copy modul Tree ...\n')
    DistTools.EnsureDir(DistDir+DistBin+'/Mod')
    DistTools.cpallWithFilter('../../src/Mod',DistDir+DistBin+'/Mod',DistTools.SetUpFilter(DistTools.ModFilter))


    #====================================================================
    # copy Documentation
    sys.stdout.write( 'Copy documentation Tree ...\n')
    DistTools.EnsureDir(DistDir+DistBin+'/Doc')
    DistTools.EnsureDir(DistDir+DistBin+'/Doc/SourceDocumentation')
    if not os.path.isdir("../../Doc/SourceDocumentation"):
        print "No Framework doc generated, do MakeDoc.py first\n"
        raise 
    DistTools.cpallWithFilter('../../Doc/SourceDocumentation',DistDir+DistBin+'/Doc/SourceDocumentation',DistTools.SetUpFilter(DistTools.DocFilter))

    DistTools.EnsureDir(DistDir+DistBin+'/Doc/free-cad.sourceforge.net')
    if not os.path.isdir("../../Doc/free-cad.sourceforge.net"):
        print "No Online doc downloaded, do DownloadOnlineDoc.py first\n"
        raise 
    DistTools.cpallWithFilter('../../Doc/free-cad.sourceforge.net',DistDir+DistBin+'/Doc/free-cad.sourceforge.net',DistTools.SetUpFilter(DistTools.DocFilter))

    #====================================================================
    # copy 3rd party libs
    sys.stdout.write( 'Copy 3rd party libs ...\n')

    LibPos= os.getenv("FREECADLIB")
    print LibPos
    if (LibPos==""):
        print "FREECADLIB env variable not specified, bailing out."
        raise 

    DistTools.EnsureDir(DistDir+DistBin+'/LibPack')
    DistTools.cpallWithFilter(LibPos,DistDir+DistBin+'/LibPack',DistTools.SetUpFilter(DistTools.LibPackFilter))

    #====================================================================
    # copy top level files

    #DistTools.cpfile("../Doc/README.html",DistDir+DistBin+"/README.html")
    #DistTools.cpfile("../Doc/INSTALL.html",DistDir+DistBin+"/INSTALL.html")
    #DistTools.cpfile("../Doc/LICENSE.GPL.html",DistDir+DistBin+"/LICENSE.GPL.html")
    #DistTools.cpfile("../Doc/LICENSE.LGPL.html",DistDir+DistBin+"/LICENSE.LGPL.html")
    #DistTools.cpfile("../Tools/BuildTool.py",DistDir+DistBin+"/BuildTool.py")

    #====================================================================
    # generating the sfx file

    BuildNbr = DistTools.GetBuildNbr()
    Version  = DistTools.GetVersion()
    BuildDate = DistTools.GetBuildDate()

    Sfx = open("sfx.txt",'w')

    Sfx.write("Title=FreeCAD V"+Version+" Build "+BuildNbr+"\n");
    Sfx.write("Text\n");
    Sfx.write("{\n");
    Sfx.write('<font size=4 face="Verdana"><b>FreeCAD '+Version+'</font></b><p>\n');
    Text = open("InstallMsg.txt",'r')
    Sfx.write(Text.read())
    Text.close()
    #Sfx.write(" \n");
    #Sfx.write(" [Version:"+Version+"|Build:"+BuildNbr+"|Date:"+BuildDate+"] \n");
    Sfx.write("\n} \n");
    Sfx.write("Path=FreeCAD \n\n");
    Sfx.write('Shortcut=P, '+DistBin+'\\bin\\FreeCAD.bat, FreeCAD'+Version+', "FreeCAD '+Version+'", "FreeCAD'+Version+'" \n\n');
    Sfx.write('Shortcut=P, '+DistBin+'\\Doc\\free-cad.sourceforge.net\\index.html, FreeCAD'+Version+', "User documentation", "UserDoc" \n\n');
    Sfx.write('Shortcut=P, '+DistBin+'\\Doc\\SourceDocumentation\\index.html, FreeCAD'+Version+', "Source documentation", "SourceDoc" \n\n');
    Sfx.write("Overwrite=1 \n\n");

    Sfx.close()


    #====================================================================
    # ziping a archiv
    Cmd = "rar.exe a -sfx -zsfx.txt -df -ep1 "+DistDir+DistBin+"B"+BuildNbr+".exe "+ DistDir+DistBin
    print Cmd
    os.popen(Cmd)
except:
    print "Somthing went wrong:", sys.exc_info()[0]
    print "Rolling back"
    DistTools.rmall(DistDir+DistBin)
    raise
else:
    print "Finished succesfully\n"
    
