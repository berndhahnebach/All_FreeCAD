# MachiningDistortion Gui stuff


class MachiningDistortionWorkbench ( Workbench ):
    "Test workbench object"
    Icon = """
        /* XPM */
        static const char *test_icon[]={
        "16 16 2 1",
        "a c #000000",
        ". c None",
        "................",
        "................",
        "..####....####..",
        "..####....####..",
        "..####....####..",
        "................",
        "......####......",
        "......####......",
        "......####......",
        "......####......",
        "......####......",
        "..####....####..",
        "..####....####..",
        "..####....####..",
        "................",
        "................"};
        """
    MenuText = "Machining Distortion"
    ToolTip = "MachiningDistortion workbench"
    
    def Initialize(self):
        import MachiningDistortionCommands
        CmdList = ["MachiningDistortion_ChooseFile","MachiningDistortion_ChooseDir","MachiningDistortion_ChooseParameter"]
        self.appendToolbar("MachiningDistortionTools",CmdList)
        self.appendMenu("Machining Distortion",CmdList)

        Log ('Loading MachiningDistortion module... done\n')
    def Activated(self):
        Msg("MachiningDistortionWorkbench::Activated()\n")
    def Deactivated(self):
        Msg("MachiningDistortionWorkbench::Deactivated()\n")

Gui.addWorkbench(MachiningDistortionWorkbench)
