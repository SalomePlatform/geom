#==============================================================================
#  File      : GEOM_usinggeom.py
#  Created   : mon apr 22 11:55:00 CET 2002
#  Author    : Damien COQUERET, Open CASCADE
#  Project   : SALOME
#  Copyright : Open CASCADE, 2002
#  $Header$
#==============================================================================

import salome
import geompy
import math

geom = salome.lcc.FindOrLoadComponent("FactoryServer", "Geometry")
myBuilder = salome.myStudy.NewBuilder()

from geompy import gg

#Create base Variables
nbtimes1  = 5      #Short
nbtimes2  = 5
mindeg = 2
maxdeg = 5
nbiter = 5
ShapeTypeFace = 4
ShapeTypeEdge = 6
WantPlanarFace = 1 #True

radius    = 10.     #Double
radius1   = 100.
radius2   = 200.
height    = 200.
d1        = 10.
d2        = 10.
step1     = 250.
step2     = 250.
angle     = 45.
angle1    = angle * math.pi / 180
angle2    = 2 * angle1
factor    = 2.
tol3d = 0.0001
tol2d = 0.0001
weight = 1000000.
waterdensity = 1.
meshingdeflection = 0.01
trimsize  = 1000.
precision = 0.00001

#Create base points
p0 = geom.MakePointStruct(0., 0., 0.) #(Double, Double, Double)->PointStruct
px = geom.MakePointStruct(100., 0., 0.)
py = geom.MakePointStruct(0., 100., 0.)
pz = geom.MakePointStruct(0., 0., 100.)
pxyz = geom.MakePointStruct(100., 100., 100.)

#Create base directions
vx = geom.MakeDirection(px)                      #(PointStruct)->DirStruct
vy = geom.MakeDirection(py)
vz = geom.MakeDirection(pz)
vxyz = geom.MakeDirection(pxyz)
ax = geom.MakeAxisStruct(0., 0., 0., 100., 0., 0.) #(Double, Double, Double, Double, Double, Double)->AxisStruct
axy = geom.MakeAxisStruct(100., 0., 0., -100., 100., 0.)
ay = geom.MakeAxisStruct(0., 0., 0., 0., 100., 0.)
az = geom.MakeAxisStruct(0., 0., 0., 0., 0., 100.)

#Create base geometry 2D
Vertex = geom.MakeVertex(100., 50., 200.) #(Double, Double, Double)->GEOM_Shape_ptr
Vector = geom.MakeVector(px, py)          #(PointStruct, PointStruct)->GEOM_Shape_ptr
Line   = geom.MakeLine(p0, vxyz)            #(PointStruct, DirStruct)->GEOM_Shape_ptr
Arc    = geom.MakeArc(py, pz, px)         #(PointStruct, PointStruct, PointStruct)->GEOM_Shape_ptr
Circle = geom.MakeCircle(p0, vz, radius1) #(PointStruct, DirStruct, Double)->GEOM_Shape_ptr
Plane  = geom.MakePlane(pz, vxyz, trimsize) #(PointStruct, DirStruct, Double)->GEOM_Shape_ptr

#Create base geometry 3D
Box      = geompy.MakeBox(0., 0., 0., 200., 200., 200.)      #(Double, Double, Double)->GEOM_Shape_ptr
Cylinder = geompy.MakeCylinder(p0, vz, radius1, height)      #(Double, Double, Double)->GEOM_Shape_ptr
Sphere   = geompy.MakeSphere(0., 0., 0., radius1)            #(Double, Double, Double)->GEOM_Shape_ptr
Cone     = geompy.MakeCone(p0, vz, radius2, radius, height) #(PointStruct)->DirStruct
Torus    = geompy.MakeTorus(p0, vz, radius2, radius)         #(Double, Double, Double)->GEOM_Shape_ptr

#Boolean (Common, Cut, Fuse, Section)
Common  = geompy.MakeBoolean(Box, Sphere, 1) #(GEOM_Shape_ptr, GEOM_Shape_ptr, Short)->GEOM_Shape_ptr
Cut     = geompy.MakeBoolean(Box, Sphere, 2)
Fuse    = geompy.MakeBoolean(Box, Sphere, 3)
Section = geompy.MakeBoolean(Box, Sphere, 4)

#IORList for Wire
ShapeListWire = []
IORListWire   = []
ShapeListWire.append(Vector)
ShapeListWire.append(Arc)
for Shape in ShapeListWire :
    IORListWire.append(Shape._get_Name())

#IORList for Compound
ShapeListCompound = []
IORListCompound   = []
i = 0
while i <= 3 :
      S = geompy.MakeTranslation(Arc, i * 100., i * 100., i * 100.)
      ShapeListCompound.append(S)
      i = i + 1
