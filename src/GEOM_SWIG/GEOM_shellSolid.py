#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#==============================================================================
#  Info.
#  Bug (from script, bug)   : shellSolid_modified.py, PAL6611
#  Modified                 : 25/11/2004
#  Author                   : Kovaltchuk Alexey
#  Project                  : PAL/SALOME
#==============================================================================
# Imports
# -------
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Interface with geometry
# -----------------------

#geom = salome.lcc.FindOrLoadComponent("FactoryServer", "GEOM")
geomgui = salome.ImportComponentGUI("GEOM")

# Interface with study
# --------------------

def addToStudy(shape, name):
    i = geompy.addToStudy(shape, name)
    salome.sg.updateObjBrowser(False)
#    geomgui.createAndDisplayGO(i)
    return i

def setName(obj):
    obj.SetName(salome.orb.object_to_string(obj))


def MakeVertex(x, y, z):
    v = geompy.MakeVertex(x, y, z)
    setName(v)
    return v

def MakeEdge(v1, v2):
    e = geompy.MakeEdge(v1, v2)
    setName(e)
    return e

def MakeArc(f, p, t):
    e = geompy.MakeArc(f, p, t)
    setName(e)
    return e

def MakeArcCenter(c, f, t):
    p = fkl(c, f, t)
    e = geompy.MakeArc(f, p, t)
    setName(e)
    return e

# Face with 4 edges
# -----------------

# cf. makeTiledSurfByContour

def MakeQuadFace(e1, e2, e3, e4):
    l = []
    l.append(e1)
    l.append(e2)
    l.append(e3)
    l.append(e4)
    w = geompy.MakeWire(l)
    f = geompy.MakeFace(w, 1)
    setName(f)
    return f

# cf. makeTiledSurfBetweenTwolines

def MakeQuadFaceEdge(e1, e2):
    return fkl

# cf. makeTiledSurfByPointsOfContour
# cf. makeMesh

def MakeQuadFaceVertex(v1, v2, v3, v4):
    l = []
    e = MakeEdge(v1, v2)
    l.append(e)
    e = MakeEdge(v2, v3)
    l.append(e)
    e = MakeEdge(v3, v4)
    l.append(e)
    e = MakeEdge(v4, v1)
    l.append(e)
    w = geompy.MakeWire(l)
    f = geompy.MakeFace(w, 1)
    setName(f)
    return f

# Solid with 6 faces
# ------------------

# cf. makePavedVolWithContour
# cf. makeVolWithEnvolope

def MakeHexaSolid(f1, f2, f3, f4, f5, f6):
    l = []
    l.append(f1)
    l.append(f2)
    l.append(f3)
    l.append(f4)
    l.append(f5)
    l.append(f6)
    s = geompy.MakeShell(l)
# FKL: bad shell
    l = []
    l.append(s)
    s = geompy.MakeSolid(l)
# FKL: MakeSolid crash
    setName(s)
    return s

# cf. makeVolBetweenTwoSurfaces

def MakeHexaSolidFace(f1, f2):
    return fkl

# Face (with 4 edges) and solid (with 6 faces)
# --------------------------------------------

# makeTiledSurfByRotationOfLine

def MakeRevolution(g, pt, ve, angle):
    """
    Creates a face  by rotation of an edge with an angle around an axis defined by a point and a vector or
    creates a solid by rotation of a  face with an angle around an axis defined by a point and a vector
    """
    axis = geompy.MakeAxisStruct(pt.x, pt.y, pt.z, ve.x, ve.y, ve.z)
    s = geompy.MakeRevolution(g, axis, angle)
    setName(s)
    return s

# cf. makeSurfSewingTwolines

def MakeSewing(ls):
    """
    Creates a face  by sewing common edges between a list of faces or
    Creates a solid by sewing common faces between a list of solids
    """
    s = geompy.MakeSewing(ls)
    setName(s)
    return s

