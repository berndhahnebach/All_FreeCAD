
#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009 Yorik van Havre <yorik@gmx.fr>                     *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

__title__="FreeCAD Draft Workbench - GUI part"
__author__ = "Yorik van Havre <yorik@uncreated.net>"
__url__ = ["http://free-cad.sourceforge.net"]

'''
This is the GUI part of the Draft module.
Report to Draft.py for info
'''

import FreeCAD, FreeCADGui, os

try:
	from PyQt4 import QtCore,QtGui,QtSvg	
except:
	FreeCAD.Console.PrintMessage("Error: Python-qt4 package must be installed on your system to use the Draft module.")

def draftPath():
	"returns the current Draft module path"
	path1 = FreeCAD.ConfigGet("AppHomePath") + "Mod/Draft"
	path2 = FreeCAD.ConfigGet("UserAppData") + "Mod/Draft"
	if os.path.exists(path2): return path2
	else: return path1

def findicons():
	"loads the icon file"
	filepath = draftPath() + os.sep + "icons.svg"
	iconmap = QtGui.QPixmap()
	if not iconmap.load(filepath):
		# If loading by plug-in fails do it the conventional way
		file=QtCore.QFile(filepath)
		if not file.open(QtCore.QFile.ReadOnly):
			raise Exception("Cannot open file %s" % (filepath))
		ba=file.readAll()
		render=QtSvg.QSvgRenderer(ba)
		image = QtGui.QImage(render.viewBox().size(),QtGui.QImage.Format_ARGB32_Premultiplied)
		image.fill(0x00000000)
		painter=QtGui.QPainter(image)
		render.render(painter)
		painter.end()
		iconmap = QtGui.QPixmap.fromImage(image)
	return iconmap
	
def getMainWindow():
	"returns the main window"
	# using QtGui.qApp.activeWindow() isn't very reliable because if another
	# widget than the mainwindow is active (e.g. a dialog) the wrong widget is
	# returned
	toplevel = QtGui.qApp.topLevelWidgets()
	for i in toplevel:
		if i.metaObject().className() == "Gui::MainWindow":
			return i
	raise Exception("No main window found")

def translate(context,text):
        "convenience function for Qt translator"
        return QtGui.QApplication.translate(context, text, None, QtGui.QApplication.UnicodeUTF8)

#---------------------------------------------------------------------------
# Customized widgets
#---------------------------------------------------------------------------

class DraftDockWidget(QtGui.QWidget):
	"custom Widget that emits a resized() signal when resized"
	def __init__(self,parent = None):
		QtGui.QDockWidget.__init__(self,parent)
	def resizeEvent(self,event):
		self.emit(QtCore.SIGNAL("resized()"))

class DraftLineEdit(QtGui.QLineEdit):
	"custom QLineEdit widget that has the power to catch Escape keypress"
	def __init__(self, parent=None):
		QtGui.QLineEdit.__init__(self, parent)
	def keyPressEvent(self, event):
		if event.key() == QtCore.Qt.Key_Escape:
			self.emit(QtCore.SIGNAL("escaped()"))
		elif event.key() == QtCore.Qt.Key_Up:
			self.emit(QtCore.SIGNAL("up()"))
		elif event.key() == QtCore.Qt.Key_Down:
			self.emit(QtCore.SIGNAL("down()"))
		elif (event.key() == QtCore.Qt.Key_Z) and QtCore.Qt.ControlModifier:
			self.emit(QtCore.SIGNAL("undo()"))
		else:
			QtGui.QLineEdit.keyPressEvent(self, event)

class toolBar:
	"main draft Toolbar"
	def __init__(self):
		
		class Ui_draftToolbar(object):

