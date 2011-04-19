class ArchWorkbench(Workbench):
	"Arch workbench object"
	Icon = """
                /* XPM */
                static char * arch_xpm[] = {
                "16 16 9 1",
                " 	c None",
                ".	c #543016",
                "+	c #6D2F08",
                "@	c #954109",
                "#	c #874C24",
                "$	c #AE6331",
                "%	c #C86423",
                "&	c #FD7C26",
                "*	c #F5924F",
                "                ",
                "                ",
                "       #        ",
                "      ***$#     ",
                "    .*******.   ",
                "   *##$****#+   ",
                " #**%&&##$#@@   ",
                ".$**%&&&&+@@+   ",
                "@&@#$$%&&@@+..  ",
                "@&&&%#.#$#+..#$.",
                " %&&&&+%#.$**$@+",
                "   @%&+&&&$##@@+",
                "     @.&&&&&@@@ ",
                "        @%&&@@  ",
                "           @+   ",
                "                "};
			"""
	MenuText = "Arch"
	ToolTip = "Architecture workbench"
	
	def Initialize(self):
                import Arch_rc,Wall,Cell,draftTools,draftGui
                Gui.addCommand('Arch_Wall',Wall.CommandWall())
                Gui.addCommand('Arch_Cell',Cell.CommandCell())
                tools = ["Arch_Wall","Arch_Cell"]
                drafttools = ["Draft_Line","Draft_Wire","Draft_Arc",
                              "Draft_Circle","Draft_Dimension",
                              "Draft_Move","Draft_Rotate",
                              "Draft_Offset","Draft_Upgrade",
                              "Draft_Downgrade"]
                self.appendToolbar("Arch tools",tools)
                self.appendToolbar("Draft tools",drafttools)
                self.appendMenu("Architecture",tools)
                self.appendMenu("Drafting",drafttools)
		Log ('Loading Arch module... done\n')
	def Activated(self):
		Msg("ArchWorkbench::Activated()\n")
	def Deactivated(self):
		Msg("ArchWorkbench::Deactivated()\n")

Gui.addIconPath(":/icons")
Gui.addLanguagePath(":/translations")
Gui.addWorkbench(ArchWorkbench)
App.addImportType("Industry Foundation Classes (*.ifc)","importIFC")
try:
        import collada
except:
        print "pycollada not found, no collada support"
else:
        App.addImportType("Collada (*.dae)","importDAE")

