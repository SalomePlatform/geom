# Creation of an Arc

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(0., 0., 0.)
p1 = geompy.MakeVertex(100., 0., 0.)
p2 = geompy.MakeVertex(50., 0., 50.)

# create an arc from a three points
arc1 = geompy.MakeArc(p0, p1, p2)

# create an arc from a center point, a start point and end point
arc2 = geompy.MakeArcCenter(p0, p1, p2, 1)

# create an arc from a center point, a major point and minor point
arc3 = geompy.MakeArcOfEllipse(p0, p1, p2)

# add objects in the study
id_arc1 = geompy.addToStudy(arc1, "Arc 1")
id_arc2 = geompy.addToStudy(arc2, "Arc 2")
id_arc3 = geompy.addToStudy(arc3, "Arc 3")

# display the arcs
gg.createAndDisplayGO(id_arc1)
gg.createAndDisplayGO(id_arc2)
gg.createAndDisplayGO(id_arc3)