# cf. makeIntersectionBetweenTwoMeshes

def MakeCommon(s1, s2):
    """
    Creates the common face  between 2 faces or
    Creates the common solid between 2 solids
    """
    s = geompy.MakeBoolean(s1, s2, 1)
    setName(s)
    return s

# cf. makeMerge

def MakeFuse(s1, s2):
    """
    Fuses 2 faces or 2 solids
    """
    s = geompy.MakeBoolean(s1, s2, 3)
    setName(s)
    return s

# Additional

def MakeCut(s1, s2):
    """
    Cuts 2 faces or 2 solids
    """
    s = geompy.MakeBoolean(s1, s2, 2)
    setName(s)
    return s

# Edge, face (with 4 edges) and solid (with 6 faces)
# --------------------------------------------------

# cf. makeSurfByTranslation
# cf. makeVolByTranslation

def MakePrism(s, v):
    """
    Creates a edge  defined by a vertex and along a vector
    Creates a face  defined by a edge   and along a vector
    Creates a solid defined by a face   and along a vector
    """
    r = geompy.MakePrism(s, point(0, 0, 0), v)
    setName(r)
    return r

# cf. homothety

def MakeScaleTransform(s, center, factor):
    """
    Creates a homothety of a geometric object
    """
    r = geompy.MakeScaleTransform(s, center, factor)
    setName(r)
    return r

# Vertex, edge, face (with 4 edges) and solid (with 6 faces)
# ----------------------------------------------------------

# cf. translate

def MakeTranslation(s, vx, vy, vz):
    """
    Translates a vertex, an edge, a face or a solid
    """
    r = geompy.MakeTranslation(s, vx, vy, vz)
    setName(r)
    return r

# cf. rotation

def MakeRotation(g, pt, ve, angle):
    """
    Creates a rotation of the geometric object with an angle around an axis defined by a point and a vector
    """
    axis = geompy.MakeAxisStruct(pt.x, pt.y, pt.z, ve.x, ve.y, ve.z)
    s = geompy.MakeRotation(g, axis, angle)
    setName(s)
    return s

# cf. symmetryByPoint

def MakeMirrorByPoint(s, point):
    """
    Creates a symmetric object by point symetry
    """
    return fkl

# cf. symmetryByLine

def MakeMirrorByLine(s, line):
    """
    Creates a symmetric object by line symetry
    """
    return fkl

# cf. symmetryByPlane

def MakeMirrorByPlane(s, pt, ve):
    """
    Creates a symmetric object by plane symetry defined by a point and a normal vector
    """
    p = plane(pt, ve)
    r = geompy.MakeMirrorByplane(s, p)
    setName(r)
    return r

# Compound
# --------

# Additional

def BlockMakeCompound(ls):
    """
    Creates a compound defined by a list and glue faces
    """
    c = MakeCompound(ls)
    r = MakeGlueFaces(c, 1.e-5)
    setName(r)
    return r

# Boolean operators
# -----------------

def mergePointsNearBy(l, v):
    """
    Modifies a list of shape by replacing all vertex near a new vextex
    """
    return fkl

# Acces operators
# ---------------

def getCoords(v):
    """
    Gets the coordinates of the vertex
    """
    return fkl

def extractBeginPoint(e):
    """
    Gets the first point of an edge
    """
    return fkl

def extractEndPoint(e):
    """
    Gets the second point of an edge
    """
    return fkl

# fkl more functionalities

# Addionnal
# =========

# Check geometry
# --------------

def CheckQuadFace(f):
    """
    Checks if a face has 4 edges
    """
    return fkl

def CheckHexaSolid(s):
    """
    Checks if a solid has 6 faces and each faces have 4 edges
    """
    return fkl

# fkl more if other kind of solid like: tetra, penta, etc

# point for building
# ------------------

def point(x, y, z):
    p = geompy.MakeVertex(x, y, z)
    return p

def plane(pt, dir):
    p = geompy.MakePlane(pt, d, 100)
    return p