#---------------------------------------------------------------------------
# General UI setup
#---------------------------------------------------------------------------

			def setupUi(self, draftToolbar):
				self.params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
				paramcolor = self.params.GetUnsigned("color")>>8
				paramlinewidth = self.params.GetInt("linewidth")
				paramconstr = self.params.GetUnsigned("constructioncolor")>>8
				icons = findicons()
				self.constrMode = False
				draftToolbar.setObjectName("draftToolbar")
				self.state = None
				self.textbuffer = []
				self.draftToolbar = draftToolbar
                                self.crossedViews = []

				def _pushButton (name, layout, hide=True, icon=None, width=60):
					button = QtGui.QPushButton(draftToolbar)
					#button.setGeometry(geometry)
					button.setObjectName(name)
                                        button.setMaximumSize(QtCore.QSize(width,22))
					if hide: button.hide()
                                        if icon:
                                                button.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(icon[0],icon[1],64,64))))
                                                button.setIconSize(QtCore.QSize(16, 16))
                                        layout.addWidget(button)
					return button

                                def _label (name, layout, hide=True):
                                        label = QtGui.QLabel(draftToolbar)
                                        label.setObjectName(name)
                                        if hide: label.hide()
                                        layout.addWidget(label)
                                        return label

                                def _lineedit (name, layout, hide=True, width=None):
                                        lineedit = DraftLineEdit(draftToolbar)
                                        lineedit.setObjectName(name)
                                        if hide: lineedit.hide()
                                        if not width: width = 800
                                        lineedit.setMaximumSize(QtCore.QSize(width,18))
                                        layout.addWidget(lineedit)
                                        return lineedit

                                self.layout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight,draftToolbar)
                                self.layout.setDirection(QtGui.QBoxLayout.LeftToRight)
                                self.layout.setObjectName("layout")

                                self.promptlabel = _label("promptlabel",self.layout)
				self.cmdlabel = _label("cmdlabel",self.layout, hide=False)
				boldtxt = QtGui.QFont()
				boldtxt.setWeight(75)
				boldtxt.setBold(True)
				self.cmdlabel.setFont(boldtxt)

				self.labelx = _label("labelx", self.layout)
				self.xValue = _lineedit("xValue",self.layout,width=60)
				self.xValue.setText("0.00")
				self.labely = _label("labely",self.layout)
				self.yValue = _lineedit("yValue",self.layout,width=60)
				self.yValue.setText("0.00")
				self.labelz = _label("labelz",self.layout)
				self.zValue = _lineedit("zValue",self.layout,width=60)
				self.zValue.setText("0.00")

				self.offsetLabel = _label("offsetlabel",self.layout)
				self.offsetValue = _lineedit("offsetValue",self.layout,width=60)
				self.offsetValue.setText("0.00")

                                self.labelRadius = _label("labelRadius",self.layout)
				self.radiusValue = _lineedit("radiusValue",self.layout,width=60)
				self.radiusValue.setText("0.00")

				self.isRelative = QtGui.QCheckBox(draftToolbar)
				self.isRelative.setChecked(True)
				self.isRelative.setObjectName("isRelative")
                                self.layout.addWidget(self.isRelative)
				self.isRelative.hide()

                                self.hasFill = QtGui.QCheckBox(draftToolbar)
				self.hasFill.setChecked(True)
				self.hasFill.setObjectName("hasFill")
                                self.hasFill.setChecked(self.params.GetBool("fillmode"))
                                self.layout.addWidget(self.hasFill)
				self.hasFill.hide()
                                
				self.undoButton = _pushButton("undoButton",self.layout,icon=(64,64))
				self.finishButton = _pushButton("finishButton",self.layout,icon=(448,64))
				self.closeButton = _pushButton("closeButton",self.layout,icon=(512,64))
                                
				self.xyButton = _pushButton("xyButton",self.layout)
				self.xzButton = _pushButton("xzButton",self.layout)
				self.yzButton = _pushButton("yzButton",self.layout)
				self.currentViewButton = _pushButton("view",self.layout)
				self.resetPlaneButton = _pushButton("none",self.layout)

				self.textValue = _lineedit("textValue",self.layout)

                                self.isCopy = QtGui.QCheckBox(draftToolbar)
				self.isCopy.setChecked(False)
				self.isCopy.setObjectName("isCopy")
                                self.layout.addWidget(self.isCopy)
				self.isCopy.hide()

                                self.labelPage = _label("labelPage",self.layout)
                                self.pageBox = QtGui.QComboBox(draftToolbar)
                                self.pageBox.setEditable(True)
                                self.pageBox.setObjectName("pageBox")
                                self.pageBox.addItem("Add New")
                                self.layout.addWidget(self.pageBox)
                                self.pageBox.hide()
                                self.labelScale = _label("labelScale",self.layout)
                                self.scaleBox = QtGui.QComboBox(draftToolbar)
                                self.scaleBox.setEditable(True)
                                self.scaleBox.setObjectName("scaleBox")
                                self.scaleBox.addItem("5")
                                self.scaleBox.addItem("10")
                                self.scaleBox.addItem("20")
                                self.scaleBox.addItem("50")
                                self.layout.addWidget(self.scaleBox)
                                self.scaleBox.hide()
                                self.labelMargin = _label("labelMargin",self.layout)
                                self.marginValue = QtGui.QSpinBox(draftToolbar)
				self.marginValue.setObjectName("marginValue")
                                self.marginValue.setMaximum(999)
				self.marginValue.setValue(50)
                                self.layout.addWidget(self.marginValue)
				self.marginValue.hide()
                                self.labelLWMod = _label("labelLWMod",self.layout)
                                self.LWModValue = QtGui.QSpinBox(draftToolbar)
				self.LWModValue.setObjectName("LWModValue")
                                self.LWModValue.setMaximum(999)
				self.LWModValue.setValue(100)
                                self.layout.addWidget(self.LWModValue)
				self.LWModValue.hide()
                                self.pageButton = _pushButton("pageButton",self.layout,icon=(640,128))

                                spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding,
                                                               QtGui.QSizePolicy.Minimum)
                                self.layout.addItem(spacerItem)

                                self.wplabel = _label("wplabel",self.layout, hide=False)
                                defaultWP = self.params.GetInt("defaultWP")
                                if defaultWP == 1:
                                        self.wplabel.setText("Top")
                                elif defaultWP == 2:
                                        self.wplabel.setText("Front")
                                elif defaultWP == 3:
                                        self.wplabel.setText("Side")
                                else:
                                        self.wplabel.setText("None")
                                self.constrButton = _pushButton("constrButton", self.layout, hide=False, icon=(640,64),width=22)
				self.constrButton.setCheckable(True)
				self.constrButton.setChecked(False)
				self.constrColor = QtGui.QColor(paramconstr)
                                style = "#constrButton:Checked {background-color: "
                                style += self.getDefaultColor("constr",rgb=True)+" }"
				draftToolbar.setStyleSheet(style)
				
				self.colorButton = _pushButton("colorButton",self.layout, hide=False,width=22)
				self.color = QtGui.QColor(paramcolor)
				self.colorPix = QtGui.QPixmap(16,16)
				self.colorPix.fill(self.color)
				self.colorButton.setIcon(QtGui.QIcon(self.colorPix))

				self.widthButton = QtGui.QSpinBox(draftToolbar)
                                self.widthButton.setMaximumSize(QtCore.QSize(50,22))
				self.widthButton.setObjectName("widthButton")
				self.widthButton.setValue(paramlinewidth)
                                self.layout.addWidget(self.widthButton)

				self.applyButton = _pushButton("applyButton", self.layout, hide=False, icon=(384,64),width=22)
				
				self.sourceCmd=None

				self.retranslateUi(draftToolbar)

				QtCore.QObject.connect(self.xValue,QtCore.SIGNAL("returnPressed()"),self.checkx)
				QtCore.QObject.connect(self.yValue,QtCore.SIGNAL("returnPressed()"),self.checky)
				QtCore.QObject.connect(self.xValue,QtCore.SIGNAL("textEdited(QString)"),self.checkSpecialChars)
				QtCore.QObject.connect(self.yValue,QtCore.SIGNAL("textEdited(QString)"),self.checkSpecialChars)
				QtCore.QObject.connect(self.zValue,QtCore.SIGNAL("textEdited(QString)"),self.checkSpecialChars)
				QtCore.QObject.connect(self.radiusValue,QtCore.SIGNAL("textEdited(QString)"),self.checkSpecialChars)
				QtCore.QObject.connect(self.zValue,QtCore.SIGNAL("returnPressed()"),self.validatePoint)
				QtCore.QObject.connect(self.radiusValue,QtCore.SIGNAL("returnPressed()"),self.validatePoint)
				QtCore.QObject.connect(self.textValue,QtCore.SIGNAL("returnPressed()"),self.sendText)
				QtCore.QObject.connect(self.textValue,QtCore.SIGNAL("escaped()"),self.finish)
				QtCore.QObject.connect(self.textValue,QtCore.SIGNAL("down()"),self.sendText)
				QtCore.QObject.connect(self.textValue,QtCore.SIGNAL("up()"),self.lineUp)
				QtCore.QObject.connect(self.zValue,QtCore.SIGNAL("returnPressed()"),self.xValue.setFocus)
				QtCore.QObject.connect(self.zValue,QtCore.SIGNAL("returnPressed()"),self.xValue.selectAll)
				QtCore.QObject.connect(self.offsetValue,QtCore.SIGNAL("textEdited(QString)"),self.checkSpecialChars)
				QtCore.QObject.connect(self.offsetValue,QtCore.SIGNAL("returnPressed()"),self.validatePoint)
				QtCore.QObject.connect(self.finishButton,QtCore.SIGNAL("pressed()"),self.finish)
				QtCore.QObject.connect(self.closeButton,QtCore.SIGNAL("pressed()"),self.closeLine)
				QtCore.QObject.connect(self.undoButton,QtCore.SIGNAL("pressed()"),self.undoSegment)
				QtCore.QObject.connect(self.xyButton,QtCore.SIGNAL("clicked()"),self.selectXY)
				QtCore.QObject.connect(self.xzButton,QtCore.SIGNAL("clicked()"),self.selectXZ)
				QtCore.QObject.connect(self.yzButton,QtCore.SIGNAL("clicked()"),self.selectYZ)
				QtCore.QObject.connect(self.currentViewButton,QtCore.SIGNAL("clicked()"),self.selectCurrentView)
				QtCore.QObject.connect(self.resetPlaneButton,QtCore.SIGNAL("clicked()"),self.selectResetPlane)
				QtCore.QObject.connect(self.xValue,QtCore.SIGNAL("escaped()"),self.finish)
				QtCore.QObject.connect(self.xValue,QtCore.SIGNAL("undo()"),self.undoSegment)
				QtCore.QObject.connect(self.yValue,QtCore.SIGNAL("escaped()"),self.finish)
				QtCore.QObject.connect(self.yValue,QtCore.SIGNAL("undo()"),self.undoSegment)
				QtCore.QObject.connect(self.zValue,QtCore.SIGNAL("escaped()"),self.finish)
				QtCore.QObject.connect(self.zValue,QtCore.SIGNAL("undo()"),self.undoSegment)
				QtCore.QObject.connect(self.radiusValue,QtCore.SIGNAL("escaped()"),self.finish)
				QtCore.QObject.connect(self.draftToolbar,QtCore.SIGNAL("resized()"),self.relocate)
				QtCore.QObject.connect(self.colorButton,QtCore.SIGNAL("pressed()"),self.getcol)
				QtCore.QObject.connect(self.widthButton,QtCore.SIGNAL("valueChanged(int)"),self.setwidth)
				QtCore.QObject.connect(self.applyButton,QtCore.SIGNAL("pressed()"),self.apply)
				QtCore.QObject.connect(self.constrButton,QtCore.SIGNAL("toggled(bool)"),self.toggleConstrMode)
                                QtCore.QObject.connect(self.pageButton,QtCore.SIGNAL("pressed()"),self.drawPage)
				QtCore.QMetaObject.connectSlotsByName(draftToolbar)

