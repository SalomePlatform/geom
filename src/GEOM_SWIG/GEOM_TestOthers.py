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
#  File   : GEOM_TestOthers.py
#  Author : Julia DOROVSKIKH
#  Module : GEOM
#
# ! Please, if you edit this example file, update also
# ! GEOM_SRC/doc/salome/gui/GEOM/input/tui_test_others.doc
# ! as some sequences of symbols from this example are used during
# ! documentation generation to identify certain places of this file

import os
import GEOM

def TestExportImport (geompy, shape):

  print "Test Export/Import ...",

  tmpDir = os.getenv("TEMP")
  if tmpDir == None:
    tmpDir = "/tmp"

  # Files for Export/Import testing
  fileExportImport = tmpDir + "/testExportImport.brep"
  fileExportImportBREP = tmpDir + "/testExportImportBREP.brep"
  fileExportImportIGES = tmpDir + "/testExportImportIGES.iges"
  fileExportImportSTEP = tmpDir + "/testExportImportSTEP.step"

  if os.access(fileExportImport, os.F_OK):
    if os.access(fileExportImport, os.W_OK):
      os.remove(fileExportImport)
    else:
      fileExportImport = tmpDir + "/testExportImport1.brep"

    if os.access(fileExportImportBREP, os.W_OK):
      os.remove(fileExportImportBREP)
    else:
      fileExportImportBREP = tmpDir + "/testExportImportBREP1.brep"

    if os.access(fileExportImportIGES, os.W_OK):
      os.remove(fileExportImportIGES)
    else:
      fileExportImportIGES = tmpDir + "/testExportImportIGES1.iges"

    if os.access(fileExportImportSTEP, os.W_OK):
      os.remove(fileExportImportSTEP)
    else:
      fileExportImportSTEP = tmpDir + "/testExportImportSTEP1.step"

  # Export
  geompy.Export(shape, fileExportImport, "BREP")

  # ExportBREP, ExportIGES, ExportSTEP
  geompy.ExportBREP(shape, fileExportImportBREP)
  geompy.ExportIGES(shape, fileExportImportIGES)
  geompy.ExportSTEP(shape, fileExportImportSTEP)

  # Import
  Import = geompy.ImportFile(fileExportImport, "BREP")

  geompy.addToStudy(Import, "Import")

  # ImportBREP, ImportIGES, ImportSTEP
  ImportBREP = geompy.ImportBREP(fileExportImportBREP)
  ImportIGES = geompy.ImportIGES(fileExportImportIGES)
  ImportSTEP = geompy.ImportSTEP(fileExportImportSTEP)

  geompy.addToStudy(ImportBREP, "ImportBREP")
  geompy.addToStudy(ImportIGES, "ImportIGES")
  geompy.addToStudy(ImportSTEP, "ImportSTEP")

  # GetIGESUnit and GetSTEPUnit
  if geompy.GetIGESUnit(fileExportImportIGES) != "M":
    ImportIGES_scaled = geompy.ImportIGES(fileExportImportIGES, True)
    geompy.addToStudy(ImportIGES_scaled, "ImportIGES_scaled")

  if geompy.GetSTEPUnit(fileExportImportSTEP) != "M":
    ImportSTEP_scaled = geompy.ImportSTEP(fileExportImportSTEP, True)
    geompy.addToStudy(ImportSTEP_scaled, "ImportSTEP_scaled")

  # Remove files for Export/Import testing
  os.remove(fileExportImport)
  os.remove(fileExportImportBREP)
  os.remove(fileExportImportIGES)
  os.remove(fileExportImportSTEP)

  # Test RestoreShape from binary BRep stream
  aStream = shape.GetShapeStream()
  aNewShape = geompy.RestoreShape(aStream)
  geompy.addToStudy(aNewShape, "aNewShape")

  print "OK"


