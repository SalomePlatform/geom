# Creation of a Shell

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

#create vertices
p0   = geompy.MakeVertex( 0.,  0.,  0.)
pxyz = geompy.MakeVertex( 5.,  5., 40.)

# create sketchers
sketcher1 = geompy.MakeSketcher("Sketcher:F 0 0:TT 70 0:TT 70 70:TT 0 70:WW")
sketcher2 = geompy.MakeSketcher("Sketcher:F 20 20:TT 50 20:TT 50 50:TT 20 50:WW")
isPlanarFace = 1

# create a face from two wires
face = geompy.MakeFaces([sketcher1, sketcher2],isPlanarFace)

# create a prism
prism = geompy.MakePrism(face, p0, pxyz)

# explode the prism into faces
prism_faces = geompy.SubShapeAllSortedCentres(prism, geompy.ShapeType["FACE"])

# create a shell from a set of faces
shell = geompy.MakeShell([prism_faces[0], prism_faces[2], prism_faces[3],
                          prism_faces[7], prism_faces[9]])

# add objects in the study
id_shell = geompy.addToStudy(shell,"Shell")

# display the shell
gg.createAndDisplayGO(id_shell)
gg.setDisplayMode(id_shell,1) 
