#!/usr/bin/python

import os,sys,string


file = open(sys.argv[1])

if(len(sys.argv) > 3):
    sys.stderr.write("Wrong Parameter\n  Usage:\n  tstocpp Infile.ts [Outfile]\n")

if(len(sys.argv) > 2):
    out = open(sys.argv[2],"w");
else:
    out = sys.stdout

lines = file.readlines()

out.write("\nstd::vector<const char*> " + sys.argv[1][:-3] + ";\n")
out.write("\nconst std::vector<const char*>& Get" + sys.argv[1][:-3] + "()\n{\n")

for line in lines:
    # remove new line
    line2 = string.rstrip(line)
    # replace special chars
    line2 = string.replace(line2,'\\','\\\\')
    line2 = string.replace(line2,'\"','\\\"')
    line2 = string.replace(line2,"\'","\\\'")

    
    # output 
    #out.write(line)
    out.write( '  ' + sys.argv[1][:-3] + '.push_back(\"' + line2 + '\\n\");\n')

out.write("  return " + sys.argv[1][:-3] + ";\n}\n")
