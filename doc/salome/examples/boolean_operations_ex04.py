# Section

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(25, 55, 0)
p2 = geompy.MakeVertex( 0,  0, 0)
v = geompy.MakeVector(p1, p2)

# create a cylinder
height = 35
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# create a sphere
sphere = geompy.MakeSphereR(40)

# make a section
section = geompy.MakeSection(cylinder, sphere)

# add objects in the study
id_section = geompy.addToStudy(section, "Section")

# display the results
gg.createAndDisplayGO(id_section)
gg.setDisplayMode(id_section,1) 