# Solid
# -----

def MakeCylinder(center, dir, radius, height):
    s = geompy.MakeCylinder(center, dir, radius, height)
    setName(s)
    return s

def MakeBox(p1, p2):
    s = geompy.MakeBoxTwoPnt(p1, p2)
    setName(s)
    return s

# Compound
# --------

def MakePartitionList(solids, tools):
    """
    Creates a list of shape by a partition of a list of solids by a list of tools
    """
    p = geompy.Partition(solids, tools, [], [], geompy.ShapeType["SHAPE"]);
    l = geompy.SubShapeAll(p, geompy.ShapeType["SHAPE"])
    return l

def MakePartition(solids, tools):
    """
    Creates a partition of a list of solids by a list of tools
    """
    l = MakePartitionList(solids, tools)
    c = MakeCompound(l)
    setName(c)
    return c

def BlockMakeMultiTranslation1D(shape, dir, step, times):
    m = geompy.MakeMultiTranslation1D(shape, dir, step, times)
    c  = MakeGlueFaces(m, 1.e-5)
    setName(c)
    return c

def BlockMakeMultiTranslation2D(shape, dir1, step1, times1, dir2, step2, times2):
    m  = geompy.MakeMultiTranslation2D(shape, dir1, step1, times1, dir2, step2, times2)
    c  = MakeGlueFaces(m, 1.e-5)
    setName(c)
    return c

def BlockMakeMultiRotation1D(shape, dir, point, times):
    m = geompy.MakeMultiRotation1D(shape, dir, point, times)
    c  = MakeGlueFaces(m, 1.e-5)
    setName(c)
    return c

def BlockMakeMultiRotation2D(shape, dir, point, angle, times1, step, times2):
    m = geompy.MakeMultiRotation2D(shape, dir, point, angle, times1, step, times2)
    c  = MakeGlueFaces(m, 1.e-5)
    setName(c)
    return c

# internal
# --------

def MakeCompound(ls):
    """
    Creates a compound defined by a list
    """
    c = geompy.MakeCompound(ls)
    return c

def MakeSewingShape(s, eps):
    """
    Creates a shape fully sewed
    """
    r = geompy.MakeSewingShape(s, eps)
    return r

def MakeGlueFaces(s, eps):
    """
    Touched faces are replaced by one
    """
    r = geompy.MakeGlueFaces(s, eps)
    return r


# Geometry
# ========

# A small cube centered and put on a great cube

# Points
# ------

greatPoint111 = point( 0,  0,  0)
greatPoint211 = point(10,  0,  0)
greatPoint311 = point(20,  0,  0)
greatPoint411 = point(30,  0,  0)

greatPoint121 = point( 0, 10,  0)
greatPoint221 = point(10, 10,  0)
greatPoint321 = point(20, 10,  0)
greatPoint421 = point(30, 10,  0)

greatPoint112 = point( 0,  0, 10)
greatPoint212 = point(10,  0, 10)
greatPoint312 = point(20,  0, 10)
greatPoint412 = point(30,  0, 10)

greatPoint122 = point( 0, 10, 10)
greatPoint222 = point(10, 10, 10)
greatPoint322 = point(20, 10, 10)
greatPoint422 = point(30, 10, 10)

greatPoint113 = point( 0,  0, 20)
greatPoint213 = point(10,  0, 20)
greatPoint313 = point(20,  0, 20)
greatPoint413 = point(30,  0, 20)

greatPoint123 = point( 0, 10, 20)
greatPoint223 = point(10, 10, 20)
greatPoint323 = point(20, 10, 20)
greatPoint423 = point(30, 10, 20)

greatPoint114 = point( 0,  0, 30)
greatPoint214 = point(10,  0, 30)
greatPoint314 = point(20,  0, 30)
greatPoint414 = point(30,  0, 30)