#---------------------------------------------------------------------------
# language tools
#---------------------------------------------------------------------------
				
			def retranslateUi(self, draftToolbar):
                                
                                self.promptlabel.setText(translate("draft", "active command:"))
				self.cmdlabel.setText(translate("draft", "None"))
				self.cmdlabel.setToolTip(translate("draft", "Active Draft command"))
				self.xValue.setToolTip(translate("draft", "X coordinate of next point"))
                                self.labelx.setText(translate("draft", "X"))
                                self.labely.setText(translate("draft", "Y"))
                                self.labelz.setText(translate("draft", "Z"))
				self.yValue.setToolTip(translate("draft", "Y coordinate of next point"))
				self.zValue.setToolTip(translate("draft", "Z coordinate of next point"))
				self.labelRadius.setText(translate("draft", "Radius"))
				self.radiusValue.setToolTip(translate("draft", "Radius of Circle"))
				self.isRelative.setText(translate("draft", "Relative"))
				self.isRelative.setToolTip(translate("draft", "Coordinates relative to last point or absolute (SPACE)"))
                                self.hasFill.setText(translate("draft", "Filled"))
				self.hasFill.setToolTip(translate("draft", "Check this if the object should appear as filled, otherwise it will appear as wireframe"))
				self.finishButton.setText(translate("draft", "Finish"))
				self.finishButton.setToolTip(translate("draft", "Finishes the current line without closing (F)"))
				self.undoButton.setText(translate("draft", "Undo"))
				self.undoButton.setToolTip(translate("draft", "Undo the last segment (CTRL+Z)"))
				self.closeButton.setText(translate("draft", "Close"))
				self.closeButton.setToolTip(translate("draft", "Finishes and closes the current line (C)"))
                                self.offsetLabel.setText(translate("draft", "Offset"))
				self.xyButton.setText(translate("draft", "XY"))
				self.xyButton.setToolTip(translate("draft", "Select XY plane"))
				self.xzButton.setText(translate("draft", "XZ"))
				self.xzButton.setToolTip(translate("draft", "Select XZ plane"))
				self.yzButton.setText(translate("draft", "YZ"))
				self.yzButton.setToolTip(translate("draft", "Select YZ plane"))
				self.currentViewButton.setText(translate("draft", "View"))
				self.currentViewButton.setToolTip(translate("draft", "Select plane perpendicular to the current view"))
				self.resetPlaneButton.setText(translate("draft", "None"))
				self.resetPlaneButton.setToolTip(translate("draft", "Do not project points to a drawing plane"))
				self.widthButton.setSuffix(translate("draft", "px"))
				self.isCopy.setText(translate("draft", "Copy"))
				self.isCopy.setToolTip(translate("draft", "If checked, objects will be copied instead of moved (C)"))
				self.colorButton.setToolTip(translate("draft", "Current line color for new objects"))
				self.widthButton.setToolTip(translate("draft", "Current line width for new objects"))
				self.applyButton.setToolTip(translate("draft", "Apply to selected objects"))
				self.constrButton.setToolTip(translate("draft", "Toggles Construction Mode"))
                                self.wplabel.setToolTip(translate("draft", "Current working plane"))
                                self.labelPage.setText(translate("draft", "Page"))
                                self.labelScale.setText(translate("draft", "Scale"))
                                self.labelMargin.setText(translate("draft", "Margin"))
                                self.labelLWMod.setText(translate("draft", "LW Mod"))
                                self.pageBox.setToolTip(translate("draft", "Page to draw to. If selecting Add New, you can edit the name"))
                                self.scaleBox.setToolTip(translate("draft", "Scale factor to apply. Drawing page is always in millimeters. So if you draw in meters, a scale of 10 means 1:100"))
                                self.marginValue.setToolTip(translate("draft", "The value of the margin (offset between the (0,0) point and the page border)"))
                                self.LWModValue.setToolTip(translate("draft", "Linewidth scale modifier. Lower values mean thicker lines."))

