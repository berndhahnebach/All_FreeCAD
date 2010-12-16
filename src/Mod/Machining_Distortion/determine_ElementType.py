def determine_ElementType() :
   #
   #    Find the informations concerning the type of Finite Element
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
   #    NB  another way could have been to read this info from file 'input_generalINFOS.txt' 
   #    BUT the use of the current module is prefered as long as the end-user does not need 
   #    to know such an information !
   #    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   #
   typeELEMENT = None
   NODE_IDs_FirstLine = 0
   NODE_IDs_SecondLine = 0
   NODE_IDs_ThirdLine = 0
   NODE_IDs_ELEMENTspecif = None
   EL_found_in_tail_NastranMESHfile = 0
   tailMESHfile = open ("tail_Nastran_originalMeshFile.bdf", 'r')
   boolGoOn = True
   tailMESHfile_CurrentLine = tailMESHfile.readline()
   while boolGoOn :
    if ( tailMESHfile_CurrentLine.startswith("ENDDATA") ) : 
     boolGoOn = False
    elif ( tailMESHfile_CurrentLine.startswith("$") ) : 
     tailMESHfile_CurrentLine = tailMESHfile.readline()
    elif ( tailMESHfile_CurrentLine.startswith("+") ) : 
     # INFO  Continuation if there is a "+" at the 73th position on the current line 
     # INFO  and                      a "+" at the 1st position on the subsequent line 
     lengthLine = len(tailMESHfile_CurrentLine)
     lengthLine -= 8  #  the first 8 digit indicate the continuation statement
     NODE_IDs_SecondLine = int(lengthLine/8)
     tailMESHfile_CurrentLine = tailMESHfile.readline()
     if ( (NODE_IDs_SecondLine > 8) & (tailMESHfile_CurrentLine.startswith("+")) ) : 
     # that means Continuation on a third line; for such a case :
       NODE_IDs_SecondLine = 8
       # verif :  print "____ NODE_IDs_SecondLine=",NODE_IDs_SecondLine
       lengthLine = len(tailMESHfile_CurrentLine)
       lengthLine -= 8  #  the first 8 digit indicate the continuation statement
       NODE_IDs_ThirdLine = int(lengthLine/8)
       # verif : print "____ NODE_IDs_ThirdLine=",NODE_IDs_ThirdLine
       tailMESHfile_CurrentLine = tailMESHfile.readline()
    else :
      EL_found_in_tail_NastranMESHfile += 1
      typeELEMENT = tailMESHfile_CurrentLine[0:8]
      lengthLine = len(tailMESHfile_CurrentLine)
      if (lengthLine > 73) :  #  actually, the 73th character is the "\n" at the end of any line
        lengthLine -= 24  #  the first 24 characters do not contain Node identifiers 
        lengthLine -= 8  #  the last 8 characters indicates the continuation  
        NODE_IDs_FirstLine = int(lengthLine/8)
        # verif : print "a) ",NODE_IDs_FirstLine,"  Node identifiers in CurrentLine = ",tailMESHfile_CurrentLine
        tailMESHfile_CurrentLine = tailMESHfile.readline()
      else :
        lengthLine -= 24  #  the first 24 characters do not contain Node identifiers 
        NODE_IDs_FirstLine = int(lengthLine/8)
        NODE_IDs_SecondLine = 0  #  because there is NO continuation in this case !
        NODE_IDs_ThirdLine = 0   #  because there is NO continuation in this case !
        # verif : print "b) ",NODE_IDs_FirstLine,"  Node identifiers in CurrentLine = ",tailMESHfile_CurrentLine
        tailMESHfile_CurrentLine = tailMESHfile.readline()
   # end while boolGoOn 
   tailMESHfile.close()
   NODE_IDs_ELEMENTspecif = NODE_IDs_FirstLine + NODE_IDs_SecondLine  + NODE_IDs_ThirdLine
   # 
   # INFO   The expected results to be transfered to the rest of the whole process are 
   # INFO   "ElementType" and "NODE_IDs_ELEMENTspecif"
   if ( ("CTETRA" in typeELEMENT)  & (NODE_IDs_ELEMENTspecif == 4) ) :
      ElementType = "C3D4" 
   elif ( ("CTETRA" in typeELEMENT)  & (NODE_IDs_ELEMENTspecif == 10) ) :
      ElementType = "C3D10" 
   elif ( NODE_IDs_ELEMENTspecif == 6 ) :
      ElementType = "C3D6" 
   elif ( NODE_IDs_ELEMENTspecif == 8 ) :
      ElementType = "C3D8" 
   elif ( NODE_IDs_ELEMENTspecif == 15 ) :
      ElementType = "C3D15" 
   elif ( NODE_IDs_ELEMENTspecif == 20 ) :
      ElementType = "C3D20" 
   # verif : print
   # verif : print "Informations extracted from *tail_Nastran_MeshFile.bdf* : "
   # verif : print NODE_IDs_SecondLine,"  NODE_IDs in Continuation SecondLine  (if  any) !"
   # verif : print NODE_IDs_ThirdLine,"  NODE_IDs in Continuation ThirdLine  (if  any) !"
   # verif : print "EL_found_in_tail_NastranMESHfile = ",EL_found_in_tail_NastranMESHfile
   # verif : print "typeELEMENT found in *tail_Nastran_MeshFile.bdf*  = ",typeELEMENT
   # verif : print "NODE_IDs_ELEMENTspecif = ",NODE_IDs_ELEMENTspecif,"  ;  NODE_IDs_FirstLine=",NODE_IDs_FirstLine 
   # verif : print "ElementType=",ElementType,"   NODE_IDs_ELEMENTspecif = ",NODE_IDs_ELEMENTspecif,"  ;  NODE_IDs_FirstLine=",NODE_IDs_FirstLine 
   #
   return ElementType,NODE_IDs_ELEMENTspecif
