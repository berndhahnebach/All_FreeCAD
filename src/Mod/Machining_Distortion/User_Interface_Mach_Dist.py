# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'User_Interface_Mach_Dist.ui'
#
# Created: Tue Jan 18 17:53:22 2011
#      by: PyQt4 UI code generator 4.8.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_dialog(object):
    def setupUi(self, dialog):
        dialog.setObjectName(_fromUtf8("dialog"))
        dialog.resize(684, 480)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(dialog.sizePolicy().hasHeightForWidth())
        dialog.setSizePolicy(sizePolicy)
        dialog.setMinimumSize(QtCore.QSize(640, 480))
        self.button_dialog = QtGui.QDialogButtonBox(dialog)
        self.button_dialog.setGeometry(QtCore.QRect(500, 450, 159, 25))
        self.button_dialog.setOrientation(QtCore.Qt.Horizontal)
        self.button_dialog.setStandardButtons(QtGui.QDialogButtonBox.Close|QtGui.QDialogButtonBox.Ok)
        self.button_dialog.setObjectName(_fromUtf8("button_dialog"))
        self.JobTable = QtGui.QTableWidget(dialog)
        self.JobTable.setGeometry(QtCore.QRect(9, 280, 671, 163))
        self.JobTable.setRowCount(0)
        self.JobTable.setColumnCount(23)
        self.JobTable.setObjectName(_fromUtf8("JobTable"))
        self.JobTable.setColumnCount(23)
        self.JobTable.setRowCount(0)
        self.button_start_calculation = QtGui.QPushButton(dialog)
        self.button_start_calculation.setEnabled(False)
        self.button_start_calculation.setGeometry(QtCore.QRect(10, 450, 88, 25))
        self.button_start_calculation.setObjectName(_fromUtf8("button_start_calculation"))
        self.layoutWidget = QtGui.QWidget(dialog)
        self.layoutWidget.setGeometry(QtCore.QRect(7, 12, 671, 261))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.gridLayout_8 = QtGui.QGridLayout(self.layoutWidget)
        self.gridLayout_8.setMargin(0)
        self.gridLayout_8.setObjectName(_fromUtf8("gridLayout_8"))
        self.gridLayout_7 = QtGui.QGridLayout()
        self.gridLayout_7.setObjectName(_fromUtf8("gridLayout_7"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.button_select_file = QtGui.QPushButton(self.layoutWidget)
        self.button_select_file.setObjectName(_fromUtf8("button_select_file"))
        self.verticalLayout.addWidget(self.button_select_file)
        self.button_select_output = QtGui.QPushButton(self.layoutWidget)
        self.button_select_output.setObjectName(_fromUtf8("button_select_output"))
        self.verticalLayout.addWidget(self.button_select_output)
        self.button_add_to_table = QtGui.QPushButton(self.layoutWidget)
        self.button_add_to_table.setEnabled(False)
        self.button_add_to_table.setObjectName(_fromUtf8("button_add_to_table"))
        self.verticalLayout.addWidget(self.button_add_to_table)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.check_fly_to_buy = QtGui.QCheckBox(self.layoutWidget)
        self.check_fly_to_buy.setMinimumSize(QtCore.QSize(148, 18))
        self.check_fly_to_buy.setChecked(False)
        self.check_fly_to_buy.setObjectName(_fromUtf8("check_fly_to_buy"))
        self.verticalLayout.addWidget(self.check_fly_to_buy)
        self.gridLayout_7.addLayout(self.verticalLayout, 0, 0, 2, 1)
        self.groupBox_2 = QtGui.QGroupBox(self.layoutWidget)
        self.groupBox_2.setObjectName(_fromUtf8("groupBox_2"))
        self.layoutWidget1 = QtGui.QWidget(self.groupBox_2)
        self.layoutWidget1.setGeometry(QtCore.QRect(5, 20, 163, 81))
        self.layoutWidget1.setObjectName(_fromUtf8("layoutWidget1"))
        self.gridLayout_2 = QtGui.QGridLayout(self.layoutWidget1)
        self.gridLayout_2.setMargin(0)
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.spinBox_z_level_from = QtGui.QSpinBox(self.layoutWidget1)
        self.spinBox_z_level_from.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_z_level_from.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.spinBox_z_level_from.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_z_level_from.setMinimum(-99)
        self.spinBox_z_level_from.setObjectName(_fromUtf8("spinBox_z_level_from"))
        self.gridLayout_2.addWidget(self.spinBox_z_level_from, 0, 0, 1, 1)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_2.addItem(spacerItem1, 0, 1, 1, 1)
        self.label_4 = QtGui.QLabel(self.layoutWidget1)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout_2.addWidget(self.label_4, 0, 2, 1, 1)
        self.spinBox_z_level_to = QtGui.QSpinBox(self.layoutWidget1)
        self.spinBox_z_level_to.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_z_level_to.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_z_level_to.setMinimum(-99)
        self.spinBox_z_level_to.setObjectName(_fromUtf8("spinBox_z_level_to"))
        self.gridLayout_2.addWidget(self.spinBox_z_level_to, 1, 0, 1, 1)
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_2.addItem(spacerItem2, 1, 1, 1, 1)
        self.label_5 = QtGui.QLabel(self.layoutWidget1)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.gridLayout_2.addWidget(self.label_5, 1, 2, 1, 1)
        spacerItem3 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_2.addItem(spacerItem3, 2, 1, 1, 1)
        self.label_22 = QtGui.QLabel(self.layoutWidget1)
        self.label_22.setObjectName(_fromUtf8("label_22"))
        self.gridLayout_2.addWidget(self.label_22, 2, 2, 1, 1)
        self.intervall = QtGui.QDoubleSpinBox(self.layoutWidget1)
        self.intervall.setMinimumSize(QtCore.QSize(60, 0))
        self.intervall.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.intervall.setSingleStep(0.1)
        self.intervall.setProperty(_fromUtf8("value"), 0.1)
        self.intervall.setObjectName(_fromUtf8("intervall"))
        self.gridLayout_2.addWidget(self.intervall, 2, 0, 1, 1)
        self.gridLayout_7.addWidget(self.groupBox_2, 0, 1, 2, 1)
        self.groupBox_3 = QtGui.QGroupBox(self.layoutWidget)
        self.groupBox_3.setObjectName(_fromUtf8("groupBox_3"))
        self.gridLayout_5 = QtGui.QGridLayout(self.groupBox_3)
        self.gridLayout_5.setObjectName(_fromUtf8("gridLayout_5"))
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.spinBox_misalignment_x = QtGui.QDoubleSpinBox(self.groupBox_3)
        self.spinBox_misalignment_x.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_misalignment_x.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_misalignment_x.setDecimals(1)
        self.spinBox_misalignment_x.setMaximum(90.0)
        self.spinBox_misalignment_x.setProperty(_fromUtf8("value"), 0.0)
        self.spinBox_misalignment_x.setObjectName(_fromUtf8("spinBox_misalignment_x"))
        self.gridLayout.addWidget(self.spinBox_misalignment_x, 0, 0, 1, 1)
        self.label = QtGui.QLabel(self.groupBox_3)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 2, 1, 1)
        self.spinBox_misalignment_y = QtGui.QDoubleSpinBox(self.groupBox_3)
        self.spinBox_misalignment_y.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_misalignment_y.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_misalignment_y.setDecimals(1)
        self.spinBox_misalignment_y.setMaximum(90.0)
        self.spinBox_misalignment_y.setObjectName(_fromUtf8("spinBox_misalignment_y"))
        self.gridLayout.addWidget(self.spinBox_misalignment_y, 1, 0, 1, 1)
        self.label_2 = QtGui.QLabel(self.groupBox_3)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 2, 1, 1)
        self.spinBox_misalignment_z = QtGui.QDoubleSpinBox(self.groupBox_3)
        self.spinBox_misalignment_z.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_misalignment_z.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_misalignment_z.setDecimals(1)
        self.spinBox_misalignment_z.setMaximum(90.0)
        self.spinBox_misalignment_z.setObjectName(_fromUtf8("spinBox_misalignment_z"))
        self.gridLayout.addWidget(self.spinBox_misalignment_z, 2, 0, 1, 1)
        self.label_3 = QtGui.QLabel(self.groupBox_3)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 2, 2, 1, 1)
        spacerItem4 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem4, 0, 1, 1, 1)
        spacerItem5 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem5, 1, 1, 1, 1)
        spacerItem6 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem6, 2, 1, 1, 1)
        self.gridLayout_5.addLayout(self.gridLayout, 0, 0, 1, 1)
        self.gridLayout_7.addWidget(self.groupBox_3, 3, 1, 1, 1)
        spacerItem7 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.gridLayout_7.addItem(spacerItem7, 3, 0, 1, 1)
        spacerItem8 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.gridLayout_7.addItem(spacerItem8, 2, 1, 1, 1)
        self.gridLayout_8.addLayout(self.gridLayout_7, 0, 0, 1, 1)
        self.groupBox = QtGui.QGroupBox(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox.sizePolicy().hasHeightForWidth())
        self.groupBox.setSizePolicy(sizePolicy)
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.widget = QtGui.QWidget(self.groupBox)
        self.widget.setGeometry(QtCore.QRect(12, 19, 299, 236))
        self.widget.setObjectName(_fromUtf8("widget"))
        self.gridLayout_9 = QtGui.QGridLayout(self.widget)
        self.gridLayout_9.setMargin(0)
        self.gridLayout_9.setObjectName(_fromUtf8("gridLayout_9"))
        self.gridLayout_4 = QtGui.QGridLayout()
        self.gridLayout_4.setObjectName(_fromUtf8("gridLayout_4"))
        spacerItem9 = QtGui.QSpacerItem(13, 17, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_4.addItem(spacerItem9, 0, 0, 1, 1)
        self.spinBox_young_modulus = QtGui.QDoubleSpinBox(self.widget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_young_modulus.sizePolicy().hasHeightForWidth())
        self.spinBox_young_modulus.setSizePolicy(sizePolicy)
        self.spinBox_young_modulus.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_young_modulus.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_young_modulus.setMaximum(999999.99)
        self.spinBox_young_modulus.setProperty(_fromUtf8("value"), 70000.0)
        self.spinBox_young_modulus.setObjectName(_fromUtf8("spinBox_young_modulus"))
        self.gridLayout_4.addWidget(self.spinBox_young_modulus, 0, 1, 1, 1)
        self.label_6 = QtGui.QLabel(self.widget)
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.gridLayout_4.addWidget(self.label_6, 0, 2, 1, 1)
        spacerItem10 = QtGui.QSpacerItem(13, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_4.addItem(spacerItem10, 1, 0, 1, 1)
        self.spinBox_poisson_ratio = QtGui.QDoubleSpinBox(self.widget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_poisson_ratio.sizePolicy().hasHeightForWidth())
        self.spinBox_poisson_ratio.setSizePolicy(sizePolicy)
        self.spinBox_poisson_ratio.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_poisson_ratio.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_poisson_ratio.setSingleStep(0.1)
        self.spinBox_poisson_ratio.setProperty(_fromUtf8("value"), 0.3)
        self.spinBox_poisson_ratio.setObjectName(_fromUtf8("spinBox_poisson_ratio"))
        self.gridLayout_4.addWidget(self.spinBox_poisson_ratio, 1, 1, 1, 1)
        self.label_7 = QtGui.QLabel(self.widget)
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.gridLayout_4.addWidget(self.label_7, 1, 2, 1, 1)
        spacerItem11 = QtGui.QSpacerItem(13, 17, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_4.addItem(spacerItem11, 2, 0, 1, 1)
        self.spinBox_Plate_Thickness = QtGui.QDoubleSpinBox(self.widget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.spinBox_Plate_Thickness.sizePolicy().hasHeightForWidth())
        self.spinBox_Plate_Thickness.setSizePolicy(sizePolicy)
        self.spinBox_Plate_Thickness.setMinimumSize(QtCore.QSize(60, 20))
        self.spinBox_Plate_Thickness.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.spinBox_Plate_Thickness.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.spinBox_Plate_Thickness.setMaximum(500.0)
        self.spinBox_Plate_Thickness.setProperty(_fromUtf8("value"), 40.0)
        self.spinBox_Plate_Thickness.setObjectName(_fromUtf8("spinBox_Plate_Thickness"))
        self.gridLayout_4.addWidget(self.spinBox_Plate_Thickness, 2, 1, 1, 1)
        self.label_8 = QtGui.QLabel(self.widget)
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.gridLayout_4.addWidget(self.label_8, 2, 2, 1, 1)
        self.gridLayout_9.addLayout(self.gridLayout_4, 0, 0, 1, 1)
        self.gridLayout_3 = QtGui.QGridLayout()
        self.gridLayout_3.setObjectName(_fromUtf8("gridLayout_3"))
        self.gridLayout_6 = QtGui.QGridLayout()
        self.gridLayout_6.setObjectName(_fromUtf8("gridLayout_6"))
        self.lc1 = QtGui.QDoubleSpinBox(self.widget)
        self.lc1.setMinimumSize(QtCore.QSize(80, 0))
        self.lc1.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lc1.setDecimals(7)
        self.lc1.setMinimum(-99.999999)
        self.lc1.setMaximum(99.999999)
        self.lc1.setSingleStep(0.1)
        self.lc1.setObjectName(_fromUtf8("lc1"))
        self.gridLayout_6.addWidget(self.lc1, 0, 0, 1, 1)
        self.label_9 = QtGui.QLabel(self.widget)
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.gridLayout_6.addWidget(self.label_9, 0, 1, 1, 1)
        self.label_16 = QtGui.QLabel(self.widget)
        self.label_16.setObjectName(_fromUtf8("label_16"))
        self.gridLayout_6.addWidget(self.label_16, 0, 3, 1, 1)
        self.lc2 = QtGui.QDoubleSpinBox(self.widget)
        self.lc2.setMinimumSize(QtCore.QSize(80, 0))
        self.lc2.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lc2.setDecimals(7)
        self.lc2.setMinimum(-99.999999)
        self.lc2.setMaximum(99.999999)
        self.lc2.setSingleStep(0.1)
        self.lc2.setObjectName(_fromUtf8("lc2"))
        self.gridLayout_6.addWidget(self.lc2, 1, 0, 1, 1)
        self.label_12 = QtGui.QLabel(self.widget)
        self.label_12.setObjectName(_fromUtf8("label_12"))
        self.gridLayout_6.addWidget(self.label_12, 1, 1, 1, 1)
        self.ltc2 = QtGui.QDoubleSpinBox(self.widget)
        self.ltc2.setMinimumSize(QtCore.QSize(80, 0))
        self.ltc2.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.ltc2.setDecimals(7)
        self.ltc2.setMinimum(-99.999999)
        self.ltc2.setMaximum(99.999999)
        self.ltc2.setSingleStep(0.1)
        self.ltc2.setObjectName(_fromUtf8("ltc2"))
        self.gridLayout_6.addWidget(self.ltc2, 1, 2, 1, 1)
        self.label_13 = QtGui.QLabel(self.widget)
        self.label_13.setObjectName(_fromUtf8("label_13"))
        self.gridLayout_6.addWidget(self.label_13, 1, 3, 1, 1)
        self.lc3 = QtGui.QDoubleSpinBox(self.widget)
        self.lc3.setMinimumSize(QtCore.QSize(80, 0))
        self.lc3.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lc3.setDecimals(7)
        self.lc3.setMinimum(-99.999999)
        self.lc3.setMaximum(99.999999)
        self.lc3.setSingleStep(0.1)
        self.lc3.setObjectName(_fromUtf8("lc3"))
        self.gridLayout_6.addWidget(self.lc3, 2, 0, 1, 1)
        self.label_11 = QtGui.QLabel(self.widget)
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.gridLayout_6.addWidget(self.label_11, 2, 1, 1, 1)
        self.ltc3 = QtGui.QDoubleSpinBox(self.widget)
        self.ltc3.setMinimumSize(QtCore.QSize(80, 0))
        self.ltc3.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.ltc3.setDecimals(7)
        self.ltc3.setMinimum(-99.999999)
        self.ltc3.setMaximum(99.999999)
        self.ltc3.setSingleStep(0.1)
        self.ltc3.setObjectName(_fromUtf8("ltc3"))
        self.gridLayout_6.addWidget(self.ltc3, 2, 2, 1, 1)
        self.label_14 = QtGui.QLabel(self.widget)
        self.label_14.setObjectName(_fromUtf8("label_14"))
        self.gridLayout_6.addWidget(self.label_14, 2, 3, 1, 1)
        self.lc4 = QtGui.QDoubleSpinBox(self.widget)
        self.lc4.setMinimumSize(QtCore.QSize(80, 0))
        self.lc4.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lc4.setDecimals(7)
        self.lc4.setMinimum(-99.999999)
        self.lc4.setMaximum(99.999999)
        self.lc4.setSingleStep(0.1)
        self.lc4.setObjectName(_fromUtf8("lc4"))
        self.gridLayout_6.addWidget(self.lc4, 3, 0, 1, 1)
        self.label_10 = QtGui.QLabel(self.widget)
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.gridLayout_6.addWidget(self.label_10, 3, 1, 1, 1)
        self.ltc4 = QtGui.QDoubleSpinBox(self.widget)
        self.ltc4.setMinimumSize(QtCore.QSize(80, 0))
        self.ltc4.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.ltc4.setDecimals(7)
        self.ltc4.setMinimum(-99.999999)
        self.ltc4.setMaximum(99.999999)
        self.ltc4.setSingleStep(0.1)
        self.ltc4.setObjectName(_fromUtf8("ltc4"))
        self.gridLayout_6.addWidget(self.ltc4, 3, 2, 1, 1)
        self.label_15 = QtGui.QLabel(self.widget)
        self.label_15.setObjectName(_fromUtf8("label_15"))
        self.gridLayout_6.addWidget(self.label_15, 3, 3, 1, 1)
        self.lc5 = QtGui.QDoubleSpinBox(self.widget)
        self.lc5.setMinimumSize(QtCore.QSize(80, 0))
        self.lc5.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lc5.setDecimals(7)
        self.lc5.setMinimum(-99.999999)
        self.lc5.setMaximum(99.999999)
        self.lc5.setSingleStep(0.1)
        self.lc5.setObjectName(_fromUtf8("lc5"))
        self.gridLayout_6.addWidget(self.lc5, 4, 0, 1, 1)
        self.label_19 = QtGui.QLabel(self.widget)
        self.label_19.setObjectName(_fromUtf8("label_19"))
        self.gridLayout_6.addWidget(self.label_19, 4, 1, 1, 1)
        self.ltc5 = QtGui.QDoubleSpinBox(self.widget)
        self.ltc5.setMinimumSize(QtCore.QSize(80, 0))
        self.ltc5.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.ltc5.setDecimals(7)
        self.ltc5.setMinimum(-99.999999)
        self.ltc5.setMaximum(99.999999)
        self.ltc5.setSingleStep(0.1)
        self.ltc5.setObjectName(_fromUtf8("ltc5"))
        self.gridLayout_6.addWidget(self.ltc5, 4, 2, 1, 1)
        self.label_17 = QtGui.QLabel(self.widget)
        self.label_17.setObjectName(_fromUtf8("label_17"))
        self.gridLayout_6.addWidget(self.label_17, 4, 3, 1, 1)
        self.lc6 = QtGui.QDoubleSpinBox(self.widget)
        self.lc6.setMinimumSize(QtCore.QSize(80, 0))
        self.lc6.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.lc6.setDecimals(7)
        self.lc6.setMinimum(-99.999999)
        self.lc6.setMaximum(99.999999)
        self.lc6.setSingleStep(0.1)
        self.lc6.setObjectName(_fromUtf8("lc6"))
        self.gridLayout_6.addWidget(self.lc6, 5, 0, 1, 1)
        self.label_20 = QtGui.QLabel(self.widget)
        self.label_20.setObjectName(_fromUtf8("label_20"))
        self.gridLayout_6.addWidget(self.label_20, 5, 1, 1, 1)
        self.ltc6 = QtGui.QDoubleSpinBox(self.widget)
        self.ltc6.setMinimumSize(QtCore.QSize(80, 0))
        self.ltc6.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.ltc6.setDecimals(7)
        self.ltc6.setMinimum(-99.999999)
        self.ltc6.setMaximum(99.999999)
        self.ltc6.setSingleStep(0.1)
        self.ltc6.setObjectName(_fromUtf8("ltc6"))
        self.gridLayout_6.addWidget(self.ltc6, 5, 2, 1, 1)
        self.label_18 = QtGui.QLabel(self.widget)
        self.label_18.setObjectName(_fromUtf8("label_18"))
        self.gridLayout_6.addWidget(self.label_18, 5, 3, 1, 1)
        self.ltc1 = QtGui.QDoubleSpinBox(self.widget)
        self.ltc1.setMinimumSize(QtCore.QSize(80, 0))
        self.ltc1.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.ltc1.setDecimals(7)
        self.ltc1.setMinimum(-99.999999)
        self.ltc1.setMaximum(99.999999)
        self.ltc1.setSingleStep(0.1)
        self.ltc1.setObjectName(_fromUtf8("ltc1"))
        self.gridLayout_6.addWidget(self.ltc1, 0, 2, 1, 1)
        self.gridLayout_3.addLayout(self.gridLayout_6, 0, 0, 1, 1)
        spacerItem12 = QtGui.QSpacerItem(13, 149, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout_3.addItem(spacerItem12, 0, 1, 1, 1)
        self.label_21 = QtGui.QLabel(self.widget)
        self.label_21.setWordWrap(True)
        self.label_21.setObjectName(_fromUtf8("label_21"))
        self.gridLayout_3.addWidget(self.label_21, 0, 2, 1, 1)
        self.gridLayout_9.addLayout(self.gridLayout_3, 1, 0, 1, 1)
        self.gridLayout_8.addWidget(self.groupBox, 0, 1, 1, 1)

        self.retranslateUi(dialog)
        QtCore.QMetaObject.connectSlotsByName(dialog)
        dialog.setTabOrder(self.button_select_file, self.button_select_output)
        dialog.setTabOrder(self.button_select_output, self.check_fly_to_buy)
        dialog.setTabOrder(self.check_fly_to_buy, self.spinBox_z_level_from)
        dialog.setTabOrder(self.spinBox_z_level_from, self.spinBox_z_level_to)
        dialog.setTabOrder(self.spinBox_z_level_to, self.intervall)
        dialog.setTabOrder(self.intervall, self.spinBox_misalignment_x)
        dialog.setTabOrder(self.spinBox_misalignment_x, self.spinBox_misalignment_y)
        dialog.setTabOrder(self.spinBox_misalignment_y, self.spinBox_misalignment_z)
        dialog.setTabOrder(self.spinBox_misalignment_z, self.spinBox_young_modulus)
        dialog.setTabOrder(self.spinBox_young_modulus, self.spinBox_poisson_ratio)
        dialog.setTabOrder(self.spinBox_poisson_ratio, self.spinBox_Plate_Thickness)
        dialog.setTabOrder(self.spinBox_Plate_Thickness, self.lc1)
        dialog.setTabOrder(self.lc1, self.lc2)
        dialog.setTabOrder(self.lc2, self.lc3)
        dialog.setTabOrder(self.lc3, self.lc4)
        dialog.setTabOrder(self.lc4, self.lc5)
        dialog.setTabOrder(self.lc5, self.lc6)
        dialog.setTabOrder(self.lc6, self.ltc1)
        dialog.setTabOrder(self.ltc1, self.ltc2)
        dialog.setTabOrder(self.ltc2, self.ltc3)
        dialog.setTabOrder(self.ltc3, self.ltc4)
        dialog.setTabOrder(self.ltc4, self.ltc5)
        dialog.setTabOrder(self.ltc5, self.ltc6)
        dialog.setTabOrder(self.ltc6, self.button_add_to_table)
        dialog.setTabOrder(self.button_add_to_table, self.button_start_calculation)
        dialog.setTabOrder(self.button_start_calculation, self.button_dialog)
        dialog.setTabOrder(self.button_dialog, self.JobTable)

    def retranslateUi(self, dialog):
        dialog.setWindowTitle(QtGui.QApplication.translate("dialog", "Machining Distortion Prediction", None, QtGui.QApplication.UnicodeUTF8))
        self.button_start_calculation.setText(QtGui.QApplication.translate("dialog", "Start Calculation", None, QtGui.QApplication.UnicodeUTF8))
        self.button_select_file.setText(QtGui.QApplication.translate("dialog", "Select Nastran Geometry File", None, QtGui.QApplication.UnicodeUTF8))
        self.button_select_output.setText(QtGui.QApplication.translate("dialog", "Select Output Folder", None, QtGui.QApplication.UnicodeUTF8))
        self.button_add_to_table.setText(QtGui.QApplication.translate("dialog", "Add Job", None, QtGui.QApplication.UnicodeUTF8))
        self.check_fly_to_buy.setText(QtGui.QApplication.translate("dialog", "Calculate Buy to Fly Ratio", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_2.setTitle(QtGui.QApplication.translate("dialog", "Z-Level Offsets", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("dialog", "From", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("dialog", "To", None, QtGui.QApplication.UnicodeUTF8))
        self.label_22.setText(QtGui.QApplication.translate("dialog", "Intervall", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_3.setTitle(QtGui.QApplication.translate("dialog", "Set User Defined Misalignment", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("dialog", "Angle X-Axis", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("dialog", "Angle Y-Axis", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("dialog", "Angle Z-Axis", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("dialog", "Material Properties", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("dialog", "Young Modulus", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("dialog", "Poisson Ratio", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("dialog", "Plate Thickness", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("dialog", "LC1", None, QtGui.QApplication.UnicodeUTF8))
        self.label_16.setText(QtGui.QApplication.translate("dialog", "LTC1", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("dialog", "LC2", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("dialog", "LTC2", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("dialog", "LC3", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("dialog", "LTC3", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("dialog", "LC4", None, QtGui.QApplication.UnicodeUTF8))
        self.label_15.setText(QtGui.QApplication.translate("dialog", "LTC4", None, QtGui.QApplication.UnicodeUTF8))
        self.label_19.setText(QtGui.QApplication.translate("dialog", "LC5", None, QtGui.QApplication.UnicodeUTF8))
        self.label_17.setText(QtGui.QApplication.translate("dialog", "LTC5", None, QtGui.QApplication.UnicodeUTF8))
        self.label_20.setText(QtGui.QApplication.translate("dialog", "LC6", None, QtGui.QApplication.UnicodeUTF8))
        self.label_18.setText(QtGui.QApplication.translate("dialog", "LTC6", None, QtGui.QApplication.UnicodeUTF8))
        self.label_21.setText(QtGui.QApplication.translate("dialog", "Interpolation Coefficients", None, QtGui.QApplication.UnicodeUTF8))

