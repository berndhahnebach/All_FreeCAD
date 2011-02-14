
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

import FreeCAD, FreeCADGui, os, Draft

try:
	from PyQt4 import QtCore,QtGui,QtSvg	
except:
	FreeCAD.Console.PrintMessage("Error: Python-qt4 package must be installed on your system to use the Draft module.")

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

class todo:
	''' static todo class, delays execution of functions.  Use todo.delay
	to schedule geometry manipulation that would crash coin if done in the
	event callback'''

	'''List of (function, argument) pairs to be executed by
	QTcore.QTimer.singleShot(0,doTodo).'''
	itinerary = []

	@staticmethod
	def doTasks():
		for f, arg in todo.itinerary:
			try:
                                # print "debug: executing",f
                                f(arg)
			except:
				wrn = "[Draft.todo] Unexpected error:" + sys.exc_info()[0]
				FreeCAD.Console.PrintWarning (wrn)
		todo.itinerary = []

	@staticmethod
	def delay (f, arg):
                # print "debug: delaying",f
		if todo.itinerary == []:
			QtCore.QTimer.singleShot(0, todo.doTasks)
		todo.itinerary.append((f,arg))

def translate(context,text):
        "convenience function for Qt translator"
        return QtGui.QApplication.translate(context, text, None,
                                            QtGui.QApplication.UnicodeUTF8)

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
                                paramfontsize = self.params.GetFloat("textheight")
				paramconstr = self.params.GetUnsigned("constructioncolor")>>8
				self.constrMode = False
				draftToolbar.setObjectName("draftToolbar")
				self.state = None
				self.textbuffer = []
				self.draftToolbar = draftToolbar
                                self.crossedViews = []
                                
                                self.layout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight,draftToolbar)
                                self.layout.setDirection(QtGui.QBoxLayout.LeftToRight)
                                self.layout.setObjectName("layout")

				def _pushButton (name, hide=True, icon=None, width=66, checkable=False):
					button = QtGui.QPushButton(draftToolbar)
					#button.setGeometry(geometry)
					button.setObjectName(name)
                                        button.setMaximumSize(QtCore.QSize(width,26))
					if hide: button.hide()
                                        if icon:
                                                button.setIcon(QtGui.QIcon(':/icons/'+icon+'.svg'))
                                                button.setIconSize(QtCore.QSize(16, 16))
                                        if checkable:
                                                button.setCheckable(True)
                                                button.setChecked(False)
                                        self.layout.addWidget(button)
					return button

                                def _label (name, hide=True):
                                        label = QtGui.QLabel(draftToolbar)
                                        label.setObjectName(name)
                                        if hide: label.hide()
                                        self.layout.addWidget(label)
                                        return label

                                def _lineedit (name, hide=True, width=None):
                                        lineedit = DraftLineEdit(draftToolbar)
                                        lineedit.setObjectName(name)
                                        if hide: lineedit.hide()
                                        if not width: width = 800
                                        lineedit.setMaximumSize(QtCore.QSize(width,22))
                                        self.layout.addWidget(lineedit)
                                        return lineedit

                                def _spinbox (name, val=None, vmax=None, hide=True, double=False, size=None):
                                        if double:
                                                sbox = QtGui.QDoubleSpinBox(draftToolbar)
                                                sbox.setDecimals(2)
                                        else:
                                                sbox = QtGui.QSpinBox(draftToolbar)
                                        sbox.setObjectName(name)
                                        if val: sbox.setValue(val)
                                        if vmax: sbox.setMaximum(vmax)
                                        if size: sbox.setMaximumSize(QtCore.QSize(size[0],size[1]))
                                        if hide: sbox.hide()
                                        self.layout.addWidget(sbox)
                                        return sbox

                                def _combo (name, hide=True, editable=True, width=None):
                                        cbox = QtGui.QComboBox(draftToolbar)
                                        cbox.setObjectName(name)
                                        if editable: cbox.setEditable(True)
                                        if hide: cbox.hide()
                                        if width: cbox.setMinimumSize(QtCore.QSize(width,22))
                                        self.layout.addWidget(cbox)
                                        return cbox

                                def _checkbox (name, checked=True, hide=True):
                                        chk = QtGui.QCheckBox(draftToolbar)
                                        chk.setChecked(checked)
                                        chk.setObjectName(name)
                                        if hide: chk.hide()
                                        self.layout.addWidget(chk)
                                        return chk

                                # command

                                self.promptlabel = _label("promptlabel", hide=False)
				self.cmdlabel = _label("cmdlabel", hide=False)
				boldtxt = QtGui.QFont()
				boldtxt.setWeight(75)
				boldtxt.setBold(True)
				self.cmdlabel.setFont(boldtxt)

                                # subcommands

                                self.addButton = _pushButton("addButton", icon="Draft_AddPoint", width=100, checkable=True)
                                self.delButton = _pushButton("delButton", icon="Draft_DelPoint", width=115, checkable=True)
                                
                                # point

				self.labelx = _label("labelx")
				self.xValue = _lineedit("xValue", width=60)
				self.xValue.setText("0.00")
                                #self.xValue.setInputMask("0.00")
				self.labely = _label("labely")
				self.yValue = _lineedit("yValue", width=60)
				self.yValue.setText("0.00")
				self.labelz = _label("labelz")
				self.zValue = _lineedit("zValue", width=60)
				self.zValue.setText("0.00")

                                # options

                                self.numFaces = _spinbox("numFaces",3)

				self.offsetLabel = _label("offsetlabel")
				self.offsetValue = _lineedit("offsetValue", width=60)
				self.offsetValue.setText("0.00")

                                self.labelRadius = _label("labelRadius")
				self.radiusValue = _lineedit("radiusValue", width=60)
				self.radiusValue.setText("0.00")

				self.isRelative = _checkbox("isRelative",checked=True)

                                self.hasFill = _checkbox("hasFill",checked=self.params.GetBool("fillmode"))

                                self.continueCmd = _checkbox("continueCmd",checked=False)
                                
				self.undoButton = _pushButton("undoButton", icon='Draft_Rotate')
				self.finishButton = _pushButton("finishButton", icon='Draft_Finish')
				self.closeButton = _pushButton("closeButton", icon='Draft_Lock')
                                
				self.xyButton = _pushButton("xyButton")
				self.xzButton = _pushButton("xzButton")
				self.yzButton = _pushButton("yzButton")
				self.currentViewButton = _pushButton("view")
				self.resetPlaneButton = _pushButton("none")

				self.textValue = _lineedit("textValue")

                                self.isCopy = QtGui.QCheckBox(draftToolbar)
				self.isCopy.setChecked(False)
				self.isCopy.setObjectName("isCopy")
                                self.layout.addWidget(self.isCopy)
				self.isCopy.hide()

                                # spacer

                                spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding,
                                                               QtGui.QSizePolicy.Minimum)
                                self.layout.addItem(spacerItem)

                                # settings buttons

                                self.wplabel = _pushButton("wplabel", icon='Draft_SelectPlane',hide=False,width=120)
                                defaultWP = self.params.GetInt("defaultWP")
                                if defaultWP == 1:
                                        self.wplabel.setText("Top")
                                elif defaultWP == 2:
                                        self.wplabel.setText("Front")
                                elif defaultWP == 3:
                                        self.wplabel.setText("Side")
                                else:
                                        self.wplabel.setText("None")
                                self.constrButton = _pushButton("constrButton", hide=False, icon='Draft_Construction',width=22, checkable=True)
				self.constrColor = QtGui.QColor(paramconstr)
				self.colorButton = _pushButton("colorButton",hide=False,width=22)
				self.color = QtGui.QColor(paramcolor)
				self.colorPix = QtGui.QPixmap(16,16)
				self.colorPix.fill(self.color)
				self.colorButton.setIcon(QtGui.QIcon(self.colorPix))
                                self.facecolorButton = _pushButton("facecolorButton",hide=False,width=22)
				self.facecolor = QtGui.QColor(204,204,204)
				self.facecolorPix = QtGui.QPixmap(16,16)
				self.facecolorPix.fill(self.facecolor)
				self.facecolorButton.setIcon(QtGui.QIcon(self.facecolorPix))
				self.widthButton = _spinbox("widthButton",val=paramlinewidth,hide=False,size=(50,22))
                                self.widthButton.setSuffix("px")
                                self.fontsizeButton = _spinbox("fontsizeButton",val=paramfontsize,hide=False,double=True,size=(50,22))
				self.applyButton = _pushButton("applyButton", hide=False, icon='Draft_Apply',width=22)

                                style = "#constrButton:Checked {background-color: "
                                style += self.getDefaultColor("constr",rgb=True)+" } "
                                style += "#addButton:Checked, #delButton:checked {"
                                style += "background-color: rgb(10,255,10) }"
				draftToolbar.setStyleSheet(style)
                                
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
                                QtCore.QObject.connect(self.addButton,QtCore.SIGNAL("toggled(bool)"),self.setAddMode)
                                QtCore.QObject.connect(self.delButton,QtCore.SIGNAL("toggled(bool)"),self.setDelMode)
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
                                QtCore.QObject.connect(self.wplabel,QtCore.SIGNAL("pressed()"),self.selectplane)
				QtCore.QObject.connect(self.colorButton,QtCore.SIGNAL("pressed()"),self.getcol)
                                QtCore.QObject.connect(self.facecolorButton,QtCore.SIGNAL("pressed()"),self.getfacecol)
				QtCore.QObject.connect(self.widthButton,QtCore.SIGNAL("valueChanged(int)"),self.setwidth)
                                QtCore.QObject.connect(self.fontsizeButton,QtCore.SIGNAL("valueChanged(double)"),self.setfontsize)
				QtCore.QObject.connect(self.applyButton,QtCore.SIGNAL("pressed()"),self.apply)
				QtCore.QObject.connect(self.constrButton,QtCore.SIGNAL("toggled(bool)"),self.toggleConstrMode)

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
				self.isRelative.setText(translate("draft", "&Relative"))
				self.isRelative.setToolTip(translate("draft", "Coordinates relative to last point or absolute (SPACE)"))
                                self.hasFill.setText(translate("draft", "&Filled"))
                                self.hasFill.setToolTip(translate("draft", "Check this if the object should appear as filled, otherwise it will appear as wireframe (F)"))
				self.finishButton.setText(translate("draft", "F&inish"))
				self.finishButton.setToolTip(translate("draft", "Finishes the current drawing or editing operation (F)"))
                                self.addButton.setText(translate("draft", "&Add Points"))
				self.addButton.setToolTip(translate("draft", "Add points to the current object"))
                                self.delButton.setText(translate("draft", "&Remove Points"))
				self.delButton.setToolTip(translate("draft", "Remove points from the current object"))
				self.undoButton.setText(translate("draft", "&Undo"))
				self.undoButton.setToolTip(translate("draft", "Undo the last segment (CTRL+Z)"))
				self.closeButton.setText(translate("draft", "&Close"))
				self.closeButton.setToolTip(translate("draft", "Finishes and closes the current line (C)"))
                                self.numFaces.setToolTip(translate("draft", "Number of sides"))
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
				self.isCopy.setText(translate("draft", "&Copy"))
				self.isCopy.setToolTip(translate("draft", "If checked, objects will be copied instead of moved (C)"))
				self.colorButton.setToolTip(translate("draft", "Line Color"))
                                self.continueCmd.setToolTip(translate("draft", "If checked, command will not finish until you press the command button again"))
                                self.continueCmd.setText(translate("draft", "&Continue"))
                                self.facecolorButton.setToolTip(translate("draft", "Face Color"))
				self.widthButton.setToolTip(translate("draft", "Line Width"))
                                self.fontsizeButton.setToolTip(translate("draft", "Font Size"))
				self.applyButton.setToolTip(translate("draft", "Apply to selected objects"))
				self.constrButton.setToolTip(translate("draft", "Toggles Construction Mode"))

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
                                self.hasFill.show()

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
                                self.numFaces.hide()
				self.isRelative.hide()
                                self.hasFill.hide()
				self.finishButton.hide()
                                self.addButton.hide()
                                self.delButton.hide()
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
                                self.continueCmd.hide()

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

                        def editUi(self):
				self.labelx.hide()
				self.labely.hide()
				self.labelz.hide()
				self.xValue.hide()
				self.yValue.hide()
				self.zValue.hide()
                                self.numFaces.hide()
				self.isRelative.hide()
                                self.hasFill.hide()
                                self.addButton.show()
                                self.delButton.show()
				self.finishButton.show()

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
                                                if "LineColor" in i.ViewObject.PropertiesList:
                                                        i.ViewObject.LineColor = col
                                                if "PointColor" in i.ViewObject.PropertiesList:
                                                        i.ViewObject.PointColor = col

                        def getfacecol(self):
				"opens a color picker dialog"
				self.facecolor=QtGui.QColorDialog.getColor()
				self.facecolorPix.fill(self.facecolor)
				self.facecolorButton.setIcon(QtGui.QIcon(self.facecolorPix))
                                r = float(self.facecolor.red()/255.0)
				g = float(self.facecolor.green()/255.0)
				b = float(self.facecolor.blue()/255.0)
				col = (r,g,b,0.0)
                                for i in FreeCADGui.Selection.getSelection():
                                        if "ShapeColor" in i.ViewObject.PropertiesList:
						i.ViewObject.ShapeColor = col
					
			def setwidth(self,val):
				if self.params.GetBool("saveonexit"):
					self.params.SetInt("linewidth",int(val))
				for i in FreeCADGui.Selection.getSelection():
					if "LineWidth" in i.ViewObject.PropertiesList:
						i.ViewObject.LineWidth = float(val)

                        def setfontsize(self,val):
				if self.params.GetBool("saveonexit"):
					self.params.SetFloat("textheight",float(val))
				for i in FreeCADGui.Selection.getSelection():
					if "FontSize" in i.ViewObject.PropertiesList:
						i.ViewObject.FontSize = float(val)

			def apply(self):
				for i in FreeCADGui.Selection.getSelection():
					Draft.formatObject(i)	
					
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
                                spec = False
				if txt.endsWith(" ") or txt.endsWith("r"):
					self.isRelative.setChecked(not self.isRelative.isChecked())
                                        spec = True
                                if txt.endsWith("i"):
                                        if self.hasFill.isVisible():
                                                self.hasFill.setChecked(not self.hasFill.isChecked())
                                        spec = True
                                if txt.endsWith("f"):
                                        if self.finishButton.isVisible():
                                                self.finish()
                                        spec = True
                                if txt.endsWith("c"):
                                        if self.closeButton.isVisible():
                                                self.closeLine()
                                        elif self.isCopy.isVisible():
                                                self.isCopy.setChecked(not self.isCopy.isChecked())
                                        elif self.continueCmd.isVisible():
                                                self.continueCmd.setChecked(not self.continueCmd.isChecked())
                                        spec = True
                                if spec:
                                        for i in [self.xValue,self.yValue,self.zValue]:
                                                if (i.text() == txt): i.setText("")
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
                                elif type == "face":
					r = float(self.facecolor.red()/255.0)
					g = float(self.facecolor.green()/255.0)
					b = float(self.facecolor.blue()/255.0)
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

                        def changePage(self,index):
                                pagename = str(self.pageBox.itemText(index))
                                vobj = FreeCADGui.ActiveDocument.getObject(pagename)
                                if vobj:
                                        self.scaleBox.setEditText(str(vobj.HintScale))
                                        self.marginXValue.setValue(float(vobj.HintOffsetX))
                                        self.marginYValue.setValue(float(vobj.HintOffsetY))

                        def selectplane(self):
                                FreeCADGui.runCommand("Draft_SelectPlane")

                        def popupMenu(self,mlist):
                                "pops up a menu filled with the given list"
                                self.groupmenu = QtGui.QMenu()
                                for i in mlist:
                                        self.groupmenu.addAction(i)
                                pos = getMainWindow().cursor().pos()
                                self.groupmenu.popup(pos)
                                QtCore.QObject.connect(self.groupmenu,QtCore.SIGNAL("triggered(QAction *)"),self.popupTriggered)

                        def popupTriggered(self,action):
                                self.sourceCmd.proceed(str(action.text()))

                        def setAddMode(self,bool):
                                if self.addButton.isChecked():
                                        self.delButton.setChecked(False)

                        def setDelMode(self,bool):
                                if self.delButton.isChecked():
                                        self.addButton.setChecked(False)

#---------------------------------------------------------------------------
# Initialization
#---------------------------------------------------------------------------
	
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

