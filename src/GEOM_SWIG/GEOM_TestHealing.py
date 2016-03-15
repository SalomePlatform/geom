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

#  GEOM_SWIG : binding of C++ implementaion with Python
#  File   : GEOM_TestHealing.py
#  Author : Julia DOROVSKIKH
#  Module : GEOM

def TestProcessShape (geompy):

  ##Load shape from BREP file
  #import os
  #datadir = os.getenv("GEOM_TEST")
  #if datadir != 0:
  #  datadir = datadir + "/Resources/"
  #else:
  #  "Please, define GEOM_TEST variable !"
  #
  #print "Import ", datadir + "aze2.brep"
  #Shape = batchmode_geompy.Import(datadir + "aze2.brep", "BREP")

  p1 = geompy.MakeVertex(0,0,0)
  p2 = geompy.MakeVertex(200,0,0)
  p3 = geompy.MakeVertex(100,150,0)

  edge = geompy.MakeEdge(p1,p2)
  arc  = geompy.MakeArc(p1,p3,p2)
  wire = geompy.MakeWire([edge,arc])
  face = geompy.MakeFace(wire, 1)

  theShape = geompy.MakePrismVecH(face, edge, 130)

  #Check shape
  print "Before ProcessShape:"
  isValid = geompy.CheckShape(theShape)
  if isValid == 0:
    print "The shape is not valid"
  else:
    print "The shape seems to be valid"

  #Process Shape
  Operators = ["FixShape"]
  Parameters = ["FixShape.Tolerance3d"]
  Values = ["1e-7"]

  PS = geompy.ProcessShape(theShape, Operators, Parameters, Values)

  #Check shape
  print "After ProcessShape:"
  isValid = geompy.CheckShape(PS)
  if isValid == 0:
    print "The shape is not valid"
    raise RuntimeError, "It seems, that the ProcessShape() has failed"
  else:
    print "The shape seems to be valid"

  #Add In Study
  Id_Shape = geompy.addToStudy(theShape, "Invalid Shape")
  Id_PS    = geompy.addToStudy(PS, "Processed Shape")

def TestSuppressFaces (geompy):

  #Create base geometry 3D
  Box = geompy.MakeBoxDXDYDZ(200., 200., 200.)

  #IDList for SuppHole
  faces = []
  faces = geompy.SubShapeAllSortedCentres(Box, geompy.ShapeType["FACE"])

  f_glob_id = geompy.GetSubShapeID(Box, faces[5])

  #Transform objects
  SuppFace = geompy.SuppressFaces(Box, [f_glob_id])

  #Add In Study
  Id_SuppFace = geompy.addToStudy(SuppFace, "SuppFace")

