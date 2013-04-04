# Check Free Boundaries

import os
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create boxes
box1 = geompy.MakeBox(0,0,0,100,50,100)
box2 = geompy.MakeBox(100,0,0,250,50,100)

# make a compound
compound = geompy.MakeCompound([box1, box2])

# import from *.brep
ImportFromBREP = geompy.ImportBREP(os.getenv("DATA_DIR")+"/Shapes/Brep/flight_solid.brep")

# get a face
faces = geompy.SubShapeAllSortedCentres(ImportFromBREP, geompy.ShapeType["FACE"])

# get the free boundary for face 32
Res = geompy.GetFreeBoundary(faces[32])
isSuccess   = Res[0]
ClosedWires = Res[1]
OpenWires   = Res[2]

if isSuccess == 1 :
    print "Checking free boudaries is OK."
else :
    print "Checking free boudaries is KO!"
print "len(ClosedWires) = ", len(ClosedWires)

i = 0
for wire in ClosedWires :
    wire_name = "Face 32 -> Close wires : WIRE %d"%(i+1)
    geompy.addToStudy(ClosedWires[i], wire_name)
    if i < len(ClosedWires) :
        i = i+ 1

print "len(OpenWires) = ", len(OpenWires)

i = 0
for wire in OpenWires :
    wire_name = "Face 32 -> Open wires : WIRE %d"%(i+1)
    geompy.addToStudy(OpenWires[i], wire_name)
    if i < len(OpenWires) :
        i = i+ 1

# get the free boundary for face 41
Res = geompy.GetFreeBoundary(faces[41])
isSuccess   = Res[0]
ClosedWires = Res[1]
OpenWires   = Res[2]

if isSuccess == 1 :
    print "Checking free boudaries is OK."
else :
    print "Checking free boudaries is KO!"
print "len(ClosedWires) = ", len(ClosedWires)

i = 0
for wire in ClosedWires :
    wire_name = "Face 41 -> Close wires : WIRE %d"%(i+1)
    geompy.addToStudy(ClosedWires[i], wire_name)
    if i < len(ClosedWires) :
        i = i+ 1

print "len(OpenWires) = ", len(OpenWires)

i = 0
for wire in OpenWires :
    wire_name = "Face 41 -> Open wires : WIRE %d"%(i+1)
    geompy.addToStudy(OpenWires[i], wire_name)
    if i < len(OpenWires) :
        i = i+ 1

# add the imported object to the study
id_ImportFromBREP = geompy.addToStudy(ImportFromBREP, "ImportFromBREP")
salome.sg.updateObjBrowser(1)
