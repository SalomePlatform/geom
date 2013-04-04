# Creation of a Line

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0 = geompy.MakeVertex(0., 0., 0.)
p100 = geompy.MakeVertexWithRef(p0, 100., 100., 100.)
px = geompy.MakeVertex(100., 0.  , 0.  )
py = geompy.MakeVertex(0.  , 100., 0.  )
pz = geompy.MakeVertex(0.  , 0.  , 100.)

# create a vector from two points
vxy  = geompy.MakeVector(px, py)

# create a line from a point and a vector
line1 = geompy.MakeLine(pz, vxy)

#create a line from two points
line2 = geompy.MakeLineTwoPnt(p0, p100)

# add objects in the study
id_vxy      = geompy.addToStudy(vxy,  "Vector")
id_line1    = geompy.addToStudy(line1,"Line1")
id_line2    = geompy.addToStudy(line2,"Line2")

# display lines
gg.createAndDisplayGO(id_vxy)
gg.createAndDisplayGO(id_line1)
gg.createAndDisplayGO(id_line2) 
