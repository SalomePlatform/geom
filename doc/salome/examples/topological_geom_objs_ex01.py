# Creation of an Edge

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

#
# create edge by two points
#

# create vertices
p0   = geompy.MakeVertex(0.  , 0.  , 0.  )
pxyz = geompy.MakeVertex(100., 100., 100.)

# create an edge
edge = geompy.MakeEdge(p0, pxyz)

# add object in the study
id_edge = geompy.addToStudy(edge,"Edge_1")

# display an edge
gg.createAndDisplayGO(id_edge) 

#
# create edge from wire
#

# create a circle
c = geompy.MakeCircle(None, None, 100)

# create a wire
w = geompy.MakeWire([c], 1e-07)

# create an edge from wire
edge = geompy.MakeEdgeWire(w)

# add object in the study
id_edge = geompy.addToStudy(edge,"Edge_2")

# display an edge
gg.createAndDisplayGO(id_edge) 

#
# create edge from existing curve and a length
#

# create a circle
c = geompy.MakeCircle(None, None, 100)

# create an edge of length 25.0 from the circle
edge = geompy.MakeEdgeOnCurveByLength(c, 25.0)

# add object in the study
id_edge = geompy.addToStudy(edge,"Edge_3")

# display an edge
gg.createAndDisplayGO(id_edge) 
