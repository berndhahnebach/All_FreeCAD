﻿#! python
# -*- coding: utf-8 -*-
# (c) 2007 Juergen Riegel GPL

Usage = """wiki2chm - conect to a wiki and spider the docu

Usage:
   wiki2chm [Optionen] WikiBaseUrl TocPageName
   
Options:
 -p, --proxy=ProxyUrl     specify a proxy
 -o  --out-path=BASPATH   use this base path the HTML Project
 -h, --help               print this help
 
Exit:
 0      No Error or Warning found
 1      Argument error, wrong or less Arguments given
 2      Run delivers Warnings (printed on standard error)
 10     Run stops with an error (printed on standard error)
 
This programm read the wiki and generate all nececary files
to generat a .chm file.

Examples:
  
   wiki2chm  "http://juergen-riegel.net/FreeCAD/Docu/" Online_Help_Toc
 
Autor:
  (c) 2007 Juergen Riegel
  juergen.riegel@web.de
  Licence: GPL

Version:
  0.1
"""

import os,sys,string,re,getopt,codecs,binascii,datetime,urllib,glob


# Globals
Woff = 0

proxies = {}
WikiBaseUrl = ""
TocPageName = ""
FetchedArticels =[]
BasePath = ""
Wget = "wget"

hhcHeader = """<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<meta name="GENERATOR" content="Microsoft&reg; HTML Help Workshop 4.1">
<!-- Sitemap 1.0 -->
</HEAD><BODY>
<UL>
"""
hhcFooter="""</UL>
</BODY></HTML>
"""
ProjectFile = """
[OPTIONS]
Compatibility=1.1 or later
Compiled file=FreeCAD.chm
Contents file=Online_Help_Toc.hhc
Default topic=index.php@title=Online_Help_Startpage
Display compile progress=Yes
Full-text search=Yes
Language=0x409 Englisch (USA)
[FILES]

"""

def runWget():
	cmdLine = Wget + " "
	cmdLine += "-k -r "    # konvert to local links and do recursive
	cmdLine += "-P tmp "   # write in this subdir
	cmdLine += "-nd "      # flat (no subdirs)
	cmdLine += '-R "*action=*" '      # Reject all action links
	cmdLine += '-R "*title=Special*" '# Reject all special pages
	cmdLine += '-R "*title=Talk*" '   # Reject all Talk pages
	cmdLine += '-R "*oldid=*" '       # Reject all history pages
	cmdLine += '-R "*printable=yes*" '# Reject all print pages
	cmdLine += 'http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Online_Help_Toc '
	
	result = os.popen(cmdLine).read()
	print result
	

def getArticle(Name):
	global proxies,WikiBaseUrl,TocPageName,BasePath
	
	url = WikiBaseUrl + 'index.php?title=' + Name.replace(" ","_") + '&printable=yes'
	
	print "GetFile: " + url
	
	urlFile = urllib.urlopen(url,proxies=proxies)
	Article = urlFile.readlines()
	file = open(BasePath + Name.replace(" ","_") + ".htm","w")
	for i in Article:
		i = i.replace("/FreeCAD/Docu/skins/common/commonPrint.css","test.css")
		file.write(i)
	file.close()

def insertTocEntry(file,Indent,points,name,folder=0):
	print "TocEntry",Indent,points,name
	name = name.replace("\n","")
	IndentStr = ""
	for i in range(points):IndentStr += "   "

	if(points > Indent):
		for i in range(points-Indent):
			file.write("<UL>")
		file.write("\n")
	if(points < Indent):
		for i in range(Indent-points):
			file.write("</UL>")
		file.write("\n")
	file.write(IndentStr + '<LI><OBJECT type="text/sitemap">\n')
	file.write(IndentStr + '      <param name="Name" value="'+ name +'">\n')
	if(not folder):
		file.write(IndentStr + '      <param name="Local" value="index.php@title='+ name.replace(" ","_") + '">\n')
	file.write(IndentStr + '   </OBJECT>\n')
	return points
	
