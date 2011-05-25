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
                import draftTools,draftGui,Arch_rc,Wall,Cell,Floor,Building,Site,Structure
                archtools = ["Arch_Wall","Arch_Structure","Arch_Cell",
                             "Arch_Floor","Arch_Building","Arch_Site"]
                drafttools = ["Draft_Line","Draft_Wire","Draft_Rectangle",
                              "Draft_Polygon","Draft_Arc",
                              "Draft_Circle","Draft_Dimension",
                              "Draft_Move","Draft_Rotate",
                              "Draft_Offset","Draft_Upgrade",
                              "Draft_Downgrade"]
                self.appendToolbar("Arch tools",archtools)
                self.appendToolbar("Draft tools",drafttools)
                self.appendMenu("Architecture",archtools)
                self.appendMenu("Drafting",drafttools)
		Log ('Loading Arch module... done\n')
	def Activated(self):
		Msg("Arch workbench activated\n")
	def Deactivated(self):
		Msg("Arch workbench deactivated\n")

Gui.addIconPath(":/icons")
Gui.addLanguagePath(":/translations")
Gui.addWorkbench(ArchWorkbench)
App.addImportType("Industry Foundation Classes (*.ifc)","importIFC")
try:
        import collada
except:
        Log("pycollada not found, no collada support\n")
else:
        App.addImportType("Collada (*.dae)","importDAE")

