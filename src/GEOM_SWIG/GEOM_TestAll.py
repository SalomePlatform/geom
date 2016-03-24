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

#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#  File   : GEOM_usinggeom.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM

# ! Please, if you edit this example file, update also
# ! GEOM_SRC/doc/salome/gui/GEOM/input/tui_test_all.doc
# ! as some sequences of symbols from this example are used during
# ! documentation generation to identify certain places of this file

def TestAll (geompy, math):
  import GEOM
  
  #Create base Variables
  nbtimes1  = 5      #Short
  nbtimes2  = 5
  mindeg = 2
  maxdeg = 5
  nbiter = 5
  WantPlanarFace = 1 #True

  radius  = 10.  #Double
  radius1 = 100.
  radius2 = 200.
  height  = 200.
  d1      = 10.
  d2      = 10.
  step1   = 250.
  step2   = 250.
  angle   = 45.
  angle1  = angle * math.pi / 180
  angle2  = 2 * angle1
  factor  = 2.
  tol3d   = 0.0001
  tol2d   = 0.0001
  weight  = 1000000.
  waterdensity = 1.
  meshingdeflection = 0.01
  trimsize  = 1000.
  precision = 0.00001

  #Create base points
  p0   = geompy.MakeVertex(0.  , 0.  , 0.  ) #(3 Doubles)->GEOM_Object
  px   = geompy.MakeVertex(100., 0.  , 0.  )
  py   = geompy.MakeVertex(0.  , 100., 0.  )
  pz   = geompy.MakeVertex(0.  , 0.  , 100.)
  pxyz = geompy.MakeVertex(100., 100., 100.)

  p200 = geompy.MakeVertexWithRef(pxyz, 100., 100., 100.) #(GEOM_Object, 3 Doubles)->GEOM_Object

  #Create base directions
  vx   = geompy.MakeVector(p0, px) #(GEOM_Object, GEOM_Object)->GEOM_Object
  vy   = geompy.MakeVector(p0, py)
  vz   = geompy.MakeVector(p0, pz)
  vxy  = geompy.MakeVector(px, py)

  vxyz = geompy.MakeVectorDXDYDZ(100., 100., 100.) #(3 Doubles)->GEOM_Object

  #Create local coordinate systems
  cs1 = geompy.MakeMarker(50,50,50, 1,0,0, 0,1,0) #(9 Doubles)->GEOM_Object
  cs2 = geompy.MakeMarker(70,80,10, 1,0,1, 1,1,0) #(9 Doubles)->GEOM_Object
  cs3 = geompy.MakeMarkerPntTwoVec(pz, vxy, vz)   #(3 GEOM_Object)->GEOM_Object

  #Create base geometry 2D
  Line   = geompy.MakeLineTwoPnt(p0, pxyz)                  #(2 GEOM_Object)->GEOM_Object
  Line1  = geompy.MakeLine(pz, vxy)                         #(2 GEOM_Object)->GEOM_Object
  Line2  = geompy.MakeLineTwoPnt(pxyz, pz)                  #(2 GEOM_Object)->GEOM_Object
  Plane  = geompy.MakePlane(pz, vxyz, trimsize)             #(2 GEOM_Object, Double)->GEOM_Object
  Plane1 = geompy.MakePlaneThreePnt(px, pz, p200, trimsize) #(4 Doubles)->GEOM_Object
  Plane2 = geompy.MakePlane2Vec(vx, vz, trimsize)	    #(2 GEOM_Object, Double)->GEOM_Object
  Plane3 = geompy.MakePlaneLCS(cs1, trimsize, 3)	    #(1 GEOM_Object, 2 Double)->GEOM_Object

  Arc      = geompy.MakeArc(py, pz, px)                   #(3 GEOM_Object)->GEOM_Object
  Arc2     = geompy.MakeArcCenter(py, pz, px,0)           #(3 GEOM_Object,Boolean)->GEOM_Object
  Arc3     = geompy.MakeArcOfEllipse(p0, px, pz)          #(3 GEOM_Object,Boolean)->GEOM_Object
  Circle   = geompy.MakeCircle(p0, vz, radius1)           #(2 GEOM_Object, Double)->GEOM_Object
  Circle1  = geompy.MakeCircleThreePnt(p0, pxyz, px)      #(3 GEOM_Object)->GEOM_Object
  Circle2  = geompy.MakeCircleCenter2Pnt(p0, pxyz, py)    #(3 GEOM_Object)->GEOM_Object
  Ellipse  = geompy.MakeEllipse(p0, vy, radius2, radius1) #(2 GEOM_Object, 2 Doubles)->GEOM_Object
  Polyline = geompy.MakePolyline([p0, pz, py, p200])      #(List of GEOM_Object)->GEOM_Object
  Bezier   = geompy.MakeBezier([p0, pz, p200, px])        #(List of GEOM_Object)->GEOM_Object
  Interpol = geompy.MakeInterpol([px, py, p200, pxyz], True) #(List of GEOM_Object,Boolean)->GEOM_Object
  InterpT1 = geompy.MakeInterpolWithTangents([px, py, pxyz], vx, vz) #(List of GO, GO, GO)->GEOM_Object
  InterpT2 = geompy.MakeInterpolWithTangents([px, py, pxyz], vxy, vxyz) #(List of GO, GO, GO)->GEOM_Object
  Sketcher = geompy.MakeSketcher("Sketcher:F -100 -100:TT 250 -100:R 0:C 100 150:R 0:L 300:WW",
                                 [100,0,0, 1,1,1, -1,1,0]) #(String, List of Doubles)->GEOM_Object

  #Create 3D wires with 3D Sketcher
  sk = geompy.Sketcher3D()
  sk.addPointsAbsolute(0,0,0, 70,0,0)
  sk.addPointsRelative(0, 0, 130)
  sk.addPointRadiusAnglesRelative(100, 50, 0,"OXY")
  sk.addPointRadiusAnglesRelative(130, 30, 80, "OXZ")
  sk.addPointRadiusAnglesAbsolute(500, 60, 30,"OXY")
  sk.addPointRadiusAngleHRelative(100, 50, 0,"OXY")
  sk.addPointRadiusAngleHRelative(130, 30, 40, "OXZ")
  sk.addPointRadiusAngleHAbsolute(800, 60, 30,"OXY")
  sk.close()
  Sketcher3d_1 = sk.wire()

  Sketcher3d_2 = geompy.Make3DSketcher([0,0,0, 50,50,50, 0,50,50, 10,0,0])

  #Create local coordinate system from shape
  cs4 = geompy.MakeMarkerFromShape(Plane)

  #Test point on curve creation
  p_on_arc = geompy.MakeVertexOnCurve(Arc, 0.25) #(GEOM_Object, Double)->GEOM_Object
  p_on_arc2 = geompy.MakeVertexOnCurveByCoord(Arc, 100, -10, 10 ) #(GEOM_Object, Double)->GEOM_Object
  p_on_arc3 = geompy.MakeVertexOnCurveByLength(Arc, 50, py) #(GEOM_Object, Double, GEOM_Object)->GEOM_Object

  #Test point on lines intersection
  p_on_l1l2 = geompy.MakeVertexOnLinesIntersection(Line1, Line2) #(2 GEOM_Object)->GEOM_Object

  #Test tangent on curve creation
  tan_on_arc = geompy.MakeTangentOnCurve(Arc, 0.7) #(GEOM_Object, Double)->GEOM_Object
  
  #Test tangent on face creation
  tan_vertex_1 = geompy.MakeVertex(0, 0, 0)
  tan_vertex_2 = geompy.MakeVertex(0, 90, 30)
  tan_vertex_3 = geompy.MakeVertex(100, 90, 0)
  tan_vertex_4 = geompy.MakeVertex(-100, 90, 0)
  tan_curve = geompy.MakeInterpol([tan_vertex_4, tan_vertex_2, tan_vertex_3, tan_vertex_1], False)
  tan_extrusion = geompy.MakePrismDXDYDZ(tan_curve, 0, 30, -60)
  tan_on_face = geompy.MakeTangentPlaneOnFace(tan_extrusion, 0.7, 0.5, 150)

  #Create base geometry 3D
  Box      = geompy.MakeBoxTwoPnt(p0, p200)                             #(2 GEOM_Object)->GEOM_Object
  Box1     = geompy.MakeBoxDXDYDZ(10, 20, 30)                           #(3 Doubles)->GEOM_Object
  Box2     = geompy.MakeBox(10,20,30, 15,25,35)                         #(6 Doubles)->GEOM_Object
  Cylinder = geompy.MakeCylinder(p0, vz, radius1, height)               #(2 GEOM_Object, 2 Doubles)->GEOM_Object
  Cyl1     = geompy.MakeCylinderRH(radius2, height)                     #(2 Doubles)->GEOM_Object
  Cylinder1= geompy.MakeCylinderA(p0, vz, radius1, height, angle1)      #(2 GEOM_Object, 3 Doubles)->GEOM_Object
  Cyl2     = geompy.MakeCylinderRHA(radius2, height, angle2)            #(3 Doubles)->GEOM_Object
  Sphere   = geompy.MakeSpherePntR(p0, radius1)                         #(GEOM_Object, Double)->GEOM_Object
  Sphere1  = geompy.MakeSphereR(radius)                                 #(Double)->GEOM_Object
  Sphere2  = geompy.MakeSphere(50, 70, 30, radius)                      #(4 Doubles)->GEOM_Object
  Cone     = geompy.MakeCone(p0, vz, radius2, radius, height)           #(2 GEOM_Object, 3 Doubles)->GEOM_Object
  Cone1    = geompy.MakeConeR1R2H(radius1, radius, height)              #(3 Doubles)->GEOM_Object
  Torus    = geompy.MakeTorus(p0, vz, radius2, radius)                  #(2 GEOM_Object, 2 Doubles)->GEOM_Object
  Torus1   = geompy.MakeTorusRR(radius2, radius1)                       #(2 Doubles)->GEOM_Object

  #Boolean (Common, Cut, Fuse, Section)
  Common  = geompy.MakeBoolean(Box, Sphere, 1) #(2 GEOM_Object, Short)->GEOM_Object
  Cut     = geompy.MakeBoolean(Box, Sphere, 2)
  Fuse    = geompy.MakeBoolean(Box, Sphere, 3)
  Section = geompy.MakeBoolean(Box, Sphere, 4)

  #Create base objects
  Edge     = geompy.MakeEdge(p0, pxyz)               #(2 GEOM_Object)->GEOM_Object
  Edge1    = geompy.MakeEdgeOnCurveByLength(Arc, 50, px) #(GEOM_Object, Double, GEOM_Object)->GEOM_Object
  Wire     = geompy.MakeWire([vxy, Arc])             #(List Of GEOM_Object)->GEOM_Object
  Face     = geompy.MakeFace(Wire, WantPlanarFace)   #(GEOM_Object, Boolean)->GEOM_Object
  Face1    = geompy.MakeFaceWires([Wire, Sketcher],
                                  WantPlanarFace)    #(List of GEOM_Object, Boolean)->GEOM_Object
  Face2    = geompy.MakeFace(Sketcher, WantPlanarFace)
  Face3    = geompy.MakeFaceHW (100., 200., 1)       #(2 Doubles, 1 Int)->GEOM_Object
  Face4    = geompy.MakeFaceObjHW (vz, 200., 100.)   #(1 GEOM_Object, 2 Doubles)->GEOM_Object
  Face5    = geompy.MakeFaceFromSurface(Face, Sketcher) #(2 GEOM_Objects)->GEOM_Object
  
  Cut2 = geompy.MakeCutList(Sphere1, [Box1], True)
  #(List of GEOM_Object)->GEOM_Object
  Face6 = geompy.MakeFaceWithConstraints([geompy.GetSubShape(Cut2, [5]),  geompy.GetSubShape(Cut2, [3]), 
                                          geompy.GetSubShape(Cut2, [11]), geompy.GetSubShape(Cut2, [3]),
                                          geompy.GetSubShape(Cut2, [13]), geompy.GetSubShape(Cut2, [3])])
  Disk     = geompy.MakeDiskPntVecR (p0, vz, radius) #(2 GEOM_Object, 1 Double)->GEOM_Object
  Disk2    = geompy.MakeDiskThreePnt(p0, p200, pz)   #(3 GEOM_Object)->GEOM_Object
  Disk3    = geompy.MakeDiskR(100., 1)               #(1 Doubles, 1 Int)->GEOM_Object
  Shell    = geompy.MakeShell([Face, Face1])         #(List of GEOM_Object)->GEOM_Object

  Prism1   = geompy.MakePrism(Face2, p0, pxyz)       #(3 GEOM_Object)->GEOM_Object
  prism1_faces = geompy.SubShapeAllSortedCentres(Prism1, geompy.ShapeType["FACE"])
  Shell1   = geompy.MakeShell([prism1_faces[0], prism1_faces[1],
                               prism1_faces[3], prism1_faces[4],
                               prism1_faces[5], prism1_faces[2]])
  Solid    = geompy.MakeSolid([Shell1])              #(List of GEOM_Object)->GEOM_Object
  
  Box1_translation = geompy.MakeTranslation(Box1, 10, 0, 0)
  Box1_shell = geompy.SubShapeAllSorted(Box1, geompy.ShapeType["SHELL"])[0]
  Box1_translation_shell = geompy.SubShapeAllSorted(Box1_translation, geompy.ShapeType["SHELL"])[0]
  
  Solid_from_shells = geompy.MakeSolidFromConnectedFaces([Box1_shell, Box1_translation_shell], 1) #(List of GEOM_Object, Boolean)->GEOM_Object
  
  # Create Isoline
  Isoline = geompy.MakeIsoline(Face1, True, 0.5)     #(1 GEOM_Object, Boolean, Double)->GEOM_Object

  ShapeListCompound = []
  i = 0
  while i <= 3 :
        S = geompy.MakeTranslation(Arc, i * 100., i * 100., i * 100.)
        ShapeListCompound.append(S)
        i = i + 1
  Compound = geompy.MakeCompound(ShapeListCompound)  #(List of GEOM_Object)->GEOM_Object

  #Test point on surface creation
  p_on_face = geompy.MakeVertexOnSurface(Face, 0.1, 0.8) #(GEOM_Object, Double, Double)->GEOM_Object
  p_on_face2 = geompy.MakeVertexOnSurfaceByCoord(Face, 0., 0., 0.) #(GEOM_Object, Double, Double, Double)->GEOM_Object
  p_on_face3 = geompy.MakeVertexInsideFace(Face) #(GEOM_Object)->GEOM_Object

  # Test plane from existing face creation
  Plane2 = geompy.MakePlaneFace(Face, trimsize)      #(GEOM_Object, Double)->GEOM_Object

  #ShapeList for Sewing
  S = geompy.MakeRotation(Face, vxy, angle1)

  #Test Line on Faces Intersection
  Line3 = geompy.MakeLineTwoFaces(prism1_faces[0], prism1_faces[1]) #(2 GEOM_Object)->GEOM_Object

  #Create advanced objects
  Prism            = geompy.MakePrismVecH(Face, vz, 100.0)     #(2 GEOM_Object, Double)->GEOM_Object
  Prism2Ways       = geompy.MakePrismVecH2Ways(Face, vz, 10.0) #(2 GEOM_Object, Double)->GEOM_Object
  PrismTwoPnt      = geompy.MakePrism(Face2, p0, pxyz)         #(3 GEOM_Object)->GEOM_Object
  PrismTwoPnt2Ways = geompy.MakePrism2Ways(Face2, p0, pxyz)    #(3 GEOM_Object)->GEOM_Object
  PrismDXDYDZ      = geompy.MakePrismDXDYDZ(Face2, 10, 20, 100)#(2 GEOM_Object, Double)->GEOM_Object
  PrismDXDYDZ2Ways = geompy.MakePrismDXDYDZ2Ways(Face, 30, -20, 200)#(2 GEOM_Object, Double)->GEOM_Object
  Revolution       = geompy.MakeRevolution(Face, vz, angle2)   #
  Revolution2Ways  = geompy.MakeRevolution(Face, vz, angle1)   #
  Filling          = geompy.MakeFilling(Compound, mindeg, maxdeg,
                                        tol2d, tol3d, nbiter)  #(GEOM_Object, 4 Doubles, Short)->GEOM_Object
  Pipe             = geompy.MakePipe(Wire, Edge)               #(2 GEOM_Object)->GEOM_Object
  Sewing           = geompy.MakeSewing([Face, S], precision)   #(List Of GEOM_Object, Double)->GEOM_Object
  ThickSolid       = geompy.MakeCopy(Box)
  faces            = geompy.SubShapeAllSortedCentres(Box, geompy.ShapeType["FACE"])
  shell            = geompy.MakeShell([faces[0], faces[1], faces[2]])
  faceIDs          = geompy.SubShapeAllSortedCentresIDs(ThickSolid, geompy.ShapeType["FACE"])
  ThickShell       = geompy.MakeThickSolid(shell, 50)          #(GEOM_Object, Double)->GEOM_Object
  geompy.Thicken(ThickSolid, 50, [faceIDs[0], faceIDs[1]])     #(GEOM_Object) modification
  Copy             = geompy.MakeCopy(Box)                      #(GEOM_Object)->GEOM_Object

  #Transform objects
  Translation = geompy.MakeTranslationTwoPoints(Box, px, pz)    #(3 GEOM_Object)->GEOM_Object
  TranslVect  = geompy.MakeTranslationVector(Box, vxyz)         #(2 GEOM_Object)->GEOM_Object
  TranslVectD = geompy.MakeTranslationVectorDistance(Box, vxyz, 50.0)   #(2 GEOM_Object)->GEOM_Object  
  Rotation    = geompy.MakeRotation(Box, vz, angle1)            #(2 GEOM_Object, Double)->GEOM_Object
  RotatPnt    = geompy.MakeRotationThreePoints(Box, px, py, pz) #(4 GEOM_Object)->GEOM_Object

  #Scale by factor relatively given point
  Scale1 = geompy.MakeScaleTransform(Box, pxyz, factor)      #(2 GEOM_Object, Double)->GEOM_Object
  #Scale by factor relatively the origin of global CS
  Scale2 = geompy.MakeScaleTransform(Box, None, factor)      #
  #Scale along axes of global CS by different factors. Scale relatively given point
  Scale3 = geompy.MakeScaleAlongAxes(Box, pxyz, 1.5, 0.5, 3) #(2 GEOM_Object, 3 Doubles)->GEOM_Object
  #Scale along axes of global CS by different factors. Scale relatively the origin of global CS
  Scale4 = geompy.MakeScaleAlongAxes(Box, None, 1.5, 0.5, 3) #

  Mirror      = geompy.MakeMirrorByPlane(Box, Plane) #(2 GEOM_Object)->GEOM_Object
  MirrorAxis  = geompy.MakeMirrorByAxis(Box, Line1)  #
  MirrorPnt   = geompy.MakeMirrorByPoint(Box, p200)  #
  Position    = geompy.MakePosition(Box, cs1, cs2)   #(3 GEOM_Object)->GEOM_Object
  Position2   = geompy.PositionAlongPath(Box, Arc, 0.5, 1, 0)  #(2 GEOM_Object, 1 Double, 2 Bool)->GEOM_Object
  Offset      = geompy.MakeOffset(Box, 10.)          #(GEOM_Object, Double)->GEOM_Object
  ProjOnWire  = geompy.MakeProjectionOnWire(p0, Wire)
  ProjOnCyl   = geompy.MakeProjectionOnCylinder(Wire, 100)
  Orientation = geompy.ChangeOrientation(Box)
  ExtEdge     = geompy.ExtendEdge(Edge1, -0.3, 1.3)
  ExtFace     = geompy.ExtendFace(Face5, -0.3, 1.3, -0.1, 1.1)
  Surface     = geompy.MakeSurfaceFromFace(Face5)

  #IDList for Fillet/Chamfer
  prism_edges = geompy.ExtractShapes(Prism, geompy.ShapeType["EDGE"], True)

  for anEdge in prism_edges:
    eid = geompy.GetSubShapeID(Prism, anEdge)
    sse = geompy.GetSubShape(Prism, [eid])

    sse_id = geompy.GetSubShapeID(Prism, sse)
    if sse_id != eid:
      print "Error: GetSubShape() or GetSubShapeID() has failed!"

  IDlist_e = []
  IDlist_e.append(geompy.GetSubShapeID(Prism, prism_edges[0]))
  IDlist_e.append(geompy.GetSubShapeID(Prism, prism_edges[1]))
  IDlist_e.append(geompy.GetSubShapeID(Prism, prism_edges[2]))

  prism_faces = geompy.ExtractShapes(Prism, geompy.ShapeType["FACE"], True)

  f_ind_1 = geompy.GetSubShapeID(Prism, prism_faces[0])
  f_ind_2 = geompy.GetSubShapeID(Prism, prism_faces[1])

  IDlist_f = [f_ind_1, f_ind_2]
  
  #Local operations
  Fillet2d = geompy.MakeFillet2D(Face3, radius, [4, 7, 9]) #(GEOM_Object, Double, ListOfLong)->GEOM_Object
  Fillet   = geompy.MakeFillet (Prism, radius, geompy.ShapeType["EDGE"],
                                IDlist_e) #(GEOM_Object, Double, Short, ListOfLong)->GEOM_Object
  Fillet2  = geompy.MakeFilletR1R2 (Prism, 7., 13., geompy.ShapeType["EDGE"],
                                    IDlist_e) #(GEOM_Object, Double, Double, Short, ListOfLong)->GEOM_Object
  Chamfer  = geompy.MakeChamferEdge(Prism, d1, d2,
                                    f_ind_1, f_ind_2) #(GEOM_Object, 2 Doubles, 2 Long)->GEOM_Object
  Chamfer2 = geompy.MakeChamferFaces(Prism, d1, d2,
                                     IDlist_f) #(GEOM_Object, 2 Doubles, ListOfLong)->GEOM_Object
  Chamfer3 = geompy.MakeChamferEdges(Prism, d1, d2,
                                     IDlist_e) #(GEOM_Object, 2 Doubles, ListOfLong)->GEOM_Object
  Chamfer4 = geompy.MakeChamferFacesAD(Prism, d1, 20. * math.pi / 180.,
                                       IDlist_f) #(GEOM_Object, 2 Doubles, ListOfLong)->GEOM_Object
  #End of Local operations

  #Create Patterns
  MultiTrans1D = geompy.MakeMultiTranslation1D(Fillet, vz, step1, nbtimes1)
  MultiTrans2D = geompy.MakeMultiTranslation2D(Fillet, vz, step1, nbtimes1, vy, step2, nbtimes2)
  MultiRot1Dt  = geompy.MultiRotate1DNbTimes(Chamfer, vx, nbtimes1)
  MultiRot1Ds  = geompy.MultiRotate1DByStep(Chamfer, vx, math.pi/4., nbtimes1)
  MultiRot2Dt  = geompy.MultiRotate2DNbTimes(Chamfer, vx, nbtimes1, step1, nbtimes2)
  MultiRot2Ds  = geompy.MultiRotate2DByStep(Chamfer, vx, angle1, nbtimes1, step1, nbtimes2)

  #Create Informations objects
  CDG        = geompy.MakeCDG(Prism)               #(GEOM_Object)->GEOM_Object
  Archimede  = geompy.Archimede(Box, weight, waterdensity,
                                meshingdeflection) #(GEOM_Object, 3 Doubles)->GEOM_Object
  mindist = geompy.MinDistanceComponents(TranslVect, Mirror) #(2 GEOM_Object)->4 Doubles
  print "Minumal distance between TranslVect and Mirror is", mindist[0],
  print "by components:", mindist[1], ",", mindist[2], ",", mindist[3]
  CheckShape = geompy.CheckShape(Prism)            #(GEOM_Object)->Boolean
  print "CheckShape(Prism) = ", CheckShape

  #Partition objects
  Partition  = geompy.MakePartition([Box], [Plane]) #(2 Lists Of GEOM_Object)->GEOM_Object
  Partition1 = geompy.MakeHalfPartition(Box, Plane) #(2 GEOM_Object)->GEOM_Object

  #Add In Study
  id_p0   = geompy.addToStudy(p0,   "Vertex 0")
  id_px   = geompy.addToStudy(px,   "Vertex X")
  id_py   = geompy.addToStudy(py,   "Vertex Y")
  id_pz   = geompy.addToStudy(pz,   "Vertex Z")
  id_pxyz = geompy.addToStudy(pxyz, "Vertex XYZ")
  id_p200 = geompy.addToStudy(p200, "Vertex 200")

  id_vx   = geompy.addToStudy(vx,   "Vector X")
  id_vy   = geompy.addToStudy(vy,   "Vector Y")
  id_vz   = geompy.addToStudy(vz,   "Vector Z")
  id_vxy  = geompy.addToStudy(vxy,  "Vector XY")
  id_vxyz = geompy.addToStudy(vxyz, "Vector XYZ")

  id_cs1 = geompy.addToStudy(cs1, "CS 50,50,50, 1,0,0, 0,1,0")
  id_cs2 = geompy.addToStudy(cs2, "CS 70,80,10, 1,0,1, 1,1,0")
  id_cs3 = geompy.addToStudy(cs3, "CS: pz, vxy, vz")
  id_cs4 = geompy.addToStudy(cs4, "CS: Plane")

  id_Line   = geompy.addToStudy(Line,   "Line")
  id_Line1  = geompy.addToStudy(Line1,  "Line by point and vector")
  id_Line3  = geompy.addToStudy(Line3,  "Line on Two Faces Intersection")
  id_Plane  = geompy.addToStudy(Plane,  "Plane")
  id_Plane1 = geompy.addToStudy(Plane1,  "Plane by 3 points")
  id_Plane2 = geompy.addToStudy(Plane2,  "Plane by 2 vectors")
  id_Plane3 = geompy.addToStudy(Plane3,  "Plane by LCS")

  id_Arc      = geompy.addToStudy(Arc,      "Arc")
  id_Arc2     = geompy.addToStudy(Arc2,     "Arc2")
  id_Arc3     = geompy.addToStudy(Arc3,     "Arc3")
  id_Circle   = geompy.addToStudy(Circle,   "Circle")
  id_Circle1  = geompy.addToStudy(Circle1,  "Circle by 3 points")
  id_Circle2  = geompy.addToStudy(Circle2,  "Circle by center and 2 points")
  id_Ellipse  = geompy.addToStudy(Ellipse,  "Ellipse")
  id_Polyline = geompy.addToStudy(Polyline, "Polyline")
  id_Bezier   = geompy.addToStudy(Bezier,   "Bezier")
  id_Interpol = geompy.addToStudy(Interpol, "Interpol")
  id_InterpT1 = geompy.addToStudy(InterpT1, "InterpT1")
  id_InterpT2 = geompy.addToStudy(InterpT2, "InterpT2")
  id_Sketcher = geompy.addToStudy(Sketcher, "Sketcher")

  id_Sketcher3d_1 = geompy.addToStudy(Sketcher3d_1, "Sketcher 3D by interface")
  id_Sketcher3d_2 = geompy.addToStudy(Sketcher3d_2, "Sketcher 3D by list")

  id_p_on_arc  = geompy.addToStudy(p_on_arc,  "Vertex on Arc (0.25)")
  id_p_on_arc2 = geompy.addToStudy(p_on_arc2, "Vertex on Arc at(100, -10, 10)" )
  id_p_on_arc3 = geompy.addToStudy(p_on_arc3, "Vertex on Arc length 50 from Vertex X" )
  
  id_p_on_l1l2 = geompy.addToStudy(p_on_l1l2, "Vertex on Lines Intersection")

  id_tan_on_arc  = geompy.addToStudy(tan_on_arc, "Tangent on Arc (0.7)")
  id_tan_on_face = geompy.addToStudy(tan_on_face, "Tangent on Face")

  id_Box      = geompy.addToStudy(Box,      "Box")
  id_Box1     = geompy.addToStudy(Box1,     "Box 10x20x30")
  id_Box2     = geompy.addToStudy(Box2,     "Box (10,20,30)-(15,25,35)")
  id_Cylinder = geompy.addToStudy(Cylinder, "Cylinder")
  id_Cyl1     = geompy.addToStudy(Cyl1,     "Cylinder RH")
  id_Cylinder1= geompy.addToStudy(Cylinder1,"CylinderA")
  id_Cyl2     = geompy.addToStudy(Cyl2,     "Cylinder RHA")
  id_Sphere   = geompy.addToStudy(Sphere,   "Sphere Pnt R")
  id_Sphere1  = geompy.addToStudy(Sphere1,  "Sphere R")
  id_Sphere2  = geompy.addToStudy(Sphere2,  "Sphere")
  id_Cone     = geompy.addToStudy(Cone,     "Cone")
  id_Cone1    = geompy.addToStudy(Cone1,    "Cone R1R2H")
  id_Torus    = geompy.addToStudy(Torus,    "Torus")
  id_Torus1   = geompy.addToStudy(Torus1,   "Torus RR")

  id_Common  = geompy.addToStudy(Common,  "Common")
  id_Cut     = geompy.addToStudy(Cut,     "Cut")
  id_Cut2    = geompy.addToStudy(Cut2,    "Cut2")
  id_Fuse    = geompy.addToStudy(Fuse,    "Fuse")
  id_Section = geompy.addToStudy(Section, "Section")

  id_Edge     = geompy.addToStudy(Edge,     "Edge")
  id_Edge1    = geompy.addToStudy(Edge1,    "Edge on Arc length 50 from Vertex Y")
  id_Wire     = geompy.addToStudy(Wire,     "Wire")
  id_Face     = geompy.addToStudy(Face,     "Face")
  id_Face1    = geompy.addToStudy(Face1,    "Face from two wires")
  id_Face2    = geompy.addToStudy(Face2,    "Face from Sketcher")
  id_Face3    = geompy.addToStudy(Face3,    "Face Height Width")
  id_Face4    = geompy.addToStudy(Face4,    "Face Plane_HW")
  id_Face5    = geompy.addToStudy(Face5,    "Face from surface and wire")
  id_Face6    = geompy.addToStudy(Face6,    "Face from edges with constraints")
  id_Disk     = geompy.addToStudy(Disk,     "Disk PntVecR")
  id_Disk2    = geompy.addToStudy(Disk2,    "Disk Three Points")
  id_Disk3    = geompy.addToStudy(Disk3,    "Disk OXY Radius")
  id_Shell    = geompy.addToStudy(Shell,    "Shell")

  id_Isoline  = geompy.addToStudy(Isoline,  "Isoline")

  id_p_on_face = geompy.addToStudy(p_on_face, "Vertex on Face (0.1, 0.8)")
  id_p_on_face2 = geompy.addToStudy(p_on_face2, "Vertex on Face at(0., 0., 0.)")
  id_p_on_face3 = geompy.addToStudy(p_on_face3, "Vertex inside Face")

  id_Prism1   = geompy.addToStudy(Prism1,     "Prism by Two Pnt")
  id_Shell1   = geompy.addToStudy(Shell1,   "Shell from Prism1 faces")
  id_Solid    = geompy.addToStudy(Solid,    "Solid")
  id_Solid1   = geompy.addToStudy(Solid_from_shells,   "Solid1")
  
  id_Compound = geompy.addToStudy(Compound, "Compound")

  id_Plane2   = geompy.addToStudy(Plane2,   "Plane on Face")

  id_Copy       = geompy.addToStudy(Copy,       "Copy")
  id_ThickShell = geompy.addToStudy(ThickShell, "ThickShell")
  id_ThickSolid = geompy.addToStudy(ThickSolid, "ThickSolid")

  id_Prism            = geompy.addToStudy(Prism,            "Prism")
  id_Prism2Ways       = geompy.addToStudy(Prism2Ways,       "Prism2Ways")
  id_PrismTwoPnt      = geompy.addToStudy(PrismTwoPnt,      "PrismTwoPnt")
  id_PrismTwoPnt2Ways = geompy.addToStudy(PrismTwoPnt2Ways, "PrismTwoPnt2Ways")
  id_PrismDXDYDZ      = geompy.addToStudy(PrismDXDYDZ,      "PrismDXDYDZ")
  id_PrismDXDYDZ2Ways = geompy.addToStudy(PrismDXDYDZ2Ways, "PrismDXDYDZ2Ways")
  id_Revolution       = geompy.addToStudy(Revolution,       "Revolution")
  id_Revolution2Ways  = geompy.addToStudy(Revolution2Ways,  "Revolution2Ways")
  id_Filling    = geompy.addToStudy(Filling,    "Filling")
  id_Pipe       = geompy.addToStudy(Pipe,       "Pipe")
  id_Sewing     = geompy.addToStudy(Sewing,     "Sewing")

  import salome_version
  if int(salome_version.getXVersion(), 16) >= int('0x060600', 16):
    [Face5, Face6] = geompy.SubShapes(Box, [31, 33])
    [b1_e1, b1_e2, b1_e3, b1_e4] = geompy.SubShapes(Box, [12, 22, 25, 29])
    [b2_e1, b2_e2, b2_e3, b2_e4] = geompy.SubShapes(Box, [ 8, 18, 26, 30])
    Path1 = geompy.RestorePath(Box, Face5, Face6)
    Path2 = geompy.RestorePathEdges(Box, [b1_e1, b1_e2, b1_e3, b1_e4], [b2_e1, b2_e2, b2_e3, b2_e4])

    id_Base1      = geompy.addToStudyInFather(Box, Face5, "Base1")
    id_Base2      = geompy.addToStudyInFather(Box, Face6, "Base2")
    id_Base1e1    = geompy.addToStudyInFather(Box, b1_e1, "Base1 Edge1")
    id_Base1e2    = geompy.addToStudyInFather(Box, b1_e2, "Base1 Edge2")
    id_Base1e3    = geompy.addToStudyInFather(Box, b1_e3, "Base1 Edge3")
    id_Base1e4    = geompy.addToStudyInFather(Box, b1_e4, "Base1 Edge4")
    id_Base2e1    = geompy.addToStudyInFather(Box, b2_e1, "Base2 Edge1")
    id_Base2e2    = geompy.addToStudyInFather(Box, b2_e2, "Base2 Edge2")
    id_Base2e3    = geompy.addToStudyInFather(Box, b2_e3, "Base2 Edge3")
    id_Base2e4    = geompy.addToStudyInFather(Box, b2_e4, "Base2 Edge4")
    id_Path1      = geompy.addToStudy(Path1,      "Path1")
    id_Path2      = geompy.addToStudy(Path2,      "Path2")
    pass

  id_Translation = geompy.addToStudy(Translation,   "Translation")
  id_TranslVect  = geompy.addToStudy(TranslVect ,   "Translation along vector")
  id_TranslVectD = geompy.addToStudy(TranslVectD,   "Translation along vector with defined distance")
  id_Rotation    = geompy.addToStudy(Rotation,      "Rotation")
  id_RotatPnt    = geompy.addToStudy(RotatPnt,      "Rotation by three points")
  id_Scale1      = geompy.addToStudy(Scale1,        "Scale1")
  id_Scale2      = geompy.addToStudy(Scale2,        "Scale2")
  id_Scale3      = geompy.addToStudy(Scale3,        "Scale3")
  id_Scale4      = geompy.addToStudy(Scale4,        "Scale4")
  id_Mirror      = geompy.addToStudy(Mirror,        "Mirror by Plane")
  id_MirrorAxis  = geompy.addToStudy(MirrorAxis,    "Mirror by Axis")
  id_MirrorPnt   = geompy.addToStudy(MirrorPnt,     "Mirror by Point")
  id_Position    = geompy.addToStudy(Position,      "Positioned box")
  id_Position2   = geompy.addToStudy(Position2,     "Positioned box along path")
  id_Offset      = geompy.addToStudy(Offset,        "Offset")
  id_Orientation = geompy.addToStudy(Orientation,   "Orientation")
  id_ProjOnWire  = geompy.addToStudy(ProjOnWire[1], "ProjOnWire")
  id_ProjOnCyl   = geompy.addToStudy(ProjOnCyl,     "ProjOnCyl")
  id_ExtEdge     = geompy.addToStudy(ExtEdge,       "ExtendedEdge")
  id_ExtFace     = geompy.addToStudy(ExtFace,       "ExtendedFace")
  id_Surface     = geompy.addToStudy(Surface,       "Surface From Face")

  id_Fillet   = geompy.addToStudy(Fillet,   "Fillet")
  id_Fillet2  = geompy.addToStudy(Fillet2,  "Fillet2")
  id_Fillet2d = geompy.addToStudy(Fillet2d, "Fillet2D")

  id_Chamfer  = geompy.addToStudy(Chamfer,  "Chamfer on Edge")
  id_Chamfer2 = geompy.addToStudy(Chamfer2, "Chamfer on Faces")
  id_Chamfer3 = geompy.addToStudy(Chamfer3, "Chamfer on Edges")
  id_Chamfer4 = geompy.addToStudy(Chamfer4, "Chamfer on Faces with params D Angle")

  id_MultiTrans1D = geompy.addToStudy(MultiTrans1D, "MultiTrans1D")
  id_MultiTrans2D = geompy.addToStudy(MultiTrans2D, "MultiTrans2D")
  id_MultiRot1Dt  = geompy.addToStudy(MultiRot1Dt,  "MultiRot1D NbTimes")
  id_MultiRot1Ds  = geompy.addToStudy(MultiRot1Ds,  "MultiRot1D ByStep")
  id_MultiRot2Dt  = geompy.addToStudy(MultiRot2Dt,  "MultiRot2D NbTimes")
  id_MultiRot2Ds  = geompy.addToStudy(MultiRot2Ds,  "MultiRot2D ByStep")

  id_CDG       = geompy.addToStudy(CDG,       "CDG")
  id_Archimede = geompy.addToStudy(Archimede, "Archimede")

  id_Partition  = geompy.addToStudy(Partition, "Partition")
  id_Partition1 = geompy.addToStudy(Partition1, "Half Partition")
  
  #Decompose objects

  # SubShape
  SubFace    = geompy.SubShape(Box, geompy.ShapeType["FACE"], [2])
  name       = geompy.SubShapeName(SubFace, Box)
  id_SubFace = geompy.addToStudyInFather(Box, SubFace, name)

  # SubShapeSortedCentres
  SubFaceS   = geompy.SubShapeSortedCentres(Box, geompy.ShapeType["FACE"], [5])
  nameS      = geompy.SubShapeName(SubFaceS, Box)
  id_SubFace = geompy.addToStudyInFather(Box, SubFaceS, nameS)

  # GetExistingSubObjects
  SubObjsAll = geompy.GetExistingSubObjects(Box, True)
  print "For now, Box has the following created sub-objects:", SubObjsAll

  # GetGroups
  SubGrpsAll = geompy.GetGroups(Box)
  print "For now, Box has the following created groups:", SubGrpsAll

  # SubShapeAll
  SubEdgeList = geompy.SubShapeAll(SubFace, geompy.ShapeType["EDGE"])
  i=0
  for SubEdge in SubEdgeList :
    name = geompy.SubShapeName(SubEdge, SubFace)
    id_SubEdge = geompy.addToStudyInFather(SubFace, SubEdge, name)

  # SubShapeAllIDs
  SubEdgeIDsList = geompy.SubShapeAllIDs(SubFace, geompy.ShapeType["EDGE"])
  print "IDs of edges of SubFace:", SubEdgeIDsList, "(unsorted)"
  group = geompy.CreateGroup(SubFace, geompy.ShapeType["EDGE"])
  geompy.UnionIDs(group, SubEdgeIDsList)
  geompy.addToStudyInFather(SubFace, group, "Group of all edges")

  # SubShapeAllSortedCentresIDs
  SubEdgeIDsList = geompy.SubShapeAllSortedCentresIDs(SubFace, geompy.ShapeType["EDGE"])
  print "IDs of edges of SubFace:", SubEdgeIDsList, "(sorted)"

  # GetSubShape and GetSubShapeID
  for ind in SubEdgeIDsList:
    edge = geompy.GetSubShape(SubFace, [ind])
    ind_e = geompy.GetSubShapeID(SubFace, edge)
    if ind_e != ind:
      print "Error in GetSubShape or GetSubShapeID"

  # RestoreSubShapes
  geompy.RestoreSubShapes(Copy)
  geompy.RestoreSubShapes(RotatPnt, [], GEOM.FSM_Transformed)
  geompy.RestoreSubShapes(Partition, [Box])
  geompy.RestoreSubShapes(Partition1)

  # GetSubShapeEdgeSorted
  p1 = geompy.GetFirstVertex(Sketcher)
  p2 = geompy.GetFirstVertex(Sketcher3d_1)
  p3 = geompy.GetFirstVertex(Sketcher3d_2)
  geompy.GetSubShapeEdgeSorted(Sketcher, p1, "OrderedEdges")
  geompy.GetSubShapeEdgeSorted(Sketcher3d_1, p2, "OrderedEdges")
  geompy.GetSubShapeEdgeSorted(Sketcher3d_2, p3, "OrderedEdges")

  # GetSubShapesWithTolerance
  geompy.GetSubShapesWithTolerance(Box, GEOM.FACE, GEOM.CC_GT, 1.e-8, "gt")
  geompy.GetSubShapesWithTolerance(Box, GEOM.FACE, GEOM.CC_GE, 1.e-7, "ge")
  geompy.GetSubShapesWithTolerance(Box, GEOM.FACE, GEOM.CC_LT, 2.e-7, "lt")
  geompy.GetSubShapesWithTolerance(Box, GEOM.FACE, GEOM.CC_LE, 1.e-7, "le")

  # MakeExtraction
  geompy.MakeExtraction(Box, [13], "Ext_no_face")
  geompy.MakeExtraction(Box, [18], "Ext_no_edge")
  geompy.MakeExtraction(Box, [16], "Ext_no_vertex")


  print "DONE"
