#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
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
#  File   : GEOM_usinggeom.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM
#  $Header$


def TestAll (geompy, math):

  #Create base Variables
  nbtimes1  = 5      #Short
  nbtimes2  = 5
  mindeg = 2
  maxdeg = 5
  nbiter = 5
  ShapeTypeFace = geompy.ShapeType["FACE"]
  ShapeTypeEdge = geompy.ShapeType["EDGE"]
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
  p0   = geompy.MakeVertex(0.  , 0.  , 0.  ) #(3 Doubles)->GEOM_Object_ptr
  px   = geompy.MakeVertex(100., 0.  , 0.  )
  py   = geompy.MakeVertex(0.  , 100., 0.  )
  pz   = geompy.MakeVertex(0.  , 0.  , 100.)
  pxyz = geompy.MakeVertex(100., 100., 100.)

  p200 = geompy.MakeVertexWithRef(pxyz, 100., 100., 100.) #(GEOM_Object_ptr, 3 Doubles)->GEOM_Object_ptr

  #Create base directions
  vx   = geompy.MakeVector(p0, px) #(GEOM_Object_ptr, GEOM_Object_ptr)->GEOM_Object_ptr
  vy   = geompy.MakeVector(p0, py)
  vz   = geompy.MakeVector(p0, pz)
  vxy  = geompy.MakeVector(px, py)

  vxyz = geompy.MakeVectorDXDYDZ(100., 100., 100.) #(3 Doubles)->GEOM_Object_ptr

  #Create local coordinate systems
  cs1 = geompy.MakeMarker(50,50,50, 1,0,0, 0,1,0)
  cs2 = geompy.MakeMarker(70,80,10, 1,0,1, 1,1,0)

  #Create base geometry 2D
  Line   = geompy.MakeLineTwoPnt(p0, pxyz)                  #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Line1  = geompy.MakeLine(pz, vxy)                         #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Line2  = geompy.MakeLineTwoPnt(pxyz, pz)                   #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Plane  = geompy.MakePlane(pz, vxyz, trimsize)             #(2 GEOM_Object_ptr, Double)->GEOM_Object_ptr
  Plane1 = geompy.MakePlaneThreePnt(px, pz, p200, trimsize) #(4 Doubles)->GEOM_Object_ptr

  Arc      = geompy.MakeArc(py, pz, px)                   #(3 GEOM_Object_ptr)->GEOM_Object_ptr
  Arc2     = geompy.MakeArcCenter(py, pz, px,0)           #(3 GEOM_Object_ptr,Boolean)->GEOM_Object_ptr
  Circle   = geompy.MakeCircle(p0, vz, radius1)           #(2 GEOM_Object_ptr, Double)->GEOM_Object_ptr
  Circle1  = geompy.MakeCircleThreePnt(p0, pxyz, px)      #(3 GEOM_Object_ptr)->GEOM_Object_ptr
  Circle2  = geompy.MakeCircleCenter2Pnt(p0, pxyz, py)    #(3 GEOM_Object_ptr)->GEOM_Object_ptr
  Ellipse  = geompy.MakeEllipse(p0, vy, radius2, radius1) #(2 GEOM_Object_ptr, 2 Doubles)->GEOM_Object_ptr
  Polyline = geompy.MakePolyline([p0, pz, py, p200])      #(List of GEOM_Object_ptr)->GEOM_Object_ptr
  Bezier   = geompy.MakeBezier([p0, pz, p200, px])        #(List of GEOM_Object_ptr)->GEOM_Object_ptr
  Interpol = geompy.MakeInterpol([px, py, p200, pxyz])    #(List of GEOM_Object_ptr)->GEOM_Object_ptr
  Sketcher = geompy.MakeSketcher("Sketcher:F -100 -100:TT 250 -100:R 0:C 100 150:R 0:L 300:WW",
                                 [100,0,0, 1,1,1, -1,1,0]) #(String, List of Doubles)->GEOM_Object_ptr

  #Test point on curve creation
  p_on_arc = geompy.MakeVertexOnCurve(Arc, 0.25) #(GEOM_Object_ptr, Double)->GEOM_Object_ptr

  #Test point on lines intersection
  pLine   = geompy.MakeVertexOnLinesIntersection( Line1, Line2 )

  #Create base geometry 3D
  Box      = geompy.MakeBoxTwoPnt(p0, p200)                   #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Box1     = geompy.MakeBoxDXDYDZ(10, 20, 30)                 #(3 Doubles)->GEOM_Object_ptr
  Box2     = geompy.MakeBox(10,20,30, 15,25,35)               #(6 Doubles)->GEOM_Object_ptr
  Cylinder = geompy.MakeCylinder(p0, vz, radius1, height)     #(2 GEOM_Object_ptr, 2 Doubles)->GEOM_Object_ptr
  Cyl1     = geompy.MakeCylinderRH(radius2, height)           #(2 Doubles)->GEOM_Object_ptr
  Sphere   = geompy.MakeSpherePntR(p0, radius1)               #(GEOM_Object_ptr, Double)->GEOM_Object_ptr
  Sphere1  = geompy.MakeSphereR(radius)                       #(Double)->GEOM_Object_ptr
  Sphere2  = geompy.MakeSphere(50, 70, 30, radius)            #(4 Doubles)->GEOM_Object_ptr
  Cone     = geompy.MakeCone(p0, vz, radius2, radius, height) #(2 GEOM_Object_ptr, 3 Doubles)->GEOM_Object_ptr
  Cone1    = geompy.MakeConeR1R2H(radius1, radius, height)    #(3 Doubles)->GEOM_Object_ptr
  Torus    = geompy.MakeTorus(p0, vz, radius2, radius)        #(2 GEOM_Object_ptr, 2 Doubles)->GEOM_Object_ptr
  Torus1   = geompy.MakeTorusRR(radius2, radius1)             #(2 Doubles)->GEOM_Object_ptr

  #Boolean (Common, Cut, Fuse, Section)
  Common  = geompy.MakeBoolean(Box, Sphere, 1) #(2 GEOM_Object_ptr, Short)->GEOM_Object_ptr
  Cut     = geompy.MakeBoolean(Box, Sphere, 2)
  Fuse    = geompy.MakeBoolean(Box, Sphere, 3)
  Section = geompy.MakeBoolean(Box, Sphere, 4)

  #Create base objects
  Edge     = geompy.MakeEdge(p0, pxyz)               #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Wire     = geompy.MakeWire([vxy, Arc])             #(List Of GEOM_Object_ptr)->GEOM_Object_ptr
  Face     = geompy.MakeFace(Wire, WantPlanarFace)   #(GEOM_Object_ptr, Boolean)->GEOM_Object_ptr
  Face1    = geompy.MakeFaceWires([Wire, Sketcher],
                                  WantPlanarFace)    #(List of GEOM_Object_ptr, Boolean)->GEOM_Object_ptr
  Face2    = geompy.MakeFace(Sketcher, WantPlanarFace)
  Shell    = geompy.MakeShell([Face, Face1])         #(List of GEOM_Object_ptr)->GEOM_Object_ptr

  Prism1   = geompy.MakePrism(Face2, p0, pxyz)       #(3 GEOM_Object_ptr)->GEOM_Object_ptr
  prism1_faces = geompy.SubShapeAllSorted(Prism1, ShapeTypeFace)
  Shell1   = geompy.MakeShell([prism1_faces[0], prism1_faces[1],
                               prism1_faces[3], prism1_faces[4],
                               prism1_faces[5], prism1_faces[2]])
  Solid    = geompy.MakeSolid([Shell1])              #(List of GEOM_Object_ptr)->GEOM_Object_ptr

  ShapeListCompound = []
  i = 0
  while i <= 3 :
        S = geompy.MakeTranslation(Arc, i * 100., i * 100., i * 100.)
        ShapeListCompound.append(S)
        i = i + 1
  Compound = geompy.MakeCompound(ShapeListCompound)  #(List of GEOM_Object_ptr)->GEOM_Object_ptr

  # Test plane from existing face creation
  Plane2 = geompy.MakePlaneFace(Face, trimsize)      #(GEOM_Object_ptr, Double)->GEOM_Object_ptr

  #ShapeList for Sewing
  S = geompy.MakeRotation(Face, vxy, angle1)
	
  #Test Line on Faces Intersection
  Line3  = geompy.MakeLineTwoFaces( prism1_faces[0], prism1_faces[1]) #(2 GEOM_Object_ptr)->GEOM_Object_ptr

  #Create advanced objects
  Copy       = geompy.MakeCopy(Box)                      #(GEOM_Object_ptr)->GEOM_Object_ptr
  Prism      = geompy.MakePrismVecH(Face, vz, 100.0)     #(2 GEOM_Object_ptr, Double)->GEOM_Object_ptr
  Prism2Ways = geompy.MakePrismVecH2Ways(Face, vz, 10.0) #(2 GEOM_Object_ptr, Double)->GEOM_Object_ptr
  Revolution = geompy.MakeRevolution(Face, vz, angle2)   #
  Revolution2Ways = geompy.MakeRevolution(Face, vz, angle1)   #
  Filling    = geompy.MakeFilling(Compound, mindeg, maxdeg,
                                  tol2d, tol3d, nbiter)  #(GEOM_Object_ptr, 4 Doubles, Short)->GEOM_Object_ptr
  Pipe       = geompy.MakePipe(Wire, Edge)               #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Sewing     = geompy.MakeSewing([Face, S], precision)   #(List Of GEOM_Object_ptr, Double)->GEOM_Object_ptr

  #Transform objects
  Translation = geompy.MakeTranslationTwoPoints(Box, px, pz)    #(3 GEOM_Object_ptr)->GEOM_Object_ptr
  TranslVect  = geompy.MakeTranslationVector(Box, vxyz)         #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  Rotation    = geompy.MakeRotation(Box, vz, angle1)            #(2 GEOM_Object_ptr, Double)->GEOM_Object_ptr
  RotatPnt    = geompy.MakeRotationThreePoints(Box, px, py, pz) #(4 GEOM_Object_ptr)->GEOM_Object_ptr
  Scale       = geompy.MakeScaleTransform(Box, p0, factor)      #
  Mirror      = geompy.MakeMirrorByPlane(Box, Plane)            #(2 GEOM_Object_ptr)->GEOM_Object_ptr
  MirrorAxis  = geompy.MakeMirrorByAxis(Box, Line1)             #
  MirrorPnt   = geompy.MakeMirrorByPoint(Box, p200)             #
  Position    = geompy.MakePosition(Box, cs1, cs2)              #(3 GEOM_Object_ptr)->GEOM_Object_ptr
  Offset      = geompy.MakeOffset(Box, 10.)                     #(GEOM_Object_ptr, Double)->GEOM_Object_ptr
  Orientation = geompy.ChangeOrientation(Box)

  #IDList for Fillet/Chamfer
  prism_edges = geompy.SubShapeAllSorted(Prism, ShapeTypeEdge)

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

  prism_faces = geompy.SubShapeAllSorted(Prism, ShapeTypeFace)

  f_ind_1 = geompy.GetSubShapeID(Prism, prism_faces[0])
  f_ind_2 = geompy.GetSubShapeID(Prism, prism_faces[1])

  IDlist_f = [f_ind_1, f_ind_2]
  
  #Local operations
  Fillet   = geompy.MakeFillet (Prism, radius, ShapeTypeEdge,
                                IDlist_e) #(GEOM_Object_ptr, Double, Short, ListOfLong)->GEOM_Object_ptr
  Fillet2  = geompy.MakeFilletR1R2 (Prism, 7., 13., ShapeTypeEdge,
                                    IDlist_e) #(GEOM_Object_ptr, Double, Double, Short, ListOfLong)->GEOM_Object_ptr
  Chamfer  = geompy.MakeChamferEdge(Prism, d1, d2,
                                    f_ind_1, f_ind_2) #(GEOM_Object_ptr, 2 Doubles, 2 Long)->GEOM_Object_ptr
  Chamfer2 = geompy.MakeChamferFaces(Prism, d1, d2,
                                     IDlist_f) #(GEOM_Object_ptr, 2 Doubles, ListOfLong)->GEOM_Object_ptr
  Chamfer3 = geompy.MakeChamferEdges(Prism, d1, d2,
                                     IDlist_e) #(GEOM_Object_ptr, 2 Doubles, ListOfLong)->GEOM_Object_ptr
  Chamfer4 = geompy.MakeChamferFacesAD(Prism, d1, 20. * math.pi / 180.,
                                       IDlist_f) #(GEOM_Object_ptr, 2 Doubles, ListOfLong)->GEOM_Object_ptr
  #Create Patterns
  MultiTrans1D = geompy.MakeMultiTranslation1D(Fillet, vz, step1, nbtimes1)
  MultiTrans2D = geompy.MakeMultiTranslation2D(Fillet, vz, step1, nbtimes1, vy, step2, nbtimes2)
  #!!!!Angle In Degree!!!!
  MultiRot1D   = geompy.MultiRotate1D(Chamfer, vx, nbtimes1)
  MultiRot2D   = geompy.MultiRotate2D(Chamfer, vx, angle, nbtimes1, step1, nbtimes2)

  #Create Informations objects
  CDG        = geompy.MakeCDG(Prism)               #(GEOM_Object_ptr)->GEOM_Object_ptr
  Archimede  = geompy.Archimede(Box, weight, waterdensity,
                                meshingdeflection) #(GEOM_Object_ptr, 3 Doubles)->GEOM_Object_ptr
  CheckShape = geompy.CheckShape(Prism)            #(GEOM_Object_ptr)->Boolean
  print "CheckShape(Prism) = ", CheckShape

  #Partition objects
  Partition  = geompy.MakePartition([Box], [Plane]) #(2 Lists Of GEOM_Object_ptr)->GEOM_Object_ptr
  Partition1 = geompy.MakeHalfPartition(Box, Plane) #(2 GEOM_Object_ptr)->GEOM_Object_ptr

  #Add In Study

  id_p0   = geompy.addToStudy(p0,   "Vertex 0")
  id_px   = geompy.addToStudy(px,   "Vertex X")
  id_py   = geompy.addToStudy(py,   "Vertex Y")
  id_pz   = geompy.addToStudy(pz,   "Vertex Z")
  id_pxyz = geompy.addToStudy(pxyz, "Vertex XYZ")
  id_p200 = geompy.addToStudy(p200, "Vertex 200")
  id_pLine = geompy.addToStudy(pLine, "Vertex on Lines Intersection")	

  id_vx   = geompy.addToStudy(vx,   "Vector X")
  id_vy   = geompy.addToStudy(vy,   "Vector Y")
  id_vz   = geompy.addToStudy(vz,   "Vector Z")
  id_vxy  = geompy.addToStudy(vxy,  "Vector XY")
  id_vxyz = geompy.addToStudy(vxyz, "Vector XYZ")

  id_cs1 = geompy.addToStudy(cs1, "CS 50,50,50, 1,0,0, 0,1,0")
  id_cs2 = geompy.addToStudy(cs2, "CS 70,80,10, 1,0,1, 1,1,0")

  id_Line   = geompy.addToStudy(Line,   "Line")
  id_Line1  = geompy.addToStudy(Line1,  "Line by point and vector")
  id_Line3  = geompy.addToStudy(Line3,  "Line on Two Faces Intersection")
  id_Plane  = geompy.addToStudy(Plane,  "Plane")
  id_Plane1 = geompy.addToStudy(Plane1,  "Plane by 3 points")

  id_Arc      = geompy.addToStudy(Arc,      "Arc")
  id_Circle   = geompy.addToStudy(Circle,   "Circle")
  id_Circle1  = geompy.addToStudy(Circle1,  "Circle by 3 points")
  id_Circle2  = geompy.addToStudy(Circle2,  "Circle by center and 2 points")
  id_Ellipse  = geompy.addToStudy(Ellipse,  "Ellipse")
  id_Polyline = geompy.addToStudy(Polyline, "Polyline")
  id_Bezier   = geompy.addToStudy(Bezier,   "Bezier")
  id_Interpol = geompy.addToStudy(Interpol, "Interpol")
  id_Sketcher = geompy.addToStudy(Sketcher, "Sketcher")

  id_p_on_arc = geompy.addToStudy(p_on_arc, "Vertex on Arc")

  id_Box      = geompy.addToStudy(Box,      "Box")
  id_Box1     = geompy.addToStudy(Box1,     "Box 10x20x30")
  id_Box2     = geompy.addToStudy(Box2,     "Box (10,20,30)-(15,25,35)")
  id_Cylinder = geompy.addToStudy(Cylinder, "Cylinder")
  id_Cyl1     = geompy.addToStudy(Cyl1,     "Cylinder RH")
  id_Sphere   = geompy.addToStudy(Sphere,   "Sphere Pnt R")
  id_Sphere1  = geompy.addToStudy(Sphere1,  "Sphere R")
  id_Sphere2  = geompy.addToStudy(Sphere2,  "Sphere")
  id_Cone     = geompy.addToStudy(Cone,     "Cone")
  id_Cone1    = geompy.addToStudy(Cone1,    "Cone R1R2H")
  id_Torus    = geompy.addToStudy(Torus,    "Torus")
  id_Torus1   = geompy.addToStudy(Torus1,   "Torus RR")

  id_Common  = geompy.addToStudy(Common,  "Common")
  id_Cut     = geompy.addToStudy(Cut,     "Cut")
  id_Fuse    = geompy.addToStudy(Fuse,    "Fuse")
  id_Section = geompy.addToStudy(Section, "Section")

  id_Edge     = geompy.addToStudy(Edge,     "Edge")
  id_Wire     = geompy.addToStudy(Wire,     "Wire")
  id_Face     = geompy.addToStudy(Face,     "Face")
  id_Face1    = geompy.addToStudy(Face1,    "Face from two wires")
  id_Face2    = geompy.addToStudy(Face2,    "Face from Sketcher")
  id_Shell    = geompy.addToStudy(Shell,    "Shell")

  id_Prism1   = geompy.addToStudy(Prism1,     "Prism by Two Pnt")
  id_Shell1   = geompy.addToStudy(Shell1,   "Shell from Prism1 faces")
  id_Solid    = geompy.addToStudy(Solid,    "Solid")
  id_Compound = geompy.addToStudy(Compound, "Compound")

  id_Plane2   = geompy.addToStudy(Plane2,   "Plane on Face")

  id_Copy       = geompy.addToStudy(Copy,       "Copy")
  id_Prism      = geompy.addToStudy(Prism,      "Prism")
  id_Prism2Ways = geompy.addToStudy(Prism2Ways, "Prism2Ways")
  id_Revolution = geompy.addToStudy(Revolution, "Revolution")
  id_Revolution2Ways = geompy.addToStudy(Revolution2Ways, "Revolution2Ways")
  id_Filling    = geompy.addToStudy(Filling,    "Filling")
  id_Pipe       = geompy.addToStudy(Pipe,       "Pipe")
  id_Sewing     = geompy.addToStudy(Sewing,     "Sewing")

  id_Translation = geompy.addToStudy(Translation, "Translation")
  id_TranslVect  = geompy.addToStudy(TranslVect , "Translation along vector")
  id_Rotation    = geompy.addToStudy(Rotation,    "Rotation")
  id_RotatPnt    = geompy.addToStudy(RotatPnt,    "Rotation by three points")
  id_Scale       = geompy.addToStudy(Scale,       "Scale")
  id_Mirror      = geompy.addToStudy(Mirror,      "Mirror by Plane")
  id_MirrorAxis  = geompy.addToStudy(MirrorAxis,  "Mirror by Axis")
  id_MirrorPnt   = geompy.addToStudy(MirrorPnt,   "Mirror by Point")
  id_Position    = geompy.addToStudy(Position,    "Positioned box")
  id_Offset      = geompy.addToStudy(Offset,      "Offset")
  id_Orientation = geompy.addToStudy(Orientation, "Orientation")

  id_Fillet   = geompy.addToStudy(Fillet,   "Fillet")
  id_Fillet2  = geompy.addToStudy(Fillet2,  "Fillet2")

  id_Chamfer  = geompy.addToStudy(Chamfer,  "Chamfer on Edge")
  id_Chamfer2 = geompy.addToStudy(Chamfer2, "Chamfer on Faces")
  id_Chamfer3 = geompy.addToStudy(Chamfer3, "Chamfer on Edges")
  id_Chamfer4 = geompy.addToStudy(Chamfer4, "Chamfer on Faces with params D Angle")

  id_MultiTrans1D = geompy.addToStudy(MultiTrans1D, "MultiTrans1D")
  id_MultiTrans2D = geompy.addToStudy(MultiTrans2D, "MultiTrans2D")
  id_MultiRot1D   = geompy.addToStudy(MultiRot1D,   "MultiRot1D")
  id_MultiRot2D   = geompy.addToStudy(MultiRot2D,   "MultiRot2D")

  id_CDG       = geompy.addToStudy(CDG,       "CDG")
  id_Archimede = geompy.addToStudy(Archimede, "Archimede")

  id_Partition  = geompy.addToStudy(Partition, "Partition")
  id_Partition1 = geompy.addToStudy(Partition1, "Half Partition")

  #Decompose objects
  SubFace    = geompy.SubShape(Box, geompy.ShapeType["FACE"], [2])
  name       = geompy.SubShapeName(SubFace, Box)
  id_SubFace = geompy.addToStudyInFather(Box, SubFace, name)

  SubFaceS   = geompy.SubShapeSorted(Box, geompy.ShapeType["FACE"], [5])
  nameS      = geompy.SubShapeName(SubFaceS, Box)
  id_SubFace = geompy.addToStudyInFather(Box, SubFaceS, nameS)

  SubEdgeList = geompy.SubShapeAll(SubFace, geompy.ShapeType["EDGE"])
  i=0
  for SubEdge in SubEdgeList :
    name = geompy.SubShapeName(SubEdge, SubFace)
    id_SubEdge = geompy.addToStudyInFather(SubFace, SubEdge, name)

  #-------------------

  print "DONE"
