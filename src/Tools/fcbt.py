#!python
#  FreeCAD Build Tool
# (c) 2004 Juergen Riegel


import os,sys,string

help1 = """
FreeCAD Build Tool
Usage:
   fcbt <module name> [module parameter]
possible modules are:
 - DistSrc       Build a source Distribution of the aktuall source tree
 - DistBin       Build a binary Distribution of the aktuall source tree
 - DistSetup     Build a Setup Distribution of the aktuall source tree
 - DistAll       Run all three above modules
 - BuildDoc      Create the documentation (source docs)
 - CreateModule  Insert a new FreeCAD Module in the module directory

For help on the modules type:
  fcbt <module name> ?

Jürgen Riegel 2004
"""

if(len(sys.argv) < 2):
	sys.stdout.write(help1)
	sys.exit(1)

Cmd = string.lower(sys.argv[1])


if Cmd == "distsrc":
	import fcbt.DistSrc
elif Cmd == "distbin":
	import fcbt.DistBin
elif Cmd == "distsetup":
	import fcbt.DistSetup
elif Cmd == "distall":
	import fcbt.DistSrc
	import fcbt.DistBin
	import fcbt.DistSetup
elif Cmd == "builddoc":
	import fcbt.BuildDoc
else:
	print sys.argv[1] + " is an unknown command!\n"
	sys.exit(1)

	
	

	