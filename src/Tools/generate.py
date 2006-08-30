#! python
# -*- coding: utf-8 -*-
# (c) 2006 Juergen Riegel DCX RBP/MJ

Usage = """generateFeature - generates a new Feature in a Module

Usage:
   robfit2qml [Optionen] Log1.txt Log2.txt Log3.txt ...
   
Options:
 -q, --qmlout=FILENAME    write the parsed file as QML to FileName
 -p  --path=BASPATH       use this base path for inspection plan
 -n, --noout              Pars the file but generate no output
 -i, --iplan              Generates only an inspection plan
 -w, --woff	              Dont print Warnings
 -h, --help               print this help
 
Exit:
 0      No Error or Warning found
 1      Argument error, wrong or less Arguments given
 2      Run delivers Warnings (printed on standard error)
 10     Run stops with an error (printed on standard error)
 
Dieses Programm liest eine oder mehrere Robfitlogdateien und extrahiert
die Sensormessungen. Die Messwerter k√∂nnen als QML wieder ausgegeben 
werden. Dies erm√∂glicht das Importieren und Bearbeiten in typische 
Qualit√§tsmanagementsysteme.

Es wird f√ºr die gefundenen Merkmale eine Standardpr√ºfplanung erzeugt.
Diese kann durch die --path option beeinflu√üt werden, um z.B. verschiedene
Anlagen unterscheiden zu k√∂nnen. Die --iplan Obtion gibt nur die 
Pr√ºfplanung aus. 

Examples:
  Als Filter:
   cat  AuditLog.txt | robfit2qml > Test3.qml
   
  Direkt:
   robfit2qml -q Audit.qml AuditLog.txt
   
  Mit einem Pr√ºfplanpr√§fix:
   robfit2qml -q Audit.qml -p /Part:Robfit/Part:Anlage1 AuditLog.txt
  
  Agregiert die Messungen (nur sinvoll f√ºr die gleiche Anlage):
   robfit2qml -q Audit.qml AuditLog1.txt AuditLog2.txt AuditLog3.txt ...
   
Autor:
  (c) 2006 DaimlerChrysler RBP/MJ Juergen Riegel
  juergen.riegel@daimlerchrysler.com
  juergen.riegel@web.de

Version:
  0.1
"""

import os,sys,string,re,getopt,codecs,binascii


# Globals
Woff = 0
Iplan = 0
ErrFileName = "<none>"
ErrFileLine = 0
NumberOfWarnings = 0
MaxValues = 0
# Inspection plan stuff
BasePath = '/Part:Root'
PathList = {}
RootPart = None

# Root of the Measurements
Measurements = []

Months = {'Jan':'01','Feb':'02','Mar':'03','Apr':'04','May':'05','Jun':'06','Jul':'07','Aug':'08','Sep':'09','Nov':'10','Sep':'11','Dez':'12'}

def uuid():
	uuid = ''.join(['%02X' % ord(x) for x in os.urandom(16)])
	return uuid[0:8]+'-'+uuid[8:12]+'-'+uuid[12:16]+'-'+uuid[16:20]+'-'+uuid[20:32]


def PathListId(path):
    global PathList
    
    if (not PathList.has_key(path)):
        PathList[path] = uuid()
    return PathList[path]
    

class Measurement:
	"Represent a Measurement"
	def __init__(self):
		self.attributes = {}	# Key: Attribute-Key (int) Value: Attribute-String 
		self.Values = []		# list of Value
		self.uuid = uuid()


	def WriteQML(self,file):
		file.write('          <Measurement guid="%s" ' %(self.uuid) )
		for k,v in self.attributes.iteritems():
			file.write('k%04d="%s" ' % (int(k),v))
		file.write('>\n')

		charID = 1
		for v in self.Values:
			v.WriteQML(file)
			charID += 1
		file.write('          </Measurement>\n')

class Value:
	"Represent a Value in a measurement"
	def __init__(self):
		self.attributes = {}	# Key: Attribute-Key (int) Value: Attribute-String 
		self.uuid = uuid()
		self.c_id = 0

	def WriteQML(self,file):
		file.write('            <V id="%s" c_id="%s" ' % (self.uuid,self.c_id) )
		for k,v in self.attributes.iteritems():
			file.write('k%04d="%s" ' % (int(k),v))
		file.write('/>\n')

class Characteristic:
	"Prepresents a Characteristic of an inspection plan"
	def __init__(self):
		self.index = 0			    # DFQ Characteristic Index
		self.charactaristics = {}	# Key: Characteristic-name  Value: part
		self.attributes = {}	    # Key: Attribute-Key    Value: Attribute-String
		self.uuid = uuid()

	def WriteQML(self,file):
		file.write('         <Characteristic guid="%s" ' % (self.uuid))
		for k,v in self.attributes.iteritems():
			file.write('k%04d="%s" ' % (int(k),v))
		file.write('>\n')
		for c in self.charactaristics.itervalues():
			c.WriteQML(file)
		file.write('         </Characteristic>\n')
	
