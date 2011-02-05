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
import os,sys,string,re,time,getopt
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

class VersionControl:
    def __init__(self):
        self.rev = ""
        self.date = ""
        self.range = ""
        self.url = ""
        self.time = ""
        self.mods = "Src not modified"
        self.mixed = "Src not mixed"

    def extractInfo(self):
        return False

    def writeVersion(self, lines):
        for line in lines:
            line = string.replace(line,'$WCREV$',self.rev)
            line = string.replace(line,'$WCDATE$',self.date)
            line = string.replace(line,'$WCRANGE$',self.range)
            line = string.replace(line,'$WCURL$',self.url)
            line = string.replace(line,'$WCNOW$',self.twime)
            line = string.replace(line,'$WCMODS?Src modified:Src not modified$',self.mods)
            line = string.replace(line,'$WCMIXED?Src mixed:Src not mixed$',self.mixed)

class UnknownControl(VersionControl):
    def extractInfo(self):
        self.rev = "Unknown"
        self.date = "Unknown"
        self.range = "Unknown"
        self.url = "Unknown"
        self.time = "Unknown"
        return True

class Subversion(VersionControl):
    def extractInfo(self):
        parser=xml.sax.make_parser()
        handler=SvnHandler()
        parser.setContentHandler(handler)

        #Create an XML stream with the required information and read in with a SAX parser
        Ver=os.popen("svnversion %s -n" % (srcdir)).read()
        Info=os.popen("svn info %s --xml" % (srcdir)).read()
        try:
            inpsrc = xml.sax.InputSource()
            strio=StringIO.StringIO(Info)
            inpsrc.setByteStream(strio)
            parser.parse(inpsrc)
            return True
        except:
            return False

        #Information of the Subversion stuff
        self.url = handler.mapping["Url"]
        self.rev = handler.mapping["Rev"]
        self.date = handler.mapping["Date"]
        self.date = self.date[:19]
        #Same format as SubWCRev does
        self.date = string.replace(self.date,'T',' ')
        self.date = string.replace(self.date,'-','/')

        #Date is given as GMT. Now we must convert to local date.
        m=time.strptime(self.date,"%Y/%m/%d %H:%M:%S")
        #Copy the tuple and set tm_isdst to 0 because it's GMT
        l=(m.tm_year,m.tm_mon,m.tm_mday,m.tm_hour,m.tm_min,m.tm_sec,m.tm_wday,m.tm_yday,0)
        #Take timezone into account
        t=time.mktime(l)-time.timezone
        self.date=time.strftime("%Y/%m/%d %H:%M:%S",time.localtime(t))

        #Get the current local date
        self.time = time.strftime("%Y/%m/%d %H:%M:%S")

        self.mods = 'Src not modified'
        self.mixed = 'Src not mixed'
        self.range = self.rev

        # if version string ends with an 'M'
        r=re.search("M$",Ver)
        if r != None:
            self.mods = 'Src modified'

        # if version string contains a range
        r=re.match("^\\d+\\:\\d+",Ver)
        if r != None:
            self.mixed = 'Src mixed'
            self.range = Ver[:r.end()]
        return True


def main():
    #if(len(sys.argv) != 2):
    #    sys.stderr.write("Usage:  SubWCRev \"`svn info .. --xml`\"\n")

    srcdir="."
    bindir="."
    try:
        opts, args = getopt.getopt(sys.argv[1:], "sb:", ["srcdir=","bindir="])
    except getopt.GetoptError:
        pass

    for o, a in opts:
        if o in ("-s", "--srcdir"):
            srcdir = a
        if o in ("-b", "--bindir"):
            bindir = a

    parser=xml.sax.make_parser()
    handler=SvnHandler()
    parser.setContentHandler(handler)

    #Create an XML stream with the required information and read in with a SAX parser
    Ver=os.popen("svnversion %s -n" % (srcdir)).read()
    Info=os.popen("svn info %s --xml" % (srcdir)).read()
    try:
        inpsrc = xml.sax.InputSource()
        strio=StringIO.StringIO(Info)
        inpsrc.setByteStream(strio)
        parser.parse(inpsrc)
    except:
        sys.stderr.write("No svn repository\n")
        out  = open("%s/src/Build/Version.h" % (bindir),"w");
        out.write("\n")
        out.write("#define FCVersionMajor \"0\"\n")
        out.write("#define FCVersionMinor \"11\"\n")
        out.write("#define FCVersionName \"Dagoba\"\n")
        out.write("#define FCRevision \"Unknown\"\n")
        out.write("#define FCRevisionDate \"Unknown\"\n")
        out.write("#define FCRepositoryURL \"Unknown\"\n")
        out.write("#define FCCurrentDateT \"Unknown\"\n")
        out.close()
        return # exit normally to not stop a build where no svn is installed

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
    file = open("%s/src/Build/Version.h.in" % (srcdir))
    lines = file.readlines()
    file.close()
    out  = open("%s/src/Build/Version.h" % (bindir),"w");

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
    out.close()
    sys.stdout.write("%s/src/Build/Version.h written\n" % (bindir))
if __name__ == "__main__":
    main()

