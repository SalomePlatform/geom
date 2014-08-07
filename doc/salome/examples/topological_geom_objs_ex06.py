# Creation of a Compound

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(  -30.,  -30.,  50.)
p2 = geompy.MakeVertex(  -60.,  -60.,  30.)
p3 = geompy.MakeVertex(  -30.,  -30.,  10.)

# create an arc from three points
arc = geompy.MakeArc(p1, p2, p3)
ShapeListCompound = []
i = 0
while i <= 3 :
    S = geompy.MakeTranslation(arc, i * 50., 0., 0.)
    ShapeListCompound.append(S)
    i = i + 1

# create a compund of the given shapes
compound = geompy.MakeCompound(ShapeListCompound)

# add object in the study
id_compound = geompy.addToStudy(compound,"Compound")

# display the compound
gg.createAndDisplayGO(id_compound) 
