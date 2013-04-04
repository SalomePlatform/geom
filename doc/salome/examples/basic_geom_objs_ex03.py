# Creation of a Circle

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(0., 0., 0.)
px = geompy.MakeVertex(100., 0.  , 0.  )
py = geompy.MakeVertex(0.  , 100., 0.  )
pz = geompy.MakeVertex(0.  , 0.  , 100.)

# create a vector on two points
vxy  = geompy.MakeVector(px, py)

# create a circle from a point, a vector and a radius
circle1 = geompy.MakeCircle(pz, vxy, 30)

#create a circle from three points
circle2 = geompy.MakeCircleThreePnt(p0, px, py)

# add objects in the study
id_vxy      = geompy.addToStudy(vxy,    "Vector")
id_circle1  = geompy.addToStudy(circle1,"Circle1")
id_circle2  = geompy.addToStudy(circle2,"Circle2")

# display circles
gg.createAndDisplayGO(id_vxy)
gg.createAndDisplayGO(id_circle1)
gg.createAndDisplayGO(id_circle2)
