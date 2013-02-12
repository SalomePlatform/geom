# Propagate

import geompy
import salome

# create a box
box = geompy.MakeBoxDXDYDZ(200, 200, 200)

# build all possible propagation groups
listChains = geompy.Propagate(check_box)

# add objects in the study
geompy.addToStudy(check_box, "Box")
ii = 1
for chain in listChains:
    geompy.addToStudyInFather(check_box, chain, "propagation chain " + `ii`)
    ii = ii + 1
    pass

salome.sg.updateObjBrowser(1) 