greatPoint124 = point( 0, 10, 30)
greatPoint224 = point(10, 10, 30)
greatPoint324 = point(20, 10, 30)
greatPoint424 = point(30, 10, 30)


smallPoint111 = greatPoint222
smallPoint211 = greatPoint322
smallPoint121 = point(10, 20, 10)
smallPoint221 = point(20, 20, 10)

smallPoint112 = greatPoint223
smallPoint212 = greatPoint323
smallPoint122 = point(10, 20, 20)
smallPoint222 = point(20, 20, 20)

# Edges
# -----

smallEdgeX11 = MakeEdge(smallPoint111, smallPoint211)
smallEdgeX21 = MakeEdge(smallPoint121, smallPoint221)
smallEdgeX12 = MakeEdge(smallPoint112, smallPoint212)
smallEdgeX22 = MakeEdge(smallPoint122, smallPoint222)

smallEdgeY11 = MakeEdge(smallPoint111, smallPoint121)
smallEdgeY21 = MakeEdge(smallPoint211, smallPoint221)
smallEdgeY12 = MakeEdge(smallPoint112, smallPoint122)
smallEdgeY22 = MakeEdge(smallPoint212, smallPoint222)

smallEdgeZ11 = MakeEdge(smallPoint111, smallPoint112)
smallEdgeZ21 = MakeEdge(smallPoint211, smallPoint212)
smallEdgeZ12 = MakeEdge(smallPoint121, smallPoint122)
smallEdgeZ22 = MakeEdge(smallPoint221, smallPoint222)


greatEdgeX111 = MakeEdge(greatPoint111, greatPoint211)
greatEdgeX211 = MakeEdge(greatPoint211, greatPoint311)
greatEdgeX311 = MakeEdge(greatPoint311, greatPoint411)
greatEdgeX121 = MakeEdge(greatPoint121, greatPoint221)
greatEdgeX221 = MakeEdge(greatPoint221, greatPoint321)
greatEdgeX321 = MakeEdge(greatPoint321, greatPoint421)

greatEdgeX112 = MakeEdge(greatPoint112, greatPoint212)
greatEdgeX212 = MakeEdge(greatPoint212, greatPoint312)
greatEdgeX312 = MakeEdge(greatPoint312, greatPoint412)
greatEdgeX122 = MakeEdge(greatPoint122, greatPoint222)
greatEdgeX222 = smallEdgeX11
greatEdgeX322 = MakeEdge(greatPoint322, greatPoint422)

greatEdgeX113 = MakeEdge(greatPoint113, greatPoint213)
greatEdgeX213 = MakeEdge(greatPoint213, greatPoint313)
greatEdgeX313 = MakeEdge(greatPoint313, greatPoint413)
greatEdgeX123 = MakeEdge(greatPoint123, greatPoint223)
greatEdgeX223 = smallEdgeX12
greatEdgeX323 = MakeEdge(greatPoint323, greatPoint423)

greatEdgeX114 = MakeEdge(greatPoint114, greatPoint214)
greatEdgeX214 = MakeEdge(greatPoint214, greatPoint314)
greatEdgeX314 = MakeEdge(greatPoint314, greatPoint414)
greatEdgeX124 = MakeEdge(greatPoint124, greatPoint224)
greatEdgeX224 = MakeEdge(greatPoint224, greatPoint324)
greatEdgeX324 = MakeEdge(greatPoint324, greatPoint424)

greatEdgeY11 = MakeEdge(greatPoint111, greatPoint121)
greatEdgeY21 = MakeEdge(greatPoint211, greatPoint221)
greatEdgeY31 = MakeEdge(greatPoint311, greatPoint321)
greatEdgeY41 = MakeEdge(greatPoint411, greatPoint421)

greatEdgeY12 = MakeEdge(greatPoint112, greatPoint122)
greatEdgeY22 = MakeEdge(greatPoint212, greatPoint222)
greatEdgeY32 = MakeEdge(greatPoint312, greatPoint322)
greatEdgeY42 = MakeEdge(greatPoint412, greatPoint422)

