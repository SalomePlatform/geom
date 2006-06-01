#  GEOM GEOM_SWIG : binding of C++ implementaion with Python
#
#  Copyright (C) 2003  CEA
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
#  File   : GEOM_TestOthers.py
#  Author : Julia DOROVSKIKH
#  Module : GEOM
#  $Header$

import os

def TestExportImport (geompy, shape):

  print "Test Export/Import ...",

  # Files for Export/Import testing
  fileExportImport = "/tmp/testExportImport.brep"
  fileExportImportBREP = "/tmp/testExportImportBREP.brep"
  fileExportImportIGES = "/tmp/testExportImportIGES.iges"
  fileExportImportSTEP = "/tmp/testExportImportSTEP.step"

  if os.access(fileExportImport, os.F_OK):
    if os.access(fileExportImport, os.W_OK):
      os.remove(fileExportImport)
    else:
      fileExportImport = "/tmp/testExportImport1.brep"

    if os.access(fileExportImportBREP, os.W_OK):
      os.remove(fileExportImportBREP)
    else:
      fileExportImportBREP = "/tmp/testExportImportBREP1.brep"

    if os.access(fileExportImportIGES, os.W_OK):
      os.remove(fileExportImportIGES)
    else:
      fileExportImportIGES = "/tmp/testExportImportIGES1.iges"

    if os.access(fileExportImportSTEP, os.W_OK):
      os.remove(fileExportImportSTEP)
    else:
      fileExportImportSTEP = "/tmp/testExportImportSTEP1.step"

  # Export
  geompy.Export(shape, fileExportImport, "BREP")

  # ExportBREP, ExportIGES, ExportSTEP
  geompy.ExportBREP(shape, fileExportImportBREP)
  geompy.ExportIGES(shape, fileExportImportIGES)
  geompy.ExportSTEP(shape, fileExportImportSTEP)

  # Import
  Import = geompy.Import(fileExportImport, "BREP")

  id_Import = geompy.addToStudy(Import, "Import")

  # ImportBREP, ImportIGES, ImportSTEP
  ImportBREP = geompy.ImportBREP(fileExportImportBREP)
  ImportIGES = geompy.ImportIGES(fileExportImportIGES)
  ImportSTEP = geompy.ImportSTEP(fileExportImportSTEP)

  id_ImportBREP = geompy.addToStudy(ImportBREP, "ImportBREP")
  id_ImportIGES = geompy.addToStudy(ImportIGES, "ImportIGES")
  id_ImportSTEP = geompy.addToStudy(ImportSTEP, "ImportSTEP")

  # Remove files for Export/Import testing
  os.remove(fileExportImport)
  os.remove(fileExportImportBREP)
  os.remove(fileExportImportIGES)
  os.remove(fileExportImportSTEP)

  print "OK"


