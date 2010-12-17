import os,sys,string,math,shutil,glob
from time import sleep

##GUI related stuff
from PyQt4 import QtCore, QtGui
from User_Interface_Mach_Dist import Ui_dialog
##------------------------------------------------

from read_generalINFOS  import  read_generalINFOS
from determine_ElementType  import  determine_ElementType
from mesh_bdf2inp  import mesh_bdf2inp
from ApplyingBC_IC  import ApplyingBC_IC
from calculix_output_postprocess  import calculix_output_postprocess

import FreeCAD,Fem,Part


class MyForm(QtGui.QDialog,Ui_dialog):
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self)
        self.setupUi(self)

        #Connect Signals and Slots
        QtCore.QObject.connect(self.button_select_files, QtCore.SIGNAL("clicked()"), self.select_files)
        QtCore.QObject.connect(self.button_select_output, QtCore.SIGNAL("clicked()"), self.select_output)
        QtCore.QObject.connect(self.button_dialog, QtCore.SIGNAL("accepted()"), self.onAbbrechen)
        QtCore.QObject.connect(self.button_dialog, QtCore.SIGNAL("rejected()"), self.onAbbrechen)
        QtCore.QObject.connect(self.button_start_calculation, QtCore.SIGNAL("clicked()"), self.start_calculation)


    def select_files(self):
        self.filenames=QtGui.QFileDialog.getOpenFileNames(None, 'Open file','','Nastran Files (*.bdf)')

    def select_output(self):
        self.dirname=QtGui.QFileDialog.getExistingDirectory(None, 'Open working directory', '', QtGui.QFileDialog.ShowDirsOnly)
        self.button_start_calculation.setEnabled(not self.dirname.isEmpty())

    def onAbbrechen(self):
        self.close()

    def start_calculation(self):
        ##Get values from the GUI
        self.young_modulus = self.spinBox_young_modulus.value()
        self.poisson_ratio = self.spinBox_poisson_ratio.value()
        self.z_level_from = self.spinBox_z_level_from.value()
        self.z_level_to = self.spinBox_z_level_to.value()
        self.misalignment_x = self.spinBox_misalignment_x.value()
        self.misalignment_y = self.spinBox_misalignment_y.value()
        self.misalignment_z = self.spinBox_misalignment_z.value()
        self.fly_to_buy = self.check_fly_to_buy.isChecked()

        current_file_name = self.filenames[0]
        filename_without_suffix = self.filenames[0].split("/").takeLast().split(".")[0]
        current_z_level = 0
        # 0) Create a directory to dispose the resulting files for the case under consideration :
        Case_Dir = self.dirname + filename_without_suffix + "_" + QtCore.QString.number(current_z_level)
        if ( os.path.exists(str(Case_Dir)) ):
          os.rmdir(str(Case_Dir))
        os.mkdir(str(Case_Dir))

        meshobject = Fem.read(str(current_file_name))
        #Perform PCA 
        Fem.SMESH_PCA(meshobject)
        #Do min routine
        Fem.minBoundingBox(meshobject)
        #Now get the Node Numbers for the Boundary Conditions
        node_numbers = []
        node_numbers = Fem.getBoundary_Conditions(meshobject)
        #Now perform the user misalignment
        rotation_around_x = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,0),FreeCAD.Base.Vector(1,0,0),self.misalignment_x)
        rotation_around_y = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,0),FreeCAD.Base.Vector(0,1,0),self.misalignment_y)
        rotation_around_z = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,0),FreeCAD.Base.Vector(0,0,1),self.misalignment_z)
        meshobject.setTransform(rotation_around_x)
        meshobject.setTransform(rotation_around_y)
        meshobject.setTransform(rotation_around_z)

        #Now we have set up the initial geometry for the calculations. Lets generate an ABAQUS input file now for each z-level with exactly the same
        #boundary conditions
        #1. Lets translate the geometry to the initial desired z-level

        #2. Generate a Folder for the current calculation z-level and output the ABAQUS Geometry and the boundary_conditions
        i = self.z_level_from
        while i <= self_z_level_to:
            translation = FreeCAD.Base.Placement(FreeCAD.Base.Vector(0,0,self.z_level_from),FreeCAD.Base.Vector(1,0,0),0)
            meshobject.setTransform(translation)
            Case_Dir = self.dirname + filename_without_suffix + "_" + QtCore.QString.number(i)
            if ( os.path.exists(str(Case_Dir)) ):
              os.rmdir(str(Case_Dir))
            os.mkdir(str(Case_Dir))
            meshobject.write(str(Case_Dir + "/" + filename_without_suffix + "_" + QtCore.QString.number(i) + ".inp"))
            ApplyingBC_IC(YoungModulus,PoissonCoeff,node_numbers[0],node_numbers[1],node_numbers[2])
            i = i+intervall
        
        #3. Generate a Batch File and begin the calculation of the different steps. Use a progress bar to make the progress visible
        ##        # Edit a batch file (it may be necessary later on) :
        batch = open ('lcmt_CALCULIX_Calculation_batch.bat','w')
        print "writing job execution file lcmt_CALCULIX_Calculation_batch.bat"
        print "Provide a file MYcase1_sigini_input.txt if necessary !"
        print "Provide a file MYcase1.NormalInfos_txt if necessary !"
        print "Provide a file MYcase1.ThicknessInfos_txt if necessary !"
        print "Provide a file MYcase1.StressInfos_txt if necessary !"
        #
        CALCULIX_Input_Output = "-i TransformedMesh_File > calculix_output.out"
        batch.write("%s %s \n" %(Path_to_FEMsoftware, CALCULIX_Input_Output))
        #
        batch.write("exit")
        batch.close()
        # Direct submission of the CALCULIX_Calculations :
        CommandLine = "./lcmt_CALCULIX_Calculation_batch.bat"
        os.system("chmod +x ./lcmt_CALCULIX_Calculation_batch.bat")
        print "Launching job; look in calculix_output.out if needed"
        os.system(CommandLine)
		
		#4. After all the calculations are done we do a postprocess step to extract the displacement values at certain nodes
        
        #fly to buy ratio wird, falls angeklickt im Outputfile nach dem Misalignment vermerkt

        

