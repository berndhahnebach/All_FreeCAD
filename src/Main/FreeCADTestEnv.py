# FreeCAD init module  
# (c) 2001 Jürgen Riegel
#
# Gathering all the information to start FreeCAD
# This is one of two init scripts, the second one
# runs when the gui is up

# using FreeCAD output (not print)


Log("FreeCAD EnvTest running...\n\n")

try:

	# Testing env variables ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Log("Checking on Environment...\n")


	envs = ['CSF_MDTVFONTDIRECTORY','CSF_MDTVTEXTURESDIRECTORY','CSF_PLUGINDEFAULTS','CSF_SHMESSAGESTD','CSF_STANDARDDEFAULTS','CSF_UNITSDEFINITION','CSF_UNITSLEXICON','CSF_XSMESSAGESTD']

	try:
		for i in envs :
			Log ( '   ' + i + ' = ' + os.getenv(i) + '\n')
	except :
		Err('One ore more OpenCasCade environment Variables are not set, please reinstall Open CasCade!\n')
		raise

	try:
		path = os.getenv("CSF_PluginDefaults")+'\\Plugin'
		f = open(path,"r")
	except :
		Err('Cant find a valid \"Plugin\" file, please reinstall Open CasCade!\n')
		raise

	try:
		path = os.getenv("CSF_PluginDefaults")+'\\Standard'
		f = open(path,"r")
	except :
		Err('Cant find a valid \"Standard\" file, please reinstall Open CasCade!\n')
		raise 

	try:
		line = f.readline()
		if(line.find('FreeCad-Part')==-1):
			raise
	except :
		Err("Seams to be the wrong \"Standard\" fiel, set the CSF_STANDARDDEFAULTS to\n")
		Err("FRECADDIR/Resources or reinstall FreeCAD\n")
		raise

except :
	Err("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n")
	Err("At least one test fails, bailing out!\n")
	Err("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n")
else:
	Msg("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n")
	Msg("FreeCAD environment test was completly succsesfull! FreeCAD shut execute \n")
	Msg("with no problems. Have fun!\n")
	Msg("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n")



Log("FreeCAD EnvTest done\n")


