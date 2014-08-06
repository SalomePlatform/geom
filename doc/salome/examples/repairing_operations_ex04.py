# Suppress Internal Wires

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(55, 65, 50)
p2 = geompy.MakeVertex(55,  0, 50)
v = geompy.MakeVector(p1, p2)

# create a cylinder
height = 100
radius1 = 40
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# create a box
box = geompy.MakeBoxDXDYDZ(100, 100, 100)

# make a cut
cut = geompy.MakeCut(box, cylinder)

# suppress all internal wires
result = geompy.SuppressInternalWires(cut, [])

# add objects in the study
id_cut = geompy.addToStudy(cut, "Cut")
id_result = geompy.addToStudy(result, "Result")

# display the results
gg.createAndDisplayGO(id_cut)
gg.setDisplayMode(id_cut,1)
gg.createAndDisplayGO(id_result)
gg.setDisplayMode(id_result,1) 