def TestOtherOperations (geompy, math):

  # MakeFaces
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
  w3 = geompy.MakeTranslation(w2, 0, 0, 10)

  id_w1 = geompy.addToStudy(w1, "Outside Wire")
  id_w2 = geompy.addToStudy(w2, "Inside Wire")
  id_w3 = geompy.addToStudy(w3, "Inside Wire, translated along OZ")

  f12 = geompy.MakeFaces([w1, w2], 0)
  id_f12 = geompy.addToStudy(f12, "MakeFaces WO + WI")

  # Export/Import
  TestExportImport(geompy, f12)

  # OrientationChange
  Box = geompy.MakeBoxDXDYDZ(200, 200, 200)
  Orientation = geompy.OrientationChange(Box)
  id_Orientation = geompy.addToStudy(Orientation, "OrientationChange")

  # MakeCommon, MakeCut, MakeFuse, MakeSection
  Sphere = geompy.MakeSphereR(100)

  Common  = geompy.MakeCommon (Box, Sphere)
  Cut     = geompy.MakeCut    (Box, Sphere)
  Fuse    = geompy.MakeFuse   (Box, Sphere)
  Section = geompy.MakeSection(Box, Sphere)

  id_Common  = geompy.addToStudy(Common,  "Common")
  id_Cut     = geompy.addToStudy(Cut,     "Cut")
  id_Fuse    = geompy.addToStudy(Fuse,    "Fuse")
  id_Section = geompy.addToStudy(Section, "Section")

  # Partition
  p100 = geompy.MakeVertex(100, 100, 100)
  p300 = geompy.MakeVertex(300, 300, 300)
  Box1 = geompy.MakeBoxTwoPnt(p100, p300)
  Partition = geompy.Partition([Box], [Box1], [], [Box])
  id_Partition = geompy.addToStudy(Partition, "Partition of Box by Box1")

  # MakeMultiRotation1D, MakeMultiRotation2D
  pz = geompy.MakeVertex(0, 0, 100)
  vy = geompy.MakeVectorDXDYDZ(0, 100, 0)

  MultiRot1D = geompy.MakeMultiRotation1D(f12, vy, pz, 6)
  MultiRot2D = geompy.MakeMultiRotation2D(f12, vy, pz, 45, 6, 30, 3)

  id_MultiRot1D = geompy.addToStudy(MultiRot1D, "MakeMultiRotation1D")
  id_MultiRot2D = geompy.addToStudy(MultiRot2D, "MakeMultiRotation2D")

  # MakeFilletAll
  radius_fillet = 10.
  face5 = geompy.SubShapeSorted(Box, geompy.ShapeType["FACE"], [5])
  f_glob_id = geompy.GetSubShapeID(Box, face5)
  SuppFace = geompy.SuppressFaces(Box, [f_glob_id])

  MakeFilletAll = geompy.MakeFilletAll(SuppFace, radius_fillet)
  id_MakeFilletAll = geompy.addToStudy(MakeFilletAll, "MakeFilletAll")

  # MakeChamferAll
  dimension_chamfer = 10.
  MakeChamferAll = geompy.MakeChamferAll(SuppFace, dimension_chamfer)
  id_MakeChamferAll = geompy.addToStudy(MakeChamferAll, "MakeChamferAll")

  # MakeChamfer
  d1 = 13.
  d2 = 7.
  box_faces = geompy.SubShapeAllSorted(Box, geompy.ShapeType["FACE"])
  f_ind_1 = geompy.GetSubShapeID(Box, box_faces[0])
  f_ind_2 = geompy.GetSubShapeID(Box, box_faces[1])
  f_ind_3 = geompy.GetSubShapeID(Box, box_faces[2])

  MakeChamfer = geompy.MakeChamfer(Box, d1, d2, geompy.ShapeType["FACE"],
                                   [f_ind_1, f_ind_2, f_ind_3])
  id_MakeChamfer = geompy.addToStudy(MakeChamfer, "MakeChamfer")

  # NumberOfFaces
  NumberOfFaces = geompy.NumberOfFaces(Box)
  if NumberOfFaces != 6:
    print "Bad number of faces in BOX!"

  # NumberOfEdges
  NumberOfEdges = geompy.NumberOfEdges(Box)
  if NumberOfEdges != 12:
    print "Bad number of edges in BOX!"

  # MakeBlockExplode
  Compound = geompy.MakeCompound([Box, Sphere])
  MakeBlockExplode = geompy.MakeBlockExplode(Compound, 6, 6)

  id_MakeBlockExplode = geompy.addToStudy(MakeBlockExplode[0], "MakeBlockExplode")

  # CheckCompoundOfBlocks
  p1 = geompy.MakeVertex(200, 0, 0)
  p2 = geompy.MakeVertex(400, 200, 200)
  p3 = geompy.MakeVertex(400, 50, 50)
  p4 = geompy.MakeVertex(600, 250, 250)

  Box2 = geompy.MakeBoxTwoPnt(p1, p2)
  Box3 = geompy.MakeBoxTwoPnt(p3, p4)
  Cyl  = geompy.MakeCylinderRH(50, 300)
  Cone = geompy.MakeConeR1R2H(150, 10, 400)

  Compound1 = geompy.MakeCompound([Box, Cyl, Cone, Box3, Box2])

  IsValid = geompy.CheckCompoundOfBlocks(Compound1)
  if IsValid == 0:
    print "The Blocks Compound is NOT VALID"
  else:
    print "The Blocks Compound is VALID"

  IsValid = geompy.CheckCompoundOfBlocks(Box)
  if IsValid == 0:
    print "The Box is NOT VALID"
  else:
    print "The Box is VALID"

  # test geometrical groups

  # CreateGroup
  CreateGroup = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])

  id_CreateGroup = geompy.addToStudy(CreateGroup, "CreateGroup")

  # AddObject
  f_ind_4 = geompy.GetSubShapeID(Box, box_faces[3])
  f_ind_5 = geompy.GetSubShapeID(Box, box_faces[4])
  f_ind_6 = geompy.GetSubShapeID(Box, box_faces[5])

  geompy.AddObject(CreateGroup, f_ind_6) # box_faces[5]
  geompy.AddObject(CreateGroup, f_ind_1) # box_faces[0]
  geompy.AddObject(CreateGroup, f_ind_4) # box_faces[3]
  # Now contains f_ind_6, f_ind_1, f_ind_4

  # UnionList
  geompy.UnionList(CreateGroup, [box_faces[2], box_faces[4], box_faces[5]])
  # Now contains f_ind_6, f_ind_1, f_ind_4, f_ind_3, f_ind_5

  # RemoveObject(theGroup, theSubShapeID)
  geompy.RemoveObject(CreateGroup, f_ind_1) # box_faces[0]
  # Now contains f_ind_6, f_ind_4, f_ind_3, f_ind_5

  # DifferenceList
  geompy.DifferenceList(CreateGroup, [box_faces[1], box_faces[0], box_faces[3]])
  # Now contains f_ind_6, f_ind_3, f_ind_5

  # GetObjectIDs
  GetObjectIDs = geompy.GetObjectIDs(CreateGroup)

  print "Group of Box's faces includes the following IDs:"
  print "(must be ", f_ind_6, ", ", f_ind_3, " and ", f_ind_5, ")"
  for ObjectID in GetObjectIDs:
    print " ", ObjectID

  BoxCopy = geompy.GetMainShape(CreateGroup)

  # DifferenceIDs
  geompy.DifferenceIDs(CreateGroup, [f_ind_3, f_ind_5])
  # Now contains f_ind_6

  # UnionIDs
  geompy.UnionIDs(CreateGroup, [f_ind_1, f_ind_2, f_ind_6])
  # Now contains f_ind_6, f_ind_1, f_ind_2

  # Check
  GetObjectIDs = geompy.GetObjectIDs(CreateGroup)
  print "Group of Box's faces includes the following IDs:"
  print "(must be ", f_ind_6, ", ", f_ind_1, " and ", f_ind_2, ")"
  for ObjectID in GetObjectIDs:
    print " ", ObjectID

  # -----------------------------------------------------------------------------
  # enumeration ShapeTypeString as a dictionary
  # -----------------------------------------------------------------------------
  ShapeTypeString = {'0':"COMPOUND", '1':"COMPSOLID", '2':"SOLID", '3':"SHELL", '4':"FACE", '5':"WIRE", '6':"EDGE", '7':"VERTEX", '8':"SHAPE"}

  GroupType = geompy.GetType(CreateGroup)
  print "Type of elements of the created group is ", ShapeTypeString[`GroupType`]

  # Prepare data for the following operations
  p0 = geompy.MakeVertex(0, 0, 0)
  b0 = geompy.MakeBox(-50, -50, -50, 50, 50, 50)
  s0 = geompy.MakeSphereR(100)

  id_b0 = geompy.addToStudy(b0, "b0")
  id_s0 = geompy.addToStudy(s0, "s0")

  v_0pp = geompy.MakeVectorDXDYDZ( 0,  1,  1)
  v_0np = geompy.MakeVectorDXDYDZ( 0, -1,  1)
  v_p0p = geompy.MakeVectorDXDYDZ( 1,  0,  1)
  v_n0p = geompy.MakeVectorDXDYDZ(-1,  0,  1)
  v_pp0 = geompy.MakeVectorDXDYDZ( 1,  1,  0)
  v_np0 = geompy.MakeVectorDXDYDZ(-1,  1,  0)

  pln_0pp = geompy.MakePlane(p0, v_0pp, 300)
  pln_0np = geompy.MakePlane(p0, v_0np, 300)
  pln_p0p = geompy.MakePlane(p0, v_p0p, 300)
  pln_n0p = geompy.MakePlane(p0, v_n0p, 300)
  pln_pp0 = geompy.MakePlane(p0, v_pp0, 300)
  pln_np0 = geompy.MakePlane(p0, v_np0, 300)

  part_tool_1 = geompy.MakePartition([b0, pln_0pp, pln_0np, pln_p0p, pln_n0p, pln_pp0, pln_np0],
                                     [],
                                     [],
                                     [b0])

  pt_pnt_1  = geompy.MakeVertex( 55,   0,  55)
  pt_pnt_2  = geompy.MakeVertex(  0,  55,  55)
  pt_pnt_3  = geompy.MakeVertex(-55,   0,  55)
  pt_pnt_4  = geompy.MakeVertex(  0, -55,  55)
  pt_pnt_5  = geompy.MakeVertex( 55,  55,   0)
  pt_pnt_6  = geompy.MakeVertex( 55, -55,   0)
  pt_pnt_7  = geompy.MakeVertex(-55,  55,   0)
  pt_pnt_8  = geompy.MakeVertex(-55, -55,   0)
  pt_pnt_9  = geompy.MakeVertex( 55,   0, -55)
  pt_pnt_10 = geompy.MakeVertex(  0,  55, -55)
  pt_pnt_11 = geompy.MakeVertex(-55,   0, -55)
  pt_pnt_12 = geompy.MakeVertex(  0, -55, -55)

  pt_face_1  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_1)
  pt_face_2  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_2)
  pt_face_3  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_3)
  pt_face_4  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_4)
  pt_face_5  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_5)
  pt_face_6  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_6)
  pt_face_7  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_7)
  pt_face_8  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_8)
  pt_face_9  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_9)
  pt_face_10 = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_10)
  pt_face_11 = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_11)
  pt_face_12 = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_12)

  pt_box = geompy.GetBlockNearPoint(part_tool_1, p0)

  part_tool = geompy.MakeCompound([pt_face_1, pt_face_4, pt_face_7, pt_face_10,
                                   pt_face_2, pt_face_5, pt_face_8, pt_face_11,
                                   pt_face_3, pt_face_6, pt_face_9, pt_face_12, pt_box])
  id_part_tool = geompy.addToStudy(part_tool, "part_tool")

  part = geompy.MakePartition([s0], [part_tool])
  geompy.addToStudy(part, "part")

  # GetFreeFacesIDs
  anIDs = geompy.GetFreeFacesIDs(part)
  freeFaces = geompy.GetSubShape(part, anIDs)

  geompy.addToStudy(freeFaces, "freeFaces")

  # RemoveExtraEdges
  freeFacesWithoutExtra = geompy.RemoveExtraEdges(freeFaces)

  geompy.addToStudy(freeFacesWithoutExtra, "freeFacesWithoutExtra")

  # GetSharedShapes
  sharedFaces = geompy.GetSharedShapes(part, freeFacesWithoutExtra, geompy.ShapeType["FACE"])

  for shFace in sharedFaces:
    geompy.addToStudy(shFace, "sharedFace")

  # CheckAndImprove
  blocksComp = geompy.CheckAndImprove(part)

  geompy.addToStudy(blocksComp, "blocksComp")

  # Propagate
  listChains = geompy.Propagate(blocksComp)

  for chain in listChains:
    geompy.addToStudyInFather(blocksComp, chain, "propagation chain")

  # GetPoint(theShape, theX, theY, theZ, theEpsilon)
  #
  # (-50,  50, 50) .-----. (50,  50, 50)
  #      pb0_top_1 |     |
  #                |     . pmidle
  #                |     |
  # (-50, -50, 50) '-----' (50, -50, 50)
  #
  pb0_top_1 = geompy.GetPoint(blocksComp, -50,  50,  50, 0.01)
  pb0_bot_1 = geompy.GetPoint(blocksComp, -50, -50, -50, 0.01)

  geompy.addToStudyInFather(blocksComp, pb0_top_1, "point from blocksComp (-50,  50,  50)")
  geompy.addToStudyInFather(blocksComp, pb0_bot_1, "point from blocksComp (-50, -50, -50)")

  # GetEdgeNearPoint(theShape, thePoint)
  pmidle = geompy.MakeVertex(50, 0, 50)
  edge1 = geompy.GetEdgeNearPoint(blocksComp, pmidle)

  geompy.addToStudyInFather(blocksComp, edge1, "edge near point (50, 0, 50)")

  # GetBlockByParts(theCompound, theParts)
  b0_image = geompy.GetBlockByParts(blocksComp, [pb0_top_1, pb0_bot_1, edge1])

  geompy.addToStudyInFather(blocksComp, b0_image, "b0 image")

  # GetShapesOnPlane
  faces_on_pln = geompy.GetShapesOnPlane(blocksComp, geompy.ShapeType["FACE"],
                                         v_0pp, geompy.GEOM.ST_ONIN)
  for face_i in faces_on_pln:
    geompy.addToStudy(face_i, "Face on Plane (N = (0, 1, 1)) or below it")

  # GetShapesOnPlaneIDs
  faces_above_pln_ids = geompy.GetShapesOnPlaneIDs(blocksComp, geompy.ShapeType["FACE"],
                                                   v_0pp, geompy.GEOM.ST_OUT)
  faces_above = geompy.CreateGroup(blocksComp, geompy.ShapeType["FACE"])
  geompy.UnionIDs(faces_above, faces_above_pln_ids)
  geompy.addToStudy(faces_above, "Group of faces above Plane (N = (0, 1, 1))")

  # GetShapesOnCylinder
  edges_out_cyl = geompy.GetShapesOnCylinder(blocksComp, geompy.ShapeType["EDGE"],
                                             vy, 55, geompy.GEOM.ST_OUT)
  for edge_i in edges_out_cyl:
    geompy.addToStudy(edge_i, "Edge out of Cylinder (axis = (0, 1, 0), r = 55)")

  # GetShapesOnCylinderIDs
  edges_in_cyl_ids = geompy.GetShapesOnCylinderIDs(blocksComp, geompy.ShapeType["EDGE"],
                                                   vy, 80, geompy.GEOM.ST_IN)
  edges_in = geompy.CreateGroup(blocksComp, geompy.ShapeType["EDGE"])
  geompy.UnionIDs(edges_in, edges_in_cyl_ids)
  geompy.addToStudy(edges_in, "Group of edges inside Cylinder (axis = (0, 1, 0), r = 55)")

  # GetShapesOnSphere
  vertices_on_sph = geompy.GetShapesOnSphere(blocksComp, geompy.ShapeType["VERTEX"],
                                             p0, 100, geompy.GEOM.ST_ON)
  for vertex_i in vertices_on_sph:
    geompy.addToStudy(vertex_i, "Vertex on Sphere (center = (0, 0, 0), r = 100)")
    pass

  # GetShapesOnSphereIDs
  vertices_on_sph_ids = geompy.GetShapesOnSphereIDs(blocksComp, geompy.ShapeType["VERTEX"],
                                                    p0, 100, geompy.GEOM.ST_ON)
  vertices_on = geompy.CreateGroup(blocksComp, geompy.ShapeType["VERTEX"])
  geompy.UnionIDs(vertices_on, vertices_on_sph_ids)
  geompy.addToStudy(vertices_on, "Group of vertices on Sphere (center = (0, 0, 0), r = 100)")

  # GetShapesOnQuadrangle

  geompy.addToStudy(f12, "F12" )

  bl = geompy.MakeVertex(10,-10, 0)
  br = geompy.MakeVertex(40,-10, 0)
  tr = geompy.MakeVertex(40, 20, 0)
  tl = geompy.MakeVertex(10, 20, 0)
  qe1 = geompy.MakeEdge(bl, br)
  qe2 = geompy.MakeEdge(br, tr)
  qe3 = geompy.MakeEdge(tr, tl)
  qe4 = geompy.MakeEdge(tl, bl)
  quadrangle = geompy.MakeWire([qe1, qe2, qe3, qe4])
  geompy.addToStudy(quadrangle, "Quadrangle")

  edges_onin_quad = geompy.GetShapesOnQuadrangle( f12, geompy.ShapeType["EDGE"],
                                                  tl, tr, bl, br, geompy.GEOM.ST_ONIN)
  comp = geompy.MakeCompound(edges_onin_quad)
  geompy.addToStudy(comp, "Edges of F12 ONIN Quadrangle")
  if len( edges_onin_quad ) != 4:
    print "Error in GetShapesOnQuadrangle()"
    pass

  # GetShapesOnQuadrangleIDs
  vertices_on_quad_ids = geompy.GetShapesOnQuadrangleIDs(f12, geompy.ShapeType["VERTEX"],
                                                         tl, tr, bl, br, geompy.GEOM.ST_ON)
  vertices_on_quad = geompy.CreateGroup(f12, geompy.ShapeType["VERTEX"])
  geompy.UnionIDs(vertices_on_quad, vertices_on_quad_ids)
  geompy.addToStudy(vertices_on_quad, "Group of vertices on Quadrangle F12")

  # GetInPlace(theShapeWhere, theShapeWhat)
  box5 = geompy.MakeBoxDXDYDZ(100, 100, 100)
  box6 = geompy.MakeTranslation(box5, 50, 50, 0)

  part = geompy.MakePartition([box5], [box6])
  geompy.addToStudy(part, "Partitioned")

  ibb = 5
  box_list = [box5, box6]
  for abox in box_list:
    geompy.addToStudy(abox, "Box " + `ibb`)
    box_faces = geompy.SubShapeAll(abox, geompy.ShapeType["FACE"])
    ifa = 1
    for aface in box_faces:
      geompy.addToStudyInFather(abox, aface, "Face" + `ifa`)
      refl_box_face = geompy.GetInPlace(part, aface)
      if refl_box_face is not None:
        geompy.addToStudyInFather(part, refl_box_face,
                                  "Reflection of Face " + `ifa` + " of box " + `ibb`)
      ifa = ifa + 1
    ibb = ibb + 1
