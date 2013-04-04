# Fillet 1D

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create box
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
# take box edges to create custom complex wire
[Edge_1,Edge_2,Edge_3,Edge_4,Edge_5,Edge_6,Edge_7,Edge_8,Edge_9,Edge_10,Edge_11,Edge_12] = geompy.SubShapeAllSortedCentres(Box_1, geompy.ShapeType["EDGE"])
# create wire
Wire_1 = geompy.MakeWire([Edge_12, Edge_7, Edge_11, Edge_6, Edge_1,Edge_4])
# make fillet at given wire vertices with giver radius
Fillet_1D_1 = geompy.MakeFillet1D(Wire_1, 55, [3, 4, 6, 8, 10])

id_Wire_1 = geompy.addToStudy(Wire_1, "Wire_1")
id_Fillet_1D_1= geompy.addToStudy(Fillet_1D_1, "Fillet_1D_1")

# display disks
gg.createAndDisplayGO(id_Wire_1)
gg.createAndDisplayGO(id_Fillet_1D_1)
