# shell and operating system
import os,sys
sys.path.append( "..\Tools" )

import FCFileTools

# line seperator 
ls = os.linesep
# path seperator
ps = os.pathsep
# dir seperator
ds = os.sep

#====================================================================
# script asume to run in src/Doc
#os.chdir("e:/Develop/FreeCADWin/src/Doc")
#LogFile = open("MakeDoc.log",'w')
if not os.path.isdir("../../Doc"):
    os.mkdir("../../Doc")
#if not os.path.isdir("../../Doc/res"):
#    os.mkdir("../../Doc/res")
#FCFileTools.cpfile("index.html","../../doc/index.html")
#FCFileTools.cpfile("FreeCAD.css","../../doc/res/FreeCAD.css")

#====================================================================
sys.stdout.write ('Running source documentation ...')
# running doxygen with the parameters from the config file
text = os.popen("doxygen doxygen.cfg").read()
#LogFile.write(text)
if not os.path.isdir("../../Doc/SourceDocumentation"):
    os.mkdir("../../Doc/SourceDocumentation")

#====================================================================
sys.stdout.write( ' done\n  Generate HTML ...')
FCFileTools.cpall("html","../../Doc/SourceDocumentation")

#====================================================================
sys.stdout.write (' done\n  Clean up temporary files ...')
FCFileTools.rmall("html")
#FCFileTools.rmall("latex")
#LogFile.close()

#====================================================================
sys.stdout.write (' done\n  Clean up temporary files ...')

#====================================================================
sys.stdout.write (' done\nDocumentation done!\n')




#print text
