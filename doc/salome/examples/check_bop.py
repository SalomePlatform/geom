# ExtractBOPFailure example

import math
from salome.kernel import salome
salome.salome_init()
from salome.kernel import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

### Prepare shapes for Partition
OX = geompy.MakeVectorDXDYDZ(1, 0, 0, theName="OX")
OY = geompy.MakeVectorDXDYDZ(0, 1, 0, theName="OY")
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1, theName="OZ")

Vertex_1 = geompy.MakeVertex(0, 0, 0, theName="Vertex_1")
Vertex_2 = geompy.MakeVertex(10, 0, 0, theName="Vertex_2")
Vertex_3 = geompy.MakeVertex(0, 0.005, 0, theName="Vertex_3")
Vertex_4 = geompy.MakeVertex(10, 0.015, 0, theName="Vertex_4")
Vertex_5 = geompy.MakeVertex(0, 0.02, 0, theName="Vertex_5")
Vertex_6 = geompy.MakeVertex(10, 0.02, 0, theName="Vertex_6")

Line_1 = geompy.MakeLineTwoPnt(Vertex_1, Vertex_2, theName="Line_1")
Line_2 = geompy.MakeLineTwoPnt(Vertex_3, Vertex_4, theName="Line_2")
Line_3 = geompy.MakeLineTwoPnt(Vertex_5, Vertex_6, theName="Line_3")

Extrusion_1 = geompy.MakePrismVecH(Line_1, OY,  1, theName="Extrusion_1")
Extrusion_2 = geompy.MakePrismVecH(Line_2, OZ,  1, theName="Extrusion_2")
Extrusion_3 = geompy.MakePrismVecH(Line_3, OY, -1, theName="Extrusion_3")

Limit_tolerance_1 = geompy.LimitTolerance(Extrusion_2, 0.01, theName="Limit_tolerance_1")

Extrusion_4 = geompy.MakePrismVecH(Extrusion_1, OZ, -1, theName="Extrusion_4")
Extrusion_5 = geompy.MakePrismVecH(Extrusion_3, OZ, -1, theName="Extrusion_5")

Revolution_1 = geompy.MakeRevolution(Limit_tolerance_1, OX, 45*math.pi/180.0, theName="Revolution_1")

### Try Partition
try :
  Partition_1 = geompy.MakePartition([Extrusion_4, Extrusion_5, Revolution_1], [], [], [], geompy.ShapeType["SOLID"], 0, [], 0, theName="Partition_1")
except :
  print("EXCEPTION: Partition problem")

### Extract failures
(IsValid, ShapeRes, ShapeErrors) = geompy.ExtractBOPFailure([Extrusion_4, Extrusion_5, Revolution_1], theTimer=True, theVerbose=True, theName="InvalidBOPResult")

### Publish incriminated sub-shapes (reflect errors in their names)
for shapeError in ShapeErrors:
  list_ids = shapeError.incriminated
  list_subs = geompy.SubShapes(ShapeRes, list_ids)
  ii = 1
  for subshape in list_subs:
    geompy.addToStudyInFather( ShapeRes, subshape, 'SubShape_with_%s_%d'%(str(shapeError.error),ii) )
    ii = ii+1

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
