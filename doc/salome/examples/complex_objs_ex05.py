# Creation of a PipeWithDifferentSections

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

Wire_1 = geompy.MakeSketcher("Sketcher:F 0 0:TT 100 0:R 0:C 100 90:T 0 200", [0, 0, 0, 0, 0, 1, 1, 0, -0])
edges = geompy.SubShapeAll(Wire_1, geompy.ShapeType["EDGE"])
vertices = geompy.SubShapeAll(Wire_1, geompy.ShapeType["VERTEX"])

# create sections
circles=[]
circles.append(geompy.MakeCircle(vertices[0], edges[0], 20))
circles.append(geompy.MakeCircle(vertices[1], edges[0], 40))
circles.append(geompy.MakeCircle(vertices[2], edges[2], 30))
circles.append(geompy.MakeCircle(vertices[3], edges[2], 20))

# create pipe
Pipe = geompy.MakePipeWithDifferentSections(circles, vertices, Wire_1, 0, 0)

# add objects in the study
geompy.addToStudy(circles[0], "circles1")
geompy.addToStudy(circles[1], "circles2")
geompy.addToStudy(circles[2], "circles3")
geompy.addToStudy(circles[3], "circles4")
id_wire = geompy.addToStudy(Wire_1, "Path")
id_pipe = geompy.addToStudy(Pipe, "Pipe")

# display the wire(path) and the pipe
gg.createAndDisplayGO(id_wire)
gg.createAndDisplayGO(id_pipe)
gg.setDisplayMode(id_pipe,1) 
