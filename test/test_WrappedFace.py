#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2026  CEA/DEN, EDF R&D, OPEN CASCADE
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

import sys
import salome
salome.salome_init()
from salome.geom import geomBuilder
import math
from inspect import getfile
from os.path import abspath, dirname, join

geompy = geomBuilder.New()

data_dir = abspath(join(dirname(getfile(lambda: None)), 'data'))

myInitFace = geompy.ImportBREP(join(data_dir, 'WrappedFace_sphere_face.brep'))
listE = geompy.ExtractShapes(myInitFace, geompy.ShapeType["EDGE"], True)

myVertices = geompy.ImportBREP(join(data_dir, 'WrappedFace_sphere_vertices.brep'))
lVertices = geompy.SubShapeAll(myVertices, geompy.ShapeType["VERTEX"])

wrappedFaceSphere = geompy.MakeWrappedFace(listE, lVertices, 0.01)
wrappedAreaSphere = geompy.BasicProperties(wrappedFaceSphere)[1]

eps = 1e-5
expectedArea = 47123.88980388897
print(wrappedAreaSphere)
assert(math.fabs(expectedArea - wrappedAreaSphere) < eps)

###########################################################################

myEdges = geompy.ImportBREP(join(data_dir, 'WrappedFace_bspline_edges.brep'))
listE2 = geompy.ExtractShapes(myEdges, geompy.ShapeType["EDGE"], True)

myVertices2 = geompy.ImportBREP(join(data_dir, 'WrappedFace_bspline_vertices.brep'))
lVertices2 = geompy.SubShapeAll(myVertices2, geompy.ShapeType["VERTEX"])

wrappedFace = geompy.MakeWrappedFace(listE2, lVertices2, 0.01)
wrappedArea = geompy.BasicProperties(wrappedFace)[1]

expectedArea = 15702.653817260858
print(wrappedArea)
assert(math.fabs(expectedArea - wrappedArea) < eps)


