#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#
#
#  File   : GEOM_example.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM
#  $Header$

import salome
import geompy

ind = 0
boxlist = []
while ind < 5:
    x1 = 10. * ind
    y1 = 0.
    z1 = 0.
    x2 = 10. * (ind+1)
    y2 = 20. * (ind+1)
    z2 = 30. * (ind+1)
    print x1, y1, z1, x2, y2, z2

    point1 = geompy.MakeVertex(x1, y1, z1)
    name1 = "point1_%d"%(ind)
    id_pt1 = geompy.addToStudy(point1, name1)
    point2 = geompy.MakeVertex(x2, y2, z2)
    name2 = "point2_%d"%(ind)
    id_pt2 = geompy.addToStudy(point2, name2)

    name = "box%d"%(ind)
    box = geompy.MakeBoxTwoPnt(point1, point2)
    id_box = geompy.addToStudy(box, name)
    boxlist.append(box)
    ind = ind + 1
