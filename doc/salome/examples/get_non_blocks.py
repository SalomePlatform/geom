# Get Non Blocks

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create solids
box = geompy.MakeBoxDXDYDZ(100, 100, 100)
cyl = geompy.MakeCylinderRH(100, 200)

geompy.addToStudy(box, 'box')
geompy.addToStudy(cyl, 'cyl')

# make a compound
compound = geompy.MakeCompound([box, cyl])
geompy.addToStudy(compound, 'compound')

# explore the compound
pair = geompy.GetNonBlocks(compound)

if pair[0] is not None:
  geompy.addToStudyInFather(compound, pair[0], "GrNonBlocks")
  pass

if pair[1] is not None:
  geompy.addToStudyInFather(compound, pair[1], "GrNonQuads")
  pass
