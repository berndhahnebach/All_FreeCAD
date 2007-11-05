# TemplatePyMod gui init module  
# (c) 2007 Juergen Riegel LGPL
#


class TemplatePyModWorkbench ( Workbench ):
	"Test workbench object"
	def Activate(self):
		try:
			Log ('Loading GUI of TemplatePyMod module...\n')
			import TestGui
			w=0
			if Gui.hasWorkbench('Test framework') == False:
				w = Gui.createWorkbench('Test framework')

				w.appendToolbar("TestTools",["TemplatePyMod_Cmd1"])

				menu = ["ModulePy &Commands","PyModuleCommands"]
				list = ["TemplatePyMod_Cmd1"]
				w.appendCommandbar("PyModuleCommands",list)
				w.appendMenu(menu,list)

				try:
					import Mesh, MeshGui
					list = ["Mesh_ExMakeMesh", "Mesh_ExMakeTool", "Mesh_ExMakeUnion"]
					w.appendMenu("Mesh",list)
				except:
					Err('Cannot load Mesh module\n')

				Log ('Setup Test menues...\n')
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