#---------------------------------------------------------------------------
# Interface modes
#---------------------------------------------------------------------------

			def selectPlaneUi(self):
				self.cmdlabel.setText(translate("draft", "Select Plane"))
				self.xyButton.show()
				self.xzButton.show()
				self.yzButton.show()
				self.currentViewButton.show()
				self.resetPlaneButton.show()
				self.offsetLabel.show()
				self.offsetValue.show()

			def lineUi(self):
				self.cmdlabel.setText(translate("draft", "Line"))
				self.isRelative.show()
                                self.hasFill.show()
				self.finishButton.show()
				self.closeButton.show()
				self.undoButton.show()
				self.pointUi()

			def circleUi(self):
				self.cmdlabel.setText(translate("draft", "Circle"))
				self.pointUi()
				self.labelx.setText(translate("draft", "Center X"))

			def arcUi(self):
				self.cmdlabel.setText(translate("draft", "Arc"))
				self.labelx.setText(translate("draft", "Center X"))
				self.pointUi()

			def pointUi(self):
				self.xValue.setEnabled(True)
				self.yValue.setEnabled(True)
				self.labelx.setText(translate("draft", "X"))
				self.labelx.show()
				self.labely.show()
				self.labelz.show()
				self.xValue.show()
				self.yValue.show()
				self.zValue.show()
				self.xValue.setFocus()
				self.xValue.selectAll()

			def offUi(self):
				self.cmdlabel.setText(translate("draft", "None"))
				self.labelx.setText(translate("draft", "X"))
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
				self.isRelative.hide()
                                self.hasFill.hide()
				self.finishButton.hide()
				self.undoButton.hide()
				self.closeButton.hide()
				self.xyButton.hide()
				self.xzButton.hide()
				self.yzButton.hide()
				self.currentViewButton.hide()
				self.resetPlaneButton.hide()
				self.offsetLabel.hide()
				self.offsetValue.hide()
				self.labelRadius.hide()
				self.radiusValue.hide()
				self.isCopy.hide()
				self.textValue.hide()
                                self.pageBox.hide()
                                self.labelPage.hide()
                                self.scaleBox.hide()
                                self.labelScale.hide()
                                self.marginValue.hide()
                                self.labelMargin.hide()
                                self.pageButton.hide()
                                self.labelLWMod.hide()
                                self.LWModValue.hide()

			def radiusUi(self):
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
				self.labelRadius.setText(translate("draft", "Radius"))
				self.labelRadius.show()
				self.radiusValue.show()
                                self.hasFill.show()

			def textUi(self):
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
				self.textValue.show()
				self.textValue.setText('')
				self.textValue.setFocus()
				self.textbuffer=[]
				self.textline=0

			def switchUi(self,store=True):
				if store:
					self.state = []
					self.state.append(self.labelx.isVisible())
					self.state.append(self.labely.isVisible())
					self.state.append(self.labelz.isVisible())
					self.state.append(self.xValue.isVisible())
					self.state.append(self.yValue.isVisible())
					self.state.append(self.zValue.isVisible())
					self.labelx.hide()
					self.labely.hide()
					self.labelz.hide()
					self.xValue.hide()
					self.yValue.hide()
					self.zValue.hide()
				else:
					if self.state:
						if self.state[0]:self.labelx.show()
						if self.state[1]:self.labely.show()
						if self.state[2]:self.labelz.show()
						if self.state[3]:self.xValue.show()
						if self.state[4]:self.yValue.show()
						if self.state[5]:self.zValue.show()
						self.state = None

			def selectUi(self):
				self.labelx.setText(translate("draft", "Pick Object"))
				self.labelx.show()

                        def pageUi(self):
				self.labelPage.show()
                                self.pageBox.show()
                                self.labelScale.show()
                                self.scaleBox.show()
                                self.marginValue.show()
                                self.labelMargin.show()
                                self.pageButton.show()
                                self.labelLWMod.show()
                                self.LWModValue.show()

			def relocate(self):
				"relocates the right-aligned buttons depending on the toolbar size"
                                if self.draftToolbar.geometry().width() < 400:
                                        self.layout.setDirection(QtGui.QBoxLayout.TopToBottom)
                                else:
                                        self.layout.setDirection(QtGui.QBoxLayout.LeftToRight) 


