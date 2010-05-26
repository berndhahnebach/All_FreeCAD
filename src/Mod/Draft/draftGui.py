
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
__author__ = "Yorik van Havre <yorik@gmx.fr>"
__url__ = ["http://yorik.orgfree.com","http://free-cad.sourceforge.net"]

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
	

#---------------------------------------------------------------------------
# Customized widgets
#---------------------------------------------------------------------------

class DraftDockWidget(QtGui.QDockWidget):
	"custom QDockWidget that emits a resized() signal when resized"
	def __init__(self,parent = None):
		QtGui.QDockWidget.__init__(self,parent)
	def resizeEvent(self,event):
		self.emit(QtCore.SIGNAL("resized()"))

class DraftLineEdit(QtGui.QLineEdit):
	"custom QLineEdit widget that has the power to catch Escape keypress"
	def __init__(self, parent = None):
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
				self.lockedz = self.params.GetBool("zlock")
				self.constrMode = False
				draftToolbar.setObjectName("draftToolbar")
				draftToolbar.resize(QtCore.QSize(QtCore.QRect(0,0,800,32).size()).expandedTo(draftToolbar.minimumSizeHint()))
				# draftToolbar.resize(800,32)
				self.state = None
				self.textbuffer = []
				self.draftToolbar = draftToolbar

				def _pushButton (geometry, name, hide=True):
					button = QtGui.QPushButton(draftToolbar)
					button.setGeometry(geometry)
					button.setObjectName(name)
					if hide: button.hide()
					return button

				self.cmdlabel = QtGui.QLabel(draftToolbar)
				self.cmdlabel.setGeometry(QtCore.QRect(110,4,111,18))
				self.cmdlabel.setObjectName("cmdlabel")
				boldtxt = QtGui.QFont()
				boldtxt.setWeight(75)
				boldtxt.setBold(True)
				self.cmdlabel.setFont(boldtxt)

				self.labelx = QtGui.QLabel(draftToolbar)
				self.labelx.setGeometry(QtCore.QRect(150,4,60,18))
				self.labelx.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
				self.labelx.setObjectName("labelx")
				self.labelx.setText("X")
				self.labelx.hide()

				self.xValue = DraftLineEdit(draftToolbar)
				self.xValue.setGeometry(QtCore.QRect(220,4,70,18))
				self.xValue.setObjectName("xValue")
				self.xValue.setText("0.00")
				self.xValue.hide()

				self.labely = QtGui.QLabel(draftToolbar)
				self.labely.setGeometry(QtCore.QRect(300,4,30,18))
				self.labely.setObjectName("labely")
				self.labely.setText("Y")
				self.labely.hide()

				self.yValue = DraftLineEdit(draftToolbar)
				self.yValue.setGeometry(QtCore.QRect(320,4,70,18))
				self.yValue.setObjectName("yValue")
				self.yValue.setText("0.00")
				self.yValue.hide()

				self.labelz = QtGui.QLabel(draftToolbar)
				self.labelz.setGeometry(QtCore.QRect(400,4,30,18))
				self.labelz.setObjectName("labelz")
				self.labelz.setText("Z")
				self.labelz.hide()

				self.zValue = DraftLineEdit(draftToolbar)
				self.zValue.setGeometry(QtCore.QRect(420,4,70,18))
				self.zValue.setObjectName("zValue")
				self.zValue.setText("0.00")
				if self.lockedz: self.zValue.setEnabled(False)
				self.zValue.hide()

				self.offsetLabel = QtGui.QLabel(draftToolbar)
				self.offsetLabel.setGeometry(QtCore.QRect(460,4,60,18))
				self.offsetLabel.setObjectName("offsetLabel")
				self.offsetLabel.setText("Offset")
				self.offsetLabel.hide()

				self.offsetValue = DraftLineEdit(draftToolbar)
				self.offsetValue.setGeometry(QtCore.QRect(500,4,70,18))
				self.offsetValue.setObjectName("offsetValue")
				self.offsetValue.setText("0.00")
				self.offsetValue.hide()

				self.lockButton = _pushButton(QtCore.QRect(500,3,20,20),"lockButton")
				self.lockButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(512,64,64,64))))
				self.lockButton.setIconSize(QtCore.QSize(16, 16))
				self.lockButton.setCheckable(True)
				if self.lockedz: self.lockButton.setChecked(True)

				self.isRelative = QtGui.QCheckBox(draftToolbar)
				self.isRelative.setGeometry(QtCore.QRect(530,6,91,18))
				self.isRelative.setChecked(True)
				self.isRelative.setObjectName("isRelative")
				self.isRelative.hide()

				self.undoButton = _pushButton(QtCore.QRect(610,3,60,20),"undoButton")
				self.undoButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(64,64,64,64))))
				self.undoButton.setIconSize(QtCore.QSize(16, 16))

				self.finishButton = _pushButton(QtCore.QRect(673,3,80,20),"finishButton")
				self.finishButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(448,64,64,64))))
				self.finishButton.setIconSize(QtCore.QSize(16, 16))

				self.closeButton = _pushButton(QtCore.QRect(756,3,60,20),"closeButton")
				self.closeButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(512,64,64,64))))
				self.closeButton.setIconSize(QtCore.QSize(16, 16))

				self.xyButton = _pushButton(QtCore.QRect(200,3,40,20),"xyButton")
				self.xzButton = _pushButton(QtCore.QRect(250,3,40,20),"xzButton")
				self.yzButton = _pushButton(QtCore.QRect(300,3,40,20),"yzButton")
				self.currentViewButton = _pushButton(QtCore.QRect(350,3,40,20),"view")
				self.resetPlaneButton = _pushButton(QtCore.QRect(400,3,40,20),"none")

				self.labelRadius = QtGui.QLabel(draftToolbar)
				self.labelRadius.setGeometry(QtCore.QRect(200,4,75,18))
				self.labelRadius.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
				self.labelRadius.setObjectName("labelRadius")
				self.labelRadius.hide()

				self.radiusValue = DraftLineEdit(draftToolbar)
				self.radiusValue.setGeometry(QtCore.QRect(280,4,70,18))
				self.radiusValue.setObjectName("radiusValue")
				self.radiusValue.setText("0.00")
				self.radiusValue.hide()

				self.labelText = QtGui.QLabel(draftToolbar)
				self.labelText.setGeometry(QtCore.QRect(200,4,75,18))
				self.labelText.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
				self.labelText.setObjectName("labelText")
				self.labelText.hide()

				self.textValue = DraftLineEdit(draftToolbar)
				self.textValue.setGeometry(QtCore.QRect(280,4,480,18))
				# self.textValue = QtGui.QPlainTextEdit(draftToolbar)
				# self.textValue.setGeometry(QtCore.QRect(280, 4, 480, 64))
				self.textValue.setObjectName("textValue")
				self.textValue.hide()
				
				self.colorButton = _pushButton(QtCore.QRect(792,2,22,22),"colorButton",hide=False)
				self.color = QtGui.QColor(paramcolor)
				self.colorPix = QtGui.QPixmap(16,16)
				self.colorPix.fill(self.color)
				self.colorButton.setIcon(QtGui.QIcon(self.colorPix))

				self.widthButton = QtGui.QSpinBox(draftToolbar)
				self.widthButton.setGeometry(QtCore.QRect(817,2,50,22))
				self.widthButton.setObjectName("widthButton")
				self.widthButton.setValue(paramlinewidth)

				self.isCopy = QtGui.QCheckBox(draftToolbar)
				self.isCopy.setGeometry(QtCore.QRect(600,6,91,18))
				self.isCopy.setChecked(False)
				self.isCopy.setObjectName("isCopy")
				self.isCopy.hide()

				self.applyButton = _pushButton(QtCore.QRect(870,2,22,22),"applyButton", hide=False)
				self.applyButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(384,64,64,64))))
				self.applyButton.setIconSize(QtCore.QSize(16, 16))

				self.constrButton = _pushButton(QtCore.QRect(738,2,22,22),"constrButton", hide=False)
                                self.constrButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(640,64,64,64))))
				self.constrButton.setIconSize(QtCore.QSize(16, 16))
				self.constrButton.setCheckable(True)
				self.constrButton.setChecked(False)
				self.constrColor = QtGui.QColor(paramconstr)
				draftToolbar.setStyleSheet("#constrButton:Checked {background-color: "+self.getDefaultColor("constr",rgb=True)+" }")

                                self.wplabel = QtGui.QLabel(draftToolbar)
                                self.wplabel.setGeometry(QtCore.QRect(500, 4, 100, 18))
                                self.wplabel.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
                                self.wplabel.setObjectName("wplabel")

                                self.labelPage = QtGui.QLabel(draftToolbar)
				self.labelPage.setGeometry(QtCore.QRect(215,4,60,18))
				self.labelPage.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
				self.labelPage.setObjectName("labelPage")
				self.labelPage.hide()
                                
                                self.pageBox = QtGui.QComboBox(draftToolbar)
                                self.pageBox.setGeometry(QtCore.QRect(280,4,120,18))
                                self.pageBox.setEditable(True)
                                self.pageBox.setObjectName("pageBox")
                                self.pageBox.addItem("Add New")
                                self.pageBox.hide()

                                self.labelScale = QtGui.QLabel(draftToolbar)
				self.labelScale.setGeometry(QtCore.QRect(405,4,60,18))
				self.labelScale.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
				self.labelScale.setObjectName("labelScale")
				self.labelScale.hide()

                                self.scaleBox = QtGui.QComboBox(draftToolbar)
                                self.scaleBox.setGeometry(QtCore.QRect(470,4,120,18))
                                self.scaleBox.setEditable(True)
                                self.scaleBox.setObjectName("scaleBox")
                                self.scaleBox.addItem("1:10")
                                self.scaleBox.hide()

                                self.labelMargin = QtGui.QLabel(draftToolbar)
				self.labelMargin.setGeometry(QtCore.QRect(600,4,60,18))
				self.labelMargin.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
				self.labelMargin.setObjectName("labelMargin")
				self.labelMargin.hide()

                                self.marginValue = QtGui.QSpinBox(draftToolbar)
				self.marginValue.setGeometry(QtCore.QRect(665,4,60,18))
				self.marginValue.setObjectName("marginValue")
                                self.marginValue.setMaximum(999)
				self.marginValue.setValue(200)
				self.marginValue.hide()

                                self.pageButton = _pushButton(QtCore.QRect(730,3,20,20),"pageButton")
				self.pageButton.setIcon(QtGui.QIcon(icons.copy(QtCore.QRect(640,128,64,64))))
				self.pageButton.setIconSize(QtCore.QSize(16, 16))
                                self.pageButton.hide()
				
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

				QtCore.QObject.connect(self.lockButton,QtCore.SIGNAL("toggled(bool)"),self.lockz)
				QtCore.QObject.connect(self.constrButton,QtCore.SIGNAL("toggled(bool)"),self.toggleConstrMode)
                                QtCore.QObject.connect(self.pageButton,QtCore.SIGNAL("pressed()"),self.drawPage)

				QtCore.QMetaObject.connectSlotsByName(draftToolbar)

