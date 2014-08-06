# Creation of an Ellipse

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(0., 0., 0.)
p1 = geompy.MakeVertex(50., 50., 50.)
p2 = geompy.MakeVertex(0., 50., 0.)

# create a normal vector from two points
normal  = geompy.MakeVector(p0, p1)

# create a major axis vector from two points
major   = geompy.MakeVector(p0, p2)

# create an ellipse from a point, a vector and radiuses
ellipse1 = geompy.MakeEllipse(p1, normal, 50, 25)

# create an ellipse from a point, a normal vector, radiuses and a major axis vector
ellipse2 = geompy.MakeEllipse(p1, normal, 50, 25, major)

# add objects in the study
id_normal   = geompy.addToStudy(normal,   "Normal")
id_major    = geompy.addToStudy(major,    "Major Axis")
id_ellipse1 = geompy.addToStudy(ellipse1, "Ellipse 1")
id_ellipse2 = geompy.addToStudy(ellipse2, "Ellipse 2")

# display the ellipse and its normal vector
gg.createAndDisplayGO(id_normal)
gg.createAndDisplayGO(id_major)
gg.createAndDisplayGO(id_ellipse1)
gg.createAndDisplayGO(id_ellipse2)