def TestSuppressInternalWires (geompy):

  #Create Face with hole
  p11 = geompy.MakeVertex( 0,  0, 0)
  p12 = geompy.MakeVertex(30,  0, 0)
  p13 = geompy.MakeVertex(30, 30, 0)
  p14 = geompy.MakeVertex( 0, 30, 0)

  p21 = geompy.MakeVertex(10, 10, 0)
  p22 = geompy.MakeVertex(20, 10, 0)
  p23 = geompy.MakeVertex(20, 20, 0)
  p24 = geompy.MakeVertex(10, 20, 0)

  e11 = geompy.MakeEdge(p11, p12)
  e12 = geompy.MakeEdge(p12, p13)
  e13 = geompy.MakeEdge(p13, p14)
  e14 = geompy.MakeEdge(p14, p11)

  e21 = geompy.MakeEdge(p21, p22)
  e22 = geompy.MakeEdge(p22, p23)
  e23 = geompy.MakeEdge(p23, p24)
  e24 = geompy.MakeEdge(p24, p21)

  w1 = geompy.MakeWire([e11, e12, e13, e14])
  w2 = geompy.MakeWire([e21, e22, e23, e24])

  id_w1 = geompy.addToStudy(w1, "Outside Wire")
  id_w2 = geompy.addToStudy(w2, "Inside Wire")

  f12 = geompy.MakeFaceWires([w1, w2], 0)
  id_f12 = geompy.addToStudy(f12, "Face WO + WI")

  #Get Free Boundary
  Res1 = geompy.GetFreeBoundary(f12)
  isSuccess1   = Res1[0]
  ClosedWires1 = Res1[1]
  OpenWires1   = Res1[2]
  nbw1 = 0

  for wire in ClosedWires1:
    nbw1 = nbw1 + 1

  if nbw1 != 2:
    raise RuntimeError, "GetFreeBoundary(f12) must return 2 closed wires, but returned ", nbw1

  #SuppressInternalWires
  face = geompy.SuppressInternalWires(f12, [])

  #Get Free Boundary
  Res2 = geompy.GetFreeBoundary(face)
  isSuccess2   = Res2[0]
  ClosedWires2 = Res2[1]
  OpenWires2   = Res2[2]
  nbw2 = 0

  for wire in ClosedWires2:
    nbw2 = nbw2 + 1

  if nbw2 != 1:
    print "GetFreeBoundary(face) must return 1 closed wires, but returned ", nbw2
    raise RuntimeError, "SuppressInternalWires() works not correctly"

  #Add In Study
  Id_face = geompy.addToStudy(face, "Face without internal wires")

def TestCloseContour (geompy):

  ##Load shape from BREP file
  #import os
  #datadir = os.getenv("GEOM_TEST")
  #if datadir != 0:
  #  datadir = datadir + "/Resources/"
  #else:
  #  "Please, define GEOM_TEST variable !"
  #
  #print "Import ", datadir + "open_cont.brep"
  #Shape = geompy.Import(datadir + "open_cont.brep", "BREP")

  p0   = geompy.MakeVertex(0.  , 0.  , 0.  )
  py   = geompy.MakeVertex(0.  , 100., 0.  )
  pz   = geompy.MakeVertex(0.  , 0.  , 100.)
  p200 = geompy.MakeVertex(200., 200., 200.)

  Shape = geompy.MakePolyline([p0, pz, py, p200])

  #Check shape
  print "Before closing contour:"
  isValid = geompy.CheckShape(Shape)
  if isValid == 0:
    print "The shape is not valid"
  else:
    print "The shape seems to be valid"

  #Close Contour
  IsCommonVertex = 0 # false

  shape_wires = geompy.SubShapeAll(Shape, geompy.ShapeType["WIRE"])
  Wires = []
  wi = 0

  for wire in shape_wires:
    Wires.append(geompy.GetSubShapeID(Shape, shape_wires[wi]))
    wi = wi + 1

  CC = geompy.CloseContour(Shape, Wires, IsCommonVertex)

  #Check shape
  print "After closing contour:"
  isValid = geompy.CheckShape(CC)
  if isValid == 0:
    print "The shape is not valid"
    raise RuntimeError, "It seems, that the contour was not closed"
  else:
    print "The shape seems to be valid"

  #Add In Study
  Id_Shape = geompy.addToStudy(Shape, "Shape with open wire")
  Id_CC    = geompy.addToStudy(CC, "Shape with closed wire")

