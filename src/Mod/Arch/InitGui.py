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
                self.appendToolbar("Arch tools",tools)
                self.appendToolbar("Draft tools",["Draft_Line","Draft_Wire","Draft_Arc"])
                self.appendMenu("Architecture",tools)
		Log ('Loading Arch module... done\n')
	def Activated(self):
		Msg("ArchWorkbench::Activated()\n")
	def Deactivated(self):
		Msg("ArchWorkbench::Deactivated()\n")

Gui.addIconPath(":/icons")
Gui.addWorkbench(ArchWorkbench)
App.addImportType("Industry Foundation Classes (*.ifc)","importIFC") 