for Shape in ShapeListCompound :
    IORListCompound.append(Shape._get_Name())

#Create base objects
Edge     = geompy.MakeEdge(p0, pxyz)             #(PointStruct, PointStruct)->GEOM_Shape_ptr
Wire     = geompy.MakeWire(IORListWire)          #(ListOfIOR)->GEOM_Shape_ptr
Face     = geompy.MakeFace(Wire, WantPlanarFace) #(GEOM_Shape_ptr, Boolean)->GEOM_Shape_ptr
Compound = geompy.MakeCompound(IORListCompound)  #(ListOfIOR)->GEOM_Shape_ptr

#IORList for Sewing
ShapeListSewing = []
IORListSewing   = []
ShapeListSewing.append(Face)
S = geompy.MakeRotation(Face, axy, angle1)
ShapeListSewing.append(S)
for Shape in ShapeListSewing :
    IORListSewing.append(Shape._get_Name())

#Create advanced objects
Copy       = geompy.MakeCopy(Box)                                               #(GEOM_Shape_ptr)->GEOM_Shape_ptr
Prism      = geompy.MakePrism(Face, p0, pz)                                     #(GEOM_Shape_ptr, PointStruct, PointStruct)->GEOM_Shape_ptr
Revolution = geompy.MakeRevolution(Face, az, angle2)                            #(GEOM_Shape_ptr, AxisStruct, Double)->GEOM_Shape_ptr
Filling    = geompy.MakeFilling(Compound, mindeg, maxdeg, tol3d, tol2d, nbiter) #(GEOM_Shape_ptr, Short, Short, Double, Double, Short)->GEOM_Shape_ptr
Pipe       = geompy.MakePipe(Edge, Wire)                                        #(GEOM_Shape_ptr, GEOM_Shape_ptr)->GEOM_Shape_ptr
Sewing     = geompy.MakeSewing(IORListSewing, precision)                        #(ListOfIOR, Double)->GEOM_Shape_ptr

#IDList for Fillet/Chamfer
IDlist = []
i = 1
while i <= 3 :
      IDlist.append(i)
      i = i + 1

#Transform objects
Translation = geompy.MakeTranslation(Box, 300., 300., 300.)            #(GEOM_Shape_ptr, Double, Double, Double)->GEOM_Shape_ptr
Rotation    = geompy.MakeRotation(Box, az, angle1)                     #(GEOM_Shape_ptr, AxisStruct, Double)->GEOM_Shape_ptr
Scale       = geompy.MakeScaleTransform(Box, p0, factor)               #(GEOM_Shape_ptr, PointStruct, Double)->GEOM_Shape_ptr
Mirror      = geompy.MakeMirrorByPlane(Box, Plane)                     #(GEOM_Shape_ptr, GEOM_Shape_ptr)->GEOM_Shape_ptr
Orientation = geompy.OrientationChange(Box)                            #(GEOM_Shape_ptr)->GEOM_Shape_ptr
Fillet      = geompy.MakeFillet (Prism, radius, ShapeTypeEdge, IDlist) #(GEOM_Shape_ptr, Double, Short, ListOfSubShapeID)->GEOM_Shape_ptr
Chamfer     = geompy.MakeChamfer(Prism, d1, d2, ShapeTypeEdge, IDlist) #(GEOM_Shape_ptr, Double, Double, Short, ListOfSubShapeID)->GEOM_Shape_ptr

#Create Patterns
MultiTrans1D = geompy.MakeMultiTranslation1D(Fillet, vz, step1, nbtimes1)                      #(GEOM_Shape_ptr, DirStruct, Double, Short)->GEOM_Shape_ptr
MultiTrans2D = geompy.MakeMultiTranslation2D(Fillet, vz, step1, nbtimes1, vy, step2, nbtimes2) #(GEOM_Shape_ptr, DirStruct, Double, Short, DirStruct, Double, Short)->GEOM_Shape_ptr
#!!!!Angle In Degree!!!!
MultiRot1D   = geompy.MakeMultiRotation1D(Chamfer, vx, px, nbtimes1)                           #(GEOM_Shape_ptr, DirStruct, PointStruct, Short)->GEOM_Shape_ptr
MultiRot2D   = geompy.MakeMultiRotation2D(Chamfer, vx, px, angle, nbtimes1, step1, nbtimes2)   #(GEOM_Shape_ptr, DirStruct, PointStruct, Double, Short, Double, Short)->GEOM_Shape_ptr

#IDList for Partition
ShapeListPartition1 = []
ShapeListPartition1.append(Box)
IORListPartition1   = []
for Shape in ShapeListPartition1 :
    IORListPartition1.append(Shape._get_Name())
