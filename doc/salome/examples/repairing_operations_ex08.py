# Glue Edges

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create boxes
box1 = geompy.MakeBox(0,0,0,100,50,100)
box2 = geompy.MakeBox(100,0,0,250,50,100)

# make compound
compound = geompy.MakeCompound([box1, box2])

# glue all compound's edges
tolerance = 1e-5
glue1 = geompy.MakeGlueEdges(compound, tolerance)

# glue some compound's edges
list_edges = geompy.GetGlueEdges(compound, tolerance)
glue2 = geompy.MakeGlueEdgesByList(compound, tolerance, [list_edges[0], list_edges[2]])

# add objects in study
geompy.addToStudy(box1, "Box1")
geompy.addToStudy(box2, "Box2")
geompy.addToStudy(compound, "Compound")
geompy.addToStudy(glue1, "Glue all edges")
geompy.addToStudy(glue2, "Glue two edges")

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
