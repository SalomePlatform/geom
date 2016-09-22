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

import salome_pluginsmanager

def t_shape_fluid(context):
    activeStudy = context.study

    #from salome.geom.t_shape import t_shape_builder
    from salome.geom.t_shape import t_shape_dialog
    dialog = t_shape_dialog.getDialog()
    dialog.setContext(context)
    dialog.show()

salome_pluginsmanager.AddFunction('T shape fluid',
                                  'Creates the fluid part of a pipe T-shape',
                                  t_shape_fluid)
