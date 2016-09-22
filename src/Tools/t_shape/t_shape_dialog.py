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
from qtsalome import *

from t_shape_dialog_ui import Ui_Dialog


class TShapeDialog(Ui_Dialog,QWidget):
    def __init__(self):
      QWidget.__init__(self)
      # Set up the user interface from Designer.
      #self = Ui_Dialog()
      self.setupUi(self)
      self.show()
      self._wasOk = False
      self.dsb_solidThickness.setEnabled(False)
      self.label_5.setEnabled(False)
      self.dsb_bigRadius.setValue(50.0)
      self.dsb_smallRadius.setValue(40.0)
      self.dsb_bigHeight.setValue(80.0)
      self.dsb_smallHeight.setValue(80.0)
   
    def setContext(self, context):
      self._activeStudy = context.study
      
    def accept(self):
      from salome.geom.t_shape import t_shape_progress
      import xalome
      self._wasOk = True
      self.close()
      r1, r2, h1, h2, thickness = self.getData()
      #QMessageBox.about(None, "Building in progress", "building shape, please be patient")
      #shape = t_shape_builder.build_shape(activeStudy, r1, r2, h1, h2, thickness)
      shapeBuilder = t_shape_progress.t_shape_progress()
      shape = shapeBuilder.run(self._activeStudy, r1, r2, h1, h2, thickness)
      entry = xalome.addToStudy(self._activeStudy, shape, "T_shape_fluid" )
      xalome.displayShape(entry)

    def getData(self):
      r1 = self.dsb_bigRadius.value()
      r2 = self.dsb_smallRadius.value()
      h1 = self.dsb_bigHeight.value()
      h2 = self.dsb_smallHeight.value()
      thickness = 0.0
      if self.cb_buildSolid.isChecked():
        thickness = self.dsb_solidThickness.value()
      
      return r1, r2, h1, h2, thickness

    def reject(self):
      self._wasOk = False
      self.close()

    def wasOk(self):
      return self._wasOk

__dialog=None
def getDialog():
    """
    This function returns a singleton instance of the plugin dialog.
    It is mandatory in order to call show without a parent ...
    """
    global __dialog
    if __dialog is None:
      __dialog = TShapeDialog()
    return __dialog
    
# ================     
# Tests
# ================

def main( args ):
    import sys
    app = QApplication(sys.argv)
    Dialog = TShapeDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    #sys.exit(app.exec_())

if __name__=="__main__":
    main(sys.argv)