def TestSuppressHoles (geompy):

  #Create base Variables
  radius = 50.
  height = 300.

  #Create base points
  p1 = geompy.MakeVertex(100., 100., 50.)

  #Create base directions
  vz = geompy.MakeVectorDXDYDZ(0., 0., 100.)

  #Create base geometry 3D
  Box      = geompy.MakeBoxDXDYDZ(200., 200., 200.)
  Cylinder = geompy.MakeCylinder(p1, vz, radius, height)

  #Boolean (Cut)
  Cut = geompy.MakeBoolean(Box, Cylinder, 2)
  idCut = geompy.addToStudy(Cut, "CUT")

  #IDList for SuppressFaces
  faces = []
  faces = geompy.SubShapeAllSortedCentres(Cut, geompy.ShapeType["FACE"])
  ind = 0
  for face in faces:
      f_name = "FACE %d"%(ind)
      f_id = geompy.addToStudyInFather(Cut, face, f_name)

      f_glob_id = geompy.GetSubShapeID(Cut, face)
      print "face ", ind, " global index = ", f_glob_id
      ind = ind + 1

  f_glob_id_0 = geompy.GetSubShapeID(Cut, faces[0])
  cut_without_f_0 = geompy.SuppressFaces(Cut, [f_glob_id_0])
  geompy.addToStudy(cut_without_f_0, "Cut without face 0")

  faces1 = []
  faces1 = geompy.SubShapeAllSortedCentres(cut_without_f_0, geompy.ShapeType["FACE"])
  ind = 0
  for face in faces1:
      f_name = "FACE %d"%(ind)
      f_id = geompy.addToStudyInFather(cut_without_f_0, face, f_name)

      f_glob_id = geompy.GetSubShapeID(cut_without_f_0, face)
      print "face ", ind, " global index = ", f_glob_id
      ind = ind + 1

  f_glob_id_3 = geompy.GetSubShapeID(cut_without_f_0, faces1[3])
  cut_without_f_0_3 = geompy.SuppressFaces(cut_without_f_0, [f_glob_id_3])
  cut_without_f_0_3_id = geompy.addToStudy(cut_without_f_0_3, "Cut without faces 0 and 3")

  #IDList for SuppHole
  wires = []
  wires = geompy.SubShapeAllSortedCentres(cut_without_f_0_3, geompy.ShapeType["WIRE"])
  ind = 0
  for wire in wires:
      w_name = "WIRE %d"%(ind)
      w_id = geompy.addToStudyInFather(cut_without_f_0_3, wire, w_name)

      w_glob_id = geompy.GetSubShapeID(cut_without_f_0_3, wire)
      print "wire ", ind, " global index = ", w_glob_id
      ind = ind + 1

  w_3 = geompy.GetSubShapeID(cut_without_f_0_3, wires[3])

  SuppHole3 = geompy.SuppressHoles(cut_without_f_0_3, [w_3])
  SuppHole3_id = geompy.addToStudy(SuppHole3, "Supp Hole 3")

def TestMakeSewing (geompy, math):

  #Create base points
  px = geompy.MakeVertex(100., 0., 0.)
  py = geompy.MakeVertex(0., 100., 0.)
  pz = geompy.MakeVertex(0., 0., 100.)

  #Create base geometry 2D & 3D
  Vector = geompy.MakeVector(px, py)
  Arc    = geompy.MakeArc(py, pz, px)

  #Create base objects
  angle     = 45. * math.pi / 180
  WantPlanarFace = 1 #True

  Wire = geompy.MakeWire([Vector, Arc])
  Face = geompy.MakeFace(Wire, WantPlanarFace)
  S    = geompy.MakeRotation(Face, Vector, angle)

  #Make Sewing
  precision = 0.00001
  Sewing = geompy.MakeSewing([Face, S], precision)

  #Add In Study
  id_Sewing = geompy.addToStudy(Sewing, "Sewing")

def TestDivideEdge (geompy):

  #Create Box
  Box = geompy.MakeBoxDXDYDZ(200., 200., 200.)

  #Divide Edge
  box_edges = geompy.SubShapeAllSortedCentres(Box, geompy.ShapeType["EDGE"])
  edge_ind = geompy.GetSubShapeID(Box, box_edges[1])

  Divide = geompy.DivideEdge(Box, edge_ind, 0.5, 1) # Obj, ind, param, is_curve_param

  #Add In Study
  Id_Box    = geompy.addToStudy(Box, "Box")
  Id_Divide = geompy.addToStudy(Divide, "Box with Divided Edge")

  # using geompy.DivideEdgeByPoint()
  p    = geompy.MakeVertex( 30, -5, 10, theName="Point to project" )
  edge = geompy.GetEdgeNearPoint( Box, p, theName="Edge to split")
  div  = geompy.DivideEdgeByPoint( Box, edge, p, theName="Box (edge divided)")
  assert geompy.NumberOfEdges( Box ) == geompy.NumberOfEdges( div ) - 1