#---------------------------------------------------------------------------
# language tools
#---------------------------------------------------------------------------
				
			def retranslateUi(self, draftToolbar):
				self.cmdlabel.setText(QtGui.QApplication.translate("draft", "None", None, QtGui.QApplication.UnicodeUTF8))
				self.cmdlabel.setToolTip(QtGui.QApplication.translate("draft", "Active Draft command", None, QtGui.QApplication.UnicodeUTF8))
				self.xValue.setToolTip(QtGui.QApplication.translate("draft", "X coordinate of next point", None, QtGui.QApplication.UnicodeUTF8))
				self.yValue.setToolTip(QtGui.QApplication.translate("draft", "Y coordinate of next point", None, QtGui.QApplication.UnicodeUTF8))
				self.zValue.setToolTip(QtGui.QApplication.translate("draft", "Z coordinate of next point", None, QtGui.QApplication.UnicodeUTF8))
				self.labelRadius.setText(QtGui.QApplication.translate("draft", "Radius", None, QtGui.QApplication.UnicodeUTF8))
				self.radiusValue.setToolTip(QtGui.QApplication.translate("draft", "Radius of Circle", None, QtGui.QApplication.UnicodeUTF8))
				self.labelText.setText(QtGui.QApplication.translate("draft", "Text", None, QtGui.QApplication.UnicodeUTF8))
				self.isRelative.setText(QtGui.QApplication.translate("draft", "Relative", None, QtGui.QApplication.UnicodeUTF8))
				self.lockButton.setToolTip(QtGui.QApplication.translate("draft", "locks the Z coordinate (L)", None, QtGui.QApplication.UnicodeUTF8))
				self.isRelative.setToolTip(QtGui.QApplication.translate("draft", "Coordinates relative to last point or absolute (SPACE)", None, QtGui.QApplication.UnicodeUTF8))
				self.finishButton.setText(QtGui.QApplication.translate("draft", "Finish", None, QtGui.QApplication.UnicodeUTF8))
				self.finishButton.setToolTip(QtGui.QApplication.translate("draft", "Finishes the current line without closing (F)", None, QtGui.QApplication.UnicodeUTF8))
				self.undoButton.setText(QtGui.QApplication.translate("draft", "Undo", None, QtGui.QApplication.UnicodeUTF8))
				self.undoButton.setToolTip(QtGui.QApplication.translate("draft", "Undo the last segment (CTRL+Z)", None, QtGui.QApplication.UnicodeUTF8))
				self.closeButton.setText(QtGui.QApplication.translate("draft", "Close", None, QtGui.QApplication.UnicodeUTF8))
				self.closeButton.setToolTip(QtGui.QApplication.translate("draft", "Finishes and closes the current line (C)", None, QtGui.QApplication.UnicodeUTF8))
				self.xyButton.setText(QtGui.QApplication.translate("draft", "XY", None, QtGui.QApplication.UnicodeUTF8))
				self.xyButton.setToolTip(QtGui.QApplication.translate("draft", "Select XY plane", None, QtGui.QApplication.UnicodeUTF8))
				self.xzButton.setText(QtGui.QApplication.translate("draft", "XZ", None, QtGui.QApplication.UnicodeUTF8))
				self.xzButton.setToolTip(QtGui.QApplication.translate("draft", "Select XZ plane", None, QtGui.QApplication.UnicodeUTF8))
				self.yzButton.setText(QtGui.QApplication.translate("draft", "YZ", None, QtGui.QApplication.UnicodeUTF8))
				self.yzButton.setToolTip(QtGui.QApplication.translate("draft", "Select YZ plane", None, QtGui.QApplication.UnicodeUTF8))
				self.currentViewButton.setText(QtGui.QApplication.translate("draft", "View", None, QtGui.QApplication.UnicodeUTF8))
				self.currentViewButton.setToolTip(QtGui.QApplication.translate("draft", "Select plane perpendicular to the current view", None, QtGui.QApplication.UnicodeUTF8))
				self.resetPlaneButton.setText(QtGui.QApplication.translate("draft", "None", None, QtGui.QApplication.UnicodeUTF8))
				self.resetPlaneButton.setToolTip(QtGui.QApplication.translate("draft", "Do not project points to a drawing plane", None, QtGui.QApplication.UnicodeUTF8))
				self.widthButton.setSuffix(QtGui.QApplication.translate("draft", "px", None, QtGui.QApplication.UnicodeUTF8))
				self.isCopy.setText(QtGui.QApplication.translate("draft", "Copy", None, QtGui.QApplication.UnicodeUTF8))
				self.isCopy.setToolTip(QtGui.QApplication.translate("draft", "If checked, objects will be copied instead of moved (C)", None, QtGui.QApplication.UnicodeUTF8))
				self.colorButton.setToolTip(QtGui.QApplication.translate("draft", "Current line color for new objects", None, QtGui.QApplication.UnicodeUTF8))
				self.widthButton.setToolTip(QtGui.QApplication.translate("draft", "Current line width for new objects", None, QtGui.QApplication.UnicodeUTF8))
				self.applyButton.setToolTip(QtGui.QApplication.translate("draft", "Apply to selected objects", None, QtGui.QApplication.UnicodeUTF8))
				self.constrButton.setToolTip(QtGui.QApplication.translate("draft", "Toggles Construction Mode", None, QtGui.QApplication.UnicodeUTF8))
                                self.wplabel.setText(QtGui.QApplication.translate("draft", "None", None, QtGui.QApplication.UnicodeUTF8))
                                self.wplabel.setToolTip(QtGui.QApplication.translate("draft", "Current working plane", None, QtGui.QApplication.UnicodeUTF8))
                                self.labelPage.setText(QtGui.QApplication.translate("draft", "Page", None, QtGui.QApplication.UnicodeUTF8))
                                self.labelScale.setText(QtGui.QApplication.translate("draft", "Scale", None, QtGui.QApplication.UnicodeUTF8))
                                self.labelMargin.setText(QtGui.QApplication.translate("draft", "Margin", None, QtGui.QApplication.UnicodeUTF8))

