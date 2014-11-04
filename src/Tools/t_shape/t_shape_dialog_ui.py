# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 't_shape_dialog.ui'
#
# Created: Tue Nov  4 16:52:09 2014
#      by: PyQt4 UI code generator 4.9.6
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(349, 283)
        self.gridLayout_3 = QtGui.QGridLayout(Dialog)
        self.gridLayout_3.setObjectName(_fromUtf8("gridLayout_3"))
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.doubleSpinBox_2 = QtGui.QDoubleSpinBox(Dialog)
        self.doubleSpinBox_2.setObjectName(_fromUtf8("doubleSpinBox_2"))
        self.gridLayout.addWidget(self.doubleSpinBox_2, 2, 1, 1, 1)
        self.label = QtGui.QLabel(Dialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.doubleSpinBox_3 = QtGui.QDoubleSpinBox(Dialog)
        self.doubleSpinBox_3.setObjectName(_fromUtf8("doubleSpinBox_3"))
        self.gridLayout.addWidget(self.doubleSpinBox_3, 3, 1, 1, 1)
        self.doubleSpinBox_4 = QtGui.QDoubleSpinBox(Dialog)
        self.doubleSpinBox_4.setObjectName(_fromUtf8("doubleSpinBox_4"))
        self.gridLayout.addWidget(self.doubleSpinBox_4, 4, 1, 1, 1)
        self.doubleSpinBox = QtGui.QDoubleSpinBox(Dialog)
        self.doubleSpinBox.setObjectName(_fromUtf8("doubleSpinBox"))
        self.gridLayout.addWidget(self.doubleSpinBox, 0, 1, 1, 1)
        self.label_3 = QtGui.QLabel(Dialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 3, 0, 1, 1)
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 2, 0, 1, 1)
        self.label_4 = QtGui.QLabel(Dialog)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 4, 0, 1, 1)
        self.gridLayout_3.addLayout(self.gridLayout, 0, 0, 1, 1)
        self.gridLayout_2 = QtGui.QGridLayout()
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.checkBox = QtGui.QCheckBox(Dialog)
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.gridLayout_2.addWidget(self.checkBox, 0, 0, 1, 1)
        self.label_5 = QtGui.QLabel(Dialog)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.gridLayout_2.addWidget(self.label_5, 1, 0, 1, 1)
        self.doubleSpinBox_5 = QtGui.QDoubleSpinBox(Dialog)
        self.doubleSpinBox_5.setObjectName(_fromUtf8("doubleSpinBox_5"))
        self.gridLayout_2.addWidget(self.doubleSpinBox_5, 1, 1, 1, 1)
        self.gridLayout_3.addLayout(self.gridLayout_2, 1, 0, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(Dialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout_3.addWidget(self.buttonBox, 2, 0, 1, 1)

        self.retranslateUi(Dialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), Dialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialog.reject)
        QtCore.QObject.connect(self.checkBox, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.label_5.setEnabled)
        QtCore.QObject.connect(self.checkBox, QtCore.SIGNAL(_fromUtf8("clicked(bool)")), self.doubleSpinBox_5.setEnabled)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "Dialog", None))
        self.label.setText(_translate("Dialog", "Big pipe radius", None))
        self.label_3.setText(_translate("Dialog", "Big pipe  height", None))
        self.label_2.setText(_translate("Dialog", "Small pipe  radius", None))
        self.label_4.setText(_translate("Dialog", "Small  pipe height", None))
        self.checkBox.setText(_translate("Dialog", "Build solid part", None))
        self.label_5.setText(_translate("Dialog", "Solid thickness", None))
        
if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    Dialog = QtGui.QDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())


 
