def  mesh_bdf2inp(ElementType,ALL_ELEMENTS_VOL_bool,\
                  Misalignement_with_DirL,Misalignement_with_DirLT) :
	#< == indent
	#
	# Variables generales 
	import os
	import sys
	import string,math,shutil,glob
        from math import pi,cos,sin,sqrt,tan,atan,asin,acos
	#
	output_MeshFile_abaqus = open ('TransformedMesh_File.inp','w')
        output_MeshFile_abaqus.write("*Heading")
        output_MeshFile_abaqus.write("\n** Job name: abaqus_model Model name: abaqus_model")
        output_MeshFile_abaqus.write("\n** Translated from NASTRAN by : Python ")
        output_MeshFile_abaqus.write("\n*Preprint, echo=NO, model=NO, history=NO, contact=NO")
        output_MeshFile_abaqus.write("\n**")
        # useless : output_MeshFile_abaqus.write("\n** PARTS")
        # useless : output_MeshFile_abaqus.write("\n*Part, name=PART-1")
        # useless : output_MeshFile_abaqus.write("\n*End Part, name=PART-1")
        # useless : output_MeshFile_abaqus.write("\n**")
        # useless : actually, the whole domain is considered as a single Part !!OA
        #
        filename = "pointG_PrincipalAxis.txt"
        AxesInertie_MeshFile = open (filename,'r')
        boolGoOn = True
        Ligne = AxesInertie_MeshFile.readline()
        Ligne = Ligne.replace("Inertial Matrix :","")  
        stringC=Ligne.split(",")
        # print "stringC = ",stringC
        Vect1_x = float(stringC[0])
        Vect1_y = float(stringC[3])
        Vect1_z = float(stringC[6])
        Vect2_x = float(stringC[1])
        Vect2_y = float(stringC[4])
        Vect2_z = float(stringC[7])
        Vect3_x = float(stringC[2])
        Vect3_y = float(stringC[5])
        Vect3_z = float(stringC[8])

        Ligne = AxesInertie_MeshFile.readline()
        Ligne = Ligne.replace("COG Position :","")
        stringC=Ligne.split(",")
        print "stringC = ",stringC
        # it appears that the COG Position is expressed in meter instead of mm
        print "a factor 1000 is used for coherence with the rest of the mesh_data !!!!!"
        pointG_x = float(stringC[0]) * 1000.0
        pointG_y = float(stringC[1]) * 1000.0
        pointG_z = float(stringC[2]) * 1000.0
        #-or while boolGoOn :
          #-or if (Ligne.startswith("Solide")):
              #-or print "Ligne=",Ligne
              #-or # Infos for GRIDs transformation : global -> local coord-systems :
              #-or # The 3x3 matrix VectI_J  allows to transform 
              #-or # a vector VectGM in respect to (pointG; X-Y-Z) coord Syst  into 
              #-or # a vector VectGM expressed in respect to (pointG; Vect1-Vect2-Vect3) coord Syst :
              #-or pointG_x = float(Ligne[246:262])
              #-or pointG_y = float(Ligne[264:280])
              #-or pointG_z = float(Ligne[282:298])
              #-or Vect1_x = float(Ligne[354:370])
              #-or Vect1_y = float(Ligne[372:388])
              #-or Vect1_z = float(Ligne[390:406])
              #-or Vect2_x = float(Ligne[408:424])
              #-or Vect2_y = float(Ligne[426:442])
              #-or Vect2_z = float(Ligne[444:460])
              #-or Vect3_x = float(Ligne[462:478])
              #-or Vect3_y = float(Ligne[480:496])
              #-or Vect3_z = float(Ligne[498:514])
              #-or boolGoOn = False
          #-or Ligne = AxesInertie_MeshFile.readline()
        AxesInertie_MeshFile.close()
        print "Center of Inertie :"
        print "pointG_x=",pointG_x
        print "pointG_y=",pointG_y
        print "pointG_z=",pointG_z
        print
        print "Vect1:cpst_x_y_z = ",Vect1_x,"   ",Vect1_y,"   ",Vect1_z
        print
        print "Vect2:cpst_x_y_z = ",Vect2_x,"   ",Vect2_y,"   ",Vect2_z
        print
        print "Vect3:cpst_x_y_z = ",Vect3_x,"   ",Vect3_y,"   ",Vect3_z
        print

        ### Statements usefull to verify the informations extrated from CATIA :
        # /////////////////////////////////////////////////////////////////////
        print "/////////////////////////////////////////////////////////////"
        print "A few verifications of the informations extrated from CATIA :"
        print
        ScalarProduct_V1_V2 = Vect1_x*Vect2_x+Vect1_y*Vect2_y+Vect1_z*Vect2_z
        print "ScalarProduct_V1_V2 = ",ScalarProduct_V1_V2
        ScalarProduct_V1_V3 = Vect1_x*Vect3_x+Vect1_y*Vect3_y+Vect1_z*Vect3_z
        print "ScalarProduct_V1_V3 = ",ScalarProduct_V1_V3
        ScalarProduct_V3_V2 = Vect3_x*Vect2_x+Vect3_y*Vect2_y+Vect3_z*Vect2_z
        print "ScalarProduct_V3_V2 = ",ScalarProduct_V3_V2
        print
        #  Evaluate angles of the 3 local-coord-system Vectors 
        #  in respect to the global-coord-system Vectors :
        Vnorm = sqrt(Vect1_x*Vect1_x + Vect1_y*Vect1_y + Vect1_z*Vect1_z)
        cos_angle_Vect1_z = Vect1_z/Vnorm
        angle_Vect1_z = acos(cos_angle_Vect1_z) * 180 / pi  #  acos function return angles in rd
        Vproj_xy = Vnorm*sin(angle_Vect1_z *pi/180)
        Vnorm_verif = sqrt(Vproj_xy*Vproj_xy + Vect1_z*Vect1_z)
        print "verif   Vect1:Vnorm=",Vnorm,"   ; dans plan x-y  Vproj_xy=",Vproj_xy
        print "verif : angle_Vect1_z=",angle_Vect1_z
        cos_angle_Vect1_x = Vect1_x/Vproj_xy
        angle_Vect1_x = acos(cos_angle_Vect1_x) * 180 / pi 
        Vect1_x_verif = Vproj_xy*cos(angle_Vect1_x *pi/180)
        Vect1_y_verif = Vproj_xy*sin(angle_Vect1_x *pi/180)
        if ( (Vect1_x_verif/Vect1_x <= 0) & (Vect1_y_verif/Vect1_y <= 0) ) :
           angle_Vect1_x = angle_Vect1_x + 180
        elif ( (Vect1_x_verif/Vect1_x >= 0) & (Vect1_y_verif/Vect1_y <= 0) ) :
           angle_Vect1_x = 0 - angle_Vect1_x 
        elif ( (Vect1_x_verif/Vect1_x <= 0) & (Vect1_y_verif/Vect1_y >= 0) ) :
           angle_Vect1_x = 180 - angle_Vect1_x 
        # elif ( (Vect1_x_verif/Vect1_x >= 0) & (Vect1_y_verif/Vect1_y >= 0) ) : do Nothing !
        Vect1_x_verif = Vproj_xy*cos(angle_Vect1_x *pi/180)
        Vect1_y_verif = Vproj_xy*sin(angle_Vect1_x *pi/180)
        Vect1_z_verif = Vnorm*cos(angle_Vect1_z *pi/180)
        print "verif : Vect1_x_verif=",Vect1_x_verif,"   ?=?  ",Vect1_x
        print "verif : Vect1_y_verif=",Vect1_y_verif,"   ?=?  ",Vect1_y
        print "verif : Vect1_z_verif=",Vect1_z_verif,"   ?=?  ",Vect1_z
	#
        Vnorm = sqrt(Vect2_x*Vect2_x + Vect2_y*Vect2_y + Vect2_z*Vect2_z)
        cos_angle_Vect2_z = Vect2_z/Vnorm
        angle_Vect2_z = acos(cos_angle_Vect2_z) * 180 / pi  #  acos function return angles in rd
        Vproj_xy = Vnorm*sin(angle_Vect2_z *pi/180)
        Vnorm_verif = sqrt(Vproj_xy*Vproj_xy + Vect2_z*Vect2_z)
        print "verif   Vect2:Vnorm=",Vnorm,"   ; dans plan x-y  Vproj_xy=",Vproj_xy
        print "verif : angle_Vect2_z=",angle_Vect2_z
        cos_angle_Vect2_x = Vect2_x/Vproj_xy
        angle_Vect2_x = acos(cos_angle_Vect2_x) * 180 / pi  #  acos function return angles in rd
        Vect2_x_verif = Vproj_xy*cos(angle_Vect2_x *pi/180)
        Vect2_y_verif = Vproj_xy*sin(angle_Vect2_x *pi/180)
        if ( (Vect2_x_verif/Vect2_x <= 0) & (Vect2_y_verif/Vect2_y <= 0) ) :
           angle_Vect2_x = angle_Vect2_x + 180
        elif ( (Vect2_x_verif/Vect2_x >= 0) & (Vect2_y_verif/Vect2_y <= 0) ) :
           angle_Vect2_x = 0 - angle_Vect2_x 
        elif ( (Vect2_x_verif/Vect2_x <= 0) & (Vect2_y_verif/Vect2_y >= 0) ) :
           angle_Vect2_x = 180 - angle_Vect2_x 
        # elif ( (Vect2_x_verif/Vect2_x >= 0) & (Vect2_y_verif/Vect2_y >= 0) ) : do Nothing !
        Vect2_x_verif = Vproj_xy*cos(angle_Vect2_x *pi/180)
        Vect2_y_verif = Vproj_xy*sin(angle_Vect2_x *pi/180)
        Vect2_z_verif = Vnorm*cos(angle_Vect2_z *pi/180)
        print "verif : Vect2_x_verif=",Vect2_x_verif,"   ?=?  ",Vect2_x
        print "verif : Vect2_y_verif=",Vect2_y_verif,"   ?=?  ",Vect2_y
        print "verif : Vect2_z_verif=",Vect2_z_verif,"   ?=?  ",Vect2_z
	#
        Vnorm = sqrt(Vect3_x*Vect3_x + Vect3_y*Vect3_y + Vect3_z*Vect3_z)
        cos_angle_Vect3_z = Vect3_z/Vnorm
        angle_Vect3_z = acos(cos_angle_Vect3_z) * 180 / pi  #  acos function return angles in rd
        if ( (-90.0 <= angle_Vect3_z) & (angle_Vect3_z <= 90.0) ) : 
           Vproj_xy = Vnorm*sin(angle_Vect3_z *pi/180)
        else :
           Vproj_xy = Vnorm*sin(angle_Vect3_z *pi/180)
        Vnorm_verif = sqrt(Vproj_xy*Vproj_xy + Vect3_z*Vect3_z)
        print "verif   Vect3:Vnorm=",Vnorm,"   ; dans plan x-y  Vproj_xy=",Vproj_xy
        print "verif : angle_Vect3_z=",angle_Vect3_z
        cos_angle_Vect3_x = Vect3_x/Vproj_xy
        angle_Vect3_x = acos(cos_angle_Vect3_x) * 180 / pi  #  acos function return angles in rd
        Vect3_x_verif = Vproj_xy*cos(angle_Vect3_x *pi/180)
        Vect3_y_verif = Vproj_xy*sin(angle_Vect3_x *pi/180)
        if ( (Vect3_x_verif/Vect3_x <= 0) & (Vect3_y_verif/Vect3_y <= 0) ) :
           angle_Vect3_x = angle_Vect3_x + 180
        elif ( (Vect3_x_verif/Vect3_x >= 0) & (Vect3_y_verif/Vect3_y <= 0) ) :
           angle_Vect3_x = 0 - angle_Vect3_x 
        elif ( (Vect3_x_verif/Vect3_x <= 0) & (Vect3_y_verif/Vect3_y >= 0) ) :
           angle_Vect3_x = 180 - angle_Vect3_x 
        # elif ( (Vect3_x_verif/Vect3_x >= 0) & (Vect3_y_verif/Vect3_y >= 0) ) : do Nothing !
        Vect3_x_verif = Vproj_xy*cos(angle_Vect3_x *pi/180)
        Vect3_y_verif = Vproj_xy*sin(angle_Vect3_x *pi/180)
        Vect3_z_verif = Vnorm*cos(angle_Vect3_z *pi/180)
        print "verif : Vect3_x_verif=",Vect3_x_verif,"   ?=?  ",Vect3_x
        print "verif : Vect3_y_verif=",Vect3_y_verif,"   ?=?  ",Vect3_y
        print "verif : Vect3_z_verif=",Vect3_z_verif,"   ?=?  ",Vect3_z
        print
        print "angle_Vect1_z=",angle_Vect1_z,"   angle_Vect1_x=",angle_Vect1_x
        print "angle_Vect2_z=",angle_Vect2_z,"   angle_Vect2_x=",angle_Vect2_x
        print "angle_Vect3_z=",angle_Vect3_z,"   angle_Vect3_x=",angle_Vect3_x
        print "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"
	print
        print
        ### end of statements to verify the informations extrated from CATIA 
        # \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        Freq = 1
        print 
        print "In order to save a great deal of time, only 1 over ",Freq," lines "
        print "are considered to look for the minimal Bounding Box !!"   
        print 
	
        Xmin = +100000000000.0
        Ymin = +100000000000.0
        Zmin = +100000000000.0
        Xmax = -100000000000.0
        Ymax = -100000000000.0
        Zmax = -100000000000.0
        newXmin = +100000000000.0
        newYmin = +100000000000.0
        newZmin = +100000000000.0
        newXmax = -100000000000.0
        newYmax = -100000000000.0
        newZmax = -100000000000.0
        new_pointG_x = 0.0
        new_pointG_y = 0.0
        new_pointG_z = 0.0
        verif_prev_pointG_x = 0.0
        verif_prev_pointG_y = 0.0
        verif_prev_pointG_z = 0.0

	# 1/ Extract from the *input_MeshFile.bdf* the Heading-lines, 
        #    then the Grid-lines and finally the Element-lines : 
        codage = None
        HeadingLines = 0
        filename1 = "Nastran_originalMeshFile.bdf"
        input_MeshFile = open (filename1,'r')
        filename2 = "Nastran_originalMeshFile_HeadLines.bdf"
        HeadingLines_File  = open (filename2,'w')
        # filename3 = "Nastran_onlyGRIDsFile.bdf"
        # ONLYoriginalGRIDs_file = open (filename3,'w')
        filename4 = "Nastran_GRIDS_in_PrincipalAxis_1overFreqlines.txt"
	TransformedGRIDs_1overFreq_file= open (filename4,'w')
        NlinesGRID = 0
        Nlines_1overFreq = 0
        filename5 = "Nastran_GRIDS_in_PrincipalAxis_ALLlines.bdf"
        GRIDS_in_PrincipalAxis_ALLlines = open (filename5,'w')
	#
        # 1/ Heading lines :
        filename1_CurrentLine = input_MeshFile.readline()
        HeadingLines_File.write("%s \n" %filename1_CurrentLine)
        boolGoOn = True
        while boolGoOn :
            if (filename1_CurrentLine.startswith("GRID")):
              boolGoOn = False
            else:
              filename1_CurrentLine = input_MeshFile.readline()
              HeadingLines_File.write("%s \n" %filename1_CurrentLine)
              HeadingLines += 1
        HeadingLines_File.close()
        #
        # 2/ Grid-lines  :
        s_GRID_ID = []
        s_GRID_X = []
        s_GRID_Y = []
        s_GRID_Z = []
        ALL_ELEMENTS_VOL = 0.0
        if ( ALL_ELEMENTS_VOL_bool ):
          print ""
          print "INFORMATION : calculation of ALL_ELEMENTS_VOLUME within the current process"
          print "CAUTION : as it is programmed and thus operated, this subtask may consume too much time"
          print ""
        else :  
          #  this subtask may consume too much time within the current process
          print ""
          print "INFORMATION : calculation of ALL_ELEMENTS_VOLUME  MUST be carried out outside "
          print "the current process , by means of NASTRAN (or CATIA) " 
          print ""
        #
        # Writing the Nodes list in abaqus format :
        output_MeshFile_abaqus.write("\n*Node , NSET=Nall")
        output_MeshFile_abaqus.write("\n")
        #
        GRIDs_lineNo1 = HeadingLines + 1
        print "GRID section starts at lineNo ",GRIDs_lineNo1,"  filename1_CurrentLine=",filename1_CurrentLine
        test = filename1_CurrentLine[8:16]
        if test != "        " :
            print "test != vide"
            codage = "8digits"
        else :
              codage = "16digits"
        print "information :  codage = ",codage
        GRIDs_lineEND = 0
        boolGoOn = True
        #
        if ( codage == "8digits" ) :
        # //////////////////////////
          while boolGoOn :
            foundGRID = filename1_CurrentLine.find("GRID")
            if (foundGRID != -1 ) : 
               GRIDs_lineEND  += 1
               NlinesGRID += 1
               GRID_ID = int(filename1_CurrentLine[8:16])
               GRID_X =  float(filename1_CurrentLine[24:32])
               GRID_Y =  float(filename1_CurrentLine[32:40])
               GRID_Z =  float(filename1_CurrentLine[40:48])
               # Storing those informations because they will be used later on :
               s_GRID_ID.append(GRID_ID)
               s_GRID_X.append(GRID_X)
               s_GRID_Y.append(GRID_Y)
               s_GRID_Z.append(GRID_Z)
               if ( GRID_X > Xmax ) :
                     Xmax=GRID_X
               elif ( GRID_X < Xmin ) :
                     Xmin=GRID_X
               if ( GRID_Y > Ymax ) :
                     Ymax=GRID_Y
               elif ( GRID_Y < Ymin ) :
                     Ymin=GRID_Y
               if ( GRID_Z > Zmax ) :
                      Zmax=GRID_Z
               elif ( GRID_Z < Zmin ) :
                      Zmin=GRID_Z
               # GRIDs transformation : global -> local coord-systems :
               # The 3x3 matrix VectI_J  allows to transform 
               # a vector VectGM in respect to (G; X-Y-Z) coord Syst  into 
               # a vector VectGM expressed in respect to (G; Vect1-Vect2-Vect3) coord Syst :
               # VectGM  <=  VectOM - VectOG : 
               GM_x = GRID_X  - pointG_x
               GM_y = GRID_Y  - pointG_y
               GM_z = GRID_Z  - pointG_z
               newGRID_X=Vect1_x*GM_x + Vect1_y*GM_y + Vect1_z*GM_z
               newGRID_Y=Vect2_x*GM_x + Vect2_y*GM_y + Vect2_z*GM_z
               newGRID_Z=Vect3_x*GM_x + Vect3_y*GM_y + Vect3_z*GM_z
               # one may restore the global position by a translation:
               newGRID_X += pointG_x
               newGRID_Y += pointG_y
               newGRID_Z += pointG_z

               verif_prev_pointG_x += GRID_X
               verif_prev_pointG_y += GRID_Y
               verif_prev_pointG_z += GRID_Z
               new_pointG_x += newGRID_X
               new_pointG_y += newGRID_Y
               new_pointG_z += newGRID_Z
               if ( newGRID_X > newXmax ) :
                     newXmax=newGRID_X
               elif ( newGRID_X < newXmin ) :
                     newXmin=newGRID_X
               if ( newGRID_Y > newYmax ) :
                     newYmax=newGRID_Y
               elif ( newGRID_Y < newYmin ) :
                     newYmin=newGRID_Y
               if ( newGRID_Z > newZmax ) :
                      newZmax=newGRID_Z
               elif ( newGRID_Z < newZmin ) :
                      newZmin=newGRID_Z

               ###??? GRIDS_in_PrincipalAxis_ALLlines.write("GRID    %8i        %8.2f%8.2f%8.2f \n" %(GRID_ID,newGRID_X,newGRID_Y,newGRID_Z))
               GRIDS_in_PrincipalAxis_ALLlines.write("%8i,%14.7e,%14.7e,%14.7e \n" %(GRID_ID,newGRID_X,newGRID_Y,newGRID_Z))
	       # ONLYoriginalGRIDs_file.write("%8i,%14.7e,%14.7e,%14.7e \n" %(GRID_ID,GRID_X,GRID_Y,GRID_Z))
               Line1overFreq = NlinesGRID/Freq*Freq
               if ( NlinesGRID == Line1overFreq ) :
	           TransformedGRIDs_1overFreq_file.write("%8i,%14.7e,%14.7e,i %14.7e \n" %(GRID_ID,newGRID_X,newGRID_Y,newGRID_Z))
                   Nlines_1overFreq += 1
               filename1_CurrentLine = input_MeshFile.readline()  #  in order to loop for the while !
            else:  #  ( foundGRID == -1 : the current line is no longer GRID line ! 
                boolGoOn = False
        # /////////////////////////////
        else  :  #  codage = "16digits" 
          while boolGoOn :
            foundGRID = filename1_CurrentLine.find("GRID")
            if (foundGRID != -1 ) : 
               GRIDs_lineEND  += 2
               NlinesGRID += 1
               # info : the first statement to read the 1st line of the Current Grid has been already done ! 
               # info : So do not do *filename1_CurrentLine = input_MeshFile.readline() here ! 
               GRID_ID =  int(filename1_CurrentLine[8:24])
               GRID_X =  float(filename1_CurrentLine[40:56])
               GRID_Y =  float(filename1_CurrentLine[56:72])
               # info : the GRID_Z  is stored on the subsequent line:
               filename1_CurrentLine = input_MeshFile.readline() # in order to read the subsequent line !
               GRID_Z =  float(filename1_CurrentLine[8:24])
               # Storing those informations because they will be used later on :
               s_GRID_ID.append(GRID_ID)
               s_GRID_X.append(GRID_X)
               s_GRID_Y.append(GRID_Y)
               s_GRID_Z.append(GRID_Z)
               if ( GRID_X > Xmax ) :
                     Xmax=GRID_X
               elif ( GRID_X < Xmin ) :
                     Xmin=GRID_X
               if ( GRID_Y > Ymax ) :
                     Ymax=GRID_Y
               elif ( GRID_Y < Ymin ) :
                     Ymin=GRID_Y
               if ( GRID_Z > Zmax ) :
                      Zmax=GRID_Z
               elif ( GRID_Z < Zmin ) :
                      Zmin=GRID_Z
               # GRIDs transformation : global -> local coord-systems :
               # The 3x3 matrix VectI_J  allows to transform 
               # a vector VectGM in respect to (G; X-Y-Z) coord Syst  into 
               # a vector VectGM expressed in respect to (G; Vect1-Vect2-Vect3) coord Syst :
               # VectGM  <=  VectOM - VectOG : 
               GM_x = GRID_X  - pointG_x
               GM_y = GRID_Y  - pointG_y
               GM_z = GRID_Z  - pointG_z
               newGRID_X=Vect1_x*GM_x + Vect1_y*GM_y + Vect1_z*GM_z
               newGRID_Y=Vect2_x*GM_x + Vect2_y*GM_y + Vect2_z*GM_z
               newGRID_Z=Vect3_x*GM_x + Vect3_y*GM_y + Vect3_z*GM_z
               # one may restore the global position by a translation:
               newGRID_X += pointG_x
               newGRID_Y += pointG_y
               newGRID_Z += pointG_z

               verif_prev_pointG_x += GRID_X
               verif_prev_pointG_y += GRID_Y
               verif_prev_pointG_z += GRID_Z
               new_pointG_x += newGRID_X
               new_pointG_y += newGRID_Y
               new_pointG_z += newGRID_Z
               if ( newGRID_X > newXmax ) :
                     newXmax=newGRID_X
               elif ( newGRID_X < newXmin ) :
                     newXmin=newGRID_X
               if ( newGRID_Y > newYmax ) :
                     newYmax=newGRID_Y
               elif ( newGRID_Y < newYmin ) :
                     newYmin=newGRID_Y
               if ( newGRID_Z > newZmax ) :
                      newZmax=newGRID_Z
               elif ( newGRID_Z < newZmin ) :
                      newZmin=newGRID_Z

               ###??? GRIDS_in_PrincipalAxis_ALLlines.write("GRID    %8i        %8.2f%8.2f%8.2f \n" %(GRID_ID,newGRID_X,newGRID_Y,newGRID_Z))
	       GRIDS_in_PrincipalAxis_ALLlines.write("%8i,%14.7e,%14.7e,%14.7e \n" %(GRID_ID,newGRID_X,newGRID_Y,newGRID_Z))
	       # ONLYoriginalGRIDs_file.write("%8i,%14.7e,%14.7e,%14.7e \n" %(GRID_ID,GRID_X,GRID_Y,GRID_Z))
               Line1overFreq = NlinesGRID/Freq*Freq
               if ( NlinesGRID == Line1overFreq ) :
	           TransformedGRIDs_1overFreq_file.write("%8i,%14.7e,%14.7e,%14.7e \n" %(GRID_ID,newGRID_X,newGRID_Y,newGRID_Z))
                   Nlines_1overFreq += 1
               filename1_CurrentLine = input_MeshFile.readline()  #  in order to loop for the while !
            else:  #  ( foundGRID == -1 : the current line is no longer GRID line ! 
              boolGoOn = False
        # end  if codage = "16digits" 
        print "GRID section ends at lineNo ",GRIDs_lineNo1+GRIDs_lineEND,"  filename1_CurrentLine=",filename1_CurrentLine
        print "Thus  ",GRIDs_lineEND,"  GRIDs lines  have been found !"
        # 
        # ONLYoriginalGRIDs_file.close()
        TransformedGRIDs_1overFreq_file.close()
        GRIDS_in_PrincipalAxis_ALLlines.close()
        #
        new_pointG_x /= NlinesGRID 
        new_pointG_y /= NlinesGRID 
        new_pointG_z /= NlinesGRID 
        verif_prev_pointG_x /= NlinesGRID 
        verif_prev_pointG_y /= NlinesGRID 
        verif_prev_pointG_z /= NlinesGRID 
        Delta_newX = newXmax-newXmin
        Delta_newY = newYmax-newYmin
        print 
	print "newXmin=",newXmin,"    newXmax=",newXmax
	print "newYmin=",newYmin,"    newYmax=",newYmax
        print "Delta_newX=",Delta_newX,"   Delta_newY=",Delta_newY
        print "new_pointG_x=",new_pointG_x,"  previous_pointG_x=",pointG_x,"  verif_prev_pointG_x=",verif_prev_pointG_x
        print "new_pointG_y=",new_pointG_y,"  previous_pointG_y=",pointG_y,"  verif_prev_pointG_y=",verif_prev_pointG_y
        print "new_pointG_z=",new_pointG_z,"  previous_pointG_z=",pointG_z,"  verif_prev_pointG_z=",verif_prev_pointG_z
        print 
        # 
        # 3/ Check and eventually better estimate the desired correction of 
        #    the coordinates so that the best alignement with the (L,LT) 
        #    coordinate system could be obtained  
        # NB1 a few iterations may be necessary to get the minimal Bounding Box 
        # >> Determining the 2 possible rotations necessary to get  
        # the XL-axis and XLT-axis without misalignement in the end !!! 
        # >> Applying the Composition of the 2 possible rotations
        # NB2 the chosen origine will be the point "new_pointG" 
        #    instead of the point (newXmin,newYmin,newZmin)  IF rotations effectively necessary ! 
        # 
        DependentRotations = False   #  or  True
        print
        if ( DependentRotations ) :
           print "determining the 2 possible re-alignement in a dependent way !"
        else :
           print "determining the 2 possible re-alignement in an INdependent way !"
        print
        #
        new_XLmin = []
        new_XLTmin = []
        new_Zmin = []
        # 3.1) For determining a first possible re-alignement :
        # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
        AngleIncr = 1.0  # expressed in degree   MAY BE TUNED !!!!!!!!!!!!!!!
        factor1 = AngleIncr *pi/180.  # expressed in rd !
        boolGoOn = True
        AngleIter = 1
        #  The 1st iter serves to determine whether it will be convenient to do 
        #  a clockwise or counter-clockwise right rotation :
        if ( boolGoOn ) :
          Angle_1 = factor1*AngleIter  # expressed in rd !
          Angle_2 = - Angle_1  
          TransformedGRIDs_1overFreq_file = open ('Nastran_GRIDS_in_PrincipalAxis_1overFreqlines.txt','r')
          prev_XLmax = - 10000000000.0
          prev_XLmin = + 10000000000.0
          prev_XLTmax = - 10000000000.0
          prev_XLTmin = + 10000000000.0
          prev_Zmax = - 10000000000.0
          prev_Zmin = + 10000000000.0
          XLmax_rot1 = - 10000000000.0
          XLmin_rot1 = + 10000000000.0
          XLTmax_rot1 = - 10000000000.0
          XLTmin_rot1 = + 10000000000.0
          XLmax_rot2 = - 10000000000.0
          XLmin_rot2 = + 10000000000.0
          XLTmax_rot2 = - 10000000000.0
          XLTmin_rot2 = + 10000000000.0
          new_Zmax = - 10000000000.0
          new_Zmin = + 10000000000.0
          for l in range(Nlines_1overFreq) :
            Line = TransformedGRIDs_1overFreq_file.readline()
            stringC = Line.split(",")
            XL = float(stringC[1])
            XLT = float(stringC[2])
            Z = float(stringC[3])
            if   ( XL > prev_XLmax ) :
                 prev_XLmax = XL
            elif ( XL < prev_XLmin ) :
                 prev_XLmin = XL 
            if   ( XLT > prev_XLTmax ) :
                 prev_XLTmax = XLT
            elif ( XLT < prev_XLTmin ) :
                 prev_XLTmin = XLT 
            if   ( Z > prev_Zmax ) :
                 prev_Zmax = Z
            elif ( Z < prev_Zmin ) :
                 prev_Zmin = Z
            # Modify global position : 
            XL  -= new_pointG_x # - newXmin
            XLT -= new_pointG_y # - newYmin
            Z   -= new_pointG_z # - newZmin
            # Performing the rotation_1 to deal with the 1st re-alignement : 
            new_Z = Z
            XL_rot1 = XL*cos(Angle_1) - XLT*sin(Angle_1)
            XLT_rot1 = XL*sin(Angle_1) + XLT*cos(Angle_1)
            XL_rot2 = XL*cos(Angle_2) - XLT*sin(Angle_2)
            XLT_rot2 = XL*sin(Angle_2) + XLT*cos(Angle_2)
            # restore global position : 
            new_Z    += new_pointG_z #  newZmin
            XL_rot1  += new_pointG_x #  newXmin
            XLT_rot1 += new_pointG_y #  newYmin
            XL_rot2  += new_pointG_x #  newXmin
            XLT_rot2 += new_pointG_y #  newYmin
            if   ( XL_rot1 > XLmax_rot1 ) :
                 XLmax_rot1 = XL_rot1
            elif ( XL_rot1 < XLmin_rot1 ) :
                 XLmin_rot1 = XL_rot1
            if   ( XLT_rot1 > XLTmax_rot1 ) :
                 XLTmax_rot1 = XLT_rot1
            elif ( XLT_rot1 < XLTmin_rot1 ) :
                 XLTmin_rot1 = XLT_rot1
            if   ( XL_rot2 > XLmax_rot2 ) :
                 XLmax_rot2 = XL_rot2
            elif ( XL_rot2 < XLmin_rot2 ) :
                 XLmin_rot2 = XL_rot2
            if   ( XLT_rot2 > XLTmax_rot2 ) :
                 XLTmax_rot2 = XLT_rot2
            elif ( XLT_rot2 < XLTmin_rot2 ) :
                 XLTmin_rot2 = XLT_rot2
            if   ( new_Z > new_Zmax ) :
                 new_Zmax = new_Z
            elif ( new_Z < new_Zmin ) :
                 new_Zmin = new_Z
          # end for l in range(Nlines_1overFreq) 
          TransformedGRIDs_1overFreq_file.close()
          previous_VolBB = (prev_Zmax-prev_Zmin)*(prev_XLTmax-prev_XLTmin)*(prev_XLmax-prev_XLmin)
          VolBB_rot1 = (new_Zmax-new_Zmin)*(XLTmax_rot1-XLTmin_rot1)*(XLmax_rot1-XLmin_rot1)
          VolBB_rot2 = (new_Zmax-new_Zmin)*(XLTmax_rot2-XLTmin_rot2)*(XLmax_rot2-XLmin_rot2)
          if ( VolBB_rot1 < previous_VolBB ) :
              print "previous_VolBB=",previous_VolBB,"  VolBB_rot1=",VolBB_rot1
              print "perform a CLOCKWISE rotation to deal with 1st re-alignement !"
              sign_rotation = +1
              previous_VolBB = VolBB_rot1
          elif ( VolBB_rot2 < previous_VolBB) :
              print "previous_VolBB=",previous_VolBB,"  VolBB_rot2=",VolBB_rot2
              print "perform a COUNTER-CLOCKWISE rotation to deal with 1st re-alignement !"
              sign_rotation = -1
              previous_VolBB = VolBB_rot2
          else :
              print "NO rotation to deal with 1st re-alignement !"
              boolGoOn = False
              sign_rotation = None 
        # end if ( boolGoOn ) 
        print
        if ( sign_rotation != None ) :
         #  From now on, go on with only the right rotation sign :
         factor1 = sign_rotation * AngleIncr *pi/180.0
         while boolGoOn :
          AngleIter += 1
          Angle_1 = factor1*AngleIter  # expressed in rd !
          TransformedGRIDs_1overFreq_file = open ('Nastran_GRIDS_in_PrincipalAxis_1overFreqlines.txt','r')
          new_XLmax = - 10000000000.0
          new_XLmin = + 10000000000.0
          new_XLTmax = - 10000000000.0
          new_XLTmin = + 10000000000.0
          new_Zmax = - 10000000000.0
          new_Zmin = + 10000000000.0
          for l in range(Nlines_1overFreq) :
            Line = TransformedGRIDs_1overFreq_file.readline()
            stringC = Line.split(",")
            XL = float(stringC[1])  - new_pointG_x # - newXmin
            XLT = float(stringC[2]) - new_pointG_y # - newYmin
            Z = float(stringC[3])   - new_pointG_z # - newZmin
            # Performing the rotation_1 to deal with the 1st re-alignement : 
            new_Z = Z
            new_XLT = XL*sin(Angle_1) + XLT*cos(Angle_1)
            new_XL = XL*cos(Angle_1) - XLT*sin(Angle_1)
            # restore global position : 
            new_Z   += new_pointG_z #  newZmin
            new_XLT += new_pointG_y #  newYmin
            new_XL  += new_pointG_x #  newXmin
            if   ( new_XL > new_XLmax ) :
                 new_XLmax = new_XL
            elif ( new_XL < new_XLmin ) :
                 new_XLmin = new_XL 
            if   ( new_XLT > new_XLTmax ) :
                 new_XLTmax = new_XLT
            elif ( new_XLT < new_XLTmin ) :
                 new_XLTmin = new_XLT 
            if   ( new_Z > new_Zmax ) :
                 new_Zmax = new_Z
            elif ( new_Z < new_Zmin ) :
                 new_Zmin = new_Z
          # end for l in range(Nlines_1overFreq) 
          TransformedGRIDs_1overFreq_file.close()
          new_VolBB = (new_Zmax-new_Zmin)*(new_XLTmax-new_XLTmin)*(new_XLmax-new_XLmin)
          print "previous_VolBB=",previous_VolBB,"  new_VolBB=",new_VolBB
          if ( new_VolBB >= previous_VolBB) :
              boolGoOn = False
              print "END of rotation attempts to deal with 1st re-alignement !"
          else :
              print "loop on rotation attempts to deal with 1st re-alignement !"
              print "Angle_1 (in degree) = ",Angle_1*180/pi
              previous_VolBB = new_VolBB
         # end while boolGoOn 
         Angle_1 = factor1*(AngleIter-1) # expressed in rd !
         print
         print "before rotation1, Z   : max=",newZmax,"  min=",newZmin
         print "before rotation1, XL  : max=",newXmax,"  min=",newXmin
         print "before rotation1, XLT : max=",newYmax,"  min=",newYmin
         print
        # end if ( sign_rotation != None ) 
        else :
         Angle_1 = 0.0
         print 
         print  "WARNING : No 1st re-alignement , thus NO rotation necessary !"
         print 
        # 
        # For testing the sensitivity  to possible small misalignements :
        # Information : Misalignement_with_DirL  should be a SMALL delta of angle expressed in degree) 
        Rot1_1x_without_Misalignement = cos(Angle_1)
        Rot1_1y_without_Misalignement = -sin(Angle_1)
        Rot1_1z_without_Misalignement = 0.0
        Rot1_2x_without_Misalignement = sin(Angle_1)
        Rot1_2y_without_Misalignement = cos(Angle_1)
        Rot1_2z_without_Misalignement = 0.0
        Rot1_3x_without_Misalignement = 0.0
        Rot1_3y_without_Misalignement = 0.0
        Rot1_3z_without_Misalignement = 1.0
        #
        Angle_1 += (Misalignement_with_DirL * pi/180)   # expressed in rd !
        Rot1_1x = cos(Angle_1)
        Rot1_1y = -sin(Angle_1)
        Rot1_1z = 0.0
        Rot1_2x = sin(Angle_1)
        Rot1_2y = cos(Angle_1)
        Rot1_2z = 0.0
        Rot1_3x = 0.0
        Rot1_3y = 0.0
        Rot1_3z = 1.0
        print
        print "For 1st re-alignement with possible SMALL misalignement with DirL :"
        print "apply a rotation with Angle (in degree)=",Angle_1*180/pi 
        print "Rot1 : Matrix_line No1 = ",Rot1_1x,"  ",Rot1_1y,"  ",Rot1_1z
        print "Rot1 : Matrix_line No2 = ",Rot1_2x,"  ",Rot1_2y,"  ",Rot1_2z
        print "Rot1 : Matrix_line No3 = ",Rot1_3x,"  ",Rot1_3y,"  ",Rot1_3z
        print "check :  Rot1_1x=",Rot1_1x,"   Rot1_1x_without_Misalignement=",Rot1_1x_without_Misalignement
        print "check :  Rot1_1y=",Rot1_1y,"   Rot1_1y_without_Misalignement=",Rot1_1y_without_Misalignement
        print "check :  Rot1_2x=",Rot1_2x,"   Rot1_2x_without_Misalignement=",Rot1_2x_without_Misalignement
        print "check :  Rot1_2y=",Rot1_2y,"   Rot1_2y_without_Misalignement=",Rot1_2y_without_Misalignement
        print
        if ( DependentRotations ) :
          new_XLmax = - 10000000000.0
          new_XLmin = + 10000000000.0
          new_XLTmax = - 10000000000.0
          new_XLTmin = + 10000000000.0
          new_Zmax = - 10000000000.0
          new_Zmin = + 10000000000.0
          output_GRIDsFile = open ('TransformedMesh_File_rot1_1overFreqlines.txt','w')
          input_GRIDsFile = open ('Nastran_GRIDS_in_PrincipalAxis_ALLlines.bdf','r')
          for l in range(NlinesGRID) :
           Line = input_GRIDsFile.readline()
           if ( ((l+1)/Freq*Freq)  ==  (l+1) ) :
             stringC = Line.split(",")
             GRID_ID = int(stringC[0])
             GM_x = float(stringC[1]) - new_pointG_x  # - newXmin
             GM_y = float(stringC[2]) - new_pointG_y  # - newYmin
             GM_z = float(stringC[3]) - new_pointG_z  # - newZmin 
             new_XL  = Rot1_1x*GM_x + Rot1_1y*GM_y # + Rot1_1z*GM_z
             new_XLT = Rot1_2x*GM_x + Rot1_2y*GM_y # + Rot1_2z*GM_z
             new_Z   = Rot1_3z*GM_z  #  + Rot1_3y*GM_y + Rot1_3x*GM_x 
             #  to restore the global position :
             new_XL  += new_pointG_x 
             new_XLT += new_pointG_y
             new_Z  += new_pointG_z 
             if   ( new_XL > new_XLmax ) :
                 new_XLmax = new_XL
             elif ( new_XL < new_XLmin ) :
                 new_XLmin = new_XL 
             if   ( new_XLT > new_XLTmax ) :
                 new_XLTmax = new_XLT
             elif ( new_XLT < new_XLTmin ) :
                 new_XLTmin = new_XLT 
             if   ( new_Z > new_Zmax ) :
                 new_Zmax = new_Z
             elif ( new_Z < new_Zmin ) :
                 new_Zmin = new_Z
             output_GRIDsFile.write("%8i,%14.7e,%14.7e,%14.7e \n" %(GRID_ID,new_XL,new_XLT,new_Z))
           # end if ( ((l+1)/Freq*Freq)  ==  (l+1) )
          # end for l in range(NlinesGRID) 
          output_GRIDsFile.close()
          input_GRIDsFile.close()
          print "IF rotation1, Z : max=",new_Zmax,"  min=",new_Zmin
          print "IF rotation1, XL : max=",new_XLmax,"  min=",new_XLmin
          print "IF rotation1, XLT : max=",new_XLTmax,"  min=",new_XLTmin
        # end if ( DependentRotations ) 
        # end for determining a first possible re-alignement 
        # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
        #
        # 3.2) For determining a second possible re-alignement :
        # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
        AngleIncr = 1.0  # expressed in degree   MAY BE TUNED !!!!!!!!!!!!!!!
        factor1 = AngleIncr *pi/180.  # expressed in rd !
        boolGoOn = True
        AngleIter = 1
        #  The 1st iter serves to determine whether it will be convenient to do 
        #  a clockwise or counter-clockwise right rotation :
        if ( boolGoOn ) :
          Angle_1 = factor1*AngleIter  # expressed in rd !
          Angle_2 = - Angle_1  
          if ( DependentRotations ) :
             TransformedGRIDs_1overFreq_file = open ('TransformedMesh_File_rot1_1overFreqlines.txt','r')
          else :
             TransformedGRIDs_1overFreq_file = open ('Nastran_GRIDS_in_PrincipalAxis_1overFreqlines.txt','r')
          prev_XLmax = - 10000000000.0
          prev_XLmin = + 10000000000.0
          prev_XLTmax = - 10000000000.0
          prev_XLTmin = + 10000000000.0
          prev_Zmax = - 10000000000.0
          prev_Zmin = + 10000000000.0
          Zmax_rot1 = - 10000000000.0
          Zmin_rot1 = + 10000000000.0
          XLTmax_rot1 = - 10000000000.0
          XLTmin_rot1 = + 10000000000.0
          Zmax_rot2 = - 10000000000.0
          Zmin_rot2 = + 10000000000.0
          XLTmax_rot2 = - 10000000000.0
          XLTmin_rot2 = + 10000000000.0
          new_XLmax = - 10000000000.0
          new_XLmin = + 10000000000.0
          for l in range(Nlines_1overFreq) :
            Line = TransformedGRIDs_1overFreq_file.readline()
            stringC = Line.split(",")
            XL = float(stringC[1])
            XLT = float(stringC[2])
            Z = float(stringC[3])
            if   ( XL > prev_XLmax ) :
                 prev_XLmax = XL
            elif ( XL < prev_XLmin ) :
                 prev_XLmin = XL 
            if   ( XLT > prev_XLTmax ) :
                 prev_XLTmax = XLT
            elif ( XLT < prev_XLTmin ) :
                 prev_XLTmin = XLT 
            if   ( Z > prev_Zmax ) :
                 prev_Zmax = Z
            elif ( Z < prev_Zmin ) :
                 prev_Zmin = Z
            #  modify the global position :
            XL  -= new_pointG_x # - newXmin
            XLT -= new_pointG_y # - newYmin
            Z   -= new_pointG_z # - newZmin
            # Performing the rotation_2 to deal with the 2nd re-alignement : 
            new_XL = XL
            XLT_rot1 = XLT*cos(Angle_1) - Z*sin(Angle_1)
            Z_rot1 = XLT*sin(Angle_1) + Z*cos(Angle_1)
            XLT_rot2 = XLT*cos(Angle_2) - Z*sin(Angle_2)
            Z_rot2 = XLT*sin(Angle_2) + Z*cos(Angle_2)
            # restore the global position :
            new_XL   +=  new_pointG_x
            XLT_rot1 +=  new_pointG_y
            XLT_rot2 +=  new_pointG_y
            Z_rot1   +=  new_pointG_z
            Z_rot2   +=  new_pointG_z
            if   ( new_XL > new_XLmax ) :
                 new_XLmax = new_XL
            elif ( new_XL < new_XLmin ) :
                 new_XLmin = new_XL
            if   ( XLT_rot1 > XLTmax_rot1 ) :
                 XLTmax_rot1 = XLT_rot1
            elif ( XLT_rot1 < XLTmin_rot1 ) :
                 XLTmin_rot1 = XLT_rot1
            if   ( Z_rot1 > Zmax_rot1 ) :
                 Zmax_rot1 = Z_rot1
            elif ( Z_rot1 < Zmin_rot1 ) :
                 Zmin_rot1 = Z_rot1
            if   ( XLT_rot2 > XLTmax_rot2 ) :
                 XLTmax_rot2 = XLT_rot2
            elif ( XLT_rot2 < XLTmin_rot2 ) :
                 XLTmin_rot2 = XLT_rot2
            if   ( Z_rot2 > Zmax_rot2 ) :
                 Zmax_rot2 = Z_rot2
            elif ( Z_rot2 < Zmin_rot2 ) :
                 Zmin_rot2 = Z_rot2
          # end for l in range(Nlines_1overFreq) 
          TransformedGRIDs_1overFreq_file.close()
          previous_VolBB = (prev_Zmax-prev_Zmin)*(prev_XLTmax-prev_XLTmin)*(prev_XLmax-prev_XLmin)
          VolBB_rot1 = (Zmax_rot1-Zmin_rot1)*(XLTmax_rot1-XLTmin_rot1)*(new_XLmax-new_XLmin)
          VolBB_rot2 = (Zmax_rot2-Zmin_rot2)*(XLTmax_rot2-XLTmin_rot2)*(new_XLmax-new_XLmin)
          if ( VolBB_rot1 < previous_VolBB ) :
              print "previous_VolBB=",previous_VolBB,"  VolBB_rot1=",VolBB_rot1
              print "perform a CLOCKWISE rotation to deal with 2nd re-alignement !"
              sign_rotation = +1
              previous_VolBB = VolBB_rot1
          elif ( VolBB_rot2 < previous_VolBB) :
              print "previous_VolBB=",previous_VolBB,"  VolBB_rot2=",VolBB_rot2
              print "perform a COUNTER-CLOCKWISE rotation to deal with 2nd re-alignement !"
              sign_rotation = -1
              previous_VolBB = VolBB_rot2
          else :
              print "NO rotation to deal with 2nd re-alignement !"
              boolGoOn = False
              sign_rotation = None 
        # end if ( boolGoOn ) 
        print
        if ( sign_rotation != None ) :
         #  From now on, go on with only the right rotation sign :
         factor1 = sign_rotation * AngleIncr *pi/180.0
         while boolGoOn :
          AngleIter += 1
          Angle_1 = factor1*AngleIter  # expressed in rd !
          if ( DependentRotations ) :
             TransformedGRIDs_1overFreq_file = open ('TransformedMesh_File_rot1_1overFreqlines.txt','r')
          else :
             TransformedGRIDs_1overFreq_file = open ('Nastran_GRIDS_in_PrincipalAxis_1overFreqlines.txt','r')
          new_XLmax = - 10000000000.0
          new_XLmin = + 10000000000.0
          new_XLTmax = - 10000000000.0
          new_XLTmin = + 10000000000.0
          new_Zmax = - 10000000000.0
          new_Zmin = + 10000000000.0
          for l in range(Nlines_1overFreq) :
            Line = TransformedGRIDs_1overFreq_file.readline()
            stringC = Line.split(",")
            XL = float(stringC[1])  - new_pointG_x # - newXmin
            XLT = float(stringC[2]) - new_pointG_y # - newYmin
            Z = float(stringC[3])   - new_pointG_z # - newZmin
            # Performing the rotation_2 to deal with the 2nd re-alignement : 
            new_XL =XL                                 
            new_XLT = XLT*cos(Angle_1) - Z*sin(Angle_1)
            new_Z = XLT*sin(Angle_1) + Z*cos(Angle_1) 
            # restore the global position :
            new_XL  +=  new_pointG_x
            new_XLT +=  new_pointG_y
            new_Z   +=  new_pointG_z
            if   ( new_XL > new_XLmax ) :
                 new_XLmax = new_XL
            elif ( new_XL < new_XLmin ) :
                 new_XLmin = new_XL 
            if   ( new_XLT > new_XLTmax ) :
                 new_XLTmax = new_XLT
            elif ( new_XLT < new_XLTmin ) :
                 new_XLTmin = new_XLT 
            if   ( new_Z > new_Zmax ) :
                 new_Zmax = new_Z
            elif ( new_Z < new_Zmin ) :
                 new_Zmin = new_Z
          # end for l in range(Nlines_1overFreq) 
          TransformedGRIDs_1overFreq_file.close()
          new_VolBB = (new_Zmax-new_Zmin)*(new_XLTmax-new_XLTmin)*(new_XLmax-new_XLmin)
          print "previous_VolBB=",previous_VolBB,"  new_VolBB=",new_VolBB
          if ( new_VolBB >= previous_VolBB) :
              boolGoOn = False
              print "END of rotation attempts to deal with 2nd re-alignement !"
          else :
              print "loop on rotation attempts to deal with 2nd re-alignement !"
              print "Angle_1 (in degree) = ",Angle_1*180/pi
              previous_VolBB = new_VolBB
         # end while boolGoOn 
         Angle_1 = factor1*(AngleIter-1) # expressed in rd !
         print 
         print "IF rotation2, Z : max=",new_Zmax,"  min=",new_Zmin
         print "IF rotation2, XL : max=",new_XLmax,"  min=",new_XLmin
         print "IF rotation2, XLT : max=",new_XLTmax,"  min=",new_XLTmin
        # end if ( sign_rotation != None ) 
        else :
         Angle_1 = 0.0
         print 
         print  "WARNING : No 2nd re-alignement , thus NO rotation necessary !"
        # 
        # For testing the sensitivity  to possible small misalignements :
        # Information : Misalignement_with_DirLT  should be a SMALL delta of angle expressed in degree) 
        Rot2_1x_without_Misalignement = 1.0
        Rot2_1y_without_Misalignement = 0.0
        Rot2_1z_without_Misalignement = 0.0
        Rot2_2x_without_Misalignement = 0.0
        Rot2_2y_without_Misalignement = cos(Angle_1)
        Rot2_2z_without_Misalignement = -sin(Angle_1)
        Rot2_3x_without_Misalignement = 0.0
        Rot2_3y_without_Misalignement = sin(Angle_1)
        Rot2_3z_without_Misalignement = cos(Angle_1)
        #
        Angle_1 += (Misalignement_with_DirLT * pi/180)   # expressed in rd !
        Rot2_1x = 1.0
        Rot2_1y = 0.0
        Rot2_1z = 0.0
        Rot2_2x = 0.0
        Rot2_2y = cos(Angle_1)
        Rot2_2z = -sin(Angle_1)
        Rot2_3x = 0.0
        Rot2_3y = sin(Angle_1)
        Rot2_3z = cos(Angle_1)
        print
        print "For 2nd re-alignement with possible SMALL misalignement with DirLT :"
        print "apply a rotation with Angle (in degree)=",Angle_1*180/pi 
        print "Rot2 : Matrix_line No1 = ",Rot2_1x,"  ",Rot2_1y,"  ",Rot2_1z
        print "Rot2 : Matrix_line No2 = ",Rot2_2x,"  ",Rot2_2y,"  ",Rot2_2z
        print "Rot2 : Matrix_line No3 = ",Rot2_3x,"  ",Rot2_3y,"  ",Rot2_3z
        print "check :  Rot2_2y=",Rot2_2y,"   Rot2_2y_without_Misalignement=",Rot2_2y_without_Misalignement
        print "check :  Rot2_2z=",Rot2_2z,"   Rot2_2z_without_Misalignement=",Rot2_2z_without_Misalignement
        # end for determining a second possible misalignement 
        # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
        #
        # Info : the composition of the two possible rotations in order to 
        # Info : deal with the two possible re-alignements is now performed :
        # 3.3) Obtention of the tho 3x3 matrices Rot1_ij*Rot2_ij => Mat_ij :
        #      NB : the possible simplifications are taken into account !
        Mat11 = Rot1_1x
        Mat12 = Rot1_1y*Rot2_2y
        Mat13 = Rot1_1y*Rot2_2z
        Mat21 = Rot1_2x
        Mat22 = Rot1_2y*Rot2_2y
        Mat23 = Rot1_2y*Rot2_2z
        Mat31 = 0.0
        Mat32 = Rot2_3y
        Mat33 = Rot2_3z
        # For testing the sensitivity  to possible small misalignements :
        # NB : the possible simplifications are taken into account !
        Mat11_without_Misalignement = Rot1_1x_without_Misalignement 
        Mat12_without_Misalignement = Rot1_1y_without_Misalignement*Rot2_2y_without_Misalignement
        Mat13_without_Misalignement = Rot1_1y_without_Misalignement*Rot2_2z_without_Misalignement
        Mat21_without_Misalignement = Rot1_2x_without_Misalignement
        Mat22_without_Misalignement = Rot1_2y_without_Misalignement*Rot2_2y_without_Misalignement
        Mat23_without_Misalignement = Rot1_2y_without_Misalignement*Rot2_2z_without_Misalignement
        Mat31_without_Misalignement = 0.0
        Mat32_without_Misalignement = Rot2_3y_without_Misalignement
        Mat33_without_Misalignement = Rot2_3z_without_Misalignement
        
        #
        # Info : global translation in order to get the final MESH into the positive octant of design space :
        # Info : actually, when Freq=1, new_XLmin, new_XLTmin and new_Zmin are OK for new_origine  
        #        IF no rotation necessary !
        # Info:  otherwise, one has to first determine the min values and then apply the translation !
        # ///////////////////////////////////////////////////////////////////////////////////////////////////
        if Freq == 1 :
           if ( new_XLmin ) :
             newXLorig = new_XLmin
             print ">>> Rotation(s) necessary to realigne the GRIDS with XL-XLT-Z axis : newXLorig<=new_XLmin"
           else :  # when no rotation necessary, new_XLmin is an empty string, so the test result is FALSE
             newXLorig = newXmin 
             print ">>> No realignement of the GRIDS with XL-XLT-Z axis : newXLorig<=newXmin"
           #
           if ( new_XLTmin ) :
             newXLTorig =  new_XLTmin
             print ">>> Rotation(s) necessary to realigne the GRIDS with XL-XLT-Z axis : newXLTorig<=new_XLTmin"
           else :  # when no rotation necessary, new_XLTmin is an empty string, so the test result is FALSE
             newXLTorig = newYmin 
             print ">>> No realignement of the GRIDS with XL-XLT-Z axis : newXLTorig<=newYmin"
           #
           if ( new_Zmin ) :
             newZorig = new_Zmin
             save_new_Zmin = new_Zmin
             print ">>> Rotation(s) necessary to realigne the GRIDS with XL-XLT-Z axis : newZorig<=new_Zmin"
           else :  # when no rotation necessary, new_Zmin is an empty string, so the test result is FALSE
             newZorig   = newZmin 
             save_new_Zmin = newZmin
             print ">>> No realignement of the GRIDS with XL-XLT-Z axis : newZorig<=newZmin"
           #
           if ( new_Zmax ) :
             save_new_Zmax = new_Zmax
           else :  # when no rotation necessary, new_Zmax is an empty string, so the test result is FALSE
             save_new_Zmax = newZmax
        #
        new_XLmax = - 10000000000.0
        new_XLmin = + 10000000000.0
        new_XLTmax = - 10000000000.0
        new_XLTmin = + 10000000000.0
        new_Zmax = - 10000000000.0
        new_Zmin = + 10000000000.0
        OUTER_GRID_No1 = None
        OUTER_GRID_No1_x = None
        OUTER_GRID_No1_y = None
        OUTER_GRID_No1_z = None
        OUTER_GRID_No2 = None
        OUTER_GRID_No2_x = None
        OUTER_GRID_No2_y = None
        OUTER_GRID_No2_z = None
        OUTER_GRID_No3 = None
        OUTER_GRID_No3_x = None
        OUTER_GRID_No3_y = None
        OUTER_GRID_No3_z = None
        #
        # 3.4) multiplication of the Mat_ij with the GRIDs yet projected in Principal Axis : 
        input_GRIDsFile = open ('Nastran_GRIDS_in_PrincipalAxis_ALLlines.bdf','r')
        output_GRIDsFile = open ('Nastran_TransformedGRIDs_File.bdf','w')
        output_GRIDsFile.write("BEGIN BULK \n")
        output_GRIDsFile.write("$.. \n")
        output_GRIDsFile.write("$..   NODES \n")
        output_GRIDsFile_without_Misalignement = open ('Nastran_TransformedGRIDs_File_without_Misalignement.bdf','w')
        output_GRIDsFile_without_Misalignement.write("BEGIN BULK \n")
        output_GRIDsFile_without_Misalignement.write("$.. \n")
        output_GRIDsFile_without_Misalignement.write("$..   NODES \n")
        if Freq == 1 :
        ##############
         subsetFileName = "subsetGRIDS_realigned_Mesh.txt"
         subsetFile = open (subsetFileName,'w')
         NlinesGRID_in_subsetFile = 0
         threshold_new_Z = (save_new_Zmax-save_new_Zmin)/10.0   + 0.0  # and not   + save_new_Zmin
         print "To remove LaterON : threshold_new_Z=",threshold_new_Z
         # INFO : This threshold will serve to select a subset of GRIDS which describe the lowest(%Z) part of the realigned mesh.
         # INFO : Within the obtained subset of GRIDS, one will search for OUTER_GRID_No1, OUTER_GRID_No2 & OUTER_GRID_No3.
         for l in range(NlinesGRID) :
           Line = input_GRIDsFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           GM_x = float(stringC[1]) - new_pointG_x 
           GM_y = float(stringC[2]) - new_pointG_y
           GM_z = float(stringC[3]) - new_pointG_z
           new_XL  = Mat11*GM_x + Mat12*GM_y + Mat13*GM_z
           new_XLT = Mat21*GM_x + Mat22*GM_y + Mat23*GM_z
           new_Z   = Mat31*GM_x + Mat32*GM_y + Mat33*GM_z
           new_XL_without_Misalignement  = Mat11_without_Misalignement*GM_x + Mat12_without_Misalignement*GM_y + Mat13_without_Misalignement*GM_z
           new_XLT_without_Misalignement = Mat21_without_Misalignement*GM_x + Mat22_without_Misalignement*GM_y + Mat23_without_Misalignement*GM_z
           new_Z_without_Misalignement   = Mat31_without_Misalignement*GM_x + Mat32_without_Misalignement*GM_y + Mat33_without_Misalignement*GM_z
           #  Global translation in order to get the final MESH into the positive octant of design space :
           new_XL  += new_pointG_x - newXLorig
           new_XLT += new_pointG_y - newXLTorig 
           new_Z   += new_pointG_z - newZorig 
           new_XL_without_Misalignement  += new_pointG_x - newXLorig
           new_XLT_without_Misalignement += new_pointG_y - newXLTorig
           new_Z_without_Misalignement   += new_pointG_z - newZorig

           #Changed by JZ to allow a loop over all points 
           subsetFile.write("%8i,%8.2f,%8.2f,%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
           NlinesGRID_in_subsetFile += 1
           #Changed by JZ

           if   ( new_XL > new_XLmax ) :
                 new_XLmax = new_XL
           elif ( new_XL < new_XLmin ) :
                 new_XLmin = new_XL 
                 # First attempt : OUTER_GRID_No1 = GRID_ID
                 # First attempt : OUTER_GRID_No1_x = new_XL
                 # First attempt : OUTER_GRID_No1_y = new_XLT
                 # First attempt : OUTER_GRID_No1_z = new_Z
           if   ( new_XLT > new_XLTmax ) :
                 new_XLTmax = new_XLT
           elif ( new_XLT < new_XLTmin ) :
                 new_XLTmin = new_XLT 
                 # First attempt : OUTER_GRID_No2 = GRID_ID
                 # First attempt : OUTER_GRID_No2_x = new_XL
                 # First attempt : OUTER_GRID_No2_y = new_XLT
                 # First attempt : OUTER_GRID_No2_z = new_Z
           if   ( new_Z > new_Zmax ) :
                 new_Zmax = new_Z
           elif ( new_Z < new_Zmin ) :
                 new_Zmin = new_Z
                 # First attempt : OUTER_GRID_No3 = GRID_ID
                 # First attempt : OUTER_GRID_No3_x = new_XL
                 # First attempt : OUTER_GRID_No3_y = new_XLT
                 # First attempt : OUTER_GRID_No3_z = new_Z
           #elif ( new_Z < threshold_new_Z ) :
           #      subsetFile.write("%8i,%8.2f,%8.2f,%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
           #      NlinesGRID_in_subsetFile += 1
           output_GRIDsFile.write("GRID    %8i        %8.2f%8.2f%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
           output_GRIDsFile_without_Misalignement.write("GRID    %8i        %8.2f%8.2f%8.2f  \n" %(GRID_ID,new_XL_without_Misalignement,new_XLT_without_Misalignement,new_Z_without_Misalignement))
           output_MeshFile_abaqus.write("%8i,%8.2f,%8.2f,%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
         # end for l in range(NlinesGRID) 

         # Let's determine now OUTER_GRID_No1, OUTER_GRID_No2 and OUTER_GRID_No3 :
         #First Boundary Condition: MinX,Miny,MinZ
         subsetFile = open (subsetFileName,'r')
         #Now we loop over all points to get the proper boundary conditions    
         current_distance = 0.0
         min_distance = +10000000000000.0
         for l in range(NlinesGRID_in_subsetFile) :
           Line = subsetFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           G_x = float(stringC[1])
           G_y = float(stringC[2])
           G_z = float(stringC[3])
           #calculate the distance to each mesh point and choose the one with minimum distance to lower left corner of bb
           current_distance = (((G_x-new_XLmin)**2) + ((G_y-new_XLTmin)**2) + ((G_z-new_Zmin)**2))
           if (current_distance < min_distance) :
              OUTER_GRID_No1 = GRID_ID
              OUTER_GRID_No1_x = G_x
              OUTER_GRID_No1_y = G_y
              OUTER_GRID_No1_z = G_z
              min_distance=current_distance
         subsetFile.close()
         #Second Boundary Condition: MaxX,MaxY,MinZ
         subsetFile = open (subsetFileName,'r')
         #Now we loop over all points to get the proper boundary conditions    
         current_distance = 0.0
         min_distance = +10000000000000.0
         for l in range(NlinesGRID_in_subsetFile) :
           Line = subsetFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           G_x = float(stringC[1])
           G_y = float(stringC[2])
           G_z = float(stringC[3])
           #calculate the distance to each mesh point and choose the one with minimum distance to lower left corner of bb
           current_distance = (((G_x-new_XLmax)**2) + ((G_y-new_XLTmax)**2) + ((G_z-new_Zmin)**2))
           if (current_distance < min_distance) :
              OUTER_GRID_No2 = GRID_ID
              OUTER_GRID_No2_x = G_x
              OUTER_GRID_No2_y = G_y
              OUTER_GRID_No2_z = G_z
              min_distance=current_distance
         subsetFile.close()
         #Third Boundary Condition: MinX,MaxY,MinZ
         subsetFile = open (subsetFileName,'r')
         #Now we loop over all points to get the proper boundary conditions    
         current_distance = 0.0
         min_distance = +10000000000000.0
         for l in range(NlinesGRID_in_subsetFile) :
           Line = subsetFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           G_x = float(stringC[1])
           G_y = float(stringC[2])
           G_z = float(stringC[3])
           #calculate the distance to each mesh point and choose the one with minimum distance to lower left corner of bb
           current_distance = (((G_x-new_XLmin)**2) + ((G_y-new_XLTmax)**2) + ((G_z-new_Zmin)**2))
           if (current_distance < min_distance) :
              OUTER_GRID_No3 = GRID_ID
              OUTER_GRID_No3_x = G_x
              OUTER_GRID_No3_y = G_y
              OUTER_GRID_No3_z = G_z
              min_distance=current_distance
         subsetFile.close()
         #
        else : # Freq > 1 :
        ###################
         GRID_IDtemp = []
         new_XLtemp = []
         new_XLTtemp = []
         new_Ztemp = []
         new_XLtemp_without_Misalignement = []
         new_XLTtemp_without_Misalignement = []
         new_Ztemp_without_Misalignement = []
         # check print 
         # check print ".... Mat11=",Mat11,"  without_Misalignement=",Mat11_without_Misalignement
         # check print ".... Mat21=",Mat21,"  without_Misalignement=",Mat21_without_Misalignement
         # check print ".... Mat31=",Mat31,"  without_Misalignement=",Mat31_without_Misalignement
         # check print ".... Mat12=",Mat12,"  without_Misalignement=",Mat12_without_Misalignement
         # check print ".... Mat22=",Mat22,"  without_Misalignement=",Mat22_without_Misalignement
         # check print ".... Mat32=",Mat32,"  without_Misalignement=",Mat32_without_Misalignement
         # check print ".... Mat13=",Mat13,"  without_Misalignement=",Mat13_without_Misalignement
         # check print ".... Mat23=",Mat23,"  without_Misalignement=",Mat23_without_Misalignement
         # check print ".... Mat33=",Mat33,"  without_Misalignement=",Mat33_without_Misalignement
         # check print 
         for l in range(NlinesGRID) :
           Line = input_GRIDsFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           GRID_IDtemp.append(GRID_ID)
           GM_x = float(stringC[1]) - new_pointG_x 
           GM_y = float(stringC[2]) - new_pointG_y
           GM_z = float(stringC[3]) - new_pointG_z
           new_XL  = Mat11*GM_x + Mat12*GM_y + Mat13*GM_z    + new_pointG_x
           new_XLT = Mat21*GM_x + Mat22*GM_y + Mat23*GM_z    + new_pointG_y
           new_Z   = Mat31*GM_x + Mat32*GM_y + Mat33*GM_z    + new_pointG_z
           new_XLtemp.append(new_XL)
           new_XLTtemp.append(new_XLT)
           new_Ztemp.append(new_Z)
           new_XL_without_Misalignement  = Mat11_without_Misalignement*GM_x \
                                         + Mat12_without_Misalignement*GM_y \
                                         + Mat13_without_Misalignement*GM_z \
                                         + new_pointG_x
           new_XLT_without_Misalignement = Mat21_without_Misalignement*GM_x \
                                         + Mat22_without_Misalignement*GM_y \
                                         + Mat23_without_Misalignement*GM_z \
                                         + new_pointG_y
           new_Z_without_Misalignement   = Mat31_without_Misalignement*GM_x \
                                         + Mat32_without_Misalignement*GM_y \
                                         + Mat33_without_Misalignement*GM_z \
                                         + new_pointG_z
           new_XLtemp_without_Misalignement.append(new_XL_without_Misalignement)
           new_XLTtemp_without_Misalignement.append(new_XLT_without_Misalignement)
           new_Ztemp_without_Misalignement.append(new_Z_without_Misalignement)
           if   ( new_XL > new_XLmax ) :
                 new_XLmax = new_XL
           elif ( new_XL < new_XLmin ) :
                 new_XLmin = new_XL 
                 # First attempt : OUTER_GRID_No1 = GRID_ID
                 # First attempt : OUTER_GRID_No1_x = new_XL
                 # First attempt : OUTER_GRID_No1_y = new_XLT
                 # First attempt : OUTER_GRID_No1_z = new_Z
           if   ( new_XLT > new_XLTmax ) :
                 new_XLTmax = new_XLT
           elif ( new_XLT < new_XLTmin ) :
                 new_XLTmin = new_XLT 
                 # First attempt : OUTER_GRID_No2 = GRID_ID
                 # First attempt : OUTER_GRID_No2_x = new_XL
                 # First attempt : OUTER_GRID_No2_y = new_XLT
                 # First attempt : OUTER_GRID_No2_z = new_Z
           if   ( new_Z > new_Zmax ) :
                 new_Zmax = new_Z
           elif ( new_Z < new_Zmin ) :
                 new_Zmin = new_Z
                 # First attempt : OUTER_GRID_No3 = GRID_ID
                 # First attempt : OUTER_GRID_No3_x = new_XL
                 # First attempt : OUTER_GRID_No3_y = new_XLT
                 # First attempt : OUTER_GRID_No3_z = new_Z
         # end for l in range(NlinesGRID) 
         # Global translation in order to get the final MESH into the positive octant of design space :
         newXLorig  = new_XLmin
         newXLTorig = new_XLTmin
         newZorig   = new_Zmin
	 new_XLmin -= newXLorig 
	 new_XLTmin -= newXLTorig
	 new_Zmin -= newZorig
	 new_XLmax -=  newXLorig
	 new_XLTmax -= newXLTorig
	 new_Zmax -= newZorig
         # First attempt : OUTER_GRID_No1_x -= newXLorig
         # First attempt : OUTER_GRID_No2_x -= newXLorig
         # First attempt : OUTER_GRID_No3_x -= newXLorig
         # First attempt : OUTER_GRID_No1_y -= newXLTorig
         # First attempt : OUTER_GRID_No2_y -= newXLTorig
         # First attempt : OUTER_GRID_No3_y -= newXLTorig
         # First attempt : OUTER_GRID_No1_z -= newZorig
         # First attempt : OUTER_GRID_No2_z -= newZorig
         # First attempt : OUTER_GRID_No3_z -= newZorig
         # 
         # check print
         # check print "new_XLtemp[1]=",new_XLtemp[1],"  without_Misalignement=",new_XLtemp_without_Misalignement[1]
         # check print "new_XLtemp[NlinesGRID-1]=",new_XLtemp[NlinesGRID-1],"  without_Misalignement=",new_XLtemp_without_Misalignement[NlinesGRID-1]
         # check print
         #
         subsetFileName = "subsetGRIDS_realigned_Mesh.txt"
         subsetFile = open (subsetFileName,'w')
         NlinesGRID_in_subsetFile = 0
         threshold_new_Z = (new_Zmax-new_Zmin)/10.0   + 0.0  # or  + new_Zmin which is equal to 0.0
         # INFO : This threshold will serve to select a subset of GRIDS which describe the lowest(%Z) part of the realigned mesh.
         # INFO : Within the obtained subset of GRIDS, one will search for OUTER_GRID_No1, OUTER_GRID_No2 and OUTER_GRID_No3.
         # 
         for l in range(NlinesGRID) :
           GRID_ID = GRID_IDtemp[l]
           new_XL  = new_XLtemp[l] - newXLorig 
           new_XLT = new_XLTtemp[l] - newXLTorig
           new_Z   = new_Ztemp[l] - newZorig 
           subsetFile.write("%8i,%8.2f,%8.2f,%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
           NlinesGRID_in_subsetFile += 1
           new_XL_without_Misalignement  = new_XLtemp_without_Misalignement[l] - newXLorig 
           new_XLT_without_Misalignement = new_XLTtemp_without_Misalignement[l] - newXLTorig
           new_Z_without_Misalignement   = new_Ztemp_without_Misalignement[l] - newZorig 
           output_GRIDsFile.write("GRID    %8i        %8.2f%8.2f%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
           output_GRIDsFile_without_Misalignement.write("GRID    %8i        %8.2f%8.2f%8.2f  \n" %(GRID_ID,new_XL_without_Misalignement,new_XLT_without_Misalignement,new_Z_without_Misalignement))
           output_MeshFile_abaqus.write("%8i,%8.2f,%8.2f,%8.2f  \n" %(GRID_ID,new_XL,new_XLT,new_Z))
         # end for l in range(NlinesGRID) 
         subsetFile.close()
         #
         # Let's determine now OUTER_GRID_No1, OUTER_GRID_No2 and OUTER_GRID_No3 :

         #First Boundary Condition: MinX,Miny,MinZ
         subsetFile = open (subsetFileName,'r')
         #Now we loop over all points to get the proper boundary conditions    
         current_distance = 0.0
         min_distance = +10000000000000.0
         for l in range(NlinesGRID_in_subsetFile) :
           Line = subsetFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           G_x = float(stringC[1])
           G_y = float(stringC[2])
           G_z = float(stringC[3])
           #calculate the distance to each mesh point and choose the one with minimum distance to lower left corner of bb
           current_distance = (((G_x-new_XLmin)**2) + ((G_y-new_XLTmin)**2) + ((G_z-new_Zmin)**2))
           if (current_distance < min_distance) :
              OUTER_GRID_No1 = GRID_ID
              OUTER_GRID_No1_x = G_x
              OUTER_GRID_No1_y = G_y
              OUTER_GRID_No1_z = G_z
              min_distance=current_distance
         subsetFile.close()
         #Second Boundary Condition: MaxX,MaxY,MinZ
         subsetFile = open (subsetFileName,'r')
         #Now we loop over all points to get the proper boundary conditions    
         current_distance = 0.0
         min_distance = +10000000000000.0
         for l in range(NlinesGRID_in_subsetFile) :
           Line = subsetFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           G_x = float(stringC[1])
           G_y = float(stringC[2])
           G_z = float(stringC[3])
           #calculate the distance to each mesh point and choose the one with minimum distance to lower left corner of bb
           current_distance = (((G_x-new_XLmax)**2) + ((G_y-new_XLTmax)**2) + ((G_z-new_Zmin)**2))
           if (current_distance < min_distance) :
              OUTER_GRID_No2 = GRID_ID
              OUTER_GRID_No2_x = G_x
              OUTER_GRID_No2_y = G_y
              OUTER_GRID_No2_z = G_z
              min_distance=current_distance
         subsetFile.close()
         #Third Boundary Condition: MinX,MaxY,MinZ
         subsetFile = open (subsetFileName,'r')
         #Now we loop over all points to get the proper boundary conditions    
         current_distance = 0.0
         min_distance = +10000000000000.0
         for l in range(NlinesGRID_in_subsetFile) :
           Line = subsetFile.readline()
           stringC = Line.split(",")
           GRID_ID = int(stringC[0])
           G_x = float(stringC[1])
           G_y = float(stringC[2])
           G_z = float(stringC[3])
           #calculate the distance to each mesh point and choose the one with minimum distance to lower left corner of bb
           current_distance = (((G_x-new_XLmin)**2) + ((G_y-new_XLTmax)**2) + ((G_z-new_Zmin)**2))
           if (current_distance < min_distance) :
              OUTER_GRID_No3 = GRID_ID
              OUTER_GRID_No3_x = G_x
              OUTER_GRID_No3_y = G_y
              OUTER_GRID_No3_z = G_z
              min_distance=current_distance
         subsetFile.close()
 
         #
        # end else Freq > 1 
        ###################
        input_GRIDsFile.close()
        #
        # info : "output_GRIDsFile.close()" was a statement done here previously ; but now  
        # info :  it is done after having dealt with ELEMENTS !!
        # info :  Likewise, "output_GRIDsFile_without_Misalignement.close()" will be done 
        # info :  after having dealt with ELEMENTS !! 
        #
        print "with rotations 1+2, Z : max=",new_Zmax,"  min=",new_Zmin
        print "with rotations 1+2, XL : max=",new_XLmax,"  min=",new_XLmin
        print "with rotations 1+2, XLT : max=",new_XLTmax,"  min=",new_XLTmin
        #
        # 4/ There may be some subsequent lines with comments, thus starting with "$" :
        CommentLines = 0
        boolGoOn = True
        while boolGoOn :
            if filename1_CurrentLine.startswith("$") :
               CommentLines += 1
               filename1_CurrentLine = input_MeshFile.readline()
            else:
              boolGoOn = False
        print CommentLines ," CommentLines have been found before ELEMENTs section !"
        #
        # 4/ Writing the Elements list in abaqus format :
        #    Here are only considered General 3D solids (not for Heat  transfer) !!
        #    Writing also the Elements list in Nastran format in the file *Nastran_ELEMENTs.txt* 
        ELEMENTs_file = open ('Nastran_ELEMENTs.txt','w')
        ELEMENTs_file.write("$.. \n")
        ELEMENTs_file.write("$..   ELEMENTS \n")
        concatenation = " , "  #  MAY BE TUNED for convenience !!!!!
        output_MeshFile_abaqus.write("*Element, TYPE=%s, ELSET=Eall \n" %ElementType)
        ELEMENTSs_lineNo1  = GRIDs_lineNo1+GRIDs_lineEND+CommentLines
        print "ELEMENTs section starts at lineNo ",ELEMENTSs_lineNo1,"  filename1_CurrentLine=",filename1_CurrentLine
        ELEMENTs_lineEND = 0
        boolGoOn = True
        output_GRIDsFile.write("$.. \n")
        output_GRIDsFile.write("$..   ELEMENTS \n")
        output_GRIDsFile_without_Misalignement.write("$.. \n")
        output_GRIDsFile_without_Misalignement.write("$..   ELEMENTS \n")
        while boolGoOn :
            if (filename1_CurrentLine.startswith("ENDDATA")) :  # what means the END of ELEMENTs lines
              boolGoOn = False
              output_GRIDsFile.write("$.. \n")
              output_GRIDsFile.write("ENDDATA \n")
              output_GRIDsFile_without_Misalignement.write("$.. \n")
              output_GRIDsFile_without_Misalignement.write("ENDDATA \n")
              ELEMENTs_file.write("$.. \n")
              ELEMENTs_file.write("ENDDATA \n")
            elif (filename1_CurrentLine.startswith("$")) :  # what means a comment line
              filename1_CurrentLine = input_MeshFile.readline()  #  in order to loop for the while !
            else :   # a true ELEMENTs line
              ELEMENTs_lineEND += 1
              #-verif  print "filename1_CurrentLine =",filename1_CurrentLine 
              Element_stringC = []
              ELEM_ID =  filename1_CurrentLine[8:16]
              Element_stringC.append(ELEM_ID)
              if ( "C3D4" in ElementType ) :   # 4-node linear tetrahedral element 
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:5])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 if  ( ALL_ELEMENTS_VOL_bool ) :
                   # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                   # Estimate ML6edges (= the Mean Length of the 6 edges of the 4-NODES element) 
                   # and use the forulae for a regular TETRAEDRE : VOL=ML6edges**3 * SQRT(2)/12 :
                   # ============================================================================
                   # At first, search the rank of the GRIDline corresponding to NODE1,NODE2,NODE3 & NODE4
                   for q in range(NlinesGRID) :
                    if ( s_GRID_ID[q] == int(NODE1) ) : 
                      Nrank_NODE1 = q 
                    elif ( s_GRID_ID[q] == int(NODE2) ) : 
                      Nrank_NODE2 = q 
                    elif ( s_GRID_ID[q] == int(NODE3) ) : 
                      Nrank_NODE3 = q 
                    elif ( s_GRID_ID[q] == int(NODE4) ) : 
                      Nrank_NODE4 = q 
                   # Now calculate the length of each of the 6 edges :
                   dx2=(s_GRID_X[Nrank_NODE1]-s_GRID_X[Nrank_NODE2])**2
                   dy2=(s_GRID_Y[Nrank_NODE1]-s_GRID_Y[Nrank_NODE2])**2
                   dz2=(s_GRID_Z[Nrank_NODE1]-s_GRID_Z[Nrank_NODE2])**2
                   length_edgeG1G2 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE1]-s_GRID_X[Nrank_NODE3])**2
                   dy2=(s_GRID_Y[Nrank_NODE1]-s_GRID_Y[Nrank_NODE3])**2
                   dz2=(s_GRID_Z[Nrank_NODE1]-s_GRID_Z[Nrank_NODE3])**2
                   length_edgeG1G3 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE2]-s_GRID_X[Nrank_NODE3])**2
                   dy2=(s_GRID_Y[Nrank_NODE2]-s_GRID_Y[Nrank_NODE3])**2
                   dz2=(s_GRID_Z[Nrank_NODE2]-s_GRID_Z[Nrank_NODE3])**2
                   length_edgeG2G3 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE1]-s_GRID_X[Nrank_NODE4])**2
                   dy2=(s_GRID_Y[Nrank_NODE1]-s_GRID_Y[Nrank_NODE4])**2
                   dz2=(s_GRID_Z[Nrank_NODE1]-s_GRID_Z[Nrank_NODE4])**2
                   length_edgeG1G4 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE2]-s_GRID_X[Nrank_NODE4])**2
                   dy2=(s_GRID_Y[Nrank_NODE2]-s_GRID_Y[Nrank_NODE4])**2
                   dz2=(s_GRID_Z[Nrank_NODE2]-s_GRID_Z[Nrank_NODE4])**2
                   length_edgeG2G4 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE3]-s_GRID_X[Nrank_NODE4])**2
                   dy2=(s_GRID_Y[Nrank_NODE3]-s_GRID_Y[Nrank_NODE4])**2
                   dz2=(s_GRID_Z[Nrank_NODE3]-s_GRID_Z[Nrank_NODE4])**2
                   length_edgeG3G4 = sqrt(dx2+dy2+dz2)
                   # Finally average the lengths of the 6 edges and apply the formulae : 
                   ML6edges = (length_edgeG1G2+length_edgeG1G3+length_edgeG2G3+\
                               length_edgeG1G4+length_edgeG2G4+length_edgeG3G4)/6
                   VolumeELEM = ML6edges**3 * sqrt(2.0)/12.0
                   ALL_ELEMENTS_VOL += VolumeELEM
                 # else : that subtask may be carried out by means of NASTRAN (or CATIA) 
                 #        outside the current process !
              elif ( "C3D6" in ElementType ) : # 6-node linear triangular prism element
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:7])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                 # WARNING  that case has not been coded yet !  To be done later.
              elif ( "C3D8" in ElementType ) : # 3D 8-node linear isoparametric element
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE7 to NODE8 in the next line
                 NODE7 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE7)
                 NODE8 =  filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE8)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:9])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                 # WARNING  that case has not been coded yet !  To be done later.
              elif ( "C3D8R" in ElementType ) : # C3D8 element with reduced integration
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE7 to NODE8 in the next line
                 NODE7 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE7)
                 NODE8 =  filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE8)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:9])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                 # WARNING  that case has not been coded yet !  To be done later.
              elif ( "C3D10" in ElementType) : # 10-node quadratic tetrahedral element
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE7 to NODE10  in the next line
                 NODE7 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE7)
                 NODE8 =  filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE8)
                 NODE9 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE9)
                 NODE10 = filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE10)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:11])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 if  ( ALL_ELEMENTS_VOL_bool ) :
                   # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                   # Estimate ML6edges (= the Mean Length of the 6 edges of the 4-NODES element) 
                   # and use the forulae for a regular TETRAEDRE : VOL=ML6edges**3 * SQRT(2)/12 :
                   # ============================================================================
                   # At first, search the rank of the GRIDline corresponding to NODE1,NODE2,NODE3 & NODE4
                   for q in range(NlinesGRID) :
                    if ( s_GRID_ID[q] == int(NODE1) ) : 
                      Nrank_NODE1 = q 
                    elif ( s_GRID_ID[q] == int(NODE2) ) : 
                      Nrank_NODE2 = q 
                    elif ( s_GRID_ID[q] == int(NODE3) ) : 
                      Nrank_NODE3 = q 
                    elif ( s_GRID_ID[q] == int(NODE4) ) : 
                      Nrank_NODE4 = q 
                   # Now calculate the length of each of the 6 edges :
                   dx2=(s_GRID_X[Nrank_NODE1]-s_GRID_X[Nrank_NODE2])**2
                   dy2=(s_GRID_Y[Nrank_NODE1]-s_GRID_Y[Nrank_NODE2])**2
                   dz2=(s_GRID_Z[Nrank_NODE1]-s_GRID_Z[Nrank_NODE2])**2
                   length_edgeG1G2 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE1]-s_GRID_X[Nrank_NODE3])**2
                   dy2=(s_GRID_Y[Nrank_NODE1]-s_GRID_Y[Nrank_NODE3])**2
                   dz2=(s_GRID_Z[Nrank_NODE1]-s_GRID_Z[Nrank_NODE3])**2
                   length_edgeG1G3 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE2]-s_GRID_X[Nrank_NODE3])**2
                   dy2=(s_GRID_Y[Nrank_NODE2]-s_GRID_Y[Nrank_NODE3])**2
                   dz2=(s_GRID_Z[Nrank_NODE2]-s_GRID_Z[Nrank_NODE3])**2
                   length_edgeG2G3 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE1]-s_GRID_X[Nrank_NODE4])**2
                   dy2=(s_GRID_Y[Nrank_NODE1]-s_GRID_Y[Nrank_NODE4])**2
                   dz2=(s_GRID_Z[Nrank_NODE1]-s_GRID_Z[Nrank_NODE4])**2
                   length_edgeG1G4 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE2]-s_GRID_X[Nrank_NODE4])**2
                   dy2=(s_GRID_Y[Nrank_NODE2]-s_GRID_Y[Nrank_NODE4])**2
                   dz2=(s_GRID_Z[Nrank_NODE2]-s_GRID_Z[Nrank_NODE4])**2
                   length_edgeG2G4 = sqrt(dx2+dy2+dz2)
                   dx2=(s_GRID_X[Nrank_NODE3]-s_GRID_X[Nrank_NODE4])**2
                   dy2=(s_GRID_Y[Nrank_NODE3]-s_GRID_Y[Nrank_NODE4])**2
                   dz2=(s_GRID_Z[Nrank_NODE3]-s_GRID_Z[Nrank_NODE4])**2
                   length_edgeG3G4 = sqrt(dx2+dy2+dz2)
                   # Finally average the lengths of the 6 edges and apply the formulae : 
                   ML6edges = (length_edgeG1G2+length_edgeG1G3+length_edgeG2G3+\
                               length_edgeG1G4+length_edgeG2G4+length_edgeG3G4)/6
                   VolumeELEM = ML6edges**3 * sqrt(2.0)/12.0
                   ALL_ELEMENTS_VOL += VolumeELEM
                 # else : that subtask may be carried out by means of NASTRAN (or CATIA) 
                 #        outside the current process !
              elif ( "C3D15" in ElementType ) : # 15-node quadratic triangular prism element
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE7 to NODE14  in the next line
                 NODE7 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE7)
                 NODE8 =  filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE8)
                 NODE9 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE9)
                 NODE10 = filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE10)
                 NODE11 = filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE11)
                 NODE12 = filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE12)
                 NODE13 = filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE13)
                 NODE14 = filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE14)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE15  in the next line
                 NODE15 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE15)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:16])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                 # WARNING  that case has not been coded yet !  To be done later.
              elif ( "C3D20" in ElementType ) : # 3D 20-node quadratic isoparametric element
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE7 to NODE14  in the next line
                 NODE7 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE7)
                 NODE8 =  filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE8)
                 NODE9 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE9)
                 NODE10 = filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE10)
                 NODE11 = filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE11)
                 NODE12 = filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE12)
                 NODE13 = filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE13)
                 NODE14 = filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE14)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE15 to NODE20  in the next line
                 NODE15 = filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE15)
                 NODE16 = filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE16)
                 NODE17 = filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE17)
                 NODE18 = filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE18)
                 NODE19 = filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE19)
                 NODE20 = filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE20)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:21])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                 # WARNING  that case has not been coded yet !  To be done later.
              elif ( "C3D20R" in ElementType ) : # C3D20 element with reduced integration
                 NODE1 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE1)
                 NODE2 =  filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE2)
                 NODE3 =  filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE3)
                 NODE4 =  filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE4)
                 NODE5 =  filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE5)
                 NODE6 =  filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE6)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE7 to NODE14  in the next line
                 NODE7 =  filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE7)
                 NODE8 =  filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE8)
                 NODE9 =  filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE9)
                 NODE10 = filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE10)
                 NODE11 = filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE11)
                 NODE12 = filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE12)
                 NODE13 = filename1_CurrentLine[56:64]
                 Element_stringC.append(NODE13)
                 NODE14 = filename1_CurrentLine[64:72]
                 Element_stringC.append(NODE14)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 filename1_CurrentLine = input_MeshFile.readline() # since NODE15 to NODE20  in the next line
                 NODE15 = filename1_CurrentLine[8:16]
                 Element_stringC.append(NODE15)
                 NODE16 = filename1_CurrentLine[16:24]
                 Element_stringC.append(NODE16)
                 NODE17 = filename1_CurrentLine[24:32]
                 Element_stringC.append(NODE17)
                 NODE18 = filename1_CurrentLine[32:40]
                 Element_stringC.append(NODE18)
                 NODE19 = filename1_CurrentLine[40:48]
                 Element_stringC.append(NODE19)
                 NODE20 = filename1_CurrentLine[48:56]
                 Element_stringC.append(NODE20)
                 concatedElement_stringC = concatenation.join(Element_stringC[0:21])
                 output_MeshFile_abaqus.write("%s \n" %concatedElement_stringC)
                 ELEMENT_line = filename1_CurrentLine.strip()
                 ELEMENTs_file.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile.write("%s \n" %ELEMENT_line)
                 output_GRIDsFile_without_Misalignement.write("%s \n" %ELEMENT_line)
                 # Calculation of the volume of the ELEMENT under consideration and accumulate it. 
                 # WARNING  that case has not been coded yet !  To be done later.
              else :
                 print "The type of 3D-solids Finite Element is ",ElementType
                 print "Case not programmed yet !   Check that matter ...."
                 print "Exit forced from module *mesh_bdf2inp.py* !"
                 sys.exit()
              #
              filename1_CurrentLine = input_MeshFile.readline()  #  in order to loop for the while !
        print "ELEMENTs section ends at lineNo ",ELEMENTSs_lineNo1+ELEMENTs_lineEND,"  filename1_CurrentLine=",filename1_CurrentLine
        print "Thus  ",ELEMENTs_lineEND,"  ELEMENTs lines  have been found !"
        #
        print
        print "verification : last line read = ",filename1_CurrentLine
        print
        X_lengthBB = Xmax-Xmin
        print "Previously : Xmin=",Xmin,"  &  Xmax=",Xmax,"  : X_lengthBB=",X_lengthBB
        print
        Y_lengthBB = Ymax-Ymin
        print "Previously : Ymin=",Ymin,"  &  Ymax=",Ymax,"  : Y_lengthBB=",Y_lengthBB
        print
        Z_lengthBB = Zmax-Zmin
        print "Previously : Zmin=",Zmin,"  &  Zmax=",Zmax,"  : Z_lengthBB=",Z_lengthBB
        print "Previously : Volume_BB = ",X_lengthBB*Y_lengthBB*Z_lengthBB
        print
        print
        original_deltaXL = new_XLmax-new_XLmin
        print "MinimalBB : Xmin=",new_XLmin,"  &  Xmax=",new_XLmax,"  : original_deltaXL=",original_deltaXL
        print
        original_deltaXLT = new_XLTmax-new_XLTmin
        print "MinimalBB : Ymin=",new_XLTmin,"  &  Ymax=",new_XLTmax,"  : original_deltaXLT=",original_deltaXLT
        print
        original_deltaZ = new_Zmax - new_Zmin
        print "MinimalBB : Zmin=",new_Zmin,"  &  Zmax=",new_Zmax,"  : original_deltaZ=",original_deltaZ
        print "MinimalBB : Volume_BB = ",original_deltaXL*original_deltaXLT*original_deltaZ
        print
        #
        #
        input_MeshFile.close()
        output_MeshFile_abaqus.close()
        # info : "output_GRIDsFile.close()" was a statement done before previously ; but now  
        # info :  it is done after having dealt with ELEMENTS !!
        output_GRIDsFile.close()
        output_GRIDsFile_without_Misalignement.close()
        #
        if  ALL_ELEMENTS_VOL != 0.0 :
           print ""
           print "VOLUME of ALL ELEMENTS is ",ALL_ELEMENTS_VOL
           print ""
        else : 
           print ""
           print "INFORMATION : calculation of ALL_ELEMENTS_VOLUME  has not been done ! "
           print "Either the calculation of ALL_ELEMENTS_VOLUME has been switched off : ALL_ELEMENTS_VOL_bool = False !"
           print "Or the calculation of the volume of an element has not been programmed yet !   Check that matter ...."
           print "verify the type of 3D-solids Finite Element which is ",ElementType
           print "WARNING Exit is NOT forced from module *mesh_bdf2inp.py* , since it is not essential !"
           print ""
        #
	return\
        OUTER_GRID_No1,OUTER_GRID_No1_x,OUTER_GRID_No1_y,OUTER_GRID_No1_z,\
        OUTER_GRID_No2,OUTER_GRID_No2_x,OUTER_GRID_No2_y,OUTER_GRID_No2_z,\
        OUTER_GRID_No3,OUTER_GRID_No3_x,OUTER_GRID_No3_y,OUTER_GRID_No3_z,\
        original_deltaXL, original_deltaXLT, original_deltaZ, ALL_ELEMENTS_VOL
