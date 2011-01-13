# Machining Distortion Commands


# import FreeCAD modules
import FreeCAD, FreeCADGui, Fem, sys
from PyQt4 import QtGui,QtCore
from FreeCAD import Base

# globals
from mach_dist_gui import MyForm
import time


#---------------------------------------------------------------------------
# The command classes
#---------------------------------------------------------------------------

class MachiningDistortion_ChooseFile:
    "Choose the file to work with"
    def Activated(self):
#        pi=Base.ProgressIndicator()
#        pi.start("Hello, World!",100)
#        for i in range(100):
#            time.sleep(1)
#            pi.next()
#        pi.stop()
        myapp = MyForm(QtGui.qApp.activeWindow())
        myapp.exec_()

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1', 'MenuText': 'Open FEM file...', 'ToolTip': 'Open the file to work with'}

class MachiningDistortion_ChooseDir:
    "Choose the file to work with"
    def Activated(self):
        from PyQt4 import QtGui
        dirname=QtGui.QFileDialog.getExistingDirectory(None, 'Open working directory', '', QtGui.QFileDialog.ShowDirsOnly)

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool2', 'MenuText': 'Open work dir...', 'ToolTip': 'Choose the working directory'}


class MachiningDistortion_ChooseParameter:
    "Start Main GUI"
    def Activated(self):
        from PyQt4 import QtGui
        import PyQt4.uic as uic
        form_class, base_class = uic.loadUiType(FreeCAD.getHomePath() + "Mod/MachiningDistortion/User_Interface_Mach_Dist.ui")
        # instaciate the UI dialog
        form = form_class()
        # create the actuall dialog
        dlg = QtGui.QDialog()
        # set the Ui to the dialog
        form.setupUi(dlg)
        #run the dialog
        rtn = dlg.exec_()
        # get the values of the Form
        
        #Connect Signals and Slots
        QtCore.QObject.connect(form.button_select_files, QtCore.SIGNAL("clicked()"), self.select_files)
        QtCore.QObject.connect(form.button_select_output, QtCore.SIGNAL("clicked()"), self.select_output)
        QtCore.QObject.connect(form.button_dialog, QtCore.SIGNAL("accepted()"), self.onAbbrechen)
        QtCore.QObject.connect(form.button_dialog, QtCore.SIGNAL("rejected()"), self.onAbbrechen)
        QtCore.QObject.connect(form.button_start_calculation, QtCore.SIGNAL("clicked()"), self.start_calculation)


    def select_files(self):
        filenames=QtGui.QFileDialog.getOpenFileNames(None, 'Open file','','Nastran Files (*.bdf)')

    def select_output(self):
        self.dirname=QtGui.QFileDialog.getExistingDirectory(None, 'Open working directory', '', QtGui.QFileDialog.ShowDirsOnly)
        self.button_start_calculation.setEnabled(True)

    def onAbbrechen(self):
        form.close()

    def start_calculation(self):
        ##Get values from the GUI
        young_modulus = form.spinBox_young_modulus.value()
        poisson_ratio = form.spinBox_poisson_ratio.value()
        z_level_from = form.spinBox_z_level_from.value()
        z_level_to = form.spinBox_z_level_to.value()
        misalignment_x = form.spinBox_misalignment_x.value()
        misalignment_y = form.spinBox_misalignment_y.value()
        misalignment_z = form.spinBox_misalignment_z.value()
        fly_to_buy = form.check_fly_to_buy.isChecked()


        FreeCAD.Console.PrintMessage("Start with:"+ `misalignment_x` + "\n" )
        
        
    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool3', 'MenuText': 'parameters...', 'ToolTip': 'Choose the parameters'}



#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------

FreeCADGui.addCommand('MachiningDistortion_ChooseFile',MachiningDistortion_ChooseFile())
FreeCADGui.addCommand('MachiningDistortion_ChooseDir',MachiningDistortion_ChooseDir())
FreeCADGui.addCommand('MachiningDistortion_ChooseParameter',MachiningDistortion_ChooseParameter())

