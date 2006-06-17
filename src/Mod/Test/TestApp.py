# FreeCAD Part module  
# (c) 2001 Juergen Riegel
#
# Part design module

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                       *   
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU Lesser General Public License (LGPL)    *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        * 
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#*   Juergen Riegel 2002                                                   *
#***************************************************************************/

import FreeCAD, os, unittest


#---------------------------------------------------------------------------
# define the functions to test the FreeCAD base code
#---------------------------------------------------------------------------


def All():
    suite = unittest.TestSuite()
    suite.addTest(unittest.defaultTestLoader.loadTestsFromName("Document") )
    suite.addTest(unittest.defaultTestLoader.loadTestsFromName("Base") )
    return suite

    
def TestText(s):
    s = unittest.defaultTestLoader.loadTestsFromName(s)
    r = unittest.TextTestRunner()
    r.run(s)

def Test(s):
    TestText(s)
    

def TestGui(s):
    import QtUnitGui
    QtUnitGui.setTest(s)
#    import unittestgui
#    import sys
#    import Tkinter
#    import tkMessageBox
#    import traceback
#    import string
#    root = Tkinter.Tk()
#    root.title("FreeCAD unit tests")
#    runner = unittestgui.TkTestRunner(root, s)
#    root.protocol('WM_DELETE_WINDOW', root.quit)
#    root.mainloop()
#    root.destroy()


    


        

