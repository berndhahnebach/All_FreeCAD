import os,sys,string
import FCFileTools

i=0
for arg in sys.argv:
    i=i+1
    if (i < len(sys.argv)):
        # this only appears if you use a DOS console or if you use a shell then there is no ui-File in this directory
        if (sys.argv[i] == "*.ui"):
            sys.stdout.write("Wrong parameter: no ui file found")
            sys.exit()
        
        # if this file does not exist check the next argument
        if not os.path.isfile(sys.argv[i]):
            sys.stdout.write("Cannot find file " + sys.argv[i] + "\n")
            continue
            
        # input file name
        file = open(sys.argv[i])
        
        # file without ".ui" extension
        inputFile  = sys.argv[i]
        inputFile  = inputFile[:-3]
        
        #make a copy of the original file
        FCFileTools.cpfile(sys.argv[i],inputFile+".old")

        # read the original file...        
        lines = file.readlines()
        
        # ... and close it immediately
        file.close()
        
        sys.stdout.write("Converting " + sys.argv[i] + "... ")
        
        a=0
        for line in lines:
            # replace special strings
            a=a+1
            line2 = line
            # if succesful go 4 lines back (this should be the correct line)
            if (string.find(line2, '_pref</cstring>') != -1):
                line3 = lines[a-4]
                if (string.find(line3, '<class>Q') != -1):
                    lines[a-4] = string.replace(line3,'<class>Q','<class>FC')

        # write the changes into the original file  
        out = open(sys.argv[i],"w");
        for line in lines:
            # output 
            out.write(line)
        
        out.close()
        
        sys.stdout.write("done.\n")
        
        # now run the uic (ui compiler)
        sys.stdout.write("Make source/header files... ")
        os.popen("uic " + inputFile + ".ui -o " + inputFile + ".h")
        os.popen("uic " + inputFile + ".ui -impl " + inputFile + ".h -o " + inputFile + ".cpp")
        sys.stdout.write("done.\n")
