#!/usr/bin/python

import os,sys,string

#os.chdir("E:\\Develop\\FreeCADWin\\scripts")


file = open(sys.argv[1])

if(len(sys.argv) > 3):
    sys.stderr.write("Wrong Parameter\n  Usage:\n  PythonToCPP Infile.py [Outfile]\n")

if(len(sys.argv) > 2):
    out = open(sys.argv[2],"w");
else:
    out = sys.stdout

lines = file.readlines()

out.write("const char " + sys.argv[1][:-3] + "[] =")

for line in lines:
    # remove new line
    line2 = string.rstrip(line)
    # replace special chars
    line2 = string.replace(line2,'\\','\\\\')
    line2 = string.replace(line2,'\"','\\\"')
    line2 = string.replace(line2,"\'","\\\'")

    
    # output 
    #out.write(line)
    out.write( '\"' + line2 + '\\n\"\n')

out.write(";\n\n\n");

    

    