#---------------------------------------------------------------------------
# Processing functions
#---------------------------------------------------------------------------

					
			def getcol(self):
				"opens a color picker dialog"
				self.color=QtGui.QColorDialog.getColor()
				self.colorPix.fill(self.color)
				self.colorButton.setIcon(QtGui.QIcon(self.colorPix))
				if self.params.GetBool("saveonexit"):
					self.params.SetUnsigned("color",self.color.rgb()<<8)
				r = float(self.color.red()/255.0)
				g = float(self.color.green()/255.0)
				b = float(self.color.blue()/255.0)
				col = (r,g,b,0.0)
				for i in FreeCADGui.Selection.getSelection():
					if (i.Type == "App::Annotation"):
						i.ViewObject.TextColor=col
					else:
						i.ViewObject.LineColor = col
					
			def setwidth(self,val):
				if self.params.GetBool("saveonexit"):
					self.params.SetInt("linewidth",int(val))
				lw = float(val)
				for i in FreeCADGui.Selection.getSelection():
					if not (i.Type == "App::Annotation"):
						i.ViewObject.LineWidth = lw

			def apply(self):
				r = float(self.color.red()/255.0)
				g = float(self.color.green()/255.0)
				b = float(self.color.blue()/255.0)
				lw = float(self.widthButton.value())
				col = (r,g,b,0.0)
				for i in FreeCADGui.Selection.getSelection():
					if (i.Type == "App::Annotation"):
						i.ViewObject.TextColor=col
					else:
						i.ViewObject.LineColor = col
						i.ViewObject.LineWidth = lw	
					
			def checkx(self):
				if self.yValue.isEnabled():
					self.yValue.setFocus()
					self.yValue.selectAll()
				else:
					self.checky()

			def checky(self):
				if self.zValue.isEnabled():
					self.zValue.setFocus()
					self.zValue.selectAll()
				else:
					self.validatePoint()

			def validatePoint(self):
				"function for checking and sending numbers entered manually"
				if self.sourceCmd != None:
					if (self.labelRadius.isVisible()):
						try:
							rad=float(self.radiusValue.text())
						except ValueError:
							pass
						else:
							self.sourceCmd.numericRadius(rad)
					elif (self.offsetLabel.isVisible()):
						try:
							offset=float(self.offsetValue.text())
						except ValueError:
							pass
						else:
							self.sourceCmd.offsetHandler(offset)
					else:
						try:
							numx=float(self.xValue.text())
							numy=float(self.yValue.text())
							numz=float(self.zValue.text())
						except ValueError:
							pass
						else:
							if self.isRelative.isVisible() and self.isRelative.isChecked():
								if self.sourceCmd.node:
									if self.sourceCmd.featureName == "Rectangle":
										last = self.sourceCmd.node[0]
									else:
										last = self.sourceCmd.node[-1]
									numx = last.x + numx
									numy = last.y + numy
									numz = last.z + numz
							self.sourceCmd.numericInput(numx,numy,numz)

			def finish(self):
				"finish button action"
				self.sourceCmd.finish(False)

			def closeLine(self):
				"close button action"
				self.sourceCmd.finish(True)
		
			def selectXY(self):
				self.sourceCmd.selectHandler("XY")
		
			def selectXZ(self):
				self.sourceCmd.selectHandler("XZ")
		
			def selectYZ(self):
				self.sourceCmd.selectHandler("YZ")

			def selectCurrentView(self):
				self.sourceCmd.selectHandler("currentView")

			def selectResetPlane(self):
				self.sourceCmd.selectHandler("reset")

			def undoSegment(self):
				"undo last line segment"
				self.sourceCmd.undolast()

			def checkSpecialChars(self,txt):
				'''
				checks for special characters in the entered coords that mut be
				treated as shortcuts
				'''
				if txt.endsWith(" "):
					self.isRelative.setChecked(not self.isRelative.isChecked())
					for i in [self.xValue,self.yValue,self.zValue]:
						if (i.text() == txt): i.setText("")
				if txt.endsWith("f"):
					if self.finishButton.isVisible(): self.finish()
				if txt.endsWith("c"):
					if self.closeButton.isVisible(): self.closeLine()
					elif self.isCopy.isVisible():
						self.isCopy.setChecked(not self.isCopy.isChecked())

			def sendText(self):
				'''
				this function sends the entered text to the active draft command
				if enter has been pressed twice. Otherwise it blanks the line.
				'''
				if self.textline == len(self.textbuffer):
					if self.textline:
						if not self.textValue.text():
							self.sourceCmd.text=self.textbuffer
							self.sourceCmd.createObject()
					self.textbuffer.append(self.textValue.text())
					self.textline += 1
					self.textValue.setText('')
				elif self.textline < len(self.textbuffer):
					self.textbuffer[self.textline] = self.textValue.text()
					self.textline += 1
					if self.textline < len(self.textbuffer):
						self.textValue.setText(self.textbuffer[self.textline])
					else:
						self.textValue.setText('')

			def lineUp(self):
				"displays previous line in text editor"
				if self.textline:
					if self.textline == len(self.textbuffer):
						self.textbuffer.append(self.textValue.text())
						self.textline -= 1
						if self.textValue.text():
							self.textValue.setText(self.textbuffer[self.textline])
					elif self.textline < len(self.textbuffer):
						self.textbuffer[self.textline] = self.textValue.text()
						self.textline -= 1
						self.textValue.setText(self.textbuffer[self.textline])

			def displayPoint(self, point, last=None):
				"this function displays the passed coords in the x, y, and z widgets"
				dp = point
				if self.isRelative.isChecked() and (last != None):
					dp = FreeCAD.Vector(point.x-last.x, point.y-last.y, point.z-last.z)
				self.xValue.setText("%.2f" % dp.x)
				self.yValue.setText("%.2f" % dp.y)
				if self.zValue.isEnabled(): self.zValue.setText("%.2f" % dp.z)
				if self.xValue.isEnabled():
					self.xValue.setFocus()
					self.xValue.selectAll()
				else:
					self.yValue.setFocus()
					self.yValue.selectAll()

			def getDefaultColor(self,type,rgb=False):
				"gets color from the preferences or toolbar"
				if type == "snap":
					color = self.params.GetUnsigned("snapcolor")
					r = ((color>>24)&0xFF)/255
					g = ((color>>16)&0xFF)/255
					b = ((color>>8)&0xFF)/255
				elif type == "ui":
					r = float(self.color.red()/255.0)
					g = float(self.color.green()/255.0)
					b = float(self.color.blue()/255.0)
				elif type == "constr":
					color = QtGui.QColor(self.params.GetUnsigned("constructioncolor")>>8)
					r = color.red()/255.0
					g = color.green()/255.0
					b = color.blue()/255.0
				else: print "draft: error: couldn't get a color for ",type," type."
				if rgb:
					return("rgb("+str(int(r*255))+","+str(int(g*255))+","+str(int(b*255))+")")
				else:
					return (r,g,b)

			def cross(self,on=True):
				if on:
					# self.app.setOverrideCursor(QtCore.Qt.CrossCursor)
                                        if not self.crossedViews:
                                                mw = getMainWindow()
                                                self.crossedViews = mw.findChildren(QtGui.QFrame,"SoQtWidgetName")
                                        for w in self.crossedViews:
                                                w.setCursor(QtCore.Qt.CrossCursor)
				else:
					# self.app.restoreOverrideCursor()
                                        for w in self.crossedViews:
                                                w.unsetCursor()
                                        self.crossedViews = []

			def toggleConstrMode(self,checked):
				self.draftToolbar.setStyleSheet("#constrButton:Checked {background-color: "+self.getDefaultColor("constr",rgb=True)+" }")
				self.constrMode = checked

                        def drawPage(self):
                                self.sourceCmd.draw()

