# Close Contour

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices and vectors
p0  = geompy.MakeVertex(  0.,   0.,   0.)
px  = geompy.MakeVertex(100.,   0.,   0.)
py  = geompy.MakeVertex(  0., 100.,   0.)
py1 = geompy.MakeVertex(  0., 140.,   0.)
pz  = geompy.MakeVertex(  0.,   0., 100.)
vxy = geompy.MakeVector(px, py)

# create an arc
arc = geompy.MakeArc(py1, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# close an open wire by creation of an edge between ends
wire_close = geompy.CloseContour(wire, [1], 0)

# add objects in the study
id_wire = geompy.addToStudy(wire, "Wire")
id_wire_close = geompy.addToStudy(wire_close, "Wire close")

# display the results
gg.createAndDisplayGO(id_wire)
gg.createAndDisplayGO(id_wire_close) 
