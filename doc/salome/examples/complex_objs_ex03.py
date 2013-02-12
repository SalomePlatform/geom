# Creation of a Filling

import geompy
import salome
gg = salome.ImportComponentGUI("GEOM")

mindeg = 2
maxdeg = 5
tol3d  = 0.0001
tol2d  = 0.0001
nbiter = 5

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

compound = geompy.MakeCompound(ShapeListCompound)

# create a filling
filling = geompy.MakeFilling(compound, mindeg, maxdeg, tol3d, tol2d, nbiter)

# add objects in the study
id_compound = geompy.addToStudy(compound,"Compound")
id_filling = geompy.addToStudy(filling,"Filling")

# display the compound and the filling
gg.createAndDisplayGO(id_compound)
gg.createAndDisplayGO(id_filling)
gg.setDisplayMode(id_filling,1) 
