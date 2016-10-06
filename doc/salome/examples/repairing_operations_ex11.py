# Fuse Collinear Edges within a Wire

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create vertices
p1 = geompy.MakeVertex(0, 0, 0)
p2 = geompy.MakeVertex(70, 0, 0)
p3 = geompy.MakeVertex(70, 50, 0)
p4 = geompy.MakeVertex(70, 80, 0)
p5 = geompy.MakeVertex(50, 80, 0)
p6 = geompy.MakeVertex(20, 80, 0)
p7 = geompy.MakeVertex(0, 80, 0)
p8 = geompy.MakeVertex(0, 30, 0)

points = [p1, p2, p3, p4, p5, p6, p7, p8]

# make a wire
wire_1 = geompy.MakePolyline(points, True)

# suppress some vertices in the wire
wire_2 = geompy.FuseCollinearEdgesWithinWire(wire_1, [p3])
wire_3 = geompy.FuseCollinearEdgesWithinWire(wire_1, [p5, p6])

# suppress all suitable vertices in the wire
wire_4 = geompy.FuseCollinearEdgesWithinWire(wire_1, [])

wires = [wire_1, wire_2, wire_3, wire_4]

# add objects in the study
ii = 1
for point in points:
    geompy.addToStudy(point, "p%d"%ii)
    ii = ii + 1
    pass

ii = 1
for wire in wires:
    geompy.addToStudy(wire, "wire_%d"%ii)
    wire_points = geompy.SubShapeAllSortedCentres(wire, geompy.ShapeType["VERTEX"])
    jj = 1
    for point in wire_points:
        geompy.addToStudyInFather(wire, point, "point_%d"%jj)
        jj = jj + 1
        pass
    ii = ii + 1
    pass

salome.sg.updateObjBrowser(True) 
