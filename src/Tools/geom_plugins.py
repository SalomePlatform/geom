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

import salome_pluginsmanager

def t_shape_fluid(context):
    from salome.geom.t_shape import t_shape_builder
    from salome.geom.t_shape import t_shape_dialog
    import xalome
    from PyQt4.QtGui import QMessageBox
    activeStudy = context.study
    
    dialog = t_shape_dialog.TShapeDialog()

    # Get the parameter values from a gui dialog box. If the dialog is
    # closed using the Ok button, then the data are requested from the
    # gui and used to create the shape of the tube.
    dialog.exec_()
    if dialog.wasOk():
      r1, r2, h1, h2, thickness = dialog.getData()
      #QMessageBox.about(None, "Building in progress", "building shape, please be patient")
      shape = t_shape_builder.build_shape(activeStudy, r1, r2, h1, h2, thickness)
      entry = xalome.addToStudy(activeStudy, shape, "T_shape_fluid" )
      xalome.displayShape(entry)
    #if dialog.wasOk():
        #radius, length, width = dialog.getData()
        #shape = tubebuilder.createGeometry(activeStudy, radius, length, width)
        #entry = xalome.addToStudy(activeStudy, shape, "Tube" )
        #xalome.displayShape(entry)


salome_pluginsmanager.AddFunction('T shape fluid',
                                  'Creates the fluid part of a pipe T-shape',
                                  t_shape_fluid)