greatEdgeY13 = MakeEdge(greatPoint113, greatPoint123)
greatEdgeY23 = MakeEdge(greatPoint213, greatPoint223)
greatEdgeY33 = MakeEdge(greatPoint313, greatPoint323)
greatEdgeY43 = MakeEdge(greatPoint413, greatPoint423)

greatEdgeY14 = MakeEdge(greatPoint114, greatPoint124)
greatEdgeY24 = MakeEdge(greatPoint214, greatPoint224)
greatEdgeY34 = MakeEdge(greatPoint314, greatPoint324)
greatEdgeY44 = MakeEdge(greatPoint414, greatPoint424)

greatEdgeZ111 = MakeEdge(greatPoint111, greatPoint112)
greatEdgeZ211 = MakeEdge(greatPoint211, greatPoint212)
greatEdgeZ311 = MakeEdge(greatPoint311, greatPoint312)
greatEdgeZ411 = MakeEdge(greatPoint411, greatPoint412)

greatEdgeZ121 = MakeEdge(greatPoint121, greatPoint122)
greatEdgeZ221 = MakeEdge(greatPoint221, greatPoint222)
greatEdgeZ321 = MakeEdge(greatPoint321, greatPoint322)
greatEdgeZ421 = MakeEdge(greatPoint421, greatPoint422)

greatEdgeZ112 = MakeEdge(greatPoint112, greatPoint113)
greatEdgeZ212 = MakeEdge(greatPoint212, greatPoint213)
greatEdgeZ312 = MakeEdge(greatPoint312, greatPoint313)
greatEdgeZ412 = MakeEdge(greatPoint412, greatPoint413)

greatEdgeZ122 = MakeEdge(greatPoint122, greatPoint123)
greatEdgeZ222 = smallEdgeZ11
greatEdgeZ322 = smallEdgeZ21
greatEdgeZ422 = MakeEdge(greatPoint422, greatPoint423)

greatEdgeZ113 = MakeEdge(greatPoint113, greatPoint114)
greatEdgeZ213 = MakeEdge(greatPoint213, greatPoint214)
greatEdgeZ313 = MakeEdge(greatPoint313, greatPoint314)
greatEdgeZ413 = MakeEdge(greatPoint413, greatPoint414)

greatEdgeZ123 = MakeEdge(greatPoint123, greatPoint124)
greatEdgeZ223 = MakeEdge(greatPoint223, greatPoint224)
greatEdgeZ323 = MakeEdge(greatPoint323, greatPoint324)
greatEdgeZ423 = MakeEdge(greatPoint423, greatPoint424)

# Faces
# -----

smallFaceX1 = MakeQuadFace(smallEdgeY11, smallEdgeZ11, smallEdgeY12, smallEdgeZ12)
smallFaceX2 = MakeQuadFace(smallEdgeY21, smallEdgeZ21, smallEdgeY22, smallEdgeZ22)
smallFaceY1 = MakeQuadFace(smallEdgeX11, smallEdgeZ11, smallEdgeX12, smallEdgeZ21)
smallFaceY2 = MakeQuadFace(smallEdgeX21, smallEdgeZ12, smallEdgeX22, smallEdgeZ22)
smallFaceZ1 = MakeQuadFace(smallEdgeX11, smallEdgeY11, smallEdgeX21, smallEdgeY21)
smallFaceZ2 = MakeQuadFace(smallEdgeX12, smallEdgeY12, smallEdgeX22, smallEdgeY22)


greatFaceX11 = MakeQuadFace(greatEdgeY11, greatEdgeZ111, greatEdgeY12, greatEdgeZ121)
greatFaceX21 = MakeQuadFace(greatEdgeY21, greatEdgeZ211, greatEdgeY22, greatEdgeZ221)
greatFaceX31 = MakeQuadFace(greatEdgeY31, greatEdgeZ311, greatEdgeY32, greatEdgeZ321)
greatFaceX41 = MakeQuadFace(greatEdgeY41, greatEdgeZ411, greatEdgeY42, greatEdgeZ421)

