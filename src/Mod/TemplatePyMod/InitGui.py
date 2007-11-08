# TemplatePyMod gui init module  
# (c) 2007 Juergen Riegel LGPL
#


class TemplatePyModWorkbench ( Workbench ):
	"Test workbench object"
	def Activate(self):
		Log ('Loading GUI of TemplatePyMod module...\n')
		import Commands

		self.appendToolbar("TemplateTools",["TemplatePyMod_Cmd1","TemplatePyMod_Cmd2"])

		menu = ["ModulePy &Commands","PyModuleCommands"]
		list = ["TemplatePyMod_Cmd1","TemplatePyMod_Cmd2"]
		self.appendCommandbar("PyModuleCommands",list)
		self.appendMenu(menu,list)

		Log ('Loading GUI of Test module... done\n')

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
