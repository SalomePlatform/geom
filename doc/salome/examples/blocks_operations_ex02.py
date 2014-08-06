# Explode on Blocks

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a box and a sphere
box = geompy.MakeBoxDXDYDZ(200, 200, 200)
sphere = geompy.MakeSphereR(100)

# make a compound
compound = geompy.MakeCompound([box, sphere])

# get all the blocks of the given compound, by criteria: min_nb_faces <= nb. of faces <= max_nb_faces
min_nb_faces = 6
max_nb_faces = 6
make_block_explode = geompy.MakeBlockExplode(compound, min_nb_faces, max_nb_faces)

# add objects in the study
id_compound = geompy.addToStudy(compound, "Compound")
id_make_block_explode = geompy.addToStudyInFather(compound, make_block_explode[0], "MakeBlockExplode")

# display the results
gg.createAndDisplayGO(id_compound)
gg.createAndDisplayGO(id_make_block_explode)
gg.setDisplayMode(id_make_block_explode,1) 
