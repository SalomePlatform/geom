# Creation of a Pipe

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0   = geompy.MakeVertex(0.  , 0.  , 0.  )
px   = geompy.MakeVertex(100., 0.  , 0.  )
py   = geompy.MakeVertex(0.  , 100., 0.  )
pz   = geompy.MakeVertex(0.  , 0.  , 100.)
pxyz = geompy.MakeVertex(100., 100., 100.)

# create a vector from two points
vxy = geompy.MakeVector(px, py)

# create an arc from three points
arc = geompy.MakeArc(py, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# create an edge
edge = geompy.MakeEdge(p0, pxyz)

# create a pipe
pipe = geompy.MakePipe(wire, edge)

# add objects in the study
id_wire = geompy.addToStudy(wire,"Wire")
id_edge = geompy.addToStudy(edge,"Edge")
id_pipe = geompy.addToStudy(pipe,"Pipe")

# display the wire, the edge (path) and the pipe
gg.createAndDisplayGO(id_wire)
gg.createAndDisplayGO(id_edge)
gg.createAndDisplayGO(id_pipe)
gg.setDisplayMode(id_pipe,1) 
