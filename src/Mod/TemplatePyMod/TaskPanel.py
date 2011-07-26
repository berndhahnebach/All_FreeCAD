# FreeCAD TemplatePyMod module  
# (c) 2011 Werner Mayer LGPL

import FreeCAD as App
import FreeCADGui as Gui
from PyQt4 import QtGui,QtCore

class TaskPanel:
    def accept(self):
        return True

    def ui(self):
        return App.getResourceDir() + "Mod/TemplatePyMod/TaskPanel.ui"

    def setupUi(self):
        mw = self.getMainWindow()
        form = mw.findChild(QtGui.QWidget, "TaskPanel")
        form.pushButton = form.findChild(QtGui.QPushButton, "pushButton")
        form.listWidget = form.findChild(QtGui.QListWidget, "listWidget")
        self.form = form
        #Connect Signals and Slots
        QtCore.QObject.connect(form.pushButton, QtCore.SIGNAL("clicked()"), self.addElement)

    def getMainWindow(self):
        "returns the main window"
        # using QtGui.qApp.activeWindow() isn't very reliable because if another
        # widget than the mainwindow is active (e.g. a dialog) the wrong widget is
        # returned
        toplevel = QtGui.qApp.topLevelWidgets()
        for i in toplevel:
            if i.metaObject().className() == "Gui::MainWindow":
                return i
        raise Exception("No main window found")
        
    def addElement(self):
        item=QtGui.QInputDialog.getText(self.form, 'Add item', 'Enter:')
        if item[1]:
            self.form.listWidget.addItem(item[0])


def createTask():
    panel = TaskPanel()
    Gui.Control.showDialog(panel)
    panel.setupUi()
    return panel
