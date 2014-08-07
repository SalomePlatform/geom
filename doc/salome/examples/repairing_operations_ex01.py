# Shape Processing

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices, an edge, an arc, a wire, a face and a prism
p1 = geompy.MakeVertex(0,0,0)
p2 = geompy.MakeVertex(200,0,0)
p3 = geompy.MakeVertex(100,150,0)
edge = geompy.MakeEdge(p1,p2)
arc  = geompy.MakeArc(p1,p3,p2)
wire = geompy.MakeWire([edge,arc])
face = geompy.MakeFace(wire, 1)
theShape = geompy.MakePrismVecH(face, edge, 130)

# check the shape at the beginning
print "Before ProcessShape:"
isValid = geompy.CheckShape(theShape)
if isValid == 0:
    print "The shape is not valid"
else:
    print "The shape seems to be valid"

# process the Shape
Operators = ["FixShape"]
Parameters = ["FixShape.Tolerance3d"]
Values = ["1e-7"]
PS = geompy.ProcessShape(theShape, Operators, Parameters, Values)

# check the shape at the end
print "After ProcessShape:"
isValid = geompy.CheckShape(PS)
if isValid == 0:
    print "The shape is not valid"
    raise RuntimeError, "It seems, that the ProcessShape() has failed"
else:
    print "The shape seems to be valid"

# add in the study and display
Id_Shape = geompy.addToStudy(theShape, "Invalid Shape")
Id_PS    = geompy.addToStudy(PS, "Processed Shape")
gg.createAndDisplayGO(Id_Shape)
gg.setDisplayMode(Id_Shape,1)
gg.createAndDisplayGO(Id_PS)
gg.setDisplayMode(Id_PS,1) 
