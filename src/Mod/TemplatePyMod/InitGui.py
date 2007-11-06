# TemplatePyMod gui init module  
# (c) 2007 Juergen Riegel LGPL
#


class TemplatePyModWorkbench ( Workbench ):
	"Test workbench object"
	def __init__(self):
		self.WorkbenchName = "TemplatePyMod"

	def Activate(self):
		try:
			Log ('Loading GUI of TemplatePyMod module...\n')
			import Commands
			w=0
			if Gui.hasWorkbench(self.WorkbenchName) == False:
				w = Gui.createWorkbench(self.WorkbenchName)

				w.appendToolbar("TemplateTools",["TemplatePyMod_Cmd1","TemplatePyMod_Cmd2"])

				menu = ["ModulePy &Commands","PyModuleCommands"]
				list = ["TemplatePyMod_Cmd1","TemplatePyMod_Cmd2"]
				w.appendCommandbar("PyModuleCommands",list)
				w.appendMenu(menu,list)

				Log ('Setup menues...\n')
			Log ('Loading GUI of Test module... done\n')
		except:
			Err('Activating of workbench "TemplatePyMod framework" failed\n')
			raise
	def GetClassName(self):
		return "Gui::PythonWorkbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *test_icon[]={\n"
			"\"16 16 2 1\",\n"
			"\"a c #000000\",\n"
			"\". c None\",\n"
			"\"................\",\n"
			"\"................\",\n"
			"\"..############..\",\n"
			"\"..############..\",\n"
			"\"..############..\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"................\",\n"
			"\"................\"};\n"]

Gui.addWorkbenchHandler("TemplatePyMod Workbench",TemplatePyModWorkbench())
