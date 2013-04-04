# Scale Transform

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a box and a sphere
box = geompy.MakeBoxDXDYDZ(200, 200, 200)

# scale the given object by the factor
p0 = geompy.MakeVertex(100, 100, 100)
factor = 0.5
scale = geompy.MakeScaleTransform(box, p0, factor)

# add objects in the study
id_box = geompy.addToStudy(box, "Box")
id_scale = geompy.addToStudy(scale, "Scale")

# display the results
gg.createAndDisplayGO(id_box)
gg.setDisplayMode(id_box,1)
gg.setTransparency(id_box,0.5)
gg.createAndDisplayGO(id_scale)
gg.setDisplayMode(id_scale,1)