greatFaceX12 = MakeQuadFace(greatEdgeY12, greatEdgeZ112, greatEdgeY13, greatEdgeZ122)
greatFaceX22 = MakeQuadFace(greatEdgeY22, greatEdgeZ212, greatEdgeY23, greatEdgeZ222)
greatFaceX32 = MakeQuadFace(greatEdgeY32, greatEdgeZ312, greatEdgeY33, greatEdgeZ322)
greatFaceX42 = MakeQuadFace(greatEdgeY42, greatEdgeZ412, greatEdgeY43, greatEdgeZ422)

greatFaceX13 = MakeQuadFace(greatEdgeY13, greatEdgeZ113, greatEdgeY14, greatEdgeZ123)
greatFaceX23 = MakeQuadFace(greatEdgeY23, greatEdgeZ213, greatEdgeY24, greatEdgeZ223)
greatFaceX33 = MakeQuadFace(greatEdgeY33, greatEdgeZ313, greatEdgeY34, greatEdgeZ323)
greatFaceX43 = MakeQuadFace(greatEdgeY43, greatEdgeZ413, greatEdgeY44, greatEdgeZ423)

greatFaceY111 = MakeQuadFace(greatEdgeX111, greatEdgeZ111, greatEdgeX112, greatEdgeZ211)
greatFaceY211 = MakeQuadFace(greatEdgeX211, greatEdgeZ211, greatEdgeX212, greatEdgeZ311)
greatFaceY311 = MakeQuadFace(greatEdgeX311, greatEdgeZ311, greatEdgeX312, greatEdgeZ411)
greatFaceY121 = MakeQuadFace(greatEdgeX121, greatEdgeZ121, greatEdgeX122, greatEdgeZ221)
greatFaceY221 = MakeQuadFace(greatEdgeX221, greatEdgeZ221, greatEdgeX222, greatEdgeZ321)
greatFaceY321 = MakeQuadFace(greatEdgeX321, greatEdgeZ321, greatEdgeX322, greatEdgeZ421)

greatFaceY112 = MakeQuadFace(greatEdgeX112, greatEdgeZ112, greatEdgeX113, greatEdgeZ212)
greatFaceY212 = MakeQuadFace(greatEdgeX212, greatEdgeZ212, greatEdgeX213, greatEdgeZ312)
greatFaceY312 = MakeQuadFace(greatEdgeX312, greatEdgeZ312, greatEdgeX313, greatEdgeZ412)
greatFaceY122 = MakeQuadFace(greatEdgeX122, greatEdgeZ122, greatEdgeX123, greatEdgeZ222)
greatFaceY222 = smallFaceY1
greatFaceY322 = MakeQuadFace(greatEdgeX322, greatEdgeZ322, greatEdgeX323, greatEdgeZ422)

greatFaceY113 = MakeQuadFace(greatEdgeX113, greatEdgeZ113, greatEdgeX114, greatEdgeZ213)
greatFaceY213 = MakeQuadFace(greatEdgeX213, greatEdgeZ213, greatEdgeX214, greatEdgeZ313)
greatFaceY313 = MakeQuadFace(greatEdgeX313, greatEdgeZ313, greatEdgeX314, greatEdgeZ413)
greatFaceY123 = MakeQuadFace(greatEdgeX123, greatEdgeZ123, greatEdgeX124, greatEdgeZ223)
greatFaceY223 = MakeQuadFace(greatEdgeX223, greatEdgeZ223, greatEdgeX224, greatEdgeZ323)
greatFaceY323 = MakeQuadFace(greatEdgeX323, greatEdgeZ323, greatEdgeX324, greatEdgeZ423)

