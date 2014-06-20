# -*- coding: utf-8 -*-
# Copyright (C) 2010-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
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
   
    def accept(self):
      print "DATA ACCEPTED"
      self._wasOk = True
      QtGui.QDialog.accept(self)
      
    def getData(self):
      print "DATA : %f"%(self.ui.doubleSpinBox.value()) 
      
    #def setupUi(self):
        #TShapeDialog_UI.setupUi(self)
        #self.handleAcceptWith(self.accept)
        #self.handleRejectWith(self.reject)

    #def handleAcceptWith(self,callbackFunction):
        #"""This defines the function to be connected to the signal 'accepted()' (click on Ok)"""
        #QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), callbackFunction)

    #def handleRejectWith(self,callbackFunction):
        #"""This defines the function to be connected to the signal 'rejected()' (click on Cancel)"""
        #QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), callbackFunction)

    #def accept(self):
        #'''Callback function when dialog is accepted (click Ok)'''
        #self._wasOk = True
        ## We should test here the validity of values
        #QtGui.QDialog.accept(self)

    #def reject(self):
        #'''Callback function when dialog is rejected (click Cancel)'''
        #self._wasOk = False
        #QtGui.QDialog.reject(self)

    #def wasOk(self):
        #return self._wasOk

    #def setData(self):
      #pass

    #def getData(self):
       #pass
     
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
