# -*- coding: utf-8 -*-
# Copyright (C) 2014-2016  EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : Renaud Nédélec (OpenCascade S.A.S)

import sys
from PyQt4 import QtGui
from PyQt4 import QtCore

from t_shape_dialog_ui import Ui_Dialog


class TShapeDialog(QtGui.QDialog):
    def __init__(self):
      QtGui.QDialog.__init__(self, None, QtCore.Qt.Tool)
      # Set up the user interface from Designer.
      self.ui = Ui_Dialog()
      self.ui.setupUi(self)
      self.show()
      self._wasOk = False
      self.ui.dsb_solidThickness.setEnabled(False)
      self.ui.label_5.setEnabled(False)
      self.ui.dsb_bigRadius.setValue(50.0)
      self.ui.dsb_smallRadius.setValue(40.0)
      self.ui.dsb_bigHeight.setValue(80.0)
      self.ui.dsb_smallHeight.setValue(80.0)
   
    def accept(self):
      self._wasOk = True
      QtGui.QDialog.accept(self)
      
    def getData(self):
      r1 = self.ui.dsb_bigRadius.value()
      r2 = self.ui.dsb_smallRadius.value()
      h1 = self.ui.dsb_bigHeight.value()
      h2 = self.ui.dsb_smallHeight.value()
      thickness = 0.0
      if self.ui.cb_buildSolid.isChecked():
        thickness = self.ui.dsb_solidThickness.value()
      
      return r1, r2, h1, h2, thickness

    def reject(self):
        self._wasOk = False
        QtGui.QDialog.reject(self)

    def wasOk(self):
        return self._wasOk

     
# ================     
# Tests
# ================

def main( args ):
    import sys
    app = QtGui.QApplication(sys.argv)
    Dialog = TShapeDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())

if __name__=="__main__":
    main(sys.argv)