def TestFuseEdges (geompy):

  # create vertices
  p1 = geompy.MakeVertex(0, 0, 0)
  p2 = geompy.MakeVertex(70, 0, 0)
  p3 = geompy.MakeVertex(70, 50, 0)
  p4 = geompy.MakeVertex(70, 80, 0)
  p5 = geompy.MakeVertex(50, 80, 0)
  p6 = geompy.MakeVertex(20, 80, 0)
  p7 = geompy.MakeVertex(0, 80, 0)
  p8 = geompy.MakeVertex(0, 30, 0)

  points = [p1, p2, p3, p4, p5, p6, p7, p8]

  # make a wire
  wire_1 = geompy.MakePolyline(points, True)

  # suppress some vertices in the wire
  wire_2 = geompy.FuseCollinearEdgesWithinWire(wire_1, [p3])
  wire_3 = geompy.FuseCollinearEdgesWithinWire(wire_1, [p5, p6])

  # suppress all suitable vertices in the wire
  wire_4 = geompy.FuseCollinearEdgesWithinWire(wire_1, [])

  wires = [wire_1, wire_2, wire_3, wire_4]

  # add objects in the study
  ii = 1
  for point in points:
    geompy.addToStudy(point, "p%d"%ii)
    ii = ii + 1
    pass

  ii = 1
  for wire in wires:
    geompy.addToStudy(wire, "wire_%d"%ii)
    wire_points = geompy.SubShapeAllSortedCentres(wire, geompy.ShapeType["VERTEX"])
    jj = 1
    for point in wire_points:
      geompy.addToStudyInFather(wire, point, "point_%d"%jj)
      jj = jj + 1
      pass
    ii = ii + 1
    pass

def TestRemoveWebs (geompy):

  # create solids with some coincident faces
  Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
  Translation_1 = geompy.MakeTranslation(Box_1, 200, 0, 0)
  Translation_2 = geompy.MakeTranslation(Box_1, 200, 200, 0)

  geompy.addToStudy(Box_1, 'Box_1')
  geompy.addToStudy(Translation_1, 'Translation_1')
  geompy.addToStudy(Translation_2, 'Translation_2')

  # partition three solids to obtain shared faces
  Partition_1 = geompy.MakePartition([Box_1, Translation_1, Translation_2])
  geompy.addToStudy(Partition_1, 'Partition_1')

  # remove shared faces to obtain one solid instead of three
  Joined_1 = geompy.RemoveInternalFaces(Partition_1)
  geompy.addToStudy(Joined_1, 'Joined_1')

