#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#  File   : GEOM_example6.py
#  Author : Dmitry MATVEITChEV
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

ind = 1
circlelist = []
while ind < 6:
    x1 = 0. + (10. * ind)
    y1 = 0.
    z1 = 0.
    x2 = 10. + (10. * ind)
    y2 = 20. * (ind+1)
    z2 = 30. * (ind+1)
    x3 = 50. + (10. * ind)
    y3 = 0. * (ind+1)
    z3 = -10. * (ind+1)

    print x1, y1, z1, x2, y2, z2, x3, y3, z3

    point1 = geompy.MakeVertex(x1, y1, z1)
    name1 = "point1_%d"%(ind)
    id_pt1 = geompy.addToStudy(point1, name1)
    point2 = geompy.MakeVertex(x2, y2, z2)
    name2 = "point2_%d"%(ind)
    id_pt2 = geompy.addToStudy(point2, name2)
    point3 = geompy.MakeVertex(x3, y3, z3)
    name3 = "point3_%d"%(ind)
    id_pt3 = geompy.addToStudy(point3, name3)

    name = "circle%d"%(ind)
    circle = geompy.MakeCircleCenter2Pnt(point1, point2, point3)
    id_circle = geompy.addToStudy(circle, name)
    circlelist.append(circle)
    ind = ind + 1
