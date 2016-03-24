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
#  File   : GEOM_Spanner.py
#  Author : Julia DOROVSKIKH
#  Module : GEOM
#
# ! Please, if you edit this example file, update also
# ! GEOM_SRC/doc/salome/gui/GEOM/input/tui_test_spanner.doc
# ! as some sequences of symbols from this example are used during
# ! documentation generation to identify certain places of this file
#
############# MakeSpanner #############
def MakeSpanner (geompy, math, isBlocksTest = 0, isMeshTest = 0, smesh = None):

  ### Variables ###

  th = 4.0

  ### Basic points and directions ###

  p0  = geompy.MakeVertex(0., 0., 0.)
  pth = geompy.MakeVertex(0., 0., th)

  vx = geompy.MakeVectorDXDYDZ(1., 0., 0.)
  vy = geompy.MakeVectorDXDYDZ(0., 1., 0.)
  vz = geompy.MakeVectorDXDYDZ(0., 0., 1.)

  vxy = geompy.MakeVectorDXDYDZ(1., 1., 0.)

  ### Block 1 ###

  p_10_0  = geompy.MakeVertex(10.,  0., 0.)
  p_10_25 = geompy.MakeVertex(10., 25., 0.)
  p_0_35  = geompy.MakeVertex( 0., 35., 0.)
  p_0_10  = geompy.MakeVertex( 0., 10., 0.)

  Face11 = geompy.MakeQuad4Vertices(p_10_0, p_10_25, p_0_10, p_0_35);
  id_face11 = geompy.addToStudy(Face11, "1 Face for Block 1")

  Face12 = geompy.MakeTranslationTwoPoints(Face11, p0, pth)
  id_face12 = geompy.addToStudy(Face12, "2 Face for Block 1")

  Block1 = geompy.MakeHexa2Faces(Face11, Face12)
  id_block1 = geompy.addToStudy(Block1, "Block 1")

  ### Block 2 ###

  p_15_0  = geompy.MakeVertex(15.,  0., 0.)
  p_15_20 = geompy.MakeVertex(15., 20., 0.)

  Edge21 = geompy.MakeEdge(p_15_0, p_15_20)
  id_edge21 = geompy.addToStudy(Edge21, "1 Edge for Block 2")

  Edge22 = geompy.GetEdge(Block1, p_10_0, p_10_25)
  id_edge22 = geompy.addToStudy(Edge22, "2 Edge for Block 2")

  Face21 = geompy.MakeQuad2Edges(Edge21, Edge22);
  id_face21 = geompy.addToStudy(Face21, "1 Face for Block 2")

  Face22 = geompy.MakeTranslationTwoPoints(Face21, p0, pth)
  id_face22 = geompy.addToStudy(Face22, "2 Face for Block 2")

  Block2 = geompy.MakeHexa2Faces(Face21, Face22)
  id_block2 = geompy.addToStudy(Block2, "Block 2")

  ### Block 3 ###

  p_15_10_1 = geompy.MakeVertex(15., 10., 1.)

  Face31 = geompy.GetFaceNearPoint(Block2, p_15_10_1)
  id_face31 = geompy.addToStudy(Face31, "1 Face for Block 3")

  Prism1 = geompy.MakePrismVecH(Face31, vx, 5);
  id_prism1 = geompy.addToStudy(Prism1, "Prism to be Block 3")

  nbFaces = geompy.ShapesOp.NumberOfFaces(Prism1)
  if nbFaces == 6:
    print "Prism 1 is a hexahedral solid"
  else:
    print "Prism 1 is not a hexahedral solid"

  Prism1_faces = geompy.SubShapeAllSortedCentres(Prism1, geompy.ShapeType["FACE"])
  ii = 1
  for aFace in  Prism1_faces:
    name = geompy.SubShapeName(aFace, Prism1)
    Id_Face = geompy.addToStudyInFather(Prism1, aFace, name)
    ii = ii + 1

  Block3 = geompy.MakeHexa(Prism1_faces[0], Prism1_faces[1],
                           Prism1_faces[2], Prism1_faces[3],
                           Prism1_faces[4], Prism1_faces[5]);
  id_block3 = geompy.addToStudy(Block3, "Block 3")

  ### Block 4 ###

  Face41 = geompy.GetOppositeFace(Block3, Face31)
  id_face41 = geompy.addToStudy(Face41, "1 Face for Block 4")

  p_25_5  = geompy.MakeVertex(25.,  5., 0.)
  p_25_20 = geompy.MakeVertex(25., 20., 0.)

  Edge41 = geompy.MakeEdge(p_25_5, p_25_20)
  id_edge41 = geompy.addToStudy(Edge41, "1 Edge for Block 4")

  p_25_5_2  = geompy.MakeVertex(25.,  5., th)
  p_25_20_2 = geompy.MakeVertex(25., 20., th)

  Edge42 = geompy.MakeEdge(p_25_5_2, p_25_20_2)
  id_edge42 = geompy.addToStudy(Edge42, "2 Edge for Block 4")

  Edge43 = geompy.MakeEdge(p_25_5, p_25_5_2)
  id_edge43 = geompy.addToStudy(Edge43, "3 Edge for Block 4")

  Edge44 = geompy.MakeEdge(p_25_20, p_25_20_2)
  id_edge44 = geompy.addToStudy(Edge44, "4 Edge for Block 4")

  Face42 = geompy.MakeQuad(Edge41, Edge42, Edge43, Edge44)
  id_face42 = geompy.addToStudy(Face42, "2 Face for Block 4")

  Block4 = geompy.MakeHexa2Faces(Face41, Face42)
  id_block4 = geompy.addToStudy(Block4, "Block 4")

  ### Block 5 ###

  p_25_30   = geompy.MakeVertex(25., 30., 0.)
  p_25_30_1 = geompy.MakeVertex(25., 30., 1)
  axis_rot = geompy.MakeVector(p_25_30, p_25_30_1)

  Face51 = Face42
  Face52 = geompy.MakeRotation(Face51, axis_rot, math.pi/2.)
  id_face52 = geompy.addToStudy(Face52, "2 Face for Block 5")

  Block5 = geompy.MakeHexa2Faces(Face51, Face52)
  id_block5 = geompy.addToStudy(Block5, "Block 5")

  ### Compound of blocks 1 - 4 ###

  Comp1234 = geompy.MakeCompound([Block1, Block2, Block3, Block4])
  Glue1234 = geompy.MakeGlueFaces(Comp1234, 1e-5)
  id_glue1234 = geompy.addToStudy(Glue1234, "Glue 1234")

  plane_mirror = geompy.MakePlane(p_25_30, vxy, 100.)
  #id_plane_mirror = geompy.addToStudy(plane_mirror, "Plane for mirror")

  Mirror1 = geompy.MakeMirrorByPlane(Glue1234, plane_mirror)
  id_mirror1 = geompy.addToStudy(Mirror1, "Mirror 1")

  ### The second end ###

  p_18_0 = geompy.MakeVertex(18., 0., 0.)
  p_20_0 = geompy.MakeVertex(20., 0., 0.)
  p_40_0 = geompy.MakeVertex(40., 0., 0.)

  Edge1s = geompy.MakeEdge(p_20_0, p_40_0)
  id_edge1s = geompy.addToStudy(Edge1s, "1 Edge for Block 1s")

  Edge2s = geompy.MakeRotation(Edge1s, vz, math.pi/12.)
  id_edge2s = geompy.addToStudy(Edge2s, "2 Edge for Block 1s")

  p_18_0_r = geompy.MakeRotation(p_18_0, vz, math.pi/24.)
  p_20_0_r = geompy.MakeRotation(p_20_0, vz, math.pi/12.)
  p_40_0_r = geompy.MakeRotation(p_40_0, vz, math.pi/12.)

  Edge3s = geompy.MakeEdge(p_40_0, p_40_0_r)
  id_edge3s = geompy.addToStudy(Edge3s, "3 Edge for Block 1s")

  Edge4s = geompy.MakeArc(p_20_0, p_18_0_r, p_20_0_r)
  id_edge4s = geompy.addToStudy(Edge4s, "4 Edge for Block 1s")

  Face1s = geompy.MakeQuad(Edge1s, Edge2s, Edge3s, Edge4s)
  id_face1s = geompy.addToStudy(Face1s, "1 Face for Block 1s")

  Block1s = geompy.MakePrismVecH(Face1s, vz, th);
  id_block1s = geompy.addToStudy(Block1s, "Block 1s")

  p_30_0_1 = geompy.MakeVertex(30., 0., 1.)

  Face2s = geompy.GetFaceNearPoint(Block1s, p_30_0_1)
  id_face2s = geompy.addToStudy(Face2s, "2 Face of Block 1s")

  f2s_ind = geompy.LocalOp.GetSubShapeIndex(Block1s, Face2s)

  Block1s_tr1 = geompy.MakeMultiTransformation1D(Block1s, f2s_ind, 0, 24)
  id_block1s_tr1 = geompy.addToStudy(Block1s_tr1, "Multi-transformed Block 1s")

  p_60_150_0 = geompy.MakeVertex(60., -150., 0.)
  Gear = geompy.MakeTranslationTwoPoints(Block1s_tr1, p0, p_60_150_0)
  id_gear = geompy.addToStudy(Gear, "Gear")

  ### Link two parts ###

  ### Linking Block 1 ###

  p_55_0_1 = geompy.MakeVertex(55., 0., 1.)
  p_55_110_1 = geompy.MakeVertex(55., -110., 1.)

  Face11l = geompy.GetFaceNearPoint(Block5, p_55_0_1)
  id_face11l = geompy.addToStudy(Face11l, "1 Face for Linking Block 1")

  Face12l = geompy.GetFaceNearPoint(Gear, p_55_110_1)
  id_face12l = geompy.addToStudy(Face12l, "2 Face for Linking Block 1")

  Block1l = geompy.MakeHexa2Faces(Face11l, Face12l)
  id_block1l = geompy.addToStudy(Block1l, "Linking Block 1")

  ### Linking Block 2 ###

  p_25_0_1 = geompy.MakeVertex(25., 0., 1.)
  p_45_110_1 = geompy.MakeVertex(45., -110., 1.)

  Face21l = geompy.GetFaceNearPoint(Block4, p_25_0_1)
  id_face21l = geompy.addToStudy(Face21l, "1 Face for Linking Block 2")

  Face22l = geompy.GetFaceNearPoint(Gear, p_45_110_1)
  id_face22l = geompy.addToStudy(Face22l, "2 Face for Linking Block 2")

  Block2l = geompy.MakeHexa2Faces(Face21l, Face22l)
  id_block2l = geompy.addToStudy(Block2l, "Linking Block 2")

  ### Linking Block 3 ###

  p_55_30_1 = geompy.MakeVertex(55., 30., 1.)
  p_65_110_1 = geompy.MakeVertex(65., -110., 1.)

  Face31l = geompy.GetFaceNearPoint(Mirror1, p_55_30_1)
  id_face31l = geompy.addToStudy(Face31l, "1 Face for Linking Block 3")

  Face32l = geompy.GetFaceNearPoint(Gear, p_65_110_1)
  id_face32l = geompy.addToStudy(Face32l, "2 Face for Linking Block 3")

  Block3l = geompy.MakeHexa2Faces(Face31l, Face32l)
  id_block3l = geompy.addToStudy(Block3l, "Linking Block 3")

  ### Handle ###

  CompLB = geompy.MakeCompound([Block1l, Block2l, Block3l])
  Handle = geompy.MakeGlueFaces(CompLB, 1e-5)
  id_handle = geompy.addToStudy(Handle, "Handle")

  Block1h = geompy.GetBlockNearPoint(Handle, p_45_110_1)
  id_block1h = geompy.addToStudyInFather(Handle, Block1h, "Block 1 of Handle")

  Block2h = geompy.GetBlockNearPoint(Handle, p_65_110_1)
  id_block2h = geompy.addToStudyInFather(Handle, Block2h, "Block 2 of Handle")

  Face11h = geompy.GetFaceByNormale(Block1h, vx)
  id_face11h = geompy.addToStudyInFather(Block1h, Face11h, "Face 1")

  Face12h = geompy.GetOppositeFace(Block1h, Face11h)
  id_face12h = geompy.addToStudyInFather(Block1h, Face12h, "Face 2")

  Face21h = geompy.GetFaceByNormale(Block2h, vx)
  id_face21h = geompy.addToStudyInFather(Block2h, Face21h, "Face 1")

  Face22h = geompy.GetOppositeFace(Block2h, Face21h)
  id_face22h = geompy.addToStudyInFather(Block2h, Face22h, "Face 2")

  try:
    Block3h = geompy.GetBlockByParts(Handle, [Face11h, Face21h])
  except RuntimeError:
    try:
      Block3h = geompy.GetBlockByParts(Handle, [Face11h, Face22h])
    except RuntimeError:
      try:
        Block3h = geompy.GetBlockByParts(Handle, [Face12h, Face21h])
      except RuntimeError:
        try:
          Block3h = geompy.GetBlockByParts(Handle, [Face12h, Face22h])
        except RuntimeError:
          print "ERROR: BlocksOp.GetBlockByParts() failed : ", geompy.BlocksOp.GetErrorCode()
        else:
          id_block3h = geompy.addToStudyInFather(Handle, Block3h, "Block 3 of Handle")

  ### The whole shape ###

  CompALL = geompy.MakeCompound([Glue1234, Block5, Mirror1, Handle, Gear])
  Spanner = geompy.MakeGlueFaces(CompALL, 1e-5)
  id_glueALL = geompy.addToStudy(Spanner, "The Spanner")

  ### Check the Spanner ###

  isCompOfBlocks6 = geompy.CheckCompoundOfBlocks(Spanner)
  if isCompOfBlocks6 == 0:
    print "Spanner is not a compound of hexahedral solids"
    (NonBlocks, NonQuads) = geompy.GetNonBlocks(Spanner)
    if NonBlocks is not None:
      geompy.addToStudyInFather(Spanner, NonBlocks, "Group of non-hexahedral solids")
    if NonQuads is not None:
      geompy.addToStudyInFather(Spanner, NonQuads, "Group of non-quadrangular faces")
  else:
    print "Spanner is a compound of hexahedral solids"

  if isBlocksTest == 1:

    print "##################### Test More #####################"

    ### Get Blocks 4 and 5 from the spanner ###

    Face42_sp = geompy.GetFaceByEdges(Spanner, Edge41, Edge42)
    id_face42_sp = geompy.addToStudyInFather(Spanner, Face42_sp, "Face 4_2")

    Blocks_f4_sp = geompy.GetBlocksByParts(Spanner, [Face42_sp])

    isMRot2D = 0

    for aBlock in Blocks_f4_sp:
      name = geompy.SubShapeName(aBlock, Spanner)
      Id_block = geompy.addToStudyInFather(Spanner, aBlock, name)

      f42_sp_ind = geompy.LocalOp.GetSubShapeIndex(aBlock, Face42_sp)

      Face_5_horiz = geompy.GetFaceByNormale(aBlock, vz)
      f_5_horiz_ind = geompy.LocalOp.GetSubShapeIndex(aBlock, Face_5_horiz)

      MRot = None
      is2Dok = True
      try:
        MRot = geompy.MakeMultiTransformation2D(aBlock, f42_sp_ind, 0, 3, f_5_horiz_ind, 0, 5)
      except:
        is2Dok = False
      if is2Dok and MRot is not None:
        isMRot2D = 1
        p_z100 = geompy.MakeVertex(0., 0., 100.)
        MRot_tr = geompy.MakeTranslationTwoPoints(MRot, p0, p_z100)
        id_MRot_tr = geompy.addToStudy(MRot_tr, "Multi-rotated block 5")

    if isMRot2D == 0:
      print "2D Multi Transformation failed"

    ### Get one face of the Gear ###

    p_20_0_tr   = geompy.MakeTranslationTwoPoints(p_20_0  , p0, p_60_150_0)
    p_20_0_r_tr = geompy.MakeTranslationTwoPoints(p_20_0_r, p0, p_60_150_0)
    p_40_0_tr   = geompy.MakeTranslationTwoPoints(p_40_0  , p0, p_60_150_0)
    p_40_0_r_tr = geompy.MakeTranslationTwoPoints(p_40_0_r, p0, p_60_150_0)

    Face_g_1 = geompy.GetFaceByPoints(Gear, p_20_0_tr, p_20_0_r_tr, p_40_0_tr, p_40_0_r_tr)
    id_face_g_1 = geompy.addToStudyInFather(Gear, Face_g_1, "Face of Gear by four points")

    edgesNb = geompy.ShapesOp.NumberOfEdges(Face_g_1)
    print "Face of Gear has ", edgesNb, " edges"

    Face_g_1_tr = geompy.MakeTranslationTwoPoints(Face_g_1, p0, pth)
    id_face_g_1_tr = geompy.addToStudyInFather(Gear, Face_g_1_tr, "Face of Gear by four points, translated")

    ### equivalent of Block1s, but made as block

    Block_new = geompy.MakeHexa2Faces(Face_g_1, Face_g_1_tr)
    id_block_new = geompy.addToStudy(Block_new, "Block New")

  if isMeshTest == 1 and smesh is not None:

    print "##################### Build Mesh #####################"


    # ---- add a middle block of spanner handle in study

    p_45_0_1 = geompy.MakeVertex(45., 0., 1.)
    BlockMh = geompy.GetBlockNearPoint(Spanner, p_45_0_1)
    id_blockMh = geompy.addToStudyInFather(Spanner, BlockMh, "Middle Block of Spanner Handle")

    # ---- add the top face of the middle block in study

    FaceTop = geompy.GetFaceByNormale(BlockMh, vz)
    id_facetop = geompy.addToStudyInFather(BlockMh, FaceTop, "Top Face")

    # ---- add long edges of the top face in study

    FaceTop_edges = geompy.SubShapeAllSortedCentres(FaceTop, geompy.ShapeType["EDGE"])
    Edge1 = FaceTop_edges[0]
    Edge2 = FaceTop_edges[3]
    Id_Edge1 = geompy.addToStudyInFather(FaceTop, Edge1, "Edge 1")
    Id_Edge2 = geompy.addToStudyInFather(FaceTop, Edge2, "Edge 2")

    print "-------------------------- Algorithm and Hypothesis"

    print "---- Init a Mesh with the Spanner"

    mesh = smesh.Mesh(Spanner, "Meshed Spanner")

    print "-------------------------- add hypothesis to Spanner"

    print "-------------------------- NumberOfSegments"
    algoReg = mesh.Segment()
    listHyp = algoReg.GetCompatibleHypothesis()
    for hyp in listHyp:
      print hyp
    print algoReg.GetName()
    print algoReg.GetId()
    algoReg.SetName("Regular_1D")


    hypNbSeg3 = algoReg.NumberOfSegments(3)
    print hypNbSeg3.GetName()
    print hypNbSeg3.GetId()
    print hypNbSeg3.GetNumberOfSegments()
    smesh.SetName(hypNbSeg3, "NumberOfSegments_3")

    print "-------------------------- Quadrangle_2D"

    algoQuad = mesh.Quadrangle()
    listHyp = algoQuad.GetCompatibleHypothesis()
    for hyp in listHyp:
        print hyp
    print algoQuad.GetName()
    print algoQuad.GetId()
    algoQuad.SetName("Quadrangle_2D")

    print "-------------------------- add hypothesis to the Middle Block"

    print "-------------------------- LocalLength"
    algoRegMb = mesh.Segment(BlockMh)
    hypLen1 = algoRegMb.LocalLength(10)
    print hypLen1.GetName()
    print hypLen1.GetId()
    print hypLen1.GetLength()
    smesh.SetName(hypLen1, "Local_Length_10")

    print "-------------------------- add hypothesis to the long edges of the Top Face of the Middle Block"

    algoRegE1 = mesh.Segment(Edge1)
    hypPropE1 = algoRegE1.Propagation()
    print hypPropE1.GetName()
    print hypPropE1.GetId()
    smesh.SetName(hypPropE1, "Propagation hypothesis")
    smesh.SetName(algoRegE1.GetSubMesh(), "SubMesh Edge 1 of Top Face")

    algoRegE2 = mesh.Segment(Edge2)
    hypPropE2 = algoRegE2.Propagation()
    print hypPropE2.GetName()
    print hypPropE2.GetId()
    smesh.SetName(hypPropE2, "Propagation hypothesis")
    smesh.SetName(algoRegE2.GetSubMesh(), "SubMesh Edge 2 of Top Face")

    print "-------------------------- compute the mesh"
    mesh.Compute()

    print "Information about the Mesh:"
    print "Number of nodes       : ", mesh.NbNodes()
    print "Number of edges       : ", mesh.NbEdges()
    print "Number of faces       : ", mesh.NbFaces()
    print "Number of triangles   : ", mesh.NbTriangles()
    print "Number of quadrangles : ", mesh.NbQuadrangles()
    print "Number of volumes     : ", mesh.NbVolumes()
    print "Number of tetrahedrons: ", mesh.NbTetras()

  return Spanner
