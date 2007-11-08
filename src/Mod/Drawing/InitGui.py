# Drawing gui init module
# (c) 2003 Juergen Riegel
#
# Gathering all the information to start FreeCAD
# This is the second one of three init scripts, the third one
# runs when the gui is up

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                        
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
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



class DrawingWorkbench ( Workbench ):
	"Drawing workbench object"
	def Activate(self):
		# load the module
		try:
			Log ('Loading GUI of Drawing module...')
			import DrawingGui
		except:
			Err('Cannot load DrawingGui')
			raise
	def GetClassName(self):
		return "DrawingGui::Workbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *colors[]={\n"
			"\"16 16 49 1\",\n"
			"\"Qt c None\",\n"
			"\".	c #B0B0B0\",\n"
			"\"+	c #C8C8C8\",\n"
			"\"@	c #CACACA\",\n"
			"\"#	c #CBCBCB\",\n"
			"\"$	c #CDCDCD\",\n"
			"\"%	c #D0D0D0\",\n"
			"\"&	c #D1D1D1\",\n"
			"\"*	c #D2D2D2\",\n"
			"\"=	c #D3D3D3\",\n"
			"\"-	c #D4D4D4\",\n"
			"\";	c #D7D7D7\",\n"
			"\">	c #D8D8D8\",\n"
			"\",	c #E6E6E6\",\n"
			"\"'	c #E7E7E7\",\n"
			"\")	c #E5E5E5\",\n"
			"\"!	c #E0E0E0\",\n"
			"\"~	c #E4E4E4\",\n"
			"\"{	c #DEDEDE\",\n"
			"\"]	c #E1E1E1\",\n"
			"\"^	c #DADADA\",\n"
			"\"/	c #CCCCCC\",\n"
			"\"(	c #EAEAEA\",\n"
			"\"_	c #EBEBEB\",\n"
			"\":	c #E9E9E9\",\n"
			"\"<	c #E8E8E8\",\n"
			"\"[	c #E2E2E2\",\n"
			"\"}	c #DDDDDD\",\n"
			"\"|	c #ECECEC\",\n"
			"\"1	c #DBDBDB\",\n"
			"\"2	c #EEEEEE\",\n"
			"\"3	c #EDEDED\",\n"
			"\"4	c #E3E3E3\",\n"
			"\"5	c #F0F0F0\",\n"
			"\"6	c #F1F1F1\",\n"
			"\"7	c #EFEFEF\",\n"
			"\"8	c #F2F2F2\",\n"
			"\"9	c #F3F3F3\",\n"
			"\"0	c #D6D6D6\",\n"
			"\"a	c #F4F4F4\",\n"
			"\"b	c #F5F5F5\",\n"
			"\"c	c #F7F7F7\",\n"
			"\"d	c #F6F6F6\",\n"
			"\"e	c #F8F8F8\",\n"
			"\"f	c #F9F9F9\",\n"
			"\"g	c #CFCFCF\",\n"
			"\"h	c #B3B3B3\",\n"
			"\"i	c #CECECE\",\n"
			"\"j	c #BBBBBB\",\n"
			"\"                \",\n"
			"\"                \",\n"
			"\" .+@#$%&*=-;>>$ \",\n"
			"\" #,'')!)~{!]!{^ \",\n"
			"\" /(___(::<',~[} \",\n"
			"\" /||||_(:<')~[1 \",\n"
			"\" /22223|_(<')4^ \",\n"
			"\" $5665723_(<,~; \",\n"
			"\" $89986523_:')0 \",\n"
			"\" /abba9673|(<)- \",\n"
			"\" $bccda852|(<,* \",\n"
			"\" $defcb852|(<,% \",\n"
			"\" /bccda852|(<,g \",\n"
			"\" hggggiiigiiiij \",\n"
			"\"                \",\n"
			"\"                \"};\n"]
        
Gui.addWorkbench("Drawing",DrawingWorkbench())

# Append the open handler
FreeCAD.EndingAdd("Image formats (*.svg *.svgz)","DrawingGui")
