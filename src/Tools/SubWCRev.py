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
import os,sys,string,time
import xml.sax
import xml.sax.handler

class SvnHandler(xml.sax.handler.ContentHandler):
  def __init__(self):
    self.inUrl = 0
    self.inDate = 0
    self.mapping = {}
 
  def startElement(self, name, attributes):
    if name == "entry":
      self.buffer = ""
      self.mapping["Rev"] = attributes["revision"]
    elif name == "root":
      self.inUrl = 1
    elif name == "date":
      self.inDate = 1
 
  def characters(self, data):
    if self.inUrl:
      self.buffer += data
    elif self.inDate:
      self.buffer += data
 
  def endElement(self, name):
    if name == "root":
      self.inUrl = 0
      self.mapping["Url"] = self.buffer
      self.buffer = ""
    elif name == "date":
      self.inDate = 0
      self.mapping["Date"] = self.buffer
      self.buffer = ""


#if(len(sys.argv) != 2):
#    sys.stderr.write("Usage:  SubWCRev \"`svn info .. --xml`\"\n")

parser=xml.sax.make_parser()
handler=SvnHandler()
parser.setContentHandler(handler)

#Create an XML file with the required information and read in with a SAX parser
os.system("svn info .. --xml > ../Build/Version.xml")
parser.parse("../Build/Version.xml")
os.remove("../Build/Version.xml")

file = open("../Build/Version.h.in")
out  = open("../Build/Version.h","w");

Url = handler.mapping["Url"]
Rev = handler.mapping["Rev"]
Date = handler.mapping["Date"]
Date = Date[:19]
#Same format as SubWCRev does
Date = string.replace(Date,'T',' ')
Date = string.replace(Date,'-','/')
Time = time.strftime("%Y/%m/%d %H:%M:%S")

lines = file.readlines()
for line in lines:
    line = string.replace(line,'$WCREV$',Rev)
    line = string.replace(line,'$WCDATE$',Date)
    line = string.replace(line,'$WCRANGE$',Rev)
    line = string.replace(line,'$WCURL$',Url)
    line = string.replace(line,'$WCNOW$',Time)
    line = string.replace(line,'$WCMODS?Src modified:Src not modified$','Src not modified')
    line = string.replace(line,'$WCMIXED?Src mixed:Src not mixed$','Src not mixed')
    # output
    out.write(line)
out.write('\n')
