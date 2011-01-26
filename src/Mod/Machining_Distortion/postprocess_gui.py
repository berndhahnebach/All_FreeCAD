import os,sys,string,math,shutil,glob
from time import sleep
from os.path import join

##GUI related stuff
from PyQt4 import QtCore, QtGui
from postprocess import Ui_dialog
##------------------------------------------------


from calculix_postprocess import calculix_postprocess

import FreeCAD,Fem,Part


class MyForm(QtGui.QDialog,Ui_dialog):
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.setupUi(self)
        #Define some global variables
        self.dirname = QtCore.QString("")
        #Connect Signals and Slots
        QtCore.QObject.connect(self.button_select_results_folder, QtCore.SIGNAL("clicked()"), self.select_output)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), self.onAbbrechen)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), self.onAbbrechen)
        QtCore.QObject.connect(self.button_start_postprocessing, QtCore.SIGNAL("clicked()"), self.start_PostProcessing)    
        

    def select_output(self):
        self.dirname=QtGui.QFileDialog.getExistingDirectory(None, 'Open working directory', '', QtGui.QFileDialog.ShowDirsOnly)
        self.button_start_postprocessing.setEnabled(True)

    def onAbbrechen(self):
        self.close()

    def start_PostProcessing(self):
        outputfile = open(str(self.dirname + "/postprocessing_input.txt"),"wb")
        sort_tuple = (1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)
        output_list = []
        for root, dirs, files in os.walk(str(self.dirname)):
           if 'final_fe_input.frd' in files:
                bbox_orig,\
                bbox_distorted,\
                relationship,\
                max_disp_x,\
                min_disp_x,\
                max_disp_y,\
                min_disp_y,\
                max_disp_z,\
                min_disp_z = calculix_postprocess(os.path.join(root,'final_fe_input.frd'))
                
                #Get the current Offset-Level from the folder-name
                current_z_level = int(root[(root.rfind("_")+1):])
                #now generate an output tuple and add it to the output-list
                output_list.append((current_z_level,relationship,max_disp_x,min_disp_x,max_disp_y,min_disp_y,max_disp_z,min_disp_z))

        sorted_list = sorted(output_list, key=lambda sort_tuple: sort_tuple[0])
        for item in sorted_list:
            outputfile.write(
                str(item[0]) + " " +
                str(item[1]) + " " +
                str(item[2]) + " " +
                str(item[3]) + " " +
                str(item[4]) + " " +
                str(item[5]) + " " +
                str(item[6]) + " " +
                str(item[7]) + "\n")
                
        outputfile.close()
        #Now create a batch file for GnuPlot and start the generation of the postprocess image generation
        self.start_gnu_plot()
        
        
    def start_gnu_plot(self):
        gnu_plot_input_file = open(str(self.dirname + "/gnu_plot_input.txt"),"wb")
        gnu_plot_input_file.write(
                                    "set term png\n" +
                                    "set output \"max_disp_x.png\"\n" +
                                    "plot \"postprocessing_input.txt\" using 1:3 with lines\n" + 
                                    "set output \"min_disp_x.png\"\n" +
                                    "plot \"postprocessing_input.txt\" using 1:4 with lines\n" + 
                                    "set output \"max_disp_y.png\"\n" +
                                    "plot \"postprocessing_input.txt\" using 1:5 with lines\n" + 
                                    "set output \"min_disp_y.png\"\n" +
                                    "plot \"postprocessing_input.txt\" using 1:6 with lines\n" + 
                                    "set output \"max_disp_z.png\"\n" +
                                    "plot \"postprocessing_input.txt\" using 1:7 with lines\n" + 
                                    "set output \"min_disp_z.png\"\n" +
                                    "plot \"postprocessing_input.txt\" using 1:8 with lines\n" + 
                                    "exit" )
                                    
        gnu_plot_input_file.close()
        os.chdir(str(self.dirname))
        command = "gnuplot gnu_plot_input.txt"
        os.system(command)