def readToc():
	global proxies,WikiBaseUrl,TocPageName,BasePath
	
	url = WikiBaseUrl + 'index.php?title=Special:Export/' + TocPageName
	print 'Open Toc url: ' + url
	urlFile = urllib.urlopen(url,proxies=proxies)
	Toc = urlFile.readlines()
	
	# compile the regexes needed
	Toc1 = re.compile("^(\*+)\s([\w|\s]*)")
	Toc2 = re.compile("^(\*+)\s\[\[([\w|\s]*)\|([\w|\s]*)\]\]")
	Toc3 = re.compile("^(\*+)\s\[\[([\w|\s]*)\\]\]")
	
	file = open(BasePath + TocPageName +".hhc","w")
	file.write(hhcHeader)
	ListIndent = 1
	
	for line in Toc:
		#print line
		TocMatch = Toc2.search(line);
		if TocMatch:
			print "Match2: ", TocMatch.group(1),TocMatch.group(2),TocMatch.group(3)
			#getArticle(TocMatch.group(2))
			ListIndent = insertTocEntry(file,ListIndent,len(TocMatch.group(1)),TocMatch.group(2))
			continue
		TocMatch = Toc3.search(line);
		if TocMatch:
			print "Match3: ", TocMatch.group(1),TocMatch.group(2)
			#getArticle(TocMatch.group(2))
			ListIndent = insertTocEntry(file,ListIndent,len(TocMatch.group(1)),TocMatch.group(2))
			continue
		TocMatch = Toc1.search(line);
		if TocMatch:
			print "Match1: ", TocMatch.group(1),TocMatch.group(2)
			ListIndent = insertTocEntry(file,ListIndent,len(TocMatch.group(1)),TocMatch.group(2),1)
			continue
	for i in range(ListIndent-1):
		file.write("</UL>")
	file.write("\n")

	file.write(hhcFooter)

def WriteProject():
	global proxies,WikiBaseUrl,TocPageName,BasePath
	
	file = open(BasePath + TocPageName +".hhp","w")
	file.write(ProjectFile)
	
	for FileEntry in os.listdir(BasePath) :
		if(not FileEntry == TocPageName +".hhp"):
			file.write(FileEntry + '\n')
	
	file.close()
	
def replaceCSS():
	paths = glob.glob(BasePath + 'index.php*')
	for file in paths:
		print "Replace: ",file
		input = open(file,'r')
		output = open(file + '_temp','w')
		for l in input:
			output.write(l.replace('"/FreeCAD/Docu/skins/monobook/main.css?9"','"chm.css"'))
		output.close()
		input.close()
		os.remove(file)
		os.rename(file + '_temp',file)
		
def main():
	global proxies,WikiBaseUrl,TocPageName,BasePath
	Proxy = ""
	Qout = None
	DFQout = None
	CSVout = None
	NoOut  = 0
	
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hwp:o:", ["help", "warnoff",  "proxy=","out-path="])
	except getopt.GetoptError:
		# print help information and exit:
		sys.stderr.write(Usage)
		sys.exit(2)

	# checking on the options
	for o, a in opts:
		if o == "-v":
			verbose = True
		if o in ("-h", "--help"):
			sys.stderr.write(Usage)
			sys.exit()
		if o in ("-w", "--warnoff"):
			Woff = 1
		if o in ("-p", "--proxy"):
			Proxy = a
		if o in ("-o", "--out-path"):
			print "Using output path: " + a +"\n"
			BasePath = a

	# runing through the files
	if(Proxy == ""):
		proxies = {}
		print 'Using no proxy'
	else:
		proxies = {'http':Proxy}
		print 'Using proxy: ' + Proxy

	if (len(args) !=2):
		sys.stderr.write("Wrong number of arguments!\n\n")
		sys.stderr.write(Usage)
		sys.exit(2)
	else:
		WikiBaseUrl = args[0]
		TocPageName = args[1]
		runWget()
		readToc()
		


if __name__ == "__main__":
	main()