class Part:
	"Represents a Part of an inspection plan"
	def __init__(self):
		self.attributes = {}		# Key: Attribute-Key        Value: Attribute-String 
		self.charactaristics = {}	# Key: Characteristic-name  Value: part
		self.parts = {}	            # Key: part-name            Value: part
		self.uuid = uuid()

	def WriteQML(self,file):
		file.write('      <Part guid="%s" ' % (self.uuid))
		for k,v in self.attributes.iteritems():
			file.write('k%04d="%s" ' % (int(k),v))
		file.write('>\n')
		for p in self.parts.itervalues():
			p.WriteQML(file)
		for c in self.charactaristics.itervalues():
			c.WriteQML(file)
		file.write('      </Part>\n')


RootPart = Part()
RootPart.attributes[1002] = "Root"

def PrintWarning(num,text):
	global ErrFileName,ErrFileLine,NumberOfWarnings,Woff
	if (Woff != 0):
		return

	sys.stderr.write('Warning (%d) in Line %d of File %s\n%s\n'%(num,ErrFileLine,ErrFileName,text))
	NumberOfWarnings += 1
	
def PrintError(num,text):
	global ErrFileName,ErrFileLine
	sys.stderr.write('Error (%d) in Line %d of File %s\n%s\n'%(num,ErrFileLine,ErrFileName,text))
	sys.exit(10)


def BuildInspectionPlan():
    global BasePath, RootPart, PathList
    
    for k, v in PathList.iteritems():
        Act = RootPart
        for sp in k[1:].split('/'):
            type,name = sp.split(':')
            if (type == 'Part'):
                if(not Act.parts.has_key(name)):
                    Act.parts[name] = Part()
                    Act.parts[name].attributes[1002] = name
                Act = Act.parts[name]
            elif (type == 'Characteristic'):
                if(not Act.charactaristics.has_key(name)):
                    Act.charactaristics[name] = Characteristic()
                    Act.charactaristics[name].attributes[2002] = name
                Act = Act.charactaristics[name]
            else:
                PrintError(0,"Wrong type specifier in Path: %s"%k)
        Act.uuid = v     
    