##        # 1) copy the input PrincipalAxis-file (produced with the macro CATIA) into the generic file *CATPART_pointG_PrincipalAxis.txt* :
##        fileName1=str(FOLDER1_path)+"/"+str(file)
##        fileName2="  pointG_PrincipalAxis.txt"
##        CommandLine = "cp "+str(fileName1)+str(fileName2)
##        print "CommandLine=",CommandLine
##        os.system(CommandLine)
##
##        # 2) find and copy the corresponding input (Nastran) original-Mesh-file into the generic file *Nastran_originalMeshFile.bdf* :
##        fileName1 = str(FOLDER2_path)+"/"+str(filename_withoutSuffixe)+".bdf"
##        if ( os.path.exists(fileName1) ) :
##            fileName2=" Nastran_originalMeshFile.bdf"
##            CommandLine = "cp "+str(fileName1)+str(fileName2)
##            print "CommandLine=",CommandLine
##            os.system(CommandLine)
##        else :
##            print "the corresponding input (Nastran) Mesh-file is missing ! "
##            print "Check that matter. Exit forced from module *CALL_module.py* !!"
##            sys.exit()
##        #
##        # Info = the GRID lines (of the billet) are expressed in a global coordinate system (for example attached to the aircraft reference frame) !!
##
##        # 3) find and copy (if any) the corresponding OtherInput files with a generic_name :
##        fileName1 = str(FOLDER3_path)+"/"+str(filename_withoutSuffixe)+"_sigini_input.txt"
##        if ( os.path.exists(fileName1) ) :
##            fileName2=" sigini_input.txt"
##            CommandLine = "cp "+str(fileName1)+str(fileName2)
##            print "CommandLine=",CommandLine
##            os.system(CommandLine)
##            print ""
##            print ">>> Information : Statements of branch_3 will be executed in *sigini* !"
##            print ""
##        else :
##        # .............................................................................
##        print "NO associated OtherInput.txt (usefull to GO in branch_3 in *sigini*) ! "
##        #
##        fileName1 = str(FOLDER3_path)+"/"+str(filename_withoutSuffixe)+".NormalInfos_txt"
##        if ( os.path.exists(fileName1) ) :
##          fileName2=" Generic_CaseName.n"
##          CommandLine = "cp "+str(fileName1)+str(fileName2)
##          print "CommandLine=",CommandLine
##          os.system(CommandLine)
##          print ""
##          print ">>> Information : Statements of branch_2 will be executed in *sigini* !"
##          print ""
##        else :
##          print "NO associated Generic_CaseName.n (usefull to GO in branch_2 in *sigini*) ! "
##        #
##        fileName1 = str(FOLDER3_path)+"/"+str(filename_withoutSuffixe)+".StressInfos_txt"
##        if ( os.path.exists(fileName1) ) :
##          fileName2=" Generic_CaseName.s"
##          CommandLine = "cp "+str(fileName1)+str(fileName2)
##          print "CommandLine=",CommandLine
##          os.system(CommandLine)
##          #  Information : Statements of branch_2 will be executed in *sigini* !
##        else :
##          print "NO associated Generic_CaseName.s (usefull to GO in branch_2 in *sigini*) ! "
##        #
##        fileName1 = str(FOLDER3_path)+"/"+str(filename_withoutSuffixe)+".ThicknessInfos_txt"
##        if ( os.path.exists(fileName1) ) :
##          fileName2=" Generic_CaseName.t"
##          CommandLine = "cp "+str(fileName1)+str(fileName2)
##          print "CommandLine=",CommandLine
##          os.system(CommandLine)
##          #  Information : Statements of branch_2 will be executed in *sigini* !
##        else :
##          print "NO associated Generic_CaseName.t (usefull to GO in branch_2 in *sigini*) ! "
##        # ...................................................................................
##        #    Information : IF none of them have been found, THEN
##        #    Information : default statements of branch_1 will be executed in *sigini* !
##
##        # 4) Determine the type of Finite Element :
##        CommandLine = "tail -6 Nastran_originalMeshFile.bdf > tail_Nastran_originalMeshFile.bdf"
##        os.system(CommandLine)
##        (ElementType,NODE_IDs_ELEMENTspecif) = determine_ElementType()
##        print "ElementType=",ElementType,"   NODE_IDs_ELEMENTspecif=",NODE_IDs_ELEMENTspecif
##
##        # 5) Convert the input (Nastran) Mesh-file directly into an abaqus Mesh-file :
##        #    NB  possible misalignement of the billet IS DEALT WITH during this subtask
##        #        so that the produced GRIDS stored according to the abaqus format
##        (OUTER_GRID_No1, OUTER_GRID_No1_x, OUTER_GRID_No1_y, OUTER_GRID_No1_z,\
##        OUTER_GRID_No2, OUTER_GRID_No2_x, OUTER_GRID_No2_y, OUTER_GRID_No2_z,\
##        OUTER_GRID_No3, OUTER_GRID_No3_x, OUTER_GRID_No3_y, OUTER_GRID_No3_z,\
##        original_deltaXL, original_deltaXLT, original_deltaZ, ALL_ELEMENTS_VOL) = mesh_bdf2inp(ElementType,ALL_ELEMENTS_VOL_bool,Misalignement_with_Dir1,Misalignement_with_Dir2)
##        print
##        print "NODE_No1_ToBeConstrained = ",OUTER_GRID_No1, OUTER_GRID_No1_x, OUTER_GRID_No1_y, OUTER_GRID_No1_z
##        print "NODE_No2_ToBeConstrained = ",OUTER_GRID_No2, OUTER_GRID_No2_x, OUTER_GRID_No2_y, OUTER_GRID_No2_z
##        print "NODE_No3_ToBeConstrained = ",OUTER_GRID_No3, OUTER_GRID_No3_x, OUTER_GRID_No3_y, OUTER_GRID_No3_z
##        print
##        # If one wants to check the difference between WITH and WITHOUT accounting for Misalignements :
##        # CommandLine = Path_to_GMESHsoftware+" Nastran_TransformedGRIDs_File_without_Misalignement.bdf"
##        # os.system(CommandLine)
##        # CommandLine = Path_to_GMESHsoftware+" Nastran_TransformedGRIDs_File.bdf"
##        # os.system(CommandLine)
##
##        # 6) Append to the *File.inp* the Applied_Conditions in order to set the conditions of the calculation:
##        ApplyingBC_IC(YoungModulus,PoissonCoeff,OUTER_GRID_No1,OUTER_GRID_No2,OUTER_GRID_No3)
##
##        # 7) CALCULIX_Calculations :
##        # Edit a batch file (it may be necessary later on) :
##        batch = open ('lcmt_CALCULIX_Calculation_batch.bat','w')
##        print "writing job execution file lcmt_CALCULIX_Calculation_batch.bat"
##        print "Provide a file MYcase1_sigini_input.txt if necessary !"
##        print "Provide a file MYcase1.NormalInfos_txt if necessary !"
##        print "Provide a file MYcase1.ThicknessInfos_txt if necessary !"
##        print "Provide a file MYcase1.StressInfos_txt if necessary !"
##        #
##        CALCULIX_Input_Output = "-i TransformedMesh_File > calculix_output.out"
##        batch.write("%s %s \n" %(Path_to_FEMsoftware, CALCULIX_Input_Output))
##        #
##        batch.write("exit")
##        batch.close()
##        # Direct submission of the CALCULIX_Calculations :
##        CommandLine = "./lcmt_CALCULIX_Calculation_batch.bat"
##        os.system("chmod +x ./lcmt_CALCULIX_Calculation_batch.bat")
##        print "Launching job; look in calculix_output.out if needed"
##        os.system(CommandLine)
##
##        # 8) Post-process the results
##        calculix_output_postprocess(original_deltaXL,original_deltaXLT,original_deltaZ,ALL_ELEMENTS_VOL)
##
##        # 9)  Building-up the Nastran_deformedGRIDs_File.bdf :
##        print "Building Nastran_deformedGRIDs_File.bdf"
##        CommandLine = "cat Nastran_displacedNODEs.txt Nastran_ELEMENTs.txt > Nastran_deformedGRIDs_File.bdf"
##        os.system(CommandLine)
##
##        # 10) Cleaning + Managing results files :
##        print "Moving results to CASE dir"
##        filesToBeKept = "pointG_PrincipalAxis.txt TransformedMesh_File.inp Nastran_TransformedGRIDs_File.bdf Applied_Conditions.txt lcmt_CALCULIX_Calculation_batch.bat Machining_distorsion.res Nastran_deformedGRIDs_File.bdf TransformedMesh_File.frd calculix_output.out Nastran_TransformedGRIDs_File_without_Misalignement.bdf"
##        CommandLine = "mv "+filesToBeKept+" "+Case_Dir
##        os.system(CommandLine)
##        CommandLine = "cp input_generalINFOS.txt "+Case_Dir
##        os.system(CommandLine)
##        print "Cleaning up"
##        CommandLine = "rm -f Nastran_originalMeshFile.bdf Nastran_originalMeshFile_HeadLines.bdf Nastran_GRIDS_in_PrincipalAxis_1overFreqlines.txt Nastran_GRIDS_in_PrincipalAxis_ALLlines.bdf tail_Nastran_originalMeshFile.bdf typeELEMENT.txt TransformedMesh_File_rot1_1overFreqlines.txt Nastran_displacedNODEs.txt Nastran_ELEMENTs.txt"
##        os.system(CommandLine)





##Main function to be used for GUI setup and entering into the main loop
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    myapp = MyForm()
    myapp.show()
    sys.exit(app.exec_())
	
	
	
	
	
	
	
	
	