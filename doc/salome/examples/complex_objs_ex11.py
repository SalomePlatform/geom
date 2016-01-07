# Apply thickness for shell

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create box
box = geompy.MakeBoxDXDYDZ(100, 100, 100)

# get the list of faces
faces = geompy.SubShapeAllSortedCentres(box, geompy.ShapeType["FACE"])

# get the list of face IDs
faceIDs = geompy.SubShapeAllSortedCentresIDs(box, geompy.ShapeType["FACE"])

# make a shell from 3 faces
shell = geompy.MakeShell([faces[0], faces[1], faces[2]])

# apply thickness
solid = geompy.MakeThickSolid(shell, 30.)

# create box
hsolid = geompy.MakeBoxDXDYDZ(100, 100, 100)

# make hollowed solid
geompy.Thicken(hsolid, 30., [faceIDs[0], faceIDs[1]])

# add objects in the study
id_shell  = geompy.addToStudy(shell, "Shell")
id_solid  = geompy.addToStudy(solid, "Solid")
id_hsolid = geompy.addToStudy(hsolid, "Hollowed Solid")

# display the shell and the result thicknen solid and hollowed solid
gg.createAndDisplayGO(id_shell)
gg.createAndDisplayGO(id_solid)
gg.createAndDisplayGO(id_hsolid)
