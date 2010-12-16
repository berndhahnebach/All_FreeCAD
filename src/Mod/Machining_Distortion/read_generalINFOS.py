#
def read_generalINFOS() :
    #
    # Variables generales
    import sys
    #
    # 1/ Find the informations concerning the WorkSpaceName  
    #    where to carry out the Machining Distorsion job . 
    #    The FOLDERS where required files should have been disposed  will be named 
    #    FOLDER1_path=WorkSpaceName + FOLDER1_name=="FOLDER_CAT_PARTs_pointG_PrincipalAxis"
    #    FOLDER2_path=WorkSpaceName + FOLDER2_name=="FOLDER_Nastran_originalMESHes"
    WorkSpaceName = None
    file = open ('input_generalINFOS.txt','r')
    boolGoOn = True
    line = file.readline()
    while boolGoOn :
       if (line.startswith("end")):
         boolGoOn = False
       else:
          line = file.readline()
          found_WorkSpaceName = line.find("WorkSpaceName")
          if ( found_WorkSpaceName != -1 ) :
             lineWithoutReturnEntry = line.rstrip()
             stringC=lineWithoutReturnEntry.split("=")
             WorkSpaceName = str(stringC[1])
    # end  while boolGoOn
    file.close()
    # print "The WorkSpace where the Machining Distorsion job will be carried out is ",WorkSpaceName 

    # 2/ Find the informations concerning the softwares used in the sequel :
    nameFEMsoftware = None
    Path_to_FEMsoftware = None
    Path_to_GMESHsoftware = None
    file = open ('input_generalINFOS.txt','r')
    boolGoOn = True
    line = file.readline()
    while boolGoOn :
       if (line.startswith("end")):
         boolGoOn = False
       else:
          line = file.readline()
          found_nameFEMsoftware = line.find("nameFEMsoftware")
          if ( found_nameFEMsoftware != -1 ) :
             lineWithoutReturnEntry = line.rstrip()
             stringC=lineWithoutReturnEntry.split("=")
             nameFEMsoftware = str(stringC[1])

          found_Path_to_FEMsoftware = line.find("Path_to_FEMsoftware")
          if ( found_Path_to_FEMsoftware != -1 ):
             lineWithoutReturnEntry = line.rstrip()
             stringC=lineWithoutReturnEntry.split("=")
             Path_to_FEMsoftware = str(stringC[1])

          found_Path_to_GMESHsoftware = line.find("Path_to_GMESHsoftware")
          if ( found_Path_to_GMESHsoftware != -1 ) :
             lineWithoutReturnEntry = line.rstrip()
             stringC=lineWithoutReturnEntry.split("=")
             Path_to_GMESHsoftware = str(stringC[1])
    # end  while boolGoOn
    file.close()
    # print "The FEM-Calculation Software will will be used is ",nameFEMsoftware
    #
    # 3/ Find the informations concerning the MATERIAL properties :
    YoungModulus = None
    PoissonCoeff = None
    file = open ('input_generalINFOS.txt','r')
    boolGoOn = True
    line = file.readline()
    while boolGoOn :
       if (line.startswith("end")):
         boolGoOn = False
       else:
          line = file.readline()
          found_YoungModulus = line.find("YoungModulus")
          if ( found_YoungModulus != -1 ) :
             lineWithoutReturnEntry = line.rstrip()
             stringC=lineWithoutReturnEntry.split("=")
             print "stringC=",stringC
             YoungModulus = float(stringC[1])

          found_PoissonCoeff = line.find("PoissonCoeff")
          if ( found_PoissonCoeff != -1 ):
             lineWithoutReturnEntry = line.rstrip()
             stringC=lineWithoutReturnEntry.split("=")
             PoissonCoeff = float(stringC[1])
    # end  while boolGoOn
    file.close()
    if ( YoungModulus == None ) | ( PoissonCoeff == None ) :
       print "CAUTION : MATERIAL properties have not been specified !"
       print "Exit forced from module *read_generalINFOS.py*"
       print 
       sys.exit() 
    #
    # 4/ Find the informations concerning the calculation of ALL_ELEMENTS_VOLUME within the current process :
    ALL_ELEMENTS_VOL_bool = []   #    NB:an empty string is False => default value 
    file = open ('input_generalINFOS.txt','r')
    boolGoOn = True
    line = file.readline()
    while boolGoOn :
       if (line.startswith("end")):
         boolGoOn = False
       else:
          line = file.readline()
          found_ALL_ELEMENTS_VOLUME = line.find("ALL_ELEMENTS_VOL_bool")
          if ( found_ALL_ELEMENTS_VOLUME != -1 ) :
             stringC=line.split("=")
             ALL_ELEMENTS_VOL_bool = stringC[1]
    # end  while boolGoOn
    file.close()
    if ( ALL_ELEMENTS_VOL_bool ) :
       print 
       print "The calculation of ALL_ELEMENTS_VOLUME will be done within the current process !"
       print 
    else : 
       print 
       print "The calculation of ALL_ELEMENTS_VOLUME MUST be done OUTSIDE the current process !"
       print 
    #
    # 5/ Find the informations concerning the calculation of ALL_ELEMENTS_VOLUME within the current process :
    Misalignement_with_Dir1 = 0.0     #   by default
    Misalignement_with_Dir2 = 0.0    #   by default
    file = open ('input_generalINFOS.txt','r')
    boolGoOn = True
    line = file.readline()
    while boolGoOn :
       if (line.startswith("end")):
         boolGoOn = False
       else:
          line = file.readline()
          found_Misalignement_with_Dir1 = line.find("Misalignement_with_Dir1")
          if ( found_Misalignement_with_Dir1 != -1 ) :
             stringC=line.split("=")
             Misalignement_with_Dir1 = float(stringC[1])
          found_Misalignement_with_Dir2 = line.find("Misalignement_with_Dir2")
          if ( found_Misalignement_with_Dir2 != -1 ) :
             stringC=line.split("=")
             Misalignement_with_Dir2 = float(stringC[1])
    # end  while boolGoOn
    file.close()
    print 
    print "Tested Misalignements: with_Dir1 = ",Misalignement_with_Dir1,"degree  &&  with_Dir2=",Misalignement_with_Dir2,"degree"
    print 
    
    
    # For Information BUT useless :
    # Find the informations concerning the type of Finite Element
    #    Here are only considered General 3D solids (not for Heat  transfer) !
    #    The FE type should be n principle among the following possibilities : 
    #    *    C3D4    (4-node linear tetrahedral element)
    #    *    C3D6    (6-node linear triangular prism element)
    #    *    C3D8    (3D 8-node linear isoparametric element)
    #    *    C3D8R   (the C3D8 element with reduced integration)
    #    *    C3D10   (10-node quadratic tetrahedral element)
    #    *    C3D15   (15-node quadratic triangular prism  element)
    #    *    C3D20   (3D 20-node quadratic isoparametric element)
    #    *    C3D20R  (the C3D20 element with reduced integration)
    #
    #    Mind the fact that these types are valid for CALCULIX software !
    #    ( for ABAQUS, they should be similar :  DC3D4 instead of C3D4, etc ... )
    #
    #ElementType = None
    #file = open ('input_generalINFOS.txt','r')
    #boolGoOn = True
    #line = file.readline()
    #while boolGoOn :
    #   if (line.startswith("end")):
    #     boolGoOn = False
    #   else:
    #      line = file.readline()
    #      found_ElementType = line.find("ElementType")
    #      if ( found_ElementType != -1 ) :
    #         lineWithoutReturnEntry = line.rstrip()
    #         stringC=lineWithoutReturnEntry.split("=")
    #         print "stringC=",stringC
    #         ElementType = stringC[1]
    #
    #file.close()
    #if ( ElementType == None ) :
    #   print "CAUTION : the type of FE has not been specified !"
    #   print "Exit forced from module *read_generalINFOS.py*"
    #   print 
    #   sys.exit() 
    #
    return WorkSpaceName,nameFEMsoftware,Path_to_FEMsoftware,Path_to_GMESHsoftware,\
           YoungModulus,PoissonCoeff,ALL_ELEMENTS_VOL_bool,\
           Misalignement_with_Dir1,Misalignement_with_Dir2
