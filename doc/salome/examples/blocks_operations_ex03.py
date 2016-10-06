# Propagate

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
check_box = geompy.MakeBoxDXDYDZ(200, 200, 200)

# build all possible propagation groups
listChains = geompy.Propagate(check_box)

# add objects in the study
geompy.addToStudy(check_box, "Box")
ii = 1
for chain in listChains:
    geompy.addToStudyInFather(check_box, chain, "propagation chain " + `ii`)
    ii = ii + 1
    pass

salome.sg.updateObjBrowser(True) 
