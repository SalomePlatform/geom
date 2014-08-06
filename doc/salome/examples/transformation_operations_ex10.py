# Fillet 2D

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a face in OXY plane
face = geompy.MakeFaceHW(100, 100, 1)
fillet2d = geompy.MakeFillet2D(face, 30, [7, 9])

# add objects in the study
id_face  = geompy.addToStudy(face,"Face_1")
id_fillet2d  = geompy.addToStudy(fillet2d,"Fillet 2D_1")

# display disks
gg.createAndDisplayGO(id_face)
gg.createAndDisplayGO(id_fillet2d)
