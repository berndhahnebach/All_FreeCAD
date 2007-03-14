#! python
# -*- coding: utf-8 -*-
# (c) 2006 Jürgen Riegel 

Usage = """generate - generates a FreeCAD Module out of an XML model

Usage:
   generate [Optionen] Model.xml Model2.xml Model3.xml ...
   
Options:
 -h, --help               print this help
 
Generate source code out of an model definition.
   
Autor:
  (c) 2006 Juergen Riegel
  juergen.riegel@web.de

Version:
  0.1
"""

import os,sys,string,re,getopt,codecs,binascii
import generateBase.generateModel_Module
import generateTemplates.templateModule
import generateTemplates.templateClassPyExport


# Globals

def generate(filename):
  # load model
  GenerateModelInst = generateBase.generateModel_Module.parse(filename)
  # Convert filename to an absolute path
  filename = os.path.abspath(filename)
  head,tail = os.path.split(filename)
  print head,tail

  if(len(GenerateModelInst.Module)!=0): 
    Module= generateTemplates.templateModule.TemplateModule()
    Module.path = head
    Module.module = GenerateModelInst.Module[0]
    Module.Generate()
    print "Done generating: " + GenerateModelInst.Module[0].Name
  else:
    Export = generateTemplates.templateClassPyExport.TemplateClassPyExport()
    Export.path = head+"/"
    Export.export = GenerateModelInst.PythonExport[0]
    Export.Generate()
    print "Done generating: " + GenerateModelInst.PythonExport[0].Name
    
  
  
  
  


def main():
	
	try:
		opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
	except getopt.GetoptError:
		# print help information and exit:
		sys.stderr.write(Usage)
		sys.exit(2)

	# checking on the options
	for o, a in opts:
		if o in ("-h", "--help"):
			sys.stderr.write(Usage)
			sys.exit()

	# runing through the files
	if (len(args) ==0):
		#sys.stderr.write(Usage)
		generate("../Mod/PartDesign/PartDesign_Model.xml")                  
	else:
		for i in args:
			generate(i)

	
if __name__ == "__main__":
	main()
