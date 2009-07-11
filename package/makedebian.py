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
BuildPath = "/tmp/build"
Log = None
ErrLog = None
Config = None
Url = "https://free-cad.svn.sourceforge.net/svnroot/free-cad/trunk"


def CallProcess(args,Msg,ret=True):
	Anim = ['-','\\','|','/']
	
	sys.stdout.write(Msg+':  ')
	Log.write("====== Call: " + args[0] + '\n')
	SVN = Popen(args, stdout=PIPE, stderr=ErrLog)
	
	i = 0
	while(SVN.poll() == None):
		line = SVN.stdout.readline()
		if(line):
			Log.write(line.replace('\n',''))
		sys.stdout.write(chr(8) + Anim[i%4])
		i+=1
		sleep(0.2)
	
	#ErrLog.write(SVN.stdout.read())
	sys.stdout.write(chr(8) + "done\n")
	if(not SVN.returncode == 0 and ret):
		print "Process returns: ",SVN.returncode
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
	
	sys.stdout.write('done\n')

# Step 4
def MakeDist():
	
	CallProcess(["./autogen.sh"], "4) Make source package")
	CallProcess(["./configure"],"")
	CallProcess(["make", "dist"],"")
	
	sys.stdout.write('done\n')

# Step 5
def BuildAll():
	
	CallProcess(["tar","-xzf","FreeCAD-0.7.2008.tar.gz"],"")
	os.chdir("FreeCAD-0.7.2008")
	CallProcess(["debuild"],"")
	
	sys.stdout.write('done\n')

def main():
	global Release, Major, Minor, Alias, FileName, BuildPath, Log, ErrLog, Config
	IniFile = "../BuildRelease.ini"
	try:
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
	Log = open("BuildRelease.log","w")
	ErrLog = open("BuildReleaseErrors.log","w")
	
	try:
		#CheckOut()
		#MakeDist()
		BuildAll()
		#SendFTP()
	except:
		Log.close()
		ErrLog.close()
		Err = open("BuildReleaseErrors.log","r")
		sys.stderr.write("Error raised:\n")
		sys.stderr.write(Err.read())
		raise
	
	os.chdir(OldCwd)
	Log.close()
	ErrLog.close()

	print "Press any key"
	sys.stdin.readline()

if __name__ == "__main__":
	main()
