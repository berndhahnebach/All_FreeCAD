import os,sys,string,math,shutil,glob
from time import sleep
from os.path import join

##GUI related stuff
from PyQt4 import QtCore, QtGui
from User_Interface_Mach_Dist import Ui_dialog
##------------------------------------------------

#from read_generalINFOS  import  read_generalINFOS
#from determine_ElementType  import  determine_ElementType
#from mesh_bdf2inp  import mesh_bdf2inp
from ApplyingBC_IC  import ApplyingBC_IC
from calculix_postprocess import calculix_postprocess

import FreeCAD,Fem,Part


class MyForm(QtGui.QDialog,Ui_dialog):
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.setupUi(self)
        #Define some global variables
        self.filename = QtCore.QString("")
        self.dirname = QtCore.QString("")
        #Connect Signals and Slots
        QtCore.QObject.connect(self.button_select_file, QtCore.SIGNAL("clicked()"), self.select_file)
        QtCore.QObject.connect(self.button_select_output, QtCore.SIGNAL("clicked()"), self.select_output)
        QtCore.QObject.connect(self.button_dialog, QtCore.SIGNAL("accepted()"), self.onAbbrechen)
        QtCore.QObject.connect(self.button_dialog, QtCore.SIGNAL("rejected()"), self.onAbbrechen)
        QtCore.QObject.connect(self.button_start_calculation, QtCore.SIGNAL("clicked()"), self.start_calculation)
        QtCore.QObject.connect(self.button_add_to_table, QtCore.SIGNAL("clicked()"), self.start_PostProcessing)
        

        #Define the table headers as we are not able to use the QT Designer for that
        self.JobTable.clear()
        self.JobTable.setHorizontalHeaderLabels(["Input File","Output Folder","Offset From","Offset To","Intervall","RotX","RotY","RotZ","Young Modulus","Poisson Ratio","LC1","LC2","LC3","LC4","LC5","LC6","LTC1","LTC2","LTC3","LTC4","LTC5","LTC6","Plate Thickness"])

    def add_to_table(self):
        self.JobTable.insertRow(0)
        item = QtGui.QTableWidgetItem(self.filename)
        self.JobTable.setItem(0,0,item)
        item = QtGui.QTableWidgetItem(self.dirname)
        self.JobTable.setItem(0,1,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_z_level_from.value())
        self.JobTable.setItem(0,2,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_z_level_to.value())
        self.JobTable.setItem(0,3,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.intervall.value())
        self.JobTable.setItem(0,4,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_misalignment_x.value())
        self.JobTable.setItem(0,5,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_misalignment_y.value())
        self.JobTable.setItem(0,6,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_misalignment_z.value())
        self.JobTable.setItem(0,7,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_young_modulus.value())
        self.JobTable.setItem(0,8,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_poisson_ratio.value())
        self.JobTable.setItem(0,9,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.lc1.value())
        self.JobTable.setItem(0,10,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.lc2.value())
        self.JobTable.setItem(0,11,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.lc3.value())
        self.JobTable.setItem(0,12,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.lc4.value())
        self.JobTable.setItem(0,13,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.lc5.value())
        self.JobTable.setItem(0,14,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.lc6.value())
        self.JobTable.setItem(0,15,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.ltc1.value())
        self.JobTable.setItem(0,16,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.ltc2.value())
        self.JobTable.setItem(0,17,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.ltc3.value())
        self.JobTable.setItem(0,18,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.ltc4.value())
        self.JobTable.setItem(0,19,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.ltc5.value())
        self.JobTable.setItem(0,20,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.ltc6.value())
        self.JobTable.setItem(0,21,item)
        item = QtGui.QTableWidgetItem()
        item.setData(QtCore.Qt.DisplayRole,self.spinBox_Plate_Thickness.value())
        self.JobTable.setItem(0,22,item)
        # This is how to get the data back test2 = self.JobTable.item(0,1).data(QtCore.Qt.DisplayRole).toInt()
        self.button_add_to_table.setEnabled(False)
        self.filename.clear()
        self.button_start_calculation.setEnabled(True)

    def select_file(self):
        self.filename=QtGui.QFileDialog.getOpenFileName(None, 'Open file','','Nastran Files (*.bdf)')
        self.button_add_to_table.setEnabled(not self.dirname.isEmpty() and not self.filename.isEmpty())

    def select_output(self):
        self.dirname=QtGui.QFileDialog.getExistingDirectory(None, 'Open working directory', '', QtGui.QFileDialog.ShowDirsOnly)
        self.button_select_output.setEnabled(False)
        self.button_add_to_table.setEnabled(not self.dirname.isEmpty() and not self.filename.isEmpty())

    def onAbbrechen(self):
        self.close()

    def start_PostProcessing(self):
        outputfile = open(str(self.dirname + "/testoutput.txt"),"wb")
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
				file.write(
                str(current_z_level) + " " +
                str(relationship) + " " +
                str(max_disp_x) + " " +
                str(min_disp_x) + " " +
                str(max_disp_y) + " " +
                str(min_disp_y) + " " +
                str(max_disp_z) + " " +
                str(min_disp_z) + "\n")
                
        outputfile.close()
               
        
    def start_calculation(self):
        self.button_add_to_table.setEnabled(False)
        self.button_select_file.setEnabled(False)
        self.button_select_output.setEnabled(False)
        self.button_start_calculation.setEnabled(False)
        ##Get values from the GUI
        if ( os.path.exists(str(self.dirname)) ):
            shutil.rmtree(str(self.dirname))
        
        os.mkdir(str(self.dirname))
        batch = open(str(self.dirname + "/" + 'lcmt_CALCULIX_Calculation_batch.bat'),'wb')

        #Now do the calculation stuff for each row in the  table
        for job in range (0,self.JobTable.rowCount()):
            self.fly_to_buy = self.check_fly_to_buy.isChecked()

            current_file_name = self.JobTable.item(job,0).text()
            filename_without_suffix = self.JobTable.item(job,0).text().split("/").takeLast().split(".")[0]
            meshobject = Fem.read(str(current_file_name))
            #Perform PCA
            Fem.SMESH_PCA(meshobject)
            #Do min routine
            Fem.minBoundingBox(meshobject)
            #Now get the Node Numbers for the Boundary Conditions
            node_numbers = []
            node_numbers = Fem.getBoundary_Conditions(meshobject)
            #Now perform the user misalignment
            rotation_around_x = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,0),FreeCAD.Base.Vector(1,0,0),self.JobTable.item(job,5).data(QtCore.Qt.DisplayRole).toDouble()[0])
            rotation_around_y = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,0),FreeCAD.Base.Vector(0,1,0),self.JobTable.item(job,6).data(QtCore.Qt.DisplayRole).toDouble()[0])
            rotation_around_z = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,0),FreeCAD.Base.Vector(0,0,1),self.JobTable.item(job,7).data(QtCore.Qt.DisplayRole).toDouble()[0])
            meshobject.setTransform(rotation_around_x)
            meshobject.setTransform(rotation_around_y)
            meshobject.setTransform(rotation_around_z)
            #Now we have set up the initial geometry for the calculations. Lets generate an ABAQUS input file now for each z-level with exactly the same
            #boundary conditions


            #1. Lets translate the geometry to the initial desired z-level

            #2. Generate a Folder for the current calculation z-level and output the ABAQUS Geometry and the boundary_conditions



            intervall = self.JobTable.item(job,4).data(QtCore.Qt.DisplayRole).toDouble()[0]
            i = self.JobTable.item(job,2).data(QtCore.Qt.DisplayRole).toInt()[0]
            while i <= self.JobTable.item(job,3).data(QtCore.Qt.DisplayRole).toInt()[0]:
                translated_mesh = meshobject.copy()
                translation = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,i),FreeCAD.Base.Vector(0,0,0),0)
                translated_mesh.setTransform(translation)
                Case_Dir = self.JobTable.item(job,1).text() + "/" + filename_without_suffix + "_" + QtCore.QString.number(i)
                if ( os.path.exists(str(Case_Dir)) ):
                  shutil.rmtree(str(Case_Dir))
                os.mkdir(str(Case_Dir))
                #Lets generate a sigini Input Deck for the calculix user subroutine
                sigini_input = open (str(Case_Dir + "/" + "sigini_input.txt"),'wb')

                
                #Write plate thickness to the sigini_file
                sigini_input.write(str(self.JobTable.item(job,22).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "\n")
                #Now write the Interpolation coefficients, first the L and then the LC ones
                sigini_input.write(\
                str(self.JobTable.item(job,10).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,11).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,12).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,13).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,14).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,15).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "\n")
                sigini_input.write(\
                str(self.JobTable.item(job,16).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,17).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,18).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,19).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,20).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "," + \
                str(self.JobTable.item(job,21).data(QtCore.Qt.DisplayRole).toDouble()[0]) + "\n")
                sigini_input.close()
                translated_mesh.write(str(Case_Dir + "/" + "geometry_fe_input.inp"))
                ApplyingBC_IC(Case_Dir, self.JobTable.item(job,8).data(QtCore.Qt.DisplayRole).toDouble()[0],self.JobTable.item(job,9).data(QtCore.Qt.DisplayRole).toDouble()[0],node_numbers[0],node_numbers[1],node_numbers[2])
                batch.write(str("cd " + filename_without_suffix + "_" + QtCore.QString.number(i) + "\n"))
                batch.write(str("/home/st_fr/to81026/CCX_2_2_COMPILATION/CalculiX/ccx_2.2/src/ccx_2.2_MT -i final_fe_input > calculix_output.out\n"))
                batch.write(str("cd ..\n"))
                i = i+intervall


        batch.write("exit")
        batch.close()


        self.button_select_file.setEnabled(True)
        self.button_select_output.setEnabled(True)


##Main function to be used for GUI setup and entering into the main loop
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    myapp = MyForm()
    myapp.show()
    sys.exit(app.exec_())









