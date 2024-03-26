#!/usr/bin/env python

import sys
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
import math
from inspect import getfile
from os.path import abspath, dirname, join
import SALOMEDS

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