#---------------------------------------------------------------------------
# Initialization
#---------------------------------------------------------------------------

		# adding command icons to FreeCAD
		iconfile = findicons()
		icons = {}
		icons['Draft_line']=iconfile.copy(QtCore.QRect(0,0,64,64))
		icons['Draft_wire']=iconfile.copy(QtCore.QRect(64,0,64,64))
		icons['Draft_rectangle']=iconfile.copy(QtCore.QRect(128,0,64,64))
		icons['Draft_circle']=iconfile.copy(QtCore.QRect(196,0,64,64))
		icons['Draft_arc']=iconfile.copy(QtCore.QRect(256,0,64,64))
		icons['Draft_text']=iconfile.copy(QtCore.QRect(320,0,64,64))
		icons['Draft_dimension']=iconfile.copy(QtCore.QRect(448,0,64,64))
		icons['Draft_move']=iconfile.copy(QtCore.QRect(0,64,64,64))
		icons['Draft_rotate']=iconfile.copy(QtCore.QRect(64,64,64,64))
		icons['Draft_offset']=iconfile.copy(QtCore.QRect(128,64,64,64))
		icons['Draft_trimex']=iconfile.copy(QtCore.QRect(196,64,64,64))
		icons['Draft_upgrade']=iconfile.copy(QtCore.QRect(256,64,64,64))
		icons['Draft_downgrade']=iconfile.copy(QtCore.QRect(320,64,64,64))
		icons['Draft_apply']=iconfile.copy(QtCore.QRect(384,64,64,64))
		icons['Draft_finish']=iconfile.copy(QtCore.QRect(448,64,64,64))
		icons['Draft_lock']=iconfile.copy(QtCore.QRect(512,64,64,64))
		icons['Draft_scale']=iconfile.copy(QtCore.QRect(576,64,64,64))
		icons['Draft_selectPlane']=iconfile.copy(QtCore.QRect(576,0,64,64))
                icons['Draft_sendToDrawing']=iconfile.copy(QtCore.QRect(640,0,64,64))
		icons['preferences-draft']=iconfile.copy(QtCore.QRect(384,0,64,64))
		for name,icon in icons.iteritems():
			ba = QtCore.QByteArray()
			bu = QtCore.QBuffer(ba)
			icon.save(bu,'XPM')
			FreeCADGui.addIcon(name,str(ba))

		# loads a translation engine
		languages = {"English":"en","French":"fr","German":"de","Italian":"it"}
		ln = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/General").GetString("Language")
		if ln in languages:
			self.translator = QtCore.QTranslator()
			self.translator.load("draft_"+languages[ln],draftPath()+os.sep+"Languages")
			QtGui.QApplication.installTranslator(self.translator)
			
		# create the draft Toolbar
		self.mw = getMainWindow()
		self.draftWidget = QtGui.QDockWidget()
                self.insideWidget = DraftDockWidget()
		self.ui = Ui_draftToolbar()
		self.ui.app = QtGui.qApp
		self.ui.setupUi(self.insideWidget)
		self.draftWidget.setObjectName("draftToolbar")
                self.draftWidget.setTitleBarWidget(self.insideWidget)
		self.draftWidget.setWindowTitle(translate("draft", "draft Command Bar"))
		self.mw.addDockWidget(QtCore.Qt.TopDockWidgetArea,self.draftWidget)
		self.draftWidget.setVisible(False)
		self.draftWidget.toggleViewAction().setVisible(False)

