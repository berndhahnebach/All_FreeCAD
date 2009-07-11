#! python
# -*- coding: utf-8 -*-
# (c) 2009 Werner Mayer  GPL

Usage = """makedebian - Build script to build a complete FreeCAD Debian package

Usage:
   makedebian [Optionen] ReleaseNbr
   
Options:
 -h, --help          print this help
 -b, --buildPath     specify the output path where the build takes place
 
This script will build a complete FreeCAD distribution which includes:
* check out sources
* packaging sources
* set the version and release numbers
* gathering change log
* completely build FreeCAD
* run tests
* build source docu
* build user docu
* build Debian package
   
On failure of one of these steps the script will stop.
Each step writes tons of info in the log file.
There is one error log file.

Autor:
  (c) 2009 Werner Mayer
	Licence: GPL

Version:
  0.1
"""

"""
* apt-get update
* apt-get upgrade
* edit configure.ac: revision number
* chmod u+x autogen.sh
* ./autogen.sh
* ./configure
* make dist

* tar -xzf FreeCAD-X.Y.xyz.tar.gz
* rename folder to freecad-X.Y.xyz
* rename tarball to freecad_X.Y.xyz.orig.tar.gz

* cd freecad-X.Y.xyz
* patch -p1 < .../debian/diff/freecad_X.Y.xyz-1hardy1.diff
* dch -v X.Y.xyz-1hardy1 "New release for Ubuntu 8.04 (Hardy Heron)"
* edit changelog
* edit rules to add further modules
* chmod u+x autogen.sh
* debuild
* add freecad_X.Y.xyz-1hardy1.diff to svn
* upload to sftp://wmayer@frs.sourceforge.net/incoming/w/wm/wmayer/uploads
"""

from subprocess import call,Popen,PIPE
from time import sleep
from zipfile import ZipFile,ZIP_DEFLATED
from string import find
import os,sys,getopt
import tarfile
import ConfigParser
import time

# global information 
Release = 0
Major = 0
Minor = 7
Alias = ""
FileName = ""
BuildPath = None
LogFile = None
Config = None
Url = "https://free-cad.svn.sourceforge.net/svnroot/free-cad/trunk"


ERROR_STR= """Error removing %(path)s, %(error)s """

def rmgeneric(path, __func__):

    try:
        __func__(path)
        print 'Removed ', path
    except OSError, (errno, strerror):
        print ERROR_STR % {'path' : path, 'error': strerror }
            
def removeall(path):

    if not os.path.isdir(path):
        return
    
    files=os.listdir(path)

    for x in files:
        fullpath=os.path.join(path, x)
        if os.path.isfile(fullpath):
            f=os.remove
            rmgeneric(fullpath, f)
        elif os.path.isdir(fullpath):
            removeall(fullpath)
            f=os.rmdir
            rmgeneric(fullpath, f)

def CallProcess(args,Msg,ret=True):
	Anim = ['-','\\','|','/']
	
	sys.stdout.write(Msg+':  ')
	LogFile.write("====== Call: " + args[0] + '\n')
	LogFile.flush()
	cmd = Popen(args, stdout=LogFile, stderr=LogFile)
	
	i = 0
	while(cmd.poll() == None):
		#line = cmd.stdout.readline()
		#if(line):
		#	LogFile.write(line)
		sys.stdout.write(chr(8) + Anim[i%4])
		i+=1
		sleep(0.2)
	
	sys.stdout.write(chr(8) + "done\n")
	if(not cmd.returncode == 0 and ret):
		print args[0],"returns:",cmd.returncode
		raise

# Step 2 & 3
def CheckOut():
	
	CallProcess(["svn", "checkout", "-r", `Release`, Url, "../"+FileName],
		     "2) Checking out")

	sys.stdout.write('3) Write version files: ')
	
	Version = open("src/Build/Version.h","w")
	Version.write('#define FCVersionMajor "' + `Major` + '"\n')
	Version.write('#define FCVersionMinor "' + `Minor` + '"\n')
	Version.write('#define FCVersionName "' + Alias + '"\n')
	Version.write('#define FCRevision "' + `Release` + '"\n')
	Version.write('#define FCRepositoryURL "' + Url + '"\n')
	Version.write('#define FCCurrentDateT  "'+time.asctime()+'"  \n')
	Version.close()
	
	Config = open("configure.ac","r")
	lines = Config.readlines()
	Config.close()
	for i in range(len(lines)):
		if (lines[i].startswith("m4_define([FREECAD_MAJOR], ")):
			lines[i] = "m4_define([FREECAD_MAJOR], [" + `Major` + "])\n"
		elif (lines[i].startswith("m4_define([FREECAD_MINOR], ")):
			lines[i] = "m4_define([FREECAD_MINOR], [" + `Minor` + "])\n"
		elif (lines[i].startswith("m4_define([FREECAD_MICRO], ")):
			lines[i] = "m4_define([FREECAD_MICRO], [" + `Release` + "])\n"
	Config = open("configure.ac","w")
	Config.writelines(lines)
	Config.close()
	
	sys.stdout.write('done\n')

# Step 4
def MakeDist():
	
	CallProcess(["./autogen.sh"], "4) Make source package")
	CallProcess(["./configure"],"   Run configure")
	CallProcess(["make", "dist"],"   make dist")

# Step 5
#def BuildAll():
	
	#removeall("FreeCAD-0.7.2008")
	#os.rmdir("FreeCAD-0.7.2008")
	#CallProcess(["tar","-xzf","FreeCAD-0.7.2008.tar.gz"],"")
	#os.chdir("FreeCAD-0.7.2008")
	#CallProcess(["debuild"],"")

def main():
	global Release, Major, Minor, Alias, FileName, BuildPath, LogFile, Config
	IniFile = "../BuildRelease.ini"
	try:
		#default build path
		BuildPath = "/home/" + os.getlogin() + "/tmp"
		opts, args = getopt.getopt(sys.argv[1:], "hb:", ["help","buildPath="])
	except getopt.GetoptError:
		# print help information and exit:
		sys.stderr.write(Usage)
		sys.exit(2)

	# checking on the options
	for o, a in opts:
		if o in ("-h", "--help"):
			sys.stderr.write(Usage)
			sys.exit()
		if o in ("-b", "--buildPath"):
			BuildPath = a


	# running through the files
	if (not len(args) == 1):
		sys.stderr.write(Usage)
		sys.exit()
	
	Release = int(args[0])
	
	Config = ConfigParser.ConfigParser()
	Config.readfp(open(IniFile))
	
	Alias   = Config.get('Version','Alias')
	Major   = Config.getint('Version','Major')
	Minor   = Config.getint('Version','Minor')

	# creating the directory and switch to
	FileName = 'FreeCAD_' + `Major` + '_' + `Minor` + '_' + `Release`
	print "=== Building:", FileName, '\n'
	BuildPath = BuildPath + '/' + FileName
	# set tool path 
	sys.path.append((BuildPath + '/src/Tools') )
	OldCwd = os.getcwd()
	print "1) Creating Build directory: ", BuildPath
	if not os.path.isdir(BuildPath):
		os.mkdir(BuildPath)
	os.chdir(BuildPath)
	LogFile = open("BuildRelease.log","w")
	
	try:
		CheckOut()
		MakeDist()
		#BuildAll()
		#SendFTP()
	except:
		LogFile.close()
		sys.stderr.write("Error raised!\n")
		#raise Exception("Build script aborted")
		raise
	
	os.chdir(OldCwd)
	LogFile.close()

	#print "Press any key"
	#sys.stdin.readline()

if __name__ == "__main__":
	main()