greatFaceZ11 = MakeQuadFace(greatEdgeX111, greatEdgeY11, greatEdgeX121, greatEdgeY21)
greatFaceZ21 = MakeQuadFace(greatEdgeX211, greatEdgeY21, greatEdgeX221, greatEdgeY31)
greatFaceZ31 = MakeQuadFace(greatEdgeX311, greatEdgeY31, greatEdgeX321, greatEdgeY41)

greatFaceZ12 = MakeQuadFace(greatEdgeX112, greatEdgeY12, greatEdgeX122, greatEdgeY22)
greatFaceZ22 = MakeQuadFace(greatEdgeX212, greatEdgeY22, greatEdgeX222, greatEdgeY32)
greatFaceZ32 = MakeQuadFace(greatEdgeX312, greatEdgeY32, greatEdgeX322, greatEdgeY42)

greatFaceZ13 = MakeQuadFace(greatEdgeX113, greatEdgeY13, greatEdgeX123, greatEdgeY23)
greatFaceZ23 = MakeQuadFace(greatEdgeX213, greatEdgeY23, greatEdgeX223, greatEdgeY33)
greatFaceZ33 = MakeQuadFace(greatEdgeX313, greatEdgeY33, greatEdgeX323, greatEdgeY43)

greatFaceZ14 = MakeQuadFace(greatEdgeX114, greatEdgeY14, greatEdgeX124, greatEdgeY24)
greatFaceZ24 = MakeQuadFace(greatEdgeX214, greatEdgeY24, greatEdgeX224, greatEdgeY34)
greatFaceZ34 = MakeQuadFace(greatEdgeX314, greatEdgeY34, greatEdgeX324, greatEdgeY44)

# Solids
# ------

smallBlock   = MakeHexaSolid(smallFaceX1, smallFaceX2, smallFaceY1, smallFaceY2, smallFaceZ1, smallFaceZ2)

greatBlock11 = MakeHexaSolid(greatFaceX11, greatFaceX21, greatFaceY111, greatFaceY121, greatFaceZ11, greatFaceZ12)
greatBlock21 = MakeHexaSolid(greatFaceX21, greatFaceX31, greatFaceY211, greatFaceY221, greatFaceZ21, greatFaceZ22)
greatBlock31 = MakeHexaSolid(greatFaceX31, greatFaceX41, greatFaceY311, greatFaceY321, greatFaceZ31, greatFaceZ32)

greatBlock12 = MakeHexaSolid(greatFaceX12, greatFaceX22, greatFaceY112, greatFaceY122, greatFaceZ12, greatFaceZ13)
greatBlock22 = MakeHexaSolid(greatFaceX22, greatFaceX32, greatFaceY212, greatFaceY222, greatFaceZ22, greatFaceZ23)
greatBlock32 = MakeHexaSolid(greatFaceX32, greatFaceX42, greatFaceY312, greatFaceY322, greatFaceZ32, greatFaceZ33)

greatBlock13 = MakeHexaSolid(greatFaceX13, greatFaceX23, greatFaceY113, greatFaceY123, greatFaceZ13, greatFaceZ14)
greatBlock23 = MakeHexaSolid(greatFaceX23, greatFaceX33, greatFaceY213, greatFaceY223, greatFaceZ23, greatFaceZ24)
greatBlock33 = MakeHexaSolid(greatFaceX33, greatFaceX43, greatFaceY313, greatFaceY323, greatFaceZ33, greatFaceZ34)

# Compound
# --------

l = []
l.append(smallBlock)
l.append(greatBlock11)
l.append(greatBlock21)
l.append(greatBlock31)
l.append(greatBlock12)
l.append(greatBlock22)
l.append(greatBlock32)
l.append(greatBlock13)
l.append(greatBlock23)
l.append(greatBlock33)

piece = MakeCompound(l)

# Add in study
# ------------

idpiece = addToStudy(piece, "Cubes2pyGibi")
#geompy.InsertOp.Export(piece,"piece.brep", "BREP")
