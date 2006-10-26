#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel 

Usage = """generate - generates a FreeCAD Module out of an XML model

Usage:
   generate [Optionen] Model1.xml Model2.xml Model3.xml ...
   
Options:
 -q, --qmlout=FILENAME    write the parsed file as QML to FileName
 -p  --path=BASPATH       use this base path for inspection plan
 -n, --noout              Pars the file but generate no output
 -i, --iplan              Generates only an inspection plan
 -w, --woff	              Dont print Warnings
 -h, --help               print this help
 
Exit:
 0      No Error or Warning found
 1      Argument error, wrong or less Arguments given
 2      Run delivers Warnings (printed on standard error)
 10     Run stops with an error (printed on standard error)
 

Examples:
  Als Filter:
   cat  AuditLog.txt | robfit2qml > Test3.qml
   
Autor:
  (c) 2006 Juergen Riegel
  juergen.riegel@web.de

Version:
  0.1
"""

import os,sys,string,re,getopt,codecs,binascii
import generateBase.generateModel_Module
import generateTemplates.templateModule


# Globals

def generate(filename):
  # load model
  GenerateModelInst = generateBase.generateModel_Module.parse(filename)
  head,tail = os.path.split(filename)
  print head,tail

  Module= generateTemplates.templateModule.TemplateModule()
  Module.path = head
  Module.module = GenerateModelInst.Module[0]
  Module.Generate()
  
  print "Done generating: " + GenerateModelInst.Module[0].Name
  
  


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