def ParseRobFit(file,FileName):
	global ErrFileName,ErrFileLine,BasePath
	
	ErrFileName = FileName
	ErrFileLine = 0

	Meas = 0
	
	# compile the regexes needed
	StartMeas = re.compile("^\w+\s(\w+)\s(\d+)\s(\d+):(\d+):(\d+)\s(\d+)\. INFO measurement finished for body (\d+)")
	Values    = re.compile("^\t(\w+)\sgap\s(-?\d+\.\d*) ± (-?\d+\.\d*) flush (-?\d+\.\d*) ± (-?\d+\.\d*)")
	Values2   = re.compile("^\t(\w+)\sx0 (-?\d+\.\d*) ± (-?\d+\.\d*) x1 (-?\d+\.\d*) ± (-?\d+\.\d*) y0 (-?\d+\.\d*) ± (-?\d+\.\d*) y1 (-?\d+\.\d*) ± (-?\d+\.\d*)")
	Values3   = re.compile("^\t(\w+)\s01 (-?\d+\.\d*) ± (-?\d+\.\d*) 02 (-?\d+\.\d*) ± (-?\d+\.\d*)")
	EmptyLine = re.compile("^\s+$")
		
	for line in file.readlines():
		ErrFileLine = ErrFileLine + 1
		NumOfChars = 1

		EmptyLineMatch = EmptyLine.match(line)
		if EmptyLineMatch:
			continue
		
		#matching new Measurement
		NewMeas = StartMeas.match(line);
		if NewMeas:
			Meas = Measurement()
			Meas.attributes[4] = "%s-%s-%sT%s:%s:%s.000"%(NewMeas.group(6),Months[NewMeas.group(1)],NewMeas.group(2),NewMeas.group(3),NewMeas.group(4),NewMeas.group(5))  # XSD DateTime format: 2006-07-19T13:35:03.000
			Meas.attributes[16] = NewMeas.group(7) # Production code
			Measurements.append(Meas)
			continue

		# match value line type one
		NewValue = Values.match(line);
		if (NewValue):
			Val1 = Value()
			Val1.c_id  = PathListId(BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_gap')
			Val2 = Value()
			Val2.c_id  = PathListId (BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_flush')
			Val1.attributes[1] = NewValue.group(2)
			Val2.attributes[1] = NewValue.group(4)
			Meas.Values.append(Val1)
			Meas.Values.append(Val2)
			continue

		# match value2 line type one
		NewValue = Values2.match(line);
		if (NewValue):
			Val1 = Value()
			Val1.c_id  = PathListId(BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_x0')
			Val2 = Value()
			Val2.c_id  = PathListId (BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_x1')
			Val3 = Value()
			Val3.c_id  = PathListId(BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_y0')
			Val4 = Value()
			Val4.c_id  = PathListId (BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_y1')
			Val1.attributes[1] = NewValue.group(2)
			Val2.attributes[1] = NewValue.group(4)
			Val3.attributes[1] = NewValue.group(6)
			Val4.attributes[1] = NewValue.group(8)
			Meas.Values.append(Val1)
			Meas.Values.append(Val2)
			Meas.Values.append(Val3)
			Meas.Values.append(Val4)
			continue

			# match value line type one
		NewValue = Values3.match(line);
		if (NewValue):
			Val1 = Value()
			Val1.c_id  = PathListId(BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_01')
			Val2 = Value()
			Val2.c_id  = PathListId (BasePath + '/Characteristic:' + NewValue.group(1) + '/Characteristic:' + NewValue.group(1) +'_02')
			Val1.attributes[1] = NewValue.group(2)
			Val2.attributes[1] = NewValue.group(4)
			Meas.Values.append(Val1)
			Meas.Values.append(Val2)
			continue

	# Build up the Inspection Plan from the gathered information
	BuildInspectionPlan()

		
def WriteQML(file):
	global Measurements,ProzessKatalogs
	file.write('<?xml version="1.0" encoding="utf-8"?>\n<QML xmlns="QML" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="QML QML-V_1_0_4.xsd" qmlSchemaLocation="QML-V_1_0_4.xsd" qmlVersion="1.0.4" k0100="0">\n')
	
	file.write("  <Parts>\n")
	
	for p in RootPart.parts.itervalues():
		p.WriteQML(file)

	file.write("   </Parts>\n")
	if (not Iplan):
		file.write("   <Values>\n")
		
		for m in Measurements:
			m.WriteQML(file)
		
		file.write("   </Values>\n")
	file.write("</QML>\n")


def WriteDFQ (file):
	file.write("K0100 %d\n" % len(_Part.charactaristics))	# Anzahl Merkmale
	if len(_Part.attributes) > 0:
		for k,v in _Part.attributes.iteritems():
			file.write("K%04d %s\n" % (int(k), v))
	else:
		file.write("K1002\n")	# Leeres Dummy Teil
		
	for i in _Part.charactaristics.iterkeys():
		c = _Part.charactaristics[i]
		for k,v in c.attributes.iteritems():
			file.write("K%04d/%d %s\n" % (k,i,v))
	for mea in Measurements:
		i1 = 0
		first = True
		for value in mea.Values:
			if value.attributes.has_key(1):
				file.write(value.attributes[1])
			if first & (len(mea.attributes) > 0):
				file.write("\x14")
			if first:
				first = False
				i2 = 0
				for k in [2, 4, 5, 6, 7, 8, 9, 12]:
					if mea.attributes.has_key(k):
						if k == 4:
							y,M,d,h,m,s = re.match("(\d+)-(\d+)-(\d+)T(\d+)\:(\d+):(\d+)", mea.attributes[k]).groups()
							file.write("%s.%s.%s/%s:%s:%s" % (d,M,y,h,m,s))
						else:
							file.write(mea.attributes[k])
						i2 += 1
					if i2 < len(mea.attributes):
						file.write("\x14")						
			i1 += 1
			if i1 < len(mea.Values):
				file.write("\x0f")
		file.write("\n")

def main():
	global ErrFileName,ErrFileLine,NumberOfWarnings,Woff,BasePath,Iplan
	Qout = None
	DFQout = None
	NoOut  = 0
	
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hwinq:d:p:", ["help", "warnoff", "iplan", "noout", "qmlout=", "dfqout=", "path="])
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
		if o in ("-i", "--iplan"):
			Iplan = 1
		if o in ("-q", "--qmlout"):
			Qout = a
		if o in ("-d", "--dfqout"):
			DFQout = a
		if o in ("-p", "--path"):
			BasePath = a
			if (BasePath[0] != '/'):
				PrintError(0,"Path has to start with / !\n")
		if o in ("-n", "--noout"):
			NoOut = 1

	# runing through the files
	try:
		if (len(args) ==0):
			ParseRobFit(sys.stdin,"stdin")
		else:
			for i in args:
				# open the file and parse line by line. Using ISO encoding
				file = codecs.open(i, "r", encoding="iso-8859-1")
				ParseRobFit(file,i)
	except:
		PrintError(0,"Unbekannter Fehler (Python exception): %s %s\n"%(sys.exc_info()[0],sys.exc_info()[1]))

	if(Qout):		 
		file = codecs.open(Qout, "w", encoding="utf-8")		
		WriteQML(file)
		file.close()
	elif (DFQout):
		file = codecs.open(DFQout, "w", encoding="utf-8")		
		WriteDFQ(file)
		file.close()
	elif (NoOut):
		sys.stderr.write("No output generated\n")
	else:
		WriteQML(sys.stdout)
		
	if(NumberOfWarnings > 0):
		sys.exit(2)
	else:
		sys.exit(0)

if __name__ == "__main__":
	main()
