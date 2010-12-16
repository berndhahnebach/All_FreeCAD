def calculix_output_postprocess(originalBB_deltaXL,originalBB_deltaXLT,originalBB_deltaZ,ALL_ELEMENTS_VOL) :
        #
        # info : the originalBB_deltaXL, originalBB_deltaXLT and originalBB_deltaZ  are 
        #        the dim of the bounding box BEFORE the Machining Distorsion !
        #        As far as their calculation from the Nodes coordinates may be wrong 
        #        if the Nodes coordinates read in the FILE.frd are not the original ones,
        #        it is safer to enter the current function with the rightly computed dimensions
        #        of the bounding box BEFORE the Machining Distorsion
        #
        #        Actually , if the GRIDs stored are indeed the undeformed ones,   
        #        the deformed GRIDs may be computed accoring to  GRIDs + Nodal_Displacements
        #
        print "Beginning CalculiX output postprocessing"
        #
        displacedNODEs_file = open ('Nastran_displacedNODEs.txt','w')
        displacedNODEs_file.write("BEGIN BULK \n")
        displacedNODEs_file.write("$.. \n")
        displacedNODEs_file.write("$..   NODES \n")
        #
        filename2 = "Machining_distorsion.res"
        out_FILE = open (filename2,'w')
        #
        XLmin = +100000000000.0
        XLTmin = +100000000000.0
        Zmin = +100000000000.0
        XLmax = -100000000000.0
        XLTmax = -100000000000.0
        Zmax = -100000000000.0
        D_x_min = +100000000000.0
        D_y_min = +100000000000.0
        D_z_min = +100000000000.0
        D_x_max = -100000000000.0
        D_y_max = -100000000000.0
        D_z_max = -100000000000.0
        field_1_min = +100000000000.0
        field_2_min = +100000000000.0
        field_3_min = +100000000000.0
        field_4_min = +100000000000.0
        field_5_min = +100000000000.0
        field_6_min = +100000000000.0
        field_1_max = -100000000000.0
        field_2_max = -100000000000.0
        field_3_max = -100000000000.0
        field_4_max = -100000000000.0
        field_5_max = -100000000000.0
        field_6_max = -100000000000.0
        deformed_Z_min = +100000000000.0
        deformed_Z_max = -100000000000.0
        deformed_Y_min = +100000000000.0
        deformed_Y_max = -100000000000.0
        deformed_X_min = +100000000000.0
        deformed_X_max = -100000000000.0
        originalBB_X = []
        originalBB_Y = []
        originalBB_Z = []
        #
        # 1/ Find the dimensions of the bounding box AFTER the Machining Distorsion !
        filename1 = "TransformedMesh_File.frd"
        inp_FILEfrd = open (filename1,'r')
        #
        # Skip the Heading-lines in order to reach the Grid-lines :
        filename1_CurrentLine = inp_FILEfrd.readline()
        HeadingLines = 0
        boolGoOn = True
        while boolGoOn :
            if (filename1_CurrentLine.startswith(" -1")):
              boolGoOn = False
            else:
              HeadingLines += 1
              filename1_CurrentLine = inp_FILEfrd.readline()  # in order to loop over lines
        GRIDs_lineNo1 = HeadingLines
        #
        # Deal with Grid-lines section  :
        out_FILE.write("GRID section starts at lineNo %i \n" %GRIDs_lineNo1)
        out_FILE.write("verif: 1st NODE-line = %s \n" %filename1_CurrentLine)
        NlinesGRID = 0
        boolGoOn = True
        while boolGoOn :
           if (filename1_CurrentLine.startswith(" -1")):
              #-info : The current line should read as follows :
              #-info :  -1        10-2.59997E+07-1.01646E-05 1.53345E-13
              #-info : 0123456789012345678901234567890123456789012345678
              #-info : FFL< NodeID ><  GRIDs_x ><  GRIDs_y ><  GRIDs_z >
              #-info : FFL is the First Field in the current Line : it is composed of  3 digits !
              # info : if FFL == " -1 " found in this line, then NODE_z-value  contained
              #-info : between digits No37 and No48 ( i.e  NODE_z=line[37:49[ )
              #-info : NODE_ID contained between digits No3 and No12   
              # NODE_ID=int(filename1_CurrentLine[3:13[) )
              XL  = float(filename1_CurrentLine[13:25])    #   one MUST have  XL==GRIDs_x original
              XLT = float(filename1_CurrentLine[25:37])    #   one MUST have  XLT==GRIDs_y original
              Z   = float(filename1_CurrentLine[37:49])    #   one MUST have  Z==GRIDs_z original
              originalBB_X.append(XL)
              originalBB_Y.append(XLT)
              originalBB_Z.append(Z)
              NlinesGRID += 1
              if   ( XL > XLmax ) :
                 XLmax = XL
              elif ( XL < XLmin ) :
                 XLmin = XL
              if   ( XLT > XLTmax ) :
                 XLTmax = XLT
              elif ( XLT < XLTmin ) :
                 XLTmin = XLT
              if   ( Z > Zmax ) :
                 Zmax = Z
              elif ( Z < Zmin ) :
                 Zmin = Z
              filename1_CurrentLine = inp_FILEfrd.readline() # in order to loop over ALL Grid-lines
           else:
              boolGoOn = False
        out_FILE.write("%i NODES have been found in file %s \n" %(NlinesGRID,filename1)) 
        out_FILE.write("read_inFILE.frd , XL : min= %f   &   max= %f \n" %(XLmin,XLmax))
        out_FILE.write("read_inFILE.frd , XLT : min= %f   &   max= %f \n" %(XLTmin,XLTmax))
        out_FILE.write("read_inFILE.frd , Z : min= %f   &   max= %f \n" %(Zmin,Zmax))
        out_FILE.write(" \n")
        #
        # Skip ALL lines for reaching DISPLACEMENT section :
        LinesSkipped  = 0
        boolGoOn = True
        while boolGoOn :
          # info : if "DISP" not found in CurrentLine, then foundDISP="-1"
          found_DISP = filename1_CurrentLine.find("DISP")
          if ( found_DISP != -1 ) :  # thus "DISP" has been found in CurrentLine
             boolGoOn = False
          else :
             filename1_CurrentLine = inp_FILEfrd.readline() 
             LinesSkipped += 1
        # end  while boolGoOn
        #
        DISP_beginLine = LinesSkipped + NlinesGRID + HeadingLines+1
        # Because the 1st DISP-line is 5 lines further : 
        for l in range(5) :
          DISP_beginLine += 1
          filename1_CurrentLine = inp_FILEfrd.readline()
        # Deal with DISPLACEMENT section  (in particular  the Z-displacements) :
        out_FILE.write("DISPLACEMENT section starts at lineNo %i \n" %DISP_beginLine)
        out_FILE.write("verif: 1st DISP-line = %s \n" %filename1_CurrentLine)
        NlinesDISP = 0
        boolGoOn = True
        while boolGoOn :
           if (filename1_CurrentLine.startswith(" -1")):
              #-info : The current line should read as follows :
              #-info :  -1        10-2.59997E+07-1.01646E-05 1.53345E-13
              #-info : 0123456789012345678901234567890123456789012345678
              #-info : FFL< NodeID ><  DISPL_x ><  DISPL_y ><  DISPL_z >
              #-info : FFL is the First Field in the current Line : it is composed of  3 digits !
              # info : if FFL == " -1 " found in this line, then NODE_z-value  contained
              #-info : between digits No37 and No48 ( i.e  DISPL_z=line[37:49[ )
              #-info : NODE_ID contained between digits No3 and No12   
              NODE_ID   =int(filename1_CurrentLine[3:13])
              D_x  = float(filename1_CurrentLine[13:25])  # Displacement_XL
              D_y  = float(filename1_CurrentLine[25:37])  # Displacement_XLT
              D_z  = float(filename1_CurrentLine[37:49])  # Displacement_Z
              if   ( D_x > D_x_max ) :
                 D_x_max = D_x
                 NodeID_Dx_max =  NODE_ID
              elif ( D_x < D_x_min ) :
                 D_x_min = D_x
                 NodeID_Dx_min =  NODE_ID
              if   ( D_y > D_y_max ) :
                 D_y_max = D_y
                 NodeID_Dy_max =  NODE_ID
              elif ( D_y < D_y_min ) :
                 D_y_min = D_y
                 NodeID_Dy_min =  NODE_ID
              if   ( D_z > D_z_max ) :
                 D_z_max = D_z
                 NodeID_Dz_max =  NODE_ID
              elif ( D_z < D_z_min ) :
                 D_z_min = D_z
                 NodeID_Dz_min =  NODE_ID
              #
              # info : originalBB_Z  and  D_z  MUST BE stored  in the same way !!!
              deformed_X = originalBB_X[NlinesDISP] + D_x   #  X_beforeMachining + Displacement_X
              deformed_Y = originalBB_Y[NlinesDISP] + D_y   #  Y_beforeMachining + Displacement_Y
              deformed_Z = originalBB_Z[NlinesDISP] + D_z   #  Z_beforeMachining + Displacement_Z
              displacedNODEs_file.write("GRID    %8i        %8.2f%8.2f%8.2f  \n" %(NODE_ID,deformed_X,deformed_Y,deformed_Z))
              NlinesDISP += 1  #  it is mandatory to keep this statement AFTER the previous one ! 
              if   ( deformed_Z > deformed_Z_max ) :
                 deformed_Z_max = deformed_Z
              if   ( deformed_Z < deformed_Z_min ) :
                 deformed_Z_min = deformed_Z
              if   ( deformed_Y > deformed_Y_max ) :
                 deformed_Y_max = deformed_Y
              if   ( deformed_Y < deformed_Y_min ) :
                 deformed_Y_min = deformed_Y
              if   ( deformed_X > deformed_X_max ) :
                 deformed_X_max = deformed_X
              if   ( deformed_X < deformed_X_min ) :
                 deformed_X_min = deformed_X
              #
              filename1_CurrentLine = inp_FILEfrd.readline() # in order to loop over lines
           else:
              boolGoOn = False
        out_FILE.write("%i DISPL-values have been found in file %s \n" %(NlinesDISP,filename1)) 
        out_FILE.write("read_inFILE.frd , found DISP_XL : min=%f   at Node_No=%i \n" %(D_x_min,NodeID_Dx_min))
        out_FILE.write("read_inFILE.frd , found DISP_XL : max=%f   at Node_No=%i \n" %(D_x_max,NodeID_Dx_max))
        out_FILE.write("read_inFILE.frd , found DISP_XLT : min=%f   at Node_No=%i \n" %(D_y_min,NodeID_Dy_min))
        out_FILE.write("read_inFILE.frd , found DISP_XLT : max=%f   at Node_No=%i \n" %(D_y_max,NodeID_Dy_max))
        out_FILE.write("read_inFILE.frd , found DISP_Z : min=%f   at Node_No=%i \n" %(D_z_min,NodeID_Dz_min))
        out_FILE.write("read_inFILE.frd , found DISP_Z : max=%f   at Node_No=%i \n" %(D_z_max,NodeID_Dz_max))
        out_FILE.write(">>> calculated deformed_Z : min= %f   &   max= %f \n" %(deformed_Z_min,deformed_Z_max))
        out_FILE.write(" \n")
        #
        # Skip ALL lines for reaching STRESS section :
        LinesSkipped  = 0
        boolGoOn = True
        while boolGoOn :
          # info : if "STRESS" not found in CurrentLine, then foundSTRESS="-1"
          found_STRESS = filename1_CurrentLine.find("STRESS")
          if ( found_STRESS != -1 ) :  # thus "STRESS" has been found in CurrentLine
             boolGoOn = False
          else :
             filename1_CurrentLine = inp_FILEfrd.readline() 
             LinesSkipped += 1
        # end  while boolGoOn
        #
        # Because the 1st STRESS-line is 7 lines further actually ! 
        for l in range(7) : filename1_CurrentLine = inp_FILEfrd.readline()
        # Deal with STRESS section :
        out_FILE.write("verif: 1st STRESS-line = %s \n" %filename1_CurrentLine)
        NlinesSTRESS = 0
        boolGoOn = True
        while boolGoOn :
           if (filename1_CurrentLine.startswith(" -1")):
              #-info : The current line should read as follows :
              #-info :  -1        10-2.59997E+07-1.01646E-05 1.53345E-13
              #-info : 0123456789012345678901234567890123456789012345678
              #-info : FFL< NodeID ><    SXX   ><    SYY   ><    SZZ   ><    SXY   ><    SYZ   ><    SZX   >
              field_1  = float(filename1_CurrentLine[13:25])  #  SXX
              field_2  = float(filename1_CurrentLine[25:37])  #  SYY
              field_3  = float(filename1_CurrentLine[37:49])  #  SZZ
              field_4  = float(filename1_CurrentLine[49:61])  #  SXY
              field_5  = float(filename1_CurrentLine[61:73])  #  SYZ
              field_6  = float(filename1_CurrentLine[73:85])  #  SZX
              NlinesSTRESS += 1
              if   ( field_1 > field_1_max ) :
                 field_1_max = field_1
              elif ( field_1 < field_1_min ) :
                 field_1_min = field_1
              if   ( field_2 > field_2_max ) :
                 field_2_max = field_2
              elif ( field_2 < field_2_min ) :
                 field_2_min = field_2
              if   ( field_3 > field_3_max ) :
                 field_3_max = field_3
              elif ( field_3 < field_3_min ) :
                 field_3_min = field_3
              if   ( field_4 > field_4_max ) :
                 field_4_max = field_4
              elif ( field_4 < field_4_min ) :
                 field_4_min = field_4
              if   ( field_5 > field_5_max ) :
                 field_5_max = field_5
              elif ( field_5 < field_5_min ) :
                 field_5_min = field_5
              if   ( field_6 > field_6_max ) :
                 field_6_max = field_6
              elif ( field_6 < field_6_min ) :
                 field_6_min = field_6
              filename1_CurrentLine = inp_FILEfrd.readline() # in order to loop over lines
           else:
              boolGoOn = False
        out_FILE.write("read_inFILE.frd , STRESS SXX : min= %f   &   max= %f \n" %(field_1_min,field_1_max))
        out_FILE.write("read_inFILE.frd , STRESS SYY : min= %f   &   max= %f \n" %(field_2_min,field_2_max))
        out_FILE.write("read_inFILE.frd , STRESS SZZ : min= %f   &   max= %f \n" %(field_3_min,field_3_max))
        out_FILE.write("read_inFILE.frd , STRESS SXY : min= %f   &   max= %f \n" %(field_4_min,field_4_max))
        out_FILE.write("read_inFILE.frd , STRESS SYZ : min= %f   &   max= %f \n" %(field_5_min,field_5_max))
        out_FILE.write("read_inFILE.frd , STRESS SZX : min= %f   &   max= %f \n" %(field_6_min,field_6_max))
        out_FILE.write(" \n")

        # 2/ Compare the dimensions of the bounding box BEFORE vs AFTER the Machining Distorsion !
        deltaXL = XLmax - XLmin        #   with   originalBB_deltaXL
        deltaXLT = XLTmax - XLTmin     #   with   originalBB_deltaXLT
        deltaZ = Zmax - Zmin           #   with   originalBB_deltaZ
        new_deltaXL = deformed_X_max - deformed_X_min
        new_deltaXLT = deformed_Y_max - deformed_Y_min
        new_deltaZ = deformed_Z_max - deformed_Z_min
        out_FILE.write(" \n")
        out_FILE.write("verif : %f deltaXL   ?=?  originalBB_deltaXL %f \n" %(deltaXL,originalBB_deltaXL))
        out_FILE.write("verif : %f deltaXLT   ?=?  originalBB_deltaXLT %f \n" %(deltaXLT,originalBB_deltaXLT))
        out_FILE.write("verif : %f deltaZ   ?=?  originalBB_deltaZ %f \n" %(deltaZ,originalBB_deltaZ))
        out_FILE.write(" \n")
        out_FILE.write(">>> originalBB_deltaX=%f   vs  deformed_deltaX=%f \n" %(deltaXL,new_deltaXL))
        out_FILE.write(">>> originalBB_deltaY=%f   vs  deformed_deltaY=%f \n" %(deltaXLT,new_deltaXLT))
        out_FILE.write(">>> originalBB_deltaZ=%f   vs  deformed_deltaZ=%f \n" %(deltaZ,new_deltaZ))
        out_FILE.write(" \n")
        originalBB_VOL = deltaXL * deltaXLT * deltaZ
        deformedBB_VOL = new_deltaXL * new_deltaXLT * new_deltaZ
        out_FILE.write(">>> VOLUME of the original minimalBB = %f \n" %originalBB_VOL)
        out_FILE.write(">>> VOLUME of the deformed minimalBB = %f \n" %deformedBB_VOL)
        out_FILE.write(" \n")
        if (ALL_ELEMENTS_VOL != 0) : 
          buy_to_fly = ALL_ELEMENTS_VOL / originalBB_VOL
          out_FILE.write(">>> VOLUME covered by ALL the ELEMENTS = %f \n" %ALL_ELEMENTS_VOL)
          out_FILE.write(">>> buy_to_fly = ALL_ELEMENTS_volume / originalBB_volume = %f \n" %buy_to_fly)
        else :
          out_FILE.write("ALL_ELEMENTS_volume not computed: flag ALL_ELEMENTS_VOL_turnedON = False")
        
        out_FILE.close()
        inp_FILEfrd.close()
        displacedNODEs_file.close()
        print "CalculiX output postprocessing ended"
        #
        return
