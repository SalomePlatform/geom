# Creation of a Solid(s) from connected faces

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a box 
box = geompy.MakeBoxDXDYDZ(200, 200, 200)

# make a copy of a box translated by X coordinate
box_translation = geompy.MakeTranslation(box, 200, 0, 0)

# extract shells from boxes
box_shell = geompy.SubShapeAllSorted(box, geompy.ShapeType["SHELL"])[0]
box_translation_shell = geompy.SubShapeAllSorted(box_translation, geompy.ShapeType["SHELL"])[0]

# extract faces from boxes 
box_faces = geompy.SubShapeAllSorted(box, geompy.ShapeType["FACE"])
box_translation_faces = geompy.SubShapeAllSorted(box_translation, geompy.ShapeType["FACE"])

# create solids from shells
msf_shells_noint = geompy.MakeSolidFromConnectedFaces([box_shell, box_translation_shell],0)
msf_shells_int = geompy.MakeSolidFromConnectedFaces([box_shell, box_translation_shell], 1)

# create solids from faces
msf_faces_noint = geompy.MakeSolidFromConnectedFaces(box_faces+box_translation_faces, 0)
msf_faces_int = geompy.MakeSolidFromConnectedFaces(box_faces+box_translation_faces, 1)

# add objects in the study
id_solid_shells_noint = geompy.addToStudy(msf_shells_noint,"Solid_from_shells_no_intersect")
id_solid_shells_int = geompy.addToStudy(msf_shells_int,"Solid_from_shells_intersect")
id_solid_faces_noint = geompy.addToStudy(msf_faces_noint,"Solid_from_faces_no_intersect")
id_solid_faces_int = geompy.addToStudy(msf_faces_int,"Solid_from_faces_intersect")

# display the results
gg.createAndDisplayGO(id_solid_shells_noint)
gg.setDisplayMode(id_solid_shells_noint,1) 
gg.createAndDisplayGO(id_solid_shells_int)
gg.setDisplayMode(id_solid_shells_int,1) 
gg.createAndDisplayGO(id_solid_faces_noint)
gg.setDisplayMode(id_solid_faces_noint,1) 
gg.createAndDisplayGO(id_solid_faces_int)
gg.setDisplayMode(id_solid_faces_int,1) 
