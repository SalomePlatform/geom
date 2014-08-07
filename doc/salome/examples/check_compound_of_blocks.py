# Check Compound of Blocks

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create boxes
box1 = geompy.MakeBox(0,0,0,100,50,100)
box2 = geompy.MakeBox(100,0,0,250,50,100)

# make a compound
compound = geompy.MakeCompound([box1, box2])

# glue the faces of the compound
tolerance = 1e-5
glue = geompy.MakeGlueFaces(compound, tolerance)
IsValid = geompy.CheckCompoundOfBlocks(glue)
if IsValid == 0:
    raise RuntimeError, "Invalid compound created"
else:
    print "\nCompound is valid"
