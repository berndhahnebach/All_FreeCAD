#! python
# -*- coding: utf-8 -*-
# (c) 2007 Juergen Riegel GPL

Usage = """dir2qrc - merging all files in a directory in a qrc file

Usage:
   dir2qrc [Optionen] 
   
Options:
 -v, --verbose            print out all files collected
 -o  --out-file=FILENAME  use this file name for output, default resorces.qrc
 -d, --directory=DIRNAME  directory to search, default PWD
 -h, --help               print this help message
 
This programm walk a directory (tree) and collect all supported files
and put them in a .qrc file, to compile in with the QT resource facility.

Examples:
  
   dir2qrc  -v -o resource.qrc -d "d:/Develop/Resources"
 
Autor:
  (c) 2007 Juergen Riegel
  juergen.riegel@web.de
  Licence: GPL

Version:
  0.1
"""

import os,sys,string,re,getopt,codecs
from os.path import join, getsize

# Globals
Verbose = False
Dir = '.'
Output = 'resources.qrc'

hhcHeader = """<!DOCTYPE RCC><RCC version="1.0">
<qresource> 
"""
hhcFooter="""</qresource>
</RCC> 
"""

EndingList = ['.xpm','.svg','.qm','.png']

def main():
	global Verbose,Dir,Output
	
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hvd:o:", ["help", "verbose",  "directory=","out-file="])
	except getopt.GetoptError:
		# print help information and exit:
		sys.stderr.write(Usage)
		sys.exit(2)

	# checking on the options
	for o, a in opts:
		if o == "-v":
			Verbose = True
		if o in ("-h", "--help"):
			sys.stderr.write(Usage)
			sys.exit()
		if o in ("-o", "--out-file"):
			Output = a
		if o in ("-d", "--directory"):
			print "Using path: " + a +"\n"
			Dir = a

	file = open(Output,"w")
	file.write(hhcHeader)
	for root, dirs, files in os.walk(Dir):
		for name in files:
			if( (1 in [c in name for c in EndingList]) and not ('.svn' in root) ):
				FilePathOrg = join(root,name)
				FilePath = FilePathOrg.replace('.\\','')
				FilePath = FilePath.replace('\\','/')
				if Verbose: print FilePathOrg + ' -> ' + FilePath 
				
				file.write('<file>' + FilePath + '</file>\n')


	file.write("\n")
	file.write(hhcFooter)
	file.close()

if __name__ == "__main__":
	main()
