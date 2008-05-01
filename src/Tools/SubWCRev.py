# FreeCAD RevInfo script to get the revision information from Subversion.
# (c) 2006 Werner Mayer
#
# Under Linux the Subversion tool SubWCRev shipped with TortoiseSVN isn't 
# available which is provided by this script. 

#***************************************************************************
#*   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

#!/usr/bin/python
import os,sys,string,re,time
import xml.sax
import xml.sax.handler
import xml.sax.xmlreader
import StringIO

# SAX handler to parse the subversion output
class SvnHandler(xml.sax.handler.ContentHandler):
  def __init__(self):
    self.inUrl = 0
    self.inDate = 0
    self.mapping = {}
 
  def startElement(self, name, attributes):
    if name == "entry":
      self.buffer = ""
      self.mapping["Rev"] = attributes["revision"]
    elif name == "url":
      self.inUrl = 1
    elif name == "date":
      self.inDate = 1
 
  def characters(self, data):
    if self.inUrl:
      self.buffer += data
    elif self.inDate:
      self.buffer += data
 
  def endElement(self, name):
    if name == "url":
      self.inUrl = 0
      self.mapping["Url"] = self.buffer
      self.buffer = ""
    elif name == "date":
      self.inDate = 0
      self.mapping["Date"] = self.buffer
      self.buffer = ""


def main():
	#if(len(sys.argv) != 2):
	#    sys.stderr.write("Usage:  SubWCRev \"`svn info .. --xml`\"\n")

	parser=xml.sax.make_parser()
	handler=SvnHandler()
	parser.setContentHandler(handler)

	#Create an XML stream with the required information and read in with a SAX parser
	Ver=os.popen("svnversion . -n").read()
	Info=os.popen("svn info . --xml").read()
	try:
		inpsrc = xml.sax.InputSource()
		strio=StringIO.StringIO(Info)
		inpsrc.setByteStream(strio)
		parser.parse(inpsrc)
	except:
		sys.stderr.write("No svn repository\n")
		sys.exit(2)

	#Information of the Subversion stuff
	Url = handler.mapping["Url"]
	Rev = handler.mapping["Rev"]
	Date = handler.mapping["Date"]
	Date = Date[:19]
	#Same format as SubWCRev does
	Date = string.replace(Date,'T',' ')
	Date = string.replace(Date,'-','/')

	#Date is given as GMT. Now we must convert to local date.
	m=time.strptime(Date,"%Y/%m/%d %H:%M:%S")
	#Copy the tuple and set tm_isdst to 0 because it's GMT
	l=(m.tm_year,m.tm_mon,m.tm_mday,m.tm_hour,m.tm_min,m.tm_sec,m.tm_wday,m.tm_yday,0)
	#Take timezone into account
	t=time.mktime(l)-time.timezone
	Date=time.strftime("%Y/%m/%d %H:%M:%S",time.localtime(t))

	#Get the current local date
	Time = time.strftime("%Y/%m/%d %H:%M:%S")

	Mods = 'Src not modified'
	Mixed = 'Src not mixed'
	Range = Rev

	# if version string ends with an 'M'
	r=re.search("M$",Ver)
	if r != None:
	    Mods = 'Src modified'

	# if version string contains a range
	r=re.match("^\\d+\\:\\d+",Ver)
	if r != None:
	    Mixed = 'Src mixed'
	    Range = Ver[:r.end()]

	# Open the template file and the version file
	file = open("./src/Build/Version.h.in")
	lines = file.readlines()
	file.close()
	out  = open("./src/Build/Version.h","w");

	for line in lines:
	    line = string.replace(line,'$WCREV$',Rev)
	    line = string.replace(line,'$WCDATE$',Date)
	    line = string.replace(line,'$WCRANGE$',Range)
	    line = string.replace(line,'$WCURL$',Url)
	    line = string.replace(line,'$WCNOW$',Time)
	    line = string.replace(line,'$WCMODS?Src modified:Src not modified$',Mods)
	    line = string.replace(line,'$WCMIXED?Src mixed:Src not mixed$',Mixed)
	    # output
	    out.write(line)

	out.write('\n')
if __name__ == "__main__":
	main()
