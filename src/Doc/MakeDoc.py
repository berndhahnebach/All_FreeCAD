

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
LogFile = open("MakeDoc.log",'w')
if not os.path.isdir("../../Doc"):
    os.mkdir("../../Doc")
if not os.path.isdir("../../Doc/res"):
    os.mkdir("../../Doc/res")
FCFileTools.cpfile("index.html","../../doc/index.html")
FCFileTools.cpfile("FreeCAD.css","../../doc/res/FreeCAD.css")

#====================================================================
sys.stdout.write ('Running source documentation ...')
# running doxygen with the parameters from the config file
text = os.popen("doxygen doxygen.cfg").read()
LogFile.write(text)
if not os.path.isdir("../../Doc/FrameWork"):
    os.mkdir("../../Doc/FrameWork")

#====================================================================
sys.stdout.write( ' done\n  Generate HTML ...')
FCFileTools.cpall("html","../../doc/FrameWork")

#====================================================================
sys.stdout.write(' done\n  Generate DVI ...')
os.chdir("latex")
text = os.popen("latex refman.tex").read()
LogFile.write(text)
text = os.popen("makeindex refman.idx").read()
LogFile.write(text)
text = os.popen("latex refman.tex").read()
text = os.popen("latex refman.tex").read()
text = os.popen("latex refman.tex").read()
FCFileTools.cpfile("refman.dvi","../../../doc/FrameWork/FrameWork.dvi")

#====================================================================
sys.stdout.write (' done\n  Generate PS ...')
text = os.popen("dvips refman.dvi").read()
LogFile.write(text)
FCFileTools.cpfile("refman.ps","../../../doc/FrameWork/FrameWork.ps")

#====================================================================
sys.stdout.write (' done\n  Generate PDF ...')
text = os.popen("pdflatex refman.tex").read()
LogFile.write(text)
FCFileTools.cpfile("refman.pdf","../../../doc/FrameWork/FrameWork.pdf")
os.chdir("..")

#====================================================================
sys.stdout.write (' done\n  Clean up temporary files ...')
FCFileTools.rmall("html")
FCFileTools.rmall("latex")

#====================================================================
sys.stdout.write (' done\nCreating manuals\n')
if not os.path.isdir("../../Doc/Manuals"):
    os.mkdir("../../Doc/Manuals")
os.chdir("Manuals")

#====================================================================
sys.stdout.write('  Generate DVI ...')
text = os.popen("latex Design_Specification.tex").read()
LogFile.write(text)
text = os.popen("makeindex Design_Specification.idx").read()
LogFile.write(text)
text = os.popen("latex Design_Specification.tex").read()
text = os.popen("latex Design_Specification.tex").read()
text = os.popen("latex Design_Specification.tex").read()
FCFileTools.cpfile("Design_Specification.dvi","../../../doc/Manuals/Design_Specification.dvi")

text = os.popen("latex Manual.tex").read()
LogFile.write(text)
text = os.popen("makeindex Manual.idx").read()
LogFile.write(text)
text = os.popen("latex Manual.tex").read()
text = os.popen("latex Manual.tex").read()
text = os.popen("latex Manual.tex").read()
FCFileTools.cpfile("Manual.dvi","../../../doc/Manuals/Manual.dvi")

#====================================================================
sys.stdout.write (' done\n  Generate PS ...')
text = os.popen("dvips Design_Specification.dvi").read()
LogFile.write(text)
FCFileTools.cpfile("Design_Specification.ps","../../../doc/Manuals/Design_Specification.ps")
text = os.popen("dvips Manual.dvi").read()
LogFile.write(text)
FCFileTools.cpfile("Manual.ps","../../../doc/Manuals/Manual.ps")

#====================================================================
sys.stdout.write (' done\n  Generate PDF ...')
text = os.popen("pdflatex Design_Specification.tex").read()
LogFile.write(text)
FCFileTools.cpfile("Design_Specification.pdf","../../../doc/Manuals/Design_Specification.pdf")
text = os.popen("pdflatex Manual.tex").read()
LogFile.write(text)
FCFileTools.cpfile("Manual.pdf","../../../doc/Manuals/Manual.pdf")

#====================================================================
#== run latex2html now NOTE: current directory MUST NOT contain any spaces !!!
sys.stdout.write (' done\n  Generate HTML ...')
if not os.path.isdir("../../../doc/Manuals/Design_Specification"):
    os.mkdir("../../../doc/Manuals/Design_Specification")
text = os.popen("latex2html Design_Specification.tex").read()
LogFile.write(text)
# if latex2html failed this directory doesn't exist
if os.path.isdir("Design_Specification"):
    FCFileTools.cpall("Design_Specification","../../../doc/Manuals/Design_Specification")
else:
    sys.stderr.write("latex2html failed!\n")

if not os.path.isdir("../../../doc/Manuals/Manual"):
    os.mkdir("../../../doc/Manuals/Manual")
text = os.popen("latex2html Manual.tex").read()
LogFile.write(text)
# if latex2html failed this directory doesn't exist
if os.path.isdir("Manual"):
    FCFileTools.cpall("Manual","../../../doc/Manuals/Manual")

#====================================================================
os.chdir("..")
sys.stdout.write (' done\n  copy online help ...')
if not os.path.isdir("../../Doc/Online"):
    os.mkdir("../../Doc/Online")
FCFileTools.cpall("Online","../../Doc/Online")

#====================================================================
sys.stdout.write (' done\n  Clean up temporary files ...')
LogFile.close()

#====================================================================
sys.stdout.write (' done\nDocumentation done!\n')




#print text