def TestOtherOperations (geompy, math):

  # prepare data for further operations
  vx = geompy.MakeVectorDXDYDZ( 1,  0,  0)
  vy = geompy.MakeVectorDXDYDZ( 0,  1,  0)
  vz = geompy.MakeVectorDXDYDZ( 0,  0,  1)

  v_y = geompy.MakeVectorDXDYDZ( 0, -1,  0)

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

  # MakeFaces
  f12 = geompy.MakeFaces([w1, w2], 0)
  id_f12 = geompy.addToStudy(f12, "MakeFaces WO + WI")

  # Export/Import
  TestExportImport(geompy, f12)

  # OrientationChange
  Box = geompy.MakeBoxDXDYDZ(200, 200, 200)
  geompy.addToStudy(Box, "Box")
  Orientation = geompy.OrientationChange(Box)
  id_Orientation = geompy.addToStudy(Orientation, "OrientationChange")

  # MakeCommon, MakeCut, MakeFuse, MakeSection
  p1 = geompy.MakeVertex(60, 120, 0)
  p2 = geompy.MakeVertex( 0,  0, 0)
  v = geompy.MakeVector(p1, p2)
  height = 90
  radius1 = 50
  cylinder = geompy.MakeCylinder(p1, v, radius1, height)
  Sphere = geompy.MakeSphereR(100)

  Common1 = geompy.MakeCommon    (Box, Sphere)
  Cut1    = geompy.MakeCut       (Box, Sphere)
  Fuse1   = geompy.MakeFuse      (Box, Sphere)
  Section = geompy.MakeSection   (Box, Sphere)
  Common2 = geompy.MakeCommonList([Box, Sphere, cylinder])
  Cut2    = geompy.MakeCutList   (Box, [Sphere, cylinder])
  Fuse2   = geompy.MakeFuseList  ([Box, Sphere, cylinder])

  id_Common1 = geompy.addToStudy(Common1,  "Common_1")
  id_Cut1    = geompy.addToStudy(Cut1,     "Cut_1")
  id_Fuse1   = geompy.addToStudy(Fuse1,    "Fuse_1")
  id_Section = geompy.addToStudy(Section, "Section")
  id_Common2 = geompy.addToStudy(Common2,  "Common_2")
  id_Cut2    = geompy.addToStudy(Cut2,     "Cut_2")
  id_Fuse2   = geompy.addToStudy(Fuse2,    "Fuse_2")

  # Partition
  p100 = geompy.MakeVertex(100, 100, 100)
  p300 = geompy.MakeVertex(300, 300, 300)
  Box1 = geompy.MakeBoxTwoPnt(p100, p300)
  #Partition = geompy.Partition([Box], [Box1], [], [Box])
  Partition = geompy.Partition([Box], [Box1])
  id_Partition = geompy.addToStudy(Partition, "Partition of Box by Box1")

  # MakeMultiRotation1D, MakeMultiRotation2D
  pz = geompy.MakeVertex(0, 0, 100)
  vy = geompy.MakeVectorDXDYDZ(0, 100, 0)

  MultiRot1Dt = geompy.MakeMultiRotation1DNbTimes(f12, vy, pz, 6)
  MultiRot1Ds = geompy.MakeMultiRotation1DByStep(f12, vy, pz, math.pi/5., 6)

  MultiRot2Dt = geompy.MakeMultiRotation2DNbTimes(f12, vy, pz, 5, 30, 3)
  MultiRot2Ds = geompy.MakeMultiRotation2DByStep(f12, vy, pz, math.pi/4., 6, 30, 3)

  geompy.addToStudy(MultiRot1Dt, "MakeMultiRotation1DNbTimes")
  geompy.addToStudy(MultiRot1Ds, "MakeMultiRotation1DByStep")
  geompy.addToStudy(MultiRot2Dt, "MakeMultiRotation2DNbTimes")
  id_MultiRot2D = geompy.addToStudy(MultiRot2Ds, "MakeMultiRotation2DByStep")

  # MakeFilletAll
  radius_fillet = 10.
  face5 = geompy.SubShapeSortedCentres(Box, geompy.ShapeType["FACE"], [5])
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
  box_faces = geompy.SubShapeAllSortedCentres(Box, geompy.ShapeType["FACE"])
  f_ind_1 = geompy.GetSubShapeID(Box, box_faces[0])
  f_ind_2 = geompy.GetSubShapeID(Box, box_faces[1])
  f_ind_3 = geompy.GetSubShapeID(Box, box_faces[2])

  MakeChamfer = geompy.MakeChamfer(Box, d1, d2, geompy.ShapeType["FACE"],
                                   [f_ind_1, f_ind_2, f_ind_3])
  id_MakeChamfer = geompy.addToStudy(MakeChamfer, "MakeChamfer")

  # NumberOf
  NumberOfFaces = geompy.NumberOfFaces(Box)
  if NumberOfFaces != 6:
    print "Bad number of faces in BOX!"

  NumberOfEdges = geompy.NumberOfEdges(Box)
  if NumberOfEdges != 12:
    print "Bad number of edges in BOX!"

  NumberOfSolids = geompy.NumberOfSolids(Box)
  if NumberOfSolids != 1:
    print "Bad number of solids in BOX!"

  NumberOfShapes = geompy.NumberOfSubShapes(Box, geompy.ShapeType["SHAPE"])
  if NumberOfShapes != 34:
    print "Bad number of shapes in BOX!"

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
    (NonBlocks, NonQuads) = geompy.GetNonBlocks(Compound1)
    if NonBlocks is not None:
      geompy.addToStudyInFather(Compound1, NonBlocks, "Group of non-hexahedral solids")
    if NonQuads is not None:
      geompy.addToStudyInFather(Compound1, NonQuads, "Group of non-quadrangular faces")
  else:
    print "The Blocks Compound is VALID"

  IsValid = geompy.CheckCompoundOfBlocks(Box)
  if IsValid == 0:
    print "The Box is NOT VALID"
  else:
    print "The Box is VALID"

  # GetSame
  Cone_ss = geompy.GetSame(Compound1, Cone)
  id_Cone_ss = geompy.addToStudyInFather(Compound1, Cone_ss, "Cone subshape")

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

  # GetMainShape
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

  # Boolean Operations on Groups (Union, Intersection, Cut)
  Group_1 = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])
  geompy.UnionIDs(Group_1, [13, 23])
  Group_2 = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])
  geompy.UnionIDs(Group_2, [3, 27])
  Group_3 = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])
  geompy.UnionIDs(Group_3, [33, 23])
  Group_4 = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])
  geompy.UnionIDs(Group_4, [31, 27])

  geompy.addToStudyInFather(Box, Group_1, 'Group_1')
  geompy.addToStudyInFather(Box, Group_2, 'Group_2')
  geompy.addToStudyInFather(Box, Group_3, 'Group_3')
  geompy.addToStudyInFather(Box, Group_4, 'Group_4')

  # union groups
  Group_U_1_2 = geompy.UnionGroups(Group_1, Group_2)
  Group_UL_3_4 = geompy.UnionListOfGroups([Group_3, Group_4])

  geompy.addToStudyInFather(Box, Group_U_1_2, 'Group_U_1_2')
  geompy.addToStudyInFather(Box, Group_UL_3_4, 'Group_UL_3_4')

  # intersect groups
  Group_I_1_3 = geompy.IntersectGroups(Group_1, Group_3)
  Group_IL_1_3 = geompy.IntersectListOfGroups([Group_1, Group_3])

  geompy.addToStudyInFather(Box, Group_I_1_3, 'Group_I_1_3')
  geompy.addToStudyInFather(Box, Group_IL_1_3, 'Group_IL_1_3')

  # cut groups
  Group_C_2_4 = geompy.CutGroups(Group_2, Group_4)
  Group_CL_2_4 = geompy.CutListOfGroups([Group_2], [Group_4])

  geompy.addToStudyInFather(Box, Group_C_2_4, 'Group_C_2_4')
  geompy.addToStudyInFather(Box, Group_CL_2_4, 'Group_CL_2_4')

  # -----------------------------------------------------------------------------
  # enumeration ShapeTypeString as a dictionary
  # -----------------------------------------------------------------------------
  ShapeTypeString = {'0':"COMPOUND", '1':"COMPSOLID", '2':"SOLID", '3':"SHELL",
                     '4':"FACE", '5':"WIRE", '6':"EDGE", '7':"VERTEX", '8':"SHAPE"}

  GroupType = geompy.GetType(CreateGroup)
  print "Type of elements of the created group is ", ShapeTypeString[`GroupType`]

  # Prepare data for the following operations
  p0 = geompy.MakeVertex(0, 0, 0)
  b0 = geompy.MakeBox(-50, -50, -50, 50, 50, 50)
  s0 = geompy.MakeSphereR(100)

  id_b0 = geompy.addToStudy(b0, "b0")
  id_s0 = geompy.addToStudy(s0, "s0")

  v_0pp = geompy.MakeVectorDXDYDZ( 0,  1,  1)
  #v_0np = geompy.MakeVectorDXDYDZ( 0, -1,  1)
  v_p0p = geompy.MakeVectorDXDYDZ( 1,  0,  1)
  v_p0n = geompy.MakeVectorDXDYDZ(1,  0,  -1)
  v_pp0 = geompy.MakeVectorDXDYDZ( 1,  1,  0)
  v_pn0 = geompy.MakeVectorDXDYDZ(1,  -1,  0)

  #pln_0pp = geompy.MakePlane(p0, v_0pp, 300)
  #pln_0np = geompy.MakePlane(p0, v_0np, 300)
  pln_p0p = geompy.MakePlane(p0, v_p0p, 300)
  pln_p0n = geompy.MakePlane(p0, v_p0n, 300)
  pln_pp0 = geompy.MakePlane(p0, v_pp0, 300)
  pln_pn0 = geompy.MakePlane(p0, v_pn0, 300)
  #
  #part_objs = [b0, pln_0pp, pln_0np, pln_p0p, pln_n0p, pln_pp0, pln_np0]
  #part_tool_1 = geompy.MakePartition(part_objs, [], [], [b0])
  #part_tool_1 = geompy.MakePartition(part_objs)
  #
  #id_part_tool_1 = geompy.addToStudy(part_tool_1, "part_tool_1")
  #
  #pt_pnt_1  = geompy.MakeVertex( 55,   0,  55)
  #pt_pnt_2  = geompy.MakeVertex(  0,  55,  55)
  #pt_pnt_3  = geompy.MakeVertex(-55,   0,  55)
  #pt_pnt_4  = geompy.MakeVertex(  0, -55,  55)
  #pt_pnt_5  = geompy.MakeVertex( 55,  55,   0)
  #pt_pnt_6  = geompy.MakeVertex( 55, -55,   0)
  #pt_pnt_7  = geompy.MakeVertex(-55,  55,   0)
  #pt_pnt_8  = geompy.MakeVertex(-55, -55,   0)
  #pt_pnt_9  = geompy.MakeVertex( 55,   0, -55)
  #pt_pnt_10 = geompy.MakeVertex(  0,  55, -55)
  #pt_pnt_11 = geompy.MakeVertex(-55,   0, -55)
  #pt_pnt_12 = geompy.MakeVertex(  0, -55, -55)
  #
  #pt_face_1  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_1)
  #pt_face_2  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_2)
  #pt_face_3  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_3)
  #pt_face_4  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_4)
  #pt_face_5  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_5)
  #pt_face_6  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_6)
  #pt_face_7  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_7)
  #pt_face_8  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_8)
  #pt_face_9  = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_9)
  #pt_face_10 = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_10)
  #pt_face_11 = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_11)
  #pt_face_12 = geompy.GetFaceNearPoint(part_tool_1, pt_pnt_12)
  #
  #pt_box = geompy.GetBlockNearPoint(part_tool_1, p0)
  #
  #comp_parts = [pt_face_1, pt_face_4, pt_face_7, pt_face_10,
  #              pt_face_2, pt_face_5, pt_face_8, pt_face_11,
  #              #pt_face_3, pt_face_6, pt_face_9, pt_face_12, pt_box]
  #              pt_face_3, pt_face_6, pt_face_9, pt_face_12]
  #part_tool = geompy.MakeCompound(comp_parts)
  #id_part_tool = geompy.addToStudy(part_tool, "part_tool")
  #
  #part = geompy.MakePartition([s0], [part_tool])
  #
  #part_tools = [pt_face_1, pt_face_4, pt_face_7, pt_face_10,
  #              pt_face_2, pt_face_5, pt_face_8, pt_face_11,
  #              pt_face_3, pt_face_6, pt_face_9, pt_face_12, b0]
  #part = geompy.MakePartition([s0], part_tools)

  p1 = geompy.MakeVertex(50, 0, 0)
  p2 = geompy.MakeVertex(-50, 0, 0)
  p3 = geompy.MakeVertex(0, 50, 0)
  p4 = geompy.MakeVertex(0, -50, 0)
  p5 = geompy.MakeVertex(0, 0, 50)
  p6 = geompy.MakeVertex(0, 0, -50)

  plnX1 = geompy.MakePlane(p1, vx, 300)
  plnX2 = geompy.MakePlane(p2, vx, 300)
  plnY1 = geompy.MakePlane(p3, vy, 300)
  plnY2 = geompy.MakePlane(p4, vy, 300)
  plnZ1 = geompy.MakePlane(p5, vz, 300)
  plnZ2 = geompy.MakePlane(p6, vz, 300)

  #part = geompy.MakePartition([s0], [plnX1,plnX2,plnY1,plnY2,plnZ1,plnZ2])
  part = geompy.MakePartition([s0], [plnX1])
  part = geompy.MakePartition([part], [plnX2])
  part = geompy.MakePartition([part], [plnY1])
  part = geompy.MakePartition([part], [plnY2])
  part = geompy.MakePartition([part], [plnZ1])
  part = geompy.MakePartition([part], [plnZ2])
  geompy.addToStudy(part, "part")

  # GetFreeFacesIDs
  anIDs = geompy.GetFreeFacesIDs(part)
  freeFaces = geompy.GetSubShape(part, anIDs)

  geompy.addToStudy(freeFaces, "freeFaces")

  # RemoveExtraEdges with union of all faces, sharing common surfaces
  tools = [pln_pp0, pln_pn0, pln_p0p, pln_p0n]

  Partition_1 = geompy.MakePartition([Sphere], tools, [], [], geompy.ShapeType["SOLID"], 0, [])
  geompy.addToStudy(Partition_1, "Partition_1")

  faces = geompy.SubShapeAllSortedCentres(Partition_1, geompy.ShapeType["FACE"])

  Face_1 = faces[0]
  Face_2 = faces[39]
  Face_3 = faces[40]

  geompy.addToStudyInFather(Partition_1, Face_1, "Face_1")
  geompy.addToStudyInFather(Partition_1, Face_2, "Face_2")
  geompy.addToStudyInFather(Partition_1, Face_3, "Face_3")

  Vector_5 = geompy.MakeVectorDXDYDZ(0, 20, 0)
  geompy.addToStudy(Vector_5, "Vector_5")

  Rotation_1 = geompy.MakeRotation(Face_1, Vector_5, 90*math.pi/180.0)
  Rotation_2 = geompy.MakeRotation(Face_1, Vector_5, 180*math.pi/180.0)
  Rotation_3 = geompy.MakeRotation(Face_1, Vector_5, 270*math.pi/180.0)

  geompy.addToStudy(Rotation_1, "Rotation_1")
  geompy.addToStudy(Rotation_2, "Rotation_2")
  geompy.addToStudy(Rotation_3, "Rotation_3")

  Vector_6 = geompy.MakeVectorDXDYDZ(0, 0, 20)
  geompy.addToStudy(Vector_6, "Vector_6")

  Rotation_4 = geompy.MakeRotation(Face_1, Vector_6, 90*math.pi/180.0)
  Rotation_5 = geompy.MakeRotation(Face_1, Vector_6, -90*math.pi/180.0)
  geompy.addToStudy(Rotation_4, "Rotation_4")
  geompy.addToStudy(Rotation_5, "Rotation_5")

  Shell_1 = geompy.MakeShell([Face_1, Rotation_1, Rotation_2, Rotation_3, Rotation_4, Rotation_5])
  Solid_1 = geompy.MakeSolid([Shell_1])
  NoExtraEdges_1 = geompy.RemoveExtraEdges(Solid_1, True) # doUnionFaces = True

  geompy.addToStudy(Shell_1, "Shell_1")
  geompy.addToStudy(Solid_1, "Solid_1")
  geompy.addToStudy(NoExtraEdges_1, "NoExtraEdges_1")

  # RemoveExtraEdges (by default, doUnionFaces = False)
  freeFacesWithoutExtra = geompy.RemoveExtraEdges(freeFaces)

  geompy.addToStudy(freeFacesWithoutExtra, "freeFacesWithoutExtra")

  # UnionFaces
  unitedFaces = geompy.UnionFaces(freeFaces)

  geompy.addToStudy(unitedFaces, "unitedFaces")

  # GetSharedShapes
  sharedFaces = geompy.GetSharedShapes(part, freeFaces,
                                       geompy.ShapeType["FACE"])
  ind = 1
  for shFace in sharedFaces:
    geompy.addToStudy(shFace, "sharedFace_" + `ind`)
    ind = ind + 1
    pass

  sharedEdges = geompy.GetSharedShapesMulti([part, freeFaces],
                                             geompy.ShapeType["EDGE"])
  ind = 1
  for shEdge in sharedEdges:
    geompy.addToStudy(shEdge, "sharedEdge_" + `ind`)
    ind = ind + 1
    pass

  # TransferData
  path = os.getenv("DATA_DIR")
  fileName = path + "/Shapes/Step/black_and_white.step"
  blackWhite = geompy.ImportSTEP(fileName)
  blackWhiteCopy = geompy.MakeCopy(blackWhite[0])
  subBlackWhite = geompy.SubShapeAll(blackWhiteCopy, GEOM.SOLID)
  geompy.TransferData(blackWhite[0], blackWhiteCopy)
  geompy.addToStudy(blackWhite[0], "blackWhite")
  geompy.addToStudy(blackWhiteCopy, "blackWhiteCopy")
  geompy.addToStudyInFather( blackWhiteCopy, subBlackWhite[0], "" )
  geompy.addToStudyInFather( blackWhiteCopy, subBlackWhite[1], "" )

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

  # GetVertexNearPoint(theShape, thePoint)
  pb0_top_2_near = geompy.MakeVertex(40, 40, 40)
  pb0_top_2      = geompy.GetVertexNearPoint(blocksComp, pb0_top_2_near)

  geompy.addToStudyInFather(blocksComp, pb0_top_2, "point from blocksComp near (40,  40,  40)")

  # GetEdge(theShape, thePoint1, thePoint2)
  edge_top_y50 = geompy.GetEdge(blocksComp, pb0_top_1, pb0_top_2)

  geompy.addToStudyInFather(blocksComp, edge_top_y50, "edge from blocksComp by two points")

  # GetEdgeNearPoint(theShape, thePoint)
  pmidle = geompy.MakeVertex(50, 0, 50)
  edge1 = geompy.GetEdgeNearPoint(blocksComp, pmidle)

  geompy.addToStudyInFather(blocksComp, edge1, "edge near point (50, 0, 50)")

  # GetBlockByParts(theCompound, theParts)
  b0_image = geompy.GetBlockByParts(blocksComp, [pb0_top_1, pb0_bot_1, edge1])

  geompy.addToStudyInFather(blocksComp, b0_image, "b0 image")

  # GetShapesNearPoint(theShape, thePoint, theShapeType, theTolerance)
  b0_faces_plus = geompy.GetShapesNearPoint(blocksComp, pb0_top_2_near, geompy.ShapeType["FACE"], 0.01)

  geompy.addToStudyInFather(blocksComp, b0_faces_plus, "faces near point (40,  40,  40)")

  # GetShapesOnPlane
  faces_on_pln = geompy.GetShapesOnPlane(blocksComp, geompy.ShapeType["FACE"],
                                         v_0pp, GEOM.ST_ONIN)
  for face_i in faces_on_pln:
    geompy.addToStudy(face_i, "Face on Plane (N = (0, 1, 1)) or below it")

  # GetShapesOnPlaneIDs
  faces_above_pln_ids = geompy.GetShapesOnPlaneIDs(blocksComp, geompy.ShapeType["FACE"],
                                                   v_0pp, GEOM.ST_OUT)
  faces_above = geompy.CreateGroup(blocksComp, geompy.ShapeType["FACE"])
  geompy.UnionIDs(faces_above, faces_above_pln_ids)
  geompy.addToStudy(faces_above, "Group of faces above Plane (N = (0, 1, 1))")

  # GetShapesOnPlaneWithLocation
  Loc = geompy.MakeVertex(0, -50, 0)
  edges_on_pln = geompy.GetShapesOnPlaneWithLocation(blocksComp, geompy.ShapeType["EDGE"],
                                                     v_y, Loc, GEOM.ST_ON)
  for edge_i in edges_on_pln:
    geompy.addToStudy(edge_i, "Edge on Plane (N = (0, -1, 0) & Location = (0, -50, 0)")

  # GetShapesOnPlaneWithLocationIDs
  edges_on_pln_ids = geompy.GetShapesOnPlaneWithLocationIDs(
           blocksComp, geompy.ShapeType["EDGE"], v_y, Loc, GEOM.ST_ON)
  group_edges_on_pln = geompy.CreateGroup(blocksComp, geompy.ShapeType["EDGE"])
  geompy.UnionIDs(group_edges_on_pln, edges_on_pln_ids)
  grname = "Group of edges on Plane (N = (0, -1, 0) & Location = (0, -50, 0))"
  geompy.addToStudy(group_edges_on_pln, grname)

  # GetShapesOnCylinder
  edges_out_cyl = geompy.GetShapesOnCylinder(blocksComp, geompy.ShapeType["EDGE"],
                                             vy, 55, GEOM.ST_OUT)
  for edge_i in edges_out_cyl:
    geompy.addToStudy(edge_i, "Edge out of Cylinder (axis = (0, 1, 0), r = 55)")

  # GetShapesOnCylinderIDs
  edges_in_cyl_ids = geompy.GetShapesOnCylinderIDs(blocksComp, geompy.ShapeType["EDGE"],
                                                   vy, 80, GEOM.ST_IN)
  edges_in = geompy.CreateGroup(blocksComp, geompy.ShapeType["EDGE"])
  geompy.UnionIDs(edges_in, edges_in_cyl_ids)
  geompy.addToStudy(edges_in, "Group of edges inside Cylinder (axis = (0, 1, 0), r = 55)")

  # GetShapesOnCylinderWithLocation
  edges_out_cyl = geompy.GetShapesOnCylinderWithLocation(blocksComp, geompy.ShapeType["EDGE"],
                                                         vy, p11, 55, GEOM.ST_OUT)
  for edge_i in edges_out_cyl:
    geompy.addToStudy(edge_i, "Edge out of Cylinder (axis = (0, 1, 0),  loc = (0, 0, 0), r = 55)")

  # GetShapesOnCylinderWithLocationIDs
  edges_in_cyl_ids = geompy.GetShapesOnCylinderWithLocationIDs(blocksComp, geompy.ShapeType["EDGE"],
                                                               vy, p11, 80, GEOM.ST_IN)
  edges_in = geompy.CreateGroup(blocksComp, geompy.ShapeType["EDGE"])
  geompy.UnionIDs(edges_in, edges_in_cyl_ids)
  geompy.addToStudy(edges_in, "Group of edges inside Cylinder (axis = (0, 1, 0), loc = (0, 0, 0), r = 80)")

  # GetShapesOnSphere
  vertices_on_sph = geompy.GetShapesOnSphere(blocksComp, geompy.ShapeType["VERTEX"],
                                             p0, 100, GEOM.ST_ON)
  for vertex_i in vertices_on_sph:
    geompy.addToStudy(vertex_i, "Vertex on Sphere (center = (0, 0, 0), r = 100)")

  # GetShapesOnSphereIDs
  vertices_on_sph_ids = geompy.GetShapesOnSphereIDs(blocksComp, geompy.ShapeType["VERTEX"],
                                                    p0, 100, GEOM.ST_ON)
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

  edges_onin_quad = geompy.GetShapesOnQuadrangle(f12, geompy.ShapeType["EDGE"],
                                                 tl, tr, bl, br, GEOM.ST_ONIN)
  comp = geompy.MakeCompound(edges_onin_quad)
  geompy.addToStudy(comp, "Edges of F12 ONIN Quadrangle")
  if len( edges_onin_quad ) != 4:
    print "Error in GetShapesOnQuadrangle()"

  # GetShapesOnQuadrangleIDs
  vertices_on_quad_ids = geompy.GetShapesOnQuadrangleIDs(f12, geompy.ShapeType["VERTEX"],
                                                         tl, tr, bl, br, GEOM.ST_ON)
  vertices_on_quad = geompy.CreateGroup(f12, geompy.ShapeType["VERTEX"])
  geompy.UnionIDs(vertices_on_quad, vertices_on_quad_ids)
  geompy.addToStudy(vertices_on_quad, "Group of vertices on Quadrangle F12")

  # GetShapesOnBox
  edges_on_box = geompy.GetShapesOnBox(b0, part, geompy.ShapeType["EDGE"],
                                       GEOM.ST_ON)
  comp = geompy.MakeCompound(edges_on_box)
  geompy.addToStudy(comp, "Edges of part ON box b0")
  if len( edges_on_box ) != 12:
    print "Error in GetShapesOnBox()"

  # GetShapesOnBoxIDs
  faces_on_box_ids = geompy.GetShapesOnBoxIDs(b0, part, geompy.ShapeType["FACE"],
                                              GEOM.ST_ON)
  faces_on_box = geompy.CreateGroup(part, geompy.ShapeType["FACE"])
  geompy.UnionIDs(faces_on_box, faces_on_box_ids)
  geompy.addToStudyInFather(part, faces_on_box, "Group of faces on box b0")

  # Prepare arguments for GetShapesOnShape
  sph1 = geompy.MakeSphere(50, 50,  50, 40)
  sph2 = geompy.MakeSphere(50, 50, -50, 40)
  pcyl = geompy.MakeVertex(50, 50, -50)
  cyli = geompy.MakeCylinder(pcyl, vz, 40, 100)
  sh_1 = geompy.MakeFuseList([sph1, cyli, sph2])
  # As after Fuse we have a compound, we need to obtain a solid from it
  #shsh = geompy.SubShapeAll(sh_1, geompy.ShapeType["SOLID"])
  #sh_1 = shsh[0]
  geompy.addToStudy(sh_1, "sh_1")

  # GetShapesOnShape
  faces_in_sh = geompy.GetShapesOnShape(sh_1, part, geompy.ShapeType["FACE"],
                                        GEOM.ST_IN)
  comp = geompy.MakeCompound(faces_in_sh)
  geompy.addToStudy(comp, "Faces of part IN shape sh_1")
  if len(faces_in_sh) != 11:
    print "Error in GetShapesOnShape()"

  # GetShapesOnShapeAsCompound
  faces_in_sh_c = geompy.GetShapesOnShapeAsCompound(sh_1, part, geompy.ShapeType["FACE"],
                                                    GEOM.ST_IN)
  geompy.addToStudy(faces_in_sh_c, "Faces of part IN shape sh_1 (as compound)")

  # GetShapesOnShapeIDs
  edges_in_sh_ids = geompy.GetShapesOnShapeIDs(sh_1, part, geompy.ShapeType["EDGE"],
                                               GEOM.ST_IN)
  edges_in_sh = geompy.CreateGroup(part, geompy.ShapeType["EDGE"])
  geompy.UnionIDs(edges_in_sh, edges_in_sh_ids)
  geompy.addToStudyInFather(part, edges_in_sh, "Group of edges in shape sh_1")
  if len(edges_in_sh_ids) != 15:
    print "Error in GetShapesOnShapeIDs()"

  # Prepare arguments for GetInPlace and GetInPlaceByHistory
  box5 = geompy.MakeBoxDXDYDZ(100, 100, 100)
  box6 = geompy.MakeTranslation(box5, 50, 50, 0)

  geompy.addToStudy(box5, "Box 5")
  geompy.addToStudy(box6, "Box 6")

  part = geompy.MakePartition([box5], [box6])
  geompy.addToStudy(part, "Partitioned")

  box5_faces = geompy.SubShapeAll(box5, geompy.ShapeType["FACE"])
  box6_faces = geompy.SubShapeAll(box6, geompy.ShapeType["FACE"])

  for ifa in range(6):
    geompy.addToStudyInFather(box5, box5_faces[ifa], "Face" + `ifa + 1`)
    geompy.addToStudyInFather(box6, box6_faces[ifa], "Face" + `ifa + 1`)

  # GetInPlace(theShapeWhere, theShapeWhat)
  ibb = 5
  faces_list = [box5_faces, box6_faces]
  for afaces in faces_list:
    ifa = 1
    for aface in afaces:
      refl_box_face = geompy.GetInPlace(part, aface)
      if ibb == 6 and (ifa == 2 or ifa == 4):
        # For two faces of the tool box
        # there is no reflection in the result.
        if refl_box_face is not None:
          error = "Result of GetInPlace must be NULL for face "
          error += `ifa` + " of box " + `ibb`
          raise RuntimeError, error
      else:
        ssname = "Reflection of face " + `ifa` + " of box " + `ibb`
        geompy.addToStudyInFather(part, refl_box_face, ssname)
      ifa = ifa + 1
    ibb = ibb + 1

  # GetInPlaceByHistory(theShapeWhere, theShapeWhat)
  part = geompy.MakePartition([box5], [box6])
  geompy.addToStudy(part, "Partitioned")

  ibb = 5
  faces_list = [box5_faces, box6_faces]
  for afaces in faces_list:
    ifa = 1
    for aface in afaces:
      ssname = "Reflection of face " + `ifa` + " of box " + `ibb` + " (by history)"
      if ibb == 6 and (ifa == 2 or ifa == 4):
        # use IDL interface directly to avoid error message appearence in Python console
        refl_box_face = geompy.ShapesOp.GetInPlaceByHistory(part, aface)
        if refl_box_face is not None:
          geompy.addToStudyInFather(part, refl_box_face, ssname)
          error = "Result of GetInPlaceByHistory must be NULL for face "
          error += `ifa` + " of box " + `ibb`
          raise RuntimeError, error
      else:
        # use geompy interface
        refl_box_face = geompy.GetInPlaceByHistory(part, aface)
        geompy.addToStudyInFather(part, refl_box_face, ssname)
      ifa = ifa + 1
    ibb = ibb + 1

#END
