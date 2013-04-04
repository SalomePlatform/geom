# Creation of a Solid

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

#create vertices
p0 = geompy.MakeVertex( 0.,  0.,  0.)
pz = geompy.MakeVertex( 0.,  0., 40.)

# create sketchers
sketcher = geompy.MakeSketcher("Sketcher:F -50 -50:TT 100 -50:R 0:C 50 70:R 0:L 100:WW")

# create faces from two wires
face = geompy.MakeFace(sketcher,1)

# create a prism
prism = geompy.MakePrism(face, p0, pz)

# explode the prism into faces
prism_faces = geompy.SubShapeAllSortedCentres(prism, geompy.ShapeType["FACE"])

# create a shell from a set of faces
shell = geompy.MakeShell([prism_faces[0], prism_faces[1],
                          prism_faces[3], prism_faces[4],
                          prism_faces[5], prism_faces[2]])

# create a solid, bounded by the given shells
solid = geompy.MakeSolid([shell])

# add objects in the study
id_solid = geompy.addToStudy(solid,"Solid")

# display the solid
gg.createAndDisplayGO(id_solid)
gg.setDisplayMode(id_solid,1) 
