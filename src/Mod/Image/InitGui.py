# Image gui init module
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



class ImageWorkbench ( Workbench ):
	"Image workbench object"
	def Activate(self):
		# load the module
		try:
			Log ('Loading GUI of Image module...')
			import ImageGui
		except:
			Err('Cannot load ImageGui')
			raise
	def GetClassName(self):
		return "ImageGui::Workbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *colors[]={\n"
			"\"16 16 134 2\",\n"
			"\"Qt c None\",\n"
			"\".# c #000000\",\n"
			"\"#y c #000000\",\n"
			"\".L c #000000\",\n"
			"\".e c #000000\",\n"
			"\"#X c #000000\",\n"
			"\"#M c #000000\",\n"
			"\"#z c #000000\",\n"
			"\"#L c #120000\",\n"
			"\".Y c #000000\",\n"
			"\".d c #0c0c00\",\n"
			"\".K c #140a00\",\n"
			"\"ad c #120900\",\n"
			"\".a c #1b0909\",\n"
			"\"ab c #0f0800\",\n"
			"\".A c #231106\",\n"
			"\"ac c #1b0b00\",\n"
			"\".c c #291a0f\",\n"
			"\".b c #2e2012\",\n"
			"\".X c #311f09\",\n"
			"\"#l c #362104\",\n"
			"\"#Y c #241000\",\n"
			"\"## c #2b1900\",\n"
			"\".l c #402300\",\n"
			"\".J c #3f2611\",\n"
			"\".M c #372000\",\n"
			"\"#7 c #3b1f00\",\n"
			"\"#. c #503511\",\n"
			"\".s c #51361e\",\n"
			"\"#6 c #542d00\",\n"
			"\"#N c #462700\",\n"
			"\".f c #603805\",\n"
			"\"#m c #4d2b00\",\n"
			"\"aa c #693900\",\n"
			"\"#A c #583000\",\n"
			"\".k c #71553d\",\n"
			"\".B c #6b4100\",\n"
			"\".t c #754800\",\n"
			"\".G c #764d27\",\n"
			"\"#x c #94651a\",\n"
			"\".W c #946b35\",\n"
			"\"#8 c #8e5100\",\n"
			"\".I c #a07749\",\n"
			"\"#W c #a5660d\",\n"
			"\"#Z c #945200\",\n"
			"\".R c #9f6932\",\n"
			"\".Z c #9d5d00\",\n"
			"\"#K c #ba801a\",\n"
			"\".j c #c4aa92\",\n"
			"\".V c #cea05c\",\n"
			"\"a# c #c67700\",\n"
			"\".F c #c69869\",\n"
			"\".g c #cda881\",\n"
			"\"#9 c #c57700\",\n"
			"\"#O c #b87700\",\n"
			"\".9 c #d4a558\",\n"
			"\".z c #dab68c\",\n"
			"\"#k c #c99c51\",\n"
			"\"#a c #d47c00\",\n"
			"\"a. c #d58300\",\n"
			"\".i c #e6d2bf\",\n"
			"\".m c #e88400\",\n"
			"\"#B c #d88300\",\n"
			"\".h c #ead8c7\",\n"
			"\"#n c #e88600\",\n"
			"\".S c #e8b877\",\n"
			"\".N c #e98600\",\n"
			"\"#0 c #e98d00\",\n"
			"\".U c #efc27c\",\n"
			"\".Q c #f0bd7e\",\n"
			"\".H c #ecc28c\",\n"
			"\"#5 c #f3a204\",\n"
			"\".r c #f2d9bb\",\n"
			"\".x c #f3d3b0\",\n"
			"\".y c #f4d6b1\",\n"
			"\".3 c #fcc478\",\n"
			"\".4 c #fdc878\",\n"
			"\"#D c #0ccd06\",\n"
			"\"#p c #13950d\",\n"
			"\"#v c #4547cc\",\n"
			"\"#u c #5e589f\",\n"
			"\"#C c #62a200\",\n"
			"\"#q c #69af39\",\n"
			"\"#i c #6e6ba1\",\n"
			"\"#I c #7a668f\",\n"
			"\"#P c #80ac00\",\n"
			"\"#j c #9084a1\",\n"
			"\"#E c #91831e\",\n"
			"\"#d c #95a24e\",\n"
			"\"#Q c #a08500\",\n"
			"\"#h c #a28d84\",\n"
			"\"#F c #a62f2f\",\n"
			"\"#t c #a8745f\",\n"
			"\"#w c #ae9381\",\n"
			"\"#J c #af8e6c\",\n"
			"\"#R c #b21e07\",\n"
			"\"#H c #b56e49\",\n"
			"\"#G c #b93932\",\n"
			"\"#o c #bd8900\",\n"
			"\"#s c #c77d4b\",\n"
			"\"#S c #cc0d12\",\n"
			"\"#T c #da2d17\",\n"
			"\"#2 c #da5100\",\n"
			"\"#3 c #db3b01\",\n"
			"\"#c c #de8f0e\",\n"
			"\"#e c #e0bb65\",\n"
			"\"#1 c #ea8300\",\n"
			"\"#r c #ebb04b\",\n"
			"\"#U c #eda11a\",\n"
			"\".8 c #edc882\",\n"
			"\"#4 c #ee9603\",\n"
			"\".7 c #f3cc83\",\n"
			"\"#g c #fcc664\",\n"
			"\".C c #fe8c00\",\n"
			"\".O c #ff8500\",\n"
			"\".D c #ff890a\",\n"
			"\".u c #ff8a00\",\n"
			"\".1 c #ff8c05\",\n"
			"\".0 c #ff8d00\",\n"
			"\"#b c #ff8e00\",\n"
			"\".v c #ff9f38\",\n"
			"\".P c #ffa336\",\n"
			"\".2 c #ffb352\",\n"
			"\"#V c #ffbf1c\",\n"
			"\"#f c #ffc563\",\n"
			"\".n c #ffc683\",\n"
			"\".E c #ffc885\",\n"
			"\".5 c #ffcd7c\",\n"
			"\".6 c #ffd17d\",\n"
			"\".T c #ffd894\",\n"
			"\".w c #ffe1bf\",\n"
			"\".p c #fff3e2\",\n"
			"\".o c #fff3e4\",\n"
			"\".q c #fff6e4\",\n"
			"\"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt\",\n"
			"\"QtQt.#.a.b.c.d.#QtQtQtQtQtQtQtQt\",\n"
			"\"Qt.e.f.g.h.i.j.k.dQtQtQtQtQtQtQt\",\n"
			"\"Qt.l.m.n.o.p.q.r.sQtQtQtQtQtQtQt\",\n"
			"\"Qt.t.u.v.w.x.y.z.AQtQtQtQtQtQtQt\",\n"
			"\"Qt.B.C.D.E.F.G.H.I.J.K.LQtQtQtQt\",\n"
			"\"Qt.M.N.O.P.Q.R.S.T.U.V.W.X.#QtQt\",\n"
			"\"Qt.Y.Z.0.1.2.3.4.5.6.7.8.9#..#Qt\",\n"
			"\"QtQt###a#b#c#d#e#f#g#h#i#j#k#lQt\",\n"
			"\"QtQt.L#m#n#o#p#q#r#s#t#u#v#w#x#y\",\n"
			"\"QtQtQt#z#A#B#C#D#E#F#G#H#I#J#K#L\",\n"
			"\"QtQtQtQt#M#N#O#P#Q#R#S#T#U#V#W#X\",\n"
			"\"QtQtQtQtQt#y#Y#Z#0#1#2#3#4#5#6Qt\",\n"
			"\"QtQtQtQtQtQtQt.Y#7#8#9a.a#aa#zQt\",\n"
			"\"QtQtQtQtQtQtQtQtQt#zabacad#yQtQt\",\n"
			"\"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt\"};\n"]

Gui.addWorkbenchHandler("Image",ImageWorkbench())

# Append the open handler
FreeCAD.EndingAdd("Image formats (*.bmp *.jpg *.png *.xpm)","ImageGui")