#---------------------------------------------------------------------------
# Interface modes
#---------------------------------------------------------------------------

			def selectPlaneUi(self):
				self.cmdlabel.setText(QtGui.QApplication.translate("draft", "Select Plane", None, QtGui.QApplication.UnicodeUTF8))
				self.xyButton.show()
				self.xzButton.show()
				self.yzButton.show()
				self.currentViewButton.show()
				self.resetPlaneButton.show()
				self.offsetLabel.show()
				self.offsetValue.show()

			def lineUi(self):
				self.cmdlabel.setText(QtGui.QApplication.translate("draft", "Line", None, QtGui.QApplication.UnicodeUTF8))
				self.isRelative.show()
				self.finishButton.show()
				self.closeButton.show()
				self.undoButton.show()
				self.pointUi()

			def circleUi(self):
				self.cmdlabel.setText(QtGui.QApplication.translate("draft", "Circle", None, QtGui.QApplication.UnicodeUTF8))
				self.pointUi()
				self.labelx.setText(QtGui.QApplication.translate("draft", "Center X", None, QtGui.QApplication.UnicodeUTF8))

			def arcUi(self):
				self.cmdlabel.setText(QtGui.QApplication.translate("draft", "Arc", None, QtGui.QApplication.UnicodeUTF8))
				self.labelx.setText(QtGui.QApplication.translate("draft", "Center X", None, QtGui.QApplication.UnicodeUTF8))
				self.pointUi()

			def pointUi(self):
				self.xValue.setEnabled(True)
				self.yValue.setEnabled(True)
				self.labelx.setText("X")
				self.labelx.show()
				self.labely.show()
				self.labelz.show()
				self.xValue.show()
				self.yValue.show()
				self.zValue.show()
				self.lockButton.show()
				self.xValue.setFocus()
				self.xValue.selectAll()

			def offUi(self):
				self.cmdlabel.setText(QtGui.QApplication.translate("draft", "None", None, QtGui.QApplication.UnicodeUTF8))
				self.labelx.setText("X")
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
				self.isRelative.hide()
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
				self.lockButton.hide()
				self.labelText.hide()
				self.textValue.hide()
                                self.pageBox.hide()
                                self.labelPage.hide()
                                self.scaleBox.hide()
                                self.labelScale.hide()
                                self.marginValue.hide()
                                self.labelMargin.hide()
                                self.pageButton.hide()

			def radiusUi(self):
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
				self.lockButton.hide()
				self.labelRadius.setText(QtGui.QApplication.translate("draft", "Radius", None, QtGui.QApplication.UnicodeUTF8))
				self.labelRadius.show()
				self.radiusValue.show()

			def textUi(self):
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
				self.lockButton.hide()
				self.labelText.show()
				self.textValue.show()
				self.textValue.setText('')
				# self.textValue.setPlainText('')
				self.textValue.setFocus()
				# self.draftToolbar.setFixedHeight(70)
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
					self.state.append(self.lockButton.isVisible())
					self.labelx.hide()
					self.labely.hide()
					self.labelz.hide()
					self.xValue.hide()
					self.yValue.hide()
					self.zValue.hide()
					self.lockButton.hide()
				else:
					if self.state:
						if self.state[0]:self.labelx.show()
						if self.state[1]:self.labely.show()
						if self.state[2]:self.labelz.show()
						if self.state[3]:self.xValue.show()
						if self.state[4]:self.yValue.show()
						if self.state[5]:self.zValue.show()
						if self.state[6]:self.lockButton.show()
						self.state = None

			def selectUi(self):
				self.labelx.setText(QtGui.QApplication.translate("draft", "Pick Object", None, QtGui.QApplication.UnicodeUTF8))
				self.labelx.show()

                        def pageUi(self):
				self.labelPage.show()
                                self.pageBox.show()
                                self.labelScale.show()
                                self.scaleBox.show()
                                self.marginValue.show()
                                self.labelMargin.show()
                                self.pageButton.show()

			def relocate(self):
				"relocates the right-aligned buttons depending on the toolbar size"
				w=self.draftToolbar.geometry().width()
				self.widthButton.setGeometry(QtCore.QRect(w-113,2,50,22))
				self.colorButton.setGeometry(QtCore.QRect(w-138,2,22,22))
				self.applyButton.setGeometry(QtCore.QRect(w-60,2,22,22))
				self.constrButton.setGeometry(QtCore.QRect(w-162,2,22,22))
                                self.wplabel.setGeometry(QtCore.QRect(w-268,4,100,18))

			def lockz(self,checked):
					self.zValue.setEnabled(not checked)
					self.lockedz = checked
					if self.params.GetBool("savezlock"):
						self.params.SetBool("zlock",self.lockedz)


				

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
				if txt.endsWith("l"):
					self.lockz(not self.lockedz)

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
					self.app.setOverrideCursor(QtCore.Qt.CrossCursor)
				else:
					self.app.restoreOverrideCursor()

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
		icons['Draft_polyline']=iconfile.copy(QtCore.QRect(64,0,64,64))
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
		self.draftWidget = DraftDockWidget()
		self.ui = Ui_draftToolbar()
		self.ui.app = QtGui.qApp
		self.ui.setupUi(self.draftWidget)
		self.draftWidget.setObjectName("draftToolbar")
		self.draftWidget.setWindowTitle(QtGui.QApplication.translate("draft", "draftCommand", None, QtGui.QApplication.UnicodeUTF8))
		self.mw.addDockWidget(QtCore.Qt.TopDockWidgetArea,self.draftWidget)
		self.draftWidget.setVisible(False)
		self.draftWidget.toggleViewAction().setVisible(False)