ShapeListPartition2 = []
ShapeListPartition2.append(Plane)
IORListPartition2   = []
for Shape in ShapeListPartition2 :
    IORListPartition2.append(Shape._get_Name())

#IDList for SubShape
IDlistSubShape = []
IDlistSubShape.append(3)

#Create Informations objects
CDG        = geompy.MakeCDG(Prism)                                          #(GEOM_Shape_ptr)->GEOM_Shape_ptr
Archimede  = geompy.Archimede(Box, weight, waterdensity, meshingdeflection) #(GEOM_Shape_ptr, Double, Double, Double)->GEOM_Shape_ptr
CheckShape = geompy.CheckShape(Prism)                                       #(GEOM_Shape_ptr)->Boolean

#Partition objects
Partition       = geompy.Partition(IORListPartition1, IORListPartition2) #(ListOfIOR, ListOfIOR)->GEOM_Shape_ptr

#Add In Study
id_Vertex       = geompy.addToStudy(Vertex,       "Vertex")
id_Vector       = geompy.addToStudy(Vector,       "Vector")
id_Line         = geompy.addToStudy(Line,         "Line")
id_Arc          = geompy.addToStudy(Arc,          "Arc")
id_Circle       = geompy.addToStudy(Circle,       "Circle")
id_Plane        = geompy.addToStudy(Plane,        "Plane")

id_Box          = geompy.addToStudy(Box,          "Box")
id_Cylinder     = geompy.addToStudy(Cylinder,     "Cylinder")
id_Sphere       = geompy.addToStudy(Sphere,       "Sphere")
id_Cone         = geompy.addToStudy(Cone,         "Cone")
id_Torus        = geompy.addToStudy(Torus,        "Torus")

id_Edge         = geompy.addToStudy(Edge,         "Edge")
id_Wire         = geompy.addToStudy(Wire,         "Wire")
id_Face         = geompy.addToStudy(Face,         "Face")
id_Compound     = geompy.addToStudy(Compound,     "Compound")

id_Common       = geompy.addToStudy(Common,       "Common")
id_Cut          = geompy.addToStudy(Cut,          "Cut")
id_Fuse         = geompy.addToStudy(Fuse,         "Fuse")
id_Section      = geompy.addToStudy(Section,      "Section")

id_Copy         = geompy.addToStudy(Copy,         "Copy")
id_Prism        = geompy.addToStudy(Prism,        "Prism")
id_Revolution   = geompy.addToStudy(Revolution,   "Revolution")
id_Filling      = geompy.addToStudy(Filling,      "Filling")
id_Pipe         = geompy.addToStudy(Pipe,         "Pipe")
id_Sewing       = geompy.addToStudy(Sewing,       "Sewing")

Id_Translation  = geompy.addToStudy(Translation,  "Translation")
Id_Rotation     = geompy.addToStudy(Rotation,     "Rotation")
Id_Scale        = geompy.addToStudy(Scale,        "Scale")
Id_Mirror       = geompy.addToStudy(Mirror,       "Mirror")
Id_Orientation  = geompy.addToStudy(Orientation,  "Orientation")
Id_Fillet       = geompy.addToStudy(Fillet,       "Fillet")
Id_Chamfer      = geompy.addToStudy(Chamfer,      "Chamfer")

Id_MultiTrans1D = geompy.addToStudy(MultiTrans1D, "MultiTrans1D")
Id_MultiTrans2D = geompy.addToStudy(MultiTrans2D, "MultiTrans2D")
Id_MultiRot1D   = geompy.addToStudy(MultiRot1D,   "MultiRot1D")
Id_MultiRot2D   = geompy.addToStudy(MultiRot2D,   "MultiRot2D")

Id_Partition    = geompy.addToStudy(Partition,  "Partition")
Id_CDG          = geompy.addToStudy(CDG,          "CDG")
Id_Archimede    = geompy.addToStudy(Archimede,    "Archimede")

#Decompose objects
SubShape        = geompy.SubShape(Box, ShapeTypeFace, IDlistSubShape)    #(GEOM_Shape_ptr, Short, ListOfSubShapeID)->GEOM_Shape_ptr
name            = geompy.SubShapeName( SubShape._get_Name(), Box._get_Name() )
Id_SubShape     = geompy.addToStudyInFather(Box, SubShape, name)

SubShapeAllList = []
SubShapeAllList = geompy.SubShapeAll(SubShape, ShapeTypeEdge)            #(GEOM_Shape_ptr, Short)->ListOfGeomShapes
for Shape in SubShapeAllList :
    name = geompy.SubShapeName( Shape._get_Name(), Box._get_Name() )
    Id_SubShapeAll  = geompy.addToStudyInFather(SubShape, Shape, name)