def TestSewGluing(geompy):

  import GEOM
  box1 = geompy.MakeBox(0,0,0, 1,1,1)
  box2 = geompy.MakeBox(1,0,0, 2,1,1)
  comp = geompy.MakeCompound( [box1, box2] )

  # no sewing with AllowNonManifold=False
  sew1 = geompy.MakeSewing( [box1,box2], 1e-5, AllowNonManifold=False)
  assert not sew1
  sew2 = geompy.MakeSewing( comp, 1e-5, AllowNonManifold=False)
  assert not sew2
  sew3 = geompy.MakeSewing( [comp], 1e-5, AllowNonManifold=False)
  assert not sew3
  sew1 = geompy.Sew( [box1,box2], 1e-5, AllowNonManifold=False)
  assert not sew1
  sew2 = geompy.Sew( comp, 1e-5, AllowNonManifold=False)
  assert not sew2
  sew3 = geompy.Sew( [comp], 1e-5, AllowNonManifold=False)
  assert not sew3

  # check MakeSewing()
  sew1 = geompy.MakeSewing( [box1,box2], 1e-5, AllowNonManifold=True)
  assert sew1.GetShapeType() == GEOM.SHELL
  assert geompy.NumberOfFaces( sew1 ) == geompy.NumberOfFaces( comp )
  assert geompy.NumberOfEdges( sew1 ) == geompy.NumberOfEdges( comp ) - 4
  sew2 = geompy.MakeSewing( comp, 1e-5, AllowNonManifold=True)
  assert sew2.GetShapeType() == GEOM.SHELL
  assert geompy.NumberOfFaces( sew2 ) == geompy.NumberOfFaces( comp )
  assert geompy.NumberOfEdges( sew2 ) == geompy.NumberOfEdges( comp ) - 4
  sew3 = geompy.MakeSewing( [comp], 1e-5, AllowNonManifold=True)
  assert sew3.GetShapeType() == GEOM.SHELL
  assert geompy.NumberOfFaces( sew3 ) == geompy.NumberOfFaces( comp )
  assert geompy.NumberOfEdges( sew3 ) == geompy.NumberOfEdges( comp ) - 4
  # check Sew()
  sew1 = geompy.Sew( [box1,box2], 1e-5, AllowNonManifold=True)
  assert sew1.GetShapeType() == GEOM.SHELL
  assert geompy.NumberOfFaces( sew1 ) == geompy.NumberOfFaces( comp )
  assert geompy.NumberOfEdges( sew1 ) == geompy.NumberOfEdges( comp ) - 4
  sew2 = geompy.Sew( comp, 1e-5, AllowNonManifold=True)
  assert sew2.GetShapeType() == GEOM.SHELL
  assert geompy.NumberOfFaces( sew2 ) == geompy.NumberOfFaces( comp )
  assert geompy.NumberOfEdges( sew2 ) == geompy.NumberOfEdges( comp ) - 4
  sew3 = geompy.Sew( [comp], 1e-5, AllowNonManifold=True)
  assert sew3.GetShapeType() == GEOM.SHELL
  assert geompy.NumberOfFaces( sew3 ) == geompy.NumberOfFaces( comp )
  assert geompy.NumberOfEdges( sew3 ) == geompy.NumberOfEdges( comp ) - 4

  # check MakeGlueFaces()
  glueF1 = geompy.MakeGlueFaces( [box1,box2], 1e-5)
  assert glueF1.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfFaces( glueF1 ) == geompy.NumberOfFaces( comp ) - 1
  assert geompy.NumberOfEdges( glueF1 ) == geompy.NumberOfEdges( comp ) - 4
  glueF2 = geompy.MakeGlueFaces( [comp], 1e-5)
  assert glueF2.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfFaces( glueF2 ) == geompy.NumberOfFaces( comp ) - 1
  assert geompy.NumberOfEdges( glueF2 ) == geompy.NumberOfEdges( comp ) - 4
  glueF3 = geompy.MakeGlueFaces( comp, 1e-5)
  assert glueF3.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfFaces( glueF3 ) == geompy.NumberOfFaces( comp ) - 1
  assert geompy.NumberOfEdges( glueF3 ) == geompy.NumberOfEdges( comp ) - 4

  # check GetGlueFaces()
  glueFF1 = geompy.GetGlueFaces( [box1,box2], 1e-5)
  assert len( glueFF1 ) == 1 and glueFF1[0].GetShapeType() == GEOM.FACE
  glueFF2 = geompy.GetGlueFaces( [comp], 1e-5)
  assert len( glueFF2 ) == 1 and glueFF2[0].GetShapeType() == GEOM.FACE
  glueFF3 = geompy.GetGlueFaces( comp, 1e-5)
  assert len( glueFF3 ) == 1 and glueFF3[0].GetShapeType() == GEOM.FACE

  #check MakeGlueFacesByList()
  glueF1 = geompy.MakeGlueFacesByList( [box1,box2], 1e-5, glueFF1)
  assert glueF1.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfFaces( glueF1 ) == geompy.NumberOfFaces( comp ) - 1
  assert geompy.NumberOfEdges( glueF1 ) == geompy.NumberOfEdges( comp ) - 4
  glueF2 = geompy.MakeGlueFacesByList( [comp], 1e-5, glueFF2)
  assert glueF2.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfFaces( glueF2 ) == geompy.NumberOfFaces( comp ) - 1
  assert geompy.NumberOfEdges( glueF2 ) == geompy.NumberOfEdges( comp ) - 4
  glueF3 = geompy.MakeGlueFacesByList( comp, 1e-5, glueFF3 )
  assert glueF3.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfFaces( glueF3 ) == geompy.NumberOfFaces( comp ) - 1
  assert geompy.NumberOfEdges( glueF3 ) == geompy.NumberOfEdges( comp ) - 4

  # check MakeGlueEdges()
  glueE1 = geompy.MakeGlueEdges( [box1,box2], 1e-5)
  assert glueE1.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfEdges( glueE1 ) == geompy.NumberOfEdges( comp ) - 4
  glueE2 = geompy.MakeGlueEdges( [comp], 1e-5)
  assert glueE2.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfEdges( glueE2 ) == geompy.NumberOfEdges( comp ) - 4
  glueE3 = geompy.MakeGlueEdges( comp, 1e-5)
  assert glueE3.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfEdges( glueE3 ) == geompy.NumberOfEdges( comp ) - 4

  # check GetGlueEdges()
  glueEE1 = geompy.GetGlueEdges( [box1,box2], 1e-5)
  assert len( glueEE1 ) == 4 and glueEE1[0].GetShapeType() == GEOM.EDGE
  glueEE2 = geompy.GetGlueEdges( [comp], 1e-5)
  assert len( glueEE2 ) == 4 and glueEE2[0].GetShapeType() == GEOM.EDGE
  glueEE3 = geompy.GetGlueEdges( comp, 1e-5)
  assert len( glueEE3 ) == 4 and glueEE3[0].GetShapeType() == GEOM.EDGE

  #check MakeGlueEdgesByList()
  glueEL1 = geompy.MakeGlueEdgesByList( [box1,box2], 1e-5, glueEE1)
  assert glueEL1.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfEdges( glueEL1 ) == geompy.NumberOfEdges( comp ) - 4
  glueEL2 = geompy.MakeGlueEdgesByList( [comp], 1e-5, glueEE2)
  assert glueEL2.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfEdges( glueEL2 ) == geompy.NumberOfEdges( comp ) - 4
  glueEL3 = geompy.MakeGlueEdgesByList( comp, 1e-5, glueEE3 )
  assert glueEL3.GetShapeType() == GEOM.COMPOUND
  assert geompy.NumberOfEdges( glueEL3 ) == geompy.NumberOfEdges( comp ) - 4

  # check GetSharedShapesMulti()
  sharedEE = geompy.GetSharedShapesMulti( glueEL3, geompy.ShapeType["EDGE"])
  assert len( sharedEE ) == 4
  assert sharedEE[0].GetShapeType() == GEOM.EDGE
  assert sharedEE[1].GetShapeType() == GEOM.EDGE
  assert sharedEE[2].GetShapeType() == GEOM.EDGE
  assert sharedEE[3].GetShapeType() == GEOM.EDGE

  return

def TestHealingOperations (geompy, math):

  TestSewGluing(geompy)
  TestMakeSewing(geompy, math)
  TestDivideEdge(geompy)
  TestSuppressHoles(geompy)
  TestSuppressInternalWires(geompy)
  TestCloseContour(geompy)
  TestSuppressFaces(geompy)
  TestProcessShape(geompy)
  TestFuseEdges(geompy)
  TestRemoveWebs(geompy